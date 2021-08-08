/************************************************************************

	工程名称：UDPTCP网络通信程序包
	工程日期：2016.12.10
	最后修改：2017.03.18
	版    本：1.0
	设    计：rainhenry
	编    码：rainhenry
	平    台：VS2013 @ Win7 SP1 64bit
	
	版本修订
		0.1版本		编写UDP服务器类
		0.2版本		对UDP服务器类增加了UNICODE编码的兼容处理
		0.3版本		将UDP_SERVER_THREAD_FUNC线程函数内部需要访问的类成员改为protected方式定义
					并将UDP_SERVER_THREAD_FUNC作为CUDPServer类的友元函数，提高信息安全和使用安全
		0.4版本		编写TCP服务器资源类
					编写TCP主机类（一个对象可以处理一个连接）
					编写TCP管理类
					TCP服务器程序OK
		0.5版本		编写TCP客户端
					修改strcpy为安全函数strcpy_s
		0.6版本		TCP客户端编写完成，并进行基本测试
		0.7版本		修复TCP管理类析构时产生的内存泄露问题
		0.8版本		修正TCP管理类重新配置连接由于容器的起始序号错误导致的崩溃BUG
		0.9版本		增加Vec方式的TCP客户端类，类似流的读取数据方式
		1.0版本		在TCP服务器资源类中增加GetSocketAddr方法，方便扩展

//////////////////////////////////////////////////////////////////////////
	本程序包提供的常用函数：
		GetAllLoaclIP								获得本机所有网卡的IP
		GetLocalHostName							获得本机名
		CUDPServer::GetIPStr						从sockaddr_in结构中提取IP信息字符串
		CUDPServer::GetIPPort						从sockaddr_in结构中提取端口值
		CUDPServer::GetIPStrFromCIPAddressCtrl		从CIPAddressCtrl控件类中获取IP信息字符串

	其他常用方法
		inet_ntoa(addrServer.sin_addr)				从sockaddr_in结构中提取IP信息字符串
		htons(addrServer.sin_port)					从sockaddr_in结构中提取端口值

	使用注意事项：
		本程序包的所有类，都不支持标准库容器，因为它们是系统资源，当采用容器的时候会导致资源失效！！
		如果想使用容器管理，请自定义管理类，并采用容器管理对象指针，一定不能直接管理对象实例！！

//////////////////////////////////////////////////////////////////////////
	UDP服务器类特点
	UDP服务器和UDP客户端，在代码上的唯一区别就在于bind函数的调用。
	经测试，UDP服务器类可以在没有发送任何信息的时候进行正常的接收。
	而UDP客户端方式的话，在没有发送数据之前，是无法接收到任何信息的。
	所以UDP服务器类完全可以取代UDP客户端，并且更好的完成这一功能。

	UDP服务器类使用说明
	//	定义接收处理函数
	void UDP_ServerFunc(
	SOCKADDR_IN remote,					//	所来自的远程主机地址信息
	char* pbuffer,								//	数据缓冲区首地址
	int	length									//	收到的字节数目
	)
	{
		//....用户代码....
		//	这里注意，remote结构体内部的信息可以用本类提供的GetIPStr方法和GetIPPort方法获取，这样更加方便
	}

	//	定义UDP服务器网络类实例，当然也可以先构造一个空的CUDPServer mCUDPServer;然后利用Config成员函数初始化也可以
	CUDPServer mCUDPServer(
	UDP_ServerFunc,
	287
	);

	//	发送
	int re = mCUDPServer.Send(
	mCUDPServer.GetIPStrFromCIPAddressCtrl(&mremoteip),		//	IP
	remote_port,											//	端口
	buff,													//	数据缓冲区
	length													//	要发送的长度
	);

//////////////////////////////////////////////////////////////////////////
	TCP服务器实现原理
	TCP服务器采用3个类实现，分别为服务器资源类、主机连接类 和 管理类
	用户在使用的时候只涉及到资源类（CTCPServerResources） 和 管理类（CTCPServerManage）。
	其中，资源类只是打开本机上的某个网卡（或全部网卡）的指定端口，然后进行绑定监听，之后作为一个资源提供给管理类使用。
	管理类中每个连接对象都是一个主机连接类对象，每一个主机连接类对象都含有有个接收线程用于接收数据，并执行用户自定义的接收处理函数。
	在管理类中又有一个应答客户端的握手请求的线程 和 一个查找所有连接对象的错误，并当发现错误的时候将连接删除等功能。
	在使用管理类的时候，需要用户提供一个接收响应函数，一个客户端接入响应函数 和 一个客户端断开响应函数。
	这里要注意的是，三个响应函数将至少被三个不同的线程调用，所以在三个响应函数中的数据要采用临界保护，不建议采用互斥量，否则关闭连接或者退出的时候会导致线程崩溃。
	最后要说明的是：服务器的资源SOCKET，与建立连接时的SOCKET是不同的东西，使用时要注意区分！！

	使用举例：
	//	定义用户接收函数
	void UserRecvProcFunc(
		SOCKET in_socket,								//	建立连接的SOCKET句柄
		char* pdat,										//	数据缓冲区首地址
		int length										//	收到的字节数目
		)
	{
		//	用户处理代码
		//	此处若要获得客户端的IP或端口可以调用管理类中的相关方法，CTCPServerManage::GetClientIPStr等
		//	也可以通过管理类里面的方法查看当前服务器的资源的IP和端口等属性
		//	......
	}

	//	定义连接响应函数
	void OnTCPConn(
		SOCKET in_socket,								//	成功建立连接的SOCKET句柄
		SOCKADDR_IN clientaddr,							//	客户端信息
		CMyTime begin_time								//	成功建立连接的时间点
		)
	{
		//	用户处理代码
		//	....	
	
	}

	//	定义断开响应函数
	void OnTCPDisco(
		SOCKET in_socket,								//	断开连接的SOCKET句柄
		SOCKADDR_IN clientaddr,							//	断开连接之前的客户端信息
		CMyTime end_time								//	断开连接的时间点
		)
	{
		//	用户处理代码
		//	....	
	}

	//	定义一个服务器资源类对象
	CTCPServerResources mTCPRes;

	//	监听本机的1234端口,正常是返回0，错误或者端口占用的时候返回SOCKET_ERROR(-1)，返回类型为int
	mTCPRes.Config(
		1234
		);

	//	定义一个服务器管理类对象
	CTCPServerManage mTCPManage;

	//	初始化配置管理类，成功返回true，失败返回false，返回类型为bool
	//	其中断开与连接响应函数可以手动指定为NULL，这样就不会处理用户的断开和连接事件
	mTCPManage.Config(
		mTCPRes,
		UserRecvProcFunc,					//	用户定义的接收处理函数
		OnTCPConn,							//	用户定义的连接响应函数
		OnTCPDisco							//	用户定义的断开响应函数
		);

	//	发送一个数据
	char re_str[] = "welcome connect host!\r\n";
	mTCPManage.Send(
		socket,										//	这个类型为SOCKET，是指的与客户端连接的SOCKET，可以在接受处理函数中得到，或者在遍历连接列表的时候得到
		re_str,										//	这里给出的多字节编码格式，如果采用宽字节可以写成： CWCharToChar(re_str, E_CHAR, E_UTF8).UTF8()  需要包含iconv.h
		strlen(re_str)								//	要发送的字节数目
		);

	//	关闭某一个指定的连接
	mTCPManage.Disconnect(socket);					//	该socket的获取方法和概念同发送数据

	//	当释放服务器资源的时候，与之使用的所有管理类对象，和里面的所有连接都将被释放
	mTCPRes.Release();

	//	遍历当前所有已经接入的客户端连接
	//	获取连接副本
	std::vector<CTCPServerManage::SReConInfo> sockcopy = mTCPManage.GetConListAllInfoCopy();

	//	获取总共接入到服务器的客户端数目
	total_con = sockcopy.size();

	//	如果仅仅为了获取连接数目，可以采用高效率的方法
	total_con = mTCPManage.GetConListCopy().size();

	//	遍历显示每个客户端的信息
	//	这种遍历方法所得到的信息仅仅为执行获取信息语句时那一刻的客户端列表，即获取连接副本的那一刻。
	for (int i = 0; i < total_con; ++i)
	{
		this->infoout.AddStr("    %d --- SOCKET(%d) ---  IP(%S)  ---  Port(%d)\r\n",
			i + 1,
			(*(sockcopy.begin() + i)).socket,
			(*(sockcopy.begin() + i)).ip_str.data(),
			(*(sockcopy.begin() + i)).port
			);
	}


//////////////////////////////////////////////////////////////////////////
	TCP客户端
	每一个客户端对象中都有一个线程，该线程负责接收用户的数据，并且处理断线时的操作，比如断线重连、等待一定时间后重连 和 统计断线次数
	配置函数Config和连接函数Connect在连接请求服务器的时候，如果此时服务器不在线或者端口没有开放，则会立即返回SOCKET_ERROR，而不会阻塞当前线程
	该类需要用户定义自己的接收处理函数，可选的是定义断开响应函数

	使用举例:

	//	定义接收响应函数
	void TCPRecvFunc(
		char* pdata,								//	数据缓冲区首地址
		int	length									//	收到的字节数目
		)
	{
		//	用户代码
	}

	//	定义断开响应函数
	void TCPBreakFunc(
		SOCKET in_socket,								//	连接时的socket
		SOCKADDR_IN addrServer,							//	服务器的地址信息
		int	total										//	已经失败的次数
		)
	{
		//	用户代码
	}

	//	定义一个TCP客户端对象
	CTCPClient mTCP_Client;

	//	配置服务器，并马上立即连接
	int re = mTCP_Client.Config(
		"192.168.2.101",							//	指定的服务器IP
		1234,										//	指定的端口
		TCPRecvFunc,								//	用户定义的接收处理函数
		true,										//	配置完成后立即进行连接
		TCPBreakFunc,								//	服务器断开响应处理函数，可选，不用的时候可以指定为NULL
		0,											//	断线重连的超时秒数，0表示断线后立即进行连接
		-1											//	重试次数，-1表示无限重试次数
		);

	//	检查连接返回结果
	if (re == SOCKET_ERROR)
	{
		//	此时为连接失败
	}
	else
	{
		//	此时为连接成功
	}

	//	发送数据
	char send_data[] = "Hello World!";
	mTCP_Client.TrySend(
		send_data,
		strlen(send_data)
		);

	//	断开操作
	mTCP_Client.Disconnect();

************************************************************************/

