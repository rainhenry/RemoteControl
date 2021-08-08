
//	REV 0.5

#include "stdafx.h"
#include "CMonitorProtocol.h"

//	最小数据尺寸
const int CMonitorProtocol::MIN_DATA_SIZE = 12;

//	无参数构造函数
CMonitorProtocol::CClient::CClient()
{
	this->pUserFunc = NULL;
	this->pReturnData = NULL;
	this->ReturnDataSize = 0;
}

//	析构函数
CMonitorProtocol::CClient::~CClient()
{
	//	检查回执缓冲区是否需要释放
	if (this->pReturnData != NULL)
	{
		this->ReturnDataSize = 0;
		delete[](this->pReturnData);
		this->pReturnData = NULL;
	}

	//	检查释放回执数据区
	for (int i = 0; (this->ReturnDataVec.begin() + i) != (this->ReturnDataVec.end()); ++i)
	{
		(*(this->ReturnDataVec.begin() + i)).length = 0;
		if ((*(this->ReturnDataVec.begin() + i)).pdat != NULL)
		{
			delete[]((*(this->ReturnDataVec.begin() + i)).pdat);
		}
		this->ReturnDataVec.erase(this->ReturnDataVec.begin() + i);
		--i;
	}
}

//	配置函数，返回true表示配置成功，false表示配置失败
bool CMonitorProtocol::CClient::Config(
	P_USER_CMP_CLIENT_FUNC puserfunc					//	传递的用户函数指针
	)
{
	//	检查传入参数
	if ((puserfunc == NULL) || (puserfunc == (P_USER_CMP_CLIENT_FUNC)(-1)))
	{
		return false;
	}

	//	配置用户定义函数指针
	this->pUserFunc = puserfunc;

	//	返回成功
	return true;
}

/*
	内部基本执行原理
	首先检查输入参数
	然后解析格式是否匹配
	调用用户函数获得返回数据
	构造回执数据区
*/
//	接收响应函数，返回true表示成功收到可以执行的命令，返回false表示无法匹配命令，这里仅仅是协议上的匹配，与用户数据区的内容无关
bool CMonitorProtocol::CClient::OnRecvHandle(
	void* in_dat,										//	收到的数据首地址
	int length											//	数据长度
	)
{
	//	检查输入参数
	if ((in_dat == NULL) || (in_dat == (void*)(-1)) || (length < MIN_DATA_SIZE))
	{
		return false;
	}

	//	检查用户函数是否可用
	if ((this->pUserFunc == NULL) || (this->pUserFunc == (P_USER_CMP_CLIENT_FUNC)(-1)))
	{
		return false;
	}


	//	解析格式
	//	检查数据头
	if (
		(*((unsigned char*)in_dat + 0) != (unsigned char)0x28) ||
		(*((unsigned char*)in_dat + 1) != (unsigned char)0x77) ||
		(*((unsigned char*)in_dat + 2) != (unsigned char)0x93) ||
		(*((unsigned char*)in_dat + 3) != (unsigned char)0x5A)
		)
	{
		return false;
	}

	//	检查命令ID
	//	获得命令ID
	unsigned char cmd_id_h = *((unsigned char*)in_dat + 5);
	unsigned char cmd_id_l = *((unsigned char*)in_dat + 4);
	u_short cmd_ID = cmd_id_h * 0x100 + cmd_id_l;

	//	获得命令ID反码
	unsigned char inv_cmd_id_h = *((unsigned char*)in_dat + 7);
	unsigned char inv_cmd_id_l = *((unsigned char*)in_dat + 6);
	u_short inv_cmd_ID = inv_cmd_id_h * 0x100 + inv_cmd_id_l;

	//	计算反码
	inv_cmd_ID = ~inv_cmd_ID;

	//	对比反码
	if (cmd_ID != inv_cmd_ID)
	{
		return false;
	}

	//	获得数据区数量
	unsigned char data_area_buff[4];
	memcpy_s(data_area_buff, 4, (unsigned char*)in_dat + 8, 4);
	int DataAreaCount = *((int*)data_area_buff);

	//	检查数据区数量
	if (DataAreaCount < 1)
	{
		return false;
	}

	//	构造请求数据区
	std::vector<SDataArea> RequestDataVec;
	int ByteCount = 0;
	for (int i = 0; i < DataAreaCount; ++i)
	{
		//	获取该数据块的数据长度
		unsigned char data_length_buff[4];
		memcpy_s(data_length_buff, 4, (unsigned char*)in_dat + ByteCount + MIN_DATA_SIZE, 4);
		int DataLength = *((int*)data_length_buff);

		//	检查数据长度，出现严重错误
		if (DataLength <= 0)
		{
			//	释放资源
			for (int i = 0; (RequestDataVec.begin() + i) != (RequestDataVec.end()); ++i)
			{
				delete[]((*(RequestDataVec.begin() + i)).pdat);
				RequestDataVec.erase(RequestDataVec.begin() + i);
				--i;
			}

			//	返回失败
			return false;
		}

		//	构造临时数据区
		char* p_tmp_dat = new char[DataLength];

		//	拷贝数据
		memcpy_s(p_tmp_dat, DataLength, (unsigned char*)in_dat + ByteCount + MIN_DATA_SIZE + 4, DataLength);

		//	构造数据块结构
		SDataArea tmp_dat_area;
		tmp_dat_area.length = DataLength;
		tmp_dat_area.pdat = p_tmp_dat;

		//	插入容器
		RequestDataVec.insert(RequestDataVec.end(), tmp_dat_area);

		//	累计字节
		ByteCount = ByteCount + DataLength + 4;
	}

	//	在获得返回数据之前，要把之前用户申请的内存空间释放掉
	for (int i = 0; (this->ReturnDataVec.begin() + i) != (this->ReturnDataVec.end()); ++i)
	{
		//	检查用户申请的内存是否合法
		if (((*(this->ReturnDataVec.begin() + i)).pdat == NULL) || ((*(this->ReturnDataVec.begin() + i)).pdat == (void*)(-1)))
		{
			//	出现严重错误
			AfxMessageBox(_T("CMonitorProtocol严重使用错误！用户函数中使用了非法指针！！"));
			::abort();
			return false;
		}

		//	释放内存
		delete[]((*(this->ReturnDataVec.begin() + i)).pdat);
		this->ReturnDataVec.erase(this->ReturnDataVec.begin() + i);
		--i;
	}

	//	调用用户函数，获得返回数据
	this->ReturnDataVec = this->pUserFunc(RequestDataVec);

	//	在用户函数之后，检查用户传递的指针是否正确
	for (int i = 0; (this->ReturnDataVec.begin() + i) != (this->ReturnDataVec.end()); ++i)
	{
		//	检查用户申请的内存是否合法
		if (((*(this->ReturnDataVec.begin() + i)).pdat == NULL) || ((*(this->ReturnDataVec.begin() + i)).pdat == (void*)(-1)))
		{
			//	出现严重错误
			AfxMessageBox(_T("CMonitorProtocol严重使用错误！用户函数中使用了非法指针！！"));
			::abort();
			return false;
		}
	}

	//	释放资源
	for (int i = 0; (RequestDataVec.begin() + i) != (RequestDataVec.end()); ++i)
	{
		delete[]((*(RequestDataVec.begin()+i)).pdat);
		RequestDataVec.erase(RequestDataVec.begin() + i);
		--i;
	}

	//-----------------------------------------------------------------------------------------
	//	构造回执数据区
	//	计算数据一共需要多少字节空间
	int return_data_bytes_count = MIN_DATA_SIZE;
	for (int i = 0; (this->ReturnDataVec.begin() + i) != (this->ReturnDataVec.end()); ++i)
	{
		return_data_bytes_count = return_data_bytes_count + (*(this->ReturnDataVec.begin() + i)).length;
	}
	return_data_bytes_count = return_data_bytes_count + this->ReturnDataVec.size() * 4;

	//	配置返回数据缓冲区，如果上一次的结果还存在
	this->ReturnDataSize = 0;
	if (this->pReturnData != NULL)
	{
		delete[](this->pReturnData);
	}
	this->pReturnData = new char[return_data_bytes_count];

	//	构造缓冲区数据
	//	构造数据头
	*((unsigned char*)(this->pReturnData) + 0) = (unsigned char)0x79;
	*((unsigned char*)(this->pReturnData) + 1) = (unsigned char)0x32;
	*((unsigned char*)(this->pReturnData) + 2) = (unsigned char)0x87;
	*((unsigned char*)(this->pReturnData) + 3) = (unsigned char)0x5A;

	//	复制请求命令的命令ID与命令ID反码
	*((unsigned char*)(this->pReturnData) + 4) = *((unsigned char*)(in_dat)+4);
	*((unsigned char*)(this->pReturnData) + 5) = *((unsigned char*)(in_dat)+5);
	*((unsigned char*)(this->pReturnData) + 6) = *((unsigned char*)(in_dat)+6);
	*((unsigned char*)(this->pReturnData) + 7) = *((unsigned char*)(in_dat)+7);

	//	设置数据区个数的4个字节
	int data_area_number = this->ReturnDataVec.size();
	*((unsigned char*)(this->pReturnData) + 8) = *((unsigned char*)(&data_area_number) + 0);
	*((unsigned char*)(this->pReturnData) + 9) = *((unsigned char*)(&data_area_number) + 1);
	*((unsigned char*)(this->pReturnData) + 10) = *((unsigned char*)(&data_area_number) + 2);
	*((unsigned char*)(this->pReturnData) + 11) = *((unsigned char*)(&data_area_number) + 3);

	//	构造数据区
	ByteCount = 0;
	for (int i = 0; i < data_area_number; ++i)
	{
		//	构造该数据区的数据长度的4个字节
		int len = (*(this->ReturnDataVec.begin() + i)).length;
		*((unsigned char*)(this->pReturnData) + ByteCount + MIN_DATA_SIZE + 0) = *((unsigned char*)(&len) + 0);
		*((unsigned char*)(this->pReturnData) + ByteCount + MIN_DATA_SIZE + 1) = *((unsigned char*)(&len) + 1);
		*((unsigned char*)(this->pReturnData) + ByteCount + MIN_DATA_SIZE + 2) = *((unsigned char*)(&len) + 2);
		*((unsigned char*)(this->pReturnData) + ByteCount + MIN_DATA_SIZE + 3) = *((unsigned char*)(&len) + 3);

		//	构造数据区
		memcpy_s((unsigned char*)(this->pReturnData) + ByteCount + MIN_DATA_SIZE + 4, len, (*(this->ReturnDataVec.begin() + i)).pdat, len);

		//	累计字节
		ByteCount = ByteCount + len + 4;
	}

	//	设置返回数据的长度
	this->ReturnDataSize = return_data_bytes_count;

	//	操作成功
	return true;
}

