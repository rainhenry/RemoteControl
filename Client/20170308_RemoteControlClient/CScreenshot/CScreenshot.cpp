
//	����汾 REV 0.3

#include "stdafx.h"
#include "CScreenshot.h"


//	�޲������캯��
CScreenshort::CScreenshort()
{
	//	�����еľ������Դ������Ϊ��
	this->hCaptureBitmap = NULL;
	this->hCaptureDC = NULL;
	this->hDesktopDC = NULL;
	this->hDesktopWnd = NULL;
	this->nScreenHeight = 0;
	this->nScreenWidth = 0;
	this->pBitmapBuffer = NULL;
	this->BitmapSize = 0;

	//	�ոչ����ʱ�����һ�ν���
	this->screenshort();
}

//	��������
CScreenshort::~CScreenshort()
{
	//	�������ͷ�����ľ����DC
	if ((this->hDesktopDC != NULL) || (this->hDesktopWnd != NULL))
	{
		ReleaseDC(this->hDesktopWnd, this->hDesktopDC);
		this->hDesktopWnd = NULL;
		this->hDesktopWnd = NULL;
	}

	//	�������DC�豸
	if (this->hCaptureDC != NULL)
	{
		DeleteDC(this->hCaptureDC);
		this->hCaptureDC = NULL;
	}

	//	���ͼ������
	if (this->hCaptureBitmap != NULL)
	{
		DeleteObject(this->hCaptureBitmap);
		this->hCaptureBitmap = NULL;
	}
	
	//	���ͼ�񻺴沢�ͷ��ڴ�
	if (this->pBitmapBuffer != NULL)
	{
		this->BitmapSize = 0;
		delete[] (this->pBitmapBuffer);
		this->pBitmapBuffer = NULL;
	}
}

//	��������
void CScreenshort::screenshort(void)
{
	//	�������ͷ�����ľ����DC
	if ((this->hDesktopDC != NULL) || (this->hDesktopWnd != NULL))
	{
		ReleaseDC(this->hDesktopWnd, this->hDesktopDC);
		this->hDesktopWnd = NULL;
		this->hDesktopWnd = NULL;
	}

	//	�������DC�豸
	if (this->hCaptureDC != NULL)
	{
		DeleteDC(this->hCaptureDC);
		this->hCaptureDC = NULL;
	}

	//	���ͼ������
	if (this->hCaptureBitmap != NULL)
	{
		DeleteObject(this->hCaptureBitmap);
		this->hCaptureBitmap = NULL;
	}

	//	�����Ļ�ķֱ���
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

	//	0.2�汾���ӣ���ͼ��ͼ�����ݿ���������
	//	��ȡ��Ҫ�Ļ����С
	//	����ͨ��CBitmap����
	CBitmap mcbitmap;
	mcbitmap.Attach(this->hCaptureBitmap);

	//	���ͼ�����Ϣ
	BITMAP mbm = { 0 };
	mcbitmap.GetBitmap(&mbm);

	//	����RGB����
	//	���ͼ�����ݻ������ĳߴ�
	int size = mbm.bmHeight*mbm.bmWidthBytes;

	//	�������䶯��ʱ�򣬽���������С����Ϊ0����ֹ���̷߳��ʴ���
	this->BitmapSize = 0;

	//	�������û�����룬���������
	if (this->pBitmapBuffer == NULL)
	{
		this->pBitmapBuffer = new char[size];
	}
	//	�ͷžɵĻ��棬��������
	else
	{
		delete[](this->pBitmapBuffer);
		this->pBitmapBuffer = new char[size];
	}

	//	��������
	mcbitmap.GetBitmapBits(size, this->pBitmapBuffer);

	//	���´�С
	this->BitmapSize = size;
}

//	ˢ�½�ͼ����screenshort��Ч
void CScreenshort::RefreshBitmap(void)
{
	this->screenshort();
}

