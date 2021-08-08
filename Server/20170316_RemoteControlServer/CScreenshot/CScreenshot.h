/***************************************************************************************

	工程名称：屏幕截图类
	创建日期：2017.03.09
	最后修改：2017.03.17
	程序版本：REV 0.2
	平台环境：VS2013 Windows7 64bit SP1 MFC
	设计编写：rainhenry

	版本修订
		0.1版本		创建工程
		0.2版本		增加GetBitmapBuffer方法的两种重载方式，一种为无参数，直接返回指针。另一种是有参数，并通过参数传递缓存位置，返回是否成功
					增加GetBitmapBufferSize方法
					注意！本类的成员，本版本暂时不支持多线程使用！！
		0.3版本		增加GetBitmapInfo方法和SBitmapInfo结构

	功能原理
		在MFC框架下，采用GDI进行截屏，并支持保存为BMP文件，并可以调用用户定义的函数遍历图像的每一个像素数据
		注意！仅仅可以应用在MFC框架下,否则需要做兼容处理

	使用举例
	//	定义截屏对象，此时已经完成一次截屏操作了
	CScreenshort mscrs;

	//	获得图像信息
	CScreenshort::SBitmapInfo bmp_info = mscrs.GetBitmapInfo();

	//	获取图像的像素数据进行遍历
	unsigned char* lpBits = mscrs.GetBitmapBuffer();
	int R, G, B;
	for (int y = 0; y < bmp_info.Height; y++)
	{
		for (int x = 0; x < bmp_info.Width; x++)
		{
			B = lpBits[y*bmp_info.WidthBytes + x * 4];
			G = lpBits[y*bmp_info.WidthBytes + x * 4 + 1];
			R = lpBits[y*bmp_info.WidthBytes + x * 4 + 2];

			//	用户处理
			//.....x y R G B
		}
	}

	//	刷新图像
	mscrs.RefreshBitmap();

	//	保存到文件
	mscrs.SaveScreenshortToBMP(_T("123.bmp"));

***************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////
//	重定义保护
#ifndef __CSCREENSHORT_H__
#define __CSCREENSHORT_H__

////////////////////////////////////////////////////////////////////////////////////////
//	包含头文件
#include <afx.h>
#include <afxwin.h>

////////////////////////////////////////////////////////////////////////////////////////
//	相关宏定义
//	定义遍历像素操作的用户函数类型，此类型访问像素是采用DWORD的方式
typedef void (*P_USER_SCREENSHORT_TRAVERSE_PIXEL_FUNC)(
	int,						//	图像的x值，横向值
	int,						//	图像的y值，纵向值
	DWORD						//	RGB值，可以通过GetRVal等宏来提取RGB的颜色分量
	);

//	定义遍历像素操作的用户函数类型，此类型访问像素是采用RGB分量的方式
typedef void(*P_USER_SCREENSHORT_TRAVERSE_PIXEL_RGB_FUNC)(
	int,						//	图像的x值，横向值
	int,						//	图像的y值，纵向值
	int,						//	R
	int,						//	G
	int							//	B
	);

////////////////////////////////////////////////////////////////////////////////////////
/*
	截屏类
	内部定义宽度高度等屏幕的尺寸信息和所有相关的设备句柄，当构造的时候进行初始化，当析构的时候释放资源
	其中构造函数是公共的方法，每次构造的时候都会对所有的资源进行检查，如果已经存在就进行析构和释放，然后再进行构造，以完成一次截屏操作
	即类对象构造的时候就会进行一次截屏操作
	这里所谓的初始化就是截屏操作函数screenshort
*/
//	类定义
class CScreenshort
{
public:
	//	图像信息结构体定义，原始定义的是采用LONG类型，在win32下为4个字节，所以这里采用int是可行的，一共该结构体为12个字节
	typedef struct tagSBitmapInfo
	{
		int Width;
		int Height;
		int WidthBytes;
	}SBitmapInfo;

private:
	//	图像位图缓存指针
	void* pBitmapBuffer;

	//	图像缓冲区的长度（字节单位）
	int BitmapSize;

protected:
	//	屏幕的宽度x值
	int nScreenWidth;

	//	屏幕的高度y值
	int nScreenHeight;

	//	整个桌面的窗体句柄
	HWND hDesktopWnd;

	//	桌面DC设备
	HDC hDesktopDC;

	//	保存截图的DC设备
	HDC hCaptureDC;

	//	截图成功后的位图句柄
	HBITMAP hCaptureBitmap;

public:
	//	无参数构造函数
	CScreenshort();

	//	析构函数
	~CScreenshort();

	//	截屏函数
	void screenshort(void);

	//	刷新截图，与screenshort等效
	void RefreshBitmap(void);

	//	保存截屏到BMP图片，操作成功返回true，否则返回false
	bool SaveScreenshortToBMP(CString filename);

	//	保存外部的HBITMAP到BMP文件
	static bool SaveHBITMAPToBMPEx(HBITMAP     hBitmap, CString     FileName);

	//	遍历像素，成功操作返回true,失败返回false
	bool TraversePixel(
		P_USER_SCREENSHORT_TRAVERSE_PIXEL_FUNC ptraversepixelfunc
		);

	//	遍历像素，采用RGB分量访问的方式，成功操作返回true,失败返回false
	bool TraversePixel(
		P_USER_SCREENSHORT_TRAVERSE_PIXEL_RGB_FUNC ptraversepixelfunc
		);

	//	得到图像的数据，返回缓冲区首地址
	void* GetBitmapBuffer(void);

	//	得到图像的数据，拷贝方法，成功返回true，失败返回false
	bool GetBitmapBuffer(
		void* out_buf,						//	输出参数，调用时传入一个可用的缓冲区首地址，以提供复制图像数据
		int* in_out_size					//	作为输入的时候，检查输出缓冲区长度是否可用，作为输出参数的时候，输出实际拷贝的内存字节数目
		);

	//	获得图像缓冲区大小
	int GetBitmapBufferSize(void);

	//	获取图像信息
	SBitmapInfo GetBitmapInfo(void);
};

////////////////////////////////////////////////////////////////////////////////////////
#endif	//	__CSCREENSHORT_H__

