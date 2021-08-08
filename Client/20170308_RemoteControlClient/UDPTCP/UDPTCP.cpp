
#include "stdafx.h"
#include "UDPTCP.h"

//	程序版本 1.0

//////////////////////////////////////////////////////////////////////////
//	获得全部的本机IP
std::vector<CString> GetAllLoaclIP(void)
{
	std::vector<CString> LocalAllIP;
	CString str;

	//	初始化异步套接字
	WSADATA wsaData;
	char szHostName[512];
	WSAStartup( MAKEWORD(2,1), &wsaData );

	struct hostent * pHost;			//	定义主机信息结构体对象
	int i;							//	定义循环变量
	char ip_str[50];
	gethostname(szHostName, sizeof(szHostName));
	pHost = gethostbyname(szHostName);			//	根据主机名获得主机信息
	//	循环得到可用的网卡IP
	for( i = 0; pHost!= NULL && pHost->h_addr_list[i]!= NULL; i++ )  
	{
		strcpy_s(ip_str,inet_ntoa (*(struct in_addr *)pHost->h_addr_list[i]));	//	将IP地址转换为字符串
#ifdef UNICODE
		str.Format(L"%s",ip_str);
#else
		str.Format("%s",ip_str);
#endif
		LocalAllIP.insert(LocalAllIP.begin(),str);								//	存入容器
	}  

	WSACleanup();
	return LocalAllIP;
}

//	获取本计算机名
CString GetLocalHostName(void)
{
	CString str;
	char szHostName[512];

	//	初始化异步套接字
	WSADATA wsaData;
	WSAStartup( MAKEWORD(2,1), &wsaData );
	gethostname(szHostName, sizeof(szHostName));
#ifdef UNICODE
	str.Format(L"%s",szHostName);
#else
	str.Format("%s",szHostName);
#endif
	WSACleanup();

	return str;
}


//////////////////////////////////////////////////////////////////////////
//	UDP服务器类接收函数声明
DWORD UDP_SERVER_THREAD_FUNC(LPVOID pParam)
{
	//	强制转换，获得操作指针
	CUDPServer* pCUDPServer = (CUDPServer*)pParam;
	SOCKET socket1;
	SOCKADDR_IN remote;
	int fromlen =sizeof(remote);
	int re;

	//	接收主循环
	while (1)
	{
		//	如果没有初始化，则一直挂起
		if (pCUDPServer->init_flag == false)
		{
			Sleep(10);
			continue;
		}

		//	获取套接字实例
		socket1 = pCUDPServer->msocket;

		//	开始接收
		if ((re = recvfrom(					//	如果操作正确，recvfrom将返回接收到的字节数目
			socket1,
			pCUDPServer->recv_buffer,
			MAX_BUFFER_SIZE,
			0,
			(struct sockaddr*)&remote,
			&fromlen
			))!=SOCKET_ERROR)
		{
			//	执行用户处理函数
			pCUDPServer->pUserRecvFunc(
				remote,							//	远端主机信息
				pCUDPServer->recv_buffer,		//	缓冲区首地址
				re								//	收到的字节数目
				);
		}
	}

	return 0;
}

//	提供端口和IP
CUDPServer::CUDPServer(
	P_USER_RECV_FUNC pFunc,			//	用户自定义的接收处理函数
	u_short		port,				//	绑定本机的端口
	const char*	ip_str				//	绑定本机的IP
	):
mCThread(
	UDP_SERVER_THREAD_FUNC,
	this,
	CThread::STOP					//	由于构造时，初始化套接字需要时间，所以不能马上启动线程类
	)
{
	//	清空字符串缓冲区
	memset(this->ip_str_for_getipstr, 0, sizeof(this->ip_str_for_getipstr));
	memset(this->ip_str_for_GetIPStrFromCIPAddressCtrl,0,sizeof(this->ip_str_for_GetIPStrFromCIPAddressCtrl));

	//	初始化套接字DLL
	if (WSAStartup(MAKEWORD(2,1),&mwsaData)) //调用Windows Sockets DLL
	{
		WSACleanup();
		abort();			//	直接终止当前程序
	}

	//	配置信息和绑定
	localaddr.sin_family=AF_INET;
	localaddr.sin_port=htons(port);						//	监听端口
	localaddr.sin_addr.s_addr=inet_addr(ip_str);		//	指定IP
	msocket=socket(AF_INET,SOCK_DGRAM,0);
	bind(msocket,(struct sockaddr*)&localaddr,sizeof(localaddr));

	//	配置用户处理函数
	pUserRecvFunc = pFunc;

	//	启动接收线程
	mCThread.Run();

	//	已经初始化
	init_flag = true;
}

//	提供端口，IP为INADDR_ANY，本机所有IP
CUDPServer::CUDPServer(
	P_USER_RECV_FUNC pFunc,			//	用户自定义的接收处理函数
	u_short	port
	):
mCThread(
	UDP_SERVER_THREAD_FUNC,
	this,
	CThread::STOP					//	由于构造时，初始化套接字需要时间，所以不能马上启动线程类
	)
{
	//	清空字符串缓冲区
	memset(this->ip_str_for_getipstr, 0, sizeof(this->ip_str_for_getipstr));
	memset(this->ip_str_for_GetIPStrFromCIPAddressCtrl, 0, sizeof(this->ip_str_for_GetIPStrFromCIPAddressCtrl));

	//	初始化套接字DLL
	if (WSAStartup(MAKEWORD(2,1),&mwsaData)) //调用Windows Sockets DLL
	{
		WSACleanup();
		abort();			//	直接终止当前程序
	}

	//	配置信息和绑定
	localaddr.sin_family=AF_INET;
	localaddr.sin_port=htons(port);						//	监听端口
	localaddr.sin_addr.s_addr=INADDR_ANY;				//	本机所有IP
	msocket=socket(AF_INET,SOCK_DGRAM,0);
	bind(msocket,(struct sockaddr*)&localaddr,sizeof(localaddr));

	//	配置用户处理函数
	pUserRecvFunc = pFunc;

	//	启动接收线程
	mCThread.Run();

	//	已经初始化
	init_flag = true;
}

//	任何参数都不提供
CUDPServer::CUDPServer():
	mCThread(
	UDP_SERVER_THREAD_FUNC,
	this,
	CThread::STOP					//	由于构造时，初始化套接字需要时间，所以不能马上启动线程类
	)
{
	//	清空字符串缓冲区
	memset(this->ip_str_for_getipstr, 0, sizeof(this->ip_str_for_getipstr));
	memset(this->ip_str_for_GetIPStrFromCIPAddressCtrl, 0, sizeof(this->ip_str_for_GetIPStrFromCIPAddressCtrl));

	//	未初始化
	init_flag = false;
}

//	发送数据
int CUDPServer::Send(
	const char* remote_ip,			//	要发送到的目标IP
	u_short remote_port,			//	要发送到的目标端口
	char* pBuffer,					//	发送数据的首地址
	int length						//	发送长度（字节单位）
	)
{
	//	如果没有初始化，则直接返回错误
	if (init_flag == false)
	{
		return SOCKET_ERROR;
	}

	//	构造远程主机信息
	SOCKADDR_IN remote;
	remote.sin_family=AF_INET;
	remote.sin_port=htons(remote_port);						//	指定端口
	remote.sin_addr.s_addr=inet_addr(remote_ip);;			//	指定IP
	
	//	获取套接字实例
	SOCKET socket1 = msocket;

	//	配置fromlen
	int fromlen =sizeof(remote);

	//	执行发送
	int re = sendto(
		socket1,
		pBuffer,
		length,
		0,
		(struct sockaddr*)&remote,
		fromlen
		);

	//	返回状态
	return re;
}

//	提供端口和IP
void CUDPServer::Config(
	P_USER_RECV_FUNC pFunc,			//	用户自定义的接收处理函数
	u_short		port,				//	绑定本机的端口
	const char*	ip_str				//	绑定本机的IP
	)
{
	//	如果已经初始化，需要析构之前的信息
	if (init_flag == true)
	{
		//	停止线程
		mCThread.Stop();
		Sleep(100);

		//	关闭套接字
		closesocket(msocket);
		WSACleanup();
	}

	//	初始化套接字DLL
	if (WSAStartup(MAKEWORD(2,1),&mwsaData)) //调用Windows Sockets DLL
	{
		WSACleanup();
		abort();			//	直接终止当前程序
	}

	//	配置信息和绑定
	localaddr.sin_family=AF_INET;
	localaddr.sin_port=htons(port);						//	监听端口
	localaddr.sin_addr.s_addr=inet_addr(ip_str);		//	指定IP
	msocket=socket(AF_INET,SOCK_DGRAM,0);
	bind(msocket,(struct sockaddr*)&localaddr,sizeof(localaddr));

	//	配置用户处理函数
	pUserRecvFunc = pFunc;

	//	启动接收线程
	mCThread.Run();

	//	已经初始化
	init_flag = true;
}

