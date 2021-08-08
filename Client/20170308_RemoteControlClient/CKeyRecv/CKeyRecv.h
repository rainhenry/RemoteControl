/**************************************************************************************************

	工程名称：键盘记录类
	创建日期：2017.3.8
	最后修改：2017.3.13
	版    本：REV 0.1
	设计编码：rainhenry
	平    台：VS2013 @ Win7 64bit SP1

	版本修订：
		0.1版本			创建项目

	功能原理：
		可以添加按键，所有被添加进来的按键当同时按下的时候才会触发用户处理函数
		该类有一个静态的心跳方法，内部的原理是检测按键的状态然后执行相关的用户函数
		每个类对象有一个按键列表 和 一个用户函数指针
		所有的对象，即类的静态变量，拥有一个包含所有类对象所创建的监听信息，用于心跳函数处理

		当多个按键的时候，最后添加的按键为主要触发按键，即主要触发按键的改变信号将触发用户函数的调用。其余按键必须在按下的情况。

	使用举例：
	//	定义按键响应函数
	void UserOnKey(
		CKeyRecv_base::EOnKeyType type,						//	按键的触发类型
		LPVOID lpparam										//	传递进来的参数
		)
	{
		//	如果为抬起的触发事件
		if (type == CKeyRecv::ON_UP)
		{
			//....
		}
	}

	//	配置对象，可以定义多个对象，每个对象也可以定义多个按键，但是对于其中的同一个对象来讲，最后添加的按键是主要按键，即主要检测触发事件
	CKeyRecv k1;											//	定义按键类实例
	k1.AddKey(VK_F4);										//	添加按键
	k1.Config(UserOnKey, &window_show_status);				//	配置之后才会生效，第二个参数为可选参数，是传递到用户响应函数的用户自定义参数

	//	然后再在OnTimer或者其他线程中调用心跳函数，一般检测10ms一次，高速要求可以1ms一次。用户的函数是由该心跳函数调用的
	CKeyRecv::HeartbeatOnHandle();

**************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////
//	重定义保护
#ifndef __CKEYRECV_H__
#define __CKEYRECV_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
//	包含头文件
#include <vector>
#include <set>
#include <algorithm>
#include <afx.h>
#include "../CMyCriticalSection/CMyCriticalSection.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
//	类型基类定义
class CKeyRecv_base
{
public:
	//	定义按键触发类型枚举
	typedef enum tagEOnKeyType
	{
		ON_DOWN = 1,						//	按下的时候
		ON_UP = 2							//	抬起的时候
	}EOnKeyType;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//	定义相关
//	需要用户定义的按键响应函数指针
typedef void(*P_USER_CKEYRECV_ONKEY_FUNC)(
	CKeyRecv_base::EOnKeyType,						//	按键的触发类型
	LPVOID											//	传递的参数
	);

//	按键抬起和按键按下的宏
#define CKEYRECV_KEY_UP_A			((SHORT)0)
#define CKEYRECV_KEY_UP_B			((SHORT)1)
#define CKEYRECV_KEY_DOWN_A			((SHORT)-127)
#define CKEYRECV_KEY_DOWN_B			((SHORT)-128)

///////////////////////////////////////////////////////////////////////////////////////////////////
//	类定义
class CKeyRecv :public CKeyRecv_base
{
public:
	//	定义存放单个对象信息的结构体
	typedef struct tagSOnKeyInfo
	{
		//	对象指针
		CKeyRecv* pObj;

		//	用于存储一个类对象的按键列表
		std::vector<int> KeyVec;

		//	定义用户响应按键的函数指针
		P_USER_CKEYRECV_ONKEY_FUNC pOnKeyFunc;

		//	用户传递给自定义函数的参数
		LPVOID lpParam;
	}SOnKeyInfo;

private:
	//	初始化配置标志
	bool init_flag;

	//	用于存储一个类对象的按键列表
	std::vector<int> KeyVec;

	//	定义用户响应按键的函数指针
	P_USER_CKEYRECV_ONKEY_FUNC pOnKeyFunc;

	//	用户传递给自定义函数的参数
	LPVOID lpParam;

	//	用于存储所有类对象信息的列表
	static std::vector<SOnKeyInfo> AllOnKeyInfo;

	//	多线程支持，增加访问公共信息的CS
	static CMyCriticalSection allonkeyinfo_cs;

protected:
	//	添加一个对象的信息
	static void AddInfo(		
		CKeyRecv* p_obj,								//	对象的指针
		std::vector<int> keyvec,						//	用于存储一个类对象的按键列表	
		P_USER_CKEYRECV_ONKEY_FUNC ponkeyfunc,			//	定义用户响应按键的函数指针
		LPVOID lpparam
		);

	//	删除一个对象的信息
	static void DeleteInfo(
		CKeyRecv* p_obj									//	对象的指针
		);

public:
	//	无参数构造函数
	CKeyRecv();

	//	析构函数
	~CKeyRecv();

	//	添加按键，返回true添加成功，false添加失败
	bool AddKey(int key_value);

	//	配置，返回true为配置成功，false为失败
	bool Config(
		P_USER_CKEYRECV_ONKEY_FUNC ponkeyfunc,			//	定义用户响应按键的函数指针
		LPVOID lpparam = NULL
		);

	//	释放资源
	void Release(void);

	//	提供外部调用的心跳处理函数
	static void HeartbeatOnHandle(void);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif	//	__CKEYRECV_H__



