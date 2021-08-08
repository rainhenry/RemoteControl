#ifndef __CHAR_CONVERT_H__  
#define __CHAR_CONVERT_H__  

/*
//--------------------------------------------------------------------------------------------------
// 用法：
std::cout << CWCharToChar(src, E_CHAR, E_WCHAR).WChar();
std::cout << CWCharToChar(src, E_CHAR, E_UTF8).UTF8();
std::cout << CWCharToChar(src, E_UTF8, E_WCHAR).WChar();
std::cout << CWCharToChar(src, E_UTF8, E_CHAR).Char();

std::cout << CWCharToChar(wsrc, E_CHAR).Char();
std::cout << CWCharToChar(wsrc, E_UTF8).UTF8();

原文出处：http://blog.csdn.net/sailing0123/article/details/7576014

测试：rainhenry
测试日期：20170220

//--------------------------------------------------------------------------------------------------
20170308
	增加
		UTF8_To_UTF16
		UTF16_To_UTF8
		GBK_To_UTF16
		UTF16_To_GBK
		GBK_To_UTF8
		UTF8_To_GBK
		WChar_To_Char
		Char_To_WChar
	静态成员函数，可以实现几乎常用的字符编码转换
	参考：
	http://blog.csdn.net/charlessimonyi/article/details/8722859
	http://blog.csdn.net/baliguan163/article/details/10324209

	说明
		在wchar中 utf16的wchar 和 unicode的wchar 和 default的wchar 观察内存的值都是完全一样的
		在char中  utf8的char 与 ansi的char 是不同的（区别主要在汉字）
		          ansi的char 和 gbk的char 和 gb2312的char 观察内存都是相同的

	内存分析举例：
	Input Txt Length = 7 bytes,CAP:
	123测试45
	default wchar_t In Hex(at wchar_t of 2 bytes):
		0031 0032 0033 6D4B 8BD5 0034 0035
	gbk char In Hex(at gbk char of 1 bytes):
		31 32 33 B2 E2 CA D4 34 35
	utf8 char In Hex(at utf8 char of 1 bytes):
		31 32 33 E6 B5 8B E8 AF 95 34 35
	utf16 wchar_t In Hex(at utf16 wchar_t of 2 bytes):
		0031 0032 0033 6D4B 8BD5 0034 0035


	使用举例

	//	进行编码转换，从utf8 char 到 ansi char （tmp_str可以为char* 或者 std::string）
	std::string ansi_char_str = CWCharToChar::UTF8_To_GBK(tmp_str);

	//	从ansi char 到 default wchar
	std::wstring unicode_wchar_str = CWCharToChar::Char_To_WChar(ansi_char_str);

//--------------------------------------------------------------------------------------------------
*/

//	获取UTF8字符串的长度
#include <string>


//	在windows下需要加的定义
#ifndef OS_WINDOWS
#define OS_WINDOWS
#endif	//	OS_WINDOWS
#define _UTF8_

//	以下为原作者的代码
#ifdef OS_WINDOWS  

#include <SDKDDKVer.h> //"targetver.h"  

#ifndef WIN32_LEAN_AND_MEAN  
#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息  
#endif  

// Windows 头文件:  
#include <windows.h>  

#else // Linux  
#include "iconv.h"  

#ifndef ICONV_BUFFER_SIZE  
#define ICONV_BUFFER_SIZE 1024  
#endif  

#define WCHAR wchar_t  
#ifdef _UNICODE  
#define TCHAR wchar_t  
#else  
#define TCHAR char  
#endif  

#endif  
/////////////////////////////  
#define E_CHAR      CWCharToChar::EChar  
#define E_WCHAR     CWCharToChar::EWChar  
#define E_UTF8      CWCharToChar::EUtf8  

#define NULL_STR    CWCharToChar::GetNullStr()  

static char g_NULL[2] = { 0 };