//	得到解析处理后的回执数据首地址
void* CMonitorProtocol::CClient::GetReturnData(void)
{
	//	返回数据区首地址
	if (this->ReturnDataSize == 0) return NULL;
	else	return this->pReturnData;
}

//	得到回执数据的长度
int CMonitorProtocol::CClient::GetReturnDataSize(void)
{
	return this->ReturnDataSize;
}

//----------------------------------------------------------------------------------------
//	无参数构造函数
CMonitorProtocol::CServer::CServer()
{
	//	配置参数
	this->SendDataBuffSize = 0;
	this->pSendDataBuff = NULL;
	this->pUserFunc = NULL;
	this->cmd_id = 0;
}

//	析构函数
CMonitorProtocol::CServer::~CServer()
{
	//	检查发送数据缓冲区是否需要释放
	if (this->pSendDataBuff != NULL)
	{
		this->SendDataBuffSize = 0;
		delete[](this->pSendDataBuff);
		this->pSendDataBuff = NULL;
	}
}

//	配置函数，配置成功返回true，失败返回false
bool CMonitorProtocol::CServer::Config(
	P_USER_CMP_SERVER_FUNC puserfunc				//	输入的用户函数指针
	)
{
	//	检查输入参数
	if ((puserfunc == NULL) || (puserfunc == (P_USER_CMP_SERVER_FUNC)(-1)))
	{
		return false;
	}

	//	配置用户函数
	this->pUserFunc = puserfunc;

	//	配置成功
	return true;
}

//	构造发送数据，构造成功返回true,失败返回false
bool CMonitorProtocol::CServer::BuildSendData(
	std::vector<SDataArea> in_vec
	)
{
	//	计算数据一共需要多少字节空间
	int return_data_bytes_count = MIN_DATA_SIZE;
	for (int i = 0; (in_vec.begin() + i) != (in_vec.end()); ++i)
	{
		return_data_bytes_count = return_data_bytes_count + (*(in_vec.begin() + i)).length;
	}
	return_data_bytes_count = return_data_bytes_count + in_vec.size() * 4;

	//	配置返回数据缓冲区，如果上一次的结果还存在
	this->SendDataBuffSize = 0;
	if (this->pSendDataBuff != NULL)
	{
		delete[](this->pSendDataBuff);
		this->pSendDataBuff = NULL;
	}
	this->pSendDataBuff = new char[return_data_bytes_count];

	//	构造缓冲区数据
	//	构造数据头
	*((unsigned char*)(this->pSendDataBuff) + 0) = (unsigned char)0x28;
	*((unsigned char*)(this->pSendDataBuff) + 1) = (unsigned char)0x77;
	*((unsigned char*)(this->pSendDataBuff) + 2) = (unsigned char)0x93;
	*((unsigned char*)(this->pSendDataBuff) + 3) = (unsigned char)0x5A;

	//	复制请求命令的命令ID与命令ID反码
	++cmd_id;
	unsigned short inv_cmd_id = ~cmd_id;
	*((unsigned char*)(this->pSendDataBuff) + 4) = *((unsigned char*)(&cmd_id) + 0);
	*((unsigned char*)(this->pSendDataBuff) + 5) = *((unsigned char*)(&cmd_id) + 1);
	*((unsigned char*)(this->pSendDataBuff) + 6) = *((unsigned char*)(&inv_cmd_id) + 0);
	*((unsigned char*)(this->pSendDataBuff) + 7) = *((unsigned char*)(&inv_cmd_id) + 1);

	//	设置数据区个数的4个字节
	int data_area_number = in_vec.size();
	*((unsigned char*)(this->pSendDataBuff) + 8) = *((unsigned char*)(&data_area_number) + 0);
	*((unsigned char*)(this->pSendDataBuff) + 9) = *((unsigned char*)(&data_area_number) + 1);
	*((unsigned char*)(this->pSendDataBuff) + 10) = *((unsigned char*)(&data_area_number) + 2);
	*((unsigned char*)(this->pSendDataBuff) + 11) = *((unsigned char*)(&data_area_number) + 3);

	//	构造数据区
	int ByteCount = 0;
	for (int i = 0; i < data_area_number; ++i)
	{
		//	构造该数据区的数据长度的4个字节
		int len = (*(in_vec.begin() + i)).length;
		*((unsigned char*)(this->pSendDataBuff) + ByteCount + MIN_DATA_SIZE + 0) = *((unsigned char*)(&len) + 0);
		*((unsigned char*)(this->pSendDataBuff) + ByteCount + MIN_DATA_SIZE + 1) = *((unsigned char*)(&len) + 1);
		*((unsigned char*)(this->pSendDataBuff) + ByteCount + MIN_DATA_SIZE + 2) = *((unsigned char*)(&len) + 2);
		*((unsigned char*)(this->pSendDataBuff) + ByteCount + MIN_DATA_SIZE + 3) = *((unsigned char*)(&len) + 3);

		//	构造数据区
		memcpy_s((unsigned char*)(this->pSendDataBuff) + ByteCount + MIN_DATA_SIZE + 4, len, (*(in_vec.begin() + i)).pdat, len);

		//	累计字节
		ByteCount = ByteCount + len + 4;
	}

	//	设置返回数据的长度
	this->SendDataBuffSize = return_data_bytes_count;

	//	检查释放用户输入数据区
	for (int i = 0; (in_vec.begin() + i) != (in_vec.end()); ++i)
	{
		if ((*(in_vec.begin() + i)).pdat != NULL)
		{
			delete[]((*(in_vec.begin() + i)).pdat); 
			(*(in_vec.begin() + i)).pdat = NULL;
		}
		in_vec.erase(in_vec.begin() + i);
		--i;
	}

	//	构造成功
	return true;
}