//	���������BMPͼƬ�������ɹ�����true�����򷵻�false
bool CScreenshort::SaveScreenshortToBMP(CString filename)
{
	//	��������ļ���
	if (filename == _T(""))
	{
		//	���ļ���Ϊ�յ�ʱ�򲻽��в���
		return false;
	}

	//	���½��б���ͼƬ����
	return CScreenshort::SaveHBITMAPToBMPEx(
		this->hCaptureBitmap,
		filename
		);
}

//	�����ⲿ��HBITMAP��BMP�ļ�
bool CScreenshort::SaveHBITMAPToBMPEx(HBITMAP     hBitmap, CString     FileName)
{
	//	�������
	if (hBitmap == NULL)	return false;
	if (FileName == _T("")) return false;

	HDC     hDC;
	//��ǰ�ֱ�����ÿ������ռ�ֽ���         
	int     iBits;
	//λͼ��ÿ������ռ�ֽ���         
	WORD     wBitCount;
	//�����ɫ���С��     λͼ�������ֽڴ�С     ��λͼ�ļ���С     ��     д���ļ��ֽ���             
	DWORD     dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	//λͼ���Խṹ             
	BITMAP     Bitmap;
	//λͼ�ļ�ͷ�ṹ         
	BITMAPFILEHEADER     bmfHdr;
	//λͼ��Ϣͷ�ṹ             
	BITMAPINFOHEADER     bi;
	//ָ��λͼ��Ϣͷ�ṹ                 
	LPBITMAPINFOHEADER     lpbi;
	//�����ļ��������ڴ�������ɫ����             
	HANDLE     fh, hDib, hPal, hOldPal = NULL;

	//����λͼ�ļ�ÿ��������ռ�ֽ���             
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

	//Ϊλͼ���ݷ����ڴ�             
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	//     �����ɫ��                 
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}

	//     ��ȡ�õ�ɫ�����µ�����ֵ             
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight,
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize,
		(BITMAPINFO *)lpbi, DIB_RGB_COLORS);

	//�ָ���ɫ��                 
	if (hOldPal)
	{
		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//����λͼ�ļ�                 
	fh = CreateFile(FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)         return     FALSE;

	//     ����λͼ�ļ�ͷ             
	bmfHdr.bfType = 0x4D42;     //     "BM"             
	dwDIBSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;
	//     д��λͼ�ļ�ͷ             
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	//     д��λͼ�ļ���������             
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	//���                 
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);

	return     TRUE;
}

//	�������أ��ɹ���������true,ʧ�ܷ���false
bool CScreenshort::TraversePixel(
	P_USER_SCREENSHORT_TRAVERSE_PIXEL_FUNC ptraversepixelfunc
	)
{
	//	������뺯����ָ��
	if ((ptraversepixelfunc == NULL) || (ptraversepixelfunc == (P_USER_SCREENSHORT_TRAVERSE_PIXEL_FUNC)(-1)))
	{
		return false;
	}

	//	����ͨ��CBitmap����
	CBitmap mcbitmap;
	mcbitmap.Attach(this->hCaptureBitmap);

	//	���ͼ�����Ϣ
	BITMAP mbm = {0};
	mcbitmap.GetBitmap(&mbm);

	//	����RGB����
	//	���ͼ�����ݻ������ĳߴ�
	int size = mbm.bmHeight*mbm.bmWidthBytes; 

	//	����ͼ�񻺳����ڴ�
	BYTE *lpBits = new BYTE[size]; 
	
	mcbitmap.GetBitmapBits(size, lpBits);//�õ�RGB����
	
	//	����RGB�ı���
	int R, G, B;

	for (int y = 0; y < mbm.bmHeight; y++)
	{
		for (int x = 0; x<mbm.bmWidth; x++)
		{
			B = lpBits[y*mbm.bmWidthBytes + x * 4];
			G = lpBits[y*mbm.bmWidthBytes + x * 4 + 1];
			R = lpBits[y*mbm.bmWidthBytes + x * 4 + 2];

			//	�����û�����
			ptraversepixelfunc(x,y,RGB(R,G,B));
		}
	}

	//	�ͷ��ڴ�
	delete[]lpBits;
	return true;
}

