/************************************************************************

	�������ƣ��߳���
	��    ����0.3
	�������ڣ�2016.12.10
	��Ʊ�д��rainhenry

	�汾�޶�
		0.2�汾		�����˲��������Ĺ��캯��
					���������ú���
		0.3�汾		�������û���ȫ��ֹ���������û��Զ������̵߳�ʱ����ø÷�������ͬ���߳�״̬
					���û��������������ʱ�������������߳��ڲ������Դ�ʱ�����Թر��߳̾��
					�����̰߳�ȫ�࣬������ȥ���߳��ڲ���ÿ��return���ǰ�����SafeStop����
					���߳̿�ʼ�ĵط�����ö��󼴿�ʵ��

	���ܸ�����
	ʵ�ֶ��̵߳ķ�װ

	ʹ��ע�����
		���߳��ڲ���return����ǰ�������һ��mCThread.SafeStop();�����ĵ���
		��mCThread.SafeStop();�����ĵ���֮�󣬱���Ҫ����return!!!

	�ڲ���ؽ���
	//	��hThread=NULLʱ��WaitForMultipleObjects����-1
	//	���߳��������е�ʱ��WaitForMultipleObjects����258 = WAIT_TIMEOUT
	//	���߳��Զ�������ʱ��WaitForMultipleObjects����0
	//	���̹߳����ʱ��WaitForMultipleObjects����258
	//	�����߳��ڲ�����CloseHandle(hThread);��ʱ��WaitForMultipleObjects����258 = WAIT_TIMEOUT�������̲߳�����ֹ���̻߳�ֱ�����е�return���Ż���ֹ��
		Ȼ�����߳��ڲ��������������������Ҳ������ִ�С�
	//	�����߳��ⲿ�ȵ���TerminateThread��Ȼ���ٵ���CloseHandle��ʱ���̻߳ᱻǿ����ֹ���޷�������������
	//	�����߳��ڲ��ȵ���TerminateThread��Ȼ���ٵ���CloseHandle��ʱ���̻߳ᱻǿ����ֹ���޷�������������
	//	�������߳��ڲ�����CloseHandle��Ȼ�������߳��ⲿ����TerminateThread��ʱ���̲߳�����ֹ����һֱ���е�return��䡣���Ǵ�ʱWaitForMultipleObjects����258 = WAIT_TIMEOUT��

	ʹ�÷���
	1�������̲߳����������ࣨ��ҪΪ�˱������ʣ�
	2�������̺߳���
	3������ʵ��
    4��������ֹͣ�Ȳ���

	ʹ�þ���	
	//	����
	extern	CThread mCThread;

	//	�̺߳�����
	DWORD MyFunc1(LPVOID lpParam)
	{
		//......�û�����

		//	�߳��˳���ʱ��
		mCThread.SafeStop();
		return 0;
	}

	//	�����߳�ʵ��
	CThread mCThread(
		MyFunc1,						//	�����̺߳���
		(LPVOID)(&mCUserInterFace)		//	���������������
		);
	//	����	
	//	�����߳�ʵ��
	CThread mCThread(
		MyFunc1,						//	�����̺߳���
		(LPVOID)(&mCUserInterFace),		//	���������������
		CThread::STOP					//	�տ�ʼ��ʱ���߳�Ϊ�ر�״̬
		);

	//	����
	mCThread.Run();		//	����
	mCThread.Susp();	//	����
	mCThread.Stop();	//	ֹͣ��������ǿ��ֹͣ������������������ᵼ���ڴ�й¶!!

	//	����߳�״̬
	CThread::EThreadState mstate = mCThread.GetState();

	//	�������̰߳�ȫ���ʱ��
	//	�̺߳�����
	DWORD MyFunc1(LPVOID lpParam)
	{
		//	���찲ȫ��
		CThread::CThreadRunSafe msafethread(mCThread);

		//......�û�����

		//	�߳��˳���ʱ��
		return 0;
	}

************************************************************************/

//////////////////////////////////////////////////////////////////////////
//	Ԥ����ͷ����
#ifndef __CTREAD_H__
#define __CTREAD_H__

//////////////////////////////////////////////////////////////////////////
//	����ͷ�ļ�
#include <iostream>
#include <afx.h>
#include <afxwin.h>

//////////////////////////////////////////////////////////////////////////
//	���������궨��
#define		UPDATE_TIME_MS			20					//	�ж��߳��Ƿ������˳���ʱ��Ϊ20ms

//////////////////////////////////////////////////////////////////////////
//	�ඨ��
class CThread
{
public:
	//	�̺߳���ָ�����Ͷ���
	typedef	DWORD (*P_THREAD_FUNC)(LPVOID lpParam);

	//	�߳�״̬����
	typedef enum tagEThreadState
	{
		RUN,				//	�߳�������
		STOP,				//	�߳�����ֹ����δ����
		SUS					//	�̱߳�����
	}EThreadState;

private:
	//	�̺߳����ĵ�ַ
	P_THREAD_FUNC pThreadFunc;

	//	�̺߳�������ڲ���
	LPVOID pThreadParam;

	//	�߳̾��
	HANDLE hThread;

	//	��ǰ���߳�״̬
	EThreadState CurrentState;

	//	���µ�ǰ���߳�״̬���ú�����Ҫ�жϵ�ǰ�߳��Ƿ�Ϊ�Զ���Ȼ��ֹ
	void UpdateState(void);

	//	��ʼ����־
	bool init_flag;

public:
	//	���������Ĺ��캯��
	CThread();

	//	���캯��
	CThread(
		P_THREAD_FUNC pFunc,		//	�̵߳ĺ���ָ��
		LPVOID	pParam,				//	���̴߳��ݵĲ���
		EThreadState state = RUN	//	�߳��������״̬��Ĭ��Ϊ���У�Ҳ��������Ϊ��ֹ�ģ��������������⣬���������Ϊ��ֹ��
		);

	//	���ú������ɹ�����true,ʧ�ܷ���false
	bool Config(
		P_THREAD_FUNC pFunc,		//	�̵߳ĺ���ָ��
		LPVOID	pParam,				//	���̴߳��ݵĲ���
		EThreadState state = RUN	//	�߳��������״̬��Ĭ��Ϊ���У�Ҳ��������Ϊ��ֹ�ģ��������������⣬���������Ϊ��ֹ��
		);

	//	��������
	~CThread();

	//	�����̻߳��߻ָ��߳�
	void Run(void);

	//	�����߳�
	void Susp(void);

	//	��ֹ�߳�
	void Stop(void);

	//	��ȡ�߳�״̬
	EThreadState GetState(void);

	//	�û��Ѿ��Լ���ֹ���߳�
	void SafeStop(void);

public:
	//	�߳����а�ȫ��
	class CThreadRunSafe
	{
	private:
		CThread* pThread;

	public:
		//	���캯��
		CThreadRunSafe(CThread* in_p);

		//	���캯��
		CThreadRunSafe(CThread& in_c);

		//	��������
		~CThreadRunSafe();
	};
};

//////////////////////////////////////////////////////////////////////////
#endif	//	__CTREAD_H__

