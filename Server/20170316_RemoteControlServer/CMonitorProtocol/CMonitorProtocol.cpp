
//	REV 0.5

#include "stdafx.h"
#include "CMonitorProtocol.h"

//	��С���ݳߴ�
const int CMonitorProtocol::MIN_DATA_SIZE = 12;

//	�޲������캯��
CMonitorProtocol::CClient::CClient()
{
	this->pUserFunc = NULL;
	this->pReturnData = NULL;
	this->ReturnDataSize = 0;
}

//	��������
CMonitorProtocol::CClient::~CClient()
{
	//	����ִ�������Ƿ���Ҫ�ͷ�
	if (this->pReturnData != NULL)
	{
		this->ReturnDataSize = 0;
		delete[](this->pReturnData);
		this->pReturnData = NULL;
	}

	//	����ͷŻ�ִ������
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

//	���ú���������true��ʾ���óɹ���false��ʾ����ʧ��
bool CMonitorProtocol::CClient::Config(
	P_USER_CMP_CLIENT_FUNC puserfunc					//	���ݵ��û�����ָ��
	)
{
	//	��鴫�����
	if ((puserfunc == NULL) || (puserfunc == (P_USER_CMP_CLIENT_FUNC)(-1)))
	{
		return false;
	}

	//	�����û����庯��ָ��
	this->pUserFunc = puserfunc;

	//	���سɹ�
	return true;
}

/*
	�ڲ�����ִ��ԭ��
	���ȼ���������
	Ȼ�������ʽ�Ƿ�ƥ��
	�����û�������÷�������
	�����ִ������
*/
//	������Ӧ����������true��ʾ�ɹ��յ�����ִ�е��������false��ʾ�޷�ƥ��������������Э���ϵ�ƥ�䣬���û��������������޹�
bool CMonitorProtocol::CClient::OnRecvHandle(
	void* in_dat,										//	�յ��������׵�ַ
	int length											//	���ݳ���
	)
{
	//	����������
	if ((in_dat == NULL) || (in_dat == (void*)(-1)) || (length < MIN_DATA_SIZE))
	{
		return false;
	}

	//	����û������Ƿ����
	if ((this->pUserFunc == NULL) || (this->pUserFunc == (P_USER_CMP_CLIENT_FUNC)(-1)))
	{
		return false;
	}


	//	������ʽ
	//	�������ͷ
	if (
		(*((unsigned char*)in_dat + 0) != (unsigned char)0x28) ||
		(*((unsigned char*)in_dat + 1) != (unsigned char)0x77) ||
		(*((unsigned char*)in_dat + 2) != (unsigned char)0x93) ||
		(*((unsigned char*)in_dat + 3) != (unsigned char)0x5A)
		)
	{
		return false;
	}

	//	�������ID
	//	�������ID
	unsigned char cmd_id_h = *((unsigned char*)in_dat + 5);
	unsigned char cmd_id_l = *((unsigned char*)in_dat + 4);
	u_short cmd_ID = cmd_id_h * 0x100 + cmd_id_l;

	//	�������ID����
	unsigned char inv_cmd_id_h = *((unsigned char*)in_dat + 7);
	unsigned char inv_cmd_id_l = *((unsigned char*)in_dat + 6);
	u_short inv_cmd_ID = inv_cmd_id_h * 0x100 + inv_cmd_id_l;

	//	���㷴��
	inv_cmd_ID = ~inv_cmd_ID;

	//	�Աȷ���
	if (cmd_ID != inv_cmd_ID)
	{
		return false;
	}

	//	�������������
	unsigned char data_area_buff[4];
	memcpy_s(data_area_buff, 4, (unsigned char*)in_dat + 8, 4);
	int DataAreaCount = *((int*)data_area_buff);

	//	�������������
	if (DataAreaCount < 1)
	{
		return false;
	}

	//	��������������
	std::vector<SDataArea> RequestDataVec;
	int ByteCount = 0;
	for (int i = 0; i < DataAreaCount; ++i)
	{
		//	��ȡ�����ݿ�����ݳ���
		unsigned char data_length_buff[4];
		memcpy_s(data_length_buff, 4, (unsigned char*)in_dat + ByteCount + MIN_DATA_SIZE, 4);
		int DataLength = *((int*)data_length_buff);

		//	������ݳ��ȣ��������ش���
		if (DataLength <= 0)
		{
			//	�ͷ���Դ
			for (int i = 0; (RequestDataVec.begin() + i) != (RequestDataVec.end()); ++i)
			{
				delete[]((*(RequestDataVec.begin() + i)).pdat);
				RequestDataVec.erase(RequestDataVec.begin() + i);
				--i;
			}

			//	����ʧ��
			return false;
		}

		//	������ʱ������
		char* p_tmp_dat = new char[DataLength];

		//	��������
		memcpy_s(p_tmp_dat, DataLength, (unsigned char*)in_dat + ByteCount + MIN_DATA_SIZE + 4, DataLength);

		//	�������ݿ�ṹ
		SDataArea tmp_dat_area;
		tmp_dat_area.length = DataLength;
		tmp_dat_area.pdat = p_tmp_dat;

		//	��������
		RequestDataVec.insert(RequestDataVec.end(), tmp_dat_area);

		//	�ۼ��ֽ�
		ByteCount = ByteCount + DataLength + 4;
	}

	//	�ڻ�÷�������֮ǰ��Ҫ��֮ǰ�û�������ڴ�ռ��ͷŵ�
	for (int i = 0; (this->ReturnDataVec.begin() + i) != (this->ReturnDataVec.end()); ++i)
	{
		//	����û�������ڴ��Ƿ�Ϸ�
		if (((*(this->ReturnDataVec.begin() + i)).pdat == NULL) || ((*(this->ReturnDataVec.begin() + i)).pdat == (void*)(-1)))
		{
			//	�������ش���
			AfxMessageBox(_T("CMonitorProtocol����ʹ�ô����û�������ʹ���˷Ƿ�ָ�룡��"));
			::abort();
			return false;
		}

		//	�ͷ��ڴ�
		delete[]((*(this->ReturnDataVec.begin() + i)).pdat);
		this->ReturnDataVec.erase(this->ReturnDataVec.begin() + i);
		--i;
	}

	//	�����û���������÷�������
	this->ReturnDataVec = this->pUserFunc(RequestDataVec);

	//	���û�����֮�󣬼���û����ݵ�ָ���Ƿ���ȷ
	for (int i = 0; (this->ReturnDataVec.begin() + i) != (this->ReturnDataVec.end()); ++i)
	{
		//	����û�������ڴ��Ƿ�Ϸ�
		if (((*(this->ReturnDataVec.begin() + i)).pdat == NULL) || ((*(this->ReturnDataVec.begin() + i)).pdat == (void*)(-1)))
		{
			//	�������ش���
			AfxMessageBox(_T("CMonitorProtocol����ʹ�ô����û�������ʹ���˷Ƿ�ָ�룡��"));
			::abort();
			return false;
		}
	}

	//	�ͷ���Դ
	for (int i = 0; (RequestDataVec.begin() + i) != (RequestDataVec.end()); ++i)
	{
		delete[]((*(RequestDataVec.begin()+i)).pdat);
		RequestDataVec.erase(RequestDataVec.begin() + i);
		--i;
	}

	//-----------------------------------------------------------------------------------------
	//	�����ִ������
	//	��������һ����Ҫ�����ֽڿռ�
	int return_data_bytes_count = MIN_DATA_SIZE;
	for (int i = 0; (this->ReturnDataVec.begin() + i) != (this->ReturnDataVec.end()); ++i)
	{
		return_data_bytes_count = return_data_bytes_count + (*(this->ReturnDataVec.begin() + i)).length;
	}
	return_data_bytes_count = return_data_bytes_count + this->ReturnDataVec.size() * 4;

	//	���÷������ݻ������������һ�εĽ��������
	this->ReturnDataSize = 0;
	if (this->pReturnData != NULL)
	{
		delete[](this->pReturnData);
	}
	this->pReturnData = new char[return_data_bytes_count];

	//	���컺��������
	//	��������ͷ
	*((unsigned char*)(this->pReturnData) + 0) = (unsigned char)0x79;
	*((unsigned char*)(this->pReturnData) + 1) = (unsigned char)0x32;
	*((unsigned char*)(this->pReturnData) + 2) = (unsigned char)0x87;
	*((unsigned char*)(this->pReturnData) + 3) = (unsigned char)0x5A;

	//	�����������������ID������ID����
	*((unsigned char*)(this->pReturnData) + 4) = *((unsigned char*)(in_dat)+4);
	*((unsigned char*)(this->pReturnData) + 5) = *((unsigned char*)(in_dat)+5);
	*((unsigned char*)(this->pReturnData) + 6) = *((unsigned char*)(in_dat)+6);
	*((unsigned char*)(this->pReturnData) + 7) = *((unsigned char*)(in_dat)+7);

	//	����������������4���ֽ�
	int data_area_number = this->ReturnDataVec.size();
	*((unsigned char*)(this->pReturnData) + 8) = *((unsigned char*)(&data_area_number) + 0);
	*((unsigned char*)(this->pReturnData) + 9) = *((unsigned char*)(&data_area_number) + 1);
	*((unsigned char*)(this->pReturnData) + 10) = *((unsigned char*)(&data_area_number) + 2);
	*((unsigned char*)(this->pReturnData) + 11) = *((unsigned char*)(&data_area_number) + 3);

	//	����������
	ByteCount = 0;
	for (int i = 0; i < data_area_number; ++i)
	{
		//	����������������ݳ��ȵ�4���ֽ�
		int len = (*(this->ReturnDataVec.begin() + i)).length;
		*((unsigned char*)(this->pReturnData) + ByteCount + MIN_DATA_SIZE + 0) = *((unsigned char*)(&len) + 0);
		*((unsigned char*)(this->pReturnData) + ByteCount + MIN_DATA_SIZE + 1) = *((unsigned char*)(&len) + 1);
		*((unsigned char*)(this->pReturnData) + ByteCount + MIN_DATA_SIZE + 2) = *((unsigned char*)(&len) + 2);
		*((unsigned char*)(this->pReturnData) + ByteCount + MIN_DATA_SIZE + 3) = *((unsigned char*)(&len) + 3);

		//	����������
		memcpy_s((unsigned char*)(this->pReturnData) + ByteCount + MIN_DATA_SIZE + 4, len, (*(this->ReturnDataVec.begin() + i)).pdat, len);

		//	�ۼ��ֽ�
		ByteCount = ByteCount + len + 4;
	}

	//	���÷������ݵĳ���
	this->ReturnDataSize = return_data_bytes_count;

	//	�����ɹ�
	return true;
}

//	�õ����������Ļ�ִ�����׵�ַ
void* CMonitorProtocol::CClient::GetReturnData(void)
{
	//	�����������׵�ַ
	if (this->ReturnDataSize == 0) return NULL;
	else	return this->pReturnData;
}

//	�õ���ִ���ݵĳ���
int CMonitorProtocol::CClient::GetReturnDataSize(void)
{
	return this->ReturnDataSize;
}

//----------------------------------------------------------------------------------------
//	�޲������캯��
CMonitorProtocol::CServer::CServer()
{
	//	���ò���
	this->SendDataBuffSize = 0;
	this->pSendDataBuff = NULL;
	this->pUserFunc = NULL;
	this->cmd_id = 0;
}

//	��������
CMonitorProtocol::CServer::~CServer()
{
	//	��鷢�����ݻ������Ƿ���Ҫ�ͷ�
	if (this->pSendDataBuff != NULL)
	{
		this->SendDataBuffSize = 0;
		delete[](this->pSendDataBuff);
		this->pSendDataBuff = NULL;
	}
}

//	���ú��������óɹ�����true��ʧ�ܷ���false
bool CMonitorProtocol::CServer::Config(
	P_USER_CMP_SERVER_FUNC puserfunc				//	������û�����ָ��
	)
{
	//	����������
	if ((puserfunc == NULL) || (puserfunc == (P_USER_CMP_SERVER_FUNC)(-1)))
	{
		return false;
	}

	//	�����û�����
	this->pUserFunc = puserfunc;

	//	���óɹ�
	return true;
}

//	���췢�����ݣ�����ɹ�����true,ʧ�ܷ���false
bool CMonitorProtocol::CServer::BuildSendData(
	std::vector<SDataArea> in_vec
	)
{
	//	��������һ����Ҫ�����ֽڿռ�
	int return_data_bytes_count = MIN_DATA_SIZE;
	for (int i = 0; (in_vec.begin() + i) != (in_vec.end()); ++i)
	{
		return_data_bytes_count = return_data_bytes_count + (*(in_vec.begin() + i)).length;
	}
	return_data_bytes_count = return_data_bytes_count + in_vec.size() * 4;

	//	���÷������ݻ������������һ�εĽ��������
	this->SendDataBuffSize = 0;
	if (this->pSendDataBuff != NULL)
	{
		delete[](this->pSendDataBuff);
		this->pSendDataBuff = NULL;
	}
	this->pSendDataBuff = new char[return_data_bytes_count];

	//	���컺��������
	//	��������ͷ
	*((unsigned char*)(this->pSendDataBuff) + 0) = (unsigned char)0x28;
	*((unsigned char*)(this->pSendDataBuff) + 1) = (unsigned char)0x77;
	*((unsigned char*)(this->pSendDataBuff) + 2) = (unsigned char)0x93;
	*((unsigned char*)(this->pSendDataBuff) + 3) = (unsigned char)0x5A;

	//	�����������������ID������ID����
	++cmd_id;
	unsigned short inv_cmd_id = ~cmd_id;
	*((unsigned char*)(this->pSendDataBuff) + 4) = *((unsigned char*)(&cmd_id) + 0);
	*((unsigned char*)(this->pSendDataBuff) + 5) = *((unsigned char*)(&cmd_id) + 1);
	*((unsigned char*)(this->pSendDataBuff) + 6) = *((unsigned char*)(&inv_cmd_id) + 0);
	*((unsigned char*)(this->pSendDataBuff) + 7) = *((unsigned char*)(&inv_cmd_id) + 1);

	//	����������������4���ֽ�
	int data_area_number = in_vec.size();
	*((unsigned char*)(this->pSendDataBuff) + 8) = *((unsigned char*)(&data_area_number) + 0);
	*((unsigned char*)(this->pSendDataBuff) + 9) = *((unsigned char*)(&data_area_number) + 1);
	*((unsigned char*)(this->pSendDataBuff) + 10) = *((unsigned char*)(&data_area_number) + 2);
	*((unsigned char*)(this->pSendDataBuff) + 11) = *((unsigned char*)(&data_area_number) + 3);

	//	����������
	int ByteCount = 0;
	for (int i = 0; i < data_area_number; ++i)
	{
		//	����������������ݳ��ȵ�4���ֽ�
		int len = (*(in_vec.begin() + i)).length;
		*((unsigned char*)(this->pSendDataBuff) + ByteCount + MIN_DATA_SIZE + 0) = *((unsigned char*)(&len) + 0);
		*((unsigned char*)(this->pSendDataBuff) + ByteCount + MIN_DATA_SIZE + 1) = *((unsigned char*)(&len) + 1);
		*((unsigned char*)(this->pSendDataBuff) + ByteCount + MIN_DATA_SIZE + 2) = *((unsigned char*)(&len) + 2);
		*((unsigned char*)(this->pSendDataBuff) + ByteCount + MIN_DATA_SIZE + 3) = *((unsigned char*)(&len) + 3);

		//	����������
		memcpy_s((unsigned char*)(this->pSendDataBuff) + ByteCount + MIN_DATA_SIZE + 4, len, (*(in_vec.begin() + i)).pdat, len);

		//	�ۼ��ֽ�
		ByteCount = ByteCount + len + 4;
	}

	//	���÷������ݵĳ���
	this->SendDataBuffSize = return_data_bytes_count;

	//	����ͷ��û�����������
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

	//	����ɹ�
	return true;
}

//	������Ӧ����������true��ʾ�ɹ��յ�����ִ�е��������false��ʾ�޷�ƥ��������������Э���ϵ�ƥ�䣬���û��������������޹�
bool CMonitorProtocol::CServer::OnRecvHandle(
	void* in_dat,										//	�յ��������׵�ַ
	int length											//	���ݳ���
	)
{
	//	����������
	if ((in_dat == NULL) || (in_dat == (void*)(-1)) || (length < MIN_DATA_SIZE))
	{
		return false;
	}

	//	����û������Ƿ����
	if ((this->pUserFunc == NULL) || (this->pUserFunc == (P_USER_CMP_SERVER_FUNC)(-1)))
	{
		return false;
	}


	//	������ʽ
	//	�������ͷ
	if (
		(*((unsigned char*)in_dat + 0) != (unsigned char)0x79) ||
		(*((unsigned char*)in_dat + 1) != (unsigned char)0x32) ||
		(*((unsigned char*)in_dat + 2) != (unsigned char)0x87) ||
		(*((unsigned char*)in_dat + 3) != (unsigned char)0x5A)
		)
	{
		return false;
	}

	//	�������ID
	//	�������ID
	unsigned char cmd_id_h = *((unsigned char*)in_dat + 5);
	unsigned char cmd_id_l = *((unsigned char*)in_dat + 4);
	u_short cmd_ID = cmd_id_h * 0x100 + cmd_id_l;

	//	�������ID����
	unsigned char inv_cmd_id_h = *((unsigned char*)in_dat + 7);
	unsigned char inv_cmd_id_l = *((unsigned char*)in_dat + 6);
	u_short inv_cmd_ID = inv_cmd_id_h * 0x100 + inv_cmd_id_l;

	//	���㷴��
	inv_cmd_ID = ~inv_cmd_ID;

	//	�Աȷ���
	if (cmd_ID != inv_cmd_ID)
	{
		return false;
	}

	//	�������������
	unsigned char data_area_buff[4];
	memcpy_s(data_area_buff, 4, (unsigned char*)in_dat + 8, 4);
	int DataAreaCount = *((int*)data_area_buff);

	//	�������������
	if (DataAreaCount < 1)
	{
		return false;
	}

	//	��������������
	std::vector<SDataArea> RequestDataVec;
	int ByteCount = 0;
	for (int i = 0; i < DataAreaCount; ++i)
	{
		//	��ȡ�����ݿ�����ݳ���
		unsigned char data_length_buff[4];
		memcpy_s(data_length_buff, 4, (unsigned char*)in_dat + ByteCount + MIN_DATA_SIZE, 4);
		int DataLength = *((int*)data_length_buff);

		//	������ݳ��ȣ��������ش���
		if (DataLength <= 0)
		{
			//	�ͷ���Դ
			for (int i = 0; (RequestDataVec.begin() + i) != (RequestDataVec.end()); ++i)
			{
				delete[]((*(RequestDataVec.begin() + i)).pdat);
				RequestDataVec.erase(RequestDataVec.begin() + i);
				--i;
			}

			//	����ʧ��
			return false;
		}

		//	������ʱ������
		char* p_tmp_dat = new char[DataLength];

		//	��������
		memcpy_s(p_tmp_dat, DataLength, (unsigned char*)in_dat + ByteCount + MIN_DATA_SIZE + 4, DataLength);

		//	�������ݿ�ṹ
		SDataArea tmp_dat_area;
		tmp_dat_area.length = DataLength;
		tmp_dat_area.pdat = p_tmp_dat;

		//	��������
		RequestDataVec.insert(RequestDataVec.end(), tmp_dat_area);

		//	�ۼ��ֽ�
		ByteCount = ByteCount + DataLength + 4;
	}
	
	//	�����û���������÷�������
	this->pUserFunc(RequestDataVec);

	//	�ͷ���Դ
	for (int i = 0; (RequestDataVec.begin() + i) != (RequestDataVec.end()); ++i)
	{
		delete[]((*(RequestDataVec.begin() + i)).pdat);
		RequestDataVec.erase(RequestDataVec.begin() + i);
		--i;
	}

	//	ƥ��ɹ�
	return true;
}

//	��÷������ݵĻ������׵�ַ
void* CMonitorProtocol::CServer::GetSendDataBuff(void)
{
	//	�����������׵�ַ
	if (this->SendDataBuffSize == 0) return NULL;
	else	return this->pSendDataBuff;
}

//	��÷������ݵĻ���������
int CMonitorProtocol::CServer::GetSendDataBuffSize(void)
{
	return this->SendDataBuffSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//	ȫ���ɵ�TCP�ͻ�����

//	���մ�����
DWORD CMonitorProtocol::OnRecvCintegratedTCPClient_Func(LPVOID pParam)
{
	//	��ȡ����
	CMonitorProtocol::CintegratedTCPClient* pc = (CMonitorProtocol::CintegratedTCPClient*)(pParam);

	//	�����̰߳�ȫ����
	CThread::CThreadRunSafe msafethread(pc->m_recv_thread);

	//	���巵��ֵ
	int re = 0;

	//	��ʼ���Ѿ����������Ĵ���
	int already_reconnect_count = 0;

	//	���崦������־,û�д���Ĵ���Ϊ0,���ݴ������Ϊ1,���Ӵ������Ϊ2
	int error_flag = 0;

	//	����ѭ��
	while (1)
	{
		//---------------------------------------------------------------------------------------------------------
		//	���֮ǰ�Ľ�����ͷ��ڴ�
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

		//	����Ͽ�����
		if (pc->GetDisconnectFlag() == true)
		{
			return 0;
		}

		//---------------------------------------------------------------------------------------------------------
		//	������ִ�е������ʱ�򣬱�ʾ�Ѿ�������������Ϊ������������
		//	���Ϊ���Ӵ���
		if (error_flag == 2)
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

			}	//	���ߺ������������������
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
			}	//	ָ�����ߺ�����������Ĵ������
		}


		//---------------------------------------------------------------------------------------------------------
		//	��ʼ�����ȡ����ͷ����
		//	��մ����־
		error_flag = 0;				//	��մ����־
		
		//	��ȡһ������ͷ�ֽ�
		char* phead = new char[1];
		re = recv(pc->sockClient, phead, 1, 0);

		//	�������
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](phead);
			error_flag = 2;			//	���Ӵ���
			continue;
		}

		//	�������
		int head = phead[0] & 0x000000FF;
		if (phead[0] != 0x28)
		{
			delete[](phead);
			error_flag = 1;			//	���ݴ���
			continue;
		}

		//-------------------------------
		//	��ȡһ������ͷ�ֽ�
		re = recv(pc->sockClient, phead, 1, 0);

		//	�������
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](phead);
			error_flag = 2;				//	���Ӵ���
			continue;
		}

		//	�������
		head = phead[0] & 0x000000FF;
		if (head != 0x77)
		{
			delete[](phead);
			error_flag = 1;				//	���ݴ���
			continue;
		}

		//-------------------------------
		//	��ȡһ������ͷ�ֽ�
		re = recv(pc->sockClient, phead, 1, 0);

		//	�������
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](phead);
			error_flag = 2;				//	���Ӵ���
			continue;
		}

		//	�������
		head = phead[0] & 0x000000FF;
		if (head != 0x93)
		{
			delete[](phead);
			error_flag = 1;				//	���ݴ���
			continue;
		}

		//-------------------------------
		//	��ȡһ������ͷ�ֽ�
		re = recv(pc->sockClient, phead, 1, 0);

		//	�������
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](phead);
			error_flag = 2;				//	���Ӵ���
			continue;
		}

		//	�������
		head = phead[0] & 0x000000FF;
		if (head != 0x5A)
		{
			delete[](phead);
			error_flag = 1;				//	���ݴ���
			continue;
		}

		//-------------------------------
		//	�ͷ�����ͷ����
		delete[](phead);

		//---------------------------------------------------------------------------------------------------------
		//	��������ID������ID����Ļ�ȡ��ⲿ��
		//	����ID�ڴ�
		char* pid = new char[4];

		//	��socket���ж�ȡ4���ֽ�
		re = recv(pc->sockClient, pid, 4, 0);

		//	�������
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](pid);
			error_flag = 2;				//	���Ӵ���
			continue;
		}

		//	�������
		u_short id = (pid[1] & 0x000000FF) * 0x100 + (pid[0] & 0x000000FF);
		u_short inv_id = (pid[3] & 0x000000FF) * 0x100 + (pid[2] & 0x000000FF);
		inv_id = ~inv_id;
		if (id != inv_id)
		{
			delete[](pid);
			error_flag = 1;				//	���ݴ���
			continue;
		}

		//	�ͷ�ID�ڴ�
		delete[](pid);

		//---------------------------------------------------------------------------------------------------------
		//	��ȡ����������
		//	�����������ڴ�
		char* pnum = new char[4];

		//	��socket���ж�ȡ4���ֽ�
		re = recv(pc->sockClient, pnum, 4, 0);

		//	�������
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](pnum);
			error_flag = 2;				//	���Ӵ���
			continue;
		}

		//	�������
		int area_num = 0;
		memcpy_s(&area_num, 4, pnum, 4);
		if (area_num <= 0)
		{
			delete[](pnum);
			error_flag = 1;				//	���ݴ���
			continue;
		}

		//	��ʼѭ����ȡ������
		for (int k = 0; k < area_num; ++k)
		{
			//	��ȡ���������Ĵ�С
			char* plength = new char[4];

			//	��socket���ж�ȡ4���ֽ�
			re = recv(pc->sockClient, plength, 4, 0);

			//	�������
			if ((re == SOCKET_ERROR) || (re == 0))
			{
				delete[](pnum);
				delete[](plength);
				error_flag = 2;				//	���Ӵ���
				break;
			}

			//	�������
			int area_length = 0;
			memcpy_s(&area_length, 4, plength, 4);
			if (area_length <= 0)
			{
				delete[](pnum);
				delete[](plength);
				error_flag = 1;				//	���ݴ���
				break;
			}

			//	��ȡ������
			char* area_buff = new char[area_length];

			//	��socket���ж�ȡarea_buff���ֽ�
			re = recv(pc->sockClient, area_buff, area_length, 0);

			//	�������
			if ((re == SOCKET_ERROR) || (re == 0))
			{
				//	20170331���޸�BUG�����������ӳ��ִ����ʱ����area_buff���ڴ�й©
				delete[](area_buff);

				delete[](plength);
				error_flag = 2;				//	���Ӵ���
				break;
			}

			//	�����ȡ�����ݸ�����ͬ����ѭ����ȡ
			int total_bytes = area_length;
			int already_bytes = re;
			while (already_bytes != total_bytes)
			{
				//	��socket���ж�ȡarea_buff���ֽ�
				re = ::recv(pc->sockClient, area_buff + already_bytes, total_bytes - already_bytes, 0);

				//	�������
				if ((re == SOCKET_ERROR) || (re == 0))
				{
					//	20170331���޸�BUG�����������ӳ��ִ����ʱ����area_buff���ڴ�й©
					delete[](area_buff);

					delete[](plength);
					error_flag = 2;				//	���Ӵ���
					break;
				}

				//	�ۼӶ�ȡ���ֽ�
				already_bytes = already_bytes + re;
			}

			//	�������
			if ((re == SOCKET_ERROR) || (re == 0))
			{
				error_flag = 2;				//	���Ӵ���
				break;
			}

			//	����������
			SDataArea tmp_dat;
			tmp_dat.length = area_length;
			tmp_dat.pdat = area_buff;
			pc->RecvVec.insert(pc->RecvVec.end(), tmp_dat);
			
			//	�ͷ���������С�ڴ�
			delete[](plength);
		}

		//	�ͷ��������ڴ�
		delete[](pnum);

		//	20170331���޸���BUG
		//	��������������ʱ��������⣬���������²���
		if (error_flag != 0) continue;

		//---------------------------------------------------------------------------------------------------------
		//	�����û�����
		pc->ReturnVec = pc->pUserFunc(pc->RecvVec);

		//---------------------------------------------------------------------------------------------------------
		//	��������һ����Ҫ�����ֽڿռ�
		int return_data_bytes_count = MIN_DATA_SIZE;
		for (int i = 0; (pc->ReturnVec.begin() + i) != (pc->ReturnVec.end()); ++i)
		{
			return_data_bytes_count = return_data_bytes_count + (*(pc->ReturnVec.begin() + i)).length;
		}
		return_data_bytes_count = return_data_bytes_count + pc->ReturnVec.size() * 4;

		//	�����ִ���ݵĻ���
		char* pReturnData = new char[return_data_bytes_count];

		//	���컺��������
		//	��������ͷ
		*((unsigned char*)(pReturnData)+0) = (unsigned char)0x79;
		*((unsigned char*)(pReturnData)+1) = (unsigned char)0x32;
		*((unsigned char*)(pReturnData)+2) = (unsigned char)0x87;
		*((unsigned char*)(pReturnData)+3) = (unsigned char)0x5A;

		//	�����������������ID������ID����
		*((unsigned char*)(pReturnData)+4) = *((unsigned char*)(&id)+0);
		*((unsigned char*)(pReturnData)+5) = *((unsigned char*)(&id)+1);
		inv_id = ~id;
		*((unsigned char*)(pReturnData)+6) = *((unsigned char*)(&inv_id)+0);
		*((unsigned char*)(pReturnData)+7) = *((unsigned char*)(&inv_id)+1);

		//	����������������4���ֽ�
		int data_area_number = pc->ReturnVec.size();
		*((unsigned char*)(pReturnData)+8) = *((unsigned char*)(&data_area_number) + 0);
		*((unsigned char*)(pReturnData)+9) = *((unsigned char*)(&data_area_number) + 1);
		*((unsigned char*)(pReturnData)+10) = *((unsigned char*)(&data_area_number) + 2);
		*((unsigned char*)(pReturnData)+11) = *((unsigned char*)(&data_area_number) + 3);

		//	����������
		int ByteCount = 0;
		for (int i = 0; i < data_area_number; ++i)
		{
			//	����������������ݳ��ȵ�4���ֽ�
			int len = (*(pc->ReturnVec.begin() + i)).length;
			*((unsigned char*)(pReturnData)+ByteCount + MIN_DATA_SIZE + 0) = *((unsigned char*)(&len) + 0);
			*((unsigned char*)(pReturnData)+ByteCount + MIN_DATA_SIZE + 1) = *((unsigned char*)(&len) + 1);
			*((unsigned char*)(pReturnData)+ByteCount + MIN_DATA_SIZE + 2) = *((unsigned char*)(&len) + 2);
			*((unsigned char*)(pReturnData)+ByteCount + MIN_DATA_SIZE + 3) = *((unsigned char*)(&len) + 3);

			//	����������
			memcpy_s((unsigned char*)(pReturnData)+ByteCount + MIN_DATA_SIZE + 4, len, (*(pc->ReturnVec.begin() + i)).pdat, len);

			//	�ۼ��ֽ�
			ByteCount = ByteCount + len + 4;
		}

		//	��������
		re = pc->TrySend(pReturnData, return_data_bytes_count);

		//	�������
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](pReturnData);
			error_flag = 2;				//	���Ӵ���
			continue;
		}

		//	�ͷŻ�ִ���ݻ�����
		delete[](pReturnData);

		//---------------------------------------------------------------------------------------------------------
	}	//	������while(1)����

	//	�߳���������
	return 0;
}

