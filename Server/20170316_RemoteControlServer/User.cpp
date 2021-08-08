
//	�汾 REV 0.2

#include "stdafx.h"
#include "User.h"

//	���嵱ǰ��fpsֵ
double current_fps = 0.0;
CMyCriticalSection current_fps_cs;

//	����TCP��������Դ����
CTCPServerResources myTCPServerRes;

//	����TCP�������������
CMonitorProtocol::CintegratedTCPServerManage myTCPManage;

//	����ͻ��˽����б�
std::vector<CMonitorProtocol::CintegratedTCPServerManage::SReConInfo> myClientListVec;

//	����ͻ��˽����б��CS
CMyCriticalSection myclientlistvec_cs;

//	���嵱ǰѡ�е����Ӷ�����Ϣ
CMonitorProtocol::CintegratedTCPServerManage::SReConInfo CurrentSelectConnInfo;

//	�����û�������ָ��
CMy20170316_RemoteControlServerDlg* pUserMainWindow;

//	��ǰ�������Ĺ���״̬��trueΪ�Ѿ������ˣ�falseΪû�й���
bool TCPServerStatus = false;

//	�����û����������б��CS
CMyCriticalSection UserInterface_ClientList_CS;

//	�����û����浱ǰ�����ı����CS
CMyCriticalSection UserInterface_CurrentClientEdit_CS;

//	��ǰ�Ѿ�ѡ��Ŀͻ�����Ϣ
CMonitorProtocol::CintegratedTCPServerManage::SReConInfo CurrentClientInfo;

//	���״̬��true��ʾ����Ѿ���ʼ��false��ʾ���û�п�ʼ
bool MonitorStatus = false;


//	������ӿͻ��˲���,����true��ʾ��ӳɹ�������false��ʾʧ��
bool AddClient(CMonitorProtocol::CintegratedTCPServerManage::SReConInfo in_info)
{
	//	�����ٽ�
	myclientlistvec_cs.Enter();

	//	���뵽��ϢVEC��
	myClientListVec.insert(myClientListVec.end(), in_info);

	//	����Ϣ���뵽�û��Ի�����б�ؼ���
	//	��ȡIP�ַ���
	std::wstring ip_str = CWCharToChar::Char_To_WChar(in_info.ip_str);

	//	��ȡ�˿��ַ���
	CString port_str;
	port_str.Format(_T("%d"), in_info.port);

	//	���SOCKET�ַ���
	CString socket_str;
	socket_str.Format(_T("%d"), in_info.socket);

	//	���ʱ���ַ���
	std::wstring begin_time_str = CWCharToChar::Char_To_WChar(in_info.begin_time.GetTimeString());

	//	��ӵ��û�������
	UserInterface_ClientList_CS.Enter();
	pUserMainWindow->mClientList.InsertItem(0, ip_str.data());
	pUserMainWindow->mClientList.SetItemText(0, 1, port_str);
	pUserMainWindow->mClientList.SetItemText(0, 2, socket_str);
	pUserMainWindow->mClientList.SetItemText(0, 3, begin_time_str.data());
	UserInterface_ClientList_CS.Leave();

	//	�˳��ٽ�
	myclientlistvec_cs.Leave();

	//	�����ɹ�
	return true;
}

//	�����Ƴ��ͻ��˲������ɹ�����true��ʧ�ܷ���false
bool RemoveClient(SOCKET in_socket)
{
	//	�����ٽ�
	myclientlistvec_cs.Enter();

	//	����Ѱ��VEC�е�ƥ�����
	for (int i = 0; (myClientListVec.begin() + i) != (myClientListVec.end()); ++i)
	{
		//	�ҵ���ִ��ɾ��
		if ((*(myClientListVec.begin() + i)).socket == in_socket)
		{
			myClientListVec.erase(myClientListVec.begin() + i);
			--i;
		}
	}

	//	�����û�������б�ؼ���ɾ��ƥ�����Ŀ
	UserInterface_ClientList_CS.Enter();
	CString in_socket_str;
	in_socket_str.Format(_T("%d"), in_socket);
	for (int i = 0; i < pUserMainWindow->mClientList.GetItemCount(); ++i)
	{
		CString socket_str = pUserMainWindow->mClientList.GetItemText(i, 2);
		if (in_socket_str == socket_str)
		{
			pUserMainWindow->mClientList.DeleteItem(i);
			--i;
		}
	}
	UserInterface_ClientList_CS.Leave();

	//	�˳��ٽ�
	myclientlistvec_cs.Leave();

	//	�����ɹ�
	return true;
}