//	提供端口，IP为INADDR_ANY，本机所有IP
void CUDPServer::Config(
	P_USER_RECV_FUNC pFunc,			//	用户自定义的接收处理函数
	u_short	port
	)
{
	//	如果已经初始化，需要析构之前的信息
	if (init_flag == true)
	{
		//	停止线程
		mCThread.Stop();
		Sleep(100);

		//	关闭套接字
		closesocket(msocket);
		WSACleanup();

	}

	//	初始化套接字DLL
	if (WSAStartup(MAKEWORD(2,1),&mwsaData)) //调用Windows Sockets DLL
	{
		WSACleanup();
		abort();			//	直接终止当前程序
	}
	
	//	配置信息和绑定
	localaddr.sin_family=AF_INET;
	localaddr.sin_port=htons(port);						//	监听端口
	localaddr.sin_addr.s_addr=INADDR_ANY;				//	本机所有IP
	msocket=socket(AF_INET,SOCK_DGRAM,0);
	bind(msocket,(struct sockaddr*)&localaddr,sizeof(localaddr));

	//	配置用户处理函数
	pUserRecvFunc = pFunc;

	//	启动接收线程
	mCThread.Run();

	//	已经初始化
	init_flag = true;
}

//	关闭连接
void CUDPServer::Close(void)
{
	//	如果已经配置
	if (init_flag == true)
	{
		//	关闭线程
		mCThread.Stop();
		Sleep(100);

		//	关闭套接字
		closesocket(msocket);
		WSACleanup();
		init_flag = false;
	}
}

//	析构函数
CUDPServer::~CUDPServer()
{
	//	如果已经配置
	if (init_flag == true)
	{
		//	关闭套接字
		closesocket(msocket);
		WSACleanup();
	}
}


//	得到IP字符串
char* CUDPServer::GetIPStr(SOCKADDR_IN in)
{
	memset(ip_str_for_getipstr, 0, sizeof(ip_str_for_getipstr));
	mGetIPCMCS.Enter();
	strcpy_s(ip_str_for_getipstr, inet_ntoa(in.sin_addr));
	mGetIPCMCS.Leave();
	return ip_str_for_getipstr;
}

//	得到端口数值
u_short CUDPServer::GetIPPort(SOCKADDR_IN in)
{
	u_short port;
	mGetPortCMCS.Enter();
	port = htons(in.sin_port);
	mGetPortCMCS.Leave();
	return port;
}

//	通过IP地址控件得到IP地址字符串
char* CUDPServer::GetIPStrFromCIPAddressCtrl(CIPAddressCtrl* pCIPAddressCtrl)
{
	//	获得目标IP字符串
	memset(ip_str_for_GetIPStrFromCIPAddressCtrl, 0, sizeof(ip_str_for_GetIPStrFromCIPAddressCtrl));
	mGetIPFromCMCS.Enter();
	SOCKADDR_IN remote;
	pCIPAddressCtrl->GetAddress(
		remote.sin_addr.S_un.S_un_b.s_b1,
		remote.sin_addr.S_un.S_un_b.s_b2,
		remote.sin_addr.S_un.S_un_b.s_b3,
		remote.sin_addr.S_un.S_un_b.s_b4
		);
	strcpy_s(ip_str_for_GetIPStrFromCIPAddressCtrl, GetIPStr(remote));
	mGetIPFromCMCS.Leave();
	return ip_str_for_GetIPStrFromCIPAddressCtrl;
}

//////////////////////////////////////////////////////////////////////////

//	不提供任何初始化信息
CTCPServerResources::CTCPServerResources()
{
	//	清空字符串缓冲区
	memset(this->ip_str,0,sizeof(this->ip_str));

	//	设置为未初始化
	init_flag = false;

	//	设定未初始化的服务器资源为空
	msocket = NULL;
}

//	释放资源
void CTCPServerResources::Release(void)
{
	//	检查临界是否都成功推出
	if (this->get_configstatus_cs.InCriticalSection() == true)	this->get_configstatus_cs.Leave();
	if (this->get_socket_cs.InCriticalSection() == true)	this->get_socket_cs.Leave();

	//	如果已经配置
	if (init_flag == true)
	{
		//	关闭套接字
		init_flag = false;
		closesocket(msocket);
		WSACleanup();
	}
}

//	析构函数
CTCPServerResources::~CTCPServerResources()
{
	//	检查临界是否都成功推出
	if (this->get_configstatus_cs.InCriticalSection() == true)	this->get_configstatus_cs.Leave();
	if (this->get_socket_cs.InCriticalSection() == true)	this->get_socket_cs.Leave();

	//	如果已经配置
	if (init_flag == true)
	{
		//	关闭套接字
		init_flag = false;
		closesocket(msocket);
		WSACleanup();
	}
}

//	配置服务端监听的IP和端口,正常是返回0，错误的时候返回SOCKET_ERROR
int CTCPServerResources::Config(
	const char* ip_str,						//	指定监听的本机IP
	u_short port,							//	指定监听的本机端口
	int backlog								//	连接完成的队列参数，增大可以提高并发数
	)
{
	//	如果已经初始化，需要析构之前的信息
	if (init_flag == true)
	{
		//	关闭套接字
		init_flag = false;
		closesocket(msocket);
		WSACleanup();

	}

	//	初始化套接字DLL
	if (WSAStartup(MAKEWORD(2, 1), &mwsaData)) //调用Windows Sockets DLL
	{
		WSACleanup();
		abort();			//	直接终止当前程序
	}

	//	创建socket
	this->msocket = socket(AF_INET, SOCK_STREAM, 0);

	//	检查socket是否合法
	if ((msocket == NULL) || (msocket == ((SOCKET)(-1))))
	{
		//	关闭套接字
		WSACleanup();
		msocket = NULL;
		return SOCKET_ERROR;
	}

	//	定义返回检查临时变量
	int re = NULL;

	//	绑定
	addrServer.sin_addr.S_un.S_addr = inet_addr(ip_str);// htonl(INADDR_ANY);
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(port);
	re = bind(msocket, (SOCKADDR *)&addrServer, sizeof(addrServer));

	//	检查绑定是否成功
	if (re == SOCKET_ERROR)
	{
		//	关闭套接字
		closesocket(msocket);
		WSACleanup();
		return SOCKET_ERROR;
	}

	// 监听
	re = listen(msocket, backlog);

	//	检查监听是否成功
	if (re == SOCKET_ERROR)
	{
		//	关闭套接字
		closesocket(msocket);
		WSACleanup();
		return SOCKET_ERROR;
	}

	//	运行到这里就配置成功了
	init_flag = true;
	return NULL;
}


//	配置服务端监听的端口,正常是返回0，错误的时候返回SOCKET_ERROR，指定所有网卡IP
int CTCPServerResources::Config(
	u_short port,							//	指定监听的本机端口
	int backlog								//	连接完成的队列参数，增大可以提高并发数
	)
{
	//	如果已经初始化，需要析构之前的信息
	if (init_flag == true)
	{
		//	关闭套接字
		init_flag = false;
		closesocket(msocket);
		WSACleanup();

	}

	//	初始化套接字DLL
	if (WSAStartup(MAKEWORD(2, 1), &mwsaData)) //调用Windows Sockets DLL
	{
		WSACleanup();
		abort();			//	直接终止当前程序
	}

	//	创建socket
	this->msocket = socket(AF_INET, SOCK_STREAM, 0);

	//	检查socket是否合法
	if ((msocket == NULL) || (msocket == ((SOCKET)(-1))))
	{
		//	关闭套接字
		WSACleanup();
		msocket = NULL;
		return SOCKET_ERROR;
	}

	//	定义返回检查临时变量
	int re = NULL;

	//	绑定
	addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(port);
	re = bind(msocket, (SOCKADDR *)&addrServer, sizeof(addrServer));

	//	检查绑定是否成功
	if (re == SOCKET_ERROR)
	{
		//	关闭套接字
		closesocket(msocket);
		WSACleanup();
		return SOCKET_ERROR;
	}

	// 监听
	re = listen(msocket, backlog);

	//	检查监听是否成功
	if (re == SOCKET_ERROR)
	{
		//	关闭套接字
		closesocket(msocket);
		WSACleanup();
		return SOCKET_ERROR;
	}

	//	运行到这里就配置成功了
	init_flag = true;
	return NULL;
}


