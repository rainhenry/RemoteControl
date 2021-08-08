
//	版本 0.6

#include "stdafx.h"
#include "CMyTime.h"

//	构造函数
CMyTime::CMyTime()
{
	//	初始化为0
	timedat.wDay = 0;
	timedat.wDayOfWeek = 0;
	timedat.wHour = 0;
	timedat.wMilliseconds = 0;
	timedat.wMinute = 0;
	timedat.wMonth = 0;
	timedat.wSecond = 0;
	timedat.wYear = 0;
	timedat.type = UNDEFINE;		//	设置为未定义的时间类型

	timedat.Multiple = 1.0;
	timedat.Dividend = 1.0;
}

//	指定时间数据实例的构造函数
CMyTime::CMyTime(CMyTime::SISYSTEMTIME dat)
{
	this->timedat = dat;
}

//	指定类指针的构造函数
CMyTime::CMyTime(CMyTime* p)
{
	//	检查指针是否可用，不可用的话就构造一个空的
	if ((p == NULL)||(p == (CMyTime*)-1))
	{
		//	初始化为0
		timedat.wDay = 0;
		timedat.wDayOfWeek = 0;
		timedat.wHour = 0;
		timedat.wMilliseconds = 0;
		timedat.wMinute = 0;
		timedat.wMonth = 0;
		timedat.wSecond = 0;
		timedat.wYear = 0;
		timedat.type = UNDEFINE;		//	设置为未定义的时间类型

		timedat.Multiple = 1.0;
		timedat.Dividend = 1.0;
	}
	
	//	赋值
	this->timedat = p->timedat;
}

//	指定时间的构造
CMyTime::CMyTime(
	int year,		//	年
	int mon,		//	月
	int day,		//	日
	int hour,		//	小时
	int min,		//	分钟
	int sec,		//	秒
	int ms,			//	毫秒
	ETimeType type	//	指定的时间类型
	)
{
	timedat.wDay = day;
	timedat.wDayOfWeek = 0;
	timedat.wHour = hour;
	timedat.wMilliseconds = ms;
	timedat.wMinute = min;
	timedat.wMonth = mon;
	timedat.wSecond = sec;
	timedat.wYear = year;
	timedat.type = type;		

	timedat.Multiple = 1.0;
	timedat.Dividend = 1.0;

	maketime();
}

//	指定时间段的构造，自定义倍数
CMyTime::CMyTime(
	int year,		//	年
	int mon,		//	月
	int day,		//	日
	int hour,		//	小时
	int min,		//	分钟
	int sec,		//	秒
	int ms,			//	毫秒
	double mul,		//	乘数
	double div		//	除数
	)
{
	timedat.wDay = day;
	timedat.wDayOfWeek = 0;
	timedat.wHour = hour;
	timedat.wMilliseconds = ms;
	timedat.wMinute = min;
	timedat.wMonth = mon;
	timedat.wSecond = sec;
	timedat.wYear = year;
	timedat.type = RANGE;			//	此时只能为时间段

	timedat.Multiple = mul;
	timedat.Dividend = div;

	maketime();
}

//	析构函数
CMyTime::~CMyTime()
{

}

//	获得时间数据
CMyTime::SISYSTEMTIME CMyTime::GetData(void)
{
	return this->timedat;
}

//	获取当前时间
void CMyTime::GetNowTime(void)
{
	//	获取本地时间
	SYSTEMTIME local_time;
	GetLocalTime(&local_time);
	timedat.wDay			= local_time.wDay			;
	timedat.wDayOfWeek		= local_time.wDayOfWeek		;
	timedat.wHour			= local_time.wHour			;
	timedat.wMilliseconds	= local_time.wMilliseconds	;
	timedat.wMinute			= local_time.wMinute		;
	timedat.wMonth			= local_time.wMonth			;
	timedat.wSecond			= local_time.wSecond		;
	timedat.wYear			= local_time.wYear			;

	timedat.Multiple = 1.0;
	timedat.Dividend = 1.0;

	//	判断时间类型，由于获取之后就为时间点数据
	timedat.type = POINT;			//	设置为时间点数据
}

//	以字符串形式输出时间数据，格式为返回类型为std::string
std::string CMyTime::GetTimeString(void)
{
	//	整理格式
	maketime();

	//	输出时间数据字符串
	char weekstr[7][4] = 
	{
		"SUN",
		"MON",
		"TUE",
		"WED",
		"THU",
		"FRI",
		"SAT"
	};
	std::string str;
	char buff[100];
	//	这里判断为那种类型
	if (this->timedat.type == UNDEFINE)
	{
		sprintf_s(buff,"TIME TYPE UNDEFINE!");
	}
	else if (this->timedat.type == POINT)
	{
		sprintf_s(buff,"%d/%d/%d %02d:%02d:%02d_%d %s",
			timedat.wYear,
			timedat.wMonth,
			timedat.wDay,
			timedat.wHour,
			timedat.wMinute,
			timedat.wSecond,
			timedat.wMilliseconds,
			(char*)&(weekstr[timedat.wDayOfWeek][0])
			);
	}
	else	//	this->timedat.type == RANGE，这里显示的为最原始的要求，所以不进行换算和转换
	{
		sprintf_s(buff,"%lg/%lg/%lg %02lg:%02lg:%02lg_%lg",
			(timedat.wYear * timedat.Multiple / timedat.Dividend),
			(timedat.wMonth * timedat.Multiple / timedat.Dividend),
			(timedat.wDay * timedat.Multiple / timedat.Dividend),
			(timedat.wHour * timedat.Multiple / timedat.Dividend),
			(timedat.wMinute * timedat.Multiple / timedat.Dividend),
			(timedat.wSecond * timedat.Multiple / timedat.Dividend),
			(timedat.wMilliseconds * timedat.Multiple / timedat.Dividend)
			);
	}
	str = buff;

	return str;
}

