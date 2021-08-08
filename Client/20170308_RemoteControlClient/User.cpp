
//	�汾 REV 0.2

#include "stdafx.h"
#include "User.h"

//--------------------------------------------------------------------
//	���徲̬�������ڼ�¼��ǰ�Ĵ��ڵ���ʾ�����Ĭ��Ϊtrue����ʾ��false�ǲ���ʾ
bool window_show_status = true;

//	���尴����Ӧ����
void UserOnKey(
	CKeyRecv_base::EOnKeyType type,						//	�����Ĵ�������
	LPVOID lpparam										//	���ݽ����Ĳ���
	)
{
	if (type == CKeyRecv::ON_UP)
	{
		//window_show_status = !window_show_status;
		bool* ptmp = (bool*)lpparam;
		*ptmp = !(*ptmp);
	}
}


CKeyRecv k1;


//---------------------------------------------------------------------------
//	�����������

//	TCP�ͻ���ʵ��
CMonitorProtocol::CintegratedTCPClient myClient;

std::vector<CMonitorProtocol::SDataArea> OnRecvData(
	std::vector<CMonitorProtocol::SDataArea> in_vec							//	��ڲ���������ʱ��������
	)
{
	std::vector<CMonitorProtocol::SDataArea> tmp_vec;

	/*for (int i = 0; (in_vec.begin() + i) != (in_vec.end()); ++i)
	{
		CMonitorProtocol::SDataArea tmp_buf;
		tmp_buf.length = (*(in_vec.begin() + i)).length;
		tmp_buf.pdat = new char[tmp_buf.length];
		memcpy_s(tmp_buf.pdat, tmp_buf.length, (*(in_vec.begin() + i)).pdat, tmp_buf.length);
		tmp_vec.insert(tmp_vec.end(),tmp_buf);
	}*/

	//--------------------------------------------------------
	//	�����������
	char* in_cmd_buf = new char[(*(in_vec.begin())).length + 1];
	memset(in_cmd_buf, 0, (*(in_vec.begin())).length + 1);
	memcpy_s(in_cmd_buf, (*(in_vec.begin())).length + 1, (*(in_vec.begin())).pdat, (*(in_vec.begin())).length);
	std::string in_cmd_str = in_cmd_buf;
	if (in_cmd_str != "screenshort")
	{
		delete[](in_cmd_buf);
		CMonitorProtocol::SDataArea tmp_buf;
		tmp_buf.length = 2;
		tmp_buf.pdat = new char[2];
		memset(tmp_buf.pdat, 0, 2);
		tmp_vec.insert(tmp_vec.end(), tmp_buf);
		return tmp_vec;
	}
	delete[](in_cmd_buf);

	//--------------------------------------------------------
	//	������Ļ��Ϣ����
	CScreenshort mscrs;
	CScreenshort::SBitmapInfo bmp_info;
	bmp_info = mscrs.GetBitmapInfo();
	CMonitorProtocol::SDataArea tmp_buf;
	tmp_buf.length = sizeof(CScreenshort::SBitmapInfo);
	tmp_buf.pdat = new char[tmp_buf.length];
	memcpy_s(tmp_buf.pdat, tmp_buf.length, &bmp_info, tmp_buf.length);
	tmp_vec.insert(tmp_vec.end(), tmp_buf);

	//	�����������
	tmp_buf.length = mscrs.GetBitmapBufferSize();
	tmp_buf.pdat = new char[tmp_buf.length];
	memcpy_s(tmp_buf.pdat, tmp_buf.length, mscrs.GetBitmapBuffer(), tmp_buf.length);
	tmp_vec.insert(tmp_vec.end(), tmp_buf);

	return tmp_vec;
}


//	�������쳣�Ͽ���Ӧ����
void OnServerBreak(
	SOCKET in_socket,								//	����ʱ��socket
	SOCKADDR_IN ServerAddr,							//	�������ĵ�ַ��Ϣ
	int fail_count									//	�Ѿ�ʧ�ܵĴ���
	)
{
	//	����״̬����
	mConnectButton_cs.Enter();
	pUserWin->SetWindowTextW(_T("Server Break!"));

	//	����ʧ��
	currect_connect_status = false;

	//	���ð�ť����
	pUserWin->mConnectButton.SetWindowTextW(_T("Connenct"));
	mConnectButton_cs.Leave();
}


//	�û�����ָ��
CMy20170308_RemoteControlClientDlg* pUserWin = NULL;

//----------------------------------------------------------
//	����������Ϣ�ṹ��
SOCKADDR_IN AppConfig = { 0 };

SOCKADDR_IN defaultAppConfig = { 0 };

//	�����ʼ������
void DedaultUserDataInit(void)
{
	defaultAppConfig.sin_port = htons(1234);
	defaultAppConfig.sin_addr.S_un.S_addr = inet_addr("192.168.2.104");
}

//	�����ļ��洢��
CFileSave myfilesave(
	&AppConfig,						//	�����ǵ�ǰ���ݽṹ��
	sizeof(SOCKADDR_IN),			//	�ṹ���С���ֽڵ�λ��
	L"config.dat",					//	�ļ�·��+�ļ���
	&defaultAppConfig,				//	Ĭ��ֵ
	DedaultUserDataInit				//	�û���ʼ������
	);

//---------------------------------------------------------------------------
//	�����û���ť��CS
CMyCriticalSection mConnectButton_cs;

//	��ǰ������״̬��trueΪ�Ѿ����ӣ�falseΪû������,�����Ӱ�ť����һ��CS
bool currect_connect_status = false;