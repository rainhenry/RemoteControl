/************************************************************************

	�������ƣ�UDPTCP����ͨ�ų����
	�������ڣ�2016.12.10
	����޸ģ�2017.03.18
	��    ����1.0
	��    �ƣ�rainhenry
	��    �룺rainhenry
	ƽ    ̨��VS2013 @ Win7 SP1 64bit
	
	�汾�޶�
		0.1�汾		��дUDP��������
		0.2�汾		��UDP��������������UNICODE����ļ��ݴ���
		0.3�汾		��UDP_SERVER_THREAD_FUNC�̺߳����ڲ���Ҫ���ʵ����Ա��Ϊprotected��ʽ����
					����UDP_SERVER_THREAD_FUNC��ΪCUDPServer�����Ԫ�����������Ϣ��ȫ��ʹ�ð�ȫ
		0.4�汾		��дTCP��������Դ��
					��дTCP�����ࣨһ��������Դ���һ�����ӣ�
					��дTCP������
					TCP����������OK
		0.5�汾		��дTCP�ͻ���
					�޸�strcpyΪ��ȫ����strcpy_s
		0.6�汾		TCP�ͻ��˱�д��ɣ������л�������
		0.7�汾		�޸�TCP����������ʱ�������ڴ�й¶����
		0.8�汾		����TCP������������������������������ʼ��Ŵ����µı���BUG
		0.9�汾		����Vec��ʽ��TCP�ͻ����࣬�������Ķ�ȡ���ݷ�ʽ
		1.0�汾		��TCP��������Դ��������GetSocketAddr������������չ

//////////////////////////////////////////////////////////////////////////
	��������ṩ�ĳ��ú�����
		GetAllLoaclIP								��ñ�������������IP
		GetLocalHostName							��ñ�����
		CUDPServer::GetIPStr						��sockaddr_in�ṹ����ȡIP��Ϣ�ַ���
		CUDPServer::GetIPPort						��sockaddr_in�ṹ����ȡ�˿�ֵ
		CUDPServer::GetIPStrFromCIPAddressCtrl		��CIPAddressCtrl�ؼ����л�ȡIP��Ϣ�ַ���

	�������÷���
		inet_ntoa(addrServer.sin_addr)				��sockaddr_in�ṹ����ȡIP��Ϣ�ַ���
		htons(addrServer.sin_port)					��sockaddr_in�ṹ����ȡ�˿�ֵ

	ʹ��ע�����
		��������������࣬����֧�ֱ�׼����������Ϊ������ϵͳ��Դ��������������ʱ��ᵼ����ԴʧЧ����
		�����ʹ�������������Զ�������࣬�����������������ָ�룬һ������ֱ�ӹ������ʵ������

//////////////////////////////////////////////////////////////////////////
	UDP���������ص�
	UDP��������UDP�ͻ��ˣ��ڴ����ϵ�Ψһ���������bind�����ĵ��á�
	�����ԣ�UDP�������������û�з����κ���Ϣ��ʱ����������Ľ��ա�
	��UDP�ͻ��˷�ʽ�Ļ�����û�з�������֮ǰ�����޷����յ��κ���Ϣ�ġ�
	����UDP����������ȫ����ȡ��UDP�ͻ��ˣ����Ҹ��õ������һ���ܡ�

	UDP��������ʹ��˵��
	//	������մ�����
	void UDP_ServerFunc(
	SOCKADDR_IN remote,					//	�����Ե�Զ��������ַ��Ϣ
	char* pbuffer,								//	���ݻ������׵�ַ
	int	length									//	�յ����ֽ���Ŀ
	)
	{
		//....�û�����....
		//	����ע�⣬remote�ṹ���ڲ�����Ϣ�����ñ����ṩ��GetIPStr������GetIPPort������ȡ���������ӷ���
	}

	//	����UDP������������ʵ������ȻҲ�����ȹ���һ���յ�CUDPServer mCUDPServer;Ȼ������Config��Ա������ʼ��Ҳ����
	CUDPServer mCUDPServer(
	UDP_ServerFunc,
	287
	);

	//	����
	int re = mCUDPServer.Send(
	mCUDPServer.GetIPStrFromCIPAddressCtrl(&mremoteip),		//	IP
	remote_port,											//	�˿�
	buff,													//	���ݻ�����
	length													//	Ҫ���͵ĳ���
	);

//////////////////////////////////////////////////////////////////////////
	TCP������ʵ��ԭ��
	TCP����������3����ʵ�֣��ֱ�Ϊ��������Դ�ࡢ���������� �� ������
	�û���ʹ�õ�ʱ��ֻ�漰����Դ�ࣨCTCPServerResources�� �� �����ࣨCTCPServerManage����
	���У���Դ��ֻ�Ǵ򿪱����ϵ�ĳ����������ȫ����������ָ���˿ڣ�Ȼ����а󶨼�����֮����Ϊһ����Դ�ṩ��������ʹ�á�
	��������ÿ�����Ӷ�����һ���������������ÿһ��������������󶼺����и������߳����ڽ������ݣ���ִ���û��Զ���Ľ��մ�������
	�ڹ�����������һ��Ӧ��ͻ��˵�����������߳� �� һ�������������Ӷ���Ĵ��󣬲������ִ����ʱ������ɾ���ȹ��ܡ�
	��ʹ�ù������ʱ����Ҫ�û��ṩһ��������Ӧ������һ���ͻ��˽�����Ӧ���� �� һ���ͻ��˶Ͽ���Ӧ������
	����Ҫע����ǣ�������Ӧ���������ٱ�������ͬ���̵߳��ã�������������Ӧ�����е�����Ҫ�����ٽ籣������������û�����������ر����ӻ����˳���ʱ��ᵼ���̱߳�����
	���Ҫ˵�����ǣ�����������ԴSOCKET���뽨������ʱ��SOCKET�ǲ�ͬ�Ķ�����ʹ��ʱҪע�����֣���

	ʹ�þ�����
	//	�����û����պ���
	void UserRecvProcFunc(
		SOCKET in_socket,								//	�������ӵ�SOCKET���
		char* pdat,										//	���ݻ������׵�ַ
		int length										//	�յ����ֽ���Ŀ
		)
	{
		//	�û��������
		//	�˴���Ҫ��ÿͻ��˵�IP��˿ڿ��Ե��ù������е���ط�����CTCPServerManage::GetClientIPStr��
		//	Ҳ����ͨ������������ķ����鿴��ǰ����������Դ��IP�Ͷ˿ڵ�����
		//	......
	}

	//	����������Ӧ����
	void OnTCPConn(
		SOCKET in_socket,								//	�ɹ��������ӵ�SOCKET���
		SOCKADDR_IN clientaddr,							//	�ͻ�����Ϣ
		CMyTime begin_time								//	�ɹ��������ӵ�ʱ���
		)
	{
		//	�û��������
		//	....	
	
	}

	//	����Ͽ���Ӧ����
	void OnTCPDisco(
		SOCKET in_socket,								//	�Ͽ����ӵ�SOCKET���
		SOCKADDR_IN clientaddr,							//	�Ͽ�����֮ǰ�Ŀͻ�����Ϣ
		CMyTime end_time								//	�Ͽ����ӵ�ʱ���
		)
	{
		//	�û��������
		//	....	
	}

	//	����һ����������Դ�����
	CTCPServerResources mTCPRes;

	//	����������1234�˿�,�����Ƿ���0��������߶˿�ռ�õ�ʱ�򷵻�SOCKET_ERROR(-1)����������Ϊint
	mTCPRes.Config(
		1234
		);

	//	����һ�����������������
	CTCPServerManage mTCPManage;

	//	��ʼ�����ù����࣬�ɹ�����true��ʧ�ܷ���false����������Ϊbool
	//	���жϿ���������Ӧ���������ֶ�ָ��ΪNULL�������Ͳ��ᴦ���û��ĶϿ��������¼�
	mTCPManage.Config(
		mTCPRes,
		UserRecvProcFunc,					//	�û�����Ľ��մ�����
		OnTCPConn,							//	�û������������Ӧ����
		OnTCPDisco							//	�û�����ĶϿ���Ӧ����
		);

	//	����һ������
	char re_str[] = "welcome connect host!\r\n";
	mTCPManage.Send(
		socket,										//	�������ΪSOCKET����ָ����ͻ������ӵ�SOCKET�������ڽ��ܴ������еõ��������ڱ��������б��ʱ��õ�
		re_str,										//	��������Ķ��ֽڱ����ʽ��������ÿ��ֽڿ���д�ɣ� CWCharToChar(re_str, E_CHAR, E_UTF8).UTF8()  ��Ҫ����iconv.h
		strlen(re_str)								//	Ҫ���͵��ֽ���Ŀ
		);

	//	�ر�ĳһ��ָ��������
	mTCPManage.Disconnect(socket);					//	��socket�Ļ�ȡ�����͸���ͬ��������

	//	���ͷŷ�������Դ��ʱ����֮ʹ�õ����й�������󣬺�������������Ӷ������ͷ�
	mTCPRes.Release();

	//	������ǰ�����Ѿ�����Ŀͻ�������
	//	��ȡ���Ӹ���
	std::vector<CTCPServerManage::SReConInfo> sockcopy = mTCPManage.GetConListAllInfoCopy();

	//	��ȡ�ܹ����뵽�������Ŀͻ�����Ŀ
	total_con = sockcopy.size();

	//	�������Ϊ�˻�ȡ������Ŀ�����Բ��ø�Ч�ʵķ���
	total_con = mTCPManage.GetConListCopy().size();

	//	������ʾÿ���ͻ��˵���Ϣ
	//	���ֱ����������õ�����Ϣ����Ϊִ�л�ȡ��Ϣ���ʱ��һ�̵Ŀͻ����б�����ȡ���Ӹ�������һ�̡�
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
	TCP�ͻ���
	ÿһ���ͻ��˶����ж���һ���̣߳����̸߳�������û������ݣ����Ҵ������ʱ�Ĳ�������������������ȴ�һ��ʱ������� �� ͳ�ƶ��ߴ���
	���ú���Config�����Ӻ���Connect�����������������ʱ�������ʱ�����������߻��߶˿�û�п��ţ������������SOCKET_ERROR��������������ǰ�߳�
	������Ҫ�û������Լ��Ľ��մ���������ѡ���Ƕ���Ͽ���Ӧ����

	ʹ�þ���:

	//	���������Ӧ����
	void TCPRecvFunc(
		char* pdata,								//	���ݻ������׵�ַ
		int	length									//	�յ����ֽ���Ŀ
		)
	{
		//	�û�����
	}

	//	����Ͽ���Ӧ����
	void TCPBreakFunc(
		SOCKET in_socket,								//	����ʱ��socket
		SOCKADDR_IN addrServer,							//	�������ĵ�ַ��Ϣ
		int	total										//	�Ѿ�ʧ�ܵĴ���
		)
	{
		//	�û�����
	}

	//	����һ��TCP�ͻ��˶���
	CTCPClient mTCP_Client;

	//	���÷���������������������
	int re = mTCP_Client.Config(
		"192.168.2.101",							//	ָ���ķ�����IP
		1234,										//	ָ���Ķ˿�
		TCPRecvFunc,								//	�û�����Ľ��մ�����
		true,										//	������ɺ�������������
		TCPBreakFunc,								//	�������Ͽ���Ӧ����������ѡ�����õ�ʱ�����ָ��ΪNULL
		0,											//	���������ĳ�ʱ������0��ʾ���ߺ�������������
		-1											//	���Դ�����-1��ʾ�������Դ���
		);

	//	������ӷ��ؽ��
	if (re == SOCKET_ERROR)
	{
		//	��ʱΪ����ʧ��
	}
	else
	{
		//	��ʱΪ���ӳɹ�
	}

	//	��������
	char send_data[] = "Hello World!";
	mTCP_Client.TrySend(
		send_data,
		strlen(send_data)
		);

	//	�Ͽ�����
	mTCP_Client.Disconnect();

************************************************************************/