//	以字符串形式输出时间数据，格式为返回类型为兼容C语言的字符串
char* CMyTime::GetTime_C_Str(void)
{
	//	清空之前的结果
	memset(re_str,0,sizeof(re_str));

	//	定义临时处理字符串
	std::string tmp_str = this->GetTimeString();

	//	检查内存容量
	if (tmp_str.size() >= sizeof(re_str))
	{
		//	提示字符串缓冲区容量过小
		strcpy_s(re_str,"String buffer too small!!");
		return re_str;
	}

	//	复制字符串
	strcpy_s(re_str,tmp_str.data());

	//	返回字符串
	return re_str;
}


//	将时间规范格式，可供外部调用
CMyTime::SISYSTEMTIME CMyTime::maketime(
	CMyTime::SISYSTEMTIME intime				//	输入的时间数据
	)
{
	//	计算毫秒余数
	int ms = intime.wMilliseconds % 1000;

	//	根据时间类型进行分类处理，若为未定义类型，则初始化为0，返回空时间
	if (intime.type == UNDEFINE)
	{
		//	初始化为0
		intime.wDay = 0;
		intime.wDayOfWeek = 0;
		intime.wHour = 0;
		intime.wMilliseconds = 0;
		intime.wMinute = 0;
		intime.wMonth = 0;
		intime.wSecond = 0;
		intime.wYear = 0;

		intime.Multiple = 1.0;
		intime.Dividend = 1.0;

		//	返回
		return intime;
	}
	//	当为时间点类型
	else if (intime.type == POINT)
	{
		//	构造tm结构
		struct tm mtm;
		time_t m_time_t;
		mtm.tm_year = intime.wYear - REF_BEGIN_YEAR;
		mtm.tm_mon = intime.wMonth - 1;
		mtm.tm_mday = intime.wDay;
		mtm.tm_isdst = 0;				//	夏令时不允许
		mtm.tm_hour = intime.wHour;
		mtm.tm_min = intime.wMinute;
		mtm.tm_sec = intime.wSecond + intime.wMilliseconds/1000;

		//	增加对ms的负值处理
		if (ms<0)
		{
			mtm.tm_sec--;
		}

		//	格式整理
		m_time_t = mktime(&mtm);

		//	构造返回数据
		SISYSTEMTIME mSISYSTEMTIME;
		struct tm mtm2;
		//memcpy(&mtm2,localtime(&m_time_t),sizeof(tm));
		localtime_s(&mtm2,&m_time_t);
		mSISYSTEMTIME.wYear = mtm2.tm_year + REF_BEGIN_YEAR;
		mSISYSTEMTIME.wMonth = mtm2.tm_mon + 1;
		mSISYSTEMTIME.wDay = mtm2.tm_mday;
		mSISYSTEMTIME.wHour = mtm2.tm_hour;
		mSISYSTEMTIME.wMinute = mtm2.tm_min;
		mSISYSTEMTIME.wSecond = mtm2.tm_sec;
		mSISYSTEMTIME.wMilliseconds = ms;
		mSISYSTEMTIME.wDayOfWeek = mtm2.tm_wday;
		mSISYSTEMTIME.type = POINT;
		mSISYSTEMTIME.Dividend = 1.0;
		mSISYSTEMTIME.Multiple = 1.0;

		//	对ms负值处理
		if (ms<0)
		{
			mSISYSTEMTIME.wMilliseconds = 1000 + ms;
		}

		//	返回
		return mSISYSTEMTIME;
	}
	//	当为时间段类型
	else	//	intime.type == RANGE
	{
		/*
			如果为时间段，理论上是不需要进行mktime的，因为为了保证时间段的精确性。
			因为30天并不等于一个月。
			这里为了方便计算，年、月都不动，而将天、时、分、秒都统一叠加到ms上面，进行统一符号。
		*/
		//	将天、时、分、秒、毫秒都转换为毫秒
		LONGLONG Totalms = intime.wDay*24LL*60LL*60LL*1000LL + intime.wHour*60LL*60LL*1000LL + intime.wMinute*60LL*1000LL + intime.wSecond*1000LL + intime.wMilliseconds;

		//	将毫秒转换为天、时、分、秒、毫秒
		intime.wDay = static_cast<int>(Totalms/24LL/60LL/60LL/1000LL);
		intime.wHour = static_cast<int>((Totalms - intime.wDay*24LL*60LL*60LL*1000LL)/60LL/60LL/1000LL);
		intime.wMinute = static_cast<int>((Totalms - intime.wDay*24LL*60LL*60LL*1000LL - intime.wHour*60LL*60LL*1000LL)/60LL/1000LL);
		intime.wSecond = static_cast<int>((Totalms - intime.wDay*24LL*60LL*60LL*1000LL - intime.wHour*60LL*60LL*1000LL - intime.wMinute*60LL*1000LL)/1000LL);
		intime.wMilliseconds = static_cast<int>(Totalms - intime.wDay*24LL*60*60LL*1000LL - intime.wHour*60LL*60LL*1000LL - intime.wMinute*60LL*1000LL - intime.wSecond*1000LL);

		//	返回
		return intime;
	}
}

