/**************************************************************************************

	工程名称：监控协议类
	创建日期：2017.03.13
	最后修改：2017.03.31
	程序版本：REV 0.5
	设计编写：rainhenry
	开发平台：VS2013 Windows7

	版本修订：
		0.1版本			创建工程，完成了客户端类
		0.2版本			增加服务器类
		0.3版本			增加集成TCP底层的客户端类
		0.4版本			增加集成TCP底层的服务器连接类
						增加集成TCP底层的服务器管理类
		0.5版本			修复当在获取数据区的时候出现问题的时候将错误调用用户函数的问题
						修复当在获取数据区的时候出现问题的时候导致的内存泄漏问题

	设计目的：
		1、设计为主从模式
		2、由于先期只考虑一对一台机器的情况，所以采用简单设计

	功能原理：
		首先服务器端的程序发送截屏请求
		然后客户端的接收程序检查收到的指令是否为截屏请求指令
		指令正确后，调用用户截屏程序，并获得截屏之后的数据
		然后用户需要调用处理完之后的回执数据再发送回服务器端

	基本协议格式

	[请求命令]各部分定义，该命令方向是 服务器 -> 客户端
	===========================================================================================
	部分名称			|	字节数目		|				用途含义				|		默认值
	----------------+---------------+-----------------------------------+----------------------
	数据头			|		4		|	用于识别数据						|	28 77 93 5A	(HEX)
	----------------+---------------+-----------------------------------+----------------------
	命令ID			|		2		|	用于区分相邻的命令，和校验命令		|	0~65536 (u_short)
	----------------+---------------+-----------------------------------+----------------------
	命令ID反码		|		2		|	命令ID的按位取反，用于校验		|	0~65536 (u_short)
	----------------+---------------+-----------------------------------+----------------------
	数据区数量		|		4		|	后续的数据区个数					|			>=1
	----------------+---------------+-----------------------------------+----------------------
	请求数据长度		|		4		|	用户请求数据的字节数目			|		
	----------------+---------------+-----------------------------------+----------------------
	用户请求数据		|		0~n		|	提供用户扩展						|		
	----------------+---------------+-----------------------------------+----------------------
	请求数据长度		|		4		|	用户请求数据的字节数目			|		（可选）
	----------------+---------------+-----------------------------------+----------------------
	用户请求数据		|		0~n		|	提供用户扩展						|		（可选）
	===========================================================================================

	[回执命令]各部分定义，该命令的方向是 客户端 -> 服务器
	===========================================================================================
	部分名称			|	字节数目		|				用途含义				|		默认值
	----------------+---------------+-----------------------------------+----------------------
	数据头			|		4		|	用于识别数据						|	79 32 87 5A	(HEX)
	----------------+---------------+-----------------------------------+----------------------
	命令ID			|		2		|	用于区分相邻的命令，和校验命令		|	0~65536 (u_short)
	----------------+---------------+-----------------------------------+----------------------
	命令ID反码		|		2		|	命令ID的按位取反，用于校验		|	0~65536 (u_short)
	----------------+---------------+-----------------------------------+----------------------
	数据区数量		|		4		|	后续的数据区个数					|			>=1	
	----------------+---------------+-----------------------------------+----------------------
	回执数据长度		|		4		|	用户回执数据的字节数目			|		
	----------------+---------------+-----------------------------------+----------------------
	用户回执数据		|		0~n		|	提供用户扩展						|		
	----------------+---------------+-----------------------------------+----------------------
	回执数据长度		|		4		|	用户回执数据的字节数目			|		（可选）
	----------------+---------------+-----------------------------------+----------------------
	用户回执数据		|		0~n		|	提供用户扩展						|		（可选）
	===========================================================================================

	所以可以知道一个数据帧的长度至少要大于等于 4+2+2+4 = 12个字节

	数据流程图

	客户端																								服务器
	--------------------------------------------------------------------------------------------------------------------------------------------------------
	网络接收响应函数调用该类对象的OnRecvHandle方法									<<============		发送请求数据（带有至少1个数据区）
		||
	    \/
	格式解析正确后，该类对象自动调用用户处理程序（回执数据至少含有1个数据区）
	获得回执数据，并且该类对象将自动回执数据打包
	    ||
		\/
	用户用GetReturnData方法获得已经打包好的回执数据
	调用网络相关的发送方法将数据回执到服务器										=============>>		网络接收响应函数调用该类对象的OnRecvHandle方法
																										||
																										\/
																									格式解析正确后，该类对象自动调用用户处理程序，处理回执数据
																										||
																										\/
																									此时完成一次数据传输
	---------------------------------------------------------------------------------------------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////////////////////

客户端使用举例：

//	监控协议接收数据处理函数
std::vector<CMonitorProtocol::SDataArea> OnRecvData(
	std::vector<CMonitorProtocol::SDataArea> in_vec							//	入口参数，请求时的数据区
	)
{
	//	in_vec为来自服务器请求命令中的数据区
	//	函数体返回的是将要回执给服务器的数据区
	//	切记不可直接 return in_vec; 操作！！ 否则同一个内存区域将会释放两次导致严重错误
	//	构造返回的std::vector<CMonitorProtocol::SDataArea>对象的时候，内部的pdat要采用new方法申请，本实例在使用完成后会自动释放
	//	切记用户不可以自己释放该函数的入口和出口的pdat数据！！本类会完成自动的释放。
}

//	监控协议实例
CMonitorProtocol::CClient myclient_protocol;

//	然后在网络类的接收处理函数中可以这样调用OnRecvHandle方法
void OnTCPRecv(
	char* in_buf,								//	数据缓冲区首地址
	int	length									//	收到的字节数目
	)
{
	//	解析命令
	bool re = myclient_protocol.OnRecvHandle(
		in_buf,
		length
		);

	//	若解析成功
	if (re == true)
	{
		myTCPCLient.TrySend(
			(char*)(myclient_protocol.GetReturnData()),
			myclient_protocol.GetReturnDataSize()
			);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
	这里由于传统的将协议与TCP底层分开的话的使用方法复杂，并且存在设计缺陷，现在已经更改为全集成方式的客户端与服务器类
	其中服务器相关的资源类保持不变，用户需要使用的管理类已经改为全集成的方式

	全集成的服务器使用说明：

	//	监控协议的服务器接收处理函数
	void OnRecvUserData(
		SOCKET in_socket,														//	输入的SOCKET，表示从哪一个连接来的数据
		std::vector<CMonitorProtocol::SDataArea> in_vec							//	入口参数，来自客户端的回执数据区
		)
	{
		//	这里为处理in_vec数据块，该数据来自客户端的回执
		//	使用最后一定不要释放in_vec里面的缓存，因为全集成服务器类会自动释放
	}

	//	TCP连接接入的响应函数
	void OnTCPJoin(
		SOCKET in_socket,								//	成功建立连接的SOCKET句柄
		SOCKADDR_IN ClientAddr,							//	客户端信息
		CMyTime	begin_time								//	成功建立连接的时间点
		)
	{
		//	用户代码...
	}

	//	TCP连接断开的响应函数
	void OnTCPBreak(
		SOCKET in_socket,								//	断开连接之前的SOCKET句柄
		SOCKADDR_IN ClientAddr,							//	断开连接之前的客户端信息
		CMyTime	end_time								//	断开连接的时间点
		)
	{
		//	用户代码...
	}

	//	定义TCP服务器资源对象
	CTCPServerResources myTCPServerRes;

	//	定义TCP服务器管理对象
	CMonitorProtocol::CintegratedTCPServerManage myTCPManage;

	//	配置监听的端口
	myTCPServerRes.Config(1234);

	//	配置全集成TCP服务器管理类
	myTCPManage.Config(
		myTCPServerRes,					//	指定的本机资源
		OnRecvUserData,					//	接收数据响应函数
		OnTCPJoin,						//	接入响应函数
		OnTCPBreak						//	断开响应函数
		);

	//	发送数据
	myTCPManage.Send(
		socket,							//	用户指定的SOCKET
		tmp_vec							//	要发送的数据块
		);
		
	//	停止服务器
	myTCPServerRes.Release();

	//	遍历当前所有已经接入的客户端连接
	//	获取连接副本
	std::vector<CMonitorProtocol::CintegratedTCPServerManage::SReConInfo> sockcopy = mTCPManage.GetConListAllInfoCopy();

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


///////////////////////////////////////////////////////////////////////////////////////
	tcp全集成客户端的使用
	
	//	TCP客户端实例
	CMonitorProtocol::CintegratedTCPClient myClient;

	//	定义用户接收处理函数
	std::vector<CMonitorProtocol::SDataArea> OnRecvData(
		std::vector<CMonitorProtocol::SDataArea> in_vec							//	入口参数，请求时的数据区
		)
	{
		//	用户代码，返回std::vector<CMonitorProtocol::SDataArea>
		//	in_vec为来自服务器请求命令中的数据区
		//	函数体返回的是将要回执给服务器的数据区
		//	切记不可直接 return in_vec; 操作！！ 否则同一个内存区域将会释放两次导致严重错误
		//	构造返回的std::vector<CMonitorProtocol::SDataArea>对象的时候，内部的pdat要采用new方法申请，本实例在使用完成后会自动释放
		//	切记用户不可以自己释放该函数的入口和出口的pdat数据！！本类会完成自动的释放。
	}

	//	服务器异常断开响应函数
	void OnServerBreak(
		SOCKET in_socket,								//	连接时的socket
		SOCKADDR_IN ServerAddr,							//	服务器的地址信息
		int fail_count									//	已经失败的次数
		)
	{
		//	用户代码。。。。
	}
							
	//	配置网络
	myClient.Config(
		"192.168.137.10",
		1234,
		true,
		OnRecvData,
		OnServerBreak,
		0,												//	断线重连的超时秒数，0表示断线后立即进行连接，>0表示断线后经过多久后重试建立连接
		0												//	重试次数，-1表示无限重试次数，0表示断线后不进行重试连接，>0表示断线后重试的次数
	);

	//	断开连接
	myClient.Disconnect();

**************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////
//	重定义保护
#ifndef __CMonitorProtocol_h__
#define __CMonitorProtocol_h__

///////////////////////////////////////////////////////////////////////////////////////
//	包含头文件
#include <vector> 
#include "../UDPTCP/UDPTCP.h"

///////////////////////////////////////////////////////////////////////////////////////
//	定义命名空间
namespace CMonitorProtocol
{
	//-------------------------------------------------------------------------------
	//	最小数据尺寸
	extern const int MIN_DATA_SIZE;

	//	定义数据区格式结构体
	typedef struct tagSDataArea
	{
		void* pdat;							//	数据区的首地址
		int length;							//	数据长度
	}SDataArea;


	//	定义用户函数类型，当客户端收到来自服务器的数据区，经过用户处理后，需要返回要送往服务器的回执数据区
	typedef std::vector<SDataArea>(*P_USER_CMP_CLIENT_FUNC)(
		std::vector<SDataArea>							//	入口参数，请求时的数据区
		);

	//	定义用户函数类型，当服务器收到客户端的回执数据，提供给用户进行处理
	typedef void(*P_USER_CMP_SERVER_FUNC)(
		std::vector<SDataArea>							//	入口参数，来自客户端的回执数据区
		);

	//-------------------------------------------------------------------------------
	/*
	主要共有方法有：
		配置、接收来自服务器的截图命令
	私有方法：
		回执图像数据到服务器
	*/
	//	客户端类，即被监控的机器
	class CClient
	{
	private:
		//	用户定义的截屏函数指针
		P_USER_CMP_CLIENT_FUNC pUserFunc;

		//	回执数据首地址和长度，已经打包的
		void* pReturnData;
		int ReturnDataSize;

		//	回执数据区
		std::vector<SDataArea> ReturnDataVec;

	public:
		//	无参数构造函数
		CClient();

		//	析构函数
		~CClient();

		//	配置函数，返回true表示配置成功，false表示配置失败
		bool Config(
			P_USER_CMP_CLIENT_FUNC puserfunc					//	传递的用户函数指针
			);

		//	接收响应函数，返回true表示成功收到可以执行的命令，返回false表示无法匹配命令，这里仅仅是协议上的匹配，与用户数据区的内容无关
		bool OnRecvHandle(
			void* in_dat,										//	收到的数据首地址
			int length											//	数据长度
			);

		//	得到解析处理后的回执数据首地址
		void* GetReturnData(void);

		//	得到回执数据的长度
		int GetReturnDataSize(void);

	};

	//-------------------------------------------------------------------------------
	//	服务器类
	class CServer
	{
	private:
		//	用户定义的接收处理函数
		P_USER_CMP_SERVER_FUNC pUserFunc;

		//	发送数据缓冲区首地址
		void* pSendDataBuff;

		//	发送数据的长度
		int SendDataBuffSize;

		//	发送时的命令ID序列
		unsigned short cmd_id;

	public:
		//	无参数构造函数
		CServer();

		//	析构函数
		~CServer();

		//	配置函数，配置成功返回true，失败返回false
		bool Config(
			P_USER_CMP_SERVER_FUNC puserfunc				//	输入的用户函数指针
			);

		//	构造发送数据，构造成功返回true,失败返回false
		bool BuildSendData(
			std::vector<SDataArea> in_vec
			);

		//	接收响应函数，返回true表示成功收到可以执行的命令，返回false表示无法匹配命令，这里仅仅是协议上的匹配，与用户数据区的内容无关
		bool OnRecvHandle(
			void* in_dat,										//	收到的数据首地址
			int length											//	数据长度
			);

		//	获得发送数据的缓冲区首地址
		void* GetSendDataBuff(void);

		//	获得发送数据的缓冲区长度
		int GetSendDataBuffSize(void);
	};

	//-------------------------------------------------------------------------------
	/*
		全集成的客户端类的数据处理流程
			设计该类的目的是为了整合原有TCP客户端类和协议客户端类之间的整合问题，而且原有设计采用的面向数据长度的通信机制，与socket的流通信相违背，导致通信严重错误
		为了从根本上解决这一问题，要从解析协议的时候就以流的思想去进行拆包分析数据流。原有的类里面的断线与重连机制和其他处理都是可以使用的，而且效果也不错。所以这里
		以派生的方式重载一些函数和方法实现全集成的功能。主要需要重载的有：

		收到数据时的处理的函数OnRecvCintegratedTCPClient_Func
			该函数为全局型的函数，也为CintegratedTCPClient类的友元函数。可以访问本类的protected成员。该函数由本类的Connect方法调用线程类启动。启动后内部有一个大
		循环，其主要工作任务是根据协议的需要调用recv函数读取数据，内存的申请方式采用new方式。初步计划内部采用单层循环的方式实现，然后将短线重连部分归结到一个状态里面。

		这里要说明一下RecvVec。该数据由OnRecvCintegratedTCPClient_Func线程在解析recv数据时写入，然后又由OnRecvCintegratedTCPClient_Func线程调用用户函数读取，最后
		由OnRecvCintegratedTCPClient_Func线程释放内存。所以该数据仅仅在一个线程中进行读写，所以不需要CS进行保护。

		关于终止线程的方式，由于在线程中有申请内存和释放内存的成对过程，二者缺一不可。所以线程的结束方式不可以为直接用线程类强制终止，必须先关闭socket连接，然后recv返
		回错误代码，然后程序再进行判断是否是其他地方发出线程终止命令，然后释放内存，此时线程安全退出。

		数据处理程序流程
			
			外部大循环开始  
			||
			\/
			清空错误标志
			||
			\/
			读取1个字节，判断是否正确，正确继续执行，错误设置标志执行continue语句
			||
			...之后的3个字节也同上处理
			||
			\/
			读取4个字节获取ID和ID反码，出错的话设置标志执行continue语句
			||
			\/
			读取4个字节获取数据块的个数,并检查读取返回值是否正确，错误设置标志，执行continue语句
			||
			\/
			进入内部小循环，该循环每个循环周期表示一个数据区块<<==========++
					||												  /\
					\/												  ||
					读取4个字节，获得当前数据区的长度					  ||
					并检查读取返回值是否正确，						  ||
					错误设置标志，执行continue语句					  ||
					||												  ||
					\/												  ||
					读取n个字节，获取当前数据区						  ||
					并检查读取返回值是否正确，						  ||
					错误设置标志，执行continue语句					  ||
					||												  ||
					\/												  ||循环完一次，判断数据区是否还有数据
					将数据构造进入RecvVec成员，采用new方法============>>++
							||
							||
							\/循环跳出，已经遍历完数据区
			++<<============++
			||
			||此时已回到外部的大循环位置
			\/
			调用用户接收数据处理函数pUserFunc，获得用户返回的回执数据区
			||
			\/
			遍历释放RecvVec内部资源
			||
			\/
			将回执数据区发送到服务器过程
						||
						\/
						计算一共需要发送多少字节，并申请发送缓存
						||
						\/
						构造数据头
						||
						\/
						构造ID和ID反码
						||
						\/
						构造数据区个数
						||
						\/						
						循环构造数据区，遍历回执数据的vec <<========================++
								||												  /\
								\/												  ||
								配置当前数据区的长度								  ||
								||												  ||
								\/												  ||循环完一次，判断VEC是否还有数据
								拷贝当前数据区===================================>>++
										||
										||
										\/循环跳出，已经遍历完数据区
						++<<============++
						||
						||此时已回到“将回执数据区发送到服务器过程”
						\/
						执行发送，并检查发送是否正确，错误设置标志，执行continue语句
						||
						\/“将回执数据区发送到服务器过程”执行完成
			++<<========++
			||
			||此时已回到外部的大循环的最后位置
			\/
			判断是否有错误标志，该错误分为两种性质，没有错误的代码为0，
			一种是由于收到的数据本身错误，但是执行接收和发送函数没有问题，即数据错误，此时只需要重新获取正确数据即可，数据错误代码为1
			另一种是接收或者发送函数出现问题，即连接问题，一般表现为服务器断线等问题。此时需要断线重连机制的处理等，连接错误代码为2
			如果为连接错误，继续处理断线问题，执行continue语句
			||
			\/
			断线重连程序与错误提示等保持与基类一样的处理方式，即把代码直接复制过来就可以，这里流程省略
			||
			\/
			外部大循环结束
			
	*/
	//	定义用户函数接收函数类型
	typedef std::vector<SDataArea>(*P_USER_ON_RECV_DATAAREA_CLIENT_FUNC)(
		std::vector<SDataArea>
		);

	//	全集成的客户端类定义
	class CintegratedTCPClient:
		protected CTCPClientVecRecv
	{
	private:
		//	断开标志,ture为要求断开，false为没有要求
		bool Disconnect_flag;

		//	断开标志的CS
		CMyCriticalSection Disconnect_flag_CS;

	protected:
		//	已经接收的数据块
		std::vector<SDataArea> RecvVec;

		//	回执给服务器的数据块
		std::vector<SDataArea> ReturnVec;

		//	用户接收处理函数
		P_USER_ON_RECV_DATAAREA_CLIENT_FUNC pUserFunc;

		//	获取端口标志,ture为要求断开，false为没有要求
		bool GetDisconnectFlag(void);

		//	设置断开标志,ture为要求断开，false为没有要求
		void SetDisconnectFlag(bool);

	public:
		//	断开函数
		using CTCPClientVecRecv::Disconnect;

		//	返回连接状态，true表示已经建立连接，false表示没有建立连接
		using CTCPClientVecRecv::GetConnectStatus;

		//	获得配置状态，true表示已经成功配置，false表示没有配置
		using CTCPClientVecRecv::GetConfigStatus;

		//	得到服务器端口
		using CTCPClientVecRecv::GetServerPort;

		//	得到服务器IP
		using CTCPClientVecRecv::GetServerIPStr;

		//	得到重试的线程状态，在连接断线的时候，如果线程还依然在运行，那么说明当前处于断线重连的时候
		using CTCPClientVecRecv::GetThreadStatus;

		//	获得SOCKET
		using CTCPClientVecRecv::GetSocket;

		//	获得重试次数
		using CTCPClientVecRecv::GetRetryCount;

		//	获得断开时的超时秒数
		using CTCPClientVecRecv::GetRetrySec;

	public:
		//	配置函数，默认配置为 断线后立即重试连接，并且无限重试次数，成功返回0，失败返回SOCKET_ERROR
		int Config(
			char* remote_ip,										//	远程主机的IP地址
			u_short remote_port,									//	远程主机的端口
			bool now_conn,											//	是否立即进行连接，true为配置成功后立即连接，false为仅仅配置，不进行连接
			P_USER_ON_RECV_DATAAREA_CLIENT_FUNC puserfunc,			//	用户函数
			P_USER_ON_TCP_SERVER_BREAK_FUNC ponbreakfunc = NULL,	//	当与服务器断开连接的时候，所调用的响应函数
			int retry_sec = 0,										//	断线重连的超时秒数，0表示断线后立即进行连接，>0表示断线后经过多久后重试建立连接
			int retry_count = -1									//	重试次数，-1表示无限重试次数，0表示断线后不进行重试连接，>0表示断线后重试的次数
			);

		//	连接函数,If no error occurs, connect returns zero. Otherwise, it returns SOCKET_ERROR, and a specific error code can be retrieved by calling WSAGetLastError.
		int Connect(void);

		//	断开函数
		void Disconnect(void);

	public:
		//	无参数构造函数
		CintegratedTCPClient();

		//	析构函数
		~CintegratedTCPClient();
		
		//	接收处理函数
		friend DWORD OnRecvCintegratedTCPClient_Func(LPVOID pParam);
	};

	//-------------------------------------------------------------------------------
	/*
		RecvVec仅仅在接收线程中提供给用户程序使用，所以不需要CS保护，由线程写入，用户函数读取，最后由线程释放。
		发送函数的入口参数的资源由用户在调用该函数之前申请，当调用该函数后，在发送函数内部进行释放资源

		基本工作流程
			首先管理类在收到连接请求后，将建立一个连接类对象，当配置成功后连接类对象启动一个接收处理线程，管理类将该连接类加入连接列表，此时如果连接类的线程函数检测出错误的时候，这里
		指连接错误。连接类线程函数将发出错误标志，并且不关闭任何连接和释放任何资源，然后当管理类的检查错误函数检测到错误的时候，会调用连接类的关闭连接函数，此时才会关闭socket和释放资源
		当服务器这边关断连接的时候，先从连接列表中查找，然后从列表中移除，然后关闭连接并释放资源。
	*/
	//	定义用户函数接收函数类型
	typedef void (*P_USER_ON_RECV_DATAAREA_SERVER_FUNC)(
		SOCKET,
		std::vector<SDataArea>
		);

	//	全集成服务器连接类
	class CintegratedTCPServerHost :
		protected CTCPServerHost
	{
	private:
		//	断开标志,ture为要求断开，false为没有要求
		bool Disconnect_flag;

		//	断开标志的CS
		CMyCriticalSection Disconnect_flag_CS;

	protected:
		//	已经接收的数据块
		std::vector<SDataArea> RecvVec;

		//	用户响应接收函数
		P_USER_ON_RECV_DATAAREA_SERVER_FUNC pUserFunc;

		//	获取端口标志,ture为要求断开，false为没有要求
		bool GetDisconnectFlag(void);

		//	设置断开标志,ture为要求断开，false为没有要求
		void SetDisconnectFlag(bool);

	public:
		//	得到当前的配置状态，true为配置成功，false为配置失败
		using CTCPServerHost::GetConfigStatus;

		//	获取失败状态
		using CTCPServerHost::GetFailStatus;

	public:
		//	配置函数,正常是返回0，错误的时候返回SOCKET_ERROR
		int Config(
			SOCKET m_socket,										//	连接的SOCKET
			P_USER_ON_RECV_DATAAREA_SERVER_FUNC puserfunc			//	传入的函数指针
			);

		//	发送数据，由于所在类为服务器，所以为向客户端发送数据,正常是返回已经发送的字节，错误的时候返回SOCKET_ERROR
		int Send(
			std::vector<SDataArea> in_vec
			);

		//	断开连接
		void Disconnect(void);
	
	public:
		//	无参数构造函数
		CintegratedTCPServerHost();

		//	析构函数
		~CintegratedTCPServerHost();

		//	接收处理函数
		friend DWORD OnRecvCintegratedTCPServerHost_Func(LPVOID pParam);
	};

	//-------------------------------------------------------------------------------
	//	全集成服务器管理类

	//	TCP管理类
	class CintegratedTCPServerManage
	{
	public:
		//	连接容器内部的类型定义,连接信息结构体
		typedef struct tagSConInfo
		{
			SOCKET socket;								//	与客户端连接的SOCKET句柄
			SOCKADDR_IN addrClient;						//	客户端的地址信息
			CMyTime* pbegin_time;						//	开始建立连接的时间
			CintegratedTCPServerHost* pCon;				//	连接对象
		}SConInfo, *pSConInfo;

		//	监听处理线程函数
		friend DWORD integrated_TCP_SERVER_MANAGE_ACCEPT_THREAD(LPVOID pParam);

		//	找到错误线程函数
		friend DWORD integrated_TCP_SERVER_MANAGE_FIND_ERROR_THREAD(LPVOID pParam);

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

	private:
		//	断开标志,ture为要求断开，false为没有要求
		bool Disconnect_ACCEPT_flag;

		//	断开标志的CS
		CMyCriticalSection Disconnect_ACCEPT_flag_CS;

		//	断开标志,ture为要求断开，false为没有要求
		bool Disconnect_FIND_ERROR_flag;

		//	断开标志的CS
		CMyCriticalSection Disconnect_FIND_ERROR_flag_CS;

	protected:
		//	获取端口标志,ture为要求断开，false为没有要求
		bool GetDisconnectACCEPTFlag(void);

		//	设置断开标志,ture为要求断开，false为没有要求
		void SetDisconnectACCEPTFlag(bool);

		//	获取端口标志,ture为要求断开，false为没有要求
		bool GetDisconnectFINDERRORFlag(void);

		//	设置断开标志,ture为要求断开，false为没有要求
		void SetDisconnectFINDERRORFlag(bool);

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
			SOCKET in_socket,										//	已经成功建立连接的SOCKET，也是唯一的标识
			SOCKADDR_IN	addrClient,									//	已连接客户端的地址信息
			P_USER_ON_RECV_DATAAREA_SERVER_FUNC precvfunc			//	用户定义的接收函数
			);

		//	从连接容器里面删除一个对象，成功返回true,失败返回false
		bool RemoveConList(
			SOCKET in_socket							//	删除指定的SOCKET
			);

		//	自动从列表中移除错误的连接
		void AutoRemoveError(void);

		//	用户的接收处理函数
		P_USER_ON_RECV_DATAAREA_SERVER_FUNC pRecvFunc;

		//	定义客户端接入响应函数
		P_USER_ON_TCP_CLIENT_JOIN_FUNC pOnJoinFunc;

		//	定义客户端断开响应函数
		P_USER_ON_TCP_CLIENT_BREAK_FUNC pOnBreadFunc;

	public:
		//	无参数构造函数
		CintegratedTCPServerManage();

		//	析构函数
		~CintegratedTCPServerManage();

		//	初始化函数，成功返回true，失败返回false
		bool Config(
			CTCPServerResources& rCTCPServerResources,					//	服务器资源的引用
			P_USER_ON_RECV_DATAAREA_SERVER_FUNC pfunc					//	用户接收函数
			);

		//	带有连接和断开响应函数的初始化配置函数，成功返回true，失败返回false
		bool Config(
			CTCPServerResources& rCTCPServerResources,					//	服务器资源的引用
			P_USER_ON_RECV_DATAAREA_SERVER_FUNC pfunc,					//	用户接收函数	
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
			std::vector<SDataArea> in_vec								//	发送的数据块
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

	//-------------------------------------------------------------------------------
};//	CMonitorProtocol命名空间结束

///////////////////////////////////////////////////////////////////////////////////////
#endif	//	__CMonitorProtocol_h__

