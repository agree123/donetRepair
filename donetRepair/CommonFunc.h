#pragma once

//���½�������ʾ
#define  MSG_PROGRESS_SHOW        WM_USER + 101

enum SYSTEMVER
{
	WIN_NULL = 0,
	WIN_XP,
	WIN_7,
	WIN_8,
	WIN_10
};

enum VERSION
{
	SYSTEM_32 = 0,
	SYSTEM_64
};



// ���� ��õ�ǰ·��
char* GetAppDir(char* szPath);

// ���� ��õ�ǰ�û���ʱĿ¼
void GetUserTempPath(char* szPath);

// ���� ��ȡ��ǰ�û�����·��
void GetUserDeskPath(char* szPath);

// խ�ַ�ת���ַ�
BOOL StringToWString(const std::string &str, std::wstring &wstr);

// ���ַ�תխ�ַ�
BOOL WStringToString(const std::wstring &wstr, std::string &str);

// ��ȡ�ļ����е��ļ�
void GetFiles(string path, vector<string>& files);

// ����ļ��Ƿ����
bool CheckFile(std::string strFileName);

// ����ļ����Ƿ����
void CheckDir(std::string strDirPath);

void InstallFile(const char* szPath);

SYSTEMVER GetSystemVersion();
VERSION CheckSytemNumber();

bool DownloadUrl(const char* url, const char* strSaveFile);

void InstallFile(char* szPath);

// url  ����ͽ���
char dec2hexChar(short int n);
short int hexChar2dec(char c);
string escapeURL(const string &URL);
string deescapeURL(const string &URL);