//	接收响应函数，返回true表示成功收到可以执行的命令，返回false表示无法匹配命令，这里仅仅是协议上的匹配，与用户数据区的内容无关
bool CMonitorProtocol::CServer::OnRecvHandle(
	void* in_dat,										//	收到的数据首地址
	int length											//	数据长度
	)
{
	//	检查输入参数
	if ((in_dat == NULL) || (in_dat == (void*)(-1)) || (length < MIN_DATA_SIZE))
	{
		return false;
	}

	//	检查用户函数是否可用
	if ((this->pUserFunc == NULL) || (this->pUserFunc == (P_USER_CMP_SERVER_FUNC)(-1)))
	{
		return false;
	}


	//	解析格式
	//	检查数据头
	if (
		(*((unsigned char*)in_dat + 0) != (unsigned char)0x79) ||
		(*((unsigned char*)in_dat + 1) != (unsigned char)0x32) ||
		(*((unsigned char*)in_dat + 2) != (unsigned char)0x87) ||
		(*((unsigned char*)in_dat + 3) != (unsigned char)0x5A)
		)
	{
		return false;
	}

	//	检查命令ID
	//	获得命令ID
	unsigned char cmd_id_h = *((unsigned char*)in_dat + 5);
	unsigned char cmd_id_l = *((unsigned char*)in_dat + 4);
	u_short cmd_ID = cmd_id_h * 0x100 + cmd_id_l;

	//	获得命令ID反码
	unsigned char inv_cmd_id_h = *((unsigned char*)in_dat + 7);
	unsigned char inv_cmd_id_l = *((unsigned char*)in_dat + 6);
	u_short inv_cmd_ID = inv_cmd_id_h * 0x100 + inv_cmd_id_l;

	//	计算反码
	inv_cmd_ID = ~inv_cmd_ID;

	//	对比反码
	if (cmd_ID != inv_cmd_ID)
	{
		return false;
	}

	//	获得数据区数量
	unsigned char data_area_buff[4];
	memcpy_s(data_area_buff, 4, (unsigned char*)in_dat + 8, 4);
	int DataAreaCount = *((int*)data_area_buff);

	//	检查数据区数量
	if (DataAreaCount < 1)
	{
		return false;
	}

	//	构造请求数据区
	std::vector<SDataArea> RequestDataVec;
	int ByteCount = 0;
	for (int i = 0; i < DataAreaCount; ++i)
	{
		//	获取该数据块的数据长度
		unsigned char data_length_buff[4];
		memcpy_s(data_length_buff, 4, (unsigned char*)in_dat + ByteCount + MIN_DATA_SIZE, 4);
		int DataLength = *((int*)data_length_buff);

		//	检查数据长度，出现严重错误
		if (DataLength <= 0)
		{
			//	释放资源
			for (int i = 0; (RequestDataVec.begin() + i) != (RequestDataVec.end()); ++i)
			{
				delete[]((*(RequestDataVec.begin() + i)).pdat);
				RequestDataVec.erase(RequestDataVec.begin() + i);
				--i;
			}

			//	返回失败
			return false;
		}

		//	构造临时数据区
		char* p_tmp_dat = new char[DataLength];

		//	拷贝数据
		memcpy_s(p_tmp_dat, DataLength, (unsigned char*)in_dat + ByteCount + MIN_DATA_SIZE + 4, DataLength);

		//	构造数据块结构
		SDataArea tmp_dat_area;
		tmp_dat_area.length = DataLength;
		tmp_dat_area.pdat = p_tmp_dat;

		//	插入容器
		RequestDataVec.insert(RequestDataVec.end(), tmp_dat_area);

		//	累计字节
		ByteCount = ByteCount + DataLength + 4;
	}
	
	//	调用用户函数，获得返回数据
	this->pUserFunc(RequestDataVec);

	//	释放资源
	for (int i = 0; (RequestDataVec.begin() + i) != (RequestDataVec.end()); ++i)
	{
		delete[]((*(RequestDataVec.begin() + i)).pdat);
		RequestDataVec.erase(RequestDataVec.begin() + i);
		--i;
	}

	//	匹配成功
	return true;
}

//	获得发送数据的缓冲区首地址
void* CMonitorProtocol::CServer::GetSendDataBuff(void)
{
	//	返回数据区首地址
	if (this->SendDataBuffSize == 0) return NULL;
	else	return this->pSendDataBuff;
}