//	获得服务器资源
SOCKET CTCPServerResources::GetSocket(void)
{
	this->get_socket_cs.Enter();
	SOCKET tmp_socket = msocket;
	this->get_socket_cs.Leave();
	return tmp_socket;
}

//	获取服务器地址结构体
SOCKADDR_IN CTCPServerResources::GetSocketAddr(void)
{
	return this->addrServer;
}

//	得到当前的配置状态，true为配置成功，false为配置失败
bool CTCPServerResources::GetConfigStatus(void)
{
	this->get_configstatus_cs.Enter();
	bool tmp_init_flag = this->init_flag;
	this->get_configstatus_cs.Leave();
	return tmp_init_flag;
}

//	提供IP端口的构造函数
CTCPServerResources::CTCPServerResources(
	const char* ip_str,						//	指定监听的本机IP
	u_short port,							//	指定监听的本机端口
	int backlog								//	连接完成的队列参数，增大可以提高并发数
	)
{
	//	清空字符串缓冲区
	memset(this->ip_str, 0, sizeof(this->ip_str));

	//	设置为未初始化
	init_flag = false;

	//	设定未初始化的服务器资源为空
	msocket = NULL;

	this->Config(
		ip_str,
		port,
		backlog
		);
}

//	提供端口的构造函数
CTCPServerResources::CTCPServerResources(
	u_short port,							//	指定监听的本机端口
	int backlog								//	连接完成的队列参数，增大可以提高并发数
	)
{
	//	清空字符串缓冲区
	memset(this->ip_str, 0, sizeof(this->ip_str));

	//	设置为未初始化
	init_flag = false;

	//	设定未初始化的服务器资源为空
	msocket = NULL;

	this->Config(
		port,
		backlog
		);
}

//	得到服务器的IP字符串
char* CTCPServerResources::GetServerIPString(void)
{
	//	定义返回字符串
	memset(ip_str,0,sizeof(ip_str));

	//	如果没有配置
	if (this->init_flag == false)
	{
		return ip_str;
	}

	//	获得服务器IP
	strcpy_s(ip_str, inet_ntoa(this->addrServer.sin_addr));

	//	返回ip字符串
	return ip_str;
}

//	得到服务器端口
int CTCPServerResources::GetServerPort(void)
{
	//	定义返回端口
	int re_port = -1;

	//	如果没有配置
	if (this->init_flag == false)
	{
		return re_port;
	}

	//	获得服务器端口
	re_port = htons(this->addrServer.sin_port);

	//	返回端口
	return re_port;
}

//-------------------------------------------------------------------------------------------------
//	这里是服务器连接处理类的定义

//	TCP服务器主机接收函数
DWORD TCP_SERVER_HOST_RECV_THREAD_FUNC(LPVOID pParam)
{
	//	得到传入参数
	CTCPServerHost* pCTCPServerHost = (CTCPServerHost*)pParam;

	//	定义返回值
	int re = 0;

	//	获取SOCKET，为了提高接收效率，将获取放到循环外
	pCTCPServerHost->opt_m_con_socket_cs.Enter();
	SOCKET tmp_socket = pCTCPServerHost->m_con_socket;
	pCTCPServerHost->opt_m_con_socket_cs.Leave();

	//	进入接收循环
	while (1)
	{

		//	阻塞接收数据
		re = ::recv(
			tmp_socket,
			pCTCPServerHost->recv_buff,
			MAX_BUFFER_SIZE,
			0
			);

		//	检查接收是否失败，当re为0的时候表示连接关闭了
		if ((re == SOCKET_ERROR)||(re == 0))
		{
			//	设置失败标识
			pCTCPServerHost->opt_fail_status_cs.Enter();
			pCTCPServerHost->fail_status = true;
			pCTCPServerHost->opt_fail_status_cs.Leave();

			//	退出接收线程
			return -1;
		}

		//	接收成功的话，执行用户处理函数，此时用户函数也为阻塞式处理，所以当下一次接收到来的时候，缓冲区的数据被用户函数所独占
		pCTCPServerHost->pFunc(
			tmp_socket,
			pCTCPServerHost->recv_buff,
			re
			);
	}

	return 0;
}

//	构造函数，不提供参数
CTCPServerHost::CTCPServerHost()
{
	opt_fail_status_cs.Enter();
	this->fail_status = false;				//	没有失败
	opt_fail_status_cs.Leave();
	this->init_flag = false;				//	未初始化
	opt_m_con_socket_cs.Enter();
	this->m_con_socket = NULL;				//	空的连接对象
	opt_m_con_socket_cs.Leave();
	this->pFunc = NULL;						//	空的接收处理函数指针
}

//	配置函数,正常是返回0，错误的时候返回SOCKET_ERROR
int CTCPServerHost::Config(
	SOCKET m_socket,						//	连接的SOCKET
	P_USER_RECV_TCP_SERVER_FUNC p_infunc			//	传入的函数指针
	)
{
	//	由于配置的时候有重新开始的意思，所以这里情况失败标识，重新配置
	opt_fail_status_cs.Enter();
	this->fail_status = false;
	opt_fail_status_cs.Leave();

	//	检查是否已经存在配置
	if (this->init_flag == true)
	{
		//	清空标志，防止操作的时候发生错误
		this->init_flag = false;

		//	终止接收线程
		this->m_recv_thread.Stop();
		Sleep(10);

		//	关闭连接
		opt_m_con_socket_cs.Enter();
		closesocket(this->m_con_socket);
		opt_m_con_socket_cs.Leave();
	}

	//	检查输入参数是否合法
	if ((m_socket == NULL) || (m_socket == (SOCKET)-1) || (p_infunc == NULL) || (p_infunc == (P_USER_RECV_TCP_SERVER_FUNC)-1))
	{
		return SOCKET_ERROR;
	}

	//	赋值参数
	opt_m_con_socket_cs.Enter();
	this->m_con_socket = m_socket;
	opt_m_con_socket_cs.Leave();
	this->pFunc = p_infunc;
	this->m_recv_thread.Config(
		TCP_SERVER_HOST_RECV_THREAD_FUNC,
		this,
		CThread::RUN
		);

	//	配置成功
	this->init_flag = true;
	return 0;
}

//	发送数据，由于所在类为服务器，所以为向客户端发送数据,,正常是返回已经发送的字节，错误的时候返回SOCKET_ERROR
int CTCPServerHost::Send(
	char* pdata,							//	数据首地址
	int length								//	数据长度
	)
{
	//	如果当前为失败状态，则不作处理
	opt_fail_status_cs.Enter();
	bool tmp_fail_status = this->fail_status;
	opt_fail_status_cs.Leave();
	if (tmp_fail_status == true)
	{
		return SOCKET_ERROR;	
	}

	//	判断是否配置，如果没有配置
	if (this->init_flag == false)
	{
		return SOCKET_ERROR;
	}

	//	检查参数，如果输入地址非法
	if ((pdata == NULL) || (pdata == (char*)-1))
	{
		return SOCKET_ERROR;
	}

	//	检查参数，如果输入长度非法
	if (length <= 0)
	{
		return SOCKET_ERROR;
	}

	//	发送数据
	opt_m_con_socket_cs.Enter();
	SOCKET tmp_socket = this->m_con_socket;
	opt_m_con_socket_cs.Leave();
	int re = ::send(
		tmp_socket,
		pdata,
		length,
		0
		);

	//	检查是否发送失败
	if (re == SOCKET_ERROR)
	{
		//	设置失败标识
		opt_fail_status_cs.Enter();
		this->fail_status = true;
		opt_fail_status_cs.Leave();

		//	终止接收线程
		this->m_recv_thread.Stop();
		Sleep(10);

		//	关闭连接
		opt_m_con_socket_cs.Enter();
		closesocket(this->m_con_socket);
		opt_m_con_socket_cs.Leave();
	}

	return re;
}

//	得到当前的配置状态，true为配置成功，false为配置失败
bool CTCPServerHost::GetConfigStatus(void)
{
	return this->init_flag;
}

//	获取失败状态
bool CTCPServerHost::GetFailStatus(void)
{
	opt_fail_status_cs.Enter();
	bool tmp_fail_status = this->fail_status;
	opt_fail_status_cs.Leave();
	return tmp_fail_status;
}

//	断开连接
void CTCPServerHost::Disconnect(void)
{
	//	如果已经配置了的话，并且在没有失败的时候
	if ((this->init_flag == true) && (GetFailStatus() == false))
	{
		//	清空标志，防止操作的时候发生错误
		this->init_flag = false;

		//	终止接收线程
		this->m_recv_thread.Stop();
		Sleep(10);

		//	关闭连接
		opt_m_con_socket_cs.Enter();
		closesocket(this->m_con_socket);
		opt_m_con_socket_cs.Leave();
	}
}

