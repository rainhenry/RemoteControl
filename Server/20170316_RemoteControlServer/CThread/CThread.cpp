
// REV 0.3

#include "stdafx.h"
#include "CThread.h"

//	不带参数的构造函数
CThread::CThread()
{
	this->pThreadFunc = NULL;
	this->pThreadParam = NULL;
	hThread = NULL;
	init_flag = false;
}

//	构造函数
CThread::CThread(
	CThread::P_THREAD_FUNC pFunc,		//	线程的函数指针
	LPVOID	pParam,						//	给线程传递的参数
	EThreadState state					//	线程启动后的状态，默认为运行，也可以设置为终止的，除了启动参数外，其余参数均为终止。
	)
{
	init_flag = false;			//	由于是在构造函数里面调用，所以一定是第一次初始化
	this->Config(
		pFunc,
		pParam,
		state
		);
}

//	配置函数，成功返回true,失败返回false
bool CThread::Config(
	P_THREAD_FUNC pFunc,		//	线程的函数指针
	LPVOID	pParam,				//	给线程传递的参数
	EThreadState state			//	线程启动后的状态，默认为运行，也可以设置为终止的，除了启动参数外，其余参数均为终止。
	)
{
	//	检查是否已经初始化过
	if (init_flag == true)
	{
		//	当前状态不为停止的时候
		if (CurrentState != STOP)
		{
			//	配置失效
			return false;
		}
	}
	
	//	检查输入参数，传入的函数指针非法
	if ((pFunc == NULL) || (pFunc == ((P_THREAD_FUNC)(-1))))
	{
		CurrentState = STOP;
		return false;
	}

	//	配置
	this->pThreadFunc = pFunc;
	pThreadParam = pParam;

	//	判读设定的启动状态，若为启动
	if (state == RUN)
	{
		hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)pThreadFunc, pThreadParam, 0, NULL);
		CurrentState = RUN;
	}
	//	若不为启动
	else
	{
		hThread = NULL;
		CurrentState = STOP;
	}

	//	配置成功
	init_flag = true;

	return true;
}

//	启动线程或者恢复线程
void CThread::Run(void)
{
	//	当未初始化过时
	if (init_flag == false)
	{
		CurrentState = STOP;
		return;
	}

	UpdateState();				//	更新线程状态

	//	判断线程状态，如果为运行
	if (CurrentState == RUN)
	{
		//	什么都不做
	} 
	//	如果为停止
	else if (CurrentState == STOP)
	{
		//	启动线程时检查指针是否合法，当为非法指针
		if ((pThreadFunc == NULL) || (pThreadFunc == ((P_THREAD_FUNC)(-1))))
		{
			CurrentState = STOP;
			return;
		}
		
		//	启动线程
		hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)pThreadFunc,pThreadParam,0,NULL);
		CurrentState = RUN;
	}
	//	否则为挂起
	else
	{
		//	恢复线程
		ResumeThread(hThread);
		CurrentState = RUN;

	}
}

//	挂起线程
void CThread::Susp(void)
{
	//	当未初始化过时
	if (init_flag == false)
	{
		CurrentState = STOP;
		return;
	}

	UpdateState();				//	更新线程状态

	//	判断线程状态，如果为运行
	if (CurrentState == RUN)
	{
		//	挂起
		SuspendThread(hThread);
		CurrentState = SUS;
	} 
	//	如果为停止
	else if (CurrentState == STOP)
	{
		//	什么都不做
	}
	//	否则为挂起
	else
	{
		//	什么都不做
	}
}


//	终止线程
void CThread::Stop(void)
{
	//	当未初始化过时
	if (init_flag == false)
	{
		CurrentState = STOP;
		return;
	}

	UpdateState();				//	更新线程状态

	//	判断线程状态，如果为运行
	if (CurrentState == RUN)
	{
		//	终止线程
		DWORD code;
		GetExitCodeThread (hThread,&code);
		TerminateThread(hThread,code);
		CloseHandle(hThread);
		hThread = NULL;
		CurrentState = STOP;
	} 
	//	如果为停止
	else if (CurrentState == STOP)
	{
		//	什么都不做
	}
	//	否则为挂起
	else
	{
		//	终止线程
		DWORD code;
		GetExitCodeThread (hThread,&code);
		TerminateThread(hThread,code);
		CloseHandle(hThread);
		hThread = NULL;
		CurrentState = STOP;
	}
}

//	获取线程状态
CThread::EThreadState CThread::GetState(void)
{
	//	当未初始化过时
	if (init_flag == false)
	{
		CurrentState = STOP;
		return CurrentState;
	}

	UpdateState();				//	更新线程状态
	return CurrentState;
}

//	析构函数
CThread::~CThread()
{
	//	终止线程
	Stop();
}

//	更新当前的线程状态，该函数主要判断当前线程是否为自动自然终止
//	当hThread=NULL时，WaitForMultipleObjects返回-1
//	当线程正常运行的时候，WaitForMultipleObjects返回258 = WAIT_TIMEOUT
//	当线程自动结束的时候，WaitForMultipleObjects返回0
//	当线程挂起的时候，WaitForMultipleObjects返回258
//		当在线程内部调用CloseHandle(hThread);的时候，WaitForMultipleObjects返回258 = WAIT_TIMEOUT，但是线程不会终止，线程会直到运行到return语句才会终止，
//		然后在线程内部构造的类对象的析构函数也会正常执行。
//	当在线程外部先调用TerminateThread，然后再调用CloseHandle的时候，线程会被强制终止，无法正常析构对象
//	当在线程内部先调用TerminateThread，然后再调用CloseHandle的时候，线程会被强制终止，无法正常析构对象
//	当先在线程内部调用CloseHandle，然后再在线程外部调用TerminateThread的时候，线程不会终止，会一直运行到return语句。但是此时WaitForMultipleObjects返回258 = WAIT_TIMEOUT。
void CThread::UpdateState(void)
{
	//	判断当前线程状态，如果线程已经停止
	if (hThread == NULL)
	{
		//	什么都不做
	}
	else
	{
		DWORD re = ::WaitForMultipleObjects(1, &hThread, TRUE, UPDATE_TIME_MS);
		//	判断返回代码，若为超时，即线程依然存在，包括运行和挂起两种情况
		if (re == WAIT_TIMEOUT)
		{
			//	什么都不做
		}
		//	否则，线程为自然终止，或内部错误
		else
		{
			//	终止线程状态更新
			DWORD code;
			GetExitCodeThread (hThread,&code);
			TerminateThread(hThread,code);
			CloseHandle(hThread);
			hThread = NULL;
			CurrentState = STOP;
		}
	}
}

//	用户已经自己终止了线程
void CThread::SafeStop(void)
{
	//	当未初始化过时
	if (init_flag == false)
	{
		CurrentState = STOP;
		return;
	}
	
	//	终止线程，此时线程并不会马上终止，随后的return会正常析构该线程的资源
	CloseHandle(hThread);
	hThread = NULL;
	CurrentState = STOP;
}

//-------------------------------------------------------------------------------------
//	线程运行安全类

//	构造函数
CThread::CThreadRunSafe::CThreadRunSafe(CThread* in_p)
{
	pThread = in_p;
}

//	构造函数
CThread::CThreadRunSafe::CThreadRunSafe(CThread& in_c)
{
	pThread = &(in_c);
}

//	析构函数
CThread::CThreadRunSafe::~CThreadRunSafe()
{
	pThread->SafeStop();
}