//////////////////////////////////////////////////////////////////////////
//	预编译头保护
#ifndef __UDPTCP_H__
#define __UDPTCP_H__

//////////////////////////////////////////////////////////////////////////
//	包含头文件
#include <stdio.h>
#include <Winsock2.h>
#include <iostream>
#include <vector>
#include "../CThread/CThread.h"
#include "../CMyCriticalSection/CMyCriticalSection.hpp"
#include "../CMyTime/CMyTime.h"

//////////////////////////////////////////////////////////////////////////
//	宏定义和配置相关和类型定义
#define		MAX_BUFFER_SIZE			10000			//	最大缓冲区字节数目

//	需要用户定义的接收函数，此函数结构对于UDP适用
//	由于该函数为线程驱动，所以由该函数所操作的数据和其他地方共享的话，需要进行线程同步保护
typedef void (*P_USER_RECV_FUNC)(
	SOCKADDR_IN,						//	所来自的远程主机地址信息
	char*,								//	数据缓冲区首地址
	int									//	收到的字节数目
	); 

//	需要用户定义的接收处理函数，此函数结构对于TCP适用
typedef void(*P_USER_RECV_TCP_SERVER_FUNC)(
	SOCKET,								//	建立连接的SOCKET句柄
	char*,								//	数据缓冲区首地址
	int									//	收到的字节数目
	);

