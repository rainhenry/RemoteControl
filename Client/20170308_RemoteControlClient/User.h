
/*****************************************************************************************

	程序名称：远程控制客户端 
	程序版本：REV 0.2
	创建日期：2017.03.08
	最后修改：2017.03.17
	设计编写：rainhenry

	版本修订：
		版本0.1			创建工程
		版本0.2			增加功能，保存上一次的链接配置，IP地址和端口
						优化按钮功能

	功能原理描述：
		仅仅带有查看远程计算机屏幕的功能
		采用远程监控协议
		客户端收到的第一个数据区内容必须是"screenshort"字符串，这里仅仅比较字符串，所以第一个数据区的长度可以随意，后面用0x00填充即可
		然后客户端会执行截屏操作，在第一个数据区将返回屏幕的信息，包括分辨率和遍历用宽度字节数目
		在第二个数据区是全部图像的像素矩阵

*****************************************************************************************/

#ifndef __USER_H__
#define __USER_H__

//--------------------------------------------------------------------
//	包含自定义头文件
#include "CMyTime\CMyTime.h"
#include "UDPTCP\UDPTCP.h"
#include "CBrowseDlg\CBrowseDlg.h"
#include "iconv.h"
#include "CScreenshot\CScreenshot.h"
#include "CKeyRecv\CKeyRecv.h"
#include "CMonitorProtocol\CMonitorProtocol.h"
#include "20170308_RemoteControlClient.h"
#include "20170308_RemoteControlClientDlg.h"
#include "CFileSave\CFileSave.h"

//--------------------------------------------------------------------
//	定义静态变量用于记录当前的窗口的显示情况，默认为true是显示，false是不显示
extern bool window_show_status;

//	定义按键响应函数
void UserOnKey(
	CKeyRecv_base::EOnKeyType type,						//	按键的触发类型
	LPVOID lpparam										//	传递进来的参数
	);

extern CKeyRecv k1;

//---------------------------------------------------------------------------
//	定义网络相关
//	TCP客户端实例
extern CMonitorProtocol::CintegratedTCPClient myClient;

//	监控协议接收数据处理函数
std::vector<CMonitorProtocol::SDataArea> OnRecvData(
	std::vector<CMonitorProtocol::SDataArea> in_vec							//	入口参数，请求时的数据区
	);

//	服务器异常断开响应函数
void OnServerBreak(
	SOCKET in_socket,								//	连接时的socket
	SOCKADDR_IN ServerAddr,							//	服务器的地址信息
	int fail_count									//	已经失败的次数
	);

//	用户窗口指针
extern CMy20170308_RemoteControlClientDlg* pUserWin;

//---------------------------------------------------------------------------
//	定义文件保存配置相关
//	定义配置信息结构体
extern SOCKADDR_IN AppConfig;
extern SOCKADDR_IN defaultAppConfig;

//	定义初始化函数
void DedaultUserDataInit(void);

//	定义文件存储类
extern CFileSave myfilesave;

//---------------------------------------------------------------------------
//	操作用户按钮的CS
extern CMyCriticalSection mConnectButton_cs;

//	当前的连接状态，true为已经连接，false为没有连接,和连接按钮共享一个CS
extern bool currect_connect_status;

#endif		//	__USER_H__