//	得到时间差的毫秒值，即时间段，当无法计算的时候返回FAIL_SPANMILLSEC
LONGLONG CMyTime::GetSpanMillsec(void)
{
	//	检查时间类型
	if (this->timedat.type == RANGE)
	{
		//	检查年和月的值，如果为不可以精确计算的
		if ((this->timedat.wYear != 0) || (this->timedat.wMonth != 0))
		{
			return FAIL_SPANMILLSEC;
		}
		else
		{
			return this->timedat.wDay * 24LL * 60LL * 60LL * 1000LL + this->timedat.wHour * 60LL * 60LL * 1000LL + this->timedat.wMinute * 60LL * 1000LL + this->timedat.wSecond * 1000LL + this->timedat.wMilliseconds;
		}
	}
	else
	{
		return FAIL_SPANMILLSEC;
	}
}

//	将时间规范格式，仅供内部调用
void CMyTime::maketime(void)
{
	timedat = CMyTime::maketime(timedat);
}

//	设置点时间
void CMyTime::SetPointTime(
	int year,		//	年
	int mon,		//	月
	int day,		//	日
	int hour,		//	小时
	int min,		//	分钟
	int sec,		//	秒
	int ms			//	毫秒
	)
{
	timedat.wDay = day;
	timedat.wHour = hour;
	timedat.wMilliseconds = ms;
	timedat.wMinute = min;
	timedat.wMonth = mon;
	timedat.wSecond = sec;
	timedat.wYear = year;
	timedat.Multiple = 1.0;
	timedat.Dividend = 1.0;
	timedat.type = POINT;		
	maketime();
}

//	设置段时间，默认为1倍数
void CMyTime::SetRangeTime(
	int year,		//	年
	int mon,		//	月
	int day,		//	日
	int hour,		//	小时
	int min,		//	分钟
	int sec,		//	秒
	int ms			//	毫秒
	)
{
	timedat.wDay = day;
	timedat.wHour = hour;
	timedat.wMilliseconds = ms;
	timedat.wMinute = min;
	timedat.wMonth = mon;
	timedat.wSecond = sec;
	timedat.wYear = year;
	timedat.wDayOfWeek = 0;		//	段时间里面不给出星期
	timedat.Multiple = 1.0;
	timedat.Dividend = 1.0;
	timedat.type = RANGE;		
	maketime();
}

//	设置段时间，用户自定义倍数
void CMyTime::SetRangeTime(
	int year,		//	年
	int mon,		//	月
	int day,		//	日
	int hour,		//	小时
	int min,		//	分钟
	int sec,		//	秒
	int ms,			//	毫秒
	double mul,		//	乘数
	double div		//	除数
	)
{
	timedat.wDay = day;
	timedat.wHour = hour;
	timedat.wMilliseconds = ms;
	timedat.wMinute = min;
	timedat.wMonth = mon;
	timedat.wSecond = sec;
	timedat.wYear = year;
	timedat.wDayOfWeek = 0;		//	段时间里面不给出星期
	timedat.Multiple = mul;
	timedat.Dividend = div;
	timedat.type = RANGE;		
	maketime();
}

//	时间段求绝对值
void CMyTime::Abs(void)
{
	//	判断是否为时间段类型
	if (this->timedat.type == RANGE)
	{
		//	整理格式
		this->maketime();

		//	对年和月进行整理，并取绝对值
		int Total_mon = this->timedat.wYear*12 + this->timedat.wMonth;
		this->timedat.wYear = static_cast<int>(::abs(static_cast<double>(Total_mon/12)));
		this->timedat.wMonth = static_cast<int>(::abs(static_cast<double>(Total_mon - this->timedat.wYear*12)));

		//	得到天以下的精确表示
		LONGLONG Totalms = this->timedat.wDay*24LL*60LL*60LL*1000LL + this->timedat.wHour*60LL*60LL*1000LL + this->timedat.wMinute*60LL*1000LL + this->timedat.wSecond*1000LL + this->timedat.wMilliseconds;
		
		//	取天以下的绝对值
		//	判断是否有小于0的数
		if(this->timedat.wDay < 0) this->timedat.wDay = static_cast<int>(::abs(static_cast<double>(this->timedat.wDay)));
		if(this->timedat.wHour < 0) this->timedat.wHour = static_cast<int>(::abs(static_cast<double>(this->timedat.wHour)));
		if(this->timedat.wMinute < 0) this->timedat.wMinute = static_cast<int>(::abs(static_cast<double>(this->timedat.wMinute)));
		if(this->timedat.wSecond < 0) this->timedat.wSecond = static_cast<int>(::abs(static_cast<double>(this->timedat.wSecond)));
		if(this->timedat.wMilliseconds < 0) this->timedat.wMilliseconds = static_cast<int>(::abs(static_cast<double>(this->timedat.wMilliseconds)));

		//	考虑是否进行变号，如果二者同号，不作处理
		if (
			((Total_mon < 0)&&(Totalms < 0)) ||
			((Total_mon > 0)&&(Totalms > 0))
			)
		{
			//	不处理
		}
		//	否则符号不同，进行变号
		else
		{
			//	变号
			this->timedat.wDay = 0-(this->timedat.wDay);
			this->timedat.wHour = 0-(this->timedat.wHour);
			this->timedat.wMinute = 0-(this->timedat.wMinute);
			this->timedat.wSecond = 0-(this->timedat.wSecond);
			this->timedat.wMilliseconds = 0-(this->timedat.wMilliseconds);
		}
	}
	//	其他类型
	else
	{
		//	不做任何操作
	}
}