/////////////////////////////  
// 简单的wchar_t 和 char 转换类, 且包含与UTF8的转换  
class CWCharToChar
{
public:
	enum
	{
		EChar = 1,
		EWChar = 2,
		EUtf8 = 4
	};

private:
	char    *   m_cDest;
	wchar_t *   m_wcDest;
	char    *   m_cUtf8;

	unsigned int m_nSrcType;

	//	定义三种长度数值
	int ansi_char_len;
	int utf8_char_len;
	int wchar_len;

public:
	char    *   Char(void) { return m_cDest; }
	wchar_t *   WChar(void){ return m_wcDest; }

	bool        IsNullChar(void)  { return (!m_cDest || g_NULL == m_cDest); }
	bool        IsNullWChar(void) { return (!m_wcDest || g_NULL == (char*)m_wcDest); }

#if defined( _UTF8_ )  
	char    *   UTF8(void) { return m_cUtf8; }
	bool        IsNullUTF8(void) { return (!m_cUtf8 || g_NULL == m_cUtf8); }
#endif  

	TCHAR   *   TChar(void)
	{
#ifdef _UNICODE  
		return WChar();
#else  
		return Char();
#endif  
	}
	////////////////////////////////////////////////////////////////////////  
	//	获取字符串长度
public:
	//	获得char ansi字节数
	int GetANSICharSize(void)
	{
		return ansi_char_len;
	}

	//	获得char utf8字节数
	int GetUTF8CharSize(void)
	{
		return utf8_char_len;
	}

	//	获得wchar数目（不是字节数目）
	int GetWCharSize(void)
	{
		return wchar_len;
	}

	////////////////////////////////////////////////////////////////////////  
	// ANSI/GBK 转 Unicode和UTF8的构造函数  

	CWCharToChar(const char* psrc, unsigned int nSrcType = EChar
#if defined( _UTF8_ )  
		, unsigned int nDestType = EUtf8)
#else  
		, unsigned int nDestType = EWChar)
#endif  
		: m_cDest(NULL)
		, m_wcDest(NULL)
		, m_cUtf8(NULL)
		, m_nSrcType(nSrcType)
	{
		//	初始化长度数值
		ansi_char_len = 0;
		utf8_char_len = 0;
		wchar_len = 0;

		// 防止空指针  
		if (!psrc) return;
		if (!*psrc)  // 如果是空字符串，也返回空字符串  
		{
			if (EChar == m_nSrcType) m_cDest = (char*)psrc;
			if (EUtf8 == m_nSrcType) m_cUtf8 = (char*)psrc;

			if (nDestType & EChar)  m_cDest = g_NULL;
			if (nDestType & EUtf8)  m_cUtf8 = g_NULL;
			if (nDestType & EWChar) m_wcDest = (wchar_t*)g_NULL;

			return;
		}

#ifdef OS_WINDOWS  
		if (EChar == m_nSrcType)
		{
			//	此时输入类型为ansi char
			ansi_char_len = strlen(psrc);

			m_cDest = (char*)psrc;


#if defined( _UTF8_ )  
			int nLen = MultiByteToWideChar(CP_ACP, 0, m_cDest, -1, NULL, 0);
			m_wcDest = new wchar_t[nLen];
			memset(m_wcDest,0,nLen);
			wchar_len = nLen;
			MultiByteToWideChar(CP_ACP, 0, m_cDest, -1, m_wcDest, nLen);

			nLen = WideCharToMultiByte(CP_UTF8, 0, m_wcDest, -1, NULL, 0, NULL, NULL);
			m_cUtf8 = new char[nLen + 1];
			memset(m_cUtf8,0,nLen+1);
			utf8_char_len = nLen + 1;
			WideCharToMultiByte(CP_UTF8, 0, m_wcDest, -1, m_cUtf8, nLen, NULL, NULL);
#else  
			int nLen = MultiByteToWideChar(CP_ACP, 0, m_cDest, -1, NULL, 0);
			m_wcDest = new wchar_t[nLen];
			MultiByteToWideChar(CP_ACP, 0, m_cDest, -1, m_wcDest, nLen);
#endif  
		}
		else if (EUtf8 == m_nSrcType)
		{
			m_cUtf8 = (char*)psrc;

			int nLen = MultiByteToWideChar(CP_UTF8, 0, m_cUtf8, -1, NULL, 0);
			m_wcDest = new wchar_t[nLen + 1];
			memset(m_wcDest,0,nLen+1);
			MultiByteToWideChar(CP_UTF8, 0, m_cUtf8, -1, m_wcDest, nLen);

			nLen = WideCharToMultiByte(CP_ACP, 0, m_wcDest, -1, NULL, 0, NULL, NULL);
			m_cDest = new char[nLen + 1];
			memset(m_cDest,0,nLen+1);
			WideCharToMultiByte(CP_ACP, 0, m_wcDest, -1, m_cDest, nLen, NULL, NULL);

			//	获取长度
		}
#else // Linux  
		if (EChar == m_nSrcType)
		{
			m_cDest = (char*)psrc;
			if (nDestType & EUtf8)
				do_iconv_convert(psrc, &m_cUtf8, "GBK", "UTF-8");

			if (nDestType & EWChar)
				do_iconv_convert(psrc, (char**)&m_wcDest, "GBK", "wchar_t");
		}
		else if (EUtf8 == m_nSrcType)
		{
			m_cUtf8 = (char*)psrc;
			if (nDestType & EChar)
				do_iconv_convert(psrc, &m_cDest, "UTF-8", "GBK");

			if (nDestType & EWChar)
				do_iconv_convert(psrc, (char**)&m_wcDest, "UTF-8", "wchar_t");
		}
#endif  
	}
	////////////////////////////////////////////////////////////////////////  
	// Unicode 转 ANSI/GBK和UTF8的构造函数  

	CWCharToChar(const wchar_t * psrc
#if defined( _UTF8_ )  
		, unsigned int nDestType = EUtf8)
