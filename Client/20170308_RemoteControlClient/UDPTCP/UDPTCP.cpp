
#include "stdafx.h"
#include "UDPTCP.h"

//	����汾 1.0

//////////////////////////////////////////////////////////////////////////
//	���ȫ���ı���IP
std::vector<CString> GetAllLoaclIP(void)
{
	std::vector<CString> LocalAllIP;
	CString str;

	//	��ʼ���첽�׽���
	WSADATA wsaData;
	char szHostName[512];
	WSAStartup( MAKEWORD(2,1), &wsaData );

	struct hostent * pHost;			//	����������Ϣ�ṹ�����
	int i;							//	����ѭ������
	char ip_str[50];
	gethostname(szHostName, sizeof(szHostName));
	pHost = gethostbyname(szHostName);			//	�������������������Ϣ
	//	ѭ���õ����õ�����IP
	for( i = 0; pHost!= NULL && pHost->h_addr_list[i]!= NULL; i++ )  
	{
		strcpy_s(ip_str,inet_ntoa (*(struct in_addr *)pHost->h_addr_list[i]));	//	��IP��ַת��Ϊ�ַ���
#ifdef UNICODE
		str.Format(L"%s",ip_str);
#else
		str.Format("%s",ip_str);
#endif
		LocalAllIP.insert(LocalAllIP.begin(),str);								//	��������
	}  

	WSACleanup();
	return LocalAllIP;
}

