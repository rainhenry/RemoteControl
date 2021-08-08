/***************************************************************************************

	�������ƣ���Ļ��ͼ��
	�������ڣ�2017.03.09
	����޸ģ�2017.03.17
	����汾��REV 0.2
	ƽ̨������VS2013 Windows7 64bit SP1 MFC
	��Ʊ�д��rainhenry

	�汾�޶�
		0.1�汾		��������
		0.2�汾		����GetBitmapBuffer�������������ط�ʽ��һ��Ϊ�޲�����ֱ�ӷ���ָ�롣��һ�����в�������ͨ���������ݻ���λ�ã������Ƿ�ɹ�
					����GetBitmapBufferSize����
					ע�⣡����ĳ�Ա�����汾��ʱ��֧�ֶ��߳�ʹ�ã���
		0.3�汾		����GetBitmapInfo������SBitmapInfo�ṹ

	����ԭ��
		��MFC����£�����GDI���н�������֧�ֱ���ΪBMP�ļ��������Ե����û�����ĺ�������ͼ���ÿһ����������
		ע�⣡��������Ӧ����MFC�����,������Ҫ�����ݴ���

	ʹ�þ���
	//	����������󣬴�ʱ�Ѿ����һ�ν���������
	CScreenshort mscrs;

	//	���ͼ����Ϣ
	CScreenshort::SBitmapInfo bmp_info = mscrs.GetBitmapInfo();

	//	��ȡͼ����������ݽ��б���
	unsigned char* lpBits = mscrs.GetBitmapBuffer();
	int R, G, B;
	for (int y = 0; y < bmp_info.Height; y++)
	{
		for (int x = 0; x < bmp_info.Width; x++)
		{
			B = lpBits[y*bmp_info.WidthBytes + x * 4];
			G = lpBits[y*bmp_info.WidthBytes + x * 4 + 1];
			R = lpBits[y*bmp_info.WidthBytes + x * 4 + 2];

			//	�û�����
			//.....x y R G B
		}
	}

	//	ˢ��ͼ��
	mscrs.RefreshBitmap();

	//	���浽�ļ�
	mscrs.SaveScreenshortToBMP(_T("123.bmp"));

***************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////
//	�ض��屣��
#ifndef __CSCREENSHORT_H__
#define __CSCREENSHORT_H__

////////////////////////////////////////////////////////////////////////////////////////
//	����ͷ�ļ�
#include <afx.h>
#include <afxwin.h>

////////////////////////////////////////////////////////////////////////////////////////
//	��غ궨��
//	����������ز������û��������ͣ������ͷ��������ǲ���DWORD�ķ�ʽ
typedef void (*P_USER_SCREENSHORT_TRAVERSE_PIXEL_FUNC)(
	int,						//	ͼ���xֵ������ֵ
	int,						//	ͼ���yֵ������ֵ
	DWORD						//	RGBֵ������ͨ��GetRVal�Ⱥ�����ȡRGB����ɫ����
	);

//	����������ز������û��������ͣ������ͷ��������ǲ���RGB�����ķ�ʽ
typedef void(*P_USER_SCREENSHORT_TRAVERSE_PIXEL_RGB_FUNC)(
	int,						//	ͼ���xֵ������ֵ
	int,						//	ͼ���yֵ������ֵ
	int,						//	R
	int,						//	G
	int							//	B
	);

////////////////////////////////////////////////////////////////////////////////////////
/*
	������
	�ڲ������ȸ߶ȵ���Ļ�ĳߴ���Ϣ��������ص��豸������������ʱ����г�ʼ������������ʱ���ͷ���Դ
	���й��캯���ǹ����ķ�����ÿ�ι����ʱ�򶼻�����е���Դ���м�飬����Ѿ����ھͽ����������ͷţ�Ȼ���ٽ��й��죬�����һ�ν�������
	����������ʱ��ͻ����һ�ν�������
	������ν�ĳ�ʼ�����ǽ�����������screenshort
*/
//	�ඨ��
class CScreenshort
{
public:
	//	ͼ����Ϣ�ṹ�嶨�壬ԭʼ������ǲ���LONG���ͣ���win32��Ϊ4���ֽڣ������������int�ǿ��еģ�һ���ýṹ��Ϊ12���ֽ�
	typedef struct tagSBitmapInfo
	{
		int Width;
		int Height;
		int WidthBytes;
	}SBitmapInfo;

private:
	//	ͼ��λͼ����ָ��
	void* pBitmapBuffer;

	//	ͼ�񻺳����ĳ��ȣ��ֽڵ�λ��
	int BitmapSize;

protected:
	//	��Ļ�Ŀ��xֵ
	int nScreenWidth;

	//	��Ļ�ĸ߶�yֵ
	int nScreenHeight;

	//	��������Ĵ�����
	HWND hDesktopWnd;

	//	����DC�豸
	HDC hDesktopDC;

	//	�����ͼ��DC�豸
	HDC hCaptureDC;

	//	��ͼ�ɹ����λͼ���
	HBITMAP hCaptureBitmap;

public:
	//	�޲������캯��
	CScreenshort();

	//	��������
	~CScreenshort();

	//	��������
	void screenshort(void);

	//	ˢ�½�ͼ����screenshort��Ч
	void RefreshBitmap(void);

	//	���������BMPͼƬ�������ɹ�����true�����򷵻�false
	bool SaveScreenshortToBMP(CString filename);

	//	�����ⲿ��HBITMAP��BMP�ļ�
	static bool SaveHBITMAPToBMPEx(HBITMAP     hBitmap, CString     FileName);

	//	�������أ��ɹ���������true,ʧ�ܷ���false
	bool TraversePixel(
		P_USER_SCREENSHORT_TRAVERSE_PIXEL_FUNC ptraversepixelfunc
		);

	//	�������أ�����RGB�������ʵķ�ʽ���ɹ���������true,ʧ�ܷ���false
	bool TraversePixel(
		P_USER_SCREENSHORT_TRAVERSE_PIXEL_RGB_FUNC ptraversepixelfunc
		);

	//	�õ�ͼ������ݣ����ػ������׵�ַ
	void* GetBitmapBuffer(void);

	//	�õ�ͼ������ݣ������������ɹ�����true��ʧ�ܷ���false
	bool GetBitmapBuffer(
		void* out_buf,						//	�������������ʱ����һ�����õĻ������׵�ַ�����ṩ����ͼ������
		int* in_out_size					//	��Ϊ�����ʱ�򣬼����������������Ƿ���ã���Ϊ���������ʱ�����ʵ�ʿ������ڴ��ֽ���Ŀ
		);

	//	���ͼ�񻺳�����С
	int GetBitmapBufferSize(void);

	//	��ȡͼ����Ϣ
	SBitmapInfo GetBitmapInfo(void);
};

////////////////////////////////////////////////////////////////////////////////////////
#endif	//	__CSCREENSHORT_H__