//	��ȡ�˿ڱ�־,tureΪҪ��Ͽ���falseΪû��Ҫ��
bool CMonitorProtocol::CintegratedTCPClient::GetDisconnectFlag(void)
{
	bool tmp_flag = false;
	this->Disconnect_flag_CS.Enter();
	tmp_flag = this->Disconnect_flag;
	this->Disconnect_flag_CS.Leave();
	return tmp_flag;
}

//	���öϿ���־,tureΪҪ��Ͽ���falseΪû��Ҫ��
void CMonitorProtocol::CintegratedTCPClient::SetDisconnectFlag(bool flag)
{
	this->Disconnect_flag_CS.Enter();
	this->Disconnect_flag = flag;
	this->Disconnect_flag_CS.Leave();
}

//	���ú�����Ĭ������Ϊ ���ߺ������������ӣ������������Դ������ɹ�����0��ʧ�ܷ���SOCKET_ERROR
int CMonitorProtocol::CintegratedTCPClient::Config(
	char* remote_ip,										//	Զ��������IP��ַ
	u_short remote_port,									//	Զ�������Ķ˿�
	bool now_conn,											//	�Ƿ������������ӣ�trueΪ���óɹ����������ӣ�falseΪ�������ã�����������
	P_USER_ON_RECV_DATAAREA_CLIENT_FUNC puserfunc,			//	�û�����
	P_USER_ON_TCP_SERVER_BREAK_FUNC ponbreakfunc,			//	����������Ͽ����ӵ�ʱ�������õ���Ӧ����
	int retry_sec,											//	���������ĳ�ʱ������0��ʾ���ߺ������������ӣ�>0��ʾ���ߺ󾭹���ú����Խ�������
	int retry_count											//	���Դ�����-1��ʾ�������Դ�����0��ʾ���ߺ󲻽����������ӣ�>0��ʾ���ߺ����ԵĴ���
	)
{
	//	�������
	if ((retry_sec < 0) || (retry_count < -1) || (puserfunc == NULL) || (puserfunc == (P_USER_ON_RECV_DATAAREA_CLIENT_FUNC)(-1)))
	{
		return SOCKET_ERROR;
	}

	//	����Ѿ�������
	if (this->init_flag == true)
	{
		//	Ϊ�˷�ֹ�����������Ƶ��µĶϿ��������������ٴ�ִ����ֹ�߳�
		SetDisconnectFlag(true);
		Sleep(10);
		while (this->m_recv_thread.GetState() != CThread::STOP) Sleep(1);
		SetDisconnectFlag(false);

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

	//	�����û�����
	this->pUserFunc = puserfunc;

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
int CMonitorProtocol::CintegratedTCPClient::Connect(void)
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
		OnRecvCintegratedTCPClient_Func,
		this,
		CThread::RUN
		);

	//	���ӳɹ�
	return 0;
}