//	需要用户定义的当TCP客户端接入时的响应函数
typedef void(*P_USER_ON_TCP_CLIENT_JOIN_FUNC)(
	SOCKET,								//	成功建立连接的SOCKET句柄
	SOCKADDR_IN,						//	客户端信息
	CMyTime								//	成功建立连接的时间点
	);

//	需要用户定义的当TCP客户端断开时的响应函数
typedef void(*P_USER_ON_TCP_CLIENT_BREAK_FUNC)(
	SOCKET,								//	断开连接之前的SOCKET句柄
	SOCKADDR_IN,						//	断开连接之前的客户端信息
	CMyTime								//	断开连接的时间点
	);

//	需要用户定义的TCP客户端接收处理函数
typedef void(*P_USER_RECV_TCP_CLIENT_FUNC)(
	char*,								//	数据缓冲区首地址
	int									//	收到的字节数目
	);

//	需要用户定义的当TCP服务器断开的时候的响应函数
typedef void(*P_USER_ON_TCP_SERVER_BREAK_FUNC)(
	SOCKET,								//	连接时的socket
	SOCKADDR_IN,						//	服务器的地址信息
	int									//	已经失败的次数
	);


//////////////////////////////////////////////////////////////////////////
//	获得全部的本机IP
std::vector<CString> GetAllLoaclIP(void);

