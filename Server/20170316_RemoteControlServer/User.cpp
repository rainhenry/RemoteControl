
//	版本 REV 0.2

#include "stdafx.h"
#include "User.h"

//	定义当前的fps值
double current_fps = 0.0;
CMyCriticalSection current_fps_cs;

//	定义TCP服务器资源对象
CTCPServerResources myTCPServerRes;

//	定义TCP服务器管理对象
CMonitorProtocol::CintegratedTCPServerManage myTCPManage;

//	定义客户端接入列表
std::vector<CMonitorProtocol::CintegratedTCPServerManage::SReConInfo> myClientListVec;

//	定义客户端接入列表的CS
CMyCriticalSection myclientlistvec_cs;

//	定义当前选中的连接对象信息
CMonitorProtocol::CintegratedTCPServerManage::SReConInfo CurrentSelectConnInfo;

//	定义用户主窗体指针
CMy20170316_RemoteControlServerDlg* pUserMainWindow;

//	当前服务器的工作状态，true为已经工作了，false为没有工作
bool TCPServerStatus = false;

//	访问用户界面连接列表的CS
CMyCriticalSection UserInterface_ClientList_CS;

//	访问用户界面当前连接文本框的CS
CMyCriticalSection UserInterface_CurrentClientEdit_CS;

//	当前已经选择的客户端信息
CMonitorProtocol::CintegratedTCPServerManage::SReConInfo CurrentClientInfo;

//	监控状态，true表示监控已经开始，false表示监控没有开始
bool MonitorStatus = false;


//	定义添加客户端操作,返回true表示添加成功，返回false表示失败
bool AddClient(CMonitorProtocol::CintegratedTCPServerManage::SReConInfo in_info)
{
	//	进入临界
	myclientlistvec_cs.Enter();

	//	插入到信息VEC中
	myClientListVec.insert(myClientListVec.end(), in_info);

	//	将信息插入到用户对话框的列表控件中
	//	获取IP字符串
	std::wstring ip_str = CWCharToChar::Char_To_WChar(in_info.ip_str);

	//	获取端口字符串
	CString port_str;
	port_str.Format(_T("%d"), in_info.port);

	//	获得SOCKET字符串
	CString socket_str;
	socket_str.Format(_T("%d"), in_info.socket);

	//	获得时间字符串
	std::wstring begin_time_str = CWCharToChar::Char_To_WChar(in_info.begin_time.GetTimeString());

	//	添加到用户界面中
	UserInterface_ClientList_CS.Enter();
	pUserMainWindow->mClientList.InsertItem(0, ip_str.data());
	pUserMainWindow->mClientList.SetItemText(0, 1, port_str);
	pUserMainWindow->mClientList.SetItemText(0, 2, socket_str);
	pUserMainWindow->mClientList.SetItemText(0, 3, begin_time_str.data());
	UserInterface_ClientList_CS.Leave();

	//	退出临界
	myclientlistvec_cs.Leave();

	//	操作成功
	return true;
}

//	定义移除客户端操作，成功返回true，失败返回false
bool RemoveClient(SOCKET in_socket)
{
	//	进入临界
	myclientlistvec_cs.Enter();

	//	尝试寻找VEC中的匹配对象
	for (int i = 0; (myClientListVec.begin() + i) != (myClientListVec.end()); ++i)
	{
		//	找到后执行删除
		if ((*(myClientListVec.begin() + i)).socket == in_socket)
		{
			myClientListVec.erase(myClientListVec.begin() + i);
			--i;
		}
	}

	//	遍历用户界面的列表控件，删除匹配的项目
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

	//	退出临界
	myclientlistvec_cs.Leave();

	//	操作成功
	return true;
}

//	移除所有客户端
void RemoveAllClient(void)
{
	//	进入临界
	myclientlistvec_cs.Enter();

	//	删除全部VEC
	myClientListVec.clear();

	//	删除全部链接列表
	UserInterface_ClientList_CS.Enter();
	while(pUserMainWindow->mClientList.GetItemCount() != 0)
		pUserMainWindow->mClientList.DeleteItem(0);
	UserInterface_ClientList_CS.Leave();

	//	退出临界
	myclientlistvec_cs.Leave();
}

//	获得当前选中的客户端信息
CMonitorProtocol::CintegratedTCPServerManage::SReConInfo GetCurrentSelectInfo(void)
{
	//	定义临时返回对象
	CMonitorProtocol::CintegratedTCPServerManage::SReConInfo tmp_info = { 0 };
	
	//	判断当前是否为选中状态
	UserInterface_ClientList_CS.Enter();
	int re = pUserMainWindow->mClientList.GetSelectionMark();
	UserInterface_ClientList_CS.Leave();
	if (re == -1)
	{
		//	返回结果
		return tmp_info;
	}

	//	进入临界
	myclientlistvec_cs.Enter();

	//	获得当前选中的socket值
	UserInterface_ClientList_CS.Enter();
	CString socket_str = pUserMainWindow->mClientList.GetItemText(pUserMainWindow->mClientList.GetSelectionMark(), 2);
	UserInterface_ClientList_CS.Leave();
	int socket_value = 0;
	swscanf_s(socket_str, _T("%d"), &socket_value);

	//	遍历VEC
	for (int i = 0; (myClientListVec.begin() + i) != (myClientListVec.end()); ++i)
	{
		//	找到后返回数据对象
		if ((*(myClientListVec.begin() + i)).socket == socket_value)
		{
			tmp_info = (*(myClientListVec.begin() + i));
			break;
		}
	}

	//	退出临界
	myclientlistvec_cs.Leave();

	//	返回结果
	return tmp_info;
}

