/************************************************************************

	工程名称：线程类
	版    本：0.3
	工程日期：2016.12.10
	设计编写：rainhenry

	版本修订
		0.2版本		增加了不带参数的构造函数
					增加了配置函数
		0.3版本		增加了用户安全终止方法，当用户自动结束线程的时候调用该方法可以同步线程状态
					当用户调用这个方法的时候，往往都是在线程内部，所以此时不可以关闭线程句柄
					增加线程安全类，可以免去在线程内部的每条return语句前面添加SafeStop方法
					在线程开始的地方构造该对象即可实现

	功能概述：
	实现对线程的封装

	使用注意事项：
		在线程内部的return函数前面请跟随一句mCThread.SafeStop();方法的调用
		在mCThread.SafeStop();方法的调用之后，必须要跟随return!!!

	内部相关介绍
	//	当hThread=NULL时，WaitForMultipleObjects返回-1
	//	当线程正常运行的时候，WaitForMultipleObjects返回258 = WAIT_TIMEOUT
	//	当线程自动结束的时候，WaitForMultipleObjects返回0
	//	当线程挂起的时候，WaitForMultipleObjects返回258
	//	当在线程内部调用CloseHandle(hThread);的时候，WaitForMultipleObjects返回258 = WAIT_TIMEOUT，但是线程不会终止，线程会直到运行到return语句才会终止，
		然后在线程内部构造的类对象的析构函数也会正常执行。
	//	当在线程外部先调用TerminateThread，然后再调用CloseHandle的时候，线程会被强制终止，无法正常析构对象
	//	当在线程内部先调用TerminateThread，然后再调用CloseHandle的时候，线程会被强制终止，无法正常析构对象
	//	当先在线程内部调用CloseHandle，然后再在线程外部调用TerminateThread的时候，线程不会终止，会一直运行到return语句。但是此时WaitForMultipleObjects返回258 = WAIT_TIMEOUT。

	使用方法
	1、定义线程参数的数据类（主要为了保护访问）
	2、定义线程函数
	3、构造实例
    4、启动、停止等操作

	使用举例	
	//	声明
	extern	CThread mCThread;

	//	线程函数体
	DWORD MyFunc1(LPVOID lpParam)
	{
		//......用户代码

		//	线程退出的时候
		mCThread.SafeStop();
		return 0;
	}

	//	定义线程实例
	CThread mCThread(
		MyFunc1,						//	定义线程函数
		(LPVOID)(&mCUserInterFace)		//	给定参数数据入口
		);
	//	或者	
	//	定义线程实例
	CThread mCThread(
		MyFunc1,						//	定义线程函数
		(LPVOID)(&mCUserInterFace),		//	给定参数数据入口
		CThread::STOP					//	刚开始的时候，线程为关闭状态
		);

	//	操作
	mCThread.Run();		//	启动
	mCThread.Susp();	//	挂起
	mCThread.Stop();	//	停止！！这是强制停止！！如果有析构函数会导致内存泄露!!

	//	获得线程状态
	CThread::EThreadState mstate = mCThread.GetState();

	//	当采用线程安全类的时候
	//	线程函数体
	DWORD MyFunc1(LPVOID lpParam)
	{
		//	构造安全类
		CThread::CThreadRunSafe msafethread(mCThread);

		//......用户代码

		//	线程退出的时候
		return 0;
	}

************************************************************************/

//////////////////////////////////////////////////////////////////////////
//	预编译头保护
#ifndef __CTREAD_H__
#define __CTREAD_H__

//////////////////////////////////////////////////////////////////////////
//	包含头文件
#include <iostream>
#include <afx.h>
#include <afxwin.h>

//////////////////////////////////////////////////////////////////////////
//	相关配置与宏定义
#define		UPDATE_TIME_MS			20					//	判断线程是否正常退出的时间为20ms

//////////////////////////////////////////////////////////////////////////
//	类定义
class CThread
{
public:
	//	线程函数指针类型定义
	typedef	DWORD (*P_THREAD_FUNC)(LPVOID lpParam);

	//	线程状态定义
	typedef enum tagEThreadState
	{
		RUN,				//	线程运行中
		STOP,				//	线程已终止，或未启动
		SUS					//	线程被挂起
	}EThreadState;

private:
	//	线程函数的地址
	P_THREAD_FUNC pThreadFunc;

	//	线程函数的入口参数
	LPVOID pThreadParam;

	//	线程句柄
	HANDLE hThread;

	//	当前的线程状态
	EThreadState CurrentState;

	//	更新当前的线程状态，该函数主要判断当前线程是否为自动自然终止
	void UpdateState(void);

	//	初始化标志
	bool init_flag;

public:
	//	不带参数的构造函数
	CThread();

	//	构造函数
	CThread(
		P_THREAD_FUNC pFunc,		//	线程的函数指针
		LPVOID	pParam,				//	给线程传递的参数
		EThreadState state = RUN	//	线程启动后的状态，默认为运行，也可以设置为终止的，除了启动参数外，其余参数均为终止。
		);

	//	配置函数，成功返回true,失败返回false
	bool Config(
		P_THREAD_FUNC pFunc,		//	线程的函数指针
		LPVOID	pParam,				//	给线程传递的参数
		EThreadState state = RUN	//	线程启动后的状态，默认为运行，也可以设置为终止的，除了启动参数外，其余参数均为终止。
		);

	//	析构函数
	~CThread();

	//	启动线程或者恢复线程
	void Run(void);

	//	挂起线程
	void Susp(void);

	//	终止线程
	void Stop(void);

	//	获取线程状态
	EThreadState GetState(void);

	//	用户已经自己终止了线程
	void SafeStop(void);

public:
	//	线程运行安全类
	class CThreadRunSafe
	{
	private:
		CThread* pThread;

	public:
		//	构造函数
		CThreadRunSafe(CThread* in_p);

		//	构造函数
		CThreadRunSafe(CThread& in_c);

		//	析构函数
		~CThreadRunSafe();
	};
};

//////////////////////////////////////////////////////////////////////////
#endif	//	__CTREAD_H__