//	�Ͽ�����
void CMonitorProtocol::CintegratedTCPClient::Disconnect(void)
{
	//	����Ƿ�û������
	if (this->init_flag != true)
	{
		return;
	}

	//	��ֹ�߳�
	SetDisconnectFlag(true);

	//	�ͷ�SOCKET
	if (this->sockClient != NULL) closesocket(this->sockClient);
	this->sockClient = NULL;

	while (this->m_recv_thread.GetState() != CThread::STOP) Sleep(1);
	SetDisconnectFlag(false);

}

//	�޲������캯��
CMonitorProtocol::CintegratedTCPClient::CintegratedTCPClient()
{
	this->Disconnect_flag = false;
	this->pUserFunc = NULL;
}

//	��������
CMonitorProtocol::CintegratedTCPClient::~CintegratedTCPClient()
{
	//	����ֻ��Ҫ�ȴ��߳̽����������������ʱ�򣬲Ż�ִ���߳�������������������ڴ�ᰲȫ�ͷ�
	this->Disconnect();
}


////////////////////////////////////////////////////////////////////////////////////////////////
//	ȫ����TCP������������

//	���մ�����
DWORD CMonitorProtocol::OnRecvCintegratedTCPServerHost_Func(LPVOID pParam)
{
	//	�õ��������
	CMonitorProtocol::CintegratedTCPServerHost* pCTCPServerHost = (CMonitorProtocol::CintegratedTCPServerHost*)pParam;

	//	�����̰߳�ȫ����
	CThread::CThreadRunSafe msafethread(pCTCPServerHost->m_recv_thread);

	//	���崦������־,û�д���Ĵ���Ϊ0,���ݴ������Ϊ1,���Ӵ������Ϊ2
	int error_flag = 0;

	//	���巵��ֵ
	int re = 0;

	//	��ȡSOCKET��Ϊ����߽���Ч�ʣ�����ȡ�ŵ�ѭ����
	pCTCPServerHost->opt_m_con_socket_cs.Enter();
	SOCKET tmp_socket = pCTCPServerHost->m_con_socket;
	pCTCPServerHost->opt_m_con_socket_cs.Leave();

	//	�������ѭ��
	while (1)
	{
		//----------------------------------------------------------------------------------------------------------------
		//	���֮ǰ�Ľ�����ͷ��ڴ棬��������ͷŽ��յ�����Դ�ڴ�
		for (int i = 0; (pCTCPServerHost->RecvVec.begin() + i) != (pCTCPServerHost->RecvVec.end()); ++i)
		{
			delete[]((*(pCTCPServerHost->RecvVec.begin() + i)).pdat);
			pCTCPServerHost->RecvVec.erase(pCTCPServerHost->RecvVec.begin() + i);
			--i;
		}
		pCTCPServerHost->RecvVec.clear();

		//----------------------------------------------------------------------------------------------------------------
		//	��������
		//	���Ϊ���Ӵ���
		if (error_flag == 2)
		{
			//	����ʧ�ܱ�ʶ
			pCTCPServerHost->opt_fail_status_cs.Enter();
			pCTCPServerHost->fail_status = true;
			pCTCPServerHost->opt_fail_status_cs.Leave();

			//	�˳������߳�
			return 0;
		}

		//----------------------------------------------------------------------------------------------------------------
		//	��������־
		error_flag = 0;

		//----------------------------------------------------------------------------------------------------------------
		//	���մ�������
		//	��ȡһ������ͷ�ֽ�
		char* phead = new char[1];
		re = recv(tmp_socket, phead, 1, 0);

		//	�������
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](phead);
			error_flag = 2;			//	���Ӵ���
			continue;
		}

		//	�������
		int head = phead[0] & 0x000000FF;
		if (phead[0] != 0x79)
		{
			delete[](phead);
			error_flag = 1;			//	���ݴ���
			continue;
		}

		//-------------------------------
		//	��ȡһ������ͷ�ֽ�
		re = recv(tmp_socket, phead, 1, 0);

		//	�������
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](phead);
			error_flag = 2;				//	���Ӵ���
			continue;
		}

		//	�������
		head = phead[0] & 0x000000FF;
		if (head != 0x32)
		{
			delete[](phead);
			error_flag = 1;				//	���ݴ���
			continue;
		}

		//-------------------------------
		//	��ȡһ������ͷ�ֽ�
		re = recv(tmp_socket, phead, 1, 0);

		//	�������
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](phead);
			error_flag = 2;				//	���Ӵ���
			continue;
		}

		//	�������
		head = phead[0] & 0x000000FF;
		if (head != 0x87)
		{
			delete[](phead);
			error_flag = 1;				//	���ݴ���
			continue;
		}

		//-------------------------------
		//	��ȡһ������ͷ�ֽ�
		re = recv(tmp_socket, phead, 1, 0);

		//	�������
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](phead);
			error_flag = 2;				//	���Ӵ���
			continue;
		}

		//	�������
		head = phead[0] & 0x000000FF;
		if (head != 0x5A)
		{
			delete[](phead);
			error_flag = 1;				//	���ݴ���
			continue;
		}

		//-------------------------------
		//	�ͷ�����ͷ����
		delete[](phead);

		//---------------------------------------------------------------------------------------------------------
		//	��������ID������ID����Ļ�ȡ��ⲿ��
		//	����ID�ڴ�
		char* pid = new char[4];

		//	��socket���ж�ȡ4���ֽ�
		re = recv(tmp_socket, pid, 4, 0);

		//	�������
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](pid);
			error_flag = 2;				//	���Ӵ���
			continue;
		}

		//	�������
		u_short id = pid[1] * 0x100 + (pid[0]&0x00ff);
		u_short inv_id = pid[3] * 0x100 + (pid[2]&0x00ff);
		inv_id = ~inv_id;
		if (id != inv_id)
		{
			delete[](pid);
			error_flag = 1;				//	���ݴ���
			continue;
		}

		//	�ͷ�ID�ڴ�
		delete[](pid);

		//---------------------------------------------------------------------------------------------------------
		//	��ȡ����������
		//	�����������ڴ�
		char* pnum = new char[4];

		//	��socket���ж�ȡ4���ֽ�
		re = recv(tmp_socket, pnum, 4, 0);

		//	�������
		if ((re == SOCKET_ERROR) || (re == 0))
		{
			delete[](pnum);
			error_flag = 2;				//	���Ӵ���
			continue;
		}

		//	�������
		int area_num = 0;
		memcpy_s(&area_num, 4, pnum, 4);
		if (area_num <= 0)
		{
			delete[](pnum);
			error_flag = 1;				//	���ݴ���
			continue;
		}

		//	��ʼѭ����ȡ������
		for (int k = 0; k < area_num; ++k)
		{
			//	��ȡ���������Ĵ�С
			char* plength = new char[4];

			//	��socket���ж�ȡ4���ֽ�
			re = recv(tmp_socket, plength, 4, 0);

			//	�������
			if ((re == SOCKET_ERROR) || (re == 0))
			{
				delete[](pnum);
				delete[](plength);
				error_flag = 2;				//	���Ӵ���
				break;
			}

			//	�������
			int area_length = 0;
			memcpy_s(&area_length, 4, plength, 4);
			if (area_length <= 0)
			{
				delete[](pnum);
				delete[](plength);
				error_flag = 1;				//	���ݴ���
				break;
			}

			//	��ȡ������
			char* area_buff = new char[area_length];

			//	��socket���ж�ȡarea_buff���ֽ�
			re = ::recv(tmp_socket, area_buff, area_length, 0);

			//	�������
			if ((re == SOCKET_ERROR) || (re == 0))
			{
				//	20170331���޸�BUG�����������ӳ��ִ����ʱ����area_buff���ڴ�й©
				delete[](area_buff);

				delete[](plength);
				error_flag = 2;				//	���Ӵ���
				break;
			}

			//	�����ȡ�����ݸ�����ͬ����ѭ����ȡ
			int total_bytes = area_length;
			int already_bytes = re;
			while (already_bytes != total_bytes)
			{
				//	��socket���ж�ȡarea_buff���ֽ�
				re = ::recv(tmp_socket, area_buff + already_bytes, total_bytes - already_bytes, 0);

				//	�������
				if ((re == SOCKET_ERROR) || (re == 0))
				{
					//	20170331���޸�BUG�����������ӳ��ִ����ʱ����area_buff���ڴ�й©
					delete[](area_buff);

					delete[](plength);
					error_flag = 2;				//	���Ӵ���
					break;
				}

				//	�ۼӶ�ȡ���ֽ�
				already_bytes = already_bytes + re;
			}

			//	�������
			if ((re == SOCKET_ERROR) || (re == 0))
			{
				error_flag = 2;				//	���Ӵ���
				break;
			}

			//	����������
			SDataArea tmp_dat;
			tmp_dat.length = area_length;
			tmp_dat.pdat = area_buff;
			pCTCPServerHost->RecvVec.insert(pCTCPServerHost->RecvVec.end(), tmp_dat);

			//	�ͷ���������С�ڴ�
			delete[](plength);
		}

		//	�ͷ��������ڴ�
		delete[](pnum);

		//	20170331���޸�BUG�����ڽ�����������ʱ����ִ���ʱ��ʱ����������û������
		//	������ݺ������Ƿ�������⣬�����������ʱ������ִ���û�����
		if (error_flag != 0) continue;

		//---------------------------------------------------------------------------------------------------------
		//	�����û�����
		pCTCPServerHost->pUserFunc(tmp_socket, pCTCPServerHost->RecvVec);

		//---------------------------------------------------------------------------------------------------------
	}	//	������while(1)ѭ������

	return 0;
}