//	获取本计算机名
CString GetLocalHostName(void);

//////////////////////////////////////////////////////////////////////////
//	UDP服务器类定义
class CUDPServer
{
private:
	//	当指定地址结构体，从中获得IP字符串的返回缓冲区定义
	char ip_str_for_getipstr[50];

	//	当指定地址控件的时候的字符串返回缓冲区定义
	char ip_str_for_GetIPStrFromCIPAddressCtrl[50];

protected:
	bool init_flag;								//	已经初始化的标识
	SOCKET msocket;
	WSADATA mwsaData;
	SOCKADDR_IN localaddr;						//	本地地址信息
	char recv_buffer[MAX_BUFFER_SIZE];			//	定义接收缓冲区
	CThread mCThread;							//	定义接收线程类实例
	P_USER_RECV_FUNC pUserRecvFunc;				//	用户自定义的接收处理函数地址

	//	对于这些函数加入临界保护可以让多线程之间共享使用它们
	CMyCriticalSection mGetIPCMCS;				//	得到IP字符串函数的临界保护
	CMyCriticalSection mGetIPFromCMCS;			//	得到IP字符串函数的临界保护
	CMyCriticalSection mGetPortCMCS;			//	得到端口数值函数的临界保护

public:
	//	构造函数

	//	任何参数都不提供
	CUDPServer();

	//	由于是构造服务端，所以需要提供所绑定本机的端口和IP
	//	提供端口和IP
	CUDPServer(
		P_USER_RECV_FUNC pFunc,			//	用户自定义的接收处理函数
		u_short		port,				//	绑定本机的端口
		const char*	ip_str				//	绑定本机的IP
		);
	
	//	提供端口，IP为INADDR_ANY，本机所有IP
	CUDPServer(
		P_USER_RECV_FUNC pFunc,			//	用户自定义的接收处理函数
		u_short	port
		);

	//	提供端口和IP
	void Config(
		P_USER_RECV_FUNC pFunc,			//	用户自定义的接收处理函数
		u_short		port,				//	绑定本机的端口
		const char*	ip_str				//	绑定本机的IP
		);

	//	提供端口，IP为INADDR_ANY，本机所有IP
	void Config(
		P_USER_RECV_FUNC pFunc,			//	用户自定义的接收处理函数
		u_short	port
		);

	//	关闭连接
	void Close(void);

	//	发送数据，操作成功会返回所发送的字节数目，否则会返回SOCKET_ERROR
	int Send(
		const char* remote_ip,			//	要发送到的目标IP
		u_short remote_port,			//	要发送到的目标端口
		char* pBuffer,					//	发送数据的首地址
		int length						//	发送长度（字节单位）
		);

	//	得到IP字符串
	char* GetIPStr(SOCKADDR_IN);

	//	得到端口数值
	u_short GetIPPort(SOCKADDR_IN);

	//	通过IP地址控件得到IP地址字符串
	char* GetIPStrFromCIPAddressCtrl(CIPAddressCtrl* pCIPAddressCtrl);

	//	析构函数
	~CUDPServer();

	//	UDP服务器类接收函数声明
	friend DWORD UDP_SERVER_THREAD_FUNC(LPVOID pParam);
};

