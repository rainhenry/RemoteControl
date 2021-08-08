
//	程序版本 REV 0.3

#include "stdafx.h"
#include "CScreenshot.h"


//	无参数构造函数
CScreenshort::CScreenshort()
{
	//	将所有的句柄和资源都设置为空
	this->hCaptureBitmap = NULL;
	this->hCaptureDC = NULL;
	this->hDesktopDC = NULL;
	this->hDesktopWnd = NULL;
	this->nScreenHeight = 0;
	this->nScreenWidth = 0;
	this->pBitmapBuffer = NULL;
	this->BitmapSize = 0;

	//	刚刚构造的时候进行一次截屏
	this->screenshort();
}

//	析构函数
CScreenshort::~CScreenshort()
{
	//	检查进行释放桌面的句柄和DC
	if ((this->hDesktopDC != NULL) || (this->hDesktopWnd != NULL))
	{
		ReleaseDC(this->hDesktopWnd, this->hDesktopDC);
		this->hDesktopWnd = NULL;
		this->hDesktopWnd = NULL;
	}

	//	检查容纳DC设备
	if (this->hCaptureDC != NULL)
	{
		DeleteDC(this->hCaptureDC);
		this->hCaptureDC = NULL;
	}

	//	检查图像容器
	if (this->hCaptureBitmap != NULL)
	{
		DeleteObject(this->hCaptureBitmap);
		this->hCaptureBitmap = NULL;
	}
	
	//	检查图像缓存并释放内存
	if (this->pBitmapBuffer != NULL)
	{
		this->BitmapSize = 0;
		delete[] (this->pBitmapBuffer);
		this->pBitmapBuffer = NULL;
	}
}

//	截屏函数
void CScreenshort::screenshort(void)
{
	//	检查进行释放桌面的句柄和DC
	if ((this->hDesktopDC != NULL) || (this->hDesktopWnd != NULL))
	{
		ReleaseDC(this->hDesktopWnd, this->hDesktopDC);
		this->hDesktopWnd = NULL;
		this->hDesktopWnd = NULL;
	}

	//	检查容纳DC设备
	if (this->hCaptureDC != NULL)
	{
		DeleteDC(this->hCaptureDC);
		this->hCaptureDC = NULL;
	}

	//	检查图像容器
	if (this->hCaptureBitmap != NULL)
	{
		DeleteObject(this->hCaptureBitmap);
		this->hCaptureBitmap = NULL;
	}

	//	获得屏幕的分辨率
	this->nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	this->nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	this->hDesktopWnd = GetDesktopWindow();
	this->hDesktopDC = GetDC(this->hDesktopWnd);
	this->hCaptureDC = CreateCompatibleDC(this->hDesktopDC);
	this->hCaptureBitmap = CreateCompatibleBitmap(
		this->hDesktopDC,
		this->nScreenWidth, this->nScreenHeight
		);
	SelectObject(this->hCaptureDC, this->hCaptureBitmap);
	BitBlt(this->hCaptureDC, 0, 0, this->nScreenWidth, this->nScreenHeight, this->hDesktopDC, 0, 0, SRCCOPY);

	//	0.2版本增加，截图后将图像数据拷贝到缓存
	//	获取需要的缓存大小
	//	尝试通过CBitmap访问
	CBitmap mcbitmap;
	mcbitmap.Attach(this->hCaptureBitmap);

	//	获得图像的信息
	BITMAP mbm = { 0 };
	mcbitmap.GetBitmap(&mbm);

	//	遍历RGB数据
	//	获得图像数据缓冲区的尺寸
	int size = mbm.bmHeight*mbm.bmWidthBytes;

	//	缓冲区变动的时候，将缓存区大小设置为0，防止多线程访问错误
	this->BitmapSize = 0;

	//	如果缓存没有申请，则进行申请
	if (this->pBitmapBuffer == NULL)
	{
		this->pBitmapBuffer = new char[size];
	}
	//	释放旧的缓存，重新申请
	else
	{
		delete[](this->pBitmapBuffer);
		this->pBitmapBuffer = new char[size];
	}

	//	复制数据
	mcbitmap.GetBitmapBits(size, this->pBitmapBuffer);

	//	更新大小
	this->BitmapSize = size;
}