//////////////////////////////////////////////////////////////////////////
//	Ԥ����ͷ����
#ifndef __UDPTCP_H__
#define __UDPTCP_H__

//////////////////////////////////////////////////////////////////////////
//	����ͷ�ļ�
#include <stdio.h>
#include <Winsock2.h>
#include <iostream>
#include <vector>
#include "../CThread/CThread.h"
#include "../CMyCriticalSection/CMyCriticalSection.hpp"
#include "../CMyTime/CMyTime.h"

//////////////////////////////////////////////////////////////////////////
//	�궨���������غ����Ͷ���
#define		MAX_BUFFER_SIZE			10000			//	��󻺳����ֽ���Ŀ

//	��Ҫ�û�����Ľ��պ������˺����ṹ����UDP����
//	���ڸú���Ϊ�߳������������ɸú��������������ݺ������ط�����Ļ�����Ҫ�����߳�ͬ������
typedef void (*P_USER_RECV_FUNC)(
	SOCKADDR_IN,						//	�����Ե�Զ��������ַ��Ϣ
	char*,								//	���ݻ������׵�ַ
	int									//	�յ����ֽ���Ŀ
	); 

//	��Ҫ�û�����Ľ��մ��������˺����ṹ����TCP����
typedef void(*P_USER_RECV_TCP_SERVER_FUNC)(
	SOCKET,								//	�������ӵ�SOCKET���
	char*,								//	���ݻ������׵�ַ
	int									//	�յ����ֽ���Ŀ
	);