//	析构函数
CTCPServerHost::~CTCPServerHost()
{
	//	断开与客户端的连接并释放资源
	this->Disconnect();
}

//----------------------------------------------------------------------
//	TCP管理类

//	监听处理线程函数
DWORD TCP_SERVER_MANAGE_ACCEPT_THREAD(LPVOID pParam)
{
	//	定义临时对象
	SOCKET sockConnection = NULL;
	SOCKADDR_IN addrClient;
	int len = sizeof(addrClient);

	//	转换输入参数
	CTCPServerManage* pCObj = (CTCPServerManage*)(pParam);

	//	进入连接等待循环
	while (1)
	{
		//	阻塞式应答连接请求
		sockConnection = ::accept(pCObj->socketServer, (SOCKADDR *)&addrClient, &len);

		//	连接成功后，将该客户端加入连接列表
		pCObj->AddConList(
			sockConnection,
			addrClient,
			pCObj->pRecvFunc
			);
	}

	return 0;
}

//	找到错误线程函数
DWORD TCP_SERVER_MANAGE_FIND_ERROR_THREAD(LPVOID pParam)
{
	//	转换输入参数
	CTCPServerManage* pCObj = (CTCPServerManage*)(pParam);

	//	进入找错误的循环
	while (1)
	{
		//	首先进行延时，每隔200ms进行一次检查错误
		Sleep(200);

		//	执行自动移除错误
		pCObj->AutoRemoveError();
	}

	return 0;
}

//	无参数构造函数
CTCPServerManage::CTCPServerManage()
{
	//	清空字符串缓冲区
	memset(this->ip_str_GetClientIPStr,0,sizeof(this->ip_str_GetClientIPStr));
	memset(this->ip_str_GetServerIPString,0,sizeof(this->ip_str_GetServerIPString));

	this->init_flag = false;					//	设置为初始化
	this->socketServer = NULL;					//	服务器资源SOCKET为空

	this->pOnJoinFunc = NULL;					//	连接响应函数为空
	this->pOnBreadFunc = NULL;					//	断开响应函数为空

}

//	析构函数
CTCPServerManage::~CTCPServerManage()
{
	//	检查是否已经初始化过
	if (this->init_flag == true)
	{
		//	停止两个线程
		this->init_flag = false;
		this->m_accept_thread.Stop();
		this->m_find_error_thread.Stop();
		Sleep(10);

		//	遍历循环关掉所有的客户端连接
		this->mconlist_cs.Enter();
		for (int i = 0; (this->mConList.begin() + i) != (this->mConList.end()); ++i)
		{			
			//	定义结构体指针
			SConInfo* ptmp_dat = NULL;

			//	获取该对象的地址
			ptmp_dat = (*(this->mConList.begin() + i));

			//	从列表中删除
			this->mConList.erase(this->mConList.begin() + i);
			--i;

			//	释放资源
			delete(ptmp_dat->pbegin_time);
			ptmp_dat->pCon->Disconnect();
			delete(ptmp_dat->pCon);
			delete(ptmp_dat);
		}
		this->mconlist_cs.Leave();
	}
}

//	向连接容器里面加入对象，成功返回true,失败返回false
bool CTCPServerManage::AddConList(
	SOCKET in_socket,					//	已经成功建立连接的SOCKET，也是唯一的标识
	SOCKADDR_IN	addrClient,				//	已连接客户端的地址信息
	P_USER_RECV_TCP_SERVER_FUNC precvfunc		//	用户定义的接收函数
	)
{
	//	检查参数
	//	检查in_socket
	if ((in_socket == NULL) || (in_socket == (SOCKET)(-1)))
	{
		return false;
	}

	//	检查函数指针
	if ((precvfunc == NULL) || (precvfunc == (P_USER_RECV_TCP_SERVER_FUNC)(-1)))
	{
		return false;
	}

	//	创建连接类
	CTCPServerHost* pCTCPServerHost = new CTCPServerHost;
	int re = pCTCPServerHost->Config(
		in_socket,
		precvfunc
		);

	//	检查是否创建成功
	if (SOCKET_ERROR == re)
	{
		//	释放掉申请的连接类
		delete pCTCPServerHost;

		//	返回失败
		return false;
	}

	//	创建结构体
	SConInfo* psconinfo = new SConInfo;

	//	配置结构体
	CMyTime* pnow_time = new CMyTime;
	pnow_time->GetNowTime();
	psconinfo->addrClient = addrClient;
	psconinfo->pCon = pCTCPServerHost;
	psconinfo->socket = in_socket;
	psconinfo->pbegin_time = pnow_time;

	//	加入队列
	mconlist_cs.Enter();
	this->mConList.insert(this->mConList.end(), psconinfo);
	mconlist_cs.Leave();

	//	调用用户接入响应函数
	if (this->pOnJoinFunc != NULL)
	{
		this->pOnJoinFunc(
			in_socket,
			addrClient,
			pnow_time
			);
	}

	//	操作成功
	return true;
}

//	从连接容器里面删除一个对象，成功返回true,失败返回false
/*
	该函数分别被 手动指定断开某个客户端连接 和 发送失败的时候调用
*/
bool CTCPServerManage::RemoveConList(
	SOCKET in_socket					//	删除指定的SOCKET
	)
{
	//	检查输入socket
	if ((in_socket == NULL) || (in_socket == (SOCKET)(-1)))
	{
		return false;
	}

	//	遍历列表寻找socket
	mconlist_cs.Enter();
	int index = -1;
	for (int i = 0; (this->mConList.begin() + i) != (this->mConList.end()); ++i)
	{
		//	如果找到
		if ((*(this->mConList.begin() + i))->socket == in_socket)
		{
			index = i;
			break;
		}
	}

	//	如果找到
	SConInfo* ptmp_dat = NULL;
	if (index != -1)
	{
		//	获取该对象的地址
		ptmp_dat = (*(this->mConList.begin() + index));

		//	从列表中删除
		this->mConList.erase(this->mConList.begin()+index);
	}
	mconlist_cs.Leave();		//	由于对mConList的操作都没有了，所以先离开临界，然后进行资源的释放

	//	如果上述过程没有找到
	if ((index == -1)||(ptmp_dat == NULL))
	{
		return false;
	}

	//	调用用户断开响应函数
	if (this->pOnBreadFunc != NULL)
	{
		CMyTime break_time;
		break_time.GetNowTime();		//	这里是获取的断开时间
		this->pOnBreadFunc(
			ptmp_dat->socket,
			ptmp_dat->addrClient,
			break_time
			);
	}

	//	释放已经从列表中删除的资源
	delete(ptmp_dat->pbegin_time);
	ptmp_dat->pCon->Disconnect();
	delete(ptmp_dat->pCon);
	delete(ptmp_dat);

	//	操作成功
	return true;
}

//	自动从列表中移除错误的连接
/*
	该函数将被 读取数据时产生错误调用
*/
void CTCPServerManage::AutoRemoveError(void)
{
	//	遍历列表中的所有连接，并访问失败标识
	this->mconlist_cs.Enter();
	for (int i = 0; (this->mConList.begin() + i) != (this->mConList.end()); ++i)
	{
		if ((*(this->mConList.begin() + i))->pCon->GetFailStatus() == true)
		{
			//	定义结构体指针
			SConInfo* ptmp_dat = NULL;

			//	获取该对象的地址
			ptmp_dat = (*(this->mConList.begin() + i));

			//	从列表中删除
			this->mConList.erase(this->mConList.begin() + i);
			--i;

			//	调用用户断开响应函数
			if (this->pOnBreadFunc != NULL)
			{
				CMyTime break_time;
				break_time.GetNowTime();		//	这里是获取的断开时间
				this->pOnBreadFunc(
					ptmp_dat->socket,
					ptmp_dat->addrClient,
					break_time
					);
			}

			//	释放资源
			delete(ptmp_dat->pbegin_time);
			ptmp_dat->pCon->Disconnect();
			delete(ptmp_dat->pCon);
			delete(ptmp_dat);
		}
	}
	this->mconlist_cs.Leave();
}

