/************************************************************************

	工程名称：自定义时间相关操作程序包
	工程日期：2016.12.31
	最后修改：2017.3.26
	版    本：0.6
	平    台：VS2010 Win7 64bit SP1
	设计编码：rainhenry

	版本修订
		0.2版本		增加对起始年份的兼容设置，REF_BEGIN_YEAR宏，在VS2010环境下为1900年起始
		0.3版本		修复了比较时间点的BUG
					修复了除法叠加的BUG
					修复了不同倍数时间段叠加的BUG
		0.4版本		增加了以C格式字符串输出时间字符串函数GetTime_C_Str
		0.5版本		修改strcpy和sprintf为安全函数strcpy_s和sprintf_s
		0.6版本		增加GetSpanMillsec方法，得到时间差毫秒，用于扩展计算统计

	免责声明
		本程序请勿用于商业用途，仅供学习参考。

	设计目的和要求：
	1、获取本机当前时间，精确到秒。
	2、时分秒年月日到标准格式的转换。 例如1230423分钟表示为标准格式，而3个月2天23小时44分0秒转换为分钟等。
	3、可以获取当前时间点。（自动分辨毫秒级精确获取，和秒级的获取，秒级别为全日历方式）
	4、两个时间点对象可以计算时差。
	5、可以比较时间点大小。

	设计思路：
	1、相对时间模式
	2、绝对时间模式

	注意事项：
	1、GetTimeString输出类型为std::string类型，所以不受到系统的字符集设置的影响。输出均为多字节字符集，GB2312编码。
	
	使用说明
	//////////////////////////////////////////////////////////////////////////
	//	本地时间获取测试
	int count = 0;
	CMyTime a;
	a.GetNowTime();
	printf("%d    %s\r\n",++count,a.GetTimeString().data());	//================1

	//------------------------------------------------------------------------
	//	时间加法、减法测试
	CMyTime b = CMyTime(2016,3,10,0,0,0,0,CMyTime::POINT) + CMyTime(0,0,0,0,0,0,-1,CMyTime::RANGE) - CMyTime(0,0,0,0,0,0,1,CMyTime::RANGE);
	printf("%d    %s\r\n",++count,b.GetTimeString().data());	//================2
	b = CMyTime(0,0,0,0,0,0,-1,CMyTime::RANGE) + CMyTime(2016,3,10,0,0,0,0,CMyTime::POINT);
	printf("%d    %s\r\n",++count,b.GetTimeString().data());	//================3

	//------------------------------------------------------------------------
	//	整理格式的时间段测试
	CMyTime c(0,-2,-15,10,-50,24,-345,CMyTime::RANGE);
	//c.SetRangeTime(2,3,1,73,50,24,345);
	//c.SetRangeTime(2,3,1,2,3,4,345);
	printf("%d    %s\r\n",++count,c.GetTimeString().data());	//================4

	//------------------------------------------------------------------------
	//	时间差测试
	CMyTime d = CMyTime(2014,4,21,4,0,0,50,CMyTime::POINT) - CMyTime(2015,4,21,0,0,0,0,CMyTime::POINT);
	printf("%d    %s\r\n",++count,d.GetTimeString().data());	//================5

	//------------------------------------------------------------------------
	//	乘法测试
	CMyTime e = CMyTime(10,2,3,10,2,1,10,CMyTime::RANGE) * 2;
	printf("%d    %s\r\n",++count,e.GetTimeString().data());	//================6

	//------------------------------------------------------------------------
	//	小数月测试
	CMyTime f = CMyTime(2014,2,1,0,0,0,0,CMyTime::POINT) + CMyTime(0,1,0,0,0,0,0,0.5,1.0);
	printf("%d    %s\r\n",++count,f.GetTimeString().data());	//================7

	//------------------------------------------------------------------------
	//	时间点相等测试
	printf("%d    %d\r\n",++count,CMyTime(2015,5,9,0,0,0,0,CMyTime::POINT)==CMyTime(2015,5,10,0,0,0,0,CMyTime::POINT));	//================8

	//------------------------------------------------------------------------
	//	时间段相等测试
	printf("%d    %d\r\n",++count,CMyTime(0,5,11,0,0,0,0,CMyTime::RANGE)==CMyTime(0,5,11,0,0,0,0,CMyTime::RANGE));		//================9

	//------------------------------------------------------------------------
	//	时间点大于测试
	printf("%d    %d\r\n",++count,CMyTime(2015,5,10,0,0,0,0,CMyTime::POINT)>CMyTime(2015,5,10,0,0,0,0,CMyTime::POINT));	//================10

	//------------------------------------------------------------------------
	//	时间段大于测试
	printf("%d    %d\r\n",++count,CMyTime(0,7,5,0,0,0,0,CMyTime::RANGE)>CMyTime(0,5,10,0,0,0,0,CMyTime::RANGE));		//================11

	//------------------------------------------------------------------------
	//	时间点小于等于测试
	CMyTime g;
	g.GetNowTime();
	printf("%d    %d\r\n",++count,g<=CMyTime(2017,1,4,0,0,0,0,CMyTime::POINT));											//================12

	//	程序运行结果
	1    2017/1/5 19:05:13_795 THU
	2    2016/3/9 23:59:59_998 WED
	3    2016/3/9 23:59:59_999 WED
	4    0/-2/-14 -14:-49:-36_-345
	5    0/0/-364 -19:-59:-59_-950
	6    20/4/6 20:04:02_20
	7    2014/2/15 00:00:00_0 SAT
	8    0
	9    1
	10    0
	11    1
	12    0


************************************************************************/