#else  
		, unsigned int nDestType = EChar)
#endif  
		: m_cDest(NULL)
		, m_wcDest(NULL)
		, m_cUtf8(NULL)
		, m_nSrcType(EWChar)
	{
		char * p = (char*)psrc;
		if (!p) return;

		m_wcDest = (wchar_t*)psrc;
		if (0 == *p && 0 == *(p + 1)) // 如果是空字符串，也返回空字符串  
		{
			if (nDestType & EChar)  m_cDest = g_NULL;
			if (nDestType & EUtf8)  m_cUtf8 = g_NULL;

			return;
		}

#ifdef OS_WINDOWS  
		// ANSI/GBK  
		{
			int nLen = WideCharToMultiByte(CP_OEMCP, NULL, m_wcDest, -1, NULL, 0, NULL, FALSE);
			m_cDest = new char[nLen];
			WideCharToMultiByte(CP_OEMCP, NULL, m_wcDest, -1, m_cDest, nLen, NULL, FALSE);
		}

#if defined( _UTF8_ )  
		{
			int nLen = WideCharToMultiByte(CP_UTF8, 0, m_wcDest, -1, NULL, 0, NULL, NULL);
			m_cUtf8 = new char[nLen + 1];
			WideCharToMultiByte(CP_UTF8, 0, m_wcDest, -1, m_cUtf8, nLen, NULL, NULL);
		}
#endif  
#else  // Linux  

		if (nDestType & EChar)
			do_iconv_convert((char*)psrc, &m_cDest, "wchar_t", "GBK");

		if (nDestType & EUtf8)
			do_iconv_convert((char*)psrc, (char**)&m_cUtf8, "wchar_t", "UTF-8");
#endif  
	}

	////////////////////////////////////////////////////////////////////////  
	~CWCharToChar()
	{
		if (EChar == m_nSrcType)
		{
			if (m_wcDest && g_NULL != (char*)m_wcDest)   delete[] m_wcDest;
			if (m_cUtf8  && g_NULL != m_cUtf8)           delete[] m_cUtf8;
		}
		else if (EWChar == m_nSrcType)
		{
			if (m_cDest && g_NULL != m_cDest)             delete[] m_cDest;
			if (m_cUtf8 && g_NULL != m_cUtf8)             delete[] m_cUtf8;
		}
		else if (EUtf8 == m_nSrcType)
		{
			if (m_cDest  && g_NULL != m_cDest)            delete[] m_cDest;
			if (m_wcDest && g_NULL != (char*)m_wcDest)   delete[] m_wcDest;
		}
	}

	////////////////////////////////////////////////////////////////////////  
	// Using iconv  for Linux  