//	初始化函数，成功返回true，失败返回false
bool CTCPServerManage::Config(
	CTCPServerResources& rCTCPServerResources,			//	服务器资源的引用
	P_USER_RECV_TCP_SERVER_FUNC pfunc							//	用户接收函数
	)
{
	//	检查输入函数指针
	if ((pfunc == NULL) || (pfunc == (P_USER_RECV_TCP_SERVER_FUNC)(-1)))
	{
		return false;
	}

	//	检查是否已经初始化过
	if (this->init_flag == true)
	{
		//	停止两个线程
		this->init_flag = false;
		this->m_accept_thread.Stop();
		this->m_find_error_thread.Stop();
		Sleep(10);

		//	遍历循环关掉所有的客户端连接
		this->mconlist_cs.Enter();
		for (int i = 0; (this->mConList.begin() + i) != (this->mConList.end()); ++i)
		{
			//	定义结构体指针
			SConInfo* ptmp_dat = NULL;

			//	获取该对象的地址
			ptmp_dat = (*(this->mConList.begin() + i));

			//	从列表中删除
			this->mConList.erase(this->mConList.begin() + i);
			--i;

			//	释放资源
			delete(ptmp_dat->pbegin_time);
			ptmp_dat->pCon->Disconnect();
			delete(ptmp_dat->pCon);
			delete(ptmp_dat);
		}
		this->mconlist_cs.Leave();
	}

	//	配置本对象的参数
	this->addrServer = rCTCPServerResources.addrServer;
	this->socketServer = rCTCPServerResources.msocket;
	this->pRecvFunc = pfunc;

	//	启动两个线程
	this->m_accept_thread.Config(
		TCP_SERVER_MANAGE_ACCEPT_THREAD,
		this,
		CThread::RUN
		);	
	this->m_find_error_thread.Config(
		TCP_SERVER_MANAGE_FIND_ERROR_THREAD,
		this,
		CThread::RUN
		);

	//	初始化配置成功
	this->init_flag = true;
	//	配置成功
	return true;
}

//	带有连接和断开响应函数的初始化配置函数，成功返回true，失败返回false
bool CTCPServerManage::Config(
	CTCPServerResources& rCTCPServerResources,			//	服务器资源的引用
	P_USER_RECV_TCP_SERVER_FUNC pfunc,							//	用户接收函数	
	P_USER_ON_TCP_CLIENT_JOIN_FUNC ponjoinfunc,			//	定义客户端接入响应函数	
	P_USER_ON_TCP_CLIENT_BREAK_FUNC ponbreadfunc		//	定义客户端断开响应函数
	)
{
	//	检查输入的响应函数
	if ((ponbreadfunc == NULL) || (ponbreadfunc == (P_USER_ON_TCP_CLIENT_BREAK_FUNC)(-1)))
	{
		this->pOnBreadFunc = NULL;
	}
	else
	{
		this->pOnBreadFunc = ponbreadfunc;
	}

	if ((ponjoinfunc == NULL) || (ponjoinfunc == (P_USER_ON_TCP_CLIENT_BREAK_FUNC)(-1)))
	{
		this->pOnJoinFunc = NULL;
	}
	else
	{
		this->pOnJoinFunc = ponjoinfunc;
	}

	//	调用配置函数
	return this->Config(
		rCTCPServerResources,
		pfunc
		);
}

//	返回连接副本
std::vector<SOCKET> CTCPServerManage::GetConListCopy(void)
{
	std::vector<SOCKET> tmp_vec;
	this->mconlist_cs.Enter();
	for (int i = 0; (this->mConList.begin() + i) != (this->mConList.end()); ++i)
	{
		tmp_vec.insert(tmp_vec.end(),(*(this->mConList.begin()+i))->socket);
	}
	this->mconlist_cs.Leave();
	return tmp_vec;
}

//	返回连接副本，包含全部信息的列表
std::vector<CTCPServerManage::SReConInfo> CTCPServerManage::GetConListAllInfoCopy(void)
{
	std::vector<CTCPServerManage::SReConInfo> tmp_vec;
	CTCPServerManage::SReConInfo tmp_s;
	tmp_s.port = 0;
	tmp_s.socket = NULL;
	this->mconlist_cs.Enter();
	for (int i = 0; (this->mConList.begin() + i) != (this->mConList.end()); ++i)
	{
		tmp_s.begin_time = (*(this->mConList.begin() + i))->pbegin_time;
		tmp_s.ip_str = inet_ntoa((*(this->mConList.begin() + i))->addrClient.sin_addr);
		tmp_s.port = htons((*(this->mConList.begin() + i))->addrClient.sin_port);
		tmp_s.socket = (*(this->mConList.begin() + i))->socket;
		tmp_vec.insert(tmp_vec.end(), tmp_s);
		tmp_s.port = 0;
		tmp_s.socket = NULL;
	}
	this->mconlist_cs.Leave();
	return tmp_vec;
}

//	断开连接，true表示操作成功，false表示列表中没有此连接
bool CTCPServerManage::Disconnect(SOCKET in_socket)
{
	return this->RemoveConList(in_socket);
}

//	发送数据，>0表示操作成功，SOCKET_ERROR表示列表中没有此连接,0表示发送失败
int CTCPServerManage::Send(
	SOCKET in_socket,									//	指定的SOCKET句柄
	char* pdata,										//	发送数据的缓冲区首地址
	int length											//	要发送的长度
	)
{
	//	检查输入参数
	if ((in_socket == NULL) || (in_socket == (SOCKET)(-1)) || (pdata == NULL) || (pdata == (char*)(-1)) || (length <= 0))
	{
		return SOCKET_ERROR;
	}

	//	遍历列表寻找socket
	mconlist_cs.Enter();
	int index = -1;
	for (int i = 0; (this->mConList.begin() + i) != (this->mConList.end()); ++i)
	{
		//	如果找到
		if ((*(this->mConList.begin() + i))->socket == in_socket)
		{
			index = i;
			break;
		}
	}

	//	如果找到
	SConInfo* ptmp_dat = NULL;
	if (index != -1)
	{
		//	获取该对象的地址
		ptmp_dat = (*(this->mConList.begin() + index));
	}
	mconlist_cs.Leave();		//	由于对mConList的操作都没有了，所以先离开临界，然后进行资源的释放

	//	如果没有找到
	if ((index == -1) || (ptmp_dat == NULL))
	{
		return SOCKET_ERROR;
	}

	//	发送数据
	int re = ptmp_dat->pCon->Send(pdata, length);

	//	检查是否发送成功，当失败的时候，就已经表示这个连接已经失效了，所以将从列表中移除这个连接
	if (re == SOCKET_ERROR)
	{
		this->RemoveConList(ptmp_dat->socket);
		return SOCKET_ERROR;
	}

	//	运行到这里就成功了
	return re;
}

//	得到IP字符串，错误将返回一个空字符串
char* CTCPServerManage::GetClientIPStr(SOCKET in_socket)
{
	//	清空之前的结果
	memset(ip_str_GetClientIPStr, 0, sizeof(ip_str_GetClientIPStr));

	//	遍历列表寻找socket
	mconlist_cs.Enter();
	int index = -1;
	for (int i = 0; (this->mConList.begin() + i) != (this->mConList.end()); ++i)
	{
		//	如果找到
		if ((*(this->mConList.begin() + i))->socket == in_socket)
		{
			index = i;
			break;
		}
	}

	//	如果找到
	SConInfo* ptmp_dat = NULL;
	if (index != -1)
	{
		//	获取该对象的地址
		ptmp_dat = (*(this->mConList.begin() + index));
	}
	mconlist_cs.Leave();		//	由于对mConList的操作都没有了，所以先离开临界，然后进行资源的释放

	//	如果没有找到
	if ((index == -1) || (ptmp_dat == NULL))
	{
		return ip_str_GetClientIPStr;
	}

	//	获取IP字符串
	strcpy_s(ip_str_GetClientIPStr, inet_ntoa(ptmp_dat->addrClient.sin_addr));

	//	返回字符串
	return ip_str_GetClientIPStr;
}

//	得到端口数值，错误将返回0
u_short CTCPServerManage::GetClientPort(SOCKET in_socket)
{
	//	定义返回端口值
	u_short re_port = 0;

	//	遍历列表寻找socket
	mconlist_cs.Enter();
	int index = -1;
	for (int i = 0; (this->mConList.begin() + i) != (this->mConList.end()); ++i)
	{
		//	如果找到
		if ((*(this->mConList.begin() + i))->socket == in_socket)
		{
			index = i;
			break;
		}
	}

	//	如果找到
	SConInfo* ptmp_dat = NULL;
	if (index != -1)
	{
		//	获取该对象的地址
		ptmp_dat = (*(this->mConList.begin() + index));
	}
	mconlist_cs.Leave();		//	由于对mConList的操作都没有了，所以先离开临界，然后进行资源的释放

	//	如果没有找到
	if ((index == -1) || (ptmp_dat == NULL))
	{
		return re_port;
	}

	//	获得端口
	re_port = htons(ptmp_dat->addrClient.sin_port);

	//	返回端口
	return re_port;
}