//	获得发送数据的缓冲区长度
int CMonitorProtocol::CServer::GetSendDataBuffSize(void)
{
	return this->SendDataBuffSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//	全集成的TCP客户端类

//	接收处理函数
DWORD CMonitorProtocol::OnRecvCintegratedTCPClient_Func(LPVOID pParam)
{
	//	获取参数
	CMonitorProtocol::CintegratedTCPClient* pc = (CMonitorProtocol::CintegratedTCPClient*)(pParam);

	//	构造线程安全对象
	CThread::CThreadRunSafe msafethread(pc->m_recv_thread);

	//	定义返回值
	int re = 0;

	//	初始化已经断线重连的次数
	int already_reconnect_count = 0;

	//	定义处理错误标志,没有错误的代码为0,数据错误代码为1,连接错误代码为2
	int error_flag = 0;

	//	进入循环
	while (1)
	{
		//---------------------------------------------------------------------------------------------------------
		//	清除之前的结果并释放内存
		for (int i = 0; (pc->RecvVec.begin() + i) != (pc->RecvVec.end()); ++i)
		{
			delete[]((*(pc->RecvVec.begin() + i)).pdat);
			pc->RecvVec.erase(pc->RecvVec.begin() + i);
			--i;
		}
		pc->RecvVec.clear();
		for (int i = 0; (pc->ReturnVec.begin() + i) != (pc->ReturnVec.end()); ++i)
		{
			delete[]((*(pc->ReturnVec.begin() + i)).pdat);
			pc->ReturnVec.erase(pc->ReturnVec.begin() + i);
			--i;
		}
		pc->ReturnVec.clear();

		//	如果断开连接
		if (pc->GetDisconnectFlag() == true)
		{
			return 0;
		}

		//---------------------------------------------------------------------------------------------------------
		//	当程序执行到这里的时候，表示已经产生错误，下面为处理错误的流程
		//	如果为连接错误
		if (error_flag == 2)
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

			}	//	断线后的无限重连次数结束
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
			}	//	指定断线后的重连次数的处理结束
		}


		//---------------------------------------------------------------------------------------------------------
		//	开始进入读取数据头部分
		//	清空错误标志
		error_flag = 0;				//	清空错误标志
		
		//	读取一个数据头字节
		char* phead = new char[1];
		re = recv(pc->sockClient, phead, 1, 0);

		//	检查连接
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](phead);
			error_flag = 2;			//	连接错误
			continue;
		}

		//	检查数据
		int head = phead[0] & 0x000000FF;
		if (phead[0] != 0x28)
		{
			delete[](phead);
			error_flag = 1;			//	数据错误
			continue;
		}

		//-------------------------------
		//	读取一个数据头字节
		re = recv(pc->sockClient, phead, 1, 0);

		//	检查连接
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](phead);
			error_flag = 2;				//	连接错误
			continue;
		}

		//	检查数据
		head = phead[0] & 0x000000FF;
		if (head != 0x77)
		{
			delete[](phead);
			error_flag = 1;				//	数据错误
			continue;
		}

		//-------------------------------
		//	读取一个数据头字节
		re = recv(pc->sockClient, phead, 1, 0);

		//	检查连接
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](phead);
			error_flag = 2;				//	连接错误
			continue;
		}

		//	检查数据
		head = phead[0] & 0x000000FF;
		if (head != 0x93)
		{
			delete[](phead);
			error_flag = 1;				//	数据错误
			continue;
		}

		//-------------------------------
		//	读取一个数据头字节
		re = recv(pc->sockClient, phead, 1, 0);

		//	检查连接
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](phead);
			error_flag = 2;				//	连接错误
			continue;
		}

		//	检查数据
		head = phead[0] & 0x000000FF;
		if (head != 0x5A)
		{
			delete[](phead);
			error_flag = 1;				//	数据错误
			continue;
		}

		//-------------------------------
		//	释放数据头缓存
		delete[](phead);

		//---------------------------------------------------------------------------------------------------------
		//	进入命令ID和命令ID反码的获取检测部分
		//	申请ID内存
		char* pid = new char[4];

		//	从socket流中读取4个字节
		re = recv(pc->sockClient, pid, 4, 0);

		//	检查连接
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](pid);
			error_flag = 2;				//	连接错误
			continue;
		}

		//	检查数据
		u_short id = (pid[1] & 0x000000FF) * 0x100 + (pid[0] & 0x000000FF);
		u_short inv_id = (pid[3] & 0x000000FF) * 0x100 + (pid[2] & 0x000000FF);
		inv_id = ~inv_id;
		if (id != inv_id)
		{
			delete[](pid);
			error_flag = 1;				//	数据错误
			continue;
		}

		//	释放ID内存
		delete[](pid);

		//---------------------------------------------------------------------------------------------------------
		//	获取数据区个数
		//	申请数据区内存
		char* pnum = new char[4];

		//	从socket流中读取4个字节
		re = recv(pc->sockClient, pnum, 4, 0);

		//	检查连接
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](pnum);
			error_flag = 2;				//	连接错误
			continue;
		}

		//	检查数据
		int area_num = 0;
		memcpy_s(&area_num, 4, pnum, 4);
		if (area_num <= 0)
		{
			delete[](pnum);
			error_flag = 1;				//	数据错误
			continue;
		}

		//	开始循环读取数据区
		for (int k = 0; k < area_num; ++k)
		{
			//	获取该数据区的大小
			char* plength = new char[4];

			//	从socket流中读取4个字节
			re = recv(pc->sockClient, plength, 4, 0);

			//	检查连接
			if ((re == SOCKET_ERROR) || (re == 0))
			{
				delete[](pnum);
				delete[](plength);
				error_flag = 2;				//	连接错误
				break;
			}

			//	检查数据
			int area_length = 0;
			memcpy_s(&area_length, 4, plength, 4);
			if (area_length <= 0)
			{
				delete[](pnum);
				delete[](plength);
				error_flag = 1;				//	数据错误
				break;
			}

			//	读取数据区
			char* area_buff = new char[area_length];

			//	从socket流中读取area_buff个字节
			re = recv(pc->sockClient, area_buff, area_length, 0);

			//	检查连接
			if ((re == SOCKET_ERROR) || (re == 0))
			{
				//	20170331，修复BUG，当上述链接出现错误的时候导致area_buff的内存泄漏
				delete[](area_buff);

				delete[](plength);
				error_flag = 2;				//	连接错误
				break;
			}

			//	如果读取的数据个数不同，则循环读取
			int total_bytes = area_length;
			int already_bytes = re;
			while (already_bytes != total_bytes)
			{
				//	从socket流中读取area_buff个字节
				re = ::recv(pc->sockClient, area_buff + already_bytes, total_bytes - already_bytes, 0);

				//	检查连接
				if ((re == SOCKET_ERROR) || (re == 0))
				{
					//	20170331，修复BUG，当上述链接出现错误的时候导致area_buff的内存泄漏
					delete[](area_buff);

					delete[](plength);
					error_flag = 2;				//	连接错误
					break;
				}

				//	累加读取的字节
				already_bytes = already_bytes + re;
			}

			//	检查连接
			if ((re == SOCKET_ERROR) || (re == 0))
			{
				error_flag = 2;				//	连接错误
				break;
			}

			//	构造数据区
			SDataArea tmp_dat;
			tmp_dat.length = area_length;
			tmp_dat.pdat = area_buff;
			pc->RecvVec.insert(pc->RecvVec.end(), tmp_dat);
			
			//	释放数据区大小内存
			delete[](plength);
		}

		//	释放数据区内存
		delete[](pnum);

		//	20170331，修复此BUG
		//	当接收数据区的时候出现问题，将跳过如下步骤
		if (error_flag != 0) continue;

		//---------------------------------------------------------------------------------------------------------
		//	调用用户程序
		pc->ReturnVec = pc->pUserFunc(pc->RecvVec);

		//---------------------------------------------------------------------------------------------------------
		//	计算数据一共需要多少字节空间
		int return_data_bytes_count = MIN_DATA_SIZE;
		for (int i = 0; (pc->ReturnVec.begin() + i) != (pc->ReturnVec.end()); ++i)
		{
			return_data_bytes_count = return_data_bytes_count + (*(pc->ReturnVec.begin() + i)).length;
		}
		return_data_bytes_count = return_data_bytes_count + pc->ReturnVec.size() * 4;

		//	申请回执数据的缓存
		char* pReturnData = new char[return_data_bytes_count];

		//	构造缓冲区数据
		//	构造数据头
		*((unsigned char*)(pReturnData)+0) = (unsigned char)0x79;
		*((unsigned char*)(pReturnData)+1) = (unsigned char)0x32;
		*((unsigned char*)(pReturnData)+2) = (unsigned char)0x87;
		*((unsigned char*)(pReturnData)+3) = (unsigned char)0x5A;

		//	复制请求命令的命令ID与命令ID反码
		*((unsigned char*)(pReturnData)+4) = *((unsigned char*)(&id)+0);
		*((unsigned char*)(pReturnData)+5) = *((unsigned char*)(&id)+1);
		inv_id = ~id;
		*((unsigned char*)(pReturnData)+6) = *((unsigned char*)(&inv_id)+0);
		*((unsigned char*)(pReturnData)+7) = *((unsigned char*)(&inv_id)+1);

		//	设置数据区个数的4个字节
		int data_area_number = pc->ReturnVec.size();
		*((unsigned char*)(pReturnData)+8) = *((unsigned char*)(&data_area_number) + 0);
		*((unsigned char*)(pReturnData)+9) = *((unsigned char*)(&data_area_number) + 1);
		*((unsigned char*)(pReturnData)+10) = *((unsigned char*)(&data_area_number) + 2);
		*((unsigned char*)(pReturnData)+11) = *((unsigned char*)(&data_area_number) + 3);

		//	构造数据区
		int ByteCount = 0;
		for (int i = 0; i < data_area_number; ++i)
		{
			//	构造该数据区的数据长度的4个字节
			int len = (*(pc->ReturnVec.begin() + i)).length;
			*((unsigned char*)(pReturnData)+ByteCount + MIN_DATA_SIZE + 0) = *((unsigned char*)(&len) + 0);
			*((unsigned char*)(pReturnData)+ByteCount + MIN_DATA_SIZE + 1) = *((unsigned char*)(&len) + 1);
			*((unsigned char*)(pReturnData)+ByteCount + MIN_DATA_SIZE + 2) = *((unsigned char*)(&len) + 2);
			*((unsigned char*)(pReturnData)+ByteCount + MIN_DATA_SIZE + 3) = *((unsigned char*)(&len) + 3);

			//	构造数据区
			memcpy_s((unsigned char*)(pReturnData)+ByteCount + MIN_DATA_SIZE + 4, len, (*(pc->ReturnVec.begin() + i)).pdat, len);

			//	累计字节
			ByteCount = ByteCount + len + 4;
		}

		//	发送数据
		re = pc->TrySend(pReturnData, return_data_bytes_count);

		//	检查连接
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](pReturnData);
			error_flag = 2;				//	连接错误
			continue;
		}

		//	释放回执数据缓冲区
		delete[](pReturnData);

		//---------------------------------------------------------------------------------------------------------
	}	//	最外层的while(1)结束

	//	线程正常结束
	return 0;
}

//	获取端口标志,ture为要求断开，false为没有要求
bool CMonitorProtocol::CintegratedTCPClient::GetDisconnectFlag(void)
{
	bool tmp_flag = false;
	this->Disconnect_flag_CS.Enter();
	tmp_flag = this->Disconnect_flag;
	this->Disconnect_flag_CS.Leave();
	return tmp_flag;
}

//	设置断开标志,ture为要求断开，false为没有要求
void CMonitorProtocol::CintegratedTCPClient::SetDisconnectFlag(bool flag)
{
	this->Disconnect_flag_CS.Enter();
	this->Disconnect_flag = flag;
	this->Disconnect_flag_CS.Leave();
}

