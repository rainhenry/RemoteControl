/**************************************************************************************

	�������ƣ����Э����
	�������ڣ�2017.03.13
	����޸ģ�2017.03.31
	����汾��REV 0.5
	��Ʊ�д��rainhenry
	����ƽ̨��VS2013 Windows7

	�汾�޶���
		0.1�汾			�������̣�����˿ͻ�����
		0.2�汾			���ӷ�������
		0.3�汾			���Ӽ���TCP�ײ�Ŀͻ�����
		0.4�汾			���Ӽ���TCP�ײ�ķ�����������
						���Ӽ���TCP�ײ�ķ�����������
		0.5�汾			�޸����ڻ�ȡ��������ʱ����������ʱ�򽫴�������û�����������
						�޸����ڻ�ȡ��������ʱ����������ʱ���µ��ڴ�й©����

	���Ŀ�ģ�
		1�����Ϊ����ģʽ
		2����������ֻ����һ��һ̨��������������Բ��ü����

	����ԭ��
		���ȷ������˵ĳ����ͽ�������
		Ȼ��ͻ��˵Ľ��ճ������յ���ָ���Ƿ�Ϊ��������ָ��
		ָ����ȷ�󣬵����û��������򣬲���ý���֮�������
		Ȼ���û���Ҫ���ô�����֮��Ļ�ִ�����ٷ��ͻط�������

	����Э���ʽ

	[��������]�����ֶ��壬��������� ������ -> �ͻ���
	===========================================================================================
	��������			|	�ֽ���Ŀ		|				��;����				|		Ĭ��ֵ
	----------------+---------------+-----------------------------------+----------------------
	����ͷ			|		4		|	����ʶ������						|	28 77 93 5A	(HEX)
	----------------+---------------+-----------------------------------+----------------------
	����ID			|		2		|	�����������ڵ������У������		|	0~65536 (u_short)
	----------------+---------------+-----------------------------------+----------------------
	����ID����		|		2		|	����ID�İ�λȡ��������У��		|	0~65536 (u_short)
	----------------+---------------+-----------------------------------+----------------------
	����������		|		4		|	����������������					|			>=1
	----------------+---------------+-----------------------------------+----------------------
	�������ݳ���		|		4		|	�û��������ݵ��ֽ���Ŀ			|		
	----------------+---------------+-----------------------------------+----------------------
	�û���������		|		0~n		|	�ṩ�û���չ						|		
	----------------+---------------+-----------------------------------+----------------------
	�������ݳ���		|		4		|	�û��������ݵ��ֽ���Ŀ			|		����ѡ��
	----------------+---------------+-----------------------------------+----------------------
	�û���������		|		0~n		|	�ṩ�û���չ						|		����ѡ��
	===========================================================================================

	[��ִ����]�����ֶ��壬������ķ����� �ͻ��� -> ������
	===========================================================================================
	��������			|	�ֽ���Ŀ		|				��;����				|		Ĭ��ֵ
	----------------+---------------+-----------------------------------+----------------------
	����ͷ			|		4		|	����ʶ������						|	79 32 87 5A	(HEX)
	----------------+---------------+-----------------------------------+----------------------
	����ID			|		2		|	�����������ڵ������У������		|	0~65536 (u_short)
	----------------+---------------+-----------------------------------+----------------------
	����ID����		|		2		|	����ID�İ�λȡ��������У��		|	0~65536 (u_short)
	----------------+---------------+-----------------------------------+----------------------
	����������		|		4		|	����������������					|			>=1	
	----------------+---------------+-----------------------------------+----------------------
	��ִ���ݳ���		|		4		|	�û���ִ���ݵ��ֽ���Ŀ			|		
	----------------+---------------+-----------------------------------+----------------------
	�û���ִ����		|		0~n		|	�ṩ�û���չ						|		
	----------------+---------------+-----------------------------------+----------------------
	��ִ���ݳ���		|		4		|	�û���ִ���ݵ��ֽ���Ŀ			|		����ѡ��
	----------------+---------------+-----------------------------------+----------------------
	�û���ִ����		|		0~n		|	�ṩ�û���չ						|		����ѡ��
	===========================================================================================

	���Կ���֪��һ������֡�ĳ�������Ҫ���ڵ��� 4+2+2+4 = 12���ֽ�

	��������ͼ

	�ͻ���																								������
	--------------------------------------------------------------------------------------------------------------------------------------------------------
	���������Ӧ�������ø�������OnRecvHandle����									<<============		�����������ݣ���������1����������
		||
	    \/
	��ʽ������ȷ�󣬸�������Զ������û�������򣨻�ִ�������ٺ���1����������
	��û�ִ���ݣ����Ҹ�������Զ���ִ���ݴ��
	    ||
		\/
	�û���GetReturnData��������Ѿ�����õĻ�ִ����
	����������صķ��ͷ��������ݻ�ִ��������										=============>>		���������Ӧ�������ø�������OnRecvHandle����
																										||
																										\/
																									��ʽ������ȷ�󣬸�������Զ������û�������򣬴����ִ����
																										||
																										\/
																									��ʱ���һ�����ݴ���
	---------------------------------------------------------------------------------------------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////////////////////

�ͻ���ʹ�þ�����

//	���Э��������ݴ�����
std::vector<CMonitorProtocol::SDataArea> OnRecvData(
	std::vector<CMonitorProtocol::SDataArea> in_vec							//	��ڲ���������ʱ��������
	)
{
	//	in_vecΪ���Է��������������е�������
	//	�����巵�ص��ǽ�Ҫ��ִ����������������
	//	�мǲ���ֱ�� return in_vec; �������� ����ͬһ���ڴ����򽫻��ͷ����ε������ش���
	//	���췵�ص�std::vector<CMonitorProtocol::SDataArea>�����ʱ���ڲ���pdatҪ����new�������룬��ʵ����ʹ����ɺ���Զ��ͷ�
	//	�м��û��������Լ��ͷŸú�������ںͳ��ڵ�pdat���ݣ������������Զ����ͷš�
}

//	���Э��ʵ��
CMonitorProtocol::CClient myclient_protocol;

//	Ȼ����������Ľ��մ������п�����������OnRecvHandle����
void OnTCPRecv(
	char* in_buf,								//	���ݻ������׵�ַ
	int	length									//	�յ����ֽ���Ŀ
	)
{
	//	��������
	bool re = myclient_protocol.OnRecvHandle(
		in_buf,
		length
		);

	//	�������ɹ�
	if (re == true)
	{
		myTCPCLient.TrySend(
			(char*)(myclient_protocol.GetReturnData()),
			myclient_protocol.GetReturnDataSize()
			);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
	�������ڴ�ͳ�Ľ�Э����TCP�ײ�ֿ��Ļ���ʹ�÷������ӣ����Ҵ������ȱ�ݣ������Ѿ�����Ϊȫ���ɷ�ʽ�Ŀͻ������������
	���з�������ص���Դ�ౣ�ֲ��䣬�û���Ҫʹ�õĹ������Ѿ���Ϊȫ���ɵķ�ʽ

	ȫ���ɵķ�����ʹ��˵����

	//	���Э��ķ��������մ�����
	void OnRecvUserData(
		SOCKET in_socket,														//	�����SOCKET����ʾ����һ��������������
		std::vector<CMonitorProtocol::SDataArea> in_vec							//	��ڲ��������Կͻ��˵Ļ�ִ������
		)
	{
		//	����Ϊ����in_vec���ݿ飬���������Կͻ��˵Ļ�ִ
		//	ʹ�����һ����Ҫ�ͷ�in_vec����Ļ��棬��Ϊȫ���ɷ���������Զ��ͷ�
	}

	//	TCP���ӽ������Ӧ����
	void OnTCPJoin(
		SOCKET in_socket,								//	�ɹ��������ӵ�SOCKET���
		SOCKADDR_IN ClientAddr,							//	�ͻ�����Ϣ
		CMyTime	begin_time								//	�ɹ��������ӵ�ʱ���
		)
	{
		//	�û�����...
	}

	//	TCP���ӶϿ�����Ӧ����
	void OnTCPBreak(
		SOCKET in_socket,								//	�Ͽ�����֮ǰ��SOCKET���
		SOCKADDR_IN ClientAddr,							//	�Ͽ�����֮ǰ�Ŀͻ�����Ϣ
		CMyTime	end_time								//	�Ͽ����ӵ�ʱ���
		)
	{
		//	�û�����...
	}

	//	����TCP��������Դ����
	CTCPServerResources myTCPServerRes;

	//	����TCP�������������
	CMonitorProtocol::CintegratedTCPServerManage myTCPManage;

	//	���ü����Ķ˿�
	myTCPServerRes.Config(1234);

	//	����ȫ����TCP������������
	myTCPManage.Config(
		myTCPServerRes,					//	ָ���ı�����Դ
		OnRecvUserData,					//	����������Ӧ����
		OnTCPJoin,						//	������Ӧ����
		OnTCPBreak						//	�Ͽ���Ӧ����
		);

	//	��������
	myTCPManage.Send(
		socket,							//	�û�ָ����SOCKET
		tmp_vec							//	Ҫ���͵����ݿ�
		);
		
	//	ֹͣ������
	myTCPServerRes.Release();

	//	������ǰ�����Ѿ�����Ŀͻ�������
	//	��ȡ���Ӹ���
	std::vector<CMonitorProtocol::CintegratedTCPServerManage::SReConInfo> sockcopy = mTCPManage.GetConListAllInfoCopy();

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


///////////////////////////////////////////////////////////////////////////////////////
	tcpȫ���ɿͻ��˵�ʹ��
	
	//	TCP�ͻ���ʵ��
	CMonitorProtocol::CintegratedTCPClient myClient;

	//	�����û����մ�����
	std::vector<CMonitorProtocol::SDataArea> OnRecvData(
		std::vector<CMonitorProtocol::SDataArea> in_vec							//	��ڲ���������ʱ��������
		)
	{
		//	�û����룬����std::vector<CMonitorProtocol::SDataArea>
		//	in_vecΪ���Է��������������е�������
		//	�����巵�ص��ǽ�Ҫ��ִ����������������
		//	�мǲ���ֱ�� return in_vec; �������� ����ͬһ���ڴ����򽫻��ͷ����ε������ش���
		//	���췵�ص�std::vector<CMonitorProtocol::SDataArea>�����ʱ���ڲ���pdatҪ����new�������룬��ʵ����ʹ����ɺ���Զ��ͷ�
		//	�м��û��������Լ��ͷŸú�������ںͳ��ڵ�pdat���ݣ������������Զ����ͷš�
	}

	//	�������쳣�Ͽ���Ӧ����
	void OnServerBreak(
		SOCKET in_socket,								//	����ʱ��socket
		SOCKADDR_IN ServerAddr,							//	�������ĵ�ַ��Ϣ
		int fail_count									//	�Ѿ�ʧ�ܵĴ���
		)
	{
		//	�û����롣������
	}
							
	//	��������
	myClient.Config(
		"192.168.137.10",
		1234,
		true,
		OnRecvData,
		OnServerBreak,
		0,												//	���������ĳ�ʱ������0��ʾ���ߺ������������ӣ�>0��ʾ���ߺ󾭹���ú����Խ�������
		0												//	���Դ�����-1��ʾ�������Դ�����0��ʾ���ߺ󲻽����������ӣ�>0��ʾ���ߺ����ԵĴ���
	);

	//	�Ͽ�����
	myClient.Disconnect();

**************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////
//	�ض��屣��
#ifndef __CMonitorProtocol_h__
#define __CMonitorProtocol_h__

///////////////////////////////////////////////////////////////////////////////////////
//	����ͷ�ļ�
#include <vector> 
#include "../UDPTCP/UDPTCP.h"

///////////////////////////////////////////////////////////////////////////////////////
//	���������ռ�
namespace CMonitorProtocol
{
	//-------------------------------------------------------------------------------
	//	��С���ݳߴ�
	extern const int MIN_DATA_SIZE;

	//	������������ʽ�ṹ��
	typedef struct tagSDataArea
	{
		void* pdat;							//	���������׵�ַ
		int length;							//	���ݳ���
	}SDataArea;


	//	�����û��������ͣ����ͻ����յ����Է��������������������û��������Ҫ����Ҫ�����������Ļ�ִ������
	typedef std::vector<SDataArea>(*P_USER_CMP_CLIENT_FUNC)(
		std::vector<SDataArea>							//	��ڲ���������ʱ��������
		);

	//	�����û��������ͣ����������յ��ͻ��˵Ļ�ִ���ݣ��ṩ���û����д���
	typedef void(*P_USER_CMP_SERVER_FUNC)(
		std::vector<SDataArea>							//	��ڲ��������Կͻ��˵Ļ�ִ������
		);

	//-------------------------------------------------------------------------------
	/*
	��Ҫ���з����У�
		���á��������Է������Ľ�ͼ����
	˽�з�����
		��ִͼ�����ݵ�������
	*/
	//	�ͻ����࣬������صĻ���
	class CClient
	{
	private:
		//	�û�����Ľ�������ָ��
		P_USER_CMP_CLIENT_FUNC pUserFunc;

		//	��ִ�����׵�ַ�ͳ��ȣ��Ѿ������
		void* pReturnData;
		int ReturnDataSize;

		//	��ִ������
		std::vector<SDataArea> ReturnDataVec;

	public:
		//	�޲������캯��
		CClient();

		//	��������
		~CClient();

		//	���ú���������true��ʾ���óɹ���false��ʾ����ʧ��
		bool Config(
			P_USER_CMP_CLIENT_FUNC puserfunc					//	���ݵ��û�����ָ��
			);

		//	������Ӧ����������true��ʾ�ɹ��յ�����ִ�е��������false��ʾ�޷�ƥ��������������Э���ϵ�ƥ�䣬���û��������������޹�
		bool OnRecvHandle(
			void* in_dat,										//	�յ��������׵�ַ
			int length											//	���ݳ���
			);

		//	�õ����������Ļ�ִ�����׵�ַ
		void* GetReturnData(void);

		//	�õ���ִ���ݵĳ���
		int GetReturnDataSize(void);

	};

	//-------------------------------------------------------------------------------
	//	��������
	class CServer
	{
	private:
		//	�û�����Ľ��մ�����
		P_USER_CMP_SERVER_FUNC pUserFunc;

		//	�������ݻ������׵�ַ
		void* pSendDataBuff;

		//	�������ݵĳ���
		int SendDataBuffSize;

		//	����ʱ������ID����
		unsigned short cmd_id;

	public:
		//	�޲������캯��
		CServer();

		//	��������
		~CServer();

		//	���ú��������óɹ�����true��ʧ�ܷ���false
		bool Config(
			P_USER_CMP_SERVER_FUNC puserfunc				//	������û�����ָ��
			);

		//	���췢�����ݣ�����ɹ�����true,ʧ�ܷ���false
		bool BuildSendData(
			std::vector<SDataArea> in_vec
			);

		//	������Ӧ����������true��ʾ�ɹ��յ�����ִ�е��������false��ʾ�޷�ƥ��������������Э���ϵ�ƥ�䣬���û��������������޹�
		bool OnRecvHandle(
			void* in_dat,										//	�յ��������׵�ַ
			int length											//	���ݳ���
			);

		//	��÷������ݵĻ������׵�ַ
		void* GetSendDataBuff(void);

		//	��÷������ݵĻ���������
		int GetSendDataBuffSize(void);
	};

	//-------------------------------------------------------------------------------
	/*
		ȫ���ɵĿͻ���������ݴ�������
			��Ƹ����Ŀ����Ϊ������ԭ��TCP�ͻ������Э��ͻ�����֮����������⣬����ԭ����Ʋ��õ��������ݳ��ȵ�ͨ�Ż��ƣ���socket����ͨ����Υ��������ͨ�����ش���
		Ϊ�˴Ӹ����Ͻ����һ���⣬Ҫ�ӽ���Э���ʱ���������˼��ȥ���в��������������ԭ�е�������Ķ������������ƺ����������ǿ���ʹ�õģ�����Ч��Ҳ������������
		�������ķ�ʽ����һЩ�����ͷ���ʵ��ȫ���ɵĹ��ܡ���Ҫ��Ҫ���ص��У�

		�յ�����ʱ�Ĵ���ĺ���OnRecvCintegratedTCPClient_Func
			�ú���Ϊȫ���͵ĺ�����ҲΪCintegratedTCPClient�����Ԫ���������Է��ʱ����protected��Ա���ú����ɱ����Connect���������߳����������������ڲ���һ����
		ѭ��������Ҫ���������Ǹ���Э�����Ҫ����recv������ȡ���ݣ��ڴ�����뷽ʽ����new��ʽ�������ƻ��ڲ����õ���ѭ���ķ�ʽʵ�֣�Ȼ�󽫶����������ֹ�ᵽһ��״̬���档

		����Ҫ˵��һ��RecvVec����������OnRecvCintegratedTCPClient_Func�߳��ڽ���recv����ʱд�룬Ȼ������OnRecvCintegratedTCPClient_Func�̵߳����û�������ȡ�����
		��OnRecvCintegratedTCPClient_Func�߳��ͷ��ڴ档���Ը����ݽ�����һ���߳��н��ж�д�����Բ���ҪCS���б�����

		������ֹ�̵߳ķ�ʽ���������߳����������ڴ���ͷ��ڴ�ĳɶԹ��̣�����ȱһ���ɡ������̵߳Ľ�����ʽ������Ϊֱ�����߳���ǿ����ֹ�������ȹر�socket���ӣ�Ȼ��recv��
		�ش�����룬Ȼ������ٽ����ж��Ƿ��������ط������߳���ֹ���Ȼ���ͷ��ڴ棬��ʱ�̰߳�ȫ�˳���

		���ݴ����������
			
			�ⲿ��ѭ����ʼ  
			||
			\/
			��մ����־
			||
			\/
			��ȡ1���ֽڣ��ж��Ƿ���ȷ����ȷ����ִ�У��������ñ�־ִ��continue���
			||
			...֮���3���ֽ�Ҳͬ�ϴ���
			||
			\/
			��ȡ4���ֽڻ�ȡID��ID���룬����Ļ����ñ�־ִ��continue���
			||
			\/
			��ȡ4���ֽڻ�ȡ���ݿ�ĸ���,������ȡ����ֵ�Ƿ���ȷ���������ñ�־��ִ��continue���
			||
			\/
			�����ڲ�Сѭ������ѭ��ÿ��ѭ�����ڱ�ʾһ����������<<==========++
					||												  /\
					\/												  ||
					��ȡ4���ֽڣ���õ�ǰ�������ĳ���					  ||
					������ȡ����ֵ�Ƿ���ȷ��						  ||
					�������ñ�־��ִ��continue���					  ||
					||												  ||
					\/												  ||
					��ȡn���ֽڣ���ȡ��ǰ������						  ||
					������ȡ����ֵ�Ƿ���ȷ��						  ||
					�������ñ�־��ִ��continue���					  ||
					||												  ||
					\/												  ||ѭ����һ�Σ��ж��������Ƿ�������
					�����ݹ������RecvVec��Ա������new����============>>++
							||
							||
							\/ѭ���������Ѿ�������������
			++<<============++
			||
			||��ʱ�ѻص��ⲿ�Ĵ�ѭ��λ��
			\/
			�����û��������ݴ�����pUserFunc������û����صĻ�ִ������
			||
			\/
			�����ͷ�RecvVec�ڲ���Դ
			||
			\/
			����ִ���������͵�����������
						||
						\/
						����һ����Ҫ���Ͷ����ֽڣ������뷢�ͻ���
						||
						\/
						��������ͷ
						||
						\/
						����ID��ID����
						||
						\/
						��������������
						||
						\/						
						ѭ��������������������ִ���ݵ�vec <<========================++
								||												  /\
								\/												  ||
								���õ�ǰ�������ĳ���								  ||
								||												  ||
								\/												  ||ѭ����һ�Σ��ж�VEC�Ƿ�������
								������ǰ������===================================>>++
										||
										||
										\/ѭ���������Ѿ�������������
						++<<============++
						||
						||��ʱ�ѻص�������ִ���������͵����������̡�
						\/
						ִ�з��ͣ�����鷢���Ƿ���ȷ���������ñ�־��ִ��continue���
						||
						\/������ִ���������͵����������̡�ִ�����
			++<<========++
			||
			||��ʱ�ѻص��ⲿ�Ĵ�ѭ�������λ��
			\/
			�ж��Ƿ��д����־���ô����Ϊ�������ʣ�û�д���Ĵ���Ϊ0��
			һ���������յ������ݱ�����󣬵���ִ�н��պͷ��ͺ���û�����⣬�����ݴ��󣬴�ʱֻ��Ҫ���»�ȡ��ȷ���ݼ��ɣ����ݴ������Ϊ1
			��һ���ǽ��ջ��߷��ͺ����������⣬���������⣬һ�����Ϊ���������ߵ����⡣��ʱ��Ҫ�����������ƵĴ���ȣ����Ӵ������Ϊ2
			���Ϊ���Ӵ��󣬼�������������⣬ִ��continue���
			||
			\/
			�������������������ʾ�ȱ��������һ���Ĵ���ʽ�����Ѵ���ֱ�Ӹ��ƹ����Ϳ��ԣ���������ʡ��
			||
			\/
			�ⲿ��ѭ������
			
	*/
	//	�����û��������պ�������
	typedef std::vector<SDataArea>(*P_USER_ON_RECV_DATAAREA_CLIENT_FUNC)(
		std::vector<SDataArea>
		);

	//	ȫ���ɵĿͻ����ඨ��
	class CintegratedTCPClient:
		protected CTCPClientVecRecv
	{
	private:
		//	�Ͽ���־,tureΪҪ��Ͽ���falseΪû��Ҫ��
		bool Disconnect_flag;

		//	�Ͽ���־��CS
		CMyCriticalSection Disconnect_flag_CS;

	protected:
		//	�Ѿ����յ����ݿ�
		std::vector<SDataArea> RecvVec;

		//	��ִ�������������ݿ�
		std::vector<SDataArea> ReturnVec;

		//	�û����մ�����
		P_USER_ON_RECV_DATAAREA_CLIENT_FUNC pUserFunc;

		//	��ȡ�˿ڱ�־,tureΪҪ��Ͽ���falseΪû��Ҫ��
		bool GetDisconnectFlag(void);

		//	���öϿ���־,tureΪҪ��Ͽ���falseΪû��Ҫ��
		void SetDisconnectFlag(bool);

	public:
		//	�Ͽ�����
		using CTCPClientVecRecv::Disconnect;

		//	��������״̬��true��ʾ�Ѿ��������ӣ�false��ʾû�н�������
		using CTCPClientVecRecv::GetConnectStatus;

		//	�������״̬��true��ʾ�Ѿ��ɹ����ã�false��ʾû������
		using CTCPClientVecRecv::GetConfigStatus;

		//	�õ��������˿�
		using CTCPClientVecRecv::GetServerPort;

		//	�õ�������IP
		using CTCPClientVecRecv::GetServerIPStr;

		//	�õ����Ե��߳�״̬�������Ӷ��ߵ�ʱ������̻߳���Ȼ�����У���ô˵����ǰ���ڶ���������ʱ��
		using CTCPClientVecRecv::GetThreadStatus;

		//	���SOCKET
		using CTCPClientVecRecv::GetSocket;

		//	������Դ���
		using CTCPClientVecRecv::GetRetryCount;

		//	��öϿ�ʱ�ĳ�ʱ����
		using CTCPClientVecRecv::GetRetrySec;

	public:
		//	���ú�����Ĭ������Ϊ ���ߺ������������ӣ������������Դ������ɹ�����0��ʧ�ܷ���SOCKET_ERROR
		int Config(
			char* remote_ip,										//	Զ��������IP��ַ
			u_short remote_port,									//	Զ�������Ķ˿�
			bool now_conn,											//	�Ƿ������������ӣ�trueΪ���óɹ����������ӣ�falseΪ�������ã�����������
			P_USER_ON_RECV_DATAAREA_CLIENT_FUNC puserfunc,			//	�û�����
			P_USER_ON_TCP_SERVER_BREAK_FUNC ponbreakfunc = NULL,	//	����������Ͽ����ӵ�ʱ�������õ���Ӧ����
			int retry_sec = 0,										//	���������ĳ�ʱ������0��ʾ���ߺ������������ӣ�>0��ʾ���ߺ󾭹���ú����Խ�������
			int retry_count = -1									//	���Դ�����-1��ʾ�������Դ�����0��ʾ���ߺ󲻽����������ӣ�>0��ʾ���ߺ����ԵĴ���
			);

		//	���Ӻ���,If no error occurs, connect returns zero. Otherwise, it returns SOCKET_ERROR, and a specific error code can be retrieved by calling WSAGetLastError.
		int Connect(void);

		//	�Ͽ�����
		void Disconnect(void);

	public:
		//	�޲������캯��
		CintegratedTCPClient();

		//	��������
		~CintegratedTCPClient();
		
		//	���մ�����
		friend DWORD OnRecvCintegratedTCPClient_Func(LPVOID pParam);
	};

	//-------------------------------------------------------------------------------
	/*
		RecvVec�����ڽ����߳����ṩ���û�����ʹ�ã����Բ���ҪCS���������߳�д�룬�û�������ȡ��������߳��ͷš�
		���ͺ�������ڲ�������Դ���û��ڵ��øú���֮ǰ���룬�����øú������ڷ��ͺ����ڲ������ͷ���Դ

		������������
			���ȹ��������յ���������󣬽�����һ����������󣬵����óɹ����������������һ�����մ����̣߳������ཫ����������������б���ʱ�����������̺߳������������ʱ������
		ָ���Ӵ����������̺߳��������������־�����Ҳ��ر��κ����Ӻ��ͷ��κ���Դ��Ȼ�󵱹�����ļ���������⵽�����ʱ�򣬻����������Ĺر����Ӻ�������ʱ�Ż�ر�socket���ͷ���Դ
		����������߹ض����ӵ�ʱ���ȴ������б��в��ң�Ȼ����б����Ƴ���Ȼ��ر����Ӳ��ͷ���Դ��
	*/
	//	�����û��������պ�������
	typedef void (*P_USER_ON_RECV_DATAAREA_SERVER_FUNC)(
		SOCKET,
		std::vector<SDataArea>
		);

	//	ȫ���ɷ�����������
	class CintegratedTCPServerHost :
		protected CTCPServerHost
	{
	private:
		//	�Ͽ���־,tureΪҪ��Ͽ���falseΪû��Ҫ��
		bool Disconnect_flag;

		//	�Ͽ���־��CS
		CMyCriticalSection Disconnect_flag_CS;

	protected:
		//	�Ѿ����յ����ݿ�
		std::vector<SDataArea> RecvVec;

		//	�û���Ӧ���պ���
		P_USER_ON_RECV_DATAAREA_SERVER_FUNC pUserFunc;

		//	��ȡ�˿ڱ�־,tureΪҪ��Ͽ���falseΪû��Ҫ��
		bool GetDisconnectFlag(void);

		//	���öϿ���־,tureΪҪ��Ͽ���falseΪû��Ҫ��
		void SetDisconnectFlag(bool);

	public:
		//	�õ���ǰ������״̬��trueΪ���óɹ���falseΪ����ʧ��
		using CTCPServerHost::GetConfigStatus;

		//	��ȡʧ��״̬
		using CTCPServerHost::GetFailStatus;

	public:
		//	���ú���,�����Ƿ���0�������ʱ�򷵻�SOCKET_ERROR
		int Config(
			SOCKET m_socket,										//	���ӵ�SOCKET
			P_USER_ON_RECV_DATAAREA_SERVER_FUNC puserfunc			//	����ĺ���ָ��
			);

		//	�������ݣ�����������Ϊ������������Ϊ��ͻ��˷�������,�����Ƿ����Ѿ����͵��ֽڣ������ʱ�򷵻�SOCKET_ERROR
		int Send(
			std::vector<SDataArea> in_vec
			);

		//	�Ͽ�����
		void Disconnect(void);
	
	public:
		//	�޲������캯��
		CintegratedTCPServerHost();

		//	��������
		~CintegratedTCPServerHost();

		//	���մ�����
		friend DWORD OnRecvCintegratedTCPServerHost_Func(LPVOID pParam);
	};

	//-------------------------------------------------------------------------------
	//	ȫ���ɷ�����������

	//	TCP������
	class CintegratedTCPServerManage
	{
	public:
		//	���������ڲ������Ͷ���,������Ϣ�ṹ��
		typedef struct tagSConInfo
		{
			SOCKET socket;								//	��ͻ������ӵ�SOCKET���
			SOCKADDR_IN addrClient;						//	�ͻ��˵ĵ�ַ��Ϣ
			CMyTime* pbegin_time;						//	��ʼ�������ӵ�ʱ��
			CintegratedTCPServerHost* pCon;				//	���Ӷ���
		}SConInfo, *pSConInfo;

		//	���������̺߳���
		friend DWORD integrated_TCP_SERVER_MANAGE_ACCEPT_THREAD(LPVOID pParam);

		//	�ҵ������̺߳���
		friend DWORD integrated_TCP_SERVER_MANAGE_FIND_ERROR_THREAD(LPVOID pParam);

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

	private:
		//	�Ͽ���־,tureΪҪ��Ͽ���falseΪû��Ҫ��
		bool Disconnect_ACCEPT_flag;

		//	�Ͽ���־��CS
		CMyCriticalSection Disconnect_ACCEPT_flag_CS;

		//	�Ͽ���־,tureΪҪ��Ͽ���falseΪû��Ҫ��
		bool Disconnect_FIND_ERROR_flag;

		//	�Ͽ���־��CS
		CMyCriticalSection Disconnect_FIND_ERROR_flag_CS;

	protected:
		//	��ȡ�˿ڱ�־,tureΪҪ��Ͽ���falseΪû��Ҫ��
		bool GetDisconnectACCEPTFlag(void);

		//	���öϿ���־,tureΪҪ��Ͽ���falseΪû��Ҫ��
		void SetDisconnectACCEPTFlag(bool);

		//	��ȡ�˿ڱ�־,tureΪҪ��Ͽ���falseΪû��Ҫ��
		bool GetDisconnectFINDERRORFlag(void);

		//	���öϿ���־,tureΪҪ��Ͽ���falseΪû��Ҫ��
		void SetDisconnectFINDERRORFlag(bool);

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
			SOCKET in_socket,										//	�Ѿ��ɹ��������ӵ�SOCKET��Ҳ��Ψһ�ı�ʶ
			SOCKADDR_IN	addrClient,									//	�����ӿͻ��˵ĵ�ַ��Ϣ
			P_USER_ON_RECV_DATAAREA_SERVER_FUNC precvfunc			//	�û�����Ľ��պ���
			);

		//	��������������ɾ��һ�����󣬳ɹ�����true,ʧ�ܷ���false
		bool RemoveConList(
			SOCKET in_socket							//	ɾ��ָ����SOCKET
			);

		//	�Զ����б����Ƴ����������
		void AutoRemoveError(void);

		//	�û��Ľ��մ�����
		P_USER_ON_RECV_DATAAREA_SERVER_FUNC pRecvFunc;

		//	����ͻ��˽�����Ӧ����
		P_USER_ON_TCP_CLIENT_JOIN_FUNC pOnJoinFunc;

		//	����ͻ��˶Ͽ���Ӧ����
		P_USER_ON_TCP_CLIENT_BREAK_FUNC pOnBreadFunc;

	public:
		//	�޲������캯��
		CintegratedTCPServerManage();

		//	��������
		~CintegratedTCPServerManage();

		//	��ʼ���������ɹ�����true��ʧ�ܷ���false
		bool Config(
			CTCPServerResources& rCTCPServerResources,					//	��������Դ������
			P_USER_ON_RECV_DATAAREA_SERVER_FUNC pfunc					//	�û����պ���
			);

		//	�������ӺͶϿ���Ӧ�����ĳ�ʼ�����ú������ɹ�����true��ʧ�ܷ���false
		bool Config(
			CTCPServerResources& rCTCPServerResources,					//	��������Դ������
			P_USER_ON_RECV_DATAAREA_SERVER_FUNC pfunc,					//	�û����պ���	
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
			std::vector<SDataArea> in_vec								//	���͵����ݿ�
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

	//-------------------------------------------------------------------------------
};//	CMonitorProtocol�����ռ����

///////////////////////////////////////////////////////////////////////////////////////
#endif	//	__CMonitorProtocol_h__

