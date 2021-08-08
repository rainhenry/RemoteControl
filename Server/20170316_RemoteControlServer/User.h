/**************************************************************************************

	程序名称：远程控制服务器
	程序版本：REV 0.2
	创建日期：2017.03.17
	最后修改：2017.03.31
	设计编写：rainhenry

	版本修订：
		0.1版本			创建工程
		0.2版本			修复pScreenshort的临界数据访问错误BUG
						优化按钮效果
						修正FPS的指示精确程度

	功能原理描述:
		通过网络监控协议，实现查看远程客户端（被监控机器）的屏幕内容，显示部分采用OpenCV的方式

**************************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////
//	重定义保护
#ifndef __USER_H__
#define __USER_H__

///////////////////////////////////////////////////////////////////////////////////////
//	包含头文件
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include "cv.h"

#include "iconv.h"
#include "UDPTCP\UDPTCP.h"
#include "CThread\CThread.h"
#include "CMyTime\CMyTime.h"
#include "CMyCriticalSection\CMyCriticalSection.hpp"
#include "CMonitorProtocol\CMonitorProtocol.h"
#include "CScreenshot\CScreenshot.h"

#include "20170316_RemoteControlServer.h"
#include "20170316_RemoteControlServerDlg.h"

///////////////////////////////////////////////////////////////////////////////////////
//	网络相关
//	定义TCP服务器资源对象
extern CTCPServerResources myTCPServerRes;

//	定义TCP服务器管理对象
extern CMonitorProtocol::CintegratedTCPServerManage myTCPManage;

//	定义当前选中的连接对象信息
extern CMonitorProtocol::CintegratedTCPServerManage::SReConInfo CurrentSelectConnInfo;

//	定义用户主窗体指针
extern CMy20170316_RemoteControlServerDlg* pUserMainWindow;

//	定义添加客户端操作,返回true表示添加成功，返回false表示失败
bool AddClient(CMonitorProtocol::CintegratedTCPServerManage::SReConInfo in_info);

//	定义移除客户端操作，成功返回true，失败返回false
bool RemoveClient(SOCKET in_socket);

//	获得当前选中的客户端信息
CMonitorProtocol::CintegratedTCPServerManage::SReConInfo GetCurrentSelectInfo(void);

//	移除所有客户端
void RemoveAllClient(void);

//	获得当前选择的客户端信息，如果有选择的，则返回true，否则返回false
bool GetCurrentSelectClientInfo(CMonitorProtocol::CintegratedTCPServerManage::SReConInfo* out_info);

//	访问用户界面连接列表的CS
extern CMyCriticalSection UserInterface_ClientList_CS;

//	访问用户界面当前连接文本框的CS
extern CMyCriticalSection UserInterface_CurrentClientEdit_CS;

//	当前已经选择的客户端信息
extern CMonitorProtocol::CintegratedTCPServerManage::SReConInfo CurrentClientInfo;

//	监控状态，true表示监控已经开始，false表示监控没有开始
extern bool MonitorStatus;

//	TCP连接接入的响应函数
void OnTCPJoin(
	SOCKET in_socket,								//	成功建立连接的SOCKET句柄
	SOCKADDR_IN ClientAddr,							//	客户端信息
	CMyTime	begin_time								//	成功建立连接的时间点
	);

//	TCP连接断开的响应函数
void OnTCPBreak(
	SOCKET in_socket,								//	断开连接之前的SOCKET句柄
	SOCKADDR_IN ClientAddr,							//	断开连接之前的客户端信息
	CMyTime	end_time								//	断开连接的时间点
	);

//	当前服务器的工作状态，true为已经工作了，false为没有工作
extern bool TCPServerStatus;

//	监控协议的服务器接收处理函数
void OnRecvUserData(
	SOCKET in_socket,														//	输入的SOCKET，表示从哪一个连接来的数据
	std::vector<CMonitorProtocol::SDataArea> in_vec							//	入口参数，来自客户端的回执数据区
	);

//	定义OpenCV图像指针
extern IplImage* pScreenshort;
extern CMyCriticalSection pScreenshort_cs;

//	定义OpenCV弹出窗口的名字
#define REMOTE_DESKTOP_WINDOW_NAME		"Remote Desktop Window"

//	收到客户端数据，并完成绘图标志，true表示已经收到，并完成处理，false表示没有收到
extern bool RecvDataFlag;

//	定义发送截屏线程类
extern CThread SendCtrlThread;

//	发送线程函数
DWORD SendCtrlThreadFunc(LPVOID lpParam);

//	定义访问监控按钮和监控状态相关的CS
extern CMyCriticalSection MonitorAboutVar_CS;

//	得到FPS
double GetFPS(void);

//	设置FPS
void SetFPS(double in);

///////////////////////////////////////////////////////////////////////////////////////
#endif	//	__USER_H__