//	��Ҫ�û�����ĵ�TCP�ͻ��˽���ʱ����Ӧ����
typedef void(*P_USER_ON_TCP_CLIENT_JOIN_FUNC)(
	SOCKET,								//	�ɹ��������ӵ�SOCKET���
	SOCKADDR_IN,						//	�ͻ�����Ϣ
	CMyTime								//	�ɹ��������ӵ�ʱ���
	);

//	��Ҫ�û�����ĵ�TCP�ͻ��˶Ͽ�ʱ����Ӧ����
typedef void(*P_USER_ON_TCP_CLIENT_BREAK_FUNC)(
	SOCKET,								//	�Ͽ�����֮ǰ��SOCKET���
	SOCKADDR_IN,						//	�Ͽ�����֮ǰ�Ŀͻ�����Ϣ
	CMyTime								//	�Ͽ����ӵ�ʱ���
	);

//	��Ҫ�û������TCP�ͻ��˽��մ�����
typedef void(*P_USER_RECV_TCP_CLIENT_FUNC)(
	char*,								//	���ݻ������׵�ַ
	int									//	�յ����ֽ���Ŀ
	);

//	��Ҫ�û�����ĵ�TCP�������Ͽ���ʱ�����Ӧ����
typedef void(*P_USER_ON_TCP_SERVER_BREAK_FUNC)(
	SOCKET,								//	����ʱ��socket
	SOCKADDR_IN,						//	�������ĵ�ַ��Ϣ
	int									//	�Ѿ�ʧ�ܵĴ���
	);