//////////////////////////////////////////////////////////////////////////
/*
	服务器资源类的作用是指定本机的IP和端口资源作为可以监听的部分，然后提供给服务器连接管理类作为使用
	连接管理类的内部有多个线程工作，一个线程负责监听指定网络资源的连接请求，另的线程负责遍历已经成功建立的连接
	遍历连接的线程的其中，有1个为发送线程，该线程负责通过遍历发送缓冲区的数据向客户端发送数据
	剩下的遍历连接线程都为接收线程，该线程的个数为成功建立连接的个数。每个线程都负责从自己的客户端中读取数据。
	当成功获取数据的时候，会统一调用一个接收函数指针，该函数由用户提供
	在读取和发送的过程中，如果发现失败的情况，即为连接断开，将从连接资源中删除。
	在删除的过程中，如果为接收过程中出现错误，则可以直接删除该资源，然后正常退出本接收线程。
	如果是在发送过程中发现的问题，则需要先结束读取线程，然后再从连接列表中移除该连接。
*/

//	TCP服务器资源类定义
class CTCPServerResources
{
private:
	//	定义返回字符串
	char ip_str[50];

protected:
	bool init_flag;								//	已经初始化的标识

	WSADATA mwsaData;

	//	本服务器资源的地址信息
	SOCKADDR_IN addrServer;

	//	服务器资源
	SOCKET msocket;

	//	获取socket的cs
	CMyCriticalSection get_socket_cs;

	//	获取配置状态的cs
	CMyCriticalSection get_configstatus_cs;
	
public:
	//	不提供任何初始化信息
	CTCPServerResources();

	//	提供IP端口的构造函数
	CTCPServerResources(
		const char* ip_str,						//	指定监听的本机IP
		u_short port,							//	指定监听的本机端口
		int backlog = SOMAXCONN					//	连接完成的队列参数，增大可以提高并发数
		);

	//	提供端口的构造函数
	CTCPServerResources(
		u_short port,							//	指定监听的本机端口
		int backlog = SOMAXCONN					//	连接完成的队列参数，增大可以提高并发数
		);


	//	配置服务端监听的IP和端口,正常是返回0，错误的时候返回SOCKET_ERROR
	int Config(
		const char* ip_str,						//	指定监听的本机IP
		u_short port,							//	指定监听的本机端口
		int backlog = SOMAXCONN					//	连接完成的队列参数，增大可以提高并发数
		);

	//	配置服务端监听的端口,正常是返回0，错误的时候返回SOCKET_ERROR，指定所有网卡IP
	int Config(
		u_short port,							//	指定监听的本机端口
		int backlog = SOMAXCONN					//	连接完成的队列参数，增大可以提高并发数
		);

	//	释放资源
	void Release(void);

	//	析构函数
	~CTCPServerResources();

	//	获得服务器资源
	SOCKET GetSocket(void);

	//	获取服务器地址结构体
	SOCKADDR_IN GetSocketAddr(void);

	//	得到当前的配置状态，true为配置成功，false为配置失败
	bool GetConfigStatus(void);

	//	得到服务器的IP字符串，当未配置的时候，返回空字符串
	char* GetServerIPString(void);

	//	得到服务器端口，当未配置的时候，返回-1
	int GetServerPort(void);

	//	定义TCP服务器管理类为资源类的友元类，即管理类可以访问资源类的保护成员
	friend class CTCPServerManage;
};

//------------------------------------------------------------------------
/*
	该TCP请求处理类的任务是：当管理类成功收到一个连接请求的时候，将创建一个本类的实例
	该实例将被赋予连接得到的SOCKET对象 和 指定的接收处理函数指针
*/
//	TCP请求处理类定义
class CTCPServerHost
{
protected:
	//	接收处理线程
	CThread m_recv_thread;

	//	初始标志
	bool init_flag;

	//	接收线程指针
	P_USER_RECV_TCP_SERVER_FUNC pFunc;

	//	连接的SOCKET
	SOCKET m_con_socket;

	//	失败状态，true为失败，false为正常
	bool fail_status;

	//	操作连接SOCKET的CS
	CMyCriticalSection opt_m_con_socket_cs;

	//	操作失败状态标志的CS
	CMyCriticalSection opt_fail_status_cs;

	//	定义接收缓冲区
	char recv_buff[MAX_BUFFER_SIZE];

public:
	//	构造函数，不提供参数
	CTCPServerHost();