//	刷新截图，与screenshort等效
void CScreenshort::RefreshBitmap(void)
{
	this->screenshort();
}

//	保存截屏到BMP图片，操作成功返回true，否则返回false
bool CScreenshort::SaveScreenshortToBMP(CString filename)
{
	//	检查输入文件名
	if (filename == _T(""))
	{
		//	当文件名为空的时候不进行操作
		return false;
	}

	//	以下进行保存图片操作
	return CScreenshort::SaveHBITMAPToBMPEx(
		this->hCaptureBitmap,
		filename
		);
}

//	保存外部的HBITMAP到BMP文件
bool CScreenshort::SaveHBITMAPToBMPEx(HBITMAP     hBitmap, CString     FileName)
{
	//	参数检查
	if (hBitmap == NULL)	return false;
	if (FileName == _T("")) return false;

	HDC     hDC;
	//当前分辨率下每象素所占字节数         
	int     iBits;
	//位图中每象素所占字节数         
	WORD     wBitCount;
	//定义调色板大小，     位图中像素字节大小     ，位图文件大小     ，     写入文件字节数             
	DWORD     dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	//位图属性结构             
	BITMAP     Bitmap;
	//位图文件头结构         
	BITMAPFILEHEADER     bmfHdr;
	//位图信息头结构             
	BITMAPINFOHEADER     bi;
	//指向位图信息头结构                 
	LPBITMAPINFOHEADER     lpbi;
	//定义文件，分配内存句柄，调色板句柄             
	HANDLE     fh, hDib, hPal, hOldPal = NULL;

	//计算位图文件每个像素所占字节数             
	hDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL)     *     GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else  if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else
		wBitCount = 24;

	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 0;

	dwBmBitsSize = ((Bitmap.bmWidth *wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	//为位图内容分配内存             
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	//     处理调色板                 
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}

	//     获取该调色板下新的像素值             
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight,
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize,
		(BITMAPINFO *)lpbi, DIB_RGB_COLORS);

	//恢复调色板                 
	if (hOldPal)
	{
		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//创建位图文件                 
	fh = CreateFile(FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)         return     FALSE;

	//     设置位图文件头             
	bmfHdr.bfType = 0x4D42;     //     "BM"             
	dwDIBSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;
	//     写入位图文件头             
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	//     写入位图文件其余内容             
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	//清除                 
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);

	return     TRUE;
}

//	遍历像素，成功操作返回true,失败返回false
bool CScreenshort::TraversePixel(
	P_USER_SCREENSHORT_TRAVERSE_PIXEL_FUNC ptraversepixelfunc
	)
{
	//	检查输入函数的指针
	if ((ptraversepixelfunc == NULL) || (ptraversepixelfunc == (P_USER_SCREENSHORT_TRAVERSE_PIXEL_FUNC)(-1)))
	{
		return false;
	}

	//	尝试通过CBitmap访问
	CBitmap mcbitmap;
	mcbitmap.Attach(this->hCaptureBitmap);

	//	获得图像的信息
	BITMAP mbm = {0};
	mcbitmap.GetBitmap(&mbm);

	//	遍历RGB数据
	//	获得图像数据缓冲区的尺寸
	int size = mbm.bmHeight*mbm.bmWidthBytes; 

	//	申请图像缓冲区内存
	BYTE *lpBits = new BYTE[size]; 
	
	mcbitmap.GetBitmapBits(size, lpBits);//得到RGB数据
	
	//	定义RGB的变量
	int R, G, B;

	for (int y = 0; y < mbm.bmHeight; y++)
	{
		for (int x = 0; x<mbm.bmWidth; x++)
		{
			B = lpBits[y*mbm.bmWidthBytes + x * 4];
			G = lpBits[y*mbm.bmWidthBytes + x * 4 + 1];
			R = lpBits[y*mbm.bmWidthBytes + x * 4 + 2];

			//	调用用户函数
			ptraversepixelfunc(x,y,RGB(R,G,B));
		}
	}

	//	释放内存
	delete[]lpBits;
	return true;
}