//////////////////////////////////////////////////////////////////////////
//	���ȫ���ı���IP
std::vector<CString> GetAllLoaclIP(void);

//	��ȡ���������
CString GetLocalHostName(void);

//////////////////////////////////////////////////////////////////////////
//	UDP�������ඨ��
class CUDPServer
{
private:
	//	��ָ����ַ�ṹ�壬���л��IP�ַ����ķ��ػ���������
	char ip_str_for_getipstr[50];

	//	��ָ����ַ�ؼ���ʱ����ַ������ػ���������
	char ip_str_for_GetIPStrFromCIPAddressCtrl[50];

protected:
	bool init_flag;								//	�Ѿ���ʼ���ı�ʶ
	SOCKET msocket;
	WSADATA mwsaData;
	SOCKADDR_IN localaddr;						//	���ص�ַ��Ϣ
	char recv_buffer[MAX_BUFFER_SIZE];			//	������ջ�����
	CThread mCThread;							//	��������߳���ʵ��
	P_USER_RECV_FUNC pUserRecvFunc;				//	�û��Զ���Ľ��մ�������ַ

	//	������Щ���������ٽ籣�������ö��߳�֮�乲��ʹ������
	CMyCriticalSection mGetIPCMCS;				//	�õ�IP�ַ����������ٽ籣��
	CMyCriticalSection mGetIPFromCMCS;			//	�õ�IP�ַ����������ٽ籣��
	CMyCriticalSection mGetPortCMCS;			//	�õ��˿���ֵ�������ٽ籣��

public:
	//	���캯��

	//	�κβ��������ṩ
	CUDPServer();

	//	�����ǹ������ˣ�������Ҫ�ṩ���󶨱����Ķ˿ں�IP
	//	�ṩ�˿ں�IP
	CUDPServer(
		P_USER_RECV_FUNC pFunc,			//	�û��Զ���Ľ��մ�����
		u_short		port,				//	�󶨱����Ķ˿�
		const char*	ip_str				//	�󶨱�����IP
		);
	
	//	�ṩ�˿ڣ�IPΪINADDR_ANY����������IP
	CUDPServer(
		P_USER_RECV_FUNC pFunc,			//	�û��Զ���Ľ��մ�����
		u_short	port
		);

	//	�ṩ�˿ں�IP
	void Config(
		P_USER_RECV_FUNC pFunc,			//	�û��Զ���Ľ��մ�����
		u_short		port,				//	�󶨱����Ķ˿�
		const char*	ip_str				//	�󶨱�����IP
		);

	//	�ṩ�˿ڣ�IPΪINADDR_ANY����������IP
	void Config(
		P_USER_RECV_FUNC pFunc,			//	�û��Զ���Ľ��մ�����
		u_short	port
		);

	//	�ر�����
	void Close(void);

	//	�������ݣ������ɹ��᷵�������͵��ֽ���Ŀ������᷵��SOCKET_ERROR
	int Send(
		const char* remote_ip,			//	Ҫ���͵���Ŀ��IP
		u_short remote_port,			//	Ҫ���͵���Ŀ��˿�
		char* pBuffer,					//	�������ݵ��׵�ַ
		int length						//	���ͳ��ȣ��ֽڵ�λ��
		);

	//	�õ�IP�ַ���
	char* GetIPStr(SOCKADDR_IN);

	//	�õ��˿���ֵ
	u_short GetIPPort(SOCKADDR_IN);

	//	ͨ��IP��ַ�ؼ��õ�IP��ַ�ַ���
	char* GetIPStrFromCIPAddressCtrl(CIPAddressCtrl* pCIPAddressCtrl);

	//	��������
	~CUDPServer();

	//	UDP����������պ�������
	friend DWORD UDP_SERVER_THREAD_FUNC(LPVOID pParam);
};

