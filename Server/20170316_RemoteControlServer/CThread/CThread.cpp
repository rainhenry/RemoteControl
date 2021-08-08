
// REV 0.3

#include "stdafx.h"
#include "CThread.h"

//	���������Ĺ��캯��
CThread::CThread()
{
	this->pThreadFunc = NULL;
	this->pThreadParam = NULL;
	hThread = NULL;
	init_flag = false;
}

//	���캯��
CThread::CThread(
	CThread::P_THREAD_FUNC pFunc,		//	�̵߳ĺ���ָ��
	LPVOID	pParam,						//	���̴߳��ݵĲ���
	EThreadState state					//	�߳��������״̬��Ĭ��Ϊ���У�Ҳ��������Ϊ��ֹ�ģ��������������⣬���������Ϊ��ֹ��
	)
{
	init_flag = false;			//	�������ڹ��캯��������ã�����һ���ǵ�һ�γ�ʼ��
	this->Config(
		pFunc,
		pParam,
		state
		);
}

//	���ú������ɹ�����true,ʧ�ܷ���false
bool CThread::Config(
	P_THREAD_FUNC pFunc,		//	�̵߳ĺ���ָ��
	LPVOID	pParam,				//	���̴߳��ݵĲ���
	EThreadState state			//	�߳��������״̬��Ĭ��Ϊ���У�Ҳ��������Ϊ��ֹ�ģ��������������⣬���������Ϊ��ֹ��
	)
{
	//	����Ƿ��Ѿ���ʼ����
	if (init_flag == true)
	{
		//	��ǰ״̬��Ϊֹͣ��ʱ��
		if (CurrentState != STOP)
		{
			//	����ʧЧ
			return false;
		}
	}
	
	//	����������������ĺ���ָ��Ƿ�
	if ((pFunc == NULL) || (pFunc == ((P_THREAD_FUNC)(-1))))
	{
		CurrentState = STOP;
		return false;
	}

	//	����
	this->pThreadFunc = pFunc;
	pThreadParam = pParam;

	//	�ж��趨������״̬����Ϊ����
	if (state == RUN)
	{
		hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)pThreadFunc, pThreadParam, 0, NULL);
		CurrentState = RUN;
	}
	//	����Ϊ����
	else
	{
		hThread = NULL;
		CurrentState = STOP;
	}

	//	���óɹ�
	init_flag = true;

	return true;
}

//	�����̻߳��߻ָ��߳�
void CThread::Run(void)
{
	//	��δ��ʼ����ʱ
	if (init_flag == false)
	{
		CurrentState = STOP;
		return;
	}

	UpdateState();				//	�����߳�״̬

	//	�ж��߳�״̬�����Ϊ����
	if (CurrentState == RUN)
	{
		//	ʲô������
	} 
	//	���Ϊֹͣ
	else if (CurrentState == STOP)
	{
		//	�����߳�ʱ���ָ���Ƿ�Ϸ�����Ϊ�Ƿ�ָ��
		if ((pThreadFunc == NULL) || (pThreadFunc == ((P_THREAD_FUNC)(-1))))
		{
			CurrentState = STOP;
			return;
		}
		
		//	�����߳�
		hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)pThreadFunc,pThreadParam,0,NULL);
		CurrentState = RUN;
	}
	//	����Ϊ����
	else
	{
		//	�ָ��߳�
		ResumeThread(hThread);
		CurrentState = RUN;

	}
}

//	�����߳�
void CThread::Susp(void)
{
	//	��δ��ʼ����ʱ
	if (init_flag == false)
	{
		CurrentState = STOP;
		return;
	}

	UpdateState();				//	�����߳�״̬

	//	�ж��߳�״̬�����Ϊ����
	if (CurrentState == RUN)
	{
		//	����
		SuspendThread(hThread);
		CurrentState = SUS;
	} 
	//	���Ϊֹͣ
	else if (CurrentState == STOP)
	{
		//	ʲô������
	}
	//	����Ϊ����
	else
	{
		//	ʲô������
	}
}