//	得到建立的开始时间，错误将返回一个未定义的时间类
CMyTime CTCPServerManage::GetClientBeginTime(SOCKET in_socket)
{
	//	定义返回时间对象
	CMyTime re_time;

	//	遍历列表寻找socket
	mconlist_cs.Enter();
	int index = -1;
	for (int i = 0; (this->mConList.begin() + i) != (this->mConList.end()); ++i)
	{
		//	如果找到
		if ((*(this->mConList.begin() + i))->socket == in_socket)
		{
			index = i;
			break;
		}
	}

	//	如果找到
	SConInfo* ptmp_dat = NULL;
	if (index != -1)
	{
		//	获取该对象的地址
		ptmp_dat = (*(this->mConList.begin() + index));
	}
	mconlist_cs.Leave();		//	由于对mConList的操作都没有了，所以先离开临界，然后进行资源的释放

	//	如果没有找到
	if ((index == -1) || (ptmp_dat == NULL))
	{
		return re_time;
	}

	//	获得端口
	re_time = ptmp_dat->pbegin_time;

	//	返回端口
	return re_time;
}

//	得到服务器的IP字符串
char* CTCPServerManage::GetServerIPString(void)
{
	//	清空之前的结果
	memset(ip_str_GetServerIPString, 0, sizeof(ip_str_GetServerIPString));

	//	如果没有配置
	if (this->init_flag == false)
	{
		return ip_str_GetServerIPString;
	}

	//	获得服务器IP
	strcpy_s(ip_str_GetServerIPString, inet_ntoa(this->addrServer.sin_addr));

	//	返回ip字符串
	return ip_str_GetServerIPString;
}

//	得到服务器端口
int CTCPServerManage::GetServerPort(void)
{
	//	定义返回端口
	int re_port = -1;

	//	如果没有配置
	if (this->init_flag == false)
	{
		return re_port;
	}

	//	获得服务器端口
	re_port = htons(this->addrServer.sin_port);

	//	返回端口
	return re_port;
}

//	获取服务器资源SOCKET，没有配置的时候返回空
SOCKET CTCPServerManage::GetServerSOCKET(void)
{
	//	定义返回SOCKET
	SOCKET re_socket = NULL;

	//	如果没有配置
	if (this->init_flag == false)
	{
		return re_socket;
	}

	//	获得服务器的SOCKET
	re_socket = this->socketServer;

	//	返回SOCKET
	return re_socket;
}

//////////////////////////////////////////////////////////////////////////////////////////
//	TCP客户端类

//	接收处理线程函数
DWORD TCP_CLIENT_RECV_PROC_THREAD(LPVOID pParam)
{
	//	获取参数
	CTCPClient* pc = (CTCPClient*)(pParam);

	//	定义返回值
	int re = 0;

	//	初始化已经断线重连的次数
	int already_reconnect_count = 0;
	
	//	进入循环
	while (1)
	{
		//	尝试读取数据
		re = recv(pc->sockClient, pc->recv_buff, sizeof(pc->recv_buff), 0);

		//	检查读取情况，如果出现问题
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			//	统计失败次数
			++already_reconnect_count;

			//	调用用户指定的断开响应函数
			if (pc->pOnBreakFunc != NULL)
			{
				pc->pOnBreakFunc(
					pc->sockClient,
					pc->addrServer,
					already_reconnect_count
					);
			}

			//	检查断线重连次数
			//	如果断线后不进行重新连接
			if (pc->RetryCount == 0)
			{
				//	得到socket副本
				SOCKET tmp_socket = pc->sockClient;
				pc->sockClient = NULL;													//	告诉外部，连接已经关闭
				if (tmp_socket != NULL) closesocket(tmp_socket);						//	关闭当前连接
				return 0;																//	线程退出
			}
			//	表示断线后无限重试次数
			else if(pc->RetryCount == -1)
			{
				//	得到socket副本
				SOCKET tmp_socket = pc->sockClient;
				pc->sockClient = NULL;													//	告诉外部，连接已经关闭
				if (tmp_socket != NULL) closesocket(tmp_socket);						//	关闭当前连接

				//	等待延时时间
				Sleep(pc->RetrySec * 1000);

				//	重新进行连接
				//	创建SOCKET
				tmp_socket = socket(AF_INET, SOCK_STREAM, 0);

				//	连接
				re = connect(tmp_socket, (SOCKADDR *)&(pc->addrServer), sizeof(pc->addrServer));

				//	如果连接成功
				if (re != SOCKET_ERROR)
				{
					//	配置连接标志
					pc->sockClient = tmp_socket;
				}
				//	如果连接失败
				else
				{
					closesocket(tmp_socket);					//	关闭当前连接
				}

			}
			//	指定断线后的重试次数
			else
			{
				//	如果失败次数过多
				if (already_reconnect_count > pc->RetryCount)
				{
					//	得到socket副本
					SOCKET tmp_socket = pc->sockClient;
					pc->sockClient = NULL;													//	告诉外部，连接已经关闭
					if (tmp_socket != NULL) closesocket(tmp_socket);						//	关闭当前连接
					return 0;																//	线程退出
				}

				//	得到socket副本
				SOCKET tmp_socket = pc->sockClient;
				pc->sockClient = NULL;													//	告诉外部，连接已经关闭
				if (tmp_socket != NULL) closesocket(tmp_socket);						//	关闭当前连接

				//	等待延时时间
				Sleep(pc->RetrySec * 1000);

				//	重新进行连接
				//	创建SOCKET
				tmp_socket = socket(AF_INET, SOCK_STREAM, 0);

				//	连接
				re = connect(tmp_socket, (SOCKADDR *)&(pc->addrServer), sizeof(pc->addrServer));

				//	如果连接成功
				if (re != SOCKET_ERROR)
				{
					//	配置连接标志
					pc->sockClient = tmp_socket;
				}
				//	如果连接失败
				else
				{
					closesocket(tmp_socket);					//	关闭当前连接
				}
			}
		}
		//	否则为正常读取
		else
		{
			//	调用用户函数
			pc->pRecvFunc(
				pc->recv_buff,
				re
				);
		}
	}

	return 0;
}

//	无参数构造函数
CTCPClient::CTCPClient()
{
	this->init_flag = false;								//	没有进行配置
	memset(this->ip_str, 0, sizeof(this->ip_str));			//	清除IP字符串返回缓冲区
	this->pRecvFunc = NULL;									//	用户接收处理函数为空
	this->RetryCount = 0;									//	默认为无限重试次数
	this->RetrySec = -1;									//	默认为断线后立即进行连接
	this->sockClient = NULL;								//	SOCKET连接为空
	pOnBreakFunc = NULL;									//	断开响应函数为空
}

//	析构函数
CTCPClient::~CTCPClient()
{
	//	如果已经初始化
	if (this->init_flag == true)
	{
		this->init_flag = false;
		this->m_recv_thread.Stop();		//	终止线程
		Sleep(10);

		//	此时如果为已经连接的话
		if (this->sockClient != NULL)
		{
			//	释放SOCKET
			if (this->sockClient != NULL) closesocket(this->sockClient);
			this->sockClient = NULL;
			WSACleanup();
		}
	}
}

//	配置函数，默认配置为 断线后立即重试连接，并且无限重试次数，成功返回0，失败返回SOCKET_ERROR
int CTCPClient::Config(
	char* remote_ip,										//	远程主机的IP地址
	u_short remote_port,									//	远程主机的端口
	P_USER_RECV_TCP_CLIENT_FUNC precvfunc,					//	用户定义的接收函数
	bool now_conn,											//	是否立即进行连接，true为配置成功后立即连接，false为仅仅配置，不进行连接
	P_USER_ON_TCP_SERVER_BREAK_FUNC ponbreakfunc,			//	当与服务器断开连接的时候，所调用的响应函数
	int retry_sec,											//	断线重连的超时秒数，0表示断线后立即进行连接，>0表示断线后经过多久后重试建立连接
	int retry_count											//	重试次数，-1表示无限重试次数，0表示断线后不进行重试连接，>0表示断线后重试的次数
	)
{
	//	参数检查
	if ((precvfunc == NULL) || (precvfunc == (P_USER_RECV_TCP_CLIENT_FUNC)(-1)) || (retry_sec < 0) || (retry_count < -1))
	{
		return SOCKET_ERROR;
	}

	//	如果已经有配置
	if (this->init_flag == true)
	{
		//	为了防止断线重连机制导致的断开假象，所以这里再次执行终止线程
		this->m_recv_thread.Stop();
		Sleep(10);

		//	如果已经连接，则断开
		if (this->sockClient != NULL)
		{
			this->Disconnect();
		}
	}
	else
	{
		//	初始化套接字DLL
		if (WSAStartup(MAKEWORD(2, 1), &mwsaData)) //调用Windows Sockets DLL
		{
			WSACleanup();
			abort();			//	直接终止当前程序
		}
	}

	//	配置断开响应函数
	if ((ponbreakfunc == NULL) || (ponbreakfunc == (P_USER_ON_TCP_SERVER_BREAK_FUNC)(-1)))
	{
		this->pOnBreakFunc = NULL;
	}
	else
	{
		this->pOnBreakFunc = ponbreakfunc;
	}

	//	socket清空
	this->sockClient = NULL;

	//	配置用户定义接收函数
	this->pRecvFunc = precvfunc;

	//	配置超时秒数
	this->RetrySec = retry_sec;

	//	配置重试次数
	this->RetryCount = retry_count;
	
	//	配置服务器地址信息
	addrServer.sin_addr.S_un.S_addr = inet_addr(remote_ip);
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(remote_port);

	//	信息已经配置
	this->init_flag = true;

	//	检查服务器是否进行连接
	if (now_conn == true)
	{
		return this->Connect();
	}

	return 0;
}