//////////////////////////////////////////////////////////////////////////
/*
	��������Դ���������ָ��������IP�Ͷ˿���Դ��Ϊ���Լ����Ĳ��֣�Ȼ���ṩ�����������ӹ�������Ϊʹ��
	���ӹ�������ڲ��ж���̹߳�����һ���̸߳������ָ��������Դ��������������̸߳�������Ѿ��ɹ�����������
	�������ӵ��̵߳����У���1��Ϊ�����̣߳����̸߳���ͨ���������ͻ�������������ͻ��˷�������
	ʣ�µı��������̶߳�Ϊ�����̣߳����̵߳ĸ���Ϊ�ɹ��������ӵĸ�����ÿ���̶߳�������Լ��Ŀͻ����ж�ȡ���ݡ�
	���ɹ���ȡ���ݵ�ʱ�򣬻�ͳһ����һ�����պ���ָ�룬�ú������û��ṩ
	�ڶ�ȡ�ͷ��͵Ĺ����У��������ʧ�ܵ��������Ϊ���ӶϿ�������������Դ��ɾ����
	��ɾ���Ĺ����У����Ϊ���չ����г��ִ��������ֱ��ɾ������Դ��Ȼ�������˳��������̡߳�
	������ڷ��͹����з��ֵ����⣬����Ҫ�Ƚ�����ȡ�̣߳�Ȼ���ٴ������б����Ƴ������ӡ�
*/

//	TCP��������Դ�ඨ��
class CTCPServerResources
{
private:
	//	���巵���ַ���
	char ip_str[50];

protected:
	bool init_flag;								//	�Ѿ���ʼ���ı�ʶ

	WSADATA mwsaData;

	//	����������Դ�ĵ�ַ��Ϣ
	SOCKADDR_IN addrServer;

	//	��������Դ
	SOCKET msocket;

	//	��ȡsocket��cs
	CMyCriticalSection get_socket_cs;

	//	��ȡ����״̬��cs
	CMyCriticalSection get_configstatus_cs;
	
public:
	//	���ṩ�κγ�ʼ����Ϣ
	CTCPServerResources();

	//	�ṩIP�˿ڵĹ��캯��
	CTCPServerResources(
		const char* ip_str,						//	ָ�������ı���IP
		u_short port,							//	ָ�������ı����˿�
		int backlog = SOMAXCONN					//	������ɵĶ��в��������������߲�����
		);

	//	�ṩ�˿ڵĹ��캯��
	CTCPServerResources(
		u_short port,							//	ָ�������ı����˿�
		int backlog = SOMAXCONN					//	������ɵĶ��в��������������߲�����
		);


	//	���÷���˼�����IP�Ͷ˿�,�����Ƿ���0�������ʱ�򷵻�SOCKET_ERROR
	int Config(
		const char* ip_str,						//	ָ�������ı���IP
		u_short port,							//	ָ�������ı����˿�
		int backlog = SOMAXCONN					//	������ɵĶ��в��������������߲�����
		);

	//	���÷���˼����Ķ˿�,�����Ƿ���0�������ʱ�򷵻�SOCKET_ERROR��ָ����������IP
	int Config(
		u_short port,							//	ָ�������ı����˿�
		int backlog = SOMAXCONN					//	������ɵĶ��в��������������߲�����
		);

	//	�ͷ���Դ
	void Release(void);

	//	��������
	~CTCPServerResources();

	//	��÷�������Դ
	SOCKET GetSocket(void);

	//	��ȡ��������ַ�ṹ��
	SOCKADDR_IN GetSocketAddr(void);

	//	�õ���ǰ������״̬��trueΪ���óɹ���falseΪ����ʧ��
	bool GetConfigStatus(void);

	//	�õ���������IP�ַ�������δ���õ�ʱ�򣬷��ؿ��ַ���
	char* GetServerIPString(void);

	//	�õ��������˿ڣ���δ���õ�ʱ�򣬷���-1
	int GetServerPort(void);

	//	����TCP������������Ϊ��Դ�����Ԫ�࣬����������Է�����Դ��ı�����Ա
	friend class CTCPServerManage;
};

//------------------------------------------------------------------------
/*
	��TCP��������������ǣ���������ɹ��յ�һ�����������ʱ�򣬽�����һ�������ʵ��
	��ʵ�������������ӵõ���SOCKET���� �� ָ���Ľ��մ�����ָ��
*/
//	TCP�������ඨ��
class CTCPServerHost
{
protected:
	//	���մ����߳�
	CThread m_recv_thread;

	//	��ʼ��־
	bool init_flag;

	//	�����߳�ָ��
	P_USER_RECV_TCP_SERVER_FUNC pFunc;

	//	���ӵ�SOCKET
	SOCKET m_con_socket;

	//	ʧ��״̬��trueΪʧ�ܣ�falseΪ����
	bool fail_status;

	//	��������SOCKET��CS
	CMyCriticalSection opt_m_con_socket_cs;

	//	����ʧ��״̬��־��CS
	CMyCriticalSection opt_fail_status_cs;

	//	������ջ�����
	char recv_buff[MAX_BUFFER_SIZE];

public:
	//	���캯�������ṩ����
	CTCPServerHost();