//	时间段求相反数
void CMyTime::Inv(void)
{
	//	判断是否为时间段类型
	if (this->timedat.type == RANGE)
	{
		//	整理格式
		this->maketime();

		//	变号
		this->timedat.wYear = 0-(this->timedat.wYear);
		this->timedat.wMonth = 0-(this->timedat.wMonth);
		this->timedat.wDay = 0-(this->timedat.wDay);
		this->timedat.wHour = 0-(this->timedat.wHour);
		this->timedat.wMinute = 0-(this->timedat.wMinute);
		this->timedat.wSecond = 0-(this->timedat.wSecond);
		this->timedat.wMilliseconds = 0-(this->timedat.wMilliseconds);
	}
	//	其他类型
	else
	{
		//	不做任何操作
	}
}

//	赋值运算重载
void CMyTime::operator=(CMyTime mcmytime)
{
	this->timedat = mcmytime.timedat;
}

//	加法运算重载
CMyTime CMyTime::operator+(CMyTime mcmytime)
{
	//	根据自身类型进行分别处理，如果为未定义
	if (this->timedat.type == UNDEFINE)
	{
		//	根据传入参数类型进行分别处理，如果为未定义
		if (mcmytime.timedat.type == UNDEFINE)
		{
			//	返回一个空的时间对象
			CMyTime m;
			return m;
		}
		//	传入参数类型为时间点
		else if (mcmytime.timedat.type == POINT)
		{
			//	返回传入对象
			return mcmytime;
		}
		//	传入参数类型为时间段
		else	//	mcmytime.timedat.type == RANGE
		{
			//	返回传入对象
			return mcmytime;
		}
	}
	//	自身类型为时间点
	else if (this->timedat.type == POINT)
	{
		//	根据传入参数类型进行分别处理，如果为未定义
		if (mcmytime.timedat.type == UNDEFINE)
		{
			//	返回本身对象
			CMyTime m;
			m.timedat = this->timedat;
			return m;
		}
		//	传入参数类型为时间点
		else if (mcmytime.timedat.type == POINT)
		{
			//	同时为时间点为错误操作，返回空的时间
//	字符集兼容配置
#ifdef UNICODE
			AfxMessageBox(L"错误！两个时间点发生相加！");
#else
			AfxMessageBox("错误！两个时间点发生相加！");
#endif
			CMyTime m;
			return m;
		}
		//	传入参数类型为时间段
		else	//	mcmytime.timedat.type == RANGE
		{
			//	执行叠加计算
			CMyTime m;
			m.timedat.wYear = mcmytime.timedat.wYear					+	this->timedat.wYear;
			m.timedat.wMonth = mcmytime.timedat.wMonth					+	this->timedat.wMonth;
			m.timedat.wDay = mcmytime.timedat.wDay						+	this->timedat.wDay;
			m.timedat.wHour = mcmytime.timedat.wHour					+	this->timedat.wHour;
			m.timedat.wMinute = mcmytime.timedat.wMinute				+	this->timedat.wMinute;
			m.timedat.wSecond = mcmytime.timedat.wSecond				+	this->timedat.wSecond;
			m.timedat.wMilliseconds = mcmytime.timedat.wMilliseconds	+	this->timedat.wMilliseconds;
			m.timedat.type = POINT;
			m.timedat.Dividend = 1.0;
			m.timedat.Multiple = 1.0;
			m.maketime();

			//	获取一倍基数所产生的时间差，新值-旧值
			CMyTime span = m - CMyTime(this);

			//	将天、时、分、秒、毫秒都转换为毫秒
			LONGLONG Totalms = 
				span.timedat.wDay*24LL*60LL*60LL*1000LL + 
				span.timedat.wHour*60LL*60LL*1000LL + 				
				span.timedat.wMinute*60LL*1000LL + 
				span.timedat.wSecond*1000LL + 
				span.timedat.wMilliseconds;

			//	用毫秒数进行倍数计算
			Totalms = static_cast<LONGLONG>(Totalms * mcmytime.timedat.Multiple / mcmytime.timedat.Dividend);

			//	将毫秒转换为天、时、分、秒、毫秒，此时span为新的时间段
			span.timedat.wDay = static_cast<int>(Totalms/24LL/60LL/60LL/1000LL);
			span.timedat.wHour = static_cast<int>((Totalms - span.timedat.wDay*24LL*60LL*60LL*1000LL)/60LL/60LL/1000LL);
			span.timedat.wMinute = static_cast<int>((Totalms - span.timedat.wDay*24LL*60LL*60LL*1000LL - span.timedat.wHour*60LL*60LL*1000LL)/60LL/1000LL);
			span.timedat.wSecond = static_cast<int>((Totalms - span.timedat.wDay*24LL*60LL*60LL*1000LL - span.timedat.wHour*60LL*60LL*1000LL - span.timedat.wMinute*60LL*1000LL)/1000LL);
			span.timedat.wMilliseconds = static_cast<int>(Totalms - span.timedat.wDay*24LL*60*60LL*1000LL - span.timedat.wHour*60LL*60LL*1000LL - span.timedat.wMinute*60LL*1000LL - span.timedat.wSecond*1000LL);

			//	代入计算新的时间段
			m.timedat.wYear = this->timedat.wYear;
			m.timedat.wMonth = this->timedat.wMonth;
			m.timedat.wDay = this->timedat.wDay						+	span.timedat.wDay;
			m.timedat.wHour = this->timedat.wHour					+	span.timedat.wHour;
			m.timedat.wMinute = this->timedat.wMinute				+	span.timedat.wMinute;
			m.timedat.wSecond = this->timedat.wSecond				+	span.timedat.wSecond;
			m.timedat.wMilliseconds = this->timedat.wMilliseconds	+	span.timedat.wMilliseconds;
			m.timedat.type = POINT;

			//	整理格式
			m.maketime();

			//	返回叠加结果
			return m;
		}
	}
	//	自身类型为时间段
	else	//	this->timedat.type == RANGE
	{
		//	根据传入参数类型进行分别处理，如果为未定义
		if (mcmytime.timedat.type == UNDEFINE)
		{
			//	返回本身对象
			CMyTime m;
			m.timedat = this->timedat;
			return m;
		}
		//	传入参数类型为时间点
		else if (mcmytime.timedat.type == POINT)
		{
			//	执行叠加计算
			CMyTime m;
			m.timedat.wYear = mcmytime.timedat.wYear					+	this->timedat.wYear;
			m.timedat.wMonth = mcmytime.timedat.wMonth					+	this->timedat.wMonth;
			m.timedat.wDay = mcmytime.timedat.wDay						+	this->timedat.wDay;
			m.timedat.wHour = mcmytime.timedat.wHour					+	this->timedat.wHour;
			m.timedat.wMinute = mcmytime.timedat.wMinute				+	this->timedat.wMinute;
			m.timedat.wSecond = mcmytime.timedat.wSecond				+	this->timedat.wSecond;
			m.timedat.wMilliseconds = mcmytime.timedat.wMilliseconds	+	this->timedat.wMilliseconds;
			m.timedat.type = POINT;
			m.timedat.Dividend = 1.0;
			m.timedat.Multiple = 1.0;
			m.maketime();

			//	获取一倍基数所产生的时间差，新值-旧值
			CMyTime span = m - mcmytime;

			//	将天、时、分、秒、毫秒都转换为毫秒
			LONGLONG Totalms = 
				span.timedat.wDay*24LL*60LL*60LL*1000LL + 
				span.timedat.wHour*60LL*60LL*1000LL + 				
				span.timedat.wMinute*60LL*1000LL + 
				span.timedat.wSecond*1000LL + 
				span.timedat.wMilliseconds;

			//	用毫秒数进行倍数计算
			Totalms = static_cast<LONGLONG>(Totalms * this->timedat.Multiple / this->timedat.Dividend);

			//	将毫秒转换为天、时、分、秒、毫秒，此时span为新的时间段
			span.timedat.wDay = static_cast<int>(Totalms/24LL/60LL/60LL/1000LL);
			span.timedat.wHour = static_cast<int>((Totalms - span.timedat.wDay*24LL*60LL*60LL*1000LL)/60LL/60LL/1000LL);
			span.timedat.wMinute = static_cast<int>((Totalms - span.timedat.wDay*24LL*60LL*60LL*1000LL - span.timedat.wHour*60LL*60LL*1000LL)/60LL/1000LL);
			span.timedat.wSecond = static_cast<int>((Totalms - span.timedat.wDay*24LL*60LL*60LL*1000LL - span.timedat.wHour*60LL*60LL*1000LL - span.timedat.wMinute*60LL*1000LL)/1000LL);
			span.timedat.wMilliseconds = static_cast<int>(Totalms - span.timedat.wDay*24LL*60*60LL*1000LL - span.timedat.wHour*60LL*60LL*1000LL - span.timedat.wMinute*60LL*1000LL - span.timedat.wSecond*1000LL);

			//	代入计算新的时间段
			m.timedat.wYear = mcmytime.timedat.wYear;
			m.timedat.wMonth = mcmytime.timedat.wMonth;
			m.timedat.wDay = mcmytime.timedat.wDay						+	span.timedat.wDay;
			m.timedat.wHour = mcmytime.timedat.wHour					+	span.timedat.wHour;
			m.timedat.wMinute = mcmytime.timedat.wMinute				+	span.timedat.wMinute;
			m.timedat.wSecond = mcmytime.timedat.wSecond				+	span.timedat.wSecond;
			m.timedat.wMilliseconds = mcmytime.timedat.wMilliseconds	+	span.timedat.wMilliseconds;
			m.timedat.type = POINT;

			//	整理格式
			m.maketime();

			//	返回叠加结果
			return m;
		}
		//	传入参数类型为时间段
		else	//	mcmytime.timedat.type == RANGE
		{
			//	如果倍数相同
			if ((this->timedat.Multiple == mcmytime.timedat.Multiple)&&(this->timedat.Dividend == mcmytime.timedat.Dividend))
			{
				//	执行叠加计算
				CMyTime m;
				m.timedat.wYear = mcmytime.timedat.wYear					+	this->timedat.wYear;
				m.timedat.wMonth = mcmytime.timedat.wMonth					+	this->timedat.wMonth;
				m.timedat.wDay = mcmytime.timedat.wDay						+	this->timedat.wDay;
				m.timedat.wHour = mcmytime.timedat.wHour					+	this->timedat.wHour;
				m.timedat.wMinute = mcmytime.timedat.wMinute				+	this->timedat.wMinute;
				m.timedat.wSecond = mcmytime.timedat.wSecond				+	this->timedat.wSecond;
				m.timedat.wMilliseconds = mcmytime.timedat.wMilliseconds	+	this->timedat.wMilliseconds;
				m.timedat.type = RANGE;
				m.maketime();
				return m;
			}
			//	若倍数不同，双方年月都为0的话
			else if ((this->timedat.wYear == 0)&&(this->timedat.wMonth == 0)&&(mcmytime.timedat.wYear == 0)&&(mcmytime.timedat.wMonth == 0))
			{
				//	将this的天、时、分、秒、毫秒都转换为毫秒
				LONGLONG Totalms_a = this->timedat.wDay*24LL*60LL*60LL*1000LL + this->timedat.wHour*60LL*60LL*1000LL + this->timedat.wMinute*60LL*1000LL + this->timedat.wSecond*1000LL + this->timedat.wMilliseconds;
				Totalms_a = static_cast<LONGLONG>(Totalms_a * this->timedat.Multiple / this->timedat.Dividend);

				//	将输入参数的天、时、分、秒、毫秒都转换为毫秒
				LONGLONG Totalms_b = mcmytime.timedat.wDay*24LL*60LL*60LL*1000LL + mcmytime.timedat.wHour*60LL*60LL*1000LL + mcmytime.timedat.wMinute*60LL*1000LL + mcmytime.timedat.wSecond*1000LL + mcmytime.timedat.wMilliseconds;
				Totalms_b = static_cast<LONGLONG>(Totalms_b * mcmytime.timedat.Multiple / mcmytime.timedat.Dividend);

				//	叠加
				LONGLONG Totalms = Totalms_a + Totalms_b;

				//	将毫秒转换为天、时、分、秒、毫秒
				CMyTime m;
				m.timedat.wDay = static_cast<int>(Totalms/24LL/60LL/60LL/1000LL);
				m.timedat.wHour = static_cast<int>((Totalms - m.timedat.wDay*24LL*60LL*60LL*1000LL)/60LL/60LL/1000LL);
				m.timedat.wMinute = static_cast<int>((Totalms - m.timedat.wDay*24LL*60LL*60LL*1000LL - m.timedat.wHour*60LL*60LL*1000LL)/60LL/1000LL);
				m.timedat.wSecond = static_cast<int>((Totalms - m.timedat.wDay*24LL*60LL*60LL*1000LL - m.timedat.wHour*60LL*60LL*1000LL - m.timedat.wMinute*60LL*1000LL)/1000LL);
				m.timedat.wMilliseconds = static_cast<int>(Totalms - m.timedat.wDay*24LL*60*60LL*1000LL - m.timedat.wHour*60LL*60LL*1000LL - m.timedat.wMinute*60LL*1000LL - m.timedat.wSecond*1000LL);
				m.timedat.type = RANGE;

				//	返回
				return m;
			}
			//	不能精确计算
			else
			{
				//	返回空的时间段
				CMyTime m(0,0,0,0,0,0,0,RANGE);
				return m;
			}
		}

	}
}