	//	配置函数,正常是返回0，错误的时候返回SOCKET_ERROR
	int Config(
		SOCKET m_socket,								//	连接的SOCKET
		P_USER_RECV_TCP_SERVER_FUNC p_infunc			//	传入的函数指针
		);

	//	发送数据，由于所在类为服务器，所以为向客户端发送数据,正常是返回已经发送的字节，错误的时候返回SOCKET_ERROR
	int Send(
		char* pdata,									//	数据首地址
		int length										//	数据长度
		);

	//	得到当前的配置状态，true为配置成功，false为配置失败
	bool GetConfigStatus(void);

	//	析构函数
	~CTCPServerHost();
	
	//	获取失败状态
	bool GetFailStatus(void);

	//	断开连接
	void Disconnect(void);

	//	TCP服务器主机类接收函数声明
	friend DWORD TCP_SERVER_HOST_RECV_THREAD_FUNC(LPVOID pParam);
};

//------------------------------------------------------------------------
/*
	TCP管理类里面定义2个线程，一个是等待所有的客户端进行连接，另一个是找到已知连接列表里面错误的连接，并把它们从列表中删除
	封装的主要功能有
		初始化函数
		获取连接列表副本函数
		断开连接函数
		发送函数
		获取指定连接SOCKET的IP和端口
		获取本管理类中配置的服务器资源的IP和端口

	管理连接对象结构体采用封装AddConList与RemoveConList的方法，这样可以更好的控制内存和资源分配
*/
//	TCP管理类
class CTCPServerManage
{
public:
	//	连接容器内部的类型定义,连接信息结构体
	typedef struct tagSConInfo
	{
		SOCKET socket;								//	与客户端连接的SOCKET句柄
		SOCKADDR_IN addrClient;						//	客户端的地址信息
		CMyTime* pbegin_time;						//	开始建立连接的时间
		CTCPServerHost* pCon;						//	连接对象
	}SConInfo,*pSConInfo;

	//	监听处理线程函数
	friend DWORD TCP_SERVER_MANAGE_ACCEPT_THREAD(LPVOID pParam);

	//	找到错误线程函数
	friend DWORD TCP_SERVER_MANAGE_FIND_ERROR_THREAD(LPVOID pParam);

	//	返回连接副本的结构信息
	typedef struct tagSReConInfo
	{
		SOCKET socket;								//	连接的SOCKET句柄
		std::string ip_str;							//	IP地址字符串
		int port;									//	端口
		CMyTime begin_time;							//	开始连接的时间
	}SReConInfo;

private:
	//	连接容器
	std::vector<SConInfo*> mConList;

	//	连接容器的操作cs
	CMyCriticalSection mconlist_cs;

	//	定义返回客户端IP地址的字符串缓冲区
	char ip_str_GetClientIPStr[50];

	//	定义返回服务器IP地址的字符串缓冲区
	char ip_str_GetServerIPString[50];

protected:
	//	初始化标志
	bool init_flag;

	//	服务器资源的SOCKET
	SOCKET socketServer;

	//	指定服务器资源的地址信息
	SOCKADDR_IN addrServer;

	//	监听处理线程
	CThread m_accept_thread;

	//	找到错误线程
	CThread m_find_error_thread;

	//	向连接容器里面加入对象，成功返回true,失败返回false
	bool AddConList(
		SOCKET in_socket,							//	已经成功建立连接的SOCKET，也是唯一的标识
		SOCKADDR_IN	addrClient,						//	已连接客户端的地址信息
		P_USER_RECV_TCP_SERVER_FUNC precvfunc		//	用户定义的接收函数
		);

	//	从连接容器里面删除一个对象，成功返回true,失败返回false
	bool RemoveConList(
		SOCKET in_socket							//	删除指定的SOCKET
		);

	//	自动从列表中移除错误的连接
	void AutoRemoveError(void);

	//	用户的接收处理函数
	P_USER_RECV_TCP_SERVER_FUNC pRecvFunc;

	//	定义客户端接入响应函数
	P_USER_ON_TCP_CLIENT_JOIN_FUNC pOnJoinFunc;

	//	定义客户端断开响应函数
	P_USER_ON_TCP_CLIENT_BREAK_FUNC pOnBreadFunc;

public:
	//	无参数构造函数
	CTCPServerManage();

	//	析构函数
	~CTCPServerManage();