//	获得当前选择的客户端信息，如果有选择的，则返回true，否则返回false
bool GetCurrentSelectClientInfo(CMonitorProtocol::CintegratedTCPServerManage::SReConInfo* out_info)
{
	//	检查输入参数
	if ((out_info == NULL) || (out_info == (CMonitorProtocol::CintegratedTCPServerManage::SReConInfo*)(-1)))
	{
		return false;
	}

	//	检查当前是否没有选择
	UserInterface_ClientList_CS.Enter();
	int re = pUserMainWindow->mClientList.GetSelectionMark();
	UserInterface_ClientList_CS.Leave();
	if (re == -1)
	{
		return false;
	}
	
	//	获取所选的SOCKET数值
	UserInterface_ClientList_CS.Enter();
	SOCKET socket_value = 0;
	CString socket_str = pUserMainWindow->mClientList.GetItemText(pUserMainWindow->mClientList.GetSelectionMark(), 2);
	swscanf_s(socket_str, _T("%d"), &socket_value);
	out_info->socket = socket_value;

	//	获取所选的IP字符串
	std::string ip_str = CWCharToChar::WChar_To_Char(pUserMainWindow->mClientList.GetItemText(pUserMainWindow->mClientList.GetSelectionMark(), 0).GetBuffer());
	out_info->ip_str = ip_str;

	//	获取所选的端口数值
	int port_value = 0;
	CString port_str = pUserMainWindow->mClientList.GetItemText(pUserMainWindow->mClientList.GetSelectionMark(), 1);
	swscanf_s(port_str, _T("%d"), &port_value);
	out_info->port = port_value;
	UserInterface_ClientList_CS.Leave();

	//	获取所选的时间
	//	进入临界
	myclientlistvec_cs.Enter();

	//	遍历查找
	for (int i = 0; (myClientListVec.begin() + i) != (myClientListVec.end()); ++i)
	{
		if ((*(myClientListVec.begin() + i)).socket == socket_value)
		{
			out_info->begin_time = (*(myClientListVec.begin() + i)).begin_time;
			break;
		}
	}

	//	退出临界
	myclientlistvec_cs.Leave();

	//	返回操作成功
	return true;
}


//	TCP连接接入的响应函数
void OnTCPJoin(
	SOCKET in_socket,								//	成功建立连接的SOCKET句柄
	SOCKADDR_IN ClientAddr,							//	客户端信息
	CMyTime	begin_time								//	成功建立连接的时间点
	)
{
	//	构造临时对象
	CMonitorProtocol::CintegratedTCPServerManage::SReConInfo tmp_info;
	tmp_info.begin_time = begin_time;
	tmp_info.ip_str = inet_ntoa(ClientAddr.sin_addr);
	tmp_info.port = htons(ClientAddr.sin_port);
	tmp_info.socket = in_socket;

	//	添加客户端到列表
	AddClient(
		tmp_info
		);
}

//	TCP连接断开的响应函数
void OnTCPBreak(
	SOCKET in_socket,								//	断开连接之前的SOCKET句柄
	SOCKADDR_IN ClientAddr,							//	断开连接之前的客户端信息
	CMyTime	end_time								//	断开连接的时间点
	)
{
	//	检查是否还有选择的
	CMonitorProtocol::CintegratedTCPServerManage::SReConInfo tmp_info;
	GetCurrentSelectClientInfo(&tmp_info);

	//	删除已经断开的对象
	RemoveClient(in_socket);

	//	判断当前断开连接的和已经选择的是否为同一个
	if (in_socket == tmp_info.socket)
	{
		//	设置选择文本
		UserInterface_CurrentClientEdit_CS.Enter();
		pUserMainWindow->mCurrentSelectEdit.SetWindowTextW(_T("Please select client"));
		UserInterface_CurrentClientEdit_CS.Leave();

		//	设置选择
		UserInterface_ClientList_CS.Enter();
		pUserMainWindow->mClientList.SetSelectionMark(-1);
		UserInterface_ClientList_CS.Leave();
	}

	//	检查图像指针是否需要释放
	pScreenshort_cs.Enter();
	if (pScreenshort != NULL)
	{
		cvReleaseImage(&pScreenshort);
		pScreenshort = NULL;
	}
	pScreenshort_cs.Leave();

	//	销毁窗口
	cvDestroyWindow(REMOTE_DESKTOP_WINDOW_NAME);

	//	当前fps为0
	current_fps_cs.Enter();
	current_fps = 0.0;
	current_fps_cs.Leave();
}