//	�Ƴ����пͻ���
void RemoveAllClient(void)
{
	//	�����ٽ�
	myclientlistvec_cs.Enter();

	//	ɾ��ȫ��VEC
	myClientListVec.clear();

	//	ɾ��ȫ�������б�
	UserInterface_ClientList_CS.Enter();
	while(pUserMainWindow->mClientList.GetItemCount() != 0)
		pUserMainWindow->mClientList.DeleteItem(0);
	UserInterface_ClientList_CS.Leave();

	//	�˳��ٽ�
	myclientlistvec_cs.Leave();
}

//	��õ�ǰѡ�еĿͻ�����Ϣ
CMonitorProtocol::CintegratedTCPServerManage::SReConInfo GetCurrentSelectInfo(void)
{
	//	������ʱ���ض���
	CMonitorProtocol::CintegratedTCPServerManage::SReConInfo tmp_info = { 0 };
	
	//	�жϵ�ǰ�Ƿ�Ϊѡ��״̬
	UserInterface_ClientList_CS.Enter();
	int re = pUserMainWindow->mClientList.GetSelectionMark();
	UserInterface_ClientList_CS.Leave();
	if (re == -1)
	{
		//	���ؽ��
		return tmp_info;
	}

	//	�����ٽ�
	myclientlistvec_cs.Enter();

	//	��õ�ǰѡ�е�socketֵ
	UserInterface_ClientList_CS.Enter();
	CString socket_str = pUserMainWindow->mClientList.GetItemText(pUserMainWindow->mClientList.GetSelectionMark(), 2);
	UserInterface_ClientList_CS.Leave();
	int socket_value = 0;
	swscanf_s(socket_str, _T("%d"), &socket_value);

	//	����VEC
	for (int i = 0; (myClientListVec.begin() + i) != (myClientListVec.end()); ++i)
	{
		//	�ҵ��󷵻����ݶ���
		if ((*(myClientListVec.begin() + i)).socket == socket_value)
		{
			tmp_info = (*(myClientListVec.begin() + i));
			break;
		}
	}

	//	�˳��ٽ�
	myclientlistvec_cs.Leave();

	//	���ؽ��
	return tmp_info;
}

//	��õ�ǰѡ��Ŀͻ�����Ϣ�������ѡ��ģ��򷵻�true�����򷵻�false
bool GetCurrentSelectClientInfo(CMonitorProtocol::CintegratedTCPServerManage::SReConInfo* out_info)
{
	//	����������
	if ((out_info == NULL) || (out_info == (CMonitorProtocol::CintegratedTCPServerManage::SReConInfo*)(-1)))
	{
		return false;
	}

	//	��鵱ǰ�Ƿ�û��ѡ��
	UserInterface_ClientList_CS.Enter();
	int re = pUserMainWindow->mClientList.GetSelectionMark();
	UserInterface_ClientList_CS.Leave();
	if (re == -1)
	{
		return false;
	}
	
	//	��ȡ��ѡ��SOCKET��ֵ
	UserInterface_ClientList_CS.Enter();
	SOCKET socket_value = 0;
	CString socket_str = pUserMainWindow->mClientList.GetItemText(pUserMainWindow->mClientList.GetSelectionMark(), 2);
	swscanf_s(socket_str, _T("%d"), &socket_value);
	out_info->socket = socket_value;

	//	��ȡ��ѡ��IP�ַ���
	std::string ip_str = CWCharToChar::WChar_To_Char(pUserMainWindow->mClientList.GetItemText(pUserMainWindow->mClientList.GetSelectionMark(), 0).GetBuffer());
	out_info->ip_str = ip_str;

	//	��ȡ��ѡ�Ķ˿���ֵ
	int port_value = 0;
	CString port_str = pUserMainWindow->mClientList.GetItemText(pUserMainWindow->mClientList.GetSelectionMark(), 1);
	swscanf_s(port_str, _T("%d"), &port_value);
	out_info->port = port_value;
	UserInterface_ClientList_CS.Leave();

	//	��ȡ��ѡ��ʱ��
	//	�����ٽ�
	myclientlistvec_cs.Enter();

	//	��������
	for (int i = 0; (myClientListVec.begin() + i) != (myClientListVec.end()); ++i)
	{
		if ((*(myClientListVec.begin() + i)).socket == socket_value)
		{
			out_info->begin_time = (*(myClientListVec.begin() + i)).begin_time;
			break;
		}
	}

	//	�˳��ٽ�
	myclientlistvec_cs.Leave();

	//	���ز����ɹ�
	return true;
}


