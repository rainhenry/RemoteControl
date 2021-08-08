//////////////////////////////////////////////////////////////////////////
/************************************************************************

	�������ƣ��ٽ���
	�������ڣ�2016.12.06
	���̰汾��0.2
	��    �ƣ�rainhenry
	��    �룺rainhenry
	������ˣ�rainhenry
	ƽ    ̨��VS2010+Win7 Sp1 64bit

	����������
	������������⸴�ƴ���ʹ�ã�����ѧϰ�о��ο�������������ҵ��;
	ʹ�ñ�����ʱ���뱣����ע�͡�

	����������
	ʵ�ֶ��߳�ͨ��ʱ�������ݣ�ͨ�������ٽ���뿪�ٽ�
	�����ԣ�ͬһ���߳��ڣ���ͬһ���ٽ������н�������Ļ�������������ֻ����������ͬ���̶߳�ͬһ���ٽ��������Ż�������
	���ٽ��ʼ����ʱ���Ѿ��������ٽ���󱣳ֲ���

	��Ʋο���
	http://blog.csdn.net/jerry4711/article/details/3906594

	ʹ�÷�����
	1������ʵ��
	2�������ٽ�
	3����������
	4���뿪�ٽ�

	ʹ�þ�����
	CMyCriticalSection myccs;
	myccs.Enter();
	//....��������
	myccs.Leave();

	�޶���
	0.2�汾����������ΪCMyCriticalSection����ֹ��afxmt.h�ļ��е������ظ���ͻ

************************************************************************/

//////////////////////////////////////////////////////////////////////////
//	Ԥ����ͷ����
#ifndef __CMyCriticalSection_hpp__
#define __CMyCriticalSection_hpp__

//////////////////////////////////////////////////////////////////////////
//	����ͷ�ļ�
#include <iostream>
#include <Windows.h>

//////////////////////////////////////////////////////////////////////////
//	�ඨ��
class CMyCriticalSection
{
private:
	//	�ٽ����
	CRITICAL_SECTION CS;

	//	�����ٽ��״̬
	bool InCS;				//	true: IN_CS			false:	NOT_CS

public:
	//	���캯��
	CMyCriticalSection()
	{
		//	��ʼ���ٽ����
		InitializeCriticalSection(&CS);

		//	���ñ�־
		InCS = false;		//	û�����ٽ���
	}

	//	��������
	~CMyCriticalSection()
	{
		//	�ͷ��ٽ����
		DeleteCriticalSection(&CS);
	}

	//	�����ٽ�
	void Enter(void)
	{
		EnterCriticalSection(&CS);
		InCS = true;
	}

	//	�뿪�ٽ�
	void Leave(void)
	{
		LeaveCriticalSection(&CS);
		InCS = false;
	}

	//	�ж��Ƿ����ٽ�
	bool InCriticalSection(void)
	{
		return InCS;
	}
};


//////////////////////////////////////////////////////////////////////////
#endif	//	__CMyCriticalSection_hpp__
