#include "stdafx.h"
#include "CommonFunc.h"
#include <WinCrypt.h>
#include <fstream>
#include <shellapi.h>
#include <io.h>
#include <direct.h>

using namespace std;

typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS_) (HANDLE, PBOOL);
LPFN_ISWOW64PROCESS_ fnIsWow64Process_;

typedef BOOL(APIENTRY *PWow64EnableWow64FsRedirection)(BOOL);


// ���� ��õ�ǰ·��
char* GetAppDir(char* szPath)
{
	char* ret = szPath;
	GetModuleFileNameA(NULL, szPath, MAX_PATH); // �õ���ǰִ���ļ����ļ���������·����
	*(strrchr(szPath, '\\')) = '\0';   // ɾ���ļ�����ֻ����Ŀ¼
	return ret;
}

void GetUserTempPath(char* szPath)
{
	GetTempPathA(MAX_PATH, szPath);
}

void GetUserDeskPath(char* szPath)
{
	//char path[255];
	SHGetSpecialFolderPathA(0, szPath, CSIDL_DESKTOPDIRECTORY, 0);
}

//խ�ַ�ת���ַ�
BOOL StringToWString(const std::string &str, std::wstring &wstr)
{
	int nLen = (int)str.length();
	wstr.resize(nLen, L' ');

	int nResult = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str.c_str(), nLen, (LPWSTR)wstr.c_str(), nLen);

	if (nResult == 0)
	{
		return FALSE;
	}

	return TRUE;
}


//���ַ�תխ�ַ�
BOOL WStringToString(const std::wstring &wstr, std::string &str)
{
	int nLen = (int)wstr.length();
	str.resize(nLen, ' ');

	int nResult = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wstr.c_str(), nLen, (LPSTR)str.c_str(), nLen, NULL, NULL);

	if (nResult == 0)
	{
		return FALSE;
	}

	return TRUE;
}



// ���� ����ļ����е��ļ�
void GetFiles(string path, vector<string>& files)
{
	//�ļ����  
	long   hFile = 0;
	//�ļ���Ϣ  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮  
			//�������,�����б�  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					GetFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}



bool CheckFile(std::string strFileName)
{
	if (CheckSytemNumber() == SYSTEM_32)
	{
		fstream fp;
		fp.open(strFileName, ios::in | ios::binary);
		if (!fp)
		{
			fp.close();
			return false;
		}

		fp.close();
	}

	if (CheckSytemNumber() == SYSTEM_64)
	{
		//PWow64EnableWow64FsRedirection pf = (PWow64EnableWow64FsRedirection)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "Wow64EnableWow64FsRedirection");

		//if (pf != NULL)
		{
			//if (pf(FALSE))
			{
				fstream fp;
				fp.open(strFileName, ios::in | ios::binary);
				if (!fp)
				{
					fp.close();
					return false;
				}

				fp.close();
				//if (FALSE == pf(TRUE))
				//{
				//	return false;
				//}
			}
		}
	}

	return true;
}


// ����ļ����Ƿ����
void CheckDir(std::string strDirPath)
{
	if (_access(strDirPath.c_str(), 0) == -1)
	{
		_mkdir(strDirPath.c_str());
	}
}

void InstallFile(const char* szPath)
{
	wstring strPath;
	StringToWString(szPath, strPath);

	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = strPath.c_str();
	ShExecInfo.lpParameters = L"/q";
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	DWORD dwExitCode;
	GetExitCodeProcess(ShExecInfo.hProcess, &dwExitCode);
	while (dwExitCode == STILL_ACTIVE)
	{
		Sleep((DWORD)5);
		GetExitCodeProcess(ShExecInfo.hProcess, &dwExitCode);
	}

	CloseHandle(ShExecInfo.hProcess);
}

SYSTEMVER GetSystemVersion()
{
	SYSTEM_INFO info;        //��SYSTEM_INFO�ṹ�ж�64λAMD������   
	GetSystemInfo(&info);    //����GetSystemInfo�������ṹ   
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (GetVersionEx((OSVERSIONINFO *)&os))
	{
		if (os.dwMajorVersion == 6 && os.dwMinorVersion == 2 && os.dwBuildNumber == 9200)
		{
			return WIN_10;
		} 
		else
		{
			if (os.dwMajorVersion == 5)
				return WIN_XP;
			if (os.dwMajorVersion == 6 && os.dwMinorVersion == 1)
				return WIN_7;
			if (os.dwMajorVersion == 6 && os.dwMinorVersion == 2)
				return WIN_8;
		}
	}
	
	return WIN_NULL;
}


VERSION CheckSytemNumber()
{
	BOOL bIsWow64 = FALSE;

	fnIsWow64Process_ = (LPFN_ISWOW64PROCESS_)GetProcAddress(
		GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

	if (NULL != fnIsWow64Process_)
	{
		fnIsWow64Process_(GetCurrentProcess(), &bIsWow64);
	}
	return bIsWow64 == TRUE ? SYSTEM_64 : SYSTEM_32;
}

bool DownloadUrl(const char* url, const char* strSaveFile)
{
	bool ret = false;

	CInternetSession Sess(_T("lpload"));

	Sess.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 5000); //5������ӳ�ʱ
	Sess.SetOption(INTERNET_OPTION_SEND_TIMEOUT, 5000); //5��ķ��ͳ�ʱ
	Sess.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 5000); //5��Ľ��ճ�ʱ
	Sess.SetOption(INTERNET_OPTION_DATA_SEND_TIMEOUT, 5000); //5��ķ��ͳ�ʱ
	Sess.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, 5000); //5��Ľ��ճ�ʱ

	DWORD dwFlag = INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD;


	CHttpFile* cFile = NULL;
	char *pBuf = NULL;
	int nBufLen = 0;

	do {
		try {
			CString str = url;
			cFile = (CHttpFile*)Sess.OpenURL((CString)url, 1, dwFlag);
			DWORD dwStatusCode;
			cFile->QueryInfoStatusCode(dwStatusCode);

			if (dwStatusCode == HTTP_STATUS_OK) {

				//��ѯ�ļ�����
				DWORD nLen = 0;
				cFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, nLen);

				//CString strFilename = GetFileName(url,TRUE);
				nBufLen = nLen;

				if (nLen <= 0) break;//
									 //����������ݻ���
				pBuf = (char*)malloc(nLen + 8);
				ZeroMemory(pBuf, nLen + 8);

				char *p = pBuf;
				while (nLen > 0) {
					//ÿ������8K
					int n = cFile->Read(p, nLen);
					//��������˳�ѭ��
					if (n <= 0) break;//
									  //���ջ������
					p += n;
					//ʣ�೤�ȵݼ�
					nLen -= n;
				}


				//���δ�������ж��˳�
				if (nLen != 0) break;

				//���ճɹ����浽�ļ�
				CFile file((CString)strSaveFile, CFile::modeCreate | CFile::modeWrite);
				file.Write(pBuf, nBufLen);
				file.Close();
				ret = true;
			}
		}
		catch (...) {
			int a = GetLastError();
			break;//
		}
	} while (0);


	//�ͷŻ���
	if (pBuf) {
		free(pBuf);
		pBuf = NULL;
		nBufLen = 0;
	}

	//�ر���������
	if (cFile) {
		cFile->Close();
		Sess.Close();
		delete cFile;
	}
	return ret;
}