	//	���ú���,�����Ƿ���0�������ʱ�򷵻�SOCKET_ERROR
	int Config(
		SOCKET m_socket,								//	���ӵ�SOCKET
		P_USER_RECV_TCP_SERVER_FUNC p_infunc			//	����ĺ���ָ��
		);

	//	�������ݣ�����������Ϊ������������Ϊ��ͻ��˷�������,�����Ƿ����Ѿ����͵��ֽڣ������ʱ�򷵻�SOCKET_ERROR
	int Send(
		char* pdata,									//	�����׵�ַ
		int length										//	���ݳ���
		);

	//	�õ���ǰ������״̬��trueΪ���óɹ���falseΪ����ʧ��
	bool GetConfigStatus(void);

	//	��������
	~CTCPServerHost();
	
	//	��ȡʧ��״̬
	bool GetFailStatus(void);

	//	�Ͽ�����
	void Disconnect(void);

	//	TCP��������������պ�������
	friend DWORD TCP_SERVER_HOST_RECV_THREAD_FUNC(LPVOID pParam);
};

//------------------------------------------------------------------------
/*
	TCP���������涨��2���̣߳�һ���ǵȴ����еĿͻ��˽������ӣ���һ�����ҵ���֪�����б������������ӣ��������Ǵ��б���ɾ��
	��װ����Ҫ������
		��ʼ������
		��ȡ�����б�������
		�Ͽ����Ӻ���
		���ͺ���
		��ȡָ������SOCKET��IP�Ͷ˿�
		��ȡ�������������õķ�������Դ��IP�Ͷ˿�

	�������Ӷ���ṹ����÷�װAddConList��RemoveConList�ķ������������Ը��õĿ����ڴ����Դ����
*/
//	TCP������
class CTCPServerManage
{
public:
	//	���������ڲ������Ͷ���,������Ϣ�ṹ��
	typedef struct tagSConInfo
	{
		SOCKET socket;								//	��ͻ������ӵ�SOCKET���
		SOCKADDR_IN addrClient;						//	�ͻ��˵ĵ�ַ��Ϣ
		CMyTime* pbegin_time;						//	��ʼ�������ӵ�ʱ��
		CTCPServerHost* pCon;						//	���Ӷ���
	}SConInfo,*pSConInfo;

	//	���������̺߳���
	friend DWORD TCP_SERVER_MANAGE_ACCEPT_THREAD(LPVOID pParam);

	//	�ҵ������̺߳���
	friend DWORD TCP_SERVER_MANAGE_FIND_ERROR_THREAD(LPVOID pParam);

	//	�������Ӹ����Ľṹ��Ϣ
	typedef struct tagSReConInfo
	{
		SOCKET socket;								//	���ӵ�SOCKET���
		std::string ip_str;							//	IP��ַ�ַ���
		int port;									//	�˿�
		CMyTime begin_time;							//	��ʼ���ӵ�ʱ��
	}SReConInfo;

private:
	//	��������
	std::vector<SConInfo*> mConList;

	//	���������Ĳ���cs
	CMyCriticalSection mconlist_cs;

	//	���巵�ؿͻ���IP��ַ���ַ���������
	char ip_str_GetClientIPStr[50];

	//	���巵�ط�����IP��ַ���ַ���������
	char ip_str_GetServerIPString[50];

protected:
	//	��ʼ����־
	bool init_flag;

	//	��������Դ��SOCKET
	SOCKET socketServer;

	//	ָ����������Դ�ĵ�ַ��Ϣ
	SOCKADDR_IN addrServer;

	//	���������߳�
	CThread m_accept_thread;

	//	�ҵ������߳�
	CThread m_find_error_thread;

	//	�������������������󣬳ɹ�����true,ʧ�ܷ���false
	bool AddConList(
		SOCKET in_socket,							//	�Ѿ��ɹ��������ӵ�SOCKET��Ҳ��Ψһ�ı�ʶ
		SOCKADDR_IN	addrClient,						//	�����ӿͻ��˵ĵ�ַ��Ϣ
		P_USER_RECV_TCP_SERVER_FUNC precvfunc		//	�û�����Ľ��պ���
		);

	//	��������������ɾ��һ�����󣬳ɹ�����true,ʧ�ܷ���false
	bool RemoveConList(
		SOCKET in_socket							//	ɾ��ָ����SOCKET
		);

	//	�Զ����б����Ƴ����������
	void AutoRemoveError(void);

	//	�û��Ľ��մ�����
	P_USER_RECV_TCP_SERVER_FUNC pRecvFunc;

	//	����ͻ��˽�����Ӧ����
	P_USER_ON_TCP_CLIENT_JOIN_FUNC pOnJoinFunc;

	//	����ͻ��˶Ͽ���Ӧ����
	P_USER_ON_TCP_CLIENT_BREAK_FUNC pOnBreadFunc;

public:
	//	�޲������캯��
	CTCPServerManage();

	//	��������
	~CTCPServerManage();