//	减法运算重载
CMyTime CMyTime::operator-(CMyTime mcmytime)
{
	//	创建临时对象
	CMyTime a,b;

	//	变号赋值，当输入参数为时间段
	if (mcmytime.timedat.type == RANGE)
	{
		//	赋值输入参数
		b.timedat.type			= mcmytime.timedat.type				;
		b.timedat.wYear			= 0-mcmytime.timedat.wYear			;
		b.timedat.wMonth		= 0-mcmytime.timedat.wMonth			;
		b.timedat.wDay			= 0-mcmytime.timedat.wDay			;
		b.timedat.wHour			= 0-mcmytime.timedat.wHour			;
		b.timedat.wMinute		= 0-mcmytime.timedat.wMinute		;
		b.timedat.wSecond		= 0-mcmytime.timedat.wSecond		;
		b.timedat.wMilliseconds = 0-mcmytime.timedat.wMilliseconds	;
		b.timedat.Dividend		= mcmytime.timedat.Dividend			;
		b.timedat.Multiple		= mcmytime.timedat.Multiple			;

		//	赋值自身参数
		a.timedat.type			= this->timedat.type			;
		a.timedat.wYear			= this->timedat.wYear			;
		a.timedat.wMonth		= this->timedat.wMonth			;
		a.timedat.wDay			= this->timedat.wDay			;
		a.timedat.wHour			= this->timedat.wHour			;
		a.timedat.wMinute		= this->timedat.wMinute			;
		a.timedat.wSecond		= this->timedat.wSecond			;
		a.timedat.wMilliseconds = this->timedat.wMilliseconds	;
		a.timedat.Dividend		= this->timedat.Dividend		;
		a.timedat.Multiple		= this->timedat.Multiple		;

		//	进行求和
		CMyTime re;
		re = a+b;

		//	返回数据
		return re;
	}
	//	当本身为时间段
	else if (this->timedat.type == RANGE)
	{
		//	赋值输入参数
		b.timedat.type			= mcmytime.timedat.type				;
		b.timedat.wYear			= mcmytime.timedat.wYear			;
		b.timedat.wMonth		= mcmytime.timedat.wMonth			;
		b.timedat.wDay			= mcmytime.timedat.wDay			;
		b.timedat.wHour			= mcmytime.timedat.wHour			;
		b.timedat.wMinute		= mcmytime.timedat.wMinute		;
		b.timedat.wSecond		= mcmytime.timedat.wSecond		;
		b.timedat.wMilliseconds = mcmytime.timedat.wMilliseconds	;
		b.timedat.Dividend		= mcmytime.timedat.Dividend			;
		b.timedat.Multiple		= mcmytime.timedat.Multiple			;

		//	赋值自身参数
		a.timedat.type			= this->timedat.type			;
		a.timedat.wYear			= 0-this->timedat.wYear			;
		a.timedat.wMonth		= 0-this->timedat.wMonth			;
		a.timedat.wDay			= 0-this->timedat.wDay			;
		a.timedat.wHour			= 0-this->timedat.wHour			;
		a.timedat.wMinute		= 0-this->timedat.wMinute			;
		a.timedat.wSecond		= 0-this->timedat.wSecond			;
		a.timedat.wMilliseconds = 0-this->timedat.wMilliseconds	;
		a.timedat.Dividend		= this->timedat.Dividend		;
		a.timedat.Multiple		= this->timedat.Multiple		;

		//	进行求和
		CMyTime re;
		re = a+b;

		//	返回数据
		return re;
	}
	//	当都为时间点的时候，则计算时间差
	else if ((mcmytime.timedat.type == POINT)&&(this->timedat.type == POINT))
	{
		//	首先分别对二者进行格式整理，主要防止ms数超出范围
		mcmytime.maketime();
		this->maketime();

		//	分别转换为秒和ms的时间值
		struct tm a,b;
		a.tm_year = this->timedat.wYear - REF_BEGIN_YEAR;
		a.tm_mon = this->timedat.wMonth - 1;
		a.tm_mday = this->timedat.wDay;
		a.tm_hour = this->timedat.wHour;
		a.tm_min = this->timedat.wMinute;
		a.tm_sec = this->timedat.wSecond;
		a.tm_isdst = 0;
		time_t a_s = mktime(&a);
		int a_ms = this->timedat.wMilliseconds;
		b.tm_year = mcmytime.timedat.wYear - REF_BEGIN_YEAR;
		b.tm_mon = mcmytime.timedat.wMonth - 1;
		b.tm_mday = mcmytime.timedat.wDay;
		b.tm_hour = mcmytime.timedat.wHour;
		b.tm_min = mcmytime.timedat.wMinute;
		b.tm_sec = mcmytime.timedat.wSecond;
		b.tm_isdst = 0;
		time_t b_s = mktime(&b);
		int b_ms = mcmytime.timedat.wMilliseconds;
		
		//	得到哪一个最小和最大
		time_t max_s,min_s;
		int max_ms,min_ms;
		//	若a的秒数大于b的秒数，此时为a-b的操作，a>b结果返回正
		if (a_s > b_s)
		{
			//	设置最大最小值
			max_s = a_s;
			max_ms = a_ms;
			min_s = b_s;
			min_ms = b_ms;

			//	计算差
			double diff_s = difftime(max_s,min_s);
			int diff_ms = max_ms - min_ms;
			
			//	构造返回的时间段，由于年、月为特殊规律，所以计算精度将控制在天以下。
			int day,hour,min,sec,ms;
			day = static_cast<int>(diff_s/60/60/24);
			hour = static_cast<int>((diff_s - day*24*60*60)/60/60);
			min = static_cast<int>((diff_s - day*24*60*60 - hour*60*60)/60);
			sec = static_cast<int>((diff_s - day*24*60*60 - hour*60*60 - min*60));
			ms = max_ms - min_ms;
			CMyTime re(0,0,day,hour,min,sec,ms,RANGE);
			re.maketime();
			//re.Abs();	//	由于采用difftime函数计算，所以结果一定为正值，所以省略该语句
			return re;
		}
		//	若a的秒数小于b的秒数，此时为a-b的操作，a<b结果返回负
		else if (a_s < b_s)
		{
			//	设置最大最小值
			max_s = b_s;
			max_ms = b_ms;
			min_s = a_s;
			min_ms = a_ms;

			//	计算差
			double diff_s = difftime(max_s,min_s);
			int diff_ms = max_ms - min_ms;

			//	构造返回的时间段，由于年、月为特殊规律，所以计算精度将控制在天以下。
			int day,hour,min,sec,ms;
			day = static_cast<int>(diff_s/60/60/24);
			hour = static_cast<int>((diff_s - day*24*60*60)/60/60);
			min = static_cast<int>((diff_s - day*24*60*60 - hour*60*60)/60);
			sec = static_cast<int>((diff_s - day*24*60*60 - hour*60*60 - min*60));
			ms = max_ms - min_ms;
			CMyTime re(0,0,day,hour,min,sec,ms,RANGE);
			re.maketime();
			re.Inv();		//	取相反数
			return re;
		}
		//	否则秒数相等，将比较ms
		else
		{
			//	这里不判断大小，直接返回代数值
			CMyTime m(0,0,0,0,0,0,a_ms-b_ms,RANGE);
			return m;
		}
	}
	//	没有时间段数据
	else
	{
		//	返回一个空的时间对象
		CMyTime m;
		return m;
	}
}