void InstallFile(char* szPath)
{
	wstring strPath;
	StringToWString(szPath, strPath);

	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = strPath.c_str();
	ShExecInfo.lpParameters = L"/s";
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;
	bool bRet = ShellExecuteEx(&ShExecInfo);
	//WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	//DWORD dwExitCode;
	//GetExitCodeProcess(ShExecInfo.hProcess, &dwExitCode);
	//while (dwExitCode == STILL_ACTIVE)
	//{
	//	Sleep((DWORD)5);
	//	GetExitCodeProcess(ShExecInfo.hProcess, &dwExitCode);
	//}


	CloseHandle(ShExecInfo.hProcess);
}



char dec2hexChar(short int n) {
	if (0 <= n && n <= 9) {
		return char(short('0') + n);
	}
	else if (10 <= n && n <= 15) {
		return char(short('A') + n - 10);
	}
	else {
		return char(0);
	}
}

short int hexChar2dec(char c) {
	if ('0' <= c && c <= '9') {
		return short(c - '0');
	}
	else if ('a' <= c && c <= 'f') {
		return (short(c - 'a') + 10);
	}
	else if ('A' <= c && c <= 'F') {
		return (short(c - 'A') + 10);
	}
	else {
		return -1;
	}
}

string escapeURL(const string &URL)
{
	string result = "";
	for (unsigned int i = 0; i < URL.size(); i++) {
		char c = URL[i];
		if (
			('0' <= c && c <= '9') ||
			('a' <= c && c <= 'z') ||
			('A' <= c && c <= 'Z') ||
			c == '/' || c == '.'
			) {
			result += c;
		}
		else {
			int j = (short int)c;
			if (j < 0) {
				j += 256;
			}
			int i1, i0;
			i1 = j / 16;
			i0 = j - i1 * 16;
			result += '%';
			result += dec2hexChar(i1);
			result += dec2hexChar(i0);
		}
	}
	return result;
}

string deescapeURL(const string &URL) {
	string result = "";
	for (unsigned int i = 0; i < URL.size(); i++) {
		char c = URL[i];
		if (c != '%') {
			result += c;
		}
		else {
			char c1 = URL[++i];
			char c0 = URL[++i];
			int num = 0;
			num += hexChar2dec(c1) * 16 + hexChar2dec(c0);
			result += char(num);
		}
	}
	return result;
}
