

//	程序版本0.4

#include "stdafx.h"
#include "CBrowseDlg.h"


//	浏览文件函数定义，未选择时返回""
CString CBrowseDlg::FileBrowseDlg()
{
	CString str;
	CFileDialog fileDlg(TRUE);
#ifdef UNICODE
	//fileDlg.m_ofn.lpstrTitle=L"文件打开对话框";
	fileDlg.m_ofn.lpstrFilter = L"All Files(*.*)\0*.*\0\0";
#else
	//fileDlg.m_ofn.lpstrTitle="文件打开对话框";
	fileDlg.m_ofn.lpstrFilter = "All Files(*.*)\0*.*\0\0";
#endif
	if (IDOK == fileDlg.DoModal())
	{
		//CFile file(fileDlg.GetFileName(),CFile::modeRead);
		str = fileDlg.GetPathName();//文件名+后缀 
	}
	return str;
}

//保存文件  ，未选择时返回""
CString CBrowseDlg::SaveFileDlg(CString default_open_path, CString default_open_file)
{
	BOOL isOpen = FALSE;        //是否打开(否则为保存)  
	CString defaultDir = default_open_path;   //默认打开的文件路径  
	CString fileName = default_open_file;         //默认打开的文件名  
	//CString filter = L"文件 (*.doc; *.ppt; *.xls)|*.doc;*.ppt;*.xls||";   //文件过虑的类型  
	CString filter = L"所有文件 (*.*)|*.*||";   //文件过虑的类型  
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, NULL);
	openFileDlg.GetOFN().lpstrInitialDir = default_open_path;
	INT_PTR result = openFileDlg.DoModal();
	CString filePath = _T("");
	if (result == IDOK) {
		filePath = openFileDlg.GetPathName();
	}
	return filePath;
}

//	路径浏览函数定义，未选择时返回""，选择时路径以"\\"结尾
CString CBrowseDlg::PathBrowseDlg()
{
	BROWSEINFO bi;
#ifdef UNICODE
	WCHAR name[MAX_PATH];
#else
	char name[MAX_PATH];
#endif
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
	bi.pszDisplayName = name;
#ifdef UNICODE
	//bi.lpszTitle = L"选择文件夹目录";
#else
	//bi.lpszTitle = "选择文件夹目录";
#endif
	bi.ulFlags = BIF_RETURNFSANCESTORS;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
#ifdef UNICODE
	if (idl == NULL)
		return L"";
#else
	if (idl == NULL)
		return "";
#endif
	CString strDirectoryPath;
	SHGetPathFromIDList(idl, strDirectoryPath.GetBuffer(MAX_PATH));
	strDirectoryPath.ReleaseBuffer();
#ifdef UNICODE
	if (strDirectoryPath.IsEmpty())
		return L"";
	if (strDirectoryPath.Right(1) != L"\\")
		strDirectoryPath += L"\\";
#else
	if (strDirectoryPath.IsEmpty())
		return "";
	if (strDirectoryPath.Right(1) != "\\")
		strDirectoryPath += "\\";
#endif

	return strDirectoryPath;
}

//	通过完整的文件路径（包括文件名在内），提取出路径的字符串
CString CBrowseDlg::GetPathString(CString in_str)
{
	//	如果输入为空，返回也为空
	if (in_str == _T(""))
	{
		return _T("");
	}

	//	逆向查找"\\"，并同时删除不是"\\"的字符
	for (int i = in_str.GetLength(); i >= 0; --i)
	{
		if (in_str.GetAt(i) != '\\')
		{
			in_str.Delete(i);
		}
		else
		{
			break;
		}
	}

	return in_str;
}