//	连接函数,If no error occurs, connect returns zero. Otherwise, it returns SOCKET_ERROR, and a specific error code can be retrieved by calling WSAGetLastError.
int CTCPClient::Connect(void)
{
	//	检查是否已经进行配置
	if (this->init_flag != true)
	{
		return SOCKET_ERROR;
	}
	
	//	检查是否已经连接
	if (this->sockClient != NULL)
	{
		return SOCKET_ERROR;
	}

	//	检查线程是否已经终止
	if (this->GetThreadStatus() != CThread::STOP)
	{
		return SOCKET_ERROR;
	}

	//	创建SOCKET
	sockClient = socket(AF_INET, SOCK_STREAM, 0);

	//	连接
	int re = connect(sockClient, (SOCKADDR *)&addrServer, sizeof(addrServer));

	//	检查连接是否成功
	if (re == SOCKET_ERROR)
	{
		//	关闭SOCKET
		closesocket(this->sockClient);
		this->sockClient = NULL;
		return SOCKET_ERROR;
	}

	//	启动接收处理线程
	this->m_recv_thread.Config(
		TCP_CLIENT_RECV_PROC_THREAD,
		this,
		CThread::RUN
		);

	//	连接成功
	return 0;
}

//	断开函数
void CTCPClient::Disconnect(void)
{
	//	检查是否没有配置
	if (this->init_flag != true)
	{
		return;
	}
	
	//	终止线程
	this->m_recv_thread.Stop();		
	Sleep(10);

	//	释放SOCKET
	if (this->sockClient != NULL) closesocket(this->sockClient);
	this->sockClient = NULL;
}

//	返回连接状态，true表示已经建立连接，false表示没有建立连接
bool CTCPClient::GetConnectStatus(void)
{
	if (this->sockClient == NULL)	return false;
	else							return true;
}

//	获得配置状态，true表示已经成功配置，false表示没有配置
bool CTCPClient::GetConfigStatus(void)
{
	return this->init_flag;
}

//	得到服务器端口
int CTCPClient::GetServerPort(void)
{
	//	定义返回端口
	int re_port = -1;

	//	如果没有配置
	if (this->init_flag == false)
	{
		return re_port;
	}

	//	获得服务器端口
	re_port = htons(this->addrServer.sin_port);

	//	返回端口
	return re_port;
}

//	得到服务器IP
char* CTCPClient::GetServerIPStr(void)
{
	//	清空之前的结果
	memset(this->ip_str, 0, sizeof(this->ip_str));

	//	如果没有配置
	if (this->init_flag == false)
	{
		return this->ip_str;
	}

	//	获得服务器IP
	strcpy_s(this->ip_str, inet_ntoa(this->addrServer.sin_addr));

	//	返回ip字符串
	return this->ip_str;
}

//	发送数据，>0表示操作成功，SOCKET_ERROR表示列表中没有此连接,0表示发送失败，该发送程序在重试连接的时候也会返回错误
int CTCPClient::TrySend(
	char* pdata,										//	发送数据的缓冲区首地址
	int length											//	要发送的长度
	)
{
	//	检查发送参数
	if ((pdata == NULL)||(pdata == (char*)(-1))||(length <= 0))
	{
		return SOCKET_ERROR;
	}

	//	检查是否没有配置
	if (this->init_flag != true)
	{
		return SOCKET_ERROR;
	}

	//	检查是否为没有连接
	if (this->sockClient == NULL)
	{
		return SOCKET_ERROR;
	}

	//	定义返回变量
	int re = 0;

	//	调用发送
	re = ::send(
		this->sockClient,
		pdata,
		length,
		0);

	//	即使此时运行出错，在线程内部的读取函数也会出错，所以这里仅仅返回失败即可，至于断线重连机制由线程进行控制
	return re;
}

//	得到重试的线程状态，在连接断线的时候，如果线程还依然在运行，那么说明当前处于断线重连的时候
CThread::EThreadState CTCPClient::GetThreadStatus(void)
{
	//	定义返回状态
	CThread::EThreadState re;

	//	获取线程状态
	re = this->m_recv_thread.GetState();

	//	返回状态
	return re;
}

//	获得SOCKET
SOCKET CTCPClient::GetSocket(void)
{
	//	定义返回变量
	SOCKET tmp_socket;

	//	获取socket
	tmp_socket = this->sockClient;

	//	返回
	return tmp_socket;
}

//	获得重试次数
int CTCPClient::GetRetryCount(void)
{
	return this->RetryCount;
}

//	获得断开时的超时秒数
int CTCPClient::GetRetrySec(void)
{
	return this->RetrySec;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//	采用容器方式读取数据的TCP客户端类

//	接收处理线程函数
DWORD TCP_CLIENT_VEC_RECV_PROC_THREAD(LPVOID pParam)
{
	//	获取参数
	CTCPClientVecRecv* pc = (CTCPClientVecRecv*)(pParam);

	//	定义返回值
	int re = 0;

	//	初始化已经断线重连的次数
	int already_reconnect_count = 0;

	//	进入循环
	while (1)
	{
		//	尝试读取数据
		re = ::recv(pc->sockClient, pc->recv_buff, sizeof(pc->recv_buff), 0);

		//	检查读取情况，如果出现问题
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			//	统计失败次数
			++already_reconnect_count;

			//	调用用户指定的断开响应函数
			if (pc->pOnBreakFunc != NULL)
			{
				pc->pOnBreakFunc(
					pc->sockClient,
					pc->addrServer,
					already_reconnect_count
					);
			}

			//	检查断线重连次数
			//	如果断线后不进行重新连接
			if (pc->RetryCount == 0)
			{
				//	得到socket副本
				SOCKET tmp_socket = pc->sockClient;
				pc->sockClient = NULL;													//	告诉外部，连接已经关闭
				if (tmp_socket != NULL) closesocket(tmp_socket);						//	关闭当前连接
				return 0;																//	线程退出
			}
			//	表示断线后无限重试次数
			else if (pc->RetryCount == -1)
			{
				//	得到socket副本
				SOCKET tmp_socket = pc->sockClient;
				pc->sockClient = NULL;													//	告诉外部，连接已经关闭
				if (tmp_socket != NULL) closesocket(tmp_socket);						//	关闭当前连接

				//	等待延时时间
				Sleep(pc->RetrySec * 1000);

				//	重新进行连接
				//	创建SOCKET
				tmp_socket = socket(AF_INET, SOCK_STREAM, 0);

				//	连接
				re = connect(tmp_socket, (SOCKADDR *)&(pc->addrServer), sizeof(pc->addrServer));

				//	如果连接成功
				if (re != SOCKET_ERROR)
				{
					//	配置连接标志
					pc->sockClient = tmp_socket;
				}
				//	如果连接失败
				else
				{
					closesocket(tmp_socket);					//	关闭当前连接
				}

			}
			//	指定断线后的重试次数
			else
			{
				//	如果失败次数过多
				if (already_reconnect_count > pc->RetryCount)
				{
					//	得到socket副本
					SOCKET tmp_socket = pc->sockClient;
					pc->sockClient = NULL;													//	告诉外部，连接已经关闭
					if (tmp_socket != NULL) closesocket(tmp_socket);						//	关闭当前连接
					return 0;																//	线程退出
				}

				//	得到socket副本
				SOCKET tmp_socket = pc->sockClient;
				pc->sockClient = NULL;													//	告诉外部，连接已经关闭
				if (tmp_socket != NULL) closesocket(tmp_socket);						//	关闭当前连接

				//	等待延时时间
				Sleep(pc->RetrySec * 1000);

				//	重新进行连接
				//	创建SOCKET
				tmp_socket = socket(AF_INET, SOCK_STREAM, 0);

				//	连接
				re = connect(tmp_socket, (SOCKADDR *)&(pc->addrServer), sizeof(pc->addrServer));

				//	如果连接成功
				if (re != SOCKET_ERROR)
				{
					//	配置连接标志
					pc->sockClient = tmp_socket;
				}
				//	如果连接失败
				else
				{
					closesocket(tmp_socket);					//	关闭当前连接
				}
			}
		}
		//	否则为正常读取
		else
		{
			//	将数据添加到VEC中
			pc->AddByteToBufferVec(
				pc->recv_buff,
				sizeof(pc->recv_buff)
				);
		}
	}

	return 0;
}