	//	初始化函数，成功返回true，失败返回false
	bool Config(
		CTCPServerResources& rCTCPServerResources,					//	服务器资源的引用
		P_USER_RECV_TCP_SERVER_FUNC pfunc							//	用户接收函数
		);

	//	带有连接和断开响应函数的初始化配置函数，成功返回true，失败返回false
	bool Config(
		CTCPServerResources& rCTCPServerResources,					//	服务器资源的引用
		P_USER_RECV_TCP_SERVER_FUNC pfunc,							//	用户接收函数	
		P_USER_ON_TCP_CLIENT_JOIN_FUNC ponjoinfunc,					//	定义客户端接入响应函数	
		P_USER_ON_TCP_CLIENT_BREAK_FUNC ponbreadfunc				//	定义客户端断开响应函数
		);

	//	返回连接副本，仅仅包含SOCKET的列表
	std::vector<SOCKET> GetConListCopy(void);

	//	返回连接副本，包含全部信息的列表
	std::vector<SReConInfo> GetConListAllInfoCopy(void);

	//	断开连接，true表示操作成功，false表示列表中没有此连接
	bool Disconnect(SOCKET in_socket);

	//	发送数据，>0表示操作成功，SOCKET_ERROR表示列表中没有此连接,0表示发送失败
	int Send(
		SOCKET in_socket,											//	指定的SOCKET句柄
		char* pdata,												//	发送数据的缓冲区首地址
		int length													//	要发送的长度
		);

	//	得到IP字符串，错误将返回一个空字符串
	char* GetClientIPStr(SOCKET in_socket);

	//	得到端口数值，错误将返回0
	u_short GetClientPort(SOCKET in_socket);

	//	得到建立的开始时间，错误将返回一个未定义的时间类
	CMyTime GetClientBeginTime(SOCKET in_socket);

	//	得到服务器的IP字符串，当未配置的时候，返回空字符串
	char* GetServerIPString(void);

	//	得到服务器端口，当未配置的时候，返回-1
	int GetServerPort(void);

	//	获取服务器资源SOCKET，没有配置的时候返回空
	SOCKET GetServerSOCKET(void);
};

//////////////////////////////////////////////////////////////////////////
/*
	TCP客户端类
	一个客户端类对象中，包含一个接收线程，该线程即控制调用用户的接收响应函数，也控制断线重连和重试等功能。
	配置是否初始化，要看init_flag的标志
	连接是否建立，要看sockClient的值

	注意：在进行连接服务器的时候，无论调用Config或者Connect都不会阻塞，如果服务端不存在，则会立即返回失败。
*/

class CTCPClient
{
private:
	//	IP字符串缓冲区
	char ip_str[50];

protected:
	//	初始化标志
	bool init_flag;

	WSADATA mwsaData;

	//	服务器的地址信息
	SOCKADDR_IN addrServer;

	//	接收线程
	CThread m_recv_thread;

	//	用户的接收处理函数
	P_USER_RECV_TCP_CLIENT_FUNC pRecvFunc;

	//	用户指定的当与服务器断开连接时的响应函数
	P_USER_ON_TCP_SERVER_BREAK_FUNC pOnBreakFunc;

	//	客户端socket，也被用来表示连接状态，当为NULL的时候，为未连接。其他情况表示连接。
	SOCKET sockClient;

	//	断线重连的超时秒数，0表示断线后立即进行连接，>0表示断线后经过多久后重试建立连接
	int RetrySec;	

	//	重试次数，-1表示无限重试次数，0表示断线后不进行重试连接，>0表示断线后重试的次数
	int RetryCount;				

	//	接收缓冲区
	char recv_buff[MAX_BUFFER_SIZE];

public:
	//	无参数构造函数
	CTCPClient();

	//	析构函数
	~CTCPClient();

