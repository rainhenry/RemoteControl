
//	�汾 0.6

#include "stdafx.h"
#include "CMyTime.h"

//	���캯��
CMyTime::CMyTime()
{
	//	��ʼ��Ϊ0
	timedat.wDay = 0;
	timedat.wDayOfWeek = 0;
	timedat.wHour = 0;
	timedat.wMilliseconds = 0;
	timedat.wMinute = 0;
	timedat.wMonth = 0;
	timedat.wSecond = 0;
	timedat.wYear = 0;
	timedat.type = UNDEFINE;		//	����Ϊδ�����ʱ������

	timedat.Multiple = 1.0;
	timedat.Dividend = 1.0;
}

//	ָ��ʱ������ʵ���Ĺ��캯��
CMyTime::CMyTime(CMyTime::SISYSTEMTIME dat)
{
	this->timedat = dat;
}

//	ָ����ָ��Ĺ��캯��
CMyTime::CMyTime(CMyTime* p)
{
	//	���ָ���Ƿ���ã������õĻ��͹���һ���յ�
	if ((p == NULL)||(p == (CMyTime*)-1))
	{
		//	��ʼ��Ϊ0
		timedat.wDay = 0;
		timedat.wDayOfWeek = 0;
		timedat.wHour = 0;
		timedat.wMilliseconds = 0;
		timedat.wMinute = 0;
		timedat.wMonth = 0;
		timedat.wSecond = 0;
		timedat.wYear = 0;
		timedat.type = UNDEFINE;		//	����Ϊδ�����ʱ������

		timedat.Multiple = 1.0;
		timedat.Dividend = 1.0;
	}
	
	//	��ֵ
	this->timedat = p->timedat;
}

