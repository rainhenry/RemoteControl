
//	版本 REV 0.5

//---------------------------------------------------
//	包含头文件
#include "stdafx.h"
#include "CFileSave.h"

//---------------------------------------------------
//	构造函数
CFileSave::CFileSave(
		void*	pCurrentData,				//	当前数据首地址
		int		size,						//	数据长度（字节单位）
		CString	file_name,					//	文件路径名称
		void*	pDefaultData,				//	默认数据首地址
		P_USER_INIT_DATA_FUNC pFunc			//	用户初始化数据用的函数
		)
{
#ifdef USE_CHS_CODE	
	//	中文路径支持
	setlocale(LC_ALL,"chs");
#endif

	//	获取文件名和数据首地址
	this->pCD = pCurrentData;
	this->DataLength = size;
	this->pDD = pDefaultData;
	this->fName = file_name;

	//	调用用户初始化数据函数，主要目的是为了初始化默认的载入数据
	if ((pFunc != NULL) && (pFunc != (P_USER_INIT_DATA_FUNC)(-1)))
	{
		pFunc();
	}

	//	检查文件是否存在，已经存在的话
	if(CheckFileExist(this->fName))
	{
		//	将文件内的数据载入到缓冲区中
		if((this->pCD != NULL)&&(this->pCD != INVALID_FILE_POINT))
			FileLoadToBuffer();					//	载入文件到RAM
	}
	//	不存在该文件
	else
	{
		//	载入默认数据
		LoadDefaultData();

		//	写入到文件
		WirteToFile();
	}
}

//	析构函数
CFileSave::~CFileSave()
{
}

//---------------------------------------------------
//	检查文件是否存在	返回 ture:存在		false:不存在
bool CFileSave::CheckFileExist(CString str)
{
#ifdef USE_CHS_CODE	
	//	中文路径支持
	setlocale(LC_ALL,"chs");
#endif

	char* pstr = new char[FILE_NAME_MAX_LENGTH];
	sprintf_s(pstr, FILE_NAME_MAX_LENGTH, "%S", str);
	FILE* fp = NULL;
	fopen_s(&fp, pstr, "r");
	delete[] pstr;

	//	打开失败
	if(fp == NULL)
	{
		return false;
	}
	//	打开成功
	else
	{
		fclose(fp);
		return true;
	}
}

//-------------------------------------------------------
//	将文件数据载入到RAM
void CFileSave::FileLoadToBuffer(void)
{
#ifdef USE_CHS_CODE	
	//	中文路径支持
	setlocale(LC_ALL,"chs");
#endif

	char* pstr = new char[FILE_NAME_MAX_LENGTH];
	sprintf_s(pstr, FILE_NAME_MAX_LENGTH, "%S", this->fName);
	FILE* fp = NULL;
	fopen_s(&fp, pstr, "r");
	delete[] pstr;

	//	打开失败
	if(fp == NULL)
	{
		return ;
	}
	//	打开成功
	else
	{
		//	载入文件到缓存
		fread(this->pCD, 1,	this->DataLength, fp);

		//	关闭文件
		fclose(fp);
		return ;
	}
}

//---------------------------------------------------------
//	载入默认数据
void CFileSave::LoadDefaultData(void)
{
	//	当默认数据首地址正确的时候
	if((this->pDD != NULL)&&(this->pDD != INVALID_FILE_POINT))
	{
		//	载入默认数据到当前内存
		memcpy(this->pCD, this->pDD, this->DataLength);
	}
}

//----------------------------------------------------------
//	将RAM数据写入到文件
void CFileSave::WirteToFile(void)
{
#ifdef USE_CHS_CODE	
	//	中文路径支持
	setlocale(LC_ALL,"chs");
#endif

	//	以清空写入的方式开始写入文件
	char* pstr = new char[FILE_NAME_MAX_LENGTH];
	sprintf_s(pstr, FILE_NAME_MAX_LENGTH, "%S", this->fName);
	FILE* fp = NULL;
	fopen_s(&fp, pstr, "w");
	delete[] pstr;
	
	//	写入文件
	fwrite(this->pCD, 1, this->DataLength, fp);

	//	关闭文件
	fclose(fp);
}