	//	配置函数，默认配置为 断线后立即重试连接，并且无限重试次数，成功返回0，失败返回SOCKET_ERROR
	int Config(
		char* remote_ip,										//	远程主机的IP地址
		u_short remote_port,									//	远程主机的端口
		P_USER_RECV_TCP_CLIENT_FUNC precvfunc,					//	用户定义的接收函数
		bool now_conn,											//	是否立即进行连接，true为配置成功后立即连接，false为仅仅配置，不进行连接
		P_USER_ON_TCP_SERVER_BREAK_FUNC ponbreakfunc = NULL,	//	当与服务器断开连接的时候，所调用的响应函数
		int retry_sec = 0,										//	断线重连的超时秒数，0表示断线后立即进行连接，>0表示断线后经过多久后重试建立连接
		int retry_count = -1									//	重试次数，-1表示无限重试次数，0表示断线后不进行重试连接，>0表示断线后重试的次数
		);

	//	连接函数,If no error occurs, connect returns zero. Otherwise, it returns SOCKET_ERROR, and a specific error code can be retrieved by calling WSAGetLastError.
	int Connect(void);

	//	断开函数
	void Disconnect(void);

	//	返回连接状态，true表示已经建立连接，false表示没有建立连接
	bool GetConnectStatus(void);

	//	获得配置状态，true表示已经成功配置，false表示没有配置
	bool GetConfigStatus(void);

	//	得到服务器端口
	int GetServerPort(void);

	//	得到服务器IP
	char* GetServerIPStr(void);

	//	发送数据，>0表示操作成功，SOCKET_ERROR表示列表中没有此连接,0表示发送失败，该发送程序在重试连接的时候也会返回错误
	int TrySend(
		char* pdata,										//	发送数据的缓冲区首地址
		int length											//	要发送的长度
		);

	//	得到重试的线程状态，在连接断线的时候，如果线程还依然在运行，那么说明当前处于断线重连的时候
	CThread::EThreadState GetThreadStatus(void);

	//	获得SOCKET
	SOCKET GetSocket(void);

	//	获得重试次数
	int GetRetryCount(void);

	//	获得断开时的超时秒数
	int GetRetrySec(void);

	//	接收处理线程函数
	friend DWORD TCP_CLIENT_RECV_PROC_THREAD(LPVOID pParam);
};

//////////////////////////////////////////////////////////////////////////
//	VEC读取方式的TCP客户端类
class CTCPClientVecRecv:
	public CTCPClient
{
protected:
	//	内部的客户端对象
	CTCPClient mTcpClient;

	//	数据接收容器
	std::vector<unsigned char> RecvBuffVec;

	//	访问接收容器的CS
	CMyCriticalSection recvbuffvec_cs;

	//	向缓冲区内追加数据
	void AddByteToBufferVec(
		char* pbuff,				//	要加入数据的首地址
		int length					//	要追加数据的长度
		);

public:
	//	无参数构造函数
	CTCPClientVecRecv();

	//	析构函数
	~CTCPClientVecRecv();
	
	//	获得已经接收到的容器中有多少的字节
	int GetBufferBytesCount(void);

	//	读取n个字节，非阻塞
	std::vector<unsigned char> TryRecv(int n);

	//	读取n个字节，阻塞读取
	std::vector<unsigned char> WhileRecv(int n);

	//	清空缓冲区
	void ClearBuffer(void);

	//	配置函数，默认配置为 断线后立即重试连接，并且无限重试次数，成功返回0，失败返回SOCKET_ERROR
	int Config(
		char* remote_ip,										//	远程主机的IP地址
		u_short remote_port,									//	远程主机的端口
		bool now_conn,											//	是否立即进行连接，true为配置成功后立即连接，false为仅仅配置，不进行连接
		P_USER_ON_TCP_SERVER_BREAK_FUNC ponbreakfunc = NULL,	//	当与服务器断开连接的时候，所调用的响应函数
		int retry_sec = 0,										//	断线重连的超时秒数，0表示断线后立即进行连接，>0表示断线后经过多久后重试建立连接
		int retry_count = -1									//	重试次数，-1表示无限重试次数，0表示断线后不进行重试连接，>0表示断线后重试的次数
		);

	//	连接函数,If no error occurs, connect returns zero. Otherwise, it returns SOCKET_ERROR, and a specific error code can be retrieved by calling WSAGetLastError.
	int Connect(void);

	//	接收处理线程函数
	friend DWORD TCP_CLIENT_VEC_RECV_PROC_THREAD(LPVOID pParam);

};

//////////////////////////////////////////////////////////////////////////
#endif		//	__UDPTCP_H__