//	��ȡ�˿ڱ�־,tureΪҪ��Ͽ���falseΪû��Ҫ��
bool CMonitorProtocol::CintegratedTCPServerHost::GetDisconnectFlag(void)
{
	bool tmp_flag = false;
	this->Disconnect_flag_CS.Enter();
	tmp_flag = this->Disconnect_flag;
	this->Disconnect_flag_CS.Leave();
	return tmp_flag;
}

//	���öϿ���־,tureΪҪ��Ͽ���falseΪû��Ҫ��
void CMonitorProtocol::CintegratedTCPServerHost::SetDisconnectFlag(bool flag)
{
	this->Disconnect_flag_CS.Enter();
	this->Disconnect_flag = flag;
	this->Disconnect_flag_CS.Leave();
}

//	���ú���,�����Ƿ���0�������ʱ�򷵻�SOCKET_ERROR
int CMonitorProtocol::CintegratedTCPServerHost::Config(
	SOCKET m_socket,										//	���ӵ�SOCKET
	P_USER_ON_RECV_DATAAREA_SERVER_FUNC puserfunc			//	����ĺ���ָ��
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

		//	��ֹ�߳�
		SetDisconnectFlag(true);
		//	�ͷ�SOCKET
		opt_m_con_socket_cs.Enter();
		if (this->m_con_socket != NULL) closesocket(this->m_con_socket);
		this->m_con_socket = NULL;
		opt_m_con_socket_cs.Leave();
		while (this->m_recv_thread.GetState() != CThread::STOP) Sleep(1);
		SetDisconnectFlag(false);
	}

	//	�����������Ƿ�Ϸ�
	if ((m_socket == NULL) || (m_socket == (SOCKET)-1) || (puserfunc == NULL) || (puserfunc == (P_USER_ON_RECV_DATAAREA_SERVER_FUNC)-1))
	{
		return SOCKET_ERROR;
	}

	//	��ֵ����
	opt_m_con_socket_cs.Enter();
	this->m_con_socket = m_socket;
	opt_m_con_socket_cs.Leave();
	this->pUserFunc = puserfunc;
	this->m_recv_thread.Config(
		OnRecvCintegratedTCPServerHost_Func,
		this,
		CThread::RUN
		);

	//	���óɹ�
	this->init_flag = true;
	return 0;
}