//	TCP���ӽ������Ӧ����
void OnTCPJoin(
	SOCKET in_socket,								//	�ɹ��������ӵ�SOCKET���
	SOCKADDR_IN ClientAddr,							//	�ͻ�����Ϣ
	CMyTime	begin_time								//	�ɹ��������ӵ�ʱ���
	)
{
	//	������ʱ����
	CMonitorProtocol::CintegratedTCPServerManage::SReConInfo tmp_info;
	tmp_info.begin_time = begin_time;
	tmp_info.ip_str = inet_ntoa(ClientAddr.sin_addr);
	tmp_info.port = htons(ClientAddr.sin_port);
	tmp_info.socket = in_socket;

	//	��ӿͻ��˵��б�
	AddClient(
		tmp_info
		);
}

//	TCP���ӶϿ�����Ӧ����
void OnTCPBreak(
	SOCKET in_socket,								//	�Ͽ�����֮ǰ��SOCKET���
	SOCKADDR_IN ClientAddr,							//	�Ͽ�����֮ǰ�Ŀͻ�����Ϣ
	CMyTime	end_time								//	�Ͽ����ӵ�ʱ���
	)
{
	//	����Ƿ���ѡ���
	CMonitorProtocol::CintegratedTCPServerManage::SReConInfo tmp_info;
	GetCurrentSelectClientInfo(&tmp_info);

	//	ɾ���Ѿ��Ͽ��Ķ���
	RemoveClient(in_socket);

	//	�жϵ�ǰ�Ͽ����ӵĺ��Ѿ�ѡ����Ƿ�Ϊͬһ��
	if (in_socket == tmp_info.socket)
	{
		//	����ѡ���ı�
		UserInterface_CurrentClientEdit_CS.Enter();
		pUserMainWindow->mCurrentSelectEdit.SetWindowTextW(_T("Please select client"));
		UserInterface_CurrentClientEdit_CS.Leave();

		//	����ѡ��
		UserInterface_ClientList_CS.Enter();
		pUserMainWindow->mClientList.SetSelectionMark(-1);
		UserInterface_ClientList_CS.Leave();
	}

	//	���ͼ��ָ���Ƿ���Ҫ�ͷ�
	pScreenshort_cs.Enter();
	if (pScreenshort != NULL)
	{
		cvReleaseImage(&pScreenshort);
		pScreenshort = NULL;
	}
	pScreenshort_cs.Leave();

	//	���ٴ���
	cvDestroyWindow(REMOTE_DESKTOP_WINDOW_NAME);

	//	��ǰfpsΪ0
	current_fps_cs.Enter();
	current_fps = 0.0;
	current_fps_cs.Leave();
}


//	���Э��ķ��������մ�����
void OnRecvUserData(
	SOCKET in_socket,														//	�����SOCKET����ʾ����һ��������������
	std::vector<CMonitorProtocol::SDataArea> in_vec							//	��ڲ��������Կͻ��˵Ļ�ִ������
	)
{
	//	20170331������BUG��������������ʱ��������������û�д�����ɣ�����һ�ηǷ��Ľ�����£����Դ�ʱ����ֹ����ͼ��
	//	��鵱ǰ�Ƿ��ڼ��״̬
	bool in_proc = false;
	MonitorAboutVar_CS.Enter();
	in_proc = MonitorStatus;
	MonitorAboutVar_CS.Leave();
	if (in_proc == false) return;

	//	������Ļ��Ϣ�ṹ��
	CScreenshort::SBitmapInfo bmp_info;
	memcpy_s(&bmp_info, sizeof(CScreenshort::SBitmapInfo), (*(in_vec.begin())).pdat, sizeof(CScreenshort::SBitmapInfo));

	//	�����ǰͼ������û�д������򴴽�ͼ��
	pScreenshort_cs.Enter();
	if (pScreenshort == NULL)
	{
		//	����ͼ��
		pScreenshort = cvCreateImage(
			cvSize(bmp_info.Width, bmp_info.Height),
			IPL_DEPTH_8U,
			3
			);
	}

	//	��ȡͼ����������ݽ��б���
	unsigned char* lpBits = (unsigned char*)((*(in_vec.begin() + 1)).pdat);
	int R, G, B;
	for (int y = 0; y < bmp_info.Height; y++)
	{
		for (int x = 0; x < bmp_info.Width; x++)
		{
			B = lpBits[y*bmp_info.WidthBytes + x * 4];
			G = lpBits[y*bmp_info.WidthBytes + x * 4 + 1];
			R = lpBits[y*bmp_info.WidthBytes + x * 4 + 2];
			
			//	���ͼ��
			unsigned char* ptr = (unsigned char*)(pScreenshort->imageData + y*pScreenshort->widthStep);
			ptr[3 * x + 0] = (unsigned char)B;
			ptr[3 * x + 1] = (unsigned char)G;
			ptr[3 * x + 2] = (unsigned char)R;
		}
	}

	//	��ʾͼ��
	cvShowImage(REMOTE_DESKTOP_WINDOW_NAME, pScreenshort);
	cvWaitKey(1);
	pScreenshort_cs.Leave();
	
	//	ˢ�����
	RecvDataFlag = true;
}

