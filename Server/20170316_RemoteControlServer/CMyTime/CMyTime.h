/************************************************************************

	�������ƣ��Զ���ʱ����ز��������
	�������ڣ�2016.12.31
	����޸ģ�2017.3.26
	��    ����0.6
	ƽ    ̨��VS2010 Win7 64bit SP1
	��Ʊ��룺rainhenry

	�汾�޶�
		0.2�汾		���Ӷ���ʼ��ݵļ������ã�REF_BEGIN_YEAR�꣬��VS2010������Ϊ1900����ʼ
		0.3�汾		�޸��˱Ƚ�ʱ����BUG
					�޸��˳������ӵ�BUG
					�޸��˲�ͬ����ʱ��ε��ӵ�BUG
		0.4�汾		��������C��ʽ�ַ������ʱ���ַ�������GetTime_C_Str
		0.5�汾		�޸�strcpy��sprintfΪ��ȫ����strcpy_s��sprintf_s
		0.6�汾		����GetSpanMillsec�������õ�ʱ�����룬������չ����ͳ��

	��������
		����������������ҵ��;������ѧϰ�ο���

	���Ŀ�ĺ�Ҫ��
	1����ȡ������ǰʱ�䣬��ȷ���롣
	2��ʱ���������յ���׼��ʽ��ת���� ����1230423���ӱ�ʾΪ��׼��ʽ����3����2��23Сʱ44��0��ת��Ϊ���ӵȡ�
	3�����Ի�ȡ��ǰʱ��㡣���Զ��ֱ���뼶��ȷ��ȡ�����뼶�Ļ�ȡ���뼶��Ϊȫ������ʽ��
	4������ʱ��������Լ���ʱ�
	5�����ԱȽ�ʱ����С��

	���˼·��
	1�����ʱ��ģʽ
	2������ʱ��ģʽ

	ע�����
	1��GetTimeString�������Ϊstd::string���ͣ����Բ��ܵ�ϵͳ���ַ������õ�Ӱ�졣�����Ϊ���ֽ��ַ�����GB2312���롣
	
	ʹ��˵��
	//////////////////////////////////////////////////////////////////////////
	//	����ʱ���ȡ����
	int count = 0;
	CMyTime a;
	a.GetNowTime();
	printf("%d    %s\r\n",++count,a.GetTimeString().data());	//================1

	//------------------------------------------------------------------------
	//	ʱ��ӷ�����������
	CMyTime b = CMyTime(2016,3,10,0,0,0,0,CMyTime::POINT) + CMyTime(0,0,0,0,0,0,-1,CMyTime::RANGE) - CMyTime(0,0,0,0,0,0,1,CMyTime::RANGE);
	printf("%d    %s\r\n",++count,b.GetTimeString().data());	//================2
	b = CMyTime(0,0,0,0,0,0,-1,CMyTime::RANGE) + CMyTime(2016,3,10,0,0,0,0,CMyTime::POINT);
	printf("%d    %s\r\n",++count,b.GetTimeString().data());	//================3

	//------------------------------------------------------------------------
	//	�����ʽ��ʱ��β���
	CMyTime c(0,-2,-15,10,-50,24,-345,CMyTime::RANGE);
	//c.SetRangeTime(2,3,1,73,50,24,345);
	//c.SetRangeTime(2,3,1,2,3,4,345);
	printf("%d    %s\r\n",++count,c.GetTimeString().data());	//================4

	//------------------------------------------------------------------------
	//	ʱ������
	CMyTime d = CMyTime(2014,4,21,4,0,0,50,CMyTime::POINT) - CMyTime(2015,4,21,0,0,0,0,CMyTime::POINT);
	printf("%d    %s\r\n",++count,d.GetTimeString().data());	//================5

	//------------------------------------------------------------------------
	//	�˷�����
	CMyTime e = CMyTime(10,2,3,10,2,1,10,CMyTime::RANGE) * 2;
	printf("%d    %s\r\n",++count,e.GetTimeString().data());	//================6

	//------------------------------------------------------------------------
	//	С���²���
	CMyTime f = CMyTime(2014,2,1,0,0,0,0,CMyTime::POINT) + CMyTime(0,1,0,0,0,0,0,0.5,1.0);
	printf("%d    %s\r\n",++count,f.GetTimeString().data());	//================7

	//------------------------------------------------------------------------
	//	ʱ�����Ȳ���
	printf("%d    %d\r\n",++count,CMyTime(2015,5,9,0,0,0,0,CMyTime::POINT)==CMyTime(2015,5,10,0,0,0,0,CMyTime::POINT));	//================8

	//------------------------------------------------------------------------
	//	ʱ�����Ȳ���
	printf("%d    %d\r\n",++count,CMyTime(0,5,11,0,0,0,0,CMyTime::RANGE)==CMyTime(0,5,11,0,0,0,0,CMyTime::RANGE));		//================9

	//------------------------------------------------------------------------
	//	ʱ�����ڲ���
	printf("%d    %d\r\n",++count,CMyTime(2015,5,10,0,0,0,0,CMyTime::POINT)>CMyTime(2015,5,10,0,0,0,0,CMyTime::POINT));	//================10

	//------------------------------------------------------------------------
	//	ʱ��δ��ڲ���
	printf("%d    %d\r\n",++count,CMyTime(0,7,5,0,0,0,0,CMyTime::RANGE)>CMyTime(0,5,10,0,0,0,0,CMyTime::RANGE));		//================11

	//------------------------------------------------------------------------
	//	ʱ���С�ڵ��ڲ���
	CMyTime g;
	g.GetNowTime();
	printf("%d    %d\r\n",++count,g<=CMyTime(2017,1,4,0,0,0,0,CMyTime::POINT));											//================12

	//	�������н��
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
//	����ͷ�ļ�
#ifndef __CMYTIME_H__
#define __CMYTIME_H__

//////////////////////////////////////////////////////////////////////////
//	MFC���ݴ���
#include <afx.h>
#include <afxwin.h>
#include <iostream>
#include <string>

//////////////////////////////////////////////////////////////////////////
//	����ͷ�ļ�
#define	REF_BEGIN_YEAR	1900
#define FAIL_SPANMILLSEC		(-1LL)

//////////////////////////////////////////////////////////////////////////
//	�ඨ��
class CMyTime
{
public:
	//	ʱ�����Ͷ���
	typedef enum tagETimeType
	{
		UNDEFINE,			//	δ�����ʱ������
		POINT,				//	ʱ�������
		RANGE				//	ʱ�������
	}ETimeType;

	//	�洢��ʽ���壬����ϵͳԭ����SYSTEMTIME�ڲ�����������Ϊunsigned short���޷���ʾ��ֵ�����ҷ�ΧҲ������
	typedef struct tagSISYSTEMTIME {
		int wYear;			//	�޷���ʾREF_BEGIN_YEAR��֮ǰ��ʱ�䣬��ʱ2017�꽫��Ӧ2017ֵ
		int wMonth;			//	1-31
		int wDayOfWeek;		//	0-6		0=�����գ�6=������
		int wDay;			//	1-31
		int wHour;			//	0-23
		int wMinute;		//	0-59
		int wSecond;		//	0-59
		int wMilliseconds;	//	0-999	
		ETimeType type;		//	��ǰ�����ͣ�δ���壬ʱ��㣬ʱ���/�Σ�
		double Multiple;	//	��Ϊʱ���ʱ���Ի������зŴ�ı������൱�ڳ˷�
		double Dividend;	//	��Ϊʱ���ʱ���Ի���������С�ı������൱�ڳ���
	} SISYSTEMTIME;

public:
	//	�޲����Ĺ���
	CMyTime();

	//	ָ����ָ��Ĺ��캯��
	CMyTime(CMyTime* p);

	//	ָ��ʱ������ʵ���Ĺ��캯��
	CMyTime(SISYSTEMTIME dat);

	//	ָ��ʱ��Ĺ��죬Ĭ��1����
	CMyTime(
		int year,		//	��
		int mon,		//	��
		int day,		//	��
		int hour,		//	Сʱ
		int min,		//	����
		int sec,		//	��
		int ms,			//	����
		ETimeType type	//	ָ����ʱ������
		);

	//	ָ��ʱ��εĹ��죬�Զ��屶��
	CMyTime(
		int year,		//	��
		int mon,		//	��
		int day,		//	��
		int hour,		//	Сʱ
		int min,		//	����
		int sec,		//	��
		int ms,			//	����
		double mul,		//	����
		double div		//	����
		);

	//	��������
	~CMyTime();

	//	��ȡ��ǰʱ��
	void GetNowTime(void);

	//	���ַ�����ʽ���ʱ�����ݣ���ʽΪ��������Ϊstd::string
	std::string GetTimeString(void);

	//	���ַ�����ʽ���ʱ�����ݣ���ʽΪ��������Ϊ����C���Ե��ַ���
	char* GetTime_C_Str(void);

	//	��ʱ��淶��ʽ���ɹ��ⲿ����
	static SISYSTEMTIME maketime(
		SISYSTEMTIME intime				//	�����ʱ������
		);

	//	���õ�ʱ��
	void SetPointTime(
		int year,		//	��
		int mon,		//	��
		int day,		//	��
		int hour,		//	Сʱ
		int min,		//	����
		int sec,		//	��
		int ms			//	����
		);

	//	���ö�ʱ�䣬Ĭ��Ϊ1����
	void SetRangeTime(
		int year,		//	��
		int mon,		//	��
		int day,		//	��
		int hour,		//	Сʱ
		int min,		//	����
		int sec,		//	��
		int ms			//	����
		);

	//	���ö�ʱ�䣬�û��Զ��屶��
	void SetRangeTime(
		int year,		//	��
		int mon,		//	��
		int day,		//	��
		int hour,		//	Сʱ
		int min,		//	����
		int sec,		//	��
		int ms,			//	����
		double mul,		//	����
		double div		//	����
		);

	//	���ʱ������
	SISYSTEMTIME GetData(void);

	//	ʱ��������ֵ
	void Abs(void);

	//	ʱ������෴��
	void Inv(void);

	//	��ֵ��������
	void operator=(CMyTime mcmytime);
	
	//	�ӷ���������
	CMyTime operator+(CMyTime mcmytime);

	//	������������
	CMyTime operator-(CMyTime mcmytime);

	//	�ɷ���������
	CMyTime operator*(double b);

	//	������������
	CMyTime operator/(double b);

	//	�Ƚ��Ƿ���ͬ
	bool operator==(CMyTime intime);
	
	//	������������
	bool operator>(CMyTime intime);
	
	//	С����������
	bool operator<(CMyTime intime);

	//	�Ƚ��Ƿ���ͬ
	bool operator!=(CMyTime intime);
	
	//	���ڵ�����������
	bool operator>=(CMyTime intime);
	
	//	С�ڵ�����������
	bool operator<=(CMyTime intime);

	//	�õ�ʱ���ĺ���ֵ����ʱ��Σ����޷������ʱ�򷵻�FAIL_SPANMILLSEC
	LONGLONG GetSpanMillsec(void);

private:
	//	�����ʱ������
	SISYSTEMTIME timedat;		

	//	��ʱ��淶��ʽ�������ڲ�����
	void maketime(void);

	//	���巵���ַ���
	char re_str[200];

};

//////////////////////////////////////////////////////////////////////////
#endif	//	__CMYTIME_H__