//	�������أ�����RGB�������ʵķ�ʽ���ɹ���������true,ʧ�ܷ���false
bool CScreenshort::TraversePixel(
	P_USER_SCREENSHORT_TRAVERSE_PIXEL_RGB_FUNC ptraversepixelfunc
	)
{
	//	������뺯����ָ��
	if ((ptraversepixelfunc == NULL) || (ptraversepixelfunc == (P_USER_SCREENSHORT_TRAVERSE_PIXEL_RGB_FUNC)(-1)))
	{
		return false;
	}

	//	����ͨ��CBitmap����
	CBitmap mcbitmap;
	mcbitmap.Attach(this->hCaptureBitmap);

	//	���ͼ�����Ϣ
	BITMAP mbm = { 0 };
	mcbitmap.GetBitmap(&mbm);

	//	����RGB����
	//	���ͼ�����ݻ������ĳߴ�
	int size = mbm.bmHeight*mbm.bmWidthBytes;

	//	����ͼ�񻺳����ڴ�
	BYTE *lpBits = new BYTE[size];

	mcbitmap.GetBitmapBits(size, lpBits);//�õ�RGB����

	//	����RGB�ı���
	int R, G, B;

	for (int y = 0; y < mbm.bmHeight; y++)
	{
		for (int x = 0; x<mbm.bmWidth; x++)
		{
			B = lpBits[y*mbm.bmWidthBytes + x * 4];
			G = lpBits[y*mbm.bmWidthBytes + x * 4 + 1];
			R = lpBits[y*mbm.bmWidthBytes + x * 4 + 2];

			//	�����û�����
			ptraversepixelfunc(x, y, R, G, B);
		}
	}

	//	�ͷ��ڴ�
	delete[]lpBits;
	return true;
}

//	�õ�ͼ������ݣ����ػ������׵�ַ
void* CScreenshort::GetBitmapBuffer(void)
{
	if (this->BitmapSize == 0)	return NULL;
	return this->pBitmapBuffer;
}

//	�õ�ͼ������ݣ������������ɹ�����true��ʧ�ܷ���false
bool CScreenshort::GetBitmapBuffer(
	void* out_buf,						//	�������������ʱ����һ�����õĻ������׵�ַ�����ṩ����ͼ������
	int* in_out_size					//	��Ϊ�����ʱ�򣬼����������������Ƿ���ã���Ϊ���������ʱ�����ʵ�ʿ������ڴ��ֽ���Ŀ
	)
{
	//	������뻺�����׵�ַ
	if ((out_buf == NULL)||(out_buf == (void*)(-1)))
	{
		return false;
	}

	//	��黺�������
	if (*in_out_size < this->BitmapSize)
	{
		return false;
	}

	//	��鱾������
	if ((this->BitmapSize == 0)||(this->pBitmapBuffer == NULL))
	{
		return false;
	}

	//	��������
	memcpy_s(out_buf, *in_out_size, this->pBitmapBuffer, this->BitmapSize);

	//	���ø��ƴ�С
	*in_out_size = this->BitmapSize;

	//	���سɹ�
	return true;
}

//	���ͼ�񻺳�����С
int CScreenshort::GetBitmapBufferSize(void)
{
	return this->BitmapSize;
}

//	��ȡͼ����Ϣ
CScreenshort::SBitmapInfo CScreenshort::GetBitmapInfo(void)
{
	//	����ͨ��CBitmap����
	CBitmap mcbitmap;
	mcbitmap.Attach(this->hCaptureBitmap);

	//	���ͼ�����Ϣ
	BITMAP mbm = { 0 };
	mcbitmap.GetBitmap(&mbm);

	//	���巵�ؽṹ��
	SBitmapInfo tmp_re;
	tmp_re.Height = mbm.bmHeight;
	tmp_re.Width = mbm.bmWidth;
	tmp_re.WidthBytes = mbm.bmWidthBytes;

	//	��������
	return tmp_re;
}