//////////////////////////////////////////////////////////////////////////
//	包含头文件
#ifndef __CMYTIME_H__
#define __CMYTIME_H__

//////////////////////////////////////////////////////////////////////////
//	MFC兼容处理
#include <afx.h>
#include <afxwin.h>
#include <iostream>
#include <string>

//////////////////////////////////////////////////////////////////////////
//	包含头文件
#define	REF_BEGIN_YEAR	1900
#define FAIL_SPANMILLSEC		(-1LL)

//////////////////////////////////////////////////////////////////////////
//	类定义
class CMyTime
{
public:
	//	时间类型定义
	typedef enum tagETimeType
	{
		UNDEFINE,			//	未定义的时间类型
		POINT,				//	时间点类型
		RANGE				//	时间段类型
	}ETimeType;

	//	存储格式定义，由于系统原来的SYSTEMTIME内部的数据类型为unsigned short将无法表示负值，并且范围也不够宽。
	typedef struct tagSISYSTEMTIME {
		int wYear;			//	无法表示REF_BEGIN_YEAR年之前的时间，此时2017年将对应2017值
		int wMonth;			//	1-31
		int wDayOfWeek;		//	0-6		0=星期日，6=星期六
		int wDay;			//	1-31
		int wHour;			//	0-23
		int wMinute;		//	0-59
		int wSecond;		//	0-59
		int wMilliseconds;	//	0-999	
		ETimeType type;		//	当前的类型（未定义，时间点，时间差/段）
		double Multiple;	//	当为时间段时，对基数进行放大的倍数，相当于乘法
		double Dividend;	//	当为时间段时，对基数进行缩小的倍数，相当于除法
	} SISYSTEMTIME;

public:
	//	无参数的构造
	CMyTime();

	//	指定类指针的构造函数
	CMyTime(CMyTime* p);

	//	指定时间数据实例的构造函数
	CMyTime(SISYSTEMTIME dat);

	//	指定时间的构造，默认1倍数
	CMyTime(
		int year,		//	年
		int mon,		//	月
		int day,		//	日
		int hour,		//	小时
		int min,		//	分钟
		int sec,		//	秒
		int ms,			//	毫秒
		ETimeType type	//	指定的时间类型
		);

	//	指定时间段的构造，自定义倍数
	CMyTime(
		int year,		//	年
		int mon,		//	月
		int day,		//	日
		int hour,		//	小时
		int min,		//	分钟
		int sec,		//	秒
		int ms,			//	毫秒
		double mul,		//	乘数
		double div		//	除数
		);

	//	析构函数
	~CMyTime();

	//	获取当前时间
	void GetNowTime(void);

	//	以字符串形式输出时间数据，格式为返回类型为std::string
	std::string GetTimeString(void);

	//	以字符串形式输出时间数据，格式为返回类型为兼容C语言的字符串
	char* GetTime_C_Str(void);

	//	将时间规范格式，可供外部调用
	static SISYSTEMTIME maketime(
		SISYSTEMTIME intime				//	输入的时间数据
		);

	//	设置点时间
	void SetPointTime(
		int year,		//	年
		int mon,		//	月
		int day,		//	日
		int hour,		//	小时
		int min,		//	分钟
		int sec,		//	秒
		int ms			//	毫秒
		);

	//	设置段时间，默认为1倍数
	void SetRangeTime(
		int year,		//	年
		int mon,		//	月
		int day,		//	日
		int hour,		//	小时
		int min,		//	分钟
		int sec,		//	秒
		int ms			//	毫秒
		);

	//	设置段时间，用户自定义倍数
	void SetRangeTime(
		int year,		//	年
		int mon,		//	月
		int day,		//	日
		int hour,		//	小时
		int min,		//	分钟
		int sec,		//	秒
		int ms,			//	毫秒
		double mul,		//	乘数
		double div		//	除数
		);

	//	获得时间数据
	SISYSTEMTIME GetData(void);

	//	时间段求绝对值
	void Abs(void);

	//	时间段求相反数
	void Inv(void);

	//	赋值运算重载
	void operator=(CMyTime mcmytime);
	
	//	加法运算重载
	CMyTime operator+(CMyTime mcmytime);

	//	减法运算重载
	CMyTime operator-(CMyTime mcmytime);

	//	成法运算重载
	CMyTime operator*(double b);

	//	除法运算重载
	CMyTime operator/(double b);

	//	比较是否相同
	bool operator==(CMyTime intime);
	
	//	大于运算重载
	bool operator>(CMyTime intime);
	
	//	小于运算重载
	bool operator<(CMyTime intime);

	//	比较是否不相同
	bool operator!=(CMyTime intime);
	
	//	大于等于运算重载
	bool operator>=(CMyTime intime);
	
	//	小于等于运算重载
	bool operator<=(CMyTime intime);

	//	得到时间差的毫秒值，即时间段，当无法计算的时候返回FAIL_SPANMILLSEC
	LONGLONG GetSpanMillsec(void);

private:
	//	保存的时间数据
	SISYSTEMTIME timedat;		

	//	将时间规范格式，仅供内部调用
	void maketime(void);

	//	定义返回字符串
	char re_str[200];

};

//////////////////////////////////////////////////////////////////////////
#endif	//	__CMYTIME_H__

