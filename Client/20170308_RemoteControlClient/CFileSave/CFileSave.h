//////////////////////////////////////////////////////////////////////////
/************************************************************************

	工程名称：文件保存类
	工程日期：2017.03.31
	工程版本：0.5
	设    计：rainhenry
	编    码：rainhenry
	调试审核：rainhenry
	平    台：VS2008+Win7 Sp1 64bit @ WinCE6.0

	版本修订：
		0.2版本		将CheckFileExist改为静态函数，可提供外部调用
		0.3版本		增加了对中文路径的支持，可以通过USE_CHS_CODE宏定义设置
		0.4版本		使用sprintf_s函数
					使用fopen_s函数
					包含头文件locale
		0.5版本		增加用户初始化数据函数

	免责声明：
	本代码可以随意复制传播使用，仅供学习研究参考，切勿用于商业用途
	使用本代码时，请保留本注释。

	功能描述：
	实现RAM到文件，文件到RAM数据的转换

	使用方法：
	1、定义实例，指定对象首地址和长度，指定路径和文件名，指定默认数据的地址
		此时会自动处理内存和文件间的关系，以及何时载入默认数据
	2、可选操作：
		同步到文件
		同步到RAM
		载入默认到RAM
		删除文件


	使用举例：		
	//	定义文件保存实例
	CFileSave myfilesave(
		&mSDownLoadData,					//	这里是当前数据结构体
		sizeof(SDownLoadData),				//	结构体大小（字节单位）
		L"\\ResidentFlash\\MilkConfig.dat",	//	文件路径+文件名
		&DefaultSDownLoadData);				//	默认值

	//--------------------------------------------
	//	当使用用户初始化函数的时候
	//	定义初始化函数
	void DedaultUserDataInit(void)
	{
		DefaultSDownLoadData.a = 0;
		DefaultSDownLoadData.b = 1;
		//	.....	
	}

	//	定义文件保存实例
	CFileSave myfilesave(
		&mSDownLoadData,					//	这里是当前数据结构体
		sizeof(SDownLoadData),				//	结构体大小（字节单位）
		L"\\ResidentFlash\\MilkConfig.dat",	//	文件路径+文件名
		&DefaultSDownLoadData,				//	默认值
		DedaultUserDataInit					//	用户初始化函数
		);

************************************************************************/

//////////////////////////////////////////////////////////////////////////
//	预编译头保护
#ifndef __CFILESAVE_H__
#define __CFILESAVE_H__

//////////////////////////////////////////////////////////////////////////
//	包含头文件
#include <stdio.h>
#include <locale>

//////////////////////////////////////////////////////////////////////////
//	宏定义
#define		FILE_NAME_MAX_LENGTH		500					//	文件名最大长度字节数
#define		INVALID_FILE_POINT			((void*)(-1))		//	无效的文件指针
#define		USE_CHS_CODE									//	中文路径支持


//////////////////////////////////////////////////////////////////////////
//	类定义
class CFileSave
{
public:
	//	定义用户初始化数据函数
	typedef void(*P_USER_INIT_DATA_FUNC)(void);

private:
	void*	pCD;								//	当前数据首地址
	void*	pDD;								//	默认数据首地址
	int		DataLength;							//	数据长度（字节单位）
	CString fName;								//	文件路径名称

public:
	//	构造函数
	CFileSave(
		void*	pCurrentData,					//	当前数据首地址
		int		size,							//	数据长度（字节单位）
		CString	file_name,						//	文件路径名称
		void*	pDefaultData,					//	默认数据首地址
		P_USER_INIT_DATA_FUNC pFunc = NULL		//	用户初始化数据用的函数
		);

	//	析构函数
	~CFileSave();		
	
	//	检查文件是否存在	返回 ture:存在		false:不存在
	static bool CheckFileExist(CString str);

	//	将文件数据载入到RAM
	void FileLoadToBuffer(void);

	//	载入默认数据
	void LoadDefaultData(void);

	//	将RAM数据写入到文件
	void WirteToFile(void);
};

//////////////////////////////////////////////////////////////////////////
#endif	//	__CFILESAVE_H__