//	��ֹ�߳�
void CThread::Stop(void)
{
	//	��δ��ʼ����ʱ
	if (init_flag == false)
	{
		CurrentState = STOP;
		return;
	}

	UpdateState();				//	�����߳�״̬

	//	�ж��߳�״̬�����Ϊ����
	if (CurrentState == RUN)
	{
		//	��ֹ�߳�
		DWORD code;
		GetExitCodeThread (hThread,&code);
		TerminateThread(hThread,code);
		CloseHandle(hThread);
		hThread = NULL;
		CurrentState = STOP;
	} 
	//	���Ϊֹͣ
	else if (CurrentState == STOP)
	{
		//	ʲô������
	}
	//	����Ϊ����
	else
	{
		//	��ֹ�߳�
		DWORD code;
		GetExitCodeThread (hThread,&code);
		TerminateThread(hThread,code);
		CloseHandle(hThread);
		hThread = NULL;
		CurrentState = STOP;
	}
}

//	��ȡ�߳�״̬
CThread::EThreadState CThread::GetState(void)
{
	//	��δ��ʼ����ʱ
	if (init_flag == false)
	{
		CurrentState = STOP;
		return CurrentState;
	}

	UpdateState();				//	�����߳�״̬
	return CurrentState;
}

//	��������
CThread::~CThread()
{
	//	��ֹ�߳�
	Stop();
}

//	���µ�ǰ���߳�״̬���ú�����Ҫ�жϵ�ǰ�߳��Ƿ�Ϊ�Զ���Ȼ��ֹ
//	��hThread=NULLʱ��WaitForMultipleObjects����-1
//	���߳��������е�ʱ��WaitForMultipleObjects����258 = WAIT_TIMEOUT
//	���߳��Զ�������ʱ��WaitForMultipleObjects����0
//	���̹߳����ʱ��WaitForMultipleObjects����258
//		�����߳��ڲ�����CloseHandle(hThread);��ʱ��WaitForMultipleObjects����258 = WAIT_TIMEOUT�������̲߳�����ֹ���̻߳�ֱ�����е�return���Ż���ֹ��
//		Ȼ�����߳��ڲ��������������������Ҳ������ִ�С�
//	�����߳��ⲿ�ȵ���TerminateThread��Ȼ���ٵ���CloseHandle��ʱ���̻߳ᱻǿ����ֹ���޷�������������
//	�����߳��ڲ��ȵ���TerminateThread��Ȼ���ٵ���CloseHandle��ʱ���̻߳ᱻǿ����ֹ���޷�������������
//	�������߳��ڲ�����CloseHandle��Ȼ�������߳��ⲿ����TerminateThread��ʱ���̲߳�����ֹ����һֱ���е�return��䡣���Ǵ�ʱWaitForMultipleObjects����258 = WAIT_TIMEOUT��
void CThread::UpdateState(void)
{
	//	�жϵ�ǰ�߳�״̬������߳��Ѿ�ֹͣ
	if (hThread == NULL)
	{
		//	ʲô������
	}
	else
	{
		DWORD re = ::WaitForMultipleObjects(1, &hThread, TRUE, UPDATE_TIME_MS);
		//	�жϷ��ش��룬��Ϊ��ʱ�����߳���Ȼ���ڣ��������к͹����������
		if (re == WAIT_TIMEOUT)
		{
			//	ʲô������
		}
		//	�����߳�Ϊ��Ȼ��ֹ�����ڲ�����
		else
		{
			//	��ֹ�߳�״̬����
			DWORD code;
			GetExitCodeThread (hThread,&code);
			TerminateThread(hThread,code);
			CloseHandle(hThread);
			hThread = NULL;
			CurrentState = STOP;
		}
	}
}

//	�û��Ѿ��Լ���ֹ���߳�
void CThread::SafeStop(void)
{
	//	��δ��ʼ����ʱ
	if (init_flag == false)
	{
		CurrentState = STOP;
		return;
	}
	
	//	��ֹ�̣߳���ʱ�̲߳�����������ֹ������return�������������̵߳���Դ
	CloseHandle(hThread);
	hThread = NULL;
	CurrentState = STOP;
}

//-------------------------------------------------------------------------------------
//	�߳����а�ȫ��

//	���캯��
CThread::CThreadRunSafe::CThreadRunSafe(CThread* in_p)
{
	pThread = in_p;
}

//	���캯��
CThread::CThreadRunSafe::CThreadRunSafe(CThread& in_c)
{
	pThread = &(in_c);
}

//	��������
CThread::CThreadRunSafe::~CThreadRunSafe()
{
	pThread->SafeStop();
}