//	����OpenCVͼ��ָ��
IplImage* pScreenshort = NULL;
CMyCriticalSection pScreenshort_cs;

//	�յ��ͻ������ݣ�����ɻ�ͼ��־��true��ʾ�Ѿ��յ�������ɴ���false��ʾû���յ�
bool RecvDataFlag = false;

//	���巢�ͽ����߳���
CThread SendCtrlThread;

//	������ʼ�ذ�ť�ͼ��״̬��ص�CS
CMyCriticalSection MonitorAboutVar_CS;


//	�õ�FPS
double GetFPS(void)
{
	double tmp = 0.0;
	current_fps_cs.Enter();
	tmp = current_fps;
	current_fps_cs.Leave();
	return tmp;
}

//	����FPS
void SetFPS(double in)
{
	current_fps_cs.Enter();
	current_fps = in;
	current_fps_cs.Leave();
}

//	�����̺߳����������߳�������ͻ���ͻ��˷������ݣ�Ȼ��ȴ���ִ������ɺ����ظ���������
DWORD SendCtrlThreadFunc(LPVOID lpParam)
{
	while (1)
	{
		//	����յ����ݱ�־
		RecvDataFlag = false;

		//	���췢������
		//	���ڸ��߳���ֹͣ��ʱ����ǿ����ֹ�ģ������������ʱ�������������û�б�ִ�У����Ե��µ��ڴ�й¶
		//	Ϊ�˱���������������������Ϊ��̬����ʽ
		static std::vector<CMonitorProtocol::SDataArea> tmp_vec;
		tmp_vec.clear();
		CMonitorProtocol::SDataArea tmp_dat;
		tmp_dat.length = strlen("screenshort");
		char* p_str = new char[tmp_dat.length];
		tmp_dat.pdat = p_str;
		memcpy_s(tmp_dat.pdat, tmp_dat.length, "screenshort", tmp_dat.length);
		tmp_vec.insert(tmp_vec.end(), tmp_dat);
		
		//	��������
		int rei = myTCPManage.Send(
			CurrentClientInfo.socket,
			tmp_vec
			);

		//	�������ʧ��
		if (rei == SOCKET_ERROR)
		{
			//	�رռ��״̬
			MonitorAboutVar_CS.Enter();
			pUserMainWindow->mMonitorButton.SetWindowTextW(_T("Monitor"));	//	���ü��			
			MonitorStatus = false;											//	ֹͣ����
			MonitorAboutVar_CS.Leave();

			return -1;
		}


		//	��ʱ�ȴ��ͻ������ݴ������
		while (RecvDataFlag == false) Sleep(1);

		static CMyTime begin_time;
		//begin_time.GetNowTime();
		static CMyTime end_time;
		end_time.GetNowTime();
		static CMyTime span_time;

		//	���Ϊ��һ������
		if (begin_time.GetData().type == CMyTime::UNDEFINE)
		{
			//	��һ�����в���ͳ��
		}
		//	�Ժ�����
		else
		{
			span_time = end_time - begin_time;
			double spad = static_cast<double>(span_time.GetSpanMillsec());
			double tmp_fps = 1000.0 / spad;
			current_fps_cs.Enter();
			current_fps = tmp_fps;
			current_fps_cs.Leave();
		}
		begin_time = end_time;

		//TRACE("\r\n========================%s========================\r\n", span_time.GetTime_C_Str());
	}
}

