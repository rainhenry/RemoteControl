//////////////////////////////////////////////////////////////////////////
/************************************************************************

	�������ƣ��ļ�������
	�������ڣ�2017.03.31
	���̰汾��0.5
	��    �ƣ�rainhenry
	��    �룺rainhenry
	������ˣ�rainhenry
	ƽ    ̨��VS2008+Win7 Sp1 64bit @ WinCE6.0

	�汾�޶���
		0.2�汾		��CheckFileExist��Ϊ��̬���������ṩ�ⲿ����
		0.3�汾		�����˶�����·����֧�֣�����ͨ��USE_CHS_CODE�궨������
		0.4�汾		ʹ��sprintf_s����
					ʹ��fopen_s����
					����ͷ�ļ�locale
		0.5�汾		�����û���ʼ�����ݺ���

	����������
	������������⸴�ƴ���ʹ�ã�����ѧϰ�о��ο�������������ҵ��;
	ʹ�ñ�����ʱ���뱣����ע�͡�

	����������
	ʵ��RAM���ļ����ļ���RAM���ݵ�ת��

	ʹ�÷�����
	1������ʵ����ָ�������׵�ַ�ͳ��ȣ�ָ��·�����ļ�����ָ��Ĭ�����ݵĵ�ַ
		��ʱ���Զ������ڴ���ļ���Ĺ�ϵ���Լ���ʱ����Ĭ������
	2����ѡ������
		ͬ�����ļ�
		ͬ����RAM
		����Ĭ�ϵ�RAM
		ɾ���ļ�


	ʹ�þ�����		
	//	�����ļ�����ʵ��
	CFileSave myfilesave(
		&mSDownLoadData,					//	�����ǵ�ǰ���ݽṹ��
		sizeof(SDownLoadData),				//	�ṹ���С���ֽڵ�λ��
		L"\\ResidentFlash\\MilkConfig.dat",	//	�ļ�·��+�ļ���
		&DefaultSDownLoadData);				//	Ĭ��ֵ

	//--------------------------------------------
	//	��ʹ���û���ʼ��������ʱ��
	//	�����ʼ������
	void DedaultUserDataInit(void)
	{
		DefaultSDownLoadData.a = 0;
		DefaultSDownLoadData.b = 1;
		//	.....	
	}

	//	�����ļ�����ʵ��
	CFileSave myfilesave(
		&mSDownLoadData,					//	�����ǵ�ǰ���ݽṹ��
		sizeof(SDownLoadData),				//	�ṹ���С���ֽڵ�λ��
		L"\\ResidentFlash\\MilkConfig.dat",	//	�ļ�·��+�ļ���
		&DefaultSDownLoadData,				//	Ĭ��ֵ
		DedaultUserDataInit					//	�û���ʼ������
		);

************************************************************************/

//////////////////////////////////////////////////////////////////////////
//	Ԥ����ͷ����
#ifndef __CFILESAVE_H__
#define __CFILESAVE_H__

//////////////////////////////////////////////////////////////////////////
//	����ͷ�ļ�
#include <stdio.h>
#include <locale>

//////////////////////////////////////////////////////////////////////////
//	�궨��
#define		FILE_NAME_MAX_LENGTH		500					//	�ļ�����󳤶��ֽ���
#define		INVALID_FILE_POINT			((void*)(-1))		//	��Ч���ļ�ָ��
#define		USE_CHS_CODE									//	����·��֧��


//////////////////////////////////////////////////////////////////////////
//	�ඨ��
class CFileSave
{
public:
	//	�����û���ʼ�����ݺ���
	typedef void(*P_USER_INIT_DATA_FUNC)(void);

private:
	void*	pCD;								//	��ǰ�����׵�ַ
	void*	pDD;								//	Ĭ�������׵�ַ
	int		DataLength;							//	���ݳ��ȣ��ֽڵ�λ��
	CString fName;								//	�ļ�·������

public:
	//	���캯��
	CFileSave(
		void*	pCurrentData,					//	��ǰ�����׵�ַ
		int		size,							//	���ݳ��ȣ��ֽڵ�λ��
		CString	file_name,						//	�ļ�·������
		void*	pDefaultData,					//	Ĭ�������׵�ַ
		P_USER_INIT_DATA_FUNC pFunc = NULL		//	�û���ʼ�������õĺ���
		);

	//	��������
	~CFileSave();		
	
	//	����ļ��Ƿ����	���� ture:����		false:������
	static bool CheckFileExist(CString str);

	//	���ļ��������뵽RAM
	void FileLoadToBuffer(void);

	//	����Ĭ������
	void LoadDefaultData(void);

	//	��RAM����д�뵽�ļ�
	void WirteToFile(void);
};

//////////////////////////////////////////////////////////////////////////
#endif	//	__CFILESAVE_H__