#ifndef OS_WINDOWS  

	int  do_iconv_convert(const char* pSrc, char**ppOut, const char* pszFromCode, const char* pszToCode)
	{
		iconv_t cd = iconv_open(pszToCode, pszFromCode);
		if (cd == (iconv_t)(-1))
		{
			printf("iconv_open failed, errno: %d - %s\n", errno, strerror(errno));
			*ppOut = g_NULL;  // 为了防止给std::string赋值抛异常而导致程序终止  
			return -1;
		}
		iconv(cd, NULL, NULL, NULL, NULL);

		size_t nSLen = 0, nOLen = 0;
		if (m_nSrcType == EWChar)
#ifdef OS_WINDOWS  
			nSLen = wcslen((wchar_t*)pSrc) * 2;  // 乘以2，转换成char的长度  
#else  // Linux  
			nSLen = wcslen((wchar_t*)pSrc) * 4;  // for Unicode -> GBK/UTF8, Linux 乘以4  
#endif  
		else
			nSLen = strlen(pSrc);

		if (0 == strcmp(pszToCode, "wchar_t"))
			nOLen = nSLen * 4;        // for GBK -> Unicode  
		else
			nOLen = nSLen * 2;

		size_t nORawLen = nOLen;
		char* pOut = new char[nOLen + 4];
		char* pOutStart = pOut;

		char ** ppin = (char**)&pSrc;
		char ** ppout = &pOut;
#ifdef _DEBUG  
		printf("Convert: %s[%lu] -> %s[%lu] bytes\n", pszFromCode, nSLen, pszToCode, nOLen);
#endif  
		size_t ret = iconv(cd, (char**)ppin, &nSLen, (char**)ppout, &nOLen);
		if (0 == ret)  // 转换完成成功  
		{
			iconv_close(cd);
			int nOutLen = nORawLen - nOLen;
			*(pOutStart + nOutLen) = 0;
			*(pOutStart + nOutLen + 1) = 0; // 给转换Unicode之用  
			*ppOut = pOutStart;

			return nOutLen;
		}
		else  // 全部或部分错误  
		{
			printf("iconv failed, errno: %d - %s\n", errno, strerror(errno));
			iconv_close(cd);
			delete[] pOutStart;
			*ppOut = g_NULL;  // 为了防止给std::string赋值抛异常而导致程序终止  
			return -1;
		}
	}

#endif  

	static const char* GetNullStr(void){ return g_NULL; }