//	遍历像素，采用RGB分量访问的方式，成功操作返回true,失败返回false
bool CScreenshort::TraversePixel(
	P_USER_SCREENSHORT_TRAVERSE_PIXEL_RGB_FUNC ptraversepixelfunc
	)
{
	//	检查输入函数的指针
	if ((ptraversepixelfunc == NULL) || (ptraversepixelfunc == (P_USER_SCREENSHORT_TRAVERSE_PIXEL_RGB_FUNC)(-1)))
	{
		return false;
	}

	//	尝试通过CBitmap访问
	CBitmap mcbitmap;
	mcbitmap.Attach(this->hCaptureBitmap);

	//	获得图像的信息
	BITMAP mbm = { 0 };
	mcbitmap.GetBitmap(&mbm);

	//	遍历RGB数据
	//	获得图像数据缓冲区的尺寸
	int size = mbm.bmHeight*mbm.bmWidthBytes;

	//	申请图像缓冲区内存
	BYTE *lpBits = new BYTE[size];

	mcbitmap.GetBitmapBits(size, lpBits);//得到RGB数据

	//	定义RGB的变量
	int R, G, B;

	for (int y = 0; y < mbm.bmHeight; y++)
	{
		for (int x = 0; x<mbm.bmWidth; x++)
		{
			B = lpBits[y*mbm.bmWidthBytes + x * 4];
			G = lpBits[y*mbm.bmWidthBytes + x * 4 + 1];
			R = lpBits[y*mbm.bmWidthBytes + x * 4 + 2];

			//	调用用户函数
			ptraversepixelfunc(x, y, R, G, B);
		}
	}

	//	释放内存
	delete[]lpBits;
	return true;
}

//	得到图像的数据，返回缓冲区首地址
void* CScreenshort::GetBitmapBuffer(void)
{
	if (this->BitmapSize == 0)	return NULL;
	return this->pBitmapBuffer;
}

//	得到图像的数据，拷贝方法，成功返回true，失败返回false
bool CScreenshort::GetBitmapBuffer(
	void* out_buf,						//	输出参数，调用时传入一个可用的缓冲区首地址，以提供复制图像数据
	int* in_out_size					//	作为输入的时候，检查输出缓冲区长度是否可用，作为输出参数的时候，输出实际拷贝的内存字节数目
	)
{
	//	检查输入缓冲区首地址
	if ((out_buf == NULL)||(out_buf == (void*)(-1)))
	{
		return false;
	}

	//	检查缓冲区深度
	if (*in_out_size < this->BitmapSize)
	{
		return false;
	}

	//	检查本地数据
	if ((this->BitmapSize == 0)||(this->pBitmapBuffer == NULL))
	{
		return false;
	}

	//	复制数据
	memcpy_s(out_buf, *in_out_size, this->pBitmapBuffer, this->BitmapSize);

	//	设置复制大小
	*in_out_size = this->BitmapSize;

	//	返回成功
	return true;
}

//	获得图像缓冲区大小
int CScreenshort::GetBitmapBufferSize(void)
{
	return this->BitmapSize;
}

//	获取图像信息
CScreenshort::SBitmapInfo CScreenshort::GetBitmapInfo(void)
{
	//	尝试通过CBitmap访问
	CBitmap mcbitmap;
	mcbitmap.Attach(this->hCaptureBitmap);

	//	获得图像的信息
	BITMAP mbm = { 0 };
	mcbitmap.GetBitmap(&mbm);

	//	定义返回结构体
	SBitmapInfo tmp_re;
	tmp_re.Height = mbm.bmHeight;
	tmp_re.Width = mbm.bmWidth;
	tmp_re.WidthBytes = mbm.bmWidthBytes;

	//	返回数据
	return tmp_re;
}
