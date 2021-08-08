
//	�汾 REV 0.5

//---------------------------------------------------
//	����ͷ�ļ�
#include "stdafx.h"
#include "CFileSave.h"

//---------------------------------------------------
//	���캯��
CFileSave::CFileSave(
		void*	pCurrentData,				//	��ǰ�����׵�ַ
		int		size,						//	���ݳ��ȣ��ֽڵ�λ��
		CString	file_name,					//	�ļ�·������
		void*	pDefaultData,				//	Ĭ�������׵�ַ
		P_USER_INIT_DATA_FUNC pFunc			//	�û���ʼ�������õĺ���
		)
{
#ifdef USE_CHS_CODE	
	//	����·��֧��
	setlocale(LC_ALL,"chs");
#endif

	//	��ȡ�ļ����������׵�ַ
	this->pCD = pCurrentData;
	this->DataLength = size;
	this->pDD = pDefaultData;
	this->fName = file_name;

	//	�����û���ʼ�����ݺ�������ҪĿ����Ϊ�˳�ʼ��Ĭ�ϵ���������
	if ((pFunc != NULL) && (pFunc != (P_USER_INIT_DATA_FUNC)(-1)))
	{
		pFunc();
	}

	//	����ļ��Ƿ���ڣ��Ѿ����ڵĻ�
	if(CheckFileExist(this->fName))
	{
		//	���ļ��ڵ��������뵽��������
		if((this->pCD != NULL)&&(this->pCD != INVALID_FILE_POINT))
			FileLoadToBuffer();					//	�����ļ���RAM
	}
	//	�����ڸ��ļ�
	else
	{
		//	����Ĭ������
		LoadDefaultData();

		//	д�뵽�ļ�
		WirteToFile();
	}
}

//	��������
CFileSave::~CFileSave()
{
}

//---------------------------------------------------
//	����ļ��Ƿ����	���� ture:����		false:������
bool CFileSave::CheckFileExist(CString str)
{
#ifdef USE_CHS_CODE	
	//	����·��֧��
	setlocale(LC_ALL,"chs");
#endif

	char* pstr = new char[FILE_NAME_MAX_LENGTH];
	sprintf_s(pstr, FILE_NAME_MAX_LENGTH, "%S", str);
	FILE* fp = NULL;
	fopen_s(&fp, pstr, "r");
	delete[] pstr;

	//	��ʧ��
	if(fp == NULL)
	{
		return false;
	}
	//	�򿪳ɹ�
	else
	{
		fclose(fp);
		return true;
	}
}

//-------------------------------------------------------
//	���ļ��������뵽RAM
void CFileSave::FileLoadToBuffer(void)
{
#ifdef USE_CHS_CODE	
	//	����·��֧��
	setlocale(LC_ALL,"chs");
#endif

	char* pstr = new char[FILE_NAME_MAX_LENGTH];
	sprintf_s(pstr, FILE_NAME_MAX_LENGTH, "%S", this->fName);
	FILE* fp = NULL;
	fopen_s(&fp, pstr, "r");
	delete[] pstr;

	//	��ʧ��
	if(fp == NULL)
	{
		return ;
	}
	//	�򿪳ɹ�
	else
	{
		//	�����ļ�������
		fread(this->pCD, 1,	this->DataLength, fp);

		//	�ر��ļ�
		fclose(fp);
		return ;
	}
}

//---------------------------------------------------------
//	����Ĭ������
void CFileSave::LoadDefaultData(void)
{
	//	��Ĭ�������׵�ַ��ȷ��ʱ��
	if((this->pDD != NULL)&&(this->pDD != INVALID_FILE_POINT))
	{
		//	����Ĭ�����ݵ���ǰ�ڴ�
		memcpy(this->pCD, this->pDD, this->DataLength);
	}
}

//----------------------------------------------------------
//	��RAM����д�뵽�ļ�
void CFileSave::WirteToFile(void)
{
#ifdef USE_CHS_CODE	
	//	����·��֧��
	setlocale(LC_ALL,"chs");
#endif

	//	�����д��ķ�ʽ��ʼд���ļ�
	char* pstr = new char[FILE_NAME_MAX_LENGTH];
	sprintf_s(pstr, FILE_NAME_MAX_LENGTH, "%S", this->fName);
	FILE* fp = NULL;
	fopen_s(&fp, pstr, "w");
	delete[] pstr;
	
	//	д���ļ�
	fwrite(this->pCD, 1, this->DataLength, fp);

	//	�ر��ļ�
	fclose(fp);
}