//	配置函数，默认配置为 断线后立即重试连接，并且无限重试次数，成功返回0，失败返回SOCKET_ERROR
int CTCPClientVecRecv::Config(
	char* remote_ip,										//	远程主机的IP地址
	u_short remote_port,									//	远程主机的端口
	bool now_conn,											//	是否立即进行连接，true为配置成功后立即连接，false为仅仅配置，不进行连接
	P_USER_ON_TCP_SERVER_BREAK_FUNC ponbreakfunc,			//	当与服务器断开连接的时候，所调用的响应函数
	int retry_sec,											//	断线重连的超时秒数，0表示断线后立即进行连接，>0表示断线后经过多久后重试建立连接
	int retry_count											//	重试次数，-1表示无限重试次数，0表示断线后不进行重试连接，>0表示断线后重试的次数
	)
{
	//	参数检查
	if ((retry_sec < 0) || (retry_count < -1))
	{
		return SOCKET_ERROR;
	}

	//	如果已经有配置
	if (this->init_flag == true)
	{
		//	为了防止断线重连机制导致的断开假象，所以这里再次执行终止线程
		this->m_recv_thread.Stop();
		Sleep(10);

		//	如果已经连接，则断开
		if (this->sockClient != NULL)
		{
			this->Disconnect();
		}
	}
	else
	{
		//	初始化套接字DLL
		if (WSAStartup(MAKEWORD(2, 1), &mwsaData)) //调用Windows Sockets DLL
		{
			WSACleanup();
			abort();			//	直接终止当前程序
		}
	}

	//	配置断开响应函数
	if ((ponbreakfunc == NULL) || (ponbreakfunc == (P_USER_ON_TCP_SERVER_BREAK_FUNC)(-1)))
	{
		this->pOnBreakFunc = NULL;
	}
	else
	{
		this->pOnBreakFunc = ponbreakfunc;
	}

	//	socket清空
	this->sockClient = NULL;

	//	配置超时秒数
	this->RetrySec = retry_sec;

	//	配置重试次数
	this->RetryCount = retry_count;

	//	配置服务器地址信息
	addrServer.sin_addr.S_un.S_addr = inet_addr(remote_ip);
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(remote_port);

	//	信息已经配置
	this->init_flag = true;

	//	检查服务器是否进行连接
	if (now_conn == true)
	{
		return this->Connect();
	}

	return 0;
}

//	连接函数,If no error occurs, connect returns zero. Otherwise, it returns SOCKET_ERROR, and a specific error code can be retrieved by calling WSAGetLastError.
int CTCPClientVecRecv::Connect(void)
{
	//	检查是否已经进行配置
	if (this->init_flag != true)
	{
		return SOCKET_ERROR;
	}

	//	检查是否已经连接
	if (this->sockClient != NULL)
	{
		return SOCKET_ERROR;
	}

	//	检查线程是否已经终止
	if (this->GetThreadStatus() != CThread::STOP)
	{
		return SOCKET_ERROR;
	}

	//	创建SOCKET
	sockClient = socket(AF_INET, SOCK_STREAM, 0);

	//	连接
	int re = connect(sockClient, (SOCKADDR *)&addrServer, sizeof(addrServer));

	//	检查连接是否成功
	if (re == SOCKET_ERROR)
	{
		//	关闭SOCKET
		closesocket(this->sockClient);
		this->sockClient = NULL;
		return SOCKET_ERROR;
	}

	//	启动接收处理线程
	this->m_recv_thread.Config(
		TCP_CLIENT_VEC_RECV_PROC_THREAD,
		this,
		CThread::RUN
		);

	//	连接成功
	return 0;
}

//	无参数构造函数
CTCPClientVecRecv::CTCPClientVecRecv()
{

}

//	析构函数
CTCPClientVecRecv::~CTCPClientVecRecv()
{

}

//	获得已经接收到的容器中有多少的字节
int CTCPClientVecRecv::GetBufferBytesCount(void)
{
	//	定义返回对象
	int tmp_count = 0;

	//	获取字节数目
	this->recvbuffvec_cs.Enter();
	tmp_count = this->RecvBuffVec.size();
	this->recvbuffvec_cs.Leave();

	//	返回
	return tmp_count;
}

//	读取n个字节，非阻塞
std::vector<unsigned char> CTCPClientVecRecv::TryRecv(int n)
{
	//	构造返回对象
	std::vector<unsigned char> tmp_vec;

	//	定义当前缓冲区内的字节数目
	int tmp_count = 0;

	//	检查输入参数
	if (n <= 0)
	{
		return tmp_vec;
	}

	//	获取字节数目
	this->recvbuffvec_cs.Enter(); 
	tmp_count = this->RecvBuffVec.size();

	//	如果为空，不操作，后面会返回空容器
	if (tmp_count == 0)
	{

	}
	//	如果容器内字节数目足够
	else if (tmp_count >= n)
	{
		for (int i = 0; i < n; ++i)
		{
			tmp_vec.insert(tmp_vec.end(), (*(this->RecvBuffVec.begin() + i)));
		}
	}
	//	容器内字节数目不够
	else
	{
		for (int i = 0; i < tmp_count; ++i)
		{
			tmp_vec.insert(tmp_vec.end(), (*(this->RecvBuffVec.begin() + i)));
		}
	}
	this->recvbuffvec_cs.Leave();


	//	返回
	return tmp_vec;
}

//	读取n个字节，阻塞读取
std::vector<unsigned char> CTCPClientVecRecv::WhileRecv(int n)
{
	//	构造返回对象
	std::vector<unsigned char> tmp_vec;

	//	定义当前缓冲区内的字节数目
	int tmp_count = 0;

	//	检查输入参数
	if (n <= 0)
	{
		return tmp_vec;
	}

	//	定义字节数目是否足够的标志，true表示足够，false表示不够
	bool bytes_enough = false;

	//	阻塞式读取
	do
	{
		//	获取字节数目
		this->recvbuffvec_cs.Enter();
		tmp_count = this->RecvBuffVec.size();

		//	如果为空，字节不够
		if (tmp_count == 0)
		{
			bytes_enough = false;
		}
		//	如果容器内字节数目足够
		else if (tmp_count >= n)
		{
			for (int i = 0; i < n; ++i)
			{
				tmp_vec.insert(tmp_vec.end(), (*(this->RecvBuffVec.begin() + i)));
			}
			bytes_enough = true;
		}
		//	容器内字节数目不够
		else
		{
			bytes_enough = false;
		}
		this->recvbuffvec_cs.Leave();

		//	退出临界后检查是否需要延时，当字节数目不够的时候，本线程延时
		if (bytes_enough == false) Sleep(1);
	} while (bytes_enough == false);			//	只要不够就一直循环读取


	//	返回
	return tmp_vec;
}

//	清空缓冲区
void CTCPClientVecRecv::ClearBuffer(void)
{
	this->recvbuffvec_cs.Enter();
	this->RecvBuffVec.clear();
	this->recvbuffvec_cs.Leave();
}

//	向缓冲区内追加数据
void CTCPClientVecRecv::AddByteToBufferVec(
	char* pbuff,				//	要加入数据的首地址
	int length					//	要追加数据的长度
	)
{
	//	检查输入参数是否合法
	if ((pbuff == NULL) || (pbuff == (char*)(-1)) || (length <= 0))
	{
		return;
	}

	//	循环插入数据
	this->recvbuffvec_cs.Enter();
	for (int i = 0; i < length; ++i)
	{
		this->RecvBuffVec.insert(this->RecvBuffVec.end(), *(pbuff + i));
	}
	this->recvbuffvec_cs.Leave();
}