//	配置函数，默认配置为 断线后立即重试连接，并且无限重试次数，成功返回0，失败返回SOCKET_ERROR
int CMonitorProtocol::CintegratedTCPClient::Config(
	char* remote_ip,										//	远程主机的IP地址
	u_short remote_port,									//	远程主机的端口
	bool now_conn,											//	是否立即进行连接，true为配置成功后立即连接，false为仅仅配置，不进行连接
	P_USER_ON_RECV_DATAAREA_CLIENT_FUNC puserfunc,			//	用户函数
	P_USER_ON_TCP_SERVER_BREAK_FUNC ponbreakfunc,			//	当与服务器断开连接的时候，所调用的响应函数
	int retry_sec,											//	断线重连的超时秒数，0表示断线后立即进行连接，>0表示断线后经过多久后重试建立连接
	int retry_count											//	重试次数，-1表示无限重试次数，0表示断线后不进行重试连接，>0表示断线后重试的次数
	)
{
	//	参数检查
	if ((retry_sec < 0) || (retry_count < -1) || (puserfunc == NULL) || (puserfunc == (P_USER_ON_RECV_DATAAREA_CLIENT_FUNC)(-1)))
	{
		return SOCKET_ERROR;
	}

	//	如果已经有配置
	if (this->init_flag == true)
	{
		//	为了防止断线重连机制导致的断开假象，所以这里再次执行终止线程
		SetDisconnectFlag(true);
		Sleep(10);
		while (this->m_recv_thread.GetState() != CThread::STOP) Sleep(1);
		SetDisconnectFlag(false);

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

	//	配置用户函数
	this->pUserFunc = puserfunc;

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
int CMonitorProtocol::CintegratedTCPClient::Connect(void)
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
		OnRecvCintegratedTCPClient_Func,
		this,
		CThread::RUN
		);

	//	连接成功
	return 0;
}

//	断开函数
void CMonitorProtocol::CintegratedTCPClient::Disconnect(void)
{
	//	检查是否没有配置
	if (this->init_flag != true)
	{
		return;
	}

	//	终止线程
	SetDisconnectFlag(true);

	//	释放SOCKET
	if (this->sockClient != NULL) closesocket(this->sockClient);
	this->sockClient = NULL;

	while (this->m_recv_thread.GetState() != CThread::STOP) Sleep(1);
	SetDisconnectFlag(false);

}

//	无参数构造函数
CMonitorProtocol::CintegratedTCPClient::CintegratedTCPClient()
{
	this->Disconnect_flag = false;
	this->pUserFunc = NULL;
}

//	析构函数
CMonitorProtocol::CintegratedTCPClient::~CintegratedTCPClient()
{
	//	这里只需要等待线程结束，当这里结束的时候，才会执行线程类的析构函数，这样内存会安全释放
	this->Disconnect();
}


////////////////////////////////////////////////////////////////////////////////////////////////
//	全集成TCP服务器连接类

//	接收处理函数
DWORD CMonitorProtocol::OnRecvCintegratedTCPServerHost_Func(LPVOID pParam)
{
	//	得到传入参数
	CMonitorProtocol::CintegratedTCPServerHost* pCTCPServerHost = (CMonitorProtocol::CintegratedTCPServerHost*)pParam;

	//	构造线程安全对象
	CThread::CThreadRunSafe msafethread(pCTCPServerHost->m_recv_thread);

	//	定义处理错误标志,没有错误的代码为0,数据错误代码为1,连接错误代码为2
	int error_flag = 0;

	//	定义返回值
	int re = 0;

	//	获取SOCKET，为了提高接收效率，将获取放到循环外
	pCTCPServerHost->opt_m_con_socket_cs.Enter();
	SOCKET tmp_socket = pCTCPServerHost->m_con_socket;
	pCTCPServerHost->opt_m_con_socket_cs.Leave();

	//	进入接收循环
	while (1)
	{
		//----------------------------------------------------------------------------------------------------------------
		//	清除之前的结果并释放内存，这里仅仅释放接收到的资源内存
		for (int i = 0; (pCTCPServerHost->RecvVec.begin() + i) != (pCTCPServerHost->RecvVec.end()); ++i)
		{
			delete[]((*(pCTCPServerHost->RecvVec.begin() + i)).pdat);
			pCTCPServerHost->RecvVec.erase(pCTCPServerHost->RecvVec.begin() + i);
			--i;
		}
		pCTCPServerHost->RecvVec.clear();

		//----------------------------------------------------------------------------------------------------------------
		//	错误处理部分
		//	如果为连接错误
		if (error_flag == 2)
		{
			//	设置失败标识
			pCTCPServerHost->opt_fail_status_cs.Enter();
			pCTCPServerHost->fail_status = true;
			pCTCPServerHost->opt_fail_status_cs.Leave();

			//	退出接收线程
			return 0;
		}

		//----------------------------------------------------------------------------------------------------------------
		//	清除错误标志
		error_flag = 0;

		//----------------------------------------------------------------------------------------------------------------
		//	接收处理流程
		//	读取一个数据头字节
		char* phead = new char[1];
		re = recv(tmp_socket, phead, 1, 0);

		//	检查连接
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](phead);
			error_flag = 2;			//	连接错误
			continue;
		}

		//	检查数据
		int head = phead[0] & 0x000000FF;
		if (phead[0] != 0x79)
		{
			delete[](phead);
			error_flag = 1;			//	数据错误
			continue;
		}

		//-------------------------------
		//	读取一个数据头字节
		re = recv(tmp_socket, phead, 1, 0);

		//	检查连接
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](phead);
			error_flag = 2;				//	连接错误
			continue;
		}

		//	检查数据
		head = phead[0] & 0x000000FF;
		if (head != 0x32)
		{
			delete[](phead);
			error_flag = 1;				//	数据错误
			continue;
		}

		//-------------------------------
		//	读取一个数据头字节
		re = recv(tmp_socket, phead, 1, 0);

		//	检查连接
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](phead);
			error_flag = 2;				//	连接错误
			continue;
		}

		//	检查数据
		head = phead[0] & 0x000000FF;
		if (head != 0x87)
		{
			delete[](phead);
			error_flag = 1;				//	数据错误
			continue;
		}

		//-------------------------------
		//	读取一个数据头字节
		re = recv(tmp_socket, phead, 1, 0);

		//	检查连接
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](phead);
			error_flag = 2;				//	连接错误
			continue;
		}

		//	检查数据
		head = phead[0] & 0x000000FF;
		if (head != 0x5A)
		{
			delete[](phead);
			error_flag = 1;				//	数据错误
			continue;
		}

		//-------------------------------
		//	释放数据头缓存
		delete[](phead);

		//---------------------------------------------------------------------------------------------------------
		//	进入命令ID和命令ID反码的获取检测部分
		//	申请ID内存
		char* pid = new char[4];

		//	从socket流中读取4个字节
		re = recv(tmp_socket, pid, 4, 0);

		//	检查连接
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](pid);
			error_flag = 2;				//	连接错误
			continue;
		}

		//	检查数据
		u_short id = pid[1] * 0x100 + (pid[0]&0x00ff);
		u_short inv_id = pid[3] * 0x100 + (pid[2]&0x00ff);
		inv_id = ~inv_id;
		if (id != inv_id)
		{
			delete[](pid);
			error_flag = 1;				//	数据错误
			continue;
		}

		//	释放ID内存
		delete[](pid);

		//---------------------------------------------------------------------------------------------------------
		//	获取数据区个数
		//	申请数据区内存
		char* pnum = new char[4];

		//	从socket流中读取4个字节
		re = recv(tmp_socket, pnum, 4, 0);

		//	检查连接
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](pnum);
			error_flag = 2;				//	连接错误
			continue;
		}

		//	检查数据
		int area_num = 0;
		memcpy_s(&area_num, 4, pnum, 4);
		if (area_num <= 0)
		{
			delete[](pnum);
			error_flag = 1;				//	数据错误
			continue;
		}

		//	开始循环读取数据区
		for (int k = 0; k < area_num; ++k)
		{
			//	获取该数据区的大小
			char* plength = new char[4];

			//	从socket流中读取4个字节
			re = recv(tmp_socket, plength, 4, 0);

			//	检查连接
			if ((re == SOCKET_ERROR) || (re == 0))
			{
				delete[](pnum);
				delete[](plength);
				error_flag = 2;				//	连接错误
				break;
			}

			//	检查数据
			int area_length = 0;
			memcpy_s(&area_length, 4, plength, 4);
			if (area_length <= 0)
			{
				delete[](pnum);
				delete[](plength);
				error_flag = 1;				//	数据错误
				break;
			}

			//	读取数据区
			char* area_buff = new char[area_length];

			//	从socket流中读取area_buff个字节
			re = ::recv(tmp_socket, area_buff, area_length, 0);

			//	检查连接
			if ((re == SOCKET_ERROR) || (re == 0))
			{
				//	20170331，修复BUG，当上述链接出现错误的时候导致area_buff的内存泄漏
				delete[](area_buff);

				delete[](plength);
				error_flag = 2;				//	连接错误
				break;
			}

			//	如果读取的数据个数不同，则循环读取
			int total_bytes = area_length;
			int already_bytes = re;
			while (already_bytes != total_bytes)
			{
				//	从socket流中读取area_buff个字节
				re = ::recv(tmp_socket, area_buff + already_bytes, total_bytes - already_bytes, 0);

				//	检查连接
				if ((re == SOCKET_ERROR) || (re == 0))
				{
					//	20170331，修复BUG，当上述链接出现错误的时候导致area_buff的内存泄漏
					delete[](area_buff);

					delete[](plength);
					error_flag = 2;				//	连接错误
					break;
				}

				//	累加读取的字节
				already_bytes = already_bytes + re;
			}

			//	检查连接
			if ((re == SOCKET_ERROR) || (re == 0))
			{
				error_flag = 2;				//	连接错误
				break;
			}

			//	构造数据区
			SDataArea tmp_dat;
			tmp_dat.length = area_length;
			tmp_dat.pdat = area_buff;
			pCTCPServerHost->RecvVec.insert(pCTCPServerHost->RecvVec.end(), tmp_dat);

			//	释放数据区大小内存
			delete[](plength);
		}

		//	释放数据区内存
		delete[](pnum);

		//	20170331，修改BUG，当在接收数据区的时候出现错误时，时不允许调用用户程序的
		//	检查数据和链接是否存在问题，当存在问题的时候跳过执行用户程序
		if (error_flag != 0) continue;

		//---------------------------------------------------------------------------------------------------------
		//	调用用户程序
		pCTCPServerHost->pUserFunc(tmp_socket, pCTCPServerHost->RecvVec);

		//---------------------------------------------------------------------------------------------------------
	}	//	最外层的while(1)循环结束

	return 0;
}