public:

	

	static std::wstring UTF8_To_UTF16(const std::string &source)
	{
		unsigned long len = ::MultiByteToWideChar(CP_UTF8, NULL, source.c_str(), -1, NULL, NULL);
		if (len == 0)
			return std::wstring();
		wchar_t *buffer = new wchar_t[len];
		::MultiByteToWideChar(CP_UTF8, NULL, source.c_str(), -1, buffer, len);

		std::wstring dest(buffer);
		delete[] buffer;
		return dest;
	}

	static std::string UTF16_To_UTF8(const std::wstring &source)
	{
		unsigned long len = ::WideCharToMultiByte(CP_UTF8, NULL, source.c_str(), -1, NULL, NULL, NULL, NULL);
		if (len == 0)
			return std::string();
		char *buffer = new char[len];
		::WideCharToMultiByte(CP_UTF8, NULL, source.c_str(), -1, buffer, len, NULL, NULL);

		std::string dest(buffer);
		delete[] buffer;
		return dest;
	}


	static std::wstring GBK_To_UTF16(const std::string &source)
	{
		enum { GB2312 = 936 };

		unsigned long len = ::MultiByteToWideChar(GB2312, NULL, source.c_str(), -1, NULL, NULL);
		if (len == 0)
			return std::wstring();
		wchar_t *buffer = new wchar_t[len];
		::MultiByteToWideChar(GB2312, NULL, source.c_str(), -1, buffer, len);

		std::wstring dest(buffer);
		delete[] buffer;
		return dest;
	}

	static std::string UTF16_To_GBK(const std::wstring &source)
	{
		enum { GB2312 = 936 };

		unsigned long len = ::WideCharToMultiByte(GB2312, NULL, source.c_str(), -1, NULL, NULL, NULL, NULL);
		if (len == 0)
			return std::string();
		char *buffer = new char[len];
		::WideCharToMultiByte(GB2312, NULL, source.c_str(), -1, buffer, len, NULL, NULL);

		std::string dest(buffer);
		delete[] buffer;
		return dest;
	}

	static std::string GBK_To_UTF8(const std::string &source)
	{
		enum { GB2312 = 936 };

		unsigned long len = ::MultiByteToWideChar(GB2312, NULL, source.c_str(), -1, NULL, NULL);
		if (len == 0)
			return std::string();
		wchar_t *wide_char_buffer = new wchar_t[len];
		::MultiByteToWideChar(GB2312, NULL, source.c_str(), -1, wide_char_buffer, len);

		len = ::WideCharToMultiByte(CP_UTF8, NULL, wide_char_buffer, -1, NULL, NULL, NULL, NULL);
		if (len == 0)
		{
			delete[] wide_char_buffer;
			return std::string();
		}
		char *multi_byte_buffer = new char[len];
		::WideCharToMultiByte(CP_UTF8, NULL, wide_char_buffer, -1, multi_byte_buffer, len, NULL, NULL);

		std::string dest(multi_byte_buffer);
		delete[] wide_char_buffer;
		delete[] multi_byte_buffer;
		return dest;
	}

	static std::string UTF8_To_GBK(const std::string &source)
	{
		enum { GB2312 = 936 };

		unsigned long len = ::MultiByteToWideChar(CP_UTF8, NULL, source.c_str(), -1, NULL, NULL);
		if (len == 0)
			return std::string();
		wchar_t *wide_char_buffer = new wchar_t[len];
		::MultiByteToWideChar(CP_UTF8, NULL, source.c_str(), -1, wide_char_buffer, len);

		len = ::WideCharToMultiByte(GB2312, NULL, wide_char_buffer, -1, NULL, NULL, NULL, NULL);
		if (len == 0)
		{
			delete[] wide_char_buffer;
			return std::string();
		}
		char *multi_byte_buffer = new char[len];
		::WideCharToMultiByte(GB2312, NULL, wide_char_buffer, -1, multi_byte_buffer, len, NULL, NULL);

		std::string dest(multi_byte_buffer);
		delete[] wide_char_buffer;
		delete[] multi_byte_buffer;
		return dest;
	}

	static std::string WChar_To_Char(const std::wstring &scr)
	{
		size_t n = scr.size() * 2;
		char *sm = new char[n];

		WideCharToMultiByte(CP_ACP, 0, scr.c_str(), -1, sm, n, NULL, 0);

		std::string des(sm);
		delete[]sm;

		return des;
	}


	static std::wstring Char_To_WChar(const std::string &scr)
	{
		size_t n = scr.size();

		wchar_t *sl = new wchar_t[n + 1];

		MultiByteToWideChar(CP_ACP, 0, scr.c_str(), -1, sl, n + 1);

		std::wstring des(sl);
		delete[]sl;

		return des;
	}

};

#endif //__CHAR_CONVERT_H__