//	监控协议的服务器接收处理函数
void OnRecvUserData(
	SOCKET in_socket,														//	输入的SOCKET，表示从哪一个连接来的数据
	std::vector<CMonitorProtocol::SDataArea> in_vec							//	入口参数，来自客户端的回执数据区
	)
{
	//	20170331，修正BUG，当结束监听的时候，由于网络数据没有处理完成，导致一次非法的界面更新，所以此时将禁止更新图像
	//	检查当前是否在监控状态
	bool in_proc = false;
	MonitorAboutVar_CS.Enter();
	in_proc = MonitorStatus;
	MonitorAboutVar_CS.Leave();
	if (in_proc == false) return;

	//	构造屏幕信息结构体
	CScreenshort::SBitmapInfo bmp_info;
	memcpy_s(&bmp_info, sizeof(CScreenshort::SBitmapInfo), (*(in_vec.begin())).pdat, sizeof(CScreenshort::SBitmapInfo));

	//	如果当前图像数据没有创建，则创建图像
	pScreenshort_cs.Enter();
	if (pScreenshort == NULL)
	{
		//	创建图像
		pScreenshort = cvCreateImage(
			cvSize(bmp_info.Width, bmp_info.Height),
			IPL_DEPTH_8U,
			3
			);
	}

	//	获取图像的像素数据进行遍历
	unsigned char* lpBits = (unsigned char*)((*(in_vec.begin() + 1)).pdat);
	int R, G, B;
	for (int y = 0; y < bmp_info.Height; y++)
	{
		for (int x = 0; x < bmp_info.Width; x++)
		{
			B = lpBits[y*bmp_info.WidthBytes + x * 4];
			G = lpBits[y*bmp_info.WidthBytes + x * 4 + 1];
			R = lpBits[y*bmp_info.WidthBytes + x * 4 + 2];
			
			//	填充图像
			unsigned char* ptr = (unsigned char*)(pScreenshort->imageData + y*pScreenshort->widthStep);
			ptr[3 * x + 0] = (unsigned char)B;
			ptr[3 * x + 1] = (unsigned char)G;
			ptr[3 * x + 2] = (unsigned char)R;
		}
	}

	//	显示图像
	cvShowImage(REMOTE_DESKTOP_WINDOW_NAME, pScreenshort);
	cvWaitKey(1);
	pScreenshort_cs.Leave();
	
	//	刷新完成
	RecvDataFlag = true;
}

//	定义OpenCV图像指针
IplImage* pScreenshort = NULL;
CMyCriticalSection pScreenshort_cs;

//	收到客户端数据，并完成绘图标志，true表示已经收到，并完成处理，false表示没有收到
bool RecvDataFlag = false;

//	定义发送截屏线程类
CThread SendCtrlThread;

//	定义访问监控按钮和监控状态相关的CS
CMyCriticalSection MonitorAboutVar_CS;


//	得到FPS
double GetFPS(void)
{
	double tmp = 0.0;
	current_fps_cs.Enter();
	tmp = current_fps;
	current_fps_cs.Leave();
	return tmp;
}

//	设置FPS
void SetFPS(double in)
{
	current_fps_cs.Enter();
	current_fps = in;
	current_fps_cs.Leave();
}

//	发送线程函数，当该线程启动后就会向客户端发送数据，然后等待回执处理完成后，再重复上述流程
DWORD SendCtrlThreadFunc(LPVOID lpParam)
{
	while (1)
	{
		//	清除收到数据标志
		RecvDataFlag = false;

		//	构造发送数据
		//	由于该线程在停止的时候是强制终止的，所以里面的临时对象的析构函数没有被执行，所以导致的内存泄露
		//	为了避免这个情况，将容器定义为静态的形式
		static std::vector<CMonitorProtocol::SDataArea> tmp_vec;
		tmp_vec.clear();
		CMonitorProtocol::SDataArea tmp_dat;
		tmp_dat.length = strlen("screenshort");
		char* p_str = new char[tmp_dat.length];
		tmp_dat.pdat = p_str;
		memcpy_s(tmp_dat.pdat, tmp_dat.length, "screenshort", tmp_dat.length);
		tmp_vec.insert(tmp_vec.end(), tmp_dat);
		
		//	发送数据
		int rei = myTCPManage.Send(
			CurrentClientInfo.socket,
			tmp_vec
			);

		//	如果发送失败
		if (rei == SOCKET_ERROR)
		{
			//	关闭监控状态
			MonitorAboutVar_CS.Enter();
			pUserMainWindow->mMonitorButton.SetWindowTextW(_T("Monitor"));	//	设置监控			
			MonitorStatus = false;											//	停止工作
			MonitorAboutVar_CS.Leave();

			return -1;
		}


		//	此时等待客户端数据处理完成
		while (RecvDataFlag == false) Sleep(1);

		static CMyTime begin_time;
		//begin_time.GetNowTime();
		static CMyTime end_time;
		end_time.GetNowTime();
		static CMyTime span_time;

		//	如果为第一次运行
		if (begin_time.GetData().type == CMyTime::UNDEFINE)
		{
			//	第一次运行不做统计
		}
		//	以后运行
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

