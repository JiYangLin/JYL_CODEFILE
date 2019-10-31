#pragma once
#include <windows.h>
#include <vector>
#include <string>
using namespace std;

inline CString GetCurPath()
{
	CString path;
	GetModuleFileName(NULL, path.GetBuffer(MAX_PATH), MAX_PATH);   

	path.ReleaseBuffer();

	path = path.Left(path.ReverseFind(_T('\\')));
	return path;
}




//打开文件
inline bool ChoiceFile(CString &filePathName,CString filter,CString startPath)
{
	//_T("xx(*.jpg)|*.jpg|所有文件(*.*)|*.*||")
	CFileDialog  dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY |OFN_OVERWRITEPROMPT,filter);
	if (startPath != _T("")) {
		dlg.m_ofn.lpstrInitialDir = startPath;
	}
	if(IDCANCEL==dlg.DoModal()) return false; 

	filePathName = dlg.GetPathName();

	return true;
}

inline bool GetDirPath(CString &strFolderPath,HWND ParentWnd=NULL,CString Title = _T("请选择一个文件夹"))  
{  
	TCHAR           szFolderPath[MAX_PATH] = {0};  

	BROWSEINFO      sInfo;  
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));  
	sInfo.pidlRoot   = 0;  
	sInfo.lpszTitle   =  Title;
	sInfo.ulFlags   = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;  
	sInfo.lpfn     = NULL;
	sInfo.hwndOwner = ParentWnd;

	// 显示文件夹选择对话框  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);   
	if (lpidlBrowse != NULL)  
	{  
		// 取得文件夹名  
		if (::SHGetPathFromIDList(lpidlBrowse,szFolderPath))    
		{  
			strFolderPath = szFolderPath;  
		}  
		::CoTaskMemFree(lpidlBrowse);  
	}  

	if(lpidlBrowse == NULL)   return false;
	return true;  
} 


inline bool DeleteDirectory(CString sDirName)  
{  
	CFileFind tempFind;  
	CString sTempFileFind; 

	sTempFileFind.Format(_T("%s\\*.*"),sDirName);
	BOOL IsFinded = tempFind.FindFile(sTempFileFind);  
	while (IsFinded)  
	{  
		IsFinded = tempFind.FindNextFile();  

		if (!tempFind.IsDots())  
		{  
			CString sFoundFileName = tempFind.GetFileName(); 

			if (tempFind.IsDirectory())  
			{  
				CString sTempDir;  
				sTempDir.Format(_T("%s\\%s"),sDirName,sFoundFileName);  
				DeleteDirectory(sTempDir);  
			}  
			else  
			{  
				CString sTempFileName;  
				sTempFileName.Format(_T("%s\\%s"),sDirName,sFoundFileName);  
				DeleteFile(sTempFileName);  
			}  
		}  
	}  
	tempFind.Close();  
	if(!RemoveDirectory(sDirName))  
	{  
		return false;  
	}  
	return true;  
} 


inline int SplitStr(char *str,char *strFind,char** pFindRet,int FindRetSize)
{//返回拆分个数
	char *p = NULL;
	int cnt = 0;
	while(NULL != (p = strstr(str,strFind)))
	{
		if(cnt >= FindRetSize) break;
		pFindRet[cnt++] = str;
		*p = 0;
		str = ++p;
	}
	if(strlen(str) != 0 && cnt < FindRetSize)
	{
		pFindRet[cnt++] = str;
	}
	return cnt;
}
inline void SplitStr(char *str,char *strFind,vector<char*> &pFindRet)
{
	char *p = NULL;
	while(NULL != (p = strstr(str,strFind)))
	{
		char *pStr = str;
		pFindRet.push_back(pStr);
		*p = 0;
		str = ++p;
	}
	if(strlen(str) != 0)
	{
		char *pStr = str;
		pFindRet.push_back(pStr);
	}
}
inline void SplitString(vector<string> &vecStr,string str,char split = ',')
{
	string strLeft;
	size_t pos = str.find(split);
	while(string::npos != pos)
	{
		strLeft = str.substr(0,pos);
		vecStr.push_back(strLeft);
		str = str.substr(pos + 1);
		pos = str.find(split);
	}

	if(str != "")
		vecStr.push_back(str);
}
inline void SplitCString(CString strIn, vector<CString>& strAryRe, CString Split)
{
	int nStart = 0;
	int nEnd = 0;
	CString strTmp;
	while ((nEnd = strIn.Find(Split, nStart)) != -1)
	{
		strTmp = strIn.Mid(nStart, nEnd - nStart);
		strAryRe.push_back(strTmp);
		nStart = nEnd + Split.GetLength();
	}
	strTmp = strIn.Mid(nStart, strIn.GetLength());
	strAryRe.push_back(strTmp);
}
inline void WriteClipboard(char* pStr,int strLen)
{
	if (!OpenClipboard(NULL)) return;

	EmptyClipboard(); 

	HANDLE hClip = GlobalAlloc(GMEM_MOVEABLE, strLen);
	void *pBuf = GlobalLock(hClip);
	memcpy_s(pBuf, strLen, pStr, strLen);

	SetClipboardData(CF_TEXT, hClip);

	GlobalUnlock(hClip);
	CloseClipboard();
}

template<typename ProcLineFun>
inline void ReadCsvFileLine(const char * _path,bool removeHead,ProcLineFun _fun)
{
	FILE *pf = NULL;
	fopen_s(&pf,_path,"r");
	if (NULL == pf)  throw(string(_path) + "  打开失败");

	char str[1024] = {0};
	vector<char*> split;

	bool breakRead = false;
	if (removeHead) fgets(str,1024,pf);
	memset(str,0,1024);
	while(fgets(str,1024,pf))
	{
		split.clear();
		SplitStr(str,",",split);
		_fun(split,breakRead);
		if (breakRead) break;
		memset(str,0,1024);
	}
	fclose(pf);
}


typedef std::function<void(const char* pFileBuf, int fileLen)>  GetFileContnet_Fun;
inline void GetFileContnet(const char *filepath, GetFileContnet_Fun fun)
{
	FILE *pf = NULL;
	fopen_s(&pf, filepath, "rb");
	if (NULL == pf) return ;
	fseek(pf, 0, SEEK_END);
	int len = ftell(pf);
	fseek(pf, 0, SEEK_SET);
	if (0 == len)
	{
		fclose(pf);
		return ;
	}
	char *pFile = new char[len];
	fread_s(pFile, len, len, 1, pf);
	fclose(pf);
	fun(pFile, len);
	delete[] pFile;
}

#define GUIDSTRLEN    33
inline void GetGUIDNum(char *pGuidBuf)
{
	memset(pGuidBuf,0,GUIDSTRLEN);
	GUID guid;
	CoCreateGuid(&guid);
	sprintf_s(pGuidBuf,GUIDSTRLEN,"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
		guid.Data1,guid.Data2,guid.Data3,
		guid.Data4[0],guid.Data4[1],guid.Data4[2],guid.Data4[3],guid.Data4[4],guid.Data4[5],guid.Data4[6],guid.Data4[7]);
}
inline void WriteIntVal(CString cfgPath,CString AppName,CString keyName,int val)
{
	CString str;
	str.Format(_T("%d"),val);
	::WritePrivateProfileString(AppName,keyName,str,cfgPath);
}

inline int CalcValidIntVal(int val,int maxVal,int minVal=0)
{
	if (val > maxVal)  val = maxVal;
	if (val < minVal)  val = minVal;
	return val;
}