//	获取端口标志,ture为要求断开，false为没有要求
bool CMonitorProtocol::CintegratedTCPServerHost::GetDisconnectFlag(void)
{
	bool tmp_flag = false;
	this->Disconnect_flag_CS.Enter();
	tmp_flag = this->Disconnect_flag;
	this->Disconnect_flag_CS.Leave();
	return tmp_flag;
}

//	设置断开标志,ture为要求断开，false为没有要求
void CMonitorProtocol::CintegratedTCPServerHost::SetDisconnectFlag(bool flag)
{
	this->Disconnect_flag_CS.Enter();
	this->Disconnect_flag = flag;
	this->Disconnect_flag_CS.Leave();
}

//	配置函数,正常是返回0，错误的时候返回SOCKET_ERROR
int CMonitorProtocol::CintegratedTCPServerHost::Config(
	SOCKET m_socket,										//	连接的SOCKET
	P_USER_ON_RECV_DATAAREA_SERVER_FUNC puserfunc			//	传入的函数指针
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

		//	终止线程
		SetDisconnectFlag(true);
		//	释放SOCKET
		opt_m_con_socket_cs.Enter();
		if (this->m_con_socket != NULL) closesocket(this->m_con_socket);
		this->m_con_socket = NULL;
		opt_m_con_socket_cs.Leave();
		while (this->m_recv_thread.GetState() != CThread::STOP) Sleep(1);
		SetDisconnectFlag(false);
	}

	//	检查输入参数是否合法
	if ((m_socket == NULL) || (m_socket == (SOCKET)-1) || (puserfunc == NULL) || (puserfunc == (P_USER_ON_RECV_DATAAREA_SERVER_FUNC)-1))
	{
		return SOCKET_ERROR;
	}

	//	赋值参数
	opt_m_con_socket_cs.Enter();
	this->m_con_socket = m_socket;
	opt_m_con_socket_cs.Leave();
	this->pUserFunc = puserfunc;
	this->m_recv_thread.Config(
		OnRecvCintegratedTCPServerHost_Func,
		this,
		CThread::RUN
		);

	//	配置成功
	this->init_flag = true;
	return 0;
}

//	发送数据，由于所在类为服务器，所以为向客户端发送数据,正常是返回已经发送的字节，错误的时候返回SOCKET_ERROR
int CMonitorProtocol::CintegratedTCPServerHost::Send(
	std::vector<SDataArea> in_vec
	)
{
	//	如果当前为失败状态，则不作处理
	opt_fail_status_cs.Enter();
	bool tmp_fail_status = this->fail_status;
	opt_fail_status_cs.Leave();
	if (tmp_fail_status == true)
	{
		//	尝试释放输入数据区
		for (int j = 0; (in_vec.begin() + j) != (in_vec.end()); ++j)
		{
			if (((*(in_vec.begin() + j)).pdat != NULL) && ((*(in_vec.begin() + j)).pdat != (char*)(-1)))
			{
				delete[]((*(in_vec.begin() + j)).pdat);
			}
			in_vec.erase(in_vec.begin() + j);
			--j;
		}
		return SOCKET_ERROR;
	}

	//	判断是否配置，如果没有配置
	if (this->init_flag == false)
	{
		//	尝试释放输入数据区
		for (int j = 0; (in_vec.begin() + j) != (in_vec.end()); ++j)
		{
			if (((*(in_vec.begin() + j)).pdat != NULL) && ((*(in_vec.begin() + j)).pdat != (char*)(-1)))
			{
				delete[]((*(in_vec.begin() + j)).pdat);
			}
			in_vec.erase(in_vec.begin() + j);
			--j;
		}
		return SOCKET_ERROR;
	}

	//	检查输入数据是否合法,遍历所有的输入数据块，分别检查长度和指针
	for (int i = 0; (in_vec.begin() + i) != (in_vec.end()); ++i)
	{
		//	如果非法
		if (((*(in_vec.begin() + i)).length <= 0) || ((*(in_vec.begin() + i)).pdat == NULL) || ((*(in_vec.begin() + i)).pdat == (char*)(-1)))
		{
			//	尝试释放输入数据区
			for (int j = 0; (in_vec.begin() + j) != (in_vec.end()); ++j)
			{
				if (((*(in_vec.begin() + j)).pdat != NULL) && ((*(in_vec.begin() + j)).pdat != (char*)(-1)))
				{
					delete[]((*(in_vec.begin() + j)).pdat);
				}
				in_vec.erase(in_vec.begin() + j);
				--j;
			}
			return SOCKET_ERROR;
		}
	}

	//	计算数据一共需要多少字节空间
	int return_data_bytes_count = MIN_DATA_SIZE;
	for (int i = 0; (in_vec.begin() + i) != (in_vec.end()); ++i)
	{
		return_data_bytes_count = return_data_bytes_count + (*(in_vec.begin() + i)).length;
	}
	return_data_bytes_count = return_data_bytes_count + in_vec.size() * 4;

	//	申请回执数据的缓存
	char* pReturnData = new char[return_data_bytes_count];

	//	构造缓冲区数据
	//	构造数据头
	*((unsigned char*)(pReturnData)+0) = (unsigned char)0x28;
	*((unsigned char*)(pReturnData)+1) = (unsigned char)0x77;
	*((unsigned char*)(pReturnData)+2) = (unsigned char)0x93;
	*((unsigned char*)(pReturnData)+3) = (unsigned char)0x5A;

	//	复制请求命令的命令ID与命令ID反码
	static u_short id = 0;
	++id;
	*((unsigned char*)(pReturnData)+4) = *((unsigned char*)(&id) + 0);
	*((unsigned char*)(pReturnData)+5) = *((unsigned char*)(&id) + 1);
	u_short inv_id = ~id;
	*((unsigned char*)(pReturnData)+6) = *((unsigned char*)(&inv_id) + 0);
	*((unsigned char*)(pReturnData)+7) = *((unsigned char*)(&inv_id) + 1);

	//	设置数据区个数的4个字节
	int data_area_number = in_vec.size();
	*((unsigned char*)(pReturnData)+8) = *((unsigned char*)(&data_area_number) + 0);
	*((unsigned char*)(pReturnData)+9) = *((unsigned char*)(&data_area_number) + 1);
	*((unsigned char*)(pReturnData)+10) = *((unsigned char*)(&data_area_number) + 2);
	*((unsigned char*)(pReturnData)+11) = *((unsigned char*)(&data_area_number) + 3);

	//	构造数据区
	int ByteCount = 0;
	for (int i = 0; i < data_area_number; ++i)
	{
		//	构造该数据区的数据长度的4个字节
		int len = (*(in_vec.begin() + i)).length;
		*((unsigned char*)(pReturnData)+ByteCount + MIN_DATA_SIZE + 0) = *((unsigned char*)(&len) + 0);
		*((unsigned char*)(pReturnData)+ByteCount + MIN_DATA_SIZE + 1) = *((unsigned char*)(&len) + 1);
		*((unsigned char*)(pReturnData)+ByteCount + MIN_DATA_SIZE + 2) = *((unsigned char*)(&len) + 2);
		*((unsigned char*)(pReturnData)+ByteCount + MIN_DATA_SIZE + 3) = *((unsigned char*)(&len) + 3);

		//	构造数据区
		memcpy_s((unsigned char*)(pReturnData)+ByteCount + MIN_DATA_SIZE + 4, len, (*(in_vec.begin() + i)).pdat, len);

		//	累计字节
		ByteCount = ByteCount + len + 4;
	}

	//	发送数据
	opt_m_con_socket_cs.Enter();
	SOCKET tmp_socket = this->m_con_socket;
	opt_m_con_socket_cs.Leave();
	int re = ::send(
		tmp_socket,
		pReturnData,
		return_data_bytes_count,
		0
		);
	
	//	释放回执数据缓冲区
	delete[](pReturnData);

	//	尝试释放输入数据区
	for (int j = 0; (in_vec.begin() + j) != (in_vec.end()); ++j)
	{
		if (((*(in_vec.begin() + j)).pdat != NULL) && ((*(in_vec.begin() + j)).pdat != (char*)(-1)))
		{
			delete[]((*(in_vec.begin() + j)).pdat);
		}
		in_vec.erase(in_vec.begin() + j);
		--j;
	}
	
	//	检查是否发送失败
	if (re == SOCKET_ERROR)
	{
		//	设置失败标识
		opt_fail_status_cs.Enter();
		this->fail_status = true;
		opt_fail_status_cs.Leave();

		//	终止线程
		SetDisconnectFlag(true);
		//	释放SOCKET
		opt_m_con_socket_cs.Enter();
		if (this->m_con_socket != NULL) closesocket(this->m_con_socket);
		this->m_con_socket = NULL;
		opt_m_con_socket_cs.Leave();
		while (this->m_recv_thread.GetState() != CThread::STOP) Sleep(1);
		SetDisconnectFlag(false);
	}

	return re;
}