//	ָ��ʱ��Ĺ���
CMyTime::CMyTime(
	int year,		//	��
	int mon,		//	��
	int day,		//	��
	int hour,		//	Сʱ
	int min,		//	����
	int sec,		//	��
	int ms,			//	����
	ETimeType type	//	ָ����ʱ������
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

//	ָ��ʱ��εĹ��죬�Զ��屶��
CMyTime::CMyTime(
	int year,		//	��
	int mon,		//	��
	int day,		//	��
	int hour,		//	Сʱ
	int min,		//	����
	int sec,		//	��
	int ms,			//	����
	double mul,		//	����
	double div		//	����
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
	timedat.type = RANGE;			//	��ʱֻ��Ϊʱ���

	timedat.Multiple = mul;
	timedat.Dividend = div;

	maketime();
}

//	��������
CMyTime::~CMyTime()
{

}

//	���ʱ������
CMyTime::SISYSTEMTIME CMyTime::GetData(void)
{
	return this->timedat;
}

//	��ȡ��ǰʱ��
void CMyTime::GetNowTime(void)
{
	//	��ȡ����ʱ��
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

	//	�ж�ʱ�����ͣ����ڻ�ȡ֮���Ϊʱ�������
	timedat.type = POINT;			//	����Ϊʱ�������
}

//	���ַ�����ʽ���ʱ�����ݣ���ʽΪ��������Ϊstd::string
std::string CMyTime::GetTimeString(void)
{
	//	�����ʽ
	maketime();

	//	���ʱ�������ַ���
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
	//	�����ж�Ϊ��������
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
	else	//	this->timedat.type == RANGE��������ʾ��Ϊ��ԭʼ��Ҫ�����Բ����л����ת��
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

//	���ַ�����ʽ���ʱ�����ݣ���ʽΪ��������Ϊ����C���Ե��ַ���
char* CMyTime::GetTime_C_Str(void)
{
	//	���֮ǰ�Ľ��
	memset(re_str,0,sizeof(re_str));

	//	������ʱ�����ַ���
	std::string tmp_str = this->GetTimeString();

	//	����ڴ�����
	if (tmp_str.size() >= sizeof(re_str))
	{
		//	��ʾ�ַ���������������С
		strcpy_s(re_str,"String buffer too small!!");
		return re_str;
	}

	//	�����ַ���
	strcpy_s(re_str,tmp_str.data());

	//	�����ַ���
	return re_str;
}


//	��ʱ��淶��ʽ���ɹ��ⲿ����
CMyTime::SISYSTEMTIME CMyTime::maketime(
	CMyTime::SISYSTEMTIME intime				//	�����ʱ������
	)
{
	//	�����������
	int ms = intime.wMilliseconds % 1000;

	//	����ʱ�����ͽ��з��ദ����Ϊδ�������ͣ����ʼ��Ϊ0�����ؿ�ʱ��
	if (intime.type == UNDEFINE)
	{
		//	��ʼ��Ϊ0
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

		//	����
		return intime;
	}
	//	��Ϊʱ�������
	else if (intime.type == POINT)
	{
		//	����tm�ṹ
		struct tm mtm;
		time_t m_time_t;
		mtm.tm_year = intime.wYear - REF_BEGIN_YEAR;
		mtm.tm_mon = intime.wMonth - 1;
		mtm.tm_mday = intime.wDay;
		mtm.tm_isdst = 0;				//	����ʱ������
		mtm.tm_hour = intime.wHour;
		mtm.tm_min = intime.wMinute;
		mtm.tm_sec = intime.wSecond + intime.wMilliseconds/1000;

		//	���Ӷ�ms�ĸ�ֵ����
		if (ms<0)
		{
			mtm.tm_sec--;
		}

		//	��ʽ����
		m_time_t = mktime(&mtm);

		//	���췵������
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

		//	��ms��ֵ����
		if (ms<0)
		{
			mSISYSTEMTIME.wMilliseconds = 1000 + ms;
		}

		//	����
		return mSISYSTEMTIME;
	}
	//	��Ϊʱ�������
	else	//	intime.type == RANGE
	{
		/*
			���Ϊʱ��Σ��������ǲ���Ҫ����mktime�ģ���ΪΪ�˱�֤ʱ��εľ�ȷ�ԡ�
			��Ϊ30�첢������һ���¡�
			����Ϊ�˷�����㣬�ꡢ�¶������������졢ʱ���֡��붼ͳһ���ӵ�ms���棬����ͳһ���š�
		*/
		//	���졢ʱ���֡��롢���붼ת��Ϊ����
		LONGLONG Totalms = intime.wDay*24LL*60LL*60LL*1000LL + intime.wHour*60LL*60LL*1000LL + intime.wMinute*60LL*1000LL + intime.wSecond*1000LL + intime.wMilliseconds;

		//	������ת��Ϊ�졢ʱ���֡��롢����
		intime.wDay = static_cast<int>(Totalms/24LL/60LL/60LL/1000LL);
		intime.wHour = static_cast<int>((Totalms - intime.wDay*24LL*60LL*60LL*1000LL)/60LL/60LL/1000LL);
		intime.wMinute = static_cast<int>((Totalms - intime.wDay*24LL*60LL*60LL*1000LL - intime.wHour*60LL*60LL*1000LL)/60LL/1000LL);
		intime.wSecond = static_cast<int>((Totalms - intime.wDay*24LL*60LL*60LL*1000LL - intime.wHour*60LL*60LL*1000LL - intime.wMinute*60LL*1000LL)/1000LL);
		intime.wMilliseconds = static_cast<int>(Totalms - intime.wDay*24LL*60*60LL*1000LL - intime.wHour*60LL*60LL*1000LL - intime.wMinute*60LL*1000LL - intime.wSecond*1000LL);

		//	����
		return intime;
	}
}

//	�õ�ʱ���ĺ���ֵ����ʱ��Σ����޷������ʱ�򷵻�FAIL_SPANMILLSEC
LONGLONG CMyTime::GetSpanMillsec(void)
{
	//	���ʱ������
	if (this->timedat.type == RANGE)
	{
		//	�������µ�ֵ�����Ϊ�����Ծ�ȷ�����
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

//	��ʱ��淶��ʽ�������ڲ�����
void CMyTime::maketime(void)
{
	timedat = CMyTime::maketime(timedat);
}

//	���õ�ʱ��
void CMyTime::SetPointTime(
	int year,		//	��
	int mon,		//	��
	int day,		//	��
	int hour,		//	Сʱ
	int min,		//	����
	int sec,		//	��
	int ms			//	����
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

//	���ö�ʱ�䣬Ĭ��Ϊ1����
void CMyTime::SetRangeTime(
	int year,		//	��
	int mon,		//	��
	int day,		//	��
	int hour,		//	Сʱ
	int min,		//	����
	int sec,		//	��
	int ms			//	����
	)
{
	timedat.wDay = day;
	timedat.wHour = hour;
	timedat.wMilliseconds = ms;
	timedat.wMinute = min;
	timedat.wMonth = mon;
	timedat.wSecond = sec;
	timedat.wYear = year;
	timedat.wDayOfWeek = 0;		//	��ʱ�����治��������
	timedat.Multiple = 1.0;
	timedat.Dividend = 1.0;
	timedat.type = RANGE;		
	maketime();
}

//	���ö�ʱ�䣬�û��Զ��屶��
void CMyTime::SetRangeTime(
	int year,		//	��
	int mon,		//	��
	int day,		//	��
	int hour,		//	Сʱ
	int min,		//	����
	int sec,		//	��
	int ms,			//	����
	double mul,		//	����
	double div		//	����
	)
{
	timedat.wDay = day;
	timedat.wHour = hour;
	timedat.wMilliseconds = ms;
	timedat.wMinute = min;
	timedat.wMonth = mon;
	timedat.wSecond = sec;
	timedat.wYear = year;
	timedat.wDayOfWeek = 0;		//	��ʱ�����治��������
	timedat.Multiple = mul;
	timedat.Dividend = div;
	timedat.type = RANGE;		
	maketime();
}

//	ʱ��������ֵ
void CMyTime::Abs(void)
{
	//	�ж��Ƿ�Ϊʱ�������
	if (this->timedat.type == RANGE)
	{
		//	�����ʽ
		this->maketime();

		//	������½���������ȡ����ֵ
		int Total_mon = this->timedat.wYear*12 + this->timedat.wMonth;
		this->timedat.wYear = static_cast<int>(::abs(static_cast<double>(Total_mon/12)));
		this->timedat.wMonth = static_cast<int>(::abs(static_cast<double>(Total_mon - this->timedat.wYear*12)));

		//	�õ������µľ�ȷ��ʾ
		LONGLONG Totalms = this->timedat.wDay*24LL*60LL*60LL*1000LL + this->timedat.wHour*60LL*60LL*1000LL + this->timedat.wMinute*60LL*1000LL + this->timedat.wSecond*1000LL + this->timedat.wMilliseconds;
		
		//	ȡ�����µľ���ֵ
		//	�ж��Ƿ���С��0����
		if(this->timedat.wDay < 0) this->timedat.wDay = static_cast<int>(::abs(static_cast<double>(this->timedat.wDay)));
		if(this->timedat.wHour < 0) this->timedat.wHour = static_cast<int>(::abs(static_cast<double>(this->timedat.wHour)));
		if(this->timedat.wMinute < 0) this->timedat.wMinute = static_cast<int>(::abs(static_cast<double>(this->timedat.wMinute)));
		if(this->timedat.wSecond < 0) this->timedat.wSecond = static_cast<int>(::abs(static_cast<double>(this->timedat.wSecond)));
		if(this->timedat.wMilliseconds < 0) this->timedat.wMilliseconds = static_cast<int>(::abs(static_cast<double>(this->timedat.wMilliseconds)));

		//	�����Ƿ���б�ţ��������ͬ�ţ���������
		if (
			((Total_mon < 0)&&(Totalms < 0)) ||
			((Total_mon > 0)&&(Totalms > 0))
			)
		{
			//	������
		}
		//	������Ų�ͬ�����б��
		else
		{
			//	���
			this->timedat.wDay = 0-(this->timedat.wDay);
			this->timedat.wHour = 0-(this->timedat.wHour);
			this->timedat.wMinute = 0-(this->timedat.wMinute);
			this->timedat.wSecond = 0-(this->timedat.wSecond);
			this->timedat.wMilliseconds = 0-(this->timedat.wMilliseconds);
		}
	}
	//	��������
	else
	{
		//	�����κβ���
	}
}

//	ʱ������෴��
void CMyTime::Inv(void)
{
	//	�ж��Ƿ�Ϊʱ�������
	if (this->timedat.type == RANGE)
	{
		//	�����ʽ
		this->maketime();

		//	���
		this->timedat.wYear = 0-(this->timedat.wYear);
		this->timedat.wMonth = 0-(this->timedat.wMonth);
		this->timedat.wDay = 0-(this->timedat.wDay);
		this->timedat.wHour = 0-(this->timedat.wHour);
		this->timedat.wMinute = 0-(this->timedat.wMinute);
		this->timedat.wSecond = 0-(this->timedat.wSecond);
		this->timedat.wMilliseconds = 0-(this->timedat.wMilliseconds);
	}
	//	��������
	else
	{
		//	�����κβ���
	}
}

//	��ֵ��������
void CMyTime::operator=(CMyTime mcmytime)
{
	this->timedat = mcmytime.timedat;
}

//	�ӷ���������
CMyTime CMyTime::operator+(CMyTime mcmytime)
{
	//	�����������ͽ��зֱ������Ϊδ����
	if (this->timedat.type == UNDEFINE)
	{
		//	���ݴ���������ͽ��зֱ������Ϊδ����
		if (mcmytime.timedat.type == UNDEFINE)
		{
			//	����һ���յ�ʱ�����
			CMyTime m;
			return m;
		}
		//	�����������Ϊʱ���
		else if (mcmytime.timedat.type == POINT)
		{
			//	���ش������
			return mcmytime;
		}
		//	�����������Ϊʱ���
		else	//	mcmytime.timedat.type == RANGE
		{
			//	���ش������
			return mcmytime;
		}
	}
	//	��������Ϊʱ���
	else if (this->timedat.type == POINT)
	{
		//	���ݴ���������ͽ��зֱ������Ϊδ����
		if (mcmytime.timedat.type == UNDEFINE)
		{
			//	���ر������
			CMyTime m;
			m.timedat = this->timedat;
			return m;
		}
		//	�����������Ϊʱ���
		else if (mcmytime.timedat.type == POINT)
		{
			//	ͬʱΪʱ���Ϊ������������ؿյ�ʱ��
//	�ַ�����������
#ifdef UNICODE
			AfxMessageBox(L"��������ʱ��㷢����ӣ�");
#else
			AfxMessageBox("��������ʱ��㷢����ӣ�");
#endif
			CMyTime m;
			return m;
		}
		//	�����������Ϊʱ���
		else	//	mcmytime.timedat.type == RANGE
		{
			//	ִ�е��Ӽ���
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

			//	��ȡһ��������������ʱ����ֵ-��ֵ
			CMyTime span = m - CMyTime(this);

			//	���졢ʱ���֡��롢���붼ת��Ϊ����
			LONGLONG Totalms = 
				span.timedat.wDay*24LL*60LL*60LL*1000LL + 
				span.timedat.wHour*60LL*60LL*1000LL + 				
				span.timedat.wMinute*60LL*1000LL + 
				span.timedat.wSecond*1000LL + 
				span.timedat.wMilliseconds;

			//	�ú��������б�������
			Totalms = static_cast<LONGLONG>(Totalms * mcmytime.timedat.Multiple / mcmytime.timedat.Dividend);

			//	������ת��Ϊ�졢ʱ���֡��롢���룬��ʱspanΪ�µ�ʱ���
			span.timedat.wDay = static_cast<int>(Totalms/24LL/60LL/60LL/1000LL);
			span.timedat.wHour = static_cast<int>((Totalms - span.timedat.wDay*24LL*60LL*60LL*1000LL)/60LL/60LL/1000LL);
			span.timedat.wMinute = static_cast<int>((Totalms - span.timedat.wDay*24LL*60LL*60LL*1000LL - span.timedat.wHour*60LL*60LL*1000LL)/60LL/1000LL);
			span.timedat.wSecond = static_cast<int>((Totalms - span.timedat.wDay*24LL*60LL*60LL*1000LL - span.timedat.wHour*60LL*60LL*1000LL - span.timedat.wMinute*60LL*1000LL)/1000LL);
			span.timedat.wMilliseconds = static_cast<int>(Totalms - span.timedat.wDay*24LL*60*60LL*1000LL - span.timedat.wHour*60LL*60LL*1000LL - span.timedat.wMinute*60LL*1000LL - span.timedat.wSecond*1000LL);

			//	��������µ�ʱ���
			m.timedat.wYear = this->timedat.wYear;
			m.timedat.wMonth = this->timedat.wMonth;
			m.timedat.wDay = this->timedat.wDay						+	span.timedat.wDay;
			m.timedat.wHour = this->timedat.wHour					+	span.timedat.wHour;
			m.timedat.wMinute = this->timedat.wMinute				+	span.timedat.wMinute;
			m.timedat.wSecond = this->timedat.wSecond				+	span.timedat.wSecond;
			m.timedat.wMilliseconds = this->timedat.wMilliseconds	+	span.timedat.wMilliseconds;
			m.timedat.type = POINT;

			//	�����ʽ
			m.maketime();

			//	���ص��ӽ��
			return m;
		}
	}
	//	��������Ϊʱ���
	else	//	this->timedat.type == RANGE
	{
		//	���ݴ���������ͽ��зֱ������Ϊδ����
		if (mcmytime.timedat.type == UNDEFINE)
		{
			//	���ر������
			CMyTime m;
			m.timedat = this->timedat;
			return m;
		}
		//	�����������Ϊʱ���
		else if (mcmytime.timedat.type == POINT)
		{
			//	ִ�е��Ӽ���
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

			//	��ȡһ��������������ʱ����ֵ-��ֵ
			CMyTime span = m - mcmytime;

			//	���졢ʱ���֡��롢���붼ת��Ϊ����
			LONGLONG Totalms = 
				span.timedat.wDay*24LL*60LL*60LL*1000LL + 
				span.timedat.wHour*60LL*60LL*1000LL + 				
				span.timedat.wMinute*60LL*1000LL + 
				span.timedat.wSecond*1000LL + 
				span.timedat.wMilliseconds;

			//	�ú��������б�������
			Totalms = static_cast<LONGLONG>(Totalms * this->timedat.Multiple / this->timedat.Dividend);

			//	������ת��Ϊ�졢ʱ���֡��롢���룬��ʱspanΪ�µ�ʱ���
			span.timedat.wDay = static_cast<int>(Totalms/24LL/60LL/60LL/1000LL);
			span.timedat.wHour = static_cast<int>((Totalms - span.timedat.wDay*24LL*60LL*60LL*1000LL)/60LL/60LL/1000LL);
			span.timedat.wMinute = static_cast<int>((Totalms - span.timedat.wDay*24LL*60LL*60LL*1000LL - span.timedat.wHour*60LL*60LL*1000LL)/60LL/1000LL);
			span.timedat.wSecond = static_cast<int>((Totalms - span.timedat.wDay*24LL*60LL*60LL*1000LL - span.timedat.wHour*60LL*60LL*1000LL - span.timedat.wMinute*60LL*1000LL)/1000LL);
			span.timedat.wMilliseconds = static_cast<int>(Totalms - span.timedat.wDay*24LL*60*60LL*1000LL - span.timedat.wHour*60LL*60LL*1000LL - span.timedat.wMinute*60LL*1000LL - span.timedat.wSecond*1000LL);

			//	��������µ�ʱ���
			m.timedat.wYear = mcmytime.timedat.wYear;
			m.timedat.wMonth = mcmytime.timedat.wMonth;
			m.timedat.wDay = mcmytime.timedat.wDay						+	span.timedat.wDay;
			m.timedat.wHour = mcmytime.timedat.wHour					+	span.timedat.wHour;
			m.timedat.wMinute = mcmytime.timedat.wMinute				+	span.timedat.wMinute;
			m.timedat.wSecond = mcmytime.timedat.wSecond				+	span.timedat.wSecond;
			m.timedat.wMilliseconds = mcmytime.timedat.wMilliseconds	+	span.timedat.wMilliseconds;
			m.timedat.type = POINT;

			//	�����ʽ
			m.maketime();

			//	���ص��ӽ��
			return m;
		}
		//	�����������Ϊʱ���
		else	//	mcmytime.timedat.type == RANGE
		{
			//	���������ͬ
			if ((this->timedat.Multiple == mcmytime.timedat.Multiple)&&(this->timedat.Dividend == mcmytime.timedat.Dividend))
			{
				//	ִ�е��Ӽ���
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
			//	��������ͬ��˫�����¶�Ϊ0�Ļ�
			else if ((this->timedat.wYear == 0)&&(this->timedat.wMonth == 0)&&(mcmytime.timedat.wYear == 0)&&(mcmytime.timedat.wMonth == 0))
			{
				//	��this���졢ʱ���֡��롢���붼ת��Ϊ����
				LONGLONG Totalms_a = this->timedat.wDay*24LL*60LL*60LL*1000LL + this->timedat.wHour*60LL*60LL*1000LL + this->timedat.wMinute*60LL*1000LL + this->timedat.wSecond*1000LL + this->timedat.wMilliseconds;
				Totalms_a = static_cast<LONGLONG>(Totalms_a * this->timedat.Multiple / this->timedat.Dividend);

				//	������������졢ʱ���֡��롢���붼ת��Ϊ����
				LONGLONG Totalms_b = mcmytime.timedat.wDay*24LL*60LL*60LL*1000LL + mcmytime.timedat.wHour*60LL*60LL*1000LL + mcmytime.timedat.wMinute*60LL*1000LL + mcmytime.timedat.wSecond*1000LL + mcmytime.timedat.wMilliseconds;
				Totalms_b = static_cast<LONGLONG>(Totalms_b * mcmytime.timedat.Multiple / mcmytime.timedat.Dividend);

				//	����
				LONGLONG Totalms = Totalms_a + Totalms_b;

				//	������ת��Ϊ�졢ʱ���֡��롢����
				CMyTime m;
				m.timedat.wDay = static_cast<int>(Totalms/24LL/60LL/60LL/1000LL);
				m.timedat.wHour = static_cast<int>((Totalms - m.timedat.wDay*24LL*60LL*60LL*1000LL)/60LL/60LL/1000LL);
				m.timedat.wMinute = static_cast<int>((Totalms - m.timedat.wDay*24LL*60LL*60LL*1000LL - m.timedat.wHour*60LL*60LL*1000LL)/60LL/1000LL);
				m.timedat.wSecond = static_cast<int>((Totalms - m.timedat.wDay*24LL*60LL*60LL*1000LL - m.timedat.wHour*60LL*60LL*1000LL - m.timedat.wMinute*60LL*1000LL)/1000LL);
				m.timedat.wMilliseconds = static_cast<int>(Totalms - m.timedat.wDay*24LL*60*60LL*1000LL - m.timedat.wHour*60LL*60LL*1000LL - m.timedat.wMinute*60LL*1000LL - m.timedat.wSecond*1000LL);
				m.timedat.type = RANGE;

				//	����
				return m;
			}
			//	���ܾ�ȷ����
			else
			{
				//	���ؿյ�ʱ���
				CMyTime m(0,0,0,0,0,0,0,RANGE);
				return m;
			}
		}

	}
}

//	������������
CMyTime CMyTime::operator-(CMyTime mcmytime)
{
	//	������ʱ����
	CMyTime a,b;

	//	��Ÿ�ֵ�����������Ϊʱ���
	if (mcmytime.timedat.type == RANGE)
	{
		//	��ֵ�������
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

		//	��ֵ�������
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

		//	�������
		CMyTime re;
		re = a+b;

		//	��������
		return re;
	}
	//	������Ϊʱ���
	else if (this->timedat.type == RANGE)
	{
		//	��ֵ�������
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

		//	��ֵ�������
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

		//	�������
		CMyTime re;
		re = a+b;

		//	��������
		return re;
	}
	//	����Ϊʱ����ʱ�������ʱ���
	else if ((mcmytime.timedat.type == POINT)&&(this->timedat.type == POINT))
	{
		//	���ȷֱ�Զ��߽��и�ʽ������Ҫ��ֹms��������Χ
		mcmytime.maketime();
		this->maketime();

		//	�ֱ�ת��Ϊ���ms��ʱ��ֵ
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
		
		//	�õ���һ����С�����
		time_t max_s,min_s;
		int max_ms,min_ms;
		//	��a����������b����������ʱΪa-b�Ĳ�����a>b���������
		if (a_s > b_s)
		{
			//	���������Сֵ
			max_s = a_s;
			max_ms = a_ms;
			min_s = b_s;
			min_ms = b_ms;

			//	�����
			double diff_s = difftime(max_s,min_s);
			int diff_ms = max_ms - min_ms;
			
			//	���췵�ص�ʱ��Σ������ꡢ��Ϊ������ɣ����Լ��㾫�Ƚ������������¡�
			int day,hour,min,sec,ms;
			day = static_cast<int>(diff_s/60/60/24);
			hour = static_cast<int>((diff_s - day*24*60*60)/60/60);
			min = static_cast<int>((diff_s - day*24*60*60 - hour*60*60)/60);
			sec = static_cast<int>((diff_s - day*24*60*60 - hour*60*60 - min*60));
			ms = max_ms - min_ms;
			CMyTime re(0,0,day,hour,min,sec,ms,RANGE);
			re.maketime();
			//re.Abs();	//	���ڲ���difftime�������㣬���Խ��һ��Ϊ��ֵ������ʡ�Ը����
			return re;
		}
		//	��a������С��b����������ʱΪa-b�Ĳ�����a<b������ظ�
		else if (a_s < b_s)
		{
			//	���������Сֵ
			max_s = b_s;
			max_ms = b_ms;
			min_s = a_s;
			min_ms = a_ms;

			//	�����
			double diff_s = difftime(max_s,min_s);
			int diff_ms = max_ms - min_ms;

			//	���췵�ص�ʱ��Σ������ꡢ��Ϊ������ɣ����Լ��㾫�Ƚ������������¡�
			int day,hour,min,sec,ms;
			day = static_cast<int>(diff_s/60/60/24);
			hour = static_cast<int>((diff_s - day*24*60*60)/60/60);
			min = static_cast<int>((diff_s - day*24*60*60 - hour*60*60)/60);
			sec = static_cast<int>((diff_s - day*24*60*60 - hour*60*60 - min*60));
			ms = max_ms - min_ms;
			CMyTime re(0,0,day,hour,min,sec,ms,RANGE);
			re.maketime();
			re.Inv();		//	ȡ�෴��
			return re;
		}
		//	����������ȣ����Ƚ�ms
		else
		{
			//	���ﲻ�жϴ�С��ֱ�ӷ��ش���ֵ
			CMyTime m(0,0,0,0,0,0,a_ms-b_ms,RANGE);
			return m;
		}
	}
	//	û��ʱ�������
	else
	{
		//	����һ���յ�ʱ�����
		CMyTime m;
		return m;
	}
}


//	�ɷ���������
CMyTime CMyTime::operator*(double b)
{
	//	ʱ��ĳ˷�����ֻ�ܿ���ʱ��Σ�δ��������ͷ���δ����
	if (this->timedat.type == UNDEFINE)
	{
		//	����һ���յ�δ����ʱ������
		CMyTime m;
		return m;
	}
	//	����ʱ��㲻�ܲ������㣬���Դ�ʱ��������
	else if (this->timedat.type == POINT)
	{
		//	���ر������
		CMyTime m;
		m.timedat = this->timedat;
		return m;
	}
	//	���һ�־��Ƿ���ʱ��εĳ˷����
	else
	{
		/*
			����Ϊ�˾�ȷ�ı�ʾʱ��ͼ���ʱ�䣬�˳��������ݽ������㵽�������棬�����ڱ���������м���
		*/
		//	���bΪ0����ô˵��Ϊ�յ�ʱ���
		if (b == 0.0)
		{
			//	���ؿյ�ʱ���
			CMyTime m(0,0,0,0,0,0,0,RANGE);
			return m;
		}
		//	��Ϊ0������㵽��������
		else
		{
			//	����
			CMyTime m(this);
			m.timedat.Multiple *= b;
			return m;
		}

	}
}

//	������������
CMyTime CMyTime::operator/(double b)
{
	//	ʱ��ĳ�������ֻ�ܿ���ʱ��Σ�δ��������ͷ���δ����
	if (this->timedat.type == UNDEFINE)
	{
		//	����һ���յ�δ����ʱ������
		CMyTime m;
		return m;
	}
	//	����ʱ��㲻�ܲ������㣬���Դ�ʱ��������
	else if (this->timedat.type == POINT)
	{
		//	���ر������
		CMyTime m;
		m.timedat = this->timedat;
		return m;
	}
	//	���һ�־��Ƿ���ʱ��εĳ������
	else
	{
		/*
			����Ϊ�˾�ȷ�ı�ʾʱ��ͼ���ʱ�䣬�����������ݽ������㵽�������棬�����ڱ���������м���
		*/
		//	���bΪ0����ô˵��Ϊ�յ�ʱ���
		if (b == 0.0)
		{
			//	���ؿյ�ʱ���
			CMyTime m(0,0,0,0,0,0,0,RANGE);
			return m;
		}
		//	��Ϊ0������㵽��������
		else
		{
			//	����
			CMyTime m(this);
			m.timedat.Dividend *= b;
			return m;
		}

	}
}

//	�Ƚ��Ƿ���ͬ
bool CMyTime::operator==(CMyTime intime)
{
	//	������������Ͳ�ƥ����ֱ�ӷ��ز����
	if (this->timedat.type != intime.timedat.type)
	{
		return false;
	}

	//	�ж����ͽ��д�����Ϊδ����
	if (this->timedat.type == UNDEFINE)
	{
		return true;
	}
	//	��Ϊ��ʱ��
	else if (this->timedat.type == POINT)
	{
		//	���Ƚ��и�ʽ����
		this->maketime();
		intime.maketime();

		//	Ȼ��Ƚ��Ƿ��в�ͬ
		if(this->timedat.wYear != intime.timedat.wYear)			return false;
		if(this->timedat.wMonth != intime.timedat.wMonth)			return false;
		if(this->timedat.wDay != intime.timedat.wDay)			return false;
		if(this->timedat.wHour != intime.timedat.wHour)			return false;
		if(this->timedat.wMinute != intime.timedat.wMinute)			return false;
		if(this->timedat.wSecond != intime.timedat.wSecond)			return false;
		if(this->timedat.wMilliseconds != intime.timedat.wMilliseconds)			return false;
		return true;
	}
	//	��Ϊʱ���
	else
	{
		//	���Ƚ��и�ʽ����
		this->maketime();
		intime.maketime();

		//	Ȼ��Ƚ��Ƿ��в�ͬ
		if(this->timedat.wYear != intime.timedat.wYear)			return false;
		if(this->timedat.wMonth != intime.timedat.wMonth)			return false;
		if(this->timedat.wDay != intime.timedat.wDay)			return false;
		if(this->timedat.wHour != intime.timedat.wHour)			return false;
		if(this->timedat.wMinute != intime.timedat.wMinute)			return false;
		if(this->timedat.wSecond != intime.timedat.wSecond)			return false;
		if(this->timedat.wMilliseconds != intime.timedat.wMilliseconds)			return false;
		if(this->timedat.Dividend != intime.timedat.Dividend)			return false;		//	��Ϊʱ��ε�ʱ����Ҫ���бȽϱ���
		if(this->timedat.Multiple != intime.timedat.Multiple)			return false;		
		return true;
	}
}


//	������������
bool CMyTime::operator>(CMyTime intime)
{
	//	������������Ͳ�ƥ����ֱ�ӷ��ز����
	if (this->timedat.type != intime.timedat.type)
	{
		return false;
	}

	//	�ж����ͽ��д�����Ϊδ����
	if (this->timedat.type == UNDEFINE)
	{
		return false;
	}
	//	��Ϊ��ʱ��
	else if (this->timedat.type == POINT)
	{
		//	���Ƚ��и�ʽ����
		this->maketime();
		intime.maketime();

		//	�ֱ�ת��Ϊ���ms��ʱ��ֵ
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

		//	��a����������b������
		if (a_s > b_s)
		{
			return true;
		}
		//	��a������С��b������
		else if (a_s < b_s)
		{
			return false;
		}
		//	����������ȣ����Ƚ�ms
		else
		{
			//	a��ms����b��ms
			if (a_ms > b_ms)
			{
				return true;
			}
			//	a��msС��b��ms
			else if (a_ms < b_ms)
			{
				return false;
			}
			//	����Ϊ�������
			else
			{
				return false;
			}
		}
	}
	//	��Ϊʱ��Σ�ʱ���Ϊ�ȽϾ���ĳ���
	else
	{
		//	���Ƚ��и�ʽ����
		this->maketime();
		intime.maketime();

		//	Ȼ��ֱ����һ���̶���ʱ��
		CMyTime m;
		m.GetNowTime();
		CMyTime a(this);
		CMyTime b = intime;
		CMyTime ma = m + a;
		CMyTime mb = m + b;

		//	����ʱ���ıȽϴ�С���
		return ma>mb;
	}
}

//	С����������
bool CMyTime::operator<(CMyTime intime)
{
	//	�������ڣ��Ҳ����ڵ�ʱ�򣬼�ΪС��
	CMyTime a(this);
	return (!(a==intime))&&(!(a>intime));
}

//	�Ƚ��Ƿ���ͬ
bool CMyTime::operator!=(CMyTime intime)
{
	CMyTime a(this);
	return !(a==intime);
}

//	���ڵ�����������
bool CMyTime::operator>=(CMyTime intime)
{
	CMyTime a(this);
	return (a>intime)||(a==intime);
}

//	С�ڵ�����������
bool CMyTime::operator<=(CMyTime intime)
{
	CMyTime a(this);
	return (a<intime)||(a==intime);
}