//	��ȡ���������
CString GetLocalHostName(void)
{
	CString str;
	char szHostName[512];

	//	��ʼ���첽�׽���
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
//	UDP����������պ�������
DWORD UDP_SERVER_THREAD_FUNC(LPVOID pParam)
{
	//	ǿ��ת������ò���ָ��
	CUDPServer* pCUDPServer = (CUDPServer*)pParam;
	SOCKET socket1;
	SOCKADDR_IN remote;
	int fromlen =sizeof(remote);
	int re;

	//	������ѭ��
	while (1)
	{
		//	���û�г�ʼ������һֱ����
		if (pCUDPServer->init_flag == false)
		{
			Sleep(10);
			continue;
		}

		//	��ȡ�׽���ʵ��
		socket1 = pCUDPServer->msocket;

		//	��ʼ����
		if ((re = recvfrom(					//	���������ȷ��recvfrom�����ؽ��յ����ֽ���Ŀ
			socket1,
			pCUDPServer->recv_buffer,
			MAX_BUFFER_SIZE,
			0,
			(struct sockaddr*)&remote,
			&fromlen
			))!=SOCKET_ERROR)
		{
			//	ִ���û�������
			pCUDPServer->pUserRecvFunc(
				remote,							//	Զ��������Ϣ
				pCUDPServer->recv_buffer,		//	�������׵�ַ
				re								//	�յ����ֽ���Ŀ
				);
		}
	}

	return 0;
}

//	�ṩ�˿ں�IP
CUDPServer::CUDPServer(
	P_USER_RECV_FUNC pFunc,			//	�û��Զ���Ľ��մ�����
	u_short		port,				//	�󶨱����Ķ˿�
	const char*	ip_str				//	�󶨱�����IP
	):
mCThread(
	UDP_SERVER_THREAD_FUNC,
	this,
	CThread::STOP					//	���ڹ���ʱ����ʼ���׽�����Ҫʱ�䣬���Բ������������߳���
	)
{
	//	����ַ���������
	memset(this->ip_str_for_getipstr, 0, sizeof(this->ip_str_for_getipstr));
	memset(this->ip_str_for_GetIPStrFromCIPAddressCtrl,0,sizeof(this->ip_str_for_GetIPStrFromCIPAddressCtrl));

	//	��ʼ���׽���DLL
	if (WSAStartup(MAKEWORD(2,1),&mwsaData)) //����Windows Sockets DLL
	{
		WSACleanup();
		abort();			//	ֱ����ֹ��ǰ����
	}

	//	������Ϣ�Ͱ�
	localaddr.sin_family=AF_INET;
	localaddr.sin_port=htons(port);						//	�����˿�
	localaddr.sin_addr.s_addr=inet_addr(ip_str);		//	ָ��IP
	msocket=socket(AF_INET,SOCK_DGRAM,0);
	bind(msocket,(struct sockaddr*)&localaddr,sizeof(localaddr));

	//	�����û�������
	pUserRecvFunc = pFunc;

	//	���������߳�
	mCThread.Run();

	//	�Ѿ���ʼ��
	init_flag = true;
}

//	�ṩ�˿ڣ�IPΪINADDR_ANY����������IP
CUDPServer::CUDPServer(
	P_USER_RECV_FUNC pFunc,			//	�û��Զ���Ľ��մ�����
	u_short	port
	):
mCThread(
	UDP_SERVER_THREAD_FUNC,
	this,
	CThread::STOP					//	���ڹ���ʱ����ʼ���׽�����Ҫʱ�䣬���Բ������������߳���
	)
{
	//	����ַ���������
	memset(this->ip_str_for_getipstr, 0, sizeof(this->ip_str_for_getipstr));
	memset(this->ip_str_for_GetIPStrFromCIPAddressCtrl, 0, sizeof(this->ip_str_for_GetIPStrFromCIPAddressCtrl));

	//	��ʼ���׽���DLL
	if (WSAStartup(MAKEWORD(2,1),&mwsaData)) //����Windows Sockets DLL
	{
		WSACleanup();
		abort();			//	ֱ����ֹ��ǰ����
	}

	//	������Ϣ�Ͱ�
	localaddr.sin_family=AF_INET;
	localaddr.sin_port=htons(port);						//	�����˿�
	localaddr.sin_addr.s_addr=INADDR_ANY;				//	��������IP
	msocket=socket(AF_INET,SOCK_DGRAM,0);
	bind(msocket,(struct sockaddr*)&localaddr,sizeof(localaddr));

	//	�����û�������
	pUserRecvFunc = pFunc;

	//	���������߳�
	mCThread.Run();

	//	�Ѿ���ʼ��
	init_flag = true;
}

//	�κβ��������ṩ
CUDPServer::CUDPServer():
	mCThread(
	UDP_SERVER_THREAD_FUNC,
	this,
	CThread::STOP					//	���ڹ���ʱ����ʼ���׽�����Ҫʱ�䣬���Բ������������߳���
	)
{
	//	����ַ���������
	memset(this->ip_str_for_getipstr, 0, sizeof(this->ip_str_for_getipstr));
	memset(this->ip_str_for_GetIPStrFromCIPAddressCtrl, 0, sizeof(this->ip_str_for_GetIPStrFromCIPAddressCtrl));

	//	δ��ʼ��
	init_flag = false;
}

//	��������
int CUDPServer::Send(
	const char* remote_ip,			//	Ҫ���͵���Ŀ��IP
	u_short remote_port,			//	Ҫ���͵���Ŀ��˿�
	char* pBuffer,					//	�������ݵ��׵�ַ
	int length						//	���ͳ��ȣ��ֽڵ�λ��
	)
{
	//	���û�г�ʼ������ֱ�ӷ��ش���
	if (init_flag == false)
	{
		return SOCKET_ERROR;
	}

	//	����Զ��������Ϣ
	SOCKADDR_IN remote;
	remote.sin_family=AF_INET;
	remote.sin_port=htons(remote_port);						//	ָ���˿�
	remote.sin_addr.s_addr=inet_addr(remote_ip);;			//	ָ��IP
	
	//	��ȡ�׽���ʵ��
	SOCKET socket1 = msocket;

	//	����fromlen
	int fromlen =sizeof(remote);

	//	ִ�з���
	int re = sendto(
		socket1,
		pBuffer,
		length,
		0,
		(struct sockaddr*)&remote,
		fromlen
		);

	//	����״̬
	return re;
}

//	�ṩ�˿ں�IP
void CUDPServer::Config(
	P_USER_RECV_FUNC pFunc,			//	�û��Զ���Ľ��մ�����
	u_short		port,				//	�󶨱����Ķ˿�
	const char*	ip_str				//	�󶨱�����IP
	)
{
	//	����Ѿ���ʼ������Ҫ����֮ǰ����Ϣ
	if (init_flag == true)
	{
		//	ֹͣ�߳�
		mCThread.Stop();
		Sleep(100);

		//	�ر��׽���
		closesocket(msocket);
		WSACleanup();
	}

	//	��ʼ���׽���DLL
	if (WSAStartup(MAKEWORD(2,1),&mwsaData)) //����Windows Sockets DLL
	{
		WSACleanup();
		abort();			//	ֱ����ֹ��ǰ����
	}

	//	������Ϣ�Ͱ�
	localaddr.sin_family=AF_INET;
	localaddr.sin_port=htons(port);						//	�����˿�
	localaddr.sin_addr.s_addr=inet_addr(ip_str);		//	ָ��IP
	msocket=socket(AF_INET,SOCK_DGRAM,0);
	bind(msocket,(struct sockaddr*)&localaddr,sizeof(localaddr));

	//	�����û�������
	pUserRecvFunc = pFunc;

	//	���������߳�
	mCThread.Run();

	//	�Ѿ���ʼ��
	init_flag = true;
}

//	�ṩ�˿ڣ�IPΪINADDR_ANY����������IP
void CUDPServer::Config(
	P_USER_RECV_FUNC pFunc,			//	�û��Զ���Ľ��մ�����
	u_short	port
	)
{
	//	����Ѿ���ʼ������Ҫ����֮ǰ����Ϣ
	if (init_flag == true)
	{
		//	ֹͣ�߳�
		mCThread.Stop();
		Sleep(100);

		//	�ر��׽���
		closesocket(msocket);
		WSACleanup();

	}

	//	��ʼ���׽���DLL
	if (WSAStartup(MAKEWORD(2,1),&mwsaData)) //����Windows Sockets DLL
	{
		WSACleanup();
		abort();			//	ֱ����ֹ��ǰ����
	}
	
	//	������Ϣ�Ͱ�
	localaddr.sin_family=AF_INET;
	localaddr.sin_port=htons(port);						//	�����˿�
	localaddr.sin_addr.s_addr=INADDR_ANY;				//	��������IP
	msocket=socket(AF_INET,SOCK_DGRAM,0);
	bind(msocket,(struct sockaddr*)&localaddr,sizeof(localaddr));

	//	�����û�������
	pUserRecvFunc = pFunc;

	//	���������߳�
	mCThread.Run();

	//	�Ѿ���ʼ��
	init_flag = true;
}

//	�ر�����
void CUDPServer::Close(void)
{
	//	����Ѿ�����
	if (init_flag == true)
	{
		//	�ر��߳�
		mCThread.Stop();
		Sleep(100);

		//	�ر��׽���
		closesocket(msocket);
		WSACleanup();
		init_flag = false;
	}
}

//	��������
CUDPServer::~CUDPServer()
{
	//	����Ѿ�����
	if (init_flag == true)
	{
		//	�ر��׽���
		closesocket(msocket);
		WSACleanup();
	}
}


//	�õ�IP�ַ���
char* CUDPServer::GetIPStr(SOCKADDR_IN in)
{
	memset(ip_str_for_getipstr, 0, sizeof(ip_str_for_getipstr));
	mGetIPCMCS.Enter();
	strcpy_s(ip_str_for_getipstr, inet_ntoa(in.sin_addr));
	mGetIPCMCS.Leave();
	return ip_str_for_getipstr;
}

//	�õ��˿���ֵ
u_short CUDPServer::GetIPPort(SOCKADDR_IN in)
{
	u_short port;
	mGetPortCMCS.Enter();
	port = htons(in.sin_port);
	mGetPortCMCS.Leave();
	return port;
}

//	ͨ��IP��ַ�ؼ��õ�IP��ַ�ַ���
char* CUDPServer::GetIPStrFromCIPAddressCtrl(CIPAddressCtrl* pCIPAddressCtrl)
{
	//	���Ŀ��IP�ַ���
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

//	���ṩ�κγ�ʼ����Ϣ
CTCPServerResources::CTCPServerResources()
{
	//	����ַ���������
	memset(this->ip_str,0,sizeof(this->ip_str));

	//	����Ϊδ��ʼ��
	init_flag = false;

	//	�趨δ��ʼ���ķ�������ԴΪ��
	msocket = NULL;
}

//	�ͷ���Դ
void CTCPServerResources::Release(void)
{
	//	����ٽ��Ƿ񶼳ɹ��Ƴ�
	if (this->get_configstatus_cs.InCriticalSection() == true)	this->get_configstatus_cs.Leave();
	if (this->get_socket_cs.InCriticalSection() == true)	this->get_socket_cs.Leave();

	//	����Ѿ�����
	if (init_flag == true)
	{
		//	�ر��׽���
		init_flag = false;
		closesocket(msocket);
		WSACleanup();
	}
}

//	��������
CTCPServerResources::~CTCPServerResources()
{
	//	����ٽ��Ƿ񶼳ɹ��Ƴ�
	if (this->get_configstatus_cs.InCriticalSection() == true)	this->get_configstatus_cs.Leave();
	if (this->get_socket_cs.InCriticalSection() == true)	this->get_socket_cs.Leave();

	//	����Ѿ�����
	if (init_flag == true)
	{
		//	�ر��׽���
		init_flag = false;
		closesocket(msocket);
		WSACleanup();
	}
}

//	���÷���˼�����IP�Ͷ˿�,�����Ƿ���0�������ʱ�򷵻�SOCKET_ERROR
int CTCPServerResources::Config(
	const char* ip_str,						//	ָ�������ı���IP
	u_short port,							//	ָ�������ı����˿�
	int backlog								//	������ɵĶ��в��������������߲�����
	)
{
	//	����Ѿ���ʼ������Ҫ����֮ǰ����Ϣ
	if (init_flag == true)
	{
		//	�ر��׽���
		init_flag = false;
		closesocket(msocket);
		WSACleanup();

	}

	//	��ʼ���׽���DLL
	if (WSAStartup(MAKEWORD(2, 1), &mwsaData)) //����Windows Sockets DLL
	{
		WSACleanup();
		abort();			//	ֱ����ֹ��ǰ����
	}

	//	����socket
	this->msocket = socket(AF_INET, SOCK_STREAM, 0);

	//	���socket�Ƿ�Ϸ�
	if ((msocket == NULL) || (msocket == ((SOCKET)(-1))))
	{
		//	�ر��׽���
		WSACleanup();
		msocket = NULL;
		return SOCKET_ERROR;
	}

	//	���巵�ؼ����ʱ����
	int re = NULL;

	//	��
	addrServer.sin_addr.S_un.S_addr = inet_addr(ip_str);// htonl(INADDR_ANY);
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(port);
	re = bind(msocket, (SOCKADDR *)&addrServer, sizeof(addrServer));

	//	�����Ƿ�ɹ�
	if (re == SOCKET_ERROR)
	{
		//	�ر��׽���
		closesocket(msocket);
		WSACleanup();
		return SOCKET_ERROR;
	}

	// ����
	re = listen(msocket, backlog);

	//	�������Ƿ�ɹ�
	if (re == SOCKET_ERROR)
	{
		//	�ر��׽���
		closesocket(msocket);
		WSACleanup();
		return SOCKET_ERROR;
	}

	//	���е���������óɹ���
	init_flag = true;
	return NULL;
}


//	���÷���˼����Ķ˿�,�����Ƿ���0�������ʱ�򷵻�SOCKET_ERROR��ָ����������IP
int CTCPServerResources::Config(
	u_short port,							//	ָ�������ı����˿�
	int backlog								//	������ɵĶ��в��������������߲�����
	)
{
	//	����Ѿ���ʼ������Ҫ����֮ǰ����Ϣ
	if (init_flag == true)
	{
		//	�ر��׽���
		init_flag = false;
		closesocket(msocket);
		WSACleanup();

	}

	//	��ʼ���׽���DLL
	if (WSAStartup(MAKEWORD(2, 1), &mwsaData)) //����Windows Sockets DLL
	{
		WSACleanup();
		abort();			//	ֱ����ֹ��ǰ����
	}

	//	����socket
	this->msocket = socket(AF_INET, SOCK_STREAM, 0);

	//	���socket�Ƿ�Ϸ�
	if ((msocket == NULL) || (msocket == ((SOCKET)(-1))))
	{
		//	�ر��׽���
		WSACleanup();
		msocket = NULL;
		return SOCKET_ERROR;
	}

	//	���巵�ؼ����ʱ����
	int re = NULL;

	//	��
	addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(port);
	re = bind(msocket, (SOCKADDR *)&addrServer, sizeof(addrServer));

	//	�����Ƿ�ɹ�
	if (re == SOCKET_ERROR)
	{
		//	�ر��׽���
		closesocket(msocket);
		WSACleanup();
		return SOCKET_ERROR;
	}

	// ����
	re = listen(msocket, backlog);

	//	�������Ƿ�ɹ�
	if (re == SOCKET_ERROR)
	{
		//	�ر��׽���
		closesocket(msocket);
		WSACleanup();
		return SOCKET_ERROR;
	}

	//	���е���������óɹ���
	init_flag = true;
	return NULL;
}


//	��÷�������Դ
SOCKET CTCPServerResources::GetSocket(void)
{
	this->get_socket_cs.Enter();
	SOCKET tmp_socket = msocket;
	this->get_socket_cs.Leave();
	return tmp_socket;
}

//	��ȡ��������ַ�ṹ��
SOCKADDR_IN CTCPServerResources::GetSocketAddr(void)
{
	return this->addrServer;
}

//	�õ���ǰ������״̬��trueΪ���óɹ���falseΪ����ʧ��
bool CTCPServerResources::GetConfigStatus(void)
{
	this->get_configstatus_cs.Enter();
	bool tmp_init_flag = this->init_flag;
	this->get_configstatus_cs.Leave();
	return tmp_init_flag;
}

//	�ṩIP�˿ڵĹ��캯��
CTCPServerResources::CTCPServerResources(
	const char* ip_str,						//	ָ�������ı���IP
	u_short port,							//	ָ�������ı����˿�
	int backlog								//	������ɵĶ��в��������������߲�����
	)
{
	//	����ַ���������
	memset(this->ip_str, 0, sizeof(this->ip_str));

	//	����Ϊδ��ʼ��
	init_flag = false;

	//	�趨δ��ʼ���ķ�������ԴΪ��
	msocket = NULL;

	this->Config(
		ip_str,
		port,
		backlog
		);
}

//	�ṩ�˿ڵĹ��캯��
CTCPServerResources::CTCPServerResources(
	u_short port,							//	ָ�������ı����˿�
	int backlog								//	������ɵĶ��в��������������߲�����
	)
{
	//	����ַ���������
	memset(this->ip_str, 0, sizeof(this->ip_str));

	//	����Ϊδ��ʼ��
	init_flag = false;

	//	�趨δ��ʼ���ķ�������ԴΪ��
	msocket = NULL;

	this->Config(
		port,
		backlog
		);
}

//	�õ���������IP�ַ���
char* CTCPServerResources::GetServerIPString(void)
{
	//	���巵���ַ���
	memset(ip_str,0,sizeof(ip_str));

	//	���û������
	if (this->init_flag == false)
	{
		return ip_str;
	}

	//	��÷�����IP
	strcpy_s(ip_str, inet_ntoa(this->addrServer.sin_addr));

	//	����ip�ַ���
	return ip_str;
}

//	�õ��������˿�
int CTCPServerResources::GetServerPort(void)
{
	//	���巵�ض˿�
	int re_port = -1;

	//	���û������
	if (this->init_flag == false)
	{
		return re_port;
	}

	//	��÷������˿�
	re_port = htons(this->addrServer.sin_port);

	//	���ض˿�
	return re_port;
}

//-------------------------------------------------------------------------------------------------
//	�����Ƿ��������Ӵ�����Ķ���

//	TCP�������������պ���
DWORD TCP_SERVER_HOST_RECV_THREAD_FUNC(LPVOID pParam)
{
	//	�õ��������
	CTCPServerHost* pCTCPServerHost = (CTCPServerHost*)pParam;

	//	���巵��ֵ
	int re = 0;

	//	��ȡSOCKET��Ϊ����߽���Ч�ʣ�����ȡ�ŵ�ѭ����
	pCTCPServerHost->opt_m_con_socket_cs.Enter();
	SOCKET tmp_socket = pCTCPServerHost->m_con_socket;
	pCTCPServerHost->opt_m_con_socket_cs.Leave();

	//	�������ѭ��
	while (1)
	{

		//	������������
		re = ::recv(
			tmp_socket,
			pCTCPServerHost->recv_buff,
			MAX_BUFFER_SIZE,
			0
			);

		//	�������Ƿ�ʧ�ܣ���reΪ0��ʱ���ʾ���ӹر���
		if ((re == SOCKET_ERROR)||(re == 0))
		{
			//	����ʧ�ܱ�ʶ
			pCTCPServerHost->opt_fail_status_cs.Enter();
			pCTCPServerHost->fail_status = true;
			pCTCPServerHost->opt_fail_status_cs.Leave();

			//	�˳������߳�
			return -1;
		}

		//	���ճɹ��Ļ���ִ���û�����������ʱ�û�����ҲΪ����ʽ�������Ե���һ�ν��յ�����ʱ�򣬻����������ݱ��û���������ռ
		pCTCPServerHost->pFunc(
			tmp_socket,
			pCTCPServerHost->recv_buff,
			re
			);
	}

	return 0;
}

//	���캯�������ṩ����
CTCPServerHost::CTCPServerHost()
{
	opt_fail_status_cs.Enter();
	this->fail_status = false;				//	û��ʧ��
	opt_fail_status_cs.Leave();
	this->init_flag = false;				//	δ��ʼ��
	opt_m_con_socket_cs.Enter();
	this->m_con_socket = NULL;				//	�յ����Ӷ���
	opt_m_con_socket_cs.Leave();
	this->pFunc = NULL;						//	�յĽ��մ�����ָ��
}

//	���ú���,�����Ƿ���0�������ʱ�򷵻�SOCKET_ERROR
int CTCPServerHost::Config(
	SOCKET m_socket,						//	���ӵ�SOCKET
	P_USER_RECV_TCP_SERVER_FUNC p_infunc			//	����ĺ���ָ��
	)
{
	//	�������õ�ʱ�������¿�ʼ����˼�������������ʧ�ܱ�ʶ����������
	opt_fail_status_cs.Enter();
	this->fail_status = false;
	opt_fail_status_cs.Leave();

	//	����Ƿ��Ѿ���������
	if (this->init_flag == true)
	{
		//	��ձ�־����ֹ������ʱ��������
		this->init_flag = false;

		//	��ֹ�����߳�
		this->m_recv_thread.Stop();
		Sleep(10);

		//	�ر�����
		opt_m_con_socket_cs.Enter();
		closesocket(this->m_con_socket);
		opt_m_con_socket_cs.Leave();
	}

	//	�����������Ƿ�Ϸ�
	if ((m_socket == NULL) || (m_socket == (SOCKET)-1) || (p_infunc == NULL) || (p_infunc == (P_USER_RECV_TCP_SERVER_FUNC)-1))
	{
		return SOCKET_ERROR;
	}

	//	��ֵ����
	opt_m_con_socket_cs.Enter();
	this->m_con_socket = m_socket;
	opt_m_con_socket_cs.Leave();
	this->pFunc = p_infunc;
	this->m_recv_thread.Config(
		TCP_SERVER_HOST_RECV_THREAD_FUNC,
		this,
		CThread::RUN
		);

	//	���óɹ�
	this->init_flag = true;
	return 0;
}

//	�������ݣ�����������Ϊ������������Ϊ��ͻ��˷�������,,�����Ƿ����Ѿ����͵��ֽڣ������ʱ�򷵻�SOCKET_ERROR
int CTCPServerHost::Send(
	char* pdata,							//	�����׵�ַ
	int length								//	���ݳ���
	)
{
	//	�����ǰΪʧ��״̬����������
	opt_fail_status_cs.Enter();
	bool tmp_fail_status = this->fail_status;
	opt_fail_status_cs.Leave();
	if (tmp_fail_status == true)
	{
		return SOCKET_ERROR;	
	}

	//	�ж��Ƿ����ã����û������
	if (this->init_flag == false)
	{
		return SOCKET_ERROR;
	}

	//	����������������ַ�Ƿ�
	if ((pdata == NULL) || (pdata == (char*)-1))
	{
		return SOCKET_ERROR;
	}

	//	��������������볤�ȷǷ�
	if (length <= 0)
	{
		return SOCKET_ERROR;
	}

	//	��������
	opt_m_con_socket_cs.Enter();
	SOCKET tmp_socket = this->m_con_socket;
	opt_m_con_socket_cs.Leave();
	int re = ::send(
		tmp_socket,
		pdata,
		length,
		0
		);

	//	����Ƿ���ʧ��
	if (re == SOCKET_ERROR)
	{
		//	����ʧ�ܱ�ʶ
		opt_fail_status_cs.Enter();
		this->fail_status = true;
		opt_fail_status_cs.Leave();

		//	��ֹ�����߳�
		this->m_recv_thread.Stop();
		Sleep(10);

		//	�ر�����
		opt_m_con_socket_cs.Enter();
		closesocket(this->m_con_socket);
		opt_m_con_socket_cs.Leave();
	}

	return re;
}

//	�õ���ǰ������״̬��trueΪ���óɹ���falseΪ����ʧ��
bool CTCPServerHost::GetConfigStatus(void)
{
	return this->init_flag;
}

//	��ȡʧ��״̬
bool CTCPServerHost::GetFailStatus(void)
{
	opt_fail_status_cs.Enter();
	bool tmp_fail_status = this->fail_status;
	opt_fail_status_cs.Leave();
	return tmp_fail_status;
}

//	�Ͽ�����
void CTCPServerHost::Disconnect(void)
{
	//	����Ѿ������˵Ļ���������û��ʧ�ܵ�ʱ��
	if ((this->init_flag == true) && (GetFailStatus() == false))
	{
		//	��ձ�־����ֹ������ʱ��������
		this->init_flag = false;

		//	��ֹ�����߳�
		this->m_recv_thread.Stop();
		Sleep(10);

		//	�ر�����
		opt_m_con_socket_cs.Enter();
		closesocket(this->m_con_socket);
		opt_m_con_socket_cs.Leave();
	}
}

//	��������
CTCPServerHost::~CTCPServerHost()
{
	//	�Ͽ���ͻ��˵����Ӳ��ͷ���Դ
	this->Disconnect();
}

//----------------------------------------------------------------------
//	TCP������

//	���������̺߳���
DWORD TCP_SERVER_MANAGE_ACCEPT_THREAD(LPVOID pParam)
{
	//	������ʱ����
	SOCKET sockConnection = NULL;
	SOCKADDR_IN addrClient;
	int len = sizeof(addrClient);

	//	ת���������
	CTCPServerManage* pCObj = (CTCPServerManage*)(pParam);

	//	�������ӵȴ�ѭ��
	while (1)
	{
		//	����ʽӦ����������
		sockConnection = ::accept(pCObj->socketServer, (SOCKADDR *)&addrClient, &len);

		//	���ӳɹ��󣬽��ÿͻ��˼��������б�
		pCObj->AddConList(
			sockConnection,
			addrClient,
			pCObj->pRecvFunc
			);
	}

	return 0;
}

//	�ҵ������̺߳���
DWORD TCP_SERVER_MANAGE_FIND_ERROR_THREAD(LPVOID pParam)
{
	//	ת���������
	CTCPServerManage* pCObj = (CTCPServerManage*)(pParam);

	//	�����Ҵ����ѭ��
	while (1)
	{
		//	���Ƚ�����ʱ��ÿ��200ms����һ�μ�����
		Sleep(200);

		//	ִ���Զ��Ƴ�����
		pCObj->AutoRemoveError();
	}

	return 0;
}

//	�޲������캯��
CTCPServerManage::CTCPServerManage()
{
	//	����ַ���������
	memset(this->ip_str_GetClientIPStr,0,sizeof(this->ip_str_GetClientIPStr));
	memset(this->ip_str_GetServerIPString,0,sizeof(this->ip_str_GetServerIPString));

	this->init_flag = false;					//	����Ϊ��ʼ��
	this->socketServer = NULL;					//	��������ԴSOCKETΪ��

	this->pOnJoinFunc = NULL;					//	������Ӧ����Ϊ��
	this->pOnBreadFunc = NULL;					//	�Ͽ���Ӧ����Ϊ��

}

//	��������
CTCPServerManage::~CTCPServerManage()
{
	//	����Ƿ��Ѿ���ʼ����
	if (this->init_flag == true)
	{
		//	ֹͣ�����߳�
		this->init_flag = false;
		this->m_accept_thread.Stop();
		this->m_find_error_thread.Stop();
		Sleep(10);

		//	����ѭ���ص����еĿͻ�������
		this->mconlist_cs.Enter();
		for (int i = 0; (this->mConList.begin() + i) != (this->mConList.end()); ++i)
		{			
			//	����ṹ��ָ��
			SConInfo* ptmp_dat = NULL;

			//	��ȡ�ö���ĵ�ַ
			ptmp_dat = (*(this->mConList.begin() + i));

			//	���б���ɾ��
			this->mConList.erase(this->mConList.begin() + i);
			--i;

			//	�ͷ���Դ
			delete(ptmp_dat->pbegin_time);
			ptmp_dat->pCon->Disconnect();
			delete(ptmp_dat->pCon);
			delete(ptmp_dat);
		}
		this->mconlist_cs.Leave();
	}
}

//	�������������������󣬳ɹ�����true,ʧ�ܷ���false
bool CTCPServerManage::AddConList(
	SOCKET in_socket,					//	�Ѿ��ɹ��������ӵ�SOCKET��Ҳ��Ψһ�ı�ʶ
	SOCKADDR_IN	addrClient,				//	�����ӿͻ��˵ĵ�ַ��Ϣ
	P_USER_RECV_TCP_SERVER_FUNC precvfunc		//	�û�����Ľ��պ���
	)
{
	//	������
	//	���in_socket
	if ((in_socket == NULL) || (in_socket == (SOCKET)(-1)))
	{
		return false;
	}

	//	��麯��ָ��
	if ((precvfunc == NULL) || (precvfunc == (P_USER_RECV_TCP_SERVER_FUNC)(-1)))
	{
		return false;
	}

	//	����������
	CTCPServerHost* pCTCPServerHost = new CTCPServerHost;
	int re = pCTCPServerHost->Config(
		in_socket,
		precvfunc
		);

	//	����Ƿ񴴽��ɹ�
	if (SOCKET_ERROR == re)
	{
		//	�ͷŵ������������
		delete pCTCPServerHost;

		//	����ʧ��
		return false;
	}

	//	�����ṹ��
	SConInfo* psconinfo = new SConInfo;

	//	���ýṹ��
	CMyTime* pnow_time = new CMyTime;
	pnow_time->GetNowTime();
	psconinfo->addrClient = addrClient;
	psconinfo->pCon = pCTCPServerHost;
	psconinfo->socket = in_socket;
	psconinfo->pbegin_time = pnow_time;

	//	�������
	mconlist_cs.Enter();
	this->mConList.insert(this->mConList.end(), psconinfo);
	mconlist_cs.Leave();

	//	�����û�������Ӧ����
	if (this->pOnJoinFunc != NULL)
	{
		this->pOnJoinFunc(
			in_socket,
			addrClient,
			pnow_time
			);
	}

	//	�����ɹ�
	return true;
}

//	��������������ɾ��һ�����󣬳ɹ�����true,ʧ�ܷ���false
/*
	�ú����ֱ� �ֶ�ָ���Ͽ�ĳ���ͻ������� �� ����ʧ�ܵ�ʱ�����
*/
bool CTCPServerManage::RemoveConList(
	SOCKET in_socket					//	ɾ��ָ����SOCKET
	)
{
	//	�������socket
	if ((in_socket == NULL) || (in_socket == (SOCKET)(-1)))
	{
		return false;
	}

	//	�����б�Ѱ��socket
	mconlist_cs.Enter();
	int index = -1;
	for (int i = 0; (this->mConList.begin() + i) != (this->mConList.end()); ++i)
	{
		//	����ҵ�
		if ((*(this->mConList.begin() + i))->socket == in_socket)
		{
			index = i;
			break;
		}
	}

	//	����ҵ�
	SConInfo* ptmp_dat = NULL;
	if (index != -1)
	{
		//	��ȡ�ö���ĵ�ַ
		ptmp_dat = (*(this->mConList.begin() + index));

		//	���б���ɾ��
		this->mConList.erase(this->mConList.begin()+index);
	}
	mconlist_cs.Leave();		//	���ڶ�mConList�Ĳ�����û���ˣ��������뿪�ٽ磬Ȼ�������Դ���ͷ�

	//	�����������û���ҵ�
	if ((index == -1)||(ptmp_dat == NULL))
	{
		return false;
	}

	//	�����û��Ͽ���Ӧ����
	if (this->pOnBreadFunc != NULL)
	{
		CMyTime break_time;
		break_time.GetNowTime();		//	�����ǻ�ȡ�ĶϿ�ʱ��
		this->pOnBreadFunc(
			ptmp_dat->socket,
			ptmp_dat->addrClient,
			break_time
			);
	}

	//	�ͷ��Ѿ����б���ɾ������Դ
	delete(ptmp_dat->pbegin_time);
	ptmp_dat->pCon->Disconnect();
	delete(ptmp_dat->pCon);
	delete(ptmp_dat);

	//	�����ɹ�
	return true;
}

//	�Զ����б����Ƴ����������
/*
	�ú������� ��ȡ����ʱ�����������
*/
void CTCPServerManage::AutoRemoveError(void)
{
	//	�����б��е��������ӣ�������ʧ�ܱ�ʶ
	this->mconlist_cs.Enter();
	for (int i = 0; (this->mConList.begin() + i) != (this->mConList.end()); ++i)
	{
		if ((*(this->mConList.begin() + i))->pCon->GetFailStatus() == true)
		{
			//	����ṹ��ָ��
			SConInfo* ptmp_dat = NULL;

			//	��ȡ�ö���ĵ�ַ
			ptmp_dat = (*(this->mConList.begin() + i));

			//	���б���ɾ��
			this->mConList.erase(this->mConList.begin() + i);
			--i;

			//	�����û��Ͽ���Ӧ����
			if (this->pOnBreadFunc != NULL)
			{
				CMyTime break_time;
				break_time.GetNowTime();		//	�����ǻ�ȡ�ĶϿ�ʱ��
				this->pOnBreadFunc(
					ptmp_dat->socket,
					ptmp_dat->addrClient,
					break_time
					);
			}

			//	�ͷ���Դ
			delete(ptmp_dat->pbegin_time);
			ptmp_dat->pCon->Disconnect();
			delete(ptmp_dat->pCon);
			delete(ptmp_dat);
		}
	}
	this->mconlist_cs.Leave();
}

//	��ʼ���������ɹ�����true��ʧ�ܷ���false
bool CTCPServerManage::Config(
	CTCPServerResources& rCTCPServerResources,			//	��������Դ������
	P_USER_RECV_TCP_SERVER_FUNC pfunc							//	�û����պ���
	)
{
	//	������뺯��ָ��
	if ((pfunc == NULL) || (pfunc == (P_USER_RECV_TCP_SERVER_FUNC)(-1)))
	{
		return false;
	}

	//	����Ƿ��Ѿ���ʼ����
	if (this->init_flag == true)
	{
		//	ֹͣ�����߳�
		this->init_flag = false;
		this->m_accept_thread.Stop();
		this->m_find_error_thread.Stop();
		Sleep(10);

		//	����ѭ���ص����еĿͻ�������
		this->mconlist_cs.Enter();
		for (int i = 0; (this->mConList.begin() + i) != (this->mConList.end()); ++i)
		{
			//	����ṹ��ָ��
			SConInfo* ptmp_dat = NULL;

			//	��ȡ�ö���ĵ�ַ
			ptmp_dat = (*(this->mConList.begin() + i));

			//	���б���ɾ��
			this->mConList.erase(this->mConList.begin() + i);
			--i;

			//	�ͷ���Դ
			delete(ptmp_dat->pbegin_time);
			ptmp_dat->pCon->Disconnect();
			delete(ptmp_dat->pCon);
			delete(ptmp_dat);
		}
		this->mconlist_cs.Leave();
	}

	//	���ñ�����Ĳ���
	this->addrServer = rCTCPServerResources.addrServer;
	this->socketServer = rCTCPServerResources.msocket;
	this->pRecvFunc = pfunc;

	//	���������߳�
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

	//	��ʼ�����óɹ�
	this->init_flag = true;
	//	���óɹ�
	return true;
}

//	�������ӺͶϿ���Ӧ�����ĳ�ʼ�����ú������ɹ�����true��ʧ�ܷ���false
bool CTCPServerManage::Config(
	CTCPServerResources& rCTCPServerResources,			//	��������Դ������
	P_USER_RECV_TCP_SERVER_FUNC pfunc,							//	�û����պ���	
	P_USER_ON_TCP_CLIENT_JOIN_FUNC ponjoinfunc,			//	����ͻ��˽�����Ӧ����	
	P_USER_ON_TCP_CLIENT_BREAK_FUNC ponbreadfunc		//	����ͻ��˶Ͽ���Ӧ����
	)
{
	//	����������Ӧ����
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

	//	�������ú���
	return this->Config(
		rCTCPServerResources,
		pfunc
		);
}

//	�������Ӹ���
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

//	�������Ӹ���������ȫ����Ϣ���б�
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

//	�Ͽ����ӣ�true��ʾ�����ɹ���false��ʾ�б���û�д�����
bool CTCPServerManage::Disconnect(SOCKET in_socket)
{
	return this->RemoveConList(in_socket);
}

//	�������ݣ�>0��ʾ�����ɹ���SOCKET_ERROR��ʾ�б���û�д�����,0��ʾ����ʧ��
int CTCPServerManage::Send(
	SOCKET in_socket,									//	ָ����SOCKET���
	char* pdata,										//	�������ݵĻ������׵�ַ
	int length											//	Ҫ���͵ĳ���
	)
{
	//	����������
	if ((in_socket == NULL) || (in_socket == (SOCKET)(-1)) || (pdata == NULL) || (pdata == (char*)(-1)) || (length <= 0))
	{
		return SOCKET_ERROR;
	}

	//	�����б�Ѱ��socket
	mconlist_cs.Enter();
	int index = -1;
	for (int i = 0; (this->mConList.begin() + i) != (this->mConList.end()); ++i)
	{
		//	����ҵ�
		if ((*(this->mConList.begin() + i))->socket == in_socket)
		{
			index = i;
			break;
		}
	}

	//	����ҵ�
	SConInfo* ptmp_dat = NULL;
	if (index != -1)
	{
		//	��ȡ�ö���ĵ�ַ
		ptmp_dat = (*(this->mConList.begin() + index));
	}
	mconlist_cs.Leave();		//	���ڶ�mConList�Ĳ�����û���ˣ��������뿪�ٽ磬Ȼ�������Դ���ͷ�

	//	���û���ҵ�
	if ((index == -1) || (ptmp_dat == NULL))
	{
		return SOCKET_ERROR;
	}

	//	��������
	int re = ptmp_dat->pCon->Send(pdata, length);

	//	����Ƿ��ͳɹ�����ʧ�ܵ�ʱ�򣬾��Ѿ���ʾ��������Ѿ�ʧЧ�ˣ����Խ����б����Ƴ��������
	if (re == SOCKET_ERROR)
	{
		this->RemoveConList(ptmp_dat->socket);
		return SOCKET_ERROR;
	}

	//	���е�����ͳɹ���
	return re;
}

//	�õ�IP�ַ��������󽫷���һ�����ַ���
char* CTCPServerManage::GetClientIPStr(SOCKET in_socket)
{
	//	���֮ǰ�Ľ��
	memset(ip_str_GetClientIPStr, 0, sizeof(ip_str_GetClientIPStr));

	//	�����б�Ѱ��socket
	mconlist_cs.Enter();
	int index = -1;
	for (int i = 0; (this->mConList.begin() + i) != (this->mConList.end()); ++i)
	{
		//	����ҵ�
		if ((*(this->mConList.begin() + i))->socket == in_socket)
		{
			index = i;
			break;
		}
	}

	//	����ҵ�
	SConInfo* ptmp_dat = NULL;
	if (index != -1)
	{
		//	��ȡ�ö���ĵ�ַ
		ptmp_dat = (*(this->mConList.begin() + index));
	}
	mconlist_cs.Leave();		//	���ڶ�mConList�Ĳ�����û���ˣ��������뿪�ٽ磬Ȼ�������Դ���ͷ�

	//	���û���ҵ�
	if ((index == -1) || (ptmp_dat == NULL))
	{
		return ip_str_GetClientIPStr;
	}

	//	��ȡIP�ַ���
	strcpy_s(ip_str_GetClientIPStr, inet_ntoa(ptmp_dat->addrClient.sin_addr));

	//	�����ַ���
	return ip_str_GetClientIPStr;
}

//	�õ��˿���ֵ�����󽫷���0
u_short CTCPServerManage::GetClientPort(SOCKET in_socket)
{
	//	���巵�ض˿�ֵ
	u_short re_port = 0;

	//	�����б�Ѱ��socket
	mconlist_cs.Enter();
	int index = -1;
	for (int i = 0; (this->mConList.begin() + i) != (this->mConList.end()); ++i)
	{
		//	����ҵ�
		if ((*(this->mConList.begin() + i))->socket == in_socket)
		{
			index = i;
			break;
		}
	}

	//	����ҵ�
	SConInfo* ptmp_dat = NULL;
	if (index != -1)
	{
		//	��ȡ�ö���ĵ�ַ
		ptmp_dat = (*(this->mConList.begin() + index));
	}
	mconlist_cs.Leave();		//	���ڶ�mConList�Ĳ�����û���ˣ��������뿪�ٽ磬Ȼ�������Դ���ͷ�

	//	���û���ҵ�
	if ((index == -1) || (ptmp_dat == NULL))
	{
		return re_port;
	}

	//	��ö˿�
	re_port = htons(ptmp_dat->addrClient.sin_port);

	//	���ض˿�
	return re_port;
}

//	�õ������Ŀ�ʼʱ�䣬���󽫷���һ��δ�����ʱ����
CMyTime CTCPServerManage::GetClientBeginTime(SOCKET in_socket)
{
	//	���巵��ʱ�����
	CMyTime re_time;

	//	�����б�Ѱ��socket
	mconlist_cs.Enter();
	int index = -1;
	for (int i = 0; (this->mConList.begin() + i) != (this->mConList.end()); ++i)
	{
		//	����ҵ�
		if ((*(this->mConList.begin() + i))->socket == in_socket)
		{
			index = i;
			break;
		}
	}

	//	����ҵ�
	SConInfo* ptmp_dat = NULL;
	if (index != -1)
	{
		//	��ȡ�ö���ĵ�ַ
		ptmp_dat = (*(this->mConList.begin() + index));
	}
	mconlist_cs.Leave();		//	���ڶ�mConList�Ĳ�����û���ˣ��������뿪�ٽ磬Ȼ�������Դ���ͷ�

	//	���û���ҵ�
	if ((index == -1) || (ptmp_dat == NULL))
	{
		return re_time;
	}

	//	��ö˿�
	re_time = ptmp_dat->pbegin_time;

	//	���ض˿�
	return re_time;
}

//	�õ���������IP�ַ���
char* CTCPServerManage::GetServerIPString(void)
{
	//	���֮ǰ�Ľ��
	memset(ip_str_GetServerIPString, 0, sizeof(ip_str_GetServerIPString));

	//	���û������
	if (this->init_flag == false)
	{
		return ip_str_GetServerIPString;
	}

	//	��÷�����IP
	strcpy_s(ip_str_GetServerIPString, inet_ntoa(this->addrServer.sin_addr));

	//	����ip�ַ���
	return ip_str_GetServerIPString;
}

//	�õ��������˿�
int CTCPServerManage::GetServerPort(void)
{
	//	���巵�ض˿�
	int re_port = -1;

	//	���û������
	if (this->init_flag == false)
	{
		return re_port;
	}

	//	��÷������˿�
	re_port = htons(this->addrServer.sin_port);

	//	���ض˿�
	return re_port;
}

//	��ȡ��������ԴSOCKET��û�����õ�ʱ�򷵻ؿ�
SOCKET CTCPServerManage::GetServerSOCKET(void)
{
	//	���巵��SOCKET
	SOCKET re_socket = NULL;

	//	���û������
	if (this->init_flag == false)
	{
		return re_socket;
	}

	//	��÷�������SOCKET
	re_socket = this->socketServer;

	//	����SOCKET
	return re_socket;
}

//////////////////////////////////////////////////////////////////////////////////////////
//	TCP�ͻ�����

//	���մ����̺߳���
DWORD TCP_CLIENT_RECV_PROC_THREAD(LPVOID pParam)
{
	//	��ȡ����
	CTCPClient* pc = (CTCPClient*)(pParam);

	//	���巵��ֵ
	int re = 0;

	//	��ʼ���Ѿ����������Ĵ���
	int already_reconnect_count = 0;
	
	//	����ѭ��
	while (1)
	{
		//	���Զ�ȡ����
		re = recv(pc->sockClient, pc->recv_buff, sizeof(pc->recv_buff), 0);

		//	����ȡ����������������
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			//	ͳ��ʧ�ܴ���
			++already_reconnect_count;

			//	�����û�ָ���ĶϿ���Ӧ����
			if (pc->pOnBreakFunc != NULL)
			{
				pc->pOnBreakFunc(
					pc->sockClient,
					pc->addrServer,
					already_reconnect_count
					);
			}

			//	��������������
			//	������ߺ󲻽�����������
			if (pc->RetryCount == 0)
			{
				//	�õ�socket����
				SOCKET tmp_socket = pc->sockClient;
				pc->sockClient = NULL;													//	�����ⲿ�������Ѿ��ر�
				if (tmp_socket != NULL) closesocket(tmp_socket);						//	�رյ�ǰ����
				return 0;																//	�߳��˳�
			}
			//	��ʾ���ߺ��������Դ���
			else if(pc->RetryCount == -1)
			{
				//	�õ�socket����
				SOCKET tmp_socket = pc->sockClient;
				pc->sockClient = NULL;													//	�����ⲿ�������Ѿ��ر�
				if (tmp_socket != NULL) closesocket(tmp_socket);						//	�رյ�ǰ����

				//	�ȴ���ʱʱ��
				Sleep(pc->RetrySec * 1000);

				//	���½�������
				//	����SOCKET
				tmp_socket = socket(AF_INET, SOCK_STREAM, 0);

				//	����
				re = connect(tmp_socket, (SOCKADDR *)&(pc->addrServer), sizeof(pc->addrServer));

				//	������ӳɹ�
				if (re != SOCKET_ERROR)
				{
					//	�������ӱ�־
					pc->sockClient = tmp_socket;
				}
				//	�������ʧ��
				else
				{
					closesocket(tmp_socket);					//	�رյ�ǰ����
				}

			}
			//	ָ�����ߺ�����Դ���
			else
			{
				//	���ʧ�ܴ�������
				if (already_reconnect_count > pc->RetryCount)
				{
					//	�õ�socket����
					SOCKET tmp_socket = pc->sockClient;
					pc->sockClient = NULL;													//	�����ⲿ�������Ѿ��ر�
					if (tmp_socket != NULL) closesocket(tmp_socket);						//	�رյ�ǰ����
					return 0;																//	�߳��˳�
				}

				//	�õ�socket����
				SOCKET tmp_socket = pc->sockClient;
				pc->sockClient = NULL;													//	�����ⲿ�������Ѿ��ر�
				if (tmp_socket != NULL) closesocket(tmp_socket);						//	�رյ�ǰ����

				//	�ȴ���ʱʱ��
				Sleep(pc->RetrySec * 1000);

				//	���½�������
				//	����SOCKET
				tmp_socket = socket(AF_INET, SOCK_STREAM, 0);

				//	����
				re = connect(tmp_socket, (SOCKADDR *)&(pc->addrServer), sizeof(pc->addrServer));

				//	������ӳɹ�
				if (re != SOCKET_ERROR)
				{
					//	�������ӱ�־
					pc->sockClient = tmp_socket;
				}
				//	�������ʧ��
				else
				{
					closesocket(tmp_socket);					//	�رյ�ǰ����
				}
			}
		}
		//	����Ϊ������ȡ
		else
		{
			//	�����û�����
			pc->pRecvFunc(
				pc->recv_buff,
				re
				);
		}
	}

	return 0;
}

//	�޲������캯��
CTCPClient::CTCPClient()
{
	this->init_flag = false;								//	û�н�������
	memset(this->ip_str, 0, sizeof(this->ip_str));			//	���IP�ַ������ػ�����
	this->pRecvFunc = NULL;									//	�û����մ�����Ϊ��
	this->RetryCount = 0;									//	Ĭ��Ϊ�������Դ���
	this->RetrySec = -1;									//	Ĭ��Ϊ���ߺ�������������
	this->sockClient = NULL;								//	SOCKET����Ϊ��
	pOnBreakFunc = NULL;									//	�Ͽ���Ӧ����Ϊ��
}

//	��������
CTCPClient::~CTCPClient()
{
	//	����Ѿ���ʼ��
	if (this->init_flag == true)
	{
		this->init_flag = false;
		this->m_recv_thread.Stop();		//	��ֹ�߳�
		Sleep(10);

		//	��ʱ���Ϊ�Ѿ����ӵĻ�
		if (this->sockClient != NULL)
		{
			//	�ͷ�SOCKET
			if (this->sockClient != NULL) closesocket(this->sockClient);
			this->sockClient = NULL;
			WSACleanup();
		}
	}
}

//	���ú�����Ĭ������Ϊ ���ߺ������������ӣ������������Դ������ɹ�����0��ʧ�ܷ���SOCKET_ERROR
int CTCPClient::Config(
	char* remote_ip,										//	Զ��������IP��ַ
	u_short remote_port,									//	Զ�������Ķ˿�
	P_USER_RECV_TCP_CLIENT_FUNC precvfunc,					//	�û�����Ľ��պ���
	bool now_conn,											//	�Ƿ������������ӣ�trueΪ���óɹ����������ӣ�falseΪ�������ã�����������
	P_USER_ON_TCP_SERVER_BREAK_FUNC ponbreakfunc,			//	����������Ͽ����ӵ�ʱ�������õ���Ӧ����
	int retry_sec,											//	���������ĳ�ʱ������0��ʾ���ߺ������������ӣ�>0��ʾ���ߺ󾭹���ú����Խ�������
	int retry_count											//	���Դ�����-1��ʾ�������Դ�����0��ʾ���ߺ󲻽����������ӣ�>0��ʾ���ߺ����ԵĴ���
	)
{
	//	�������
	if ((precvfunc == NULL) || (precvfunc == (P_USER_RECV_TCP_CLIENT_FUNC)(-1)) || (retry_sec < 0) || (retry_count < -1))
	{
		return SOCKET_ERROR;
	}

	//	����Ѿ�������
	if (this->init_flag == true)
	{
		//	Ϊ�˷�ֹ�����������Ƶ��µĶϿ��������������ٴ�ִ����ֹ�߳�
		this->m_recv_thread.Stop();
		Sleep(10);

		//	����Ѿ����ӣ���Ͽ�
		if (this->sockClient != NULL)
		{
			this->Disconnect();
		}
	}
	else
	{
		//	��ʼ���׽���DLL
		if (WSAStartup(MAKEWORD(2, 1), &mwsaData)) //����Windows Sockets DLL
		{
			WSACleanup();
			abort();			//	ֱ����ֹ��ǰ����
		}
	}

	//	���öϿ���Ӧ����
	if ((ponbreakfunc == NULL) || (ponbreakfunc == (P_USER_ON_TCP_SERVER_BREAK_FUNC)(-1)))
	{
		this->pOnBreakFunc = NULL;
	}
	else
	{
		this->pOnBreakFunc = ponbreakfunc;
	}

	//	socket���
	this->sockClient = NULL;

	//	�����û�������պ���
	this->pRecvFunc = precvfunc;

	//	���ó�ʱ����
	this->RetrySec = retry_sec;

	//	�������Դ���
	this->RetryCount = retry_count;
	
	//	���÷�������ַ��Ϣ
	addrServer.sin_addr.S_un.S_addr = inet_addr(remote_ip);
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(remote_port);

	//	��Ϣ�Ѿ�����
	this->init_flag = true;

	//	���������Ƿ��������
	if (now_conn == true)
	{
		return this->Connect();
	}

	return 0;
}

//	���Ӻ���,If no error occurs, connect returns zero. Otherwise, it returns SOCKET_ERROR, and a specific error code can be retrieved by calling WSAGetLastError.
int CTCPClient::Connect(void)
{
	//	����Ƿ��Ѿ���������
	if (this->init_flag != true)
	{
		return SOCKET_ERROR;
	}
	
	//	����Ƿ��Ѿ�����
	if (this->sockClient != NULL)
	{
		return SOCKET_ERROR;
	}

	//	����߳��Ƿ��Ѿ���ֹ
	if (this->GetThreadStatus() != CThread::STOP)
	{
		return SOCKET_ERROR;
	}

	//	����SOCKET
	sockClient = socket(AF_INET, SOCK_STREAM, 0);

	//	����
	int re = connect(sockClient, (SOCKADDR *)&addrServer, sizeof(addrServer));

	//	��������Ƿ�ɹ�
	if (re == SOCKET_ERROR)
	{
		//	�ر�SOCKET
		closesocket(this->sockClient);
		this->sockClient = NULL;
		return SOCKET_ERROR;
	}

	//	�������մ����߳�
	this->m_recv_thread.Config(
		TCP_CLIENT_RECV_PROC_THREAD,
		this,
		CThread::RUN
		);

	//	���ӳɹ�
	return 0;
}

//	�Ͽ�����
void CTCPClient::Disconnect(void)
{
	//	����Ƿ�û������
	if (this->init_flag != true)
	{
		return;
	}
	
	//	��ֹ�߳�
	this->m_recv_thread.Stop();		
	Sleep(10);

	//	�ͷ�SOCKET
	if (this->sockClient != NULL) closesocket(this->sockClient);
	this->sockClient = NULL;
}

//	��������״̬��true��ʾ�Ѿ��������ӣ�false��ʾû�н�������
bool CTCPClient::GetConnectStatus(void)
{
	if (this->sockClient == NULL)	return false;
	else							return true;
}

//	�������״̬��true��ʾ�Ѿ��ɹ����ã�false��ʾû������
bool CTCPClient::GetConfigStatus(void)
{
	return this->init_flag;
}

//	�õ��������˿�
int CTCPClient::GetServerPort(void)
{
	//	���巵�ض˿�
	int re_port = -1;

	//	���û������
	if (this->init_flag == false)
	{
		return re_port;
	}

	//	��÷������˿�
	re_port = htons(this->addrServer.sin_port);

	//	���ض˿�
	return re_port;
}

//	�õ�������IP
char* CTCPClient::GetServerIPStr(void)
{
	//	���֮ǰ�Ľ��
	memset(this->ip_str, 0, sizeof(this->ip_str));

	//	���û������
	if (this->init_flag == false)
	{
		return this->ip_str;
	}

	//	��÷�����IP
	strcpy_s(this->ip_str, inet_ntoa(this->addrServer.sin_addr));

	//	����ip�ַ���
	return this->ip_str;
}

//	�������ݣ�>0��ʾ�����ɹ���SOCKET_ERROR��ʾ�б���û�д�����,0��ʾ����ʧ�ܣ��÷��ͳ������������ӵ�ʱ��Ҳ�᷵�ش���
int CTCPClient::TrySend(
	char* pdata,										//	�������ݵĻ������׵�ַ
	int length											//	Ҫ���͵ĳ���
	)
{
	//	��鷢�Ͳ���
	if ((pdata == NULL)||(pdata == (char*)(-1))||(length <= 0))
	{
		return SOCKET_ERROR;
	}

	//	����Ƿ�û������
	if (this->init_flag != true)
	{
		return SOCKET_ERROR;
	}

	//	����Ƿ�Ϊû������
	if (this->sockClient == NULL)
	{
		return SOCKET_ERROR;
	}

	//	���巵�ر���
	int re = 0;

	//	���÷���
	re = ::send(
		this->sockClient,
		pdata,
		length,
		0);

	//	��ʹ��ʱ���г������߳��ڲ��Ķ�ȡ����Ҳ��������������������ʧ�ܼ��ɣ����ڶ��������������߳̽��п���
	return re;
}

//	�õ����Ե��߳�״̬�������Ӷ��ߵ�ʱ������̻߳���Ȼ�����У���ô˵����ǰ���ڶ���������ʱ��
CThread::EThreadState CTCPClient::GetThreadStatus(void)
{
	//	���巵��״̬
	CThread::EThreadState re;

	//	��ȡ�߳�״̬
	re = this->m_recv_thread.GetState();

	//	����״̬
	return re;
}

//	���SOCKET
SOCKET CTCPClient::GetSocket(void)
{
	//	���巵�ر���
	SOCKET tmp_socket;

	//	��ȡsocket
	tmp_socket = this->sockClient;

	//	����
	return tmp_socket;
}

//	������Դ���
int CTCPClient::GetRetryCount(void)
{
	return this->RetryCount;
}

//	��öϿ�ʱ�ĳ�ʱ����
int CTCPClient::GetRetrySec(void)
{
	return this->RetrySec;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//	����������ʽ��ȡ���ݵ�TCP�ͻ�����

//	���մ����̺߳���
DWORD TCP_CLIENT_VEC_RECV_PROC_THREAD(LPVOID pParam)
{
	//	��ȡ����
	CTCPClientVecRecv* pc = (CTCPClientVecRecv*)(pParam);

	//	���巵��ֵ
	int re = 0;

	//	��ʼ���Ѿ����������Ĵ���
	int already_reconnect_count = 0;

	//	����ѭ��
	while (1)
	{
		//	���Զ�ȡ����
		re = ::recv(pc->sockClient, pc->recv_buff, sizeof(pc->recv_buff), 0);

		//	����ȡ����������������
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			//	ͳ��ʧ�ܴ���
			++already_reconnect_count;

			//	�����û�ָ���ĶϿ���Ӧ����
			if (pc->pOnBreakFunc != NULL)
			{
				pc->pOnBreakFunc(
					pc->sockClient,
					pc->addrServer,
					already_reconnect_count
					);
			}

			//	��������������
			//	������ߺ󲻽�����������
			if (pc->RetryCount == 0)
			{
				//	�õ�socket����
				SOCKET tmp_socket = pc->sockClient;
				pc->sockClient = NULL;													//	�����ⲿ�������Ѿ��ر�
				if (tmp_socket != NULL) closesocket(tmp_socket);						//	�رյ�ǰ����
				return 0;																//	�߳��˳�
			}
			//	��ʾ���ߺ��������Դ���
			else if (pc->RetryCount == -1)
			{
				//	�õ�socket����
				SOCKET tmp_socket = pc->sockClient;
				pc->sockClient = NULL;													//	�����ⲿ�������Ѿ��ر�
				if (tmp_socket != NULL) closesocket(tmp_socket);						//	�رյ�ǰ����

				//	�ȴ���ʱʱ��
				Sleep(pc->RetrySec * 1000);

				//	���½�������
				//	����SOCKET
				tmp_socket = socket(AF_INET, SOCK_STREAM, 0);

				//	����
				re = connect(tmp_socket, (SOCKADDR *)&(pc->addrServer), sizeof(pc->addrServer));

				//	������ӳɹ�
				if (re != SOCKET_ERROR)
				{
					//	�������ӱ�־
					pc->sockClient = tmp_socket;
				}
				//	�������ʧ��
				else
				{
					closesocket(tmp_socket);					//	�رյ�ǰ����
				}

			}
			//	ָ�����ߺ�����Դ���
			else
			{
				//	���ʧ�ܴ�������
				if (already_reconnect_count > pc->RetryCount)
				{
					//	�õ�socket����
					SOCKET tmp_socket = pc->sockClient;
					pc->sockClient = NULL;													//	�����ⲿ�������Ѿ��ر�
					if (tmp_socket != NULL) closesocket(tmp_socket);						//	�رյ�ǰ����
					return 0;																//	�߳��˳�
				}

				//	�õ�socket����
				SOCKET tmp_socket = pc->sockClient;
				pc->sockClient = NULL;													//	�����ⲿ�������Ѿ��ر�
				if (tmp_socket != NULL) closesocket(tmp_socket);						//	�رյ�ǰ����

				//	�ȴ���ʱʱ��
				Sleep(pc->RetrySec * 1000);

				//	���½�������
				//	����SOCKET
				tmp_socket = socket(AF_INET, SOCK_STREAM, 0);

				//	����
				re = connect(tmp_socket, (SOCKADDR *)&(pc->addrServer), sizeof(pc->addrServer));

				//	������ӳɹ�
				if (re != SOCKET_ERROR)
				{
					//	�������ӱ�־
					pc->sockClient = tmp_socket;
				}
				//	�������ʧ��
				else
				{
					closesocket(tmp_socket);					//	�رյ�ǰ����
				}
			}
		}
		//	����Ϊ������ȡ
		else
		{
			//	��������ӵ�VEC��
			pc->AddByteToBufferVec(
				pc->recv_buff,
				sizeof(pc->recv_buff)
				);
		}
	}

	return 0;
}

//	���ú�����Ĭ������Ϊ ���ߺ������������ӣ������������Դ������ɹ�����0��ʧ�ܷ���SOCKET_ERROR
int CTCPClientVecRecv::Config(
	char* remote_ip,										//	Զ��������IP��ַ
	u_short remote_port,									//	Զ�������Ķ˿�
	bool now_conn,											//	�Ƿ������������ӣ�trueΪ���óɹ����������ӣ�falseΪ�������ã�����������
	P_USER_ON_TCP_SERVER_BREAK_FUNC ponbreakfunc,			//	����������Ͽ����ӵ�ʱ�������õ���Ӧ����
	int retry_sec,											//	���������ĳ�ʱ������0��ʾ���ߺ������������ӣ�>0��ʾ���ߺ󾭹���ú����Խ�������
	int retry_count											//	���Դ�����-1��ʾ�������Դ�����0��ʾ���ߺ󲻽����������ӣ�>0��ʾ���ߺ����ԵĴ���
	)
{
	//	�������
	if ((retry_sec < 0) || (retry_count < -1))
	{
		return SOCKET_ERROR;
	}

	//	����Ѿ�������
	if (this->init_flag == true)
	{
		//	Ϊ�˷�ֹ�����������Ƶ��µĶϿ��������������ٴ�ִ����ֹ�߳�
		this->m_recv_thread.Stop();
		Sleep(10);

		//	����Ѿ����ӣ���Ͽ�
		if (this->sockClient != NULL)
		{
			this->Disconnect();
		}
	}
	else
	{
		//	��ʼ���׽���DLL
		if (WSAStartup(MAKEWORD(2, 1), &mwsaData)) //����Windows Sockets DLL
		{
			WSACleanup();
			abort();			//	ֱ����ֹ��ǰ����
		}
	}

	//	���öϿ���Ӧ����
	if ((ponbreakfunc == NULL) || (ponbreakfunc == (P_USER_ON_TCP_SERVER_BREAK_FUNC)(-1)))
	{
		this->pOnBreakFunc = NULL;
	}
	else
	{
		this->pOnBreakFunc = ponbreakfunc;
	}

	//	socket���
	this->sockClient = NULL;

	//	���ó�ʱ����
	this->RetrySec = retry_sec;

	//	�������Դ���
	this->RetryCount = retry_count;

	//	���÷�������ַ��Ϣ
	addrServer.sin_addr.S_un.S_addr = inet_addr(remote_ip);
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(remote_port);

	//	��Ϣ�Ѿ�����
	this->init_flag = true;

	//	���������Ƿ��������
	if (now_conn == true)
	{
		return this->Connect();
	}

	return 0;
}

//	���Ӻ���,If no error occurs, connect returns zero. Otherwise, it returns SOCKET_ERROR, and a specific error code can be retrieved by calling WSAGetLastError.
int CTCPClientVecRecv::Connect(void)
{
	//	����Ƿ��Ѿ���������
	if (this->init_flag != true)
	{
		return SOCKET_ERROR;
	}

	//	����Ƿ��Ѿ�����
	if (this->sockClient != NULL)
	{
		return SOCKET_ERROR;
	}

	//	����߳��Ƿ��Ѿ���ֹ
	if (this->GetThreadStatus() != CThread::STOP)
	{
		return SOCKET_ERROR;
	}

	//	����SOCKET
	sockClient = socket(AF_INET, SOCK_STREAM, 0);

	//	����
	int re = connect(sockClient, (SOCKADDR *)&addrServer, sizeof(addrServer));

	//	��������Ƿ�ɹ�
	if (re == SOCKET_ERROR)
	{
		//	�ر�SOCKET
		closesocket(this->sockClient);
		this->sockClient = NULL;
		return SOCKET_ERROR;
	}

	//	�������մ����߳�
	this->m_recv_thread.Config(
		TCP_CLIENT_VEC_RECV_PROC_THREAD,
		this,
		CThread::RUN
		);

	//	���ӳɹ�
	return 0;
}

//	�޲������캯��
CTCPClientVecRecv::CTCPClientVecRecv()
{

}

//	��������
CTCPClientVecRecv::~CTCPClientVecRecv()
{

}

//	����Ѿ����յ����������ж��ٵ��ֽ�
int CTCPClientVecRecv::GetBufferBytesCount(void)
{
	//	���巵�ض���
	int tmp_count = 0;

	//	��ȡ�ֽ���Ŀ
	this->recvbuffvec_cs.Enter();
	tmp_count = this->RecvBuffVec.size();
	this->recvbuffvec_cs.Leave();

	//	����
	return tmp_count;
}

//	��ȡn���ֽڣ�������
std::vector<unsigned char> CTCPClientVecRecv::TryRecv(int n)
{
	//	���췵�ض���
	std::vector<unsigned char> tmp_vec;

	//	���嵱ǰ�������ڵ��ֽ���Ŀ
	int tmp_count = 0;

	//	����������
	if (n <= 0)
	{
		return tmp_vec;
	}

	//	��ȡ�ֽ���Ŀ
	this->recvbuffvec_cs.Enter(); 
	tmp_count = this->RecvBuffVec.size();

	//	���Ϊ�գ�������������᷵�ؿ�����
	if (tmp_count == 0)
	{

	}
	//	����������ֽ���Ŀ�㹻
	else if (tmp_count >= n)
	{
		for (int i = 0; i < n; ++i)
		{
			tmp_vec.insert(tmp_vec.end(), (*(this->RecvBuffVec.begin() + i)));
		}
	}
	//	�������ֽ���Ŀ����
	else
	{
		for (int i = 0; i < tmp_count; ++i)
		{
			tmp_vec.insert(tmp_vec.end(), (*(this->RecvBuffVec.begin() + i)));
		}
	}
	this->recvbuffvec_cs.Leave();


	//	����
	return tmp_vec;
}

//	��ȡn���ֽڣ�������ȡ
std::vector<unsigned char> CTCPClientVecRecv::WhileRecv(int n)
{
	//	���췵�ض���
	std::vector<unsigned char> tmp_vec;

	//	���嵱ǰ�������ڵ��ֽ���Ŀ
	int tmp_count = 0;

	//	����������
	if (n <= 0)
	{
		return tmp_vec;
	}

	//	�����ֽ���Ŀ�Ƿ��㹻�ı�־��true��ʾ�㹻��false��ʾ����
	bool bytes_enough = false;

	//	����ʽ��ȡ
	do
	{
		//	��ȡ�ֽ���Ŀ
		this->recvbuffvec_cs.Enter();
		tmp_count = this->RecvBuffVec.size();

		//	���Ϊ�գ��ֽڲ���
		if (tmp_count == 0)
		{
			bytes_enough = false;
		}
		//	����������ֽ���Ŀ�㹻
		else if (tmp_count >= n)
		{
			for (int i = 0; i < n; ++i)
			{
				tmp_vec.insert(tmp_vec.end(), (*(this->RecvBuffVec.begin() + i)));
			}
			bytes_enough = true;
		}
		//	�������ֽ���Ŀ����
		else
		{
			bytes_enough = false;
		}
		this->recvbuffvec_cs.Leave();

		//	�˳��ٽ�����Ƿ���Ҫ��ʱ�����ֽ���Ŀ������ʱ�򣬱��߳���ʱ
		if (bytes_enough == false) Sleep(1);
	} while (bytes_enough == false);			//	ֻҪ������һֱѭ����ȡ


	//	����
	return tmp_vec;
}

//	��ջ�����
void CTCPClientVecRecv::ClearBuffer(void)
{
	this->recvbuffvec_cs.Enter();
	this->RecvBuffVec.clear();
	this->recvbuffvec_cs.Leave();
}

//	�򻺳�����׷������
void CTCPClientVecRecv::AddByteToBufferVec(
	char* pbuff,				//	Ҫ�������ݵ��׵�ַ
	int length					//	Ҫ׷�����ݵĳ���
	)
{
	//	�����������Ƿ�Ϸ�
	if ((pbuff == NULL) || (pbuff == (char*)(-1)) || (length <= 0))
	{
		return;
	}

	//	ѭ����������
	this->recvbuffvec_cs.Enter();
	for (int i = 0; i < length; ++i)
	{
		this->RecvBuffVec.insert(this->RecvBuffVec.end(), *(pbuff + i));
	}
	this->recvbuffvec_cs.Leave();
}