	//	��ʼ���������ɹ�����true��ʧ�ܷ���false
	bool Config(
		CTCPServerResources& rCTCPServerResources,					//	��������Դ������
		P_USER_RECV_TCP_SERVER_FUNC pfunc							//	�û����պ���
		);

	//	�������ӺͶϿ���Ӧ�����ĳ�ʼ�����ú������ɹ�����true��ʧ�ܷ���false
	bool Config(
		CTCPServerResources& rCTCPServerResources,					//	��������Դ������
		P_USER_RECV_TCP_SERVER_FUNC pfunc,							//	�û����պ���	
		P_USER_ON_TCP_CLIENT_JOIN_FUNC ponjoinfunc,					//	����ͻ��˽�����Ӧ����	
		P_USER_ON_TCP_CLIENT_BREAK_FUNC ponbreadfunc				//	����ͻ��˶Ͽ���Ӧ����
		);

	//	�������Ӹ�������������SOCKET���б�
	std::vector<SOCKET> GetConListCopy(void);

	//	�������Ӹ���������ȫ����Ϣ���б�
	std::vector<SReConInfo> GetConListAllInfoCopy(void);

	//	�Ͽ����ӣ�true��ʾ�����ɹ���false��ʾ�б���û�д�����
	bool Disconnect(SOCKET in_socket);

	//	�������ݣ�>0��ʾ�����ɹ���SOCKET_ERROR��ʾ�б���û�д�����,0��ʾ����ʧ��
	int Send(
		SOCKET in_socket,											//	ָ����SOCKET���
		char* pdata,												//	�������ݵĻ������׵�ַ
		int length													//	Ҫ���͵ĳ���
		);

	//	�õ�IP�ַ��������󽫷���һ�����ַ���
	char* GetClientIPStr(SOCKET in_socket);

	//	�õ��˿���ֵ�����󽫷���0
	u_short GetClientPort(SOCKET in_socket);

	//	�õ������Ŀ�ʼʱ�䣬���󽫷���һ��δ�����ʱ����
	CMyTime GetClientBeginTime(SOCKET in_socket);

	//	�õ���������IP�ַ�������δ���õ�ʱ�򣬷��ؿ��ַ���
	char* GetServerIPString(void);

	//	�õ��������˿ڣ���δ���õ�ʱ�򣬷���-1
	int GetServerPort(void);

	//	��ȡ��������ԴSOCKET��û�����õ�ʱ�򷵻ؿ�
	SOCKET GetServerSOCKET(void);
};

//////////////////////////////////////////////////////////////////////////
/*
	TCP�ͻ�����
	һ���ͻ���������У�����һ�������̣߳����̼߳����Ƶ����û��Ľ�����Ӧ������Ҳ���ƶ������������Եȹ��ܡ�
	�����Ƿ��ʼ����Ҫ��init_flag�ı�־
	�����Ƿ�����Ҫ��sockClient��ֵ

	ע�⣺�ڽ������ӷ�������ʱ�����۵���Config����Connect�������������������˲����ڣ������������ʧ�ܡ�
*/

class CTCPClient
{
private:
	//	IP�ַ���������
	char ip_str[50];

protected:
	//	��ʼ����־
	bool init_flag;

	WSADATA mwsaData;

	//	�������ĵ�ַ��Ϣ
	SOCKADDR_IN addrServer;

	//	�����߳�
	CThread m_recv_thread;

	//	�û��Ľ��մ�����
	P_USER_RECV_TCP_CLIENT_FUNC pRecvFunc;

	//	�û�ָ���ĵ���������Ͽ�����ʱ����Ӧ����
	P_USER_ON_TCP_SERVER_BREAK_FUNC pOnBreakFunc;

	//	�ͻ���socket��Ҳ��������ʾ����״̬����ΪNULL��ʱ��Ϊδ���ӡ����������ʾ���ӡ�
	SOCKET sockClient;

	//	���������ĳ�ʱ������0��ʾ���ߺ������������ӣ�>0��ʾ���ߺ󾭹���ú����Խ�������
	int RetrySec;	

	//	���Դ�����-1��ʾ�������Դ�����0��ʾ���ߺ󲻽����������ӣ�>0��ʾ���ߺ����ԵĴ���
	int RetryCount;				

	//	���ջ�����
	char recv_buff[MAX_BUFFER_SIZE];

public:
	//	�޲������캯��
	CTCPClient();

	//	��������
	~CTCPClient();