//	成法运算重载
CMyTime CMyTime::operator*(double b)
{
	//	时间的乘法运算只能考虑时间段，未定义的类型返回未定义
	if (this->timedat.type == UNDEFINE)
	{
		//	返回一个空的未定义时间类型
		CMyTime m;
		return m;
	}
	//	由于时间点不能参与运算，所以此时返回自身
	else if (this->timedat.type == POINT)
	{
		//	返回本身对象
		CMyTime m;
		m.timedat = this->timedat;
		return m;
	}
	//	最后一种就是返回时间段的乘法结果
	else
	{
		/*
			这里为了精确的表示时间和计算时间，乘除法的内容将不运算到基数里面，仅仅在倍数里面进行计算
		*/
		//	如果b为0，那么说明为空的时间段
		if (b == 0.0)
		{
			//	返回空的时间段
			CMyTime m(0,0,0,0,0,0,0,RANGE);
			return m;
		}
		//	不为0，则计算到倍数里面
		else
		{
			//	返回
			CMyTime m(this);
			m.timedat.Multiple *= b;
			return m;
		}

	}
}

//	除法运算重载
CMyTime CMyTime::operator/(double b)
{
	//	时间的除法运算只能考虑时间段，未定义的类型返回未定义
	if (this->timedat.type == UNDEFINE)
	{
		//	返回一个空的未定义时间类型
		CMyTime m;
		return m;
	}
	//	由于时间点不能参与运算，所以此时返回自身
	else if (this->timedat.type == POINT)
	{
		//	返回本身对象
		CMyTime m;
		m.timedat = this->timedat;
		return m;
	}
	//	最后一种就是返回时间段的除法结果
	else
	{
		/*
			这里为了精确的表示时间和计算时间，除除法的内容将不运算到基数里面，仅仅在倍数里面进行计算
		*/
		//	如果b为0，那么说明为空的时间段
		if (b == 0.0)
		{
			//	返回空的时间段
			CMyTime m(0,0,0,0,0,0,0,RANGE);
			return m;
		}
		//	不为0，则计算到倍数里面
		else
		{
			//	返回
			CMyTime m(this);
			m.timedat.Dividend *= b;
			return m;
		}

	}
}

