//////////////////////////////////////////////////////////////////////////
/************************************************************************

	工程名称：临界类
	工程日期：2016.12.06
	工程版本：0.2
	设    计：rainhenry
	编    码：rainhenry
	调试审核：rainhenry
	平    台：VS2010+Win7 Sp1 64bit

	免责声明：
	本代码可以随意复制传播使用，仅供学习研究参考，切勿用于商业用途
	使用本代码时，请保留本注释。

	功能描述：
	实现多线程通信时保护数据，通过进入临界和离开临界
	经测试，同一个线程内，对同一个临界对象进行进入操作的话，不会锁死。只有在两个不同的线程对同一个临界对象操作才会锁死。
	当临界初始化的时候，已经上锁的临界对象保持不变

	设计参考：
	http://blog.csdn.net/jerry4711/article/details/3906594

	使用方法：
	1、定义实例
	2、进入临界
	3、操作数据
	4、离开临界

	使用举例：
	CMyCriticalSection myccs;
	myccs.Enter();
	//....操作数据
	myccs.Leave();

	修订：
	0.2版本：更改类名为CMyCriticalSection，防止与afxmt.h文件中的类名重复冲突

************************************************************************/

//////////////////////////////////////////////////////////////////////////
//	预编译头保护
#ifndef __CMyCriticalSection_hpp__
#define __CMyCriticalSection_hpp__

//////////////////////////////////////////////////////////////////////////
//	包含头文件
#include <iostream>
#include <Windows.h>

//////////////////////////////////////////////////////////////////////////
//	类定义
class CMyCriticalSection
{
private:
	//	临界变量
	CRITICAL_SECTION CS;

	//	进入临界的状态
	bool InCS;				//	true: IN_CS			false:	NOT_CS

public:
	//	构造函数
	CMyCriticalSection()
	{
		//	初始化临界变量
		InitializeCriticalSection(&CS);

		//	设置标志
		InCS = false;		//	没有在临界中
	}

	//	析构函数
	~CMyCriticalSection()
	{
		//	释放临界变量
		DeleteCriticalSection(&CS);
	}

	//	进入临界
	void Enter(void)
	{
		EnterCriticalSection(&CS);
		InCS = true;
	}

	//	离开临界
	void Leave(void)
	{
		LeaveCriticalSection(&CS);
		InCS = false;
	}

	//	判断是否在临界
	bool InCriticalSection(void)
	{
		return InCS;
	}
};


//////////////////////////////////////////////////////////////////////////
#endif	//	__CMyCriticalSection_hpp__
