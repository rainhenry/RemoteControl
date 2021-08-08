
//	版本 REV 0.2

#include "stdafx.h"
#include "User.h"

//--------------------------------------------------------------------
//	定义静态变量用于记录当前的窗口的显示情况，默认为true是显示，false是不显示
bool window_show_status = true;

//	定义按键响应函数
void UserOnKey(
	CKeyRecv_base::EOnKeyType type,						//	按键的触发类型
	LPVOID lpparam										//	传递进来的参数
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
//	定义网络相关

//	TCP客户端实例
CMonitorProtocol::CintegratedTCPClient myClient;

std::vector<CMonitorProtocol::SDataArea> OnRecvData(
	std::vector<CMonitorProtocol::SDataArea> in_vec							//	入口参数，请求时的数据区
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
	//	检查输入命令
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
	//	构造屏幕信息数据
	CScreenshort mscrs;
	CScreenshort::SBitmapInfo bmp_info;
	bmp_info = mscrs.GetBitmapInfo();
	CMonitorProtocol::SDataArea tmp_buf;
	tmp_buf.length = sizeof(CScreenshort::SBitmapInfo);
	tmp_buf.pdat = new char[tmp_buf.length];
	memcpy_s(tmp_buf.pdat, tmp_buf.length, &bmp_info, tmp_buf.length);
	tmp_vec.insert(tmp_vec.end(), tmp_buf);

	//	构造截屏数据
	tmp_buf.length = mscrs.GetBitmapBufferSize();
	tmp_buf.pdat = new char[tmp_buf.length];
	memcpy_s(tmp_buf.pdat, tmp_buf.length, mscrs.GetBitmapBuffer(), tmp_buf.length);
	tmp_vec.insert(tmp_vec.end(), tmp_buf);

	return tmp_vec;
}


//	服务器异常断开响应函数
void OnServerBreak(
	SOCKET in_socket,								//	连接时的socket
	SOCKADDR_IN ServerAddr,							//	服务器的地址信息
	int fail_count									//	已经失败的次数
	)
{
	//	设置状态文字
	mConnectButton_cs.Enter();
	pUserWin->SetWindowTextW(_T("Server Break!"));

	//	连接失败
	currect_connect_status = false;

	//	设置按钮文字
	pUserWin->mConnectButton.SetWindowTextW(_T("Connenct"));
	mConnectButton_cs.Leave();
}


//	用户窗口指针
CMy20170308_RemoteControlClientDlg* pUserWin = NULL;

//----------------------------------------------------------
//	定义配置信息结构体
SOCKADDR_IN AppConfig = { 0 };

SOCKADDR_IN defaultAppConfig = { 0 };

//	定义初始化函数
void DedaultUserDataInit(void)
{
	defaultAppConfig.sin_port = htons(1234);
	defaultAppConfig.sin_addr.S_un.S_addr = inet_addr("192.168.2.104");
}

//	定义文件存储类
CFileSave myfilesave(
	&AppConfig,						//	这里是当前数据结构体
	sizeof(SOCKADDR_IN),			//	结构体大小（字节单位）
	L"config.dat",					//	文件路径+文件名
	&defaultAppConfig,				//	默认值
	DedaultUserDataInit				//	用户初始化函数
	);

//---------------------------------------------------------------------------
//	操作用户按钮的CS
CMyCriticalSection mConnectButton_cs;

//	当前的连接状态，true为已经连接，false为没有连接,和连接按钮共享一个CS
bool currect_connect_status = false;