//	断开连接
void CMonitorProtocol::CintegratedTCPServerHost::Disconnect(void)
{
	//	如果已经配置了的话，并且在没有失败的时候
	if (this->init_flag == true)
	{
		//	清空标志，防止操作的时候发生错误
		this->init_flag = false;

		//	终止线程
		SetDisconnectFlag(true);
		//	释放SOCKET
		opt_m_con_socket_cs.Enter();
		if (this->m_con_socket != NULL) closesocket(this->m_con_socket);
		this->m_con_socket = NULL;
		opt_m_con_socket_cs.Leave();
		while (this->m_recv_thread.GetState() != CThread::STOP) Sleep(1);
		SetDisconnectFlag(false);
	}
}

//	无参数构造函数
CMonitorProtocol::CintegratedTCPServerHost::CintegratedTCPServerHost()
{
	this->Disconnect_flag = false;
	this->pUserFunc = NULL;
}

//	析构函数
CMonitorProtocol::CintegratedTCPServerHost::~CintegratedTCPServerHost()
{
	//	断开与客户端的连接并释放资源
	this->Disconnect();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	全集成的服务器管理类

//	监听处理线程函数
DWORD CMonitorProtocol::integrated_TCP_SERVER_MANAGE_ACCEPT_THREAD(LPVOID pParam)
{
	//	转换输入参数
	CintegratedTCPServerManage* pCObj = (CintegratedTCPServerManage*)(pParam);

	//	构造线程安全对象
	CThread::CThreadRunSafe msafethread(pCObj->m_accept_thread);

	//	定义临时对象
	SOCKET sockConnection = NULL;
	SOCKADDR_IN addrClient;
	int len = sizeof(addrClient);


	//	进入连接等待循环
	while (1)
	{
		//	阻塞式应答连接请求
		sockConnection = ::accept(pCObj->socketServer, (SOCKADDR *)&addrClient, &len);

		//	检查连接是否正常
		if ((sockConnection == NULL) || (pCObj->GetDisconnectACCEPTFlag() == true))
		{
			return 0;
		}

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
DWORD CMonitorProtocol::integrated_TCP_SERVER_MANAGE_FIND_ERROR_THREAD(LPVOID pParam)
{
	//	转换输入参数
	CintegratedTCPServerManage* pCObj = (CintegratedTCPServerManage*)(pParam);

	//	构造线程安全对象
	CThread::CThreadRunSafe msafethread(pCObj->m_find_error_thread);

	//	进入找错误的循环
	while (1)
	{
		//	首先进行延时，每隔200ms进行一次检查错误
		Sleep(200);
		
		//	检查连接是否正常
		if (pCObj->GetDisconnectFINDERRORFlag() == true)
		{
			return 0;
		}

		//	执行自动移除错误
		pCObj->AutoRemoveError();
	}

	return 0;
}

//	获取端口标志,ture为要求断开，false为没有要求
bool CMonitorProtocol::CintegratedTCPServerManage::GetDisconnectACCEPTFlag(void)
{
	bool tmp_flag = false;
	this->Disconnect_ACCEPT_flag_CS.Enter();
	tmp_flag = this->Disconnect_ACCEPT_flag;
	this->Disconnect_ACCEPT_flag_CS.Leave();
	return tmp_flag;
}

//	设置断开标志,ture为要求断开，false为没有要求
void CMonitorProtocol::CintegratedTCPServerManage::SetDisconnectACCEPTFlag(bool flag)
{
	this->Disconnect_ACCEPT_flag_CS.Enter();
	this->Disconnect_ACCEPT_flag = flag;
	this->Disconnect_ACCEPT_flag_CS.Leave();
}

//	获取端口标志,ture为要求断开，false为没有要求
bool CMonitorProtocol::CintegratedTCPServerManage::GetDisconnectFINDERRORFlag(void)
{
	bool tmp_flag = false;
	this->Disconnect_FIND_ERROR_flag_CS.Enter();
	tmp_flag = this->Disconnect_FIND_ERROR_flag;
	this->Disconnect_FIND_ERROR_flag_CS.Leave();
	return tmp_flag;
}

//	设置断开标志,ture为要求断开，false为没有要求
void CMonitorProtocol::CintegratedTCPServerManage::SetDisconnectFINDERRORFlag(bool flag)
{
	this->Disconnect_FIND_ERROR_flag_CS.Enter();
	this->Disconnect_FIND_ERROR_flag = flag;
	this->Disconnect_FIND_ERROR_flag_CS.Leave();
}

//	无参数构造函数
CMonitorProtocol::CintegratedTCPServerManage::CintegratedTCPServerManage()
{
	//	清空字符串缓冲区
	memset(this->ip_str_GetClientIPStr, 0, sizeof(this->ip_str_GetClientIPStr));
	memset(this->ip_str_GetServerIPString, 0, sizeof(this->ip_str_GetServerIPString));

	this->init_flag = false;					//	设置为初始化
	this->socketServer = NULL;					//	服务器资源SOCKET为空

	this->pOnJoinFunc = NULL;					//	连接响应函数为空
	this->pOnBreadFunc = NULL;					//	断开响应函数为空

	this->Disconnect_ACCEPT_flag = false;
	this->Disconnect_FIND_ERROR_flag = false;
}

//	析构函数
CMonitorProtocol::CintegratedTCPServerManage::~CintegratedTCPServerManage()
{
	//	检查是否已经初始化过
	if (this->init_flag == true)
	{
		//	终止线程
		this->init_flag = false;
		this->SetDisconnectACCEPTFlag(true);
		closesocket(this->socketServer);
		while (this->m_accept_thread.GetState() != CThread::STOP) Sleep(1);
		this->SetDisconnectACCEPTFlag(false);
		this->SetDisconnectFINDERRORFlag(true);
		while (this->m_find_error_thread.GetState() != CThread::STOP) Sleep(1);
		this->SetDisconnectFINDERRORFlag(false);

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
bool CMonitorProtocol::CintegratedTCPServerManage::AddConList(
	SOCKET in_socket,										//	已经成功建立连接的SOCKET，也是唯一的标识
	SOCKADDR_IN	addrClient,									//	已连接客户端的地址信息
	P_USER_ON_RECV_DATAAREA_SERVER_FUNC precvfunc			//	用户定义的接收函数
	)
{
	//	检查参数
	//	检查in_socket
	if ((in_socket == NULL) || (in_socket == (SOCKET)(-1)))
	{
		return false;
	}

	//	检查函数指针
	if ((precvfunc == NULL) || (precvfunc == (P_USER_ON_RECV_DATAAREA_SERVER_FUNC)(-1)))
	{
		return false;
	}

	//	创建连接类
	CintegratedTCPServerHost* pCTCPServerHost = new CintegratedTCPServerHost;
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
bool CMonitorProtocol::CintegratedTCPServerManage::RemoveConList(
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
		this->mConList.erase(this->mConList.begin() + index);
	}
	mconlist_cs.Leave();		//	由于对mConList的操作都没有了，所以先离开临界，然后进行资源的释放

	//	如果上述过程没有找到
	if ((index == -1) || (ptmp_dat == NULL))
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
void CMonitorProtocol::CintegratedTCPServerManage::AutoRemoveError(void)
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
bool CMonitorProtocol::CintegratedTCPServerManage::Config(
	CTCPServerResources& rCTCPServerResources,							//	服务器资源的引用
	P_USER_ON_RECV_DATAAREA_SERVER_FUNC pfunc							//	用户接收函数
	)
{
	//	检查输入函数指针
	if ((pfunc == NULL) || (pfunc == (P_USER_ON_RECV_DATAAREA_SERVER_FUNC)(-1)))
	{
		return false;
	}

	//	检查是否已经初始化过
	if (this->init_flag == true)
	{
		//	终止线程
		this->init_flag = false;
		this->SetDisconnectACCEPTFlag(true);
		closesocket(this->socketServer);
		while (this->m_accept_thread.GetState() != CThread::STOP) Sleep(1);
		this->SetDisconnectACCEPTFlag(false);
		this->SetDisconnectFINDERRORFlag(true);
		while (this->m_find_error_thread.GetState() != CThread::STOP) Sleep(1);
		this->SetDisconnectFINDERRORFlag(false);

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
	this->addrServer = rCTCPServerResources.GetSocketAddr();
	this->socketServer = rCTCPServerResources.GetSocket();
	this->pRecvFunc = pfunc;

	//	启动两个线程
	this->m_accept_thread.Config(
		integrated_TCP_SERVER_MANAGE_ACCEPT_THREAD,
		this,
		CThread::RUN
		);
	this->m_find_error_thread.Config(
		integrated_TCP_SERVER_MANAGE_FIND_ERROR_THREAD,
		this,
		CThread::RUN
		);

	//	初始化配置成功
	this->init_flag = true;
	//	配置成功
	return true;
}

//	带有连接和断开响应函数的初始化配置函数，成功返回true，失败返回false
bool CMonitorProtocol::CintegratedTCPServerManage::Config(
	CTCPServerResources& rCTCPServerResources,			//	服务器资源的引用
	P_USER_ON_RECV_DATAAREA_SERVER_FUNC pfunc,			//	用户接收函数	
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
std::vector<SOCKET> CMonitorProtocol::CintegratedTCPServerManage::GetConListCopy(void)
{
	std::vector<SOCKET> tmp_vec;
	this->mconlist_cs.Enter();
	for (int i = 0; (this->mConList.begin() + i) != (this->mConList.end()); ++i)
	{
		tmp_vec.insert(tmp_vec.end(), (*(this->mConList.begin() + i))->socket);
	}
	this->mconlist_cs.Leave();
	return tmp_vec;
}

//	返回连接副本，包含全部信息的列表
std::vector<CMonitorProtocol::CintegratedTCPServerManage::SReConInfo> CMonitorProtocol::CintegratedTCPServerManage::GetConListAllInfoCopy(void)
{
	std::vector<CMonitorProtocol::CintegratedTCPServerManage::SReConInfo> tmp_vec;
	CMonitorProtocol::CintegratedTCPServerManage::SReConInfo tmp_s;
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
bool CMonitorProtocol::CintegratedTCPServerManage::Disconnect(SOCKET in_socket)
{
	return this->RemoveConList(in_socket);
}

//	发送数据，>0表示操作成功，SOCKET_ERROR表示列表中没有此连接,0表示发送失败
int CMonitorProtocol::CintegratedTCPServerManage::Send(
	SOCKET in_socket,											//	指定的SOCKET句柄
	std::vector<SDataArea> in_vec								//	发送的数据块
	)
{
	//	检查输入参数
	if ((in_socket == NULL) || (in_socket == (SOCKET)(-1)))
	{
		//	尝试释放输入数据区
		for (int j = 0; (in_vec.begin() + j) != (in_vec.end()); ++j)
		{
			if (((*(in_vec.begin() + j)).pdat != NULL) && ((*(in_vec.begin() + j)).pdat != (char*)(-1)))
			{
				delete[]((*(in_vec.begin() + j)).pdat);
			}
			in_vec.erase(in_vec.begin() + j);
			--j;
		}
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
		//	尝试释放输入数据区
		for (int j = 0; (in_vec.begin() + j) != (in_vec.end()); ++j)
		{
			if (((*(in_vec.begin() + j)).pdat != NULL) && ((*(in_vec.begin() + j)).pdat != (char*)(-1)))
			{
				delete[]((*(in_vec.begin() + j)).pdat);
			}
			in_vec.erase(in_vec.begin() + j);
			--j;
		}
		return SOCKET_ERROR;
	}

	//	发送数据
	int re = ptmp_dat->pCon->Send(in_vec);

	//	检查是否发送成功，当失败的时候，就已经表示这个连接已经失效了，所以将从列表中移除这个连接
	if (re == SOCKET_ERROR)
	{
		this->RemoveConList(ptmp_dat->socket);
		//	尝试释放输入数据区
		for (int j = 0; (in_vec.begin() + j) != (in_vec.end()); ++j)
		{
			if (((*(in_vec.begin() + j)).pdat != NULL) && ((*(in_vec.begin() + j)).pdat != (char*)(-1)))
			{
				delete[]((*(in_vec.begin() + j)).pdat);
			}
			in_vec.erase(in_vec.begin() + j);
			--j;
		}
		return SOCKET_ERROR;
	}

	//	运行到这里就成功了
	return re;
}

//	得到IP字符串，错误将返回一个空字符串
char* CMonitorProtocol::CintegratedTCPServerManage::GetClientIPStr(SOCKET in_socket)
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
u_short CMonitorProtocol::CintegratedTCPServerManage::GetClientPort(SOCKET in_socket)
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
CMyTime CMonitorProtocol::CintegratedTCPServerManage::GetClientBeginTime(SOCKET in_socket)
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
char* CMonitorProtocol::CintegratedTCPServerManage::GetServerIPString(void)
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
int CMonitorProtocol::CintegratedTCPServerManage::GetServerPort(void)
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
SOCKET CMonitorProtocol::CintegratedTCPServerManage::GetServerSOCKET(void)
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