//	比较是否相同
bool CMyTime::operator==(CMyTime intime)
{
	//	如果两参数类型不匹配则直接返回不相等
	if (this->timedat.type != intime.timedat.type)
	{
		return false;
	}

	//	判断类型进行处理，若为未定义
	if (this->timedat.type == UNDEFINE)
	{
		return true;
	}
	//	若为点时间
	else if (this->timedat.type == POINT)
	{
		//	首先进行格式整理
		this->maketime();
		intime.maketime();

		//	然后比较是否有不同
		if(this->timedat.wYear != intime.timedat.wYear)			return false;
		if(this->timedat.wMonth != intime.timedat.wMonth)			return false;
		if(this->timedat.wDay != intime.timedat.wDay)			return false;
		if(this->timedat.wHour != intime.timedat.wHour)			return false;
		if(this->timedat.wMinute != intime.timedat.wMinute)			return false;
		if(this->timedat.wSecond != intime.timedat.wSecond)			return false;
		if(this->timedat.wMilliseconds != intime.timedat.wMilliseconds)			return false;
		return true;
	}
	//	若为时间段
	else
	{
		//	首先进行格式整理
		this->maketime();
		intime.maketime();

		//	然后比较是否有不同
		if(this->timedat.wYear != intime.timedat.wYear)			return false;
		if(this->timedat.wMonth != intime.timedat.wMonth)			return false;
		if(this->timedat.wDay != intime.timedat.wDay)			return false;
		if(this->timedat.wHour != intime.timedat.wHour)			return false;
		if(this->timedat.wMinute != intime.timedat.wMinute)			return false;
		if(this->timedat.wSecond != intime.timedat.wSecond)			return false;
		if(this->timedat.wMilliseconds != intime.timedat.wMilliseconds)			return false;
		if(this->timedat.Dividend != intime.timedat.Dividend)			return false;		//	当为时间段的时候需要进行比较倍数
		if(this->timedat.Multiple != intime.timedat.Multiple)			return false;		
		return true;
	}
}