	//	���ú�����Ĭ������Ϊ ���ߺ������������ӣ������������Դ������ɹ�����0��ʧ�ܷ���SOCKET_ERROR
	int Config(
		char* remote_ip,										//	Զ��������IP��ַ
		u_short remote_port,									//	Զ�������Ķ˿�
		P_USER_RECV_TCP_CLIENT_FUNC precvfunc,					//	�û�����Ľ��պ���
		bool now_conn,											//	�Ƿ������������ӣ�trueΪ���óɹ����������ӣ�falseΪ�������ã�����������
		P_USER_ON_TCP_SERVER_BREAK_FUNC ponbreakfunc = NULL,	//	����������Ͽ����ӵ�ʱ�������õ���Ӧ����
		int retry_sec = 0,										//	���������ĳ�ʱ������0��ʾ���ߺ������������ӣ�>0��ʾ���ߺ󾭹���ú����Խ�������
		int retry_count = -1									//	���Դ�����-1��ʾ�������Դ�����0��ʾ���ߺ󲻽����������ӣ�>0��ʾ���ߺ����ԵĴ���
		);

	//	���Ӻ���,If no error occurs, connect returns zero. Otherwise, it returns SOCKET_ERROR, and a specific error code can be retrieved by calling WSAGetLastError.
	int Connect(void);

	//	�Ͽ�����
	void Disconnect(void);

	//	��������״̬��true��ʾ�Ѿ��������ӣ�false��ʾû�н�������
	bool GetConnectStatus(void);

	//	�������״̬��true��ʾ�Ѿ��ɹ����ã�false��ʾû������
	bool GetConfigStatus(void);

	//	�õ��������˿�
	int GetServerPort(void);

	//	�õ�������IP
	char* GetServerIPStr(void);

	//	�������ݣ�>0��ʾ�����ɹ���SOCKET_ERROR��ʾ�б���û�д�����,0��ʾ����ʧ�ܣ��÷��ͳ������������ӵ�ʱ��Ҳ�᷵�ش���
	int TrySend(
		char* pdata,										//	�������ݵĻ������׵�ַ
		int length											//	Ҫ���͵ĳ���
		);

	//	�õ����Ե��߳�״̬�������Ӷ��ߵ�ʱ������̻߳���Ȼ�����У���ô˵����ǰ���ڶ���������ʱ��
	CThread::EThreadState GetThreadStatus(void);

	//	���SOCKET
	SOCKET GetSocket(void);

	//	������Դ���
	int GetRetryCount(void);

	//	��öϿ�ʱ�ĳ�ʱ����
	int GetRetrySec(void);

	//	���մ����̺߳���
	friend DWORD TCP_CLIENT_RECV_PROC_THREAD(LPVOID pParam);
};

//////////////////////////////////////////////////////////////////////////
//	VEC��ȡ��ʽ��TCP�ͻ�����
class CTCPClientVecRecv:
	public CTCPClient
{
protected:
	//	�ڲ��Ŀͻ��˶���
	CTCPClient mTcpClient;

	//	���ݽ�������
	std::vector<unsigned char> RecvBuffVec;

	//	���ʽ���������CS
	CMyCriticalSection recvbuffvec_cs;

	//	�򻺳�����׷������
	void AddByteToBufferVec(
		char* pbuff,				//	Ҫ�������ݵ��׵�ַ
		int length					//	Ҫ׷�����ݵĳ���
		);

public:
	//	�޲������캯��
	CTCPClientVecRecv();

	//	��������
	~CTCPClientVecRecv();
	
	//	����Ѿ����յ����������ж��ٵ��ֽ�
	int GetBufferBytesCount(void);

	//	��ȡn���ֽڣ�������
	std::vector<unsigned char> TryRecv(int n);

	//	��ȡn���ֽڣ�������ȡ
	std::vector<unsigned char> WhileRecv(int n);

	//	��ջ�����
	void ClearBuffer(void);

	//	���ú�����Ĭ������Ϊ ���ߺ������������ӣ������������Դ������ɹ�����0��ʧ�ܷ���SOCKET_ERROR
	int Config(
		char* remote_ip,										//	Զ��������IP��ַ
		u_short remote_port,									//	Զ�������Ķ˿�
		bool now_conn,											//	�Ƿ������������ӣ�trueΪ���óɹ����������ӣ�falseΪ�������ã�����������
		P_USER_ON_TCP_SERVER_BREAK_FUNC ponbreakfunc = NULL,	//	����������Ͽ����ӵ�ʱ�������õ���Ӧ����
		int retry_sec = 0,										//	���������ĳ�ʱ������0��ʾ���ߺ������������ӣ�>0��ʾ���ߺ󾭹���ú����Խ�������
		int retry_count = -1									//	���Դ�����-1��ʾ�������Դ�����0��ʾ���ߺ󲻽����������ӣ�>0��ʾ���ߺ����ԵĴ���
		);

	//	���Ӻ���,If no error occurs, connect returns zero. Otherwise, it returns SOCKET_ERROR, and a specific error code can be retrieved by calling WSAGetLastError.
	int Connect(void);

	//	���մ����̺߳���
	friend DWORD TCP_CLIENT_VEC_RECV_PROC_THREAD(LPVOID pParam);

};

//////////////////////////////////////////////////////////////////////////
#endif		//	__UDPTCP_H__