//	�������ݣ�����������Ϊ������������Ϊ��ͻ��˷�������,�����Ƿ����Ѿ����͵��ֽڣ������ʱ�򷵻�SOCKET_ERROR
int CMonitorProtocol::CintegratedTCPServerHost::Send(
	std::vector<SDataArea> in_vec
	)
{
	//	�����ǰΪʧ��״̬����������
	opt_fail_status_cs.Enter();
	bool tmp_fail_status = this->fail_status;
	opt_fail_status_cs.Leave();
	if (tmp_fail_status == true)
	{
		//	�����ͷ�����������
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

	//	�ж��Ƿ����ã����û������
	if (this->init_flag == false)
	{
		//	�����ͷ�����������
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

	//	������������Ƿ�Ϸ�,�������е��������ݿ飬�ֱ��鳤�Ⱥ�ָ��
	for (int i = 0; (in_vec.begin() + i) != (in_vec.end()); ++i)
	{
		//	����Ƿ�
		if (((*(in_vec.begin() + i)).length <= 0) || ((*(in_vec.begin() + i)).pdat == NULL) || ((*(in_vec.begin() + i)).pdat == (char*)(-1)))
		{
			//	�����ͷ�����������
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

	//	��������һ����Ҫ�����ֽڿռ�
	int return_data_bytes_count = MIN_DATA_SIZE;
	for (int i = 0; (in_vec.begin() + i) != (in_vec.end()); ++i)
	{
		return_data_bytes_count = return_data_bytes_count + (*(in_vec.begin() + i)).length;
	}
	return_data_bytes_count = return_data_bytes_count + in_vec.size() * 4;

	//	�����ִ���ݵĻ���
	char* pReturnData = new char[return_data_bytes_count];

	//	���컺��������
	//	��������ͷ
	*((unsigned char*)(pReturnData)+0) = (unsigned char)0x28;
	*((unsigned char*)(pReturnData)+1) = (unsigned char)0x77;
	*((unsigned char*)(pReturnData)+2) = (unsigned char)0x93;
	*((unsigned char*)(pReturnData)+3) = (unsigned char)0x5A;

	//	�����������������ID������ID����
	static u_short id = 0;
	++id;
	*((unsigned char*)(pReturnData)+4) = *((unsigned char*)(&id) + 0);
	*((unsigned char*)(pReturnData)+5) = *((unsigned char*)(&id) + 1);
	u_short inv_id = ~id;
	*((unsigned char*)(pReturnData)+6) = *((unsigned char*)(&inv_id) + 0);
	*((unsigned char*)(pReturnData)+7) = *((unsigned char*)(&inv_id) + 1);

	//	����������������4���ֽ�
	int data_area_number = in_vec.size();
	*((unsigned char*)(pReturnData)+8) = *((unsigned char*)(&data_area_number) + 0);
	*((unsigned char*)(pReturnData)+9) = *((unsigned char*)(&data_area_number) + 1);
	*((unsigned char*)(pReturnData)+10) = *((unsigned char*)(&data_area_number) + 2);
	*((unsigned char*)(pReturnData)+11) = *((unsigned char*)(&data_area_number) + 3);

	//	����������
	int ByteCount = 0;
	for (int i = 0; i < data_area_number; ++i)
	{
		//	����������������ݳ��ȵ�4���ֽ�
		int len = (*(in_vec.begin() + i)).length;
		*((unsigned char*)(pReturnData)+ByteCount + MIN_DATA_SIZE + 0) = *((unsigned char*)(&len) + 0);
		*((unsigned char*)(pReturnData)+ByteCount + MIN_DATA_SIZE + 1) = *((unsigned char*)(&len) + 1);
		*((unsigned char*)(pReturnData)+ByteCount + MIN_DATA_SIZE + 2) = *((unsigned char*)(&len) + 2);
		*((unsigned char*)(pReturnData)+ByteCount + MIN_DATA_SIZE + 3) = *((unsigned char*)(&len) + 3);

		//	����������
		memcpy_s((unsigned char*)(pReturnData)+ByteCount + MIN_DATA_SIZE + 4, len, (*(in_vec.begin() + i)).pdat, len);

		//	�ۼ��ֽ�
		ByteCount = ByteCount + len + 4;
	}

	//	��������
	opt_m_con_socket_cs.Enter();
	SOCKET tmp_socket = this->m_con_socket;
	opt_m_con_socket_cs.Leave();
	int re = ::send(
		tmp_socket,
		pReturnData,
		return_data_bytes_count,
		0
		);
	
	//	�ͷŻ�ִ���ݻ�����
	delete[](pReturnData);

	//	�����ͷ�����������
	for (int j = 0; (in_vec.begin() + j) != (in_vec.end()); ++j)
	{
		if (((*(in_vec.begin() + j)).pdat != NULL) && ((*(in_vec.begin() + j)).pdat != (char*)(-1)))
		{
			delete[]((*(in_vec.begin() + j)).pdat);
		}
		in_vec.erase(in_vec.begin() + j);
		--j;
	}
	
	//	����Ƿ���ʧ��
	if (re == SOCKET_ERROR)
	{
		//	����ʧ�ܱ�ʶ
		opt_fail_status_cs.Enter();
		this->fail_status = true;
		opt_fail_status_cs.Leave();

		//	��ֹ�߳�
		SetDisconnectFlag(true);
		//	�ͷ�SOCKET
		opt_m_con_socket_cs.Enter();
		if (this->m_con_socket != NULL) closesocket(this->m_con_socket);
		this->m_con_socket = NULL;
		opt_m_con_socket_cs.Leave();
		while (this->m_recv_thread.GetState() != CThread::STOP) Sleep(1);
		SetDisconnectFlag(false);
	}

	return re;
}

//	�Ͽ�����
void CMonitorProtocol::CintegratedTCPServerHost::Disconnect(void)
{
	//	����Ѿ������˵Ļ���������û��ʧ�ܵ�ʱ��
	if (this->init_flag == true)
	{
		//	��ձ�־����ֹ������ʱ��������
		this->init_flag = false;

		//	��ֹ�߳�
		SetDisconnectFlag(true);
		//	�ͷ�SOCKET
		opt_m_con_socket_cs.Enter();
		if (this->m_con_socket != NULL) closesocket(this->m_con_socket);
		this->m_con_socket = NULL;
		opt_m_con_socket_cs.Leave();
		while (this->m_recv_thread.GetState() != CThread::STOP) Sleep(1);
		SetDisconnectFlag(false);
	}
}

//	�޲������캯��
CMonitorProtocol::CintegratedTCPServerHost::CintegratedTCPServerHost()
{
	this->Disconnect_flag = false;
	this->pUserFunc = NULL;
}

//	��������
CMonitorProtocol::CintegratedTCPServerHost::~CintegratedTCPServerHost()
{
	//	�Ͽ���ͻ��˵����Ӳ��ͷ���Դ
	this->Disconnect();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	ȫ���ɵķ�����������

//	���������̺߳���
DWORD CMonitorProtocol::integrated_TCP_SERVER_MANAGE_ACCEPT_THREAD(LPVOID pParam)
{
	//	ת���������
	CintegratedTCPServerManage* pCObj = (CintegratedTCPServerManage*)(pParam);

	//	�����̰߳�ȫ����
	CThread::CThreadRunSafe msafethread(pCObj->m_accept_thread);

	//	������ʱ����
	SOCKET sockConnection = NULL;
	SOCKADDR_IN addrClient;
	int len = sizeof(addrClient);


	//	�������ӵȴ�ѭ��
	while (1)
	{
		//	����ʽӦ����������
		sockConnection = ::accept(pCObj->socketServer, (SOCKADDR *)&addrClient, &len);

		//	��������Ƿ�����
		if ((sockConnection == NULL) || (pCObj->GetDisconnectACCEPTFlag() == true))
		{
			return 0;
		}

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
DWORD CMonitorProtocol::integrated_TCP_SERVER_MANAGE_FIND_ERROR_THREAD(LPVOID pParam)
{
	//	ת���������
	CintegratedTCPServerManage* pCObj = (CintegratedTCPServerManage*)(pParam);

	//	�����̰߳�ȫ����
	CThread::CThreadRunSafe msafethread(pCObj->m_find_error_thread);

	//	�����Ҵ����ѭ��
	while (1)
	{
		//	���Ƚ�����ʱ��ÿ��200ms����һ�μ�����
		Sleep(200);
		
		//	��������Ƿ�����
		if (pCObj->GetDisconnectFINDERRORFlag() == true)
		{
			return 0;
		}

		//	ִ���Զ��Ƴ�����
		pCObj->AutoRemoveError();
	}

	return 0;
}

//	��ȡ�˿ڱ�־,tureΪҪ��Ͽ���falseΪû��Ҫ��
bool CMonitorProtocol::CintegratedTCPServerManage::GetDisconnectACCEPTFlag(void)
{
	bool tmp_flag = false;
	this->Disconnect_ACCEPT_flag_CS.Enter();
	tmp_flag = this->Disconnect_ACCEPT_flag;
	this->Disconnect_ACCEPT_flag_CS.Leave();
	return tmp_flag;
}

//	���öϿ���־,tureΪҪ��Ͽ���falseΪû��Ҫ��
void CMonitorProtocol::CintegratedTCPServerManage::SetDisconnectACCEPTFlag(bool flag)
{
	this->Disconnect_ACCEPT_flag_CS.Enter();
	this->Disconnect_ACCEPT_flag = flag;
	this->Disconnect_ACCEPT_flag_CS.Leave();
}

//	��ȡ�˿ڱ�־,tureΪҪ��Ͽ���falseΪû��Ҫ��
bool CMonitorProtocol::CintegratedTCPServerManage::GetDisconnectFINDERRORFlag(void)
{
	bool tmp_flag = false;
	this->Disconnect_FIND_ERROR_flag_CS.Enter();
	tmp_flag = this->Disconnect_FIND_ERROR_flag;
	this->Disconnect_FIND_ERROR_flag_CS.Leave();
	return tmp_flag;
}

//	���öϿ���־,tureΪҪ��Ͽ���falseΪû��Ҫ��
void CMonitorProtocol::CintegratedTCPServerManage::SetDisconnectFINDERRORFlag(bool flag)
{
	this->Disconnect_FIND_ERROR_flag_CS.Enter();
	this->Disconnect_FIND_ERROR_flag = flag;
	this->Disconnect_FIND_ERROR_flag_CS.Leave();
}

//	�޲������캯��
CMonitorProtocol::CintegratedTCPServerManage::CintegratedTCPServerManage()
{
	//	����ַ���������
	memset(this->ip_str_GetClientIPStr, 0, sizeof(this->ip_str_GetClientIPStr));
	memset(this->ip_str_GetServerIPString, 0, sizeof(this->ip_str_GetServerIPString));

	this->init_flag = false;					//	����Ϊ��ʼ��
	this->socketServer = NULL;					//	��������ԴSOCKETΪ��

	this->pOnJoinFunc = NULL;					//	������Ӧ����Ϊ��
	this->pOnBreadFunc = NULL;					//	�Ͽ���Ӧ����Ϊ��

	this->Disconnect_ACCEPT_flag = false;
	this->Disconnect_FIND_ERROR_flag = false;
}

//	��������
CMonitorProtocol::CintegratedTCPServerManage::~CintegratedTCPServerManage()
{
	//	����Ƿ��Ѿ���ʼ����
	if (this->init_flag == true)
	{
		//	��ֹ�߳�
		this->init_flag = false;
		this->SetDisconnectACCEPTFlag(true);
		closesocket(this->socketServer);
		while (this->m_accept_thread.GetState() != CThread::STOP) Sleep(1);
		this->SetDisconnectACCEPTFlag(false);
		this->SetDisconnectFINDERRORFlag(true);
		while (this->m_find_error_thread.GetState() != CThread::STOP) Sleep(1);
		this->SetDisconnectFINDERRORFlag(false);

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
bool CMonitorProtocol::CintegratedTCPServerManage::AddConList(
	SOCKET in_socket,										//	�Ѿ��ɹ��������ӵ�SOCKET��Ҳ��Ψһ�ı�ʶ
	SOCKADDR_IN	addrClient,									//	�����ӿͻ��˵ĵ�ַ��Ϣ
	P_USER_ON_RECV_DATAAREA_SERVER_FUNC precvfunc			//	�û�����Ľ��պ���
	)
{
	//	������
	//	���in_socket
	if ((in_socket == NULL) || (in_socket == (SOCKET)(-1)))
	{
		return false;
	}

	//	��麯��ָ��
	if ((precvfunc == NULL) || (precvfunc == (P_USER_ON_RECV_DATAAREA_SERVER_FUNC)(-1)))
	{
		return false;
	}

	//	����������
	CintegratedTCPServerHost* pCTCPServerHost = new CintegratedTCPServerHost;
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
bool CMonitorProtocol::CintegratedTCPServerManage::RemoveConList(
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
		this->mConList.erase(this->mConList.begin() + index);
	}
	mconlist_cs.Leave();		//	���ڶ�mConList�Ĳ�����û���ˣ��������뿪�ٽ磬Ȼ�������Դ���ͷ�

	//	�����������û���ҵ�
	if ((index == -1) || (ptmp_dat == NULL))
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
void CMonitorProtocol::CintegratedTCPServerManage::AutoRemoveError(void)
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
bool CMonitorProtocol::CintegratedTCPServerManage::Config(
	CTCPServerResources& rCTCPServerResources,							//	��������Դ������
	P_USER_ON_RECV_DATAAREA_SERVER_FUNC pfunc							//	�û����պ���
	)
{
	//	������뺯��ָ��
	if ((pfunc == NULL) || (pfunc == (P_USER_ON_RECV_DATAAREA_SERVER_FUNC)(-1)))
	{
		return false;
	}

	//	����Ƿ��Ѿ���ʼ����
	if (this->init_flag == true)
	{
		//	��ֹ�߳�
		this->init_flag = false;
		this->SetDisconnectACCEPTFlag(true);
		closesocket(this->socketServer);
		while (this->m_accept_thread.GetState() != CThread::STOP) Sleep(1);
		this->SetDisconnectACCEPTFlag(false);
		this->SetDisconnectFINDERRORFlag(true);
		while (this->m_find_error_thread.GetState() != CThread::STOP) Sleep(1);
		this->SetDisconnectFINDERRORFlag(false);

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
	this->addrServer = rCTCPServerResources.GetSocketAddr();
	this->socketServer = rCTCPServerResources.GetSocket();
	this->pRecvFunc = pfunc;

	//	���������߳�
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

	//	��ʼ�����óɹ�
	this->init_flag = true;
	//	���óɹ�
	return true;
}

//	�������ӺͶϿ���Ӧ�����ĳ�ʼ�����ú������ɹ�����true��ʧ�ܷ���false
bool CMonitorProtocol::CintegratedTCPServerManage::Config(
	CTCPServerResources& rCTCPServerResources,			//	��������Դ������
	P_USER_ON_RECV_DATAAREA_SERVER_FUNC pfunc,			//	�û����պ���	
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

//	�������Ӹ���������ȫ����Ϣ���б�
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

//	�Ͽ����ӣ�true��ʾ�����ɹ���false��ʾ�б���û�д�����
bool CMonitorProtocol::CintegratedTCPServerManage::Disconnect(SOCKET in_socket)
{
	return this->RemoveConList(in_socket);
}

//	�������ݣ�>0��ʾ�����ɹ���SOCKET_ERROR��ʾ�б���û�д�����,0��ʾ����ʧ��
int CMonitorProtocol::CintegratedTCPServerManage::Send(
	SOCKET in_socket,											//	ָ����SOCKET���
	std::vector<SDataArea> in_vec								//	���͵����ݿ�
	)
{
	//	����������
	if ((in_socket == NULL) || (in_socket == (SOCKET)(-1)))
	{
		//	�����ͷ�����������
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
		//	�����ͷ�����������
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

	//	��������
	int re = ptmp_dat->pCon->Send(in_vec);

	//	����Ƿ��ͳɹ�����ʧ�ܵ�ʱ�򣬾��Ѿ���ʾ��������Ѿ�ʧЧ�ˣ����Խ����б����Ƴ��������
	if (re == SOCKET_ERROR)
	{
		this->RemoveConList(ptmp_dat->socket);
		//	�����ͷ�����������
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

	//	���е�����ͳɹ���
	return re;
}

//	�õ�IP�ַ��������󽫷���һ�����ַ���
char* CMonitorProtocol::CintegratedTCPServerManage::GetClientIPStr(SOCKET in_socket)
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
u_short CMonitorProtocol::CintegratedTCPServerManage::GetClientPort(SOCKET in_socket)
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
CMyTime CMonitorProtocol::CintegratedTCPServerManage::GetClientBeginTime(SOCKET in_socket)
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
char* CMonitorProtocol::CintegratedTCPServerManage::GetServerIPString(void)
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
int CMonitorProtocol::CintegratedTCPServerManage::GetServerPort(void)
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
SOCKET CMonitorProtocol::CintegratedTCPServerManage::GetServerSOCKET(void)
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