//	大于运算重载
bool CMyTime::operator>(CMyTime intime)
{
	//	如果两参数类型不匹配则直接返回不相等
	if (this->timedat.type != intime.timedat.type)
	{
		return false;
	}

	//	判断类型进行处理，若为未定义
	if (this->timedat.type == UNDEFINE)
	{
		return false;
	}
	//	若为点时间
	else if (this->timedat.type == POINT)
	{
		//	首先进行格式整理
		this->maketime();
		intime.maketime();

		//	分别转换为秒和ms的时间值
		struct tm a,b;
		a.tm_year = this->timedat.wYear - REF_BEGIN_YEAR;
		a.tm_mon = this->timedat.wMonth - 1;
		a.tm_mday = this->timedat.wDay;
		a.tm_hour = this->timedat.wHour;
		a.tm_min = this->timedat.wMinute;
		a.tm_sec = this->timedat.wSecond;
		a.tm_isdst = 0;
		time_t a_s = mktime(&a);
		int a_ms = this->timedat.wMilliseconds;
		b.tm_year = intime.timedat.wYear - REF_BEGIN_YEAR;
		b.tm_mon = intime.timedat.wMonth - 1;
		b.tm_mday = intime.timedat.wDay;
		b.tm_hour = intime.timedat.wHour;
		b.tm_min = intime.timedat.wMinute;
		b.tm_sec = intime.timedat.wSecond;
		b.tm_isdst = 0;
		time_t b_s = mktime(&b);
		int b_ms = intime.timedat.wMilliseconds;

		//	若a的秒数大于b的秒数
		if (a_s > b_s)
		{
			return true;
		}
		//	若a的秒数小于b的秒数
		else if (a_s < b_s)
		{
			return false;
		}
		//	否则秒数相等，将比较ms
		else
		{
			//	a的ms大于b的ms
			if (a_ms > b_ms)
			{
				return true;
			}
			//	a的ms小于b的ms
			else if (a_ms < b_ms)
			{
				return false;
			}
			//	否则为两个相等
			else
			{
				return false;
			}
		}
	}
	//	若为时间段，时间段为比较距离的长度
	else
	{
		//	首先进行格式整理
		this->maketime();
		intime.maketime();

		//	然后分别加上一个固定的时间
		CMyTime m;
		m.GetNowTime();
		CMyTime a(this);
		CMyTime b = intime;
		CMyTime ma = m + a;
		CMyTime mb = m + b;

		//	返回时间点的比较大小结果
		return ma>mb;
	}
}

//	小于运算重载
bool CMyTime::operator<(CMyTime intime)
{
	//	当不等于，且不大于的时候，即为小于
	CMyTime a(this);
	return (!(a==intime))&&(!(a>intime));
}

//	比较是否不相同
bool CMyTime::operator!=(CMyTime intime)
{
	CMyTime a(this);
	return !(a==intime);
}

//	大于等于运算重载
bool CMyTime::operator>=(CMyTime intime)
{
	CMyTime a(this);
	return (a>intime)||(a==intime);
}

//	小于等于运算重载
bool CMyTime::operator<=(CMyTime intime)
{
	CMyTime a(this);
	return (a<intime)||(a==intime);
}