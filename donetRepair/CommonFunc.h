#pragma once

//更新进度条显示
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



// 功能 获得当前路径
char* GetAppDir(char* szPath);

// 功能 获得当前用户临时目录
void GetUserTempPath(char* szPath);

// 功能 获取当前用户桌面路径
void GetUserDeskPath(char* szPath);

// 窄字符转宽字符
BOOL StringToWString(const std::string &str, std::wstring &wstr);

// 宽字符转窄字符
BOOL WStringToString(const std::wstring &wstr, std::string &str);

// 获取文件夹中的文件
void GetFiles(string path, vector<string>& files);

// 检查文件是否存在
bool CheckFile(std::string strFileName);

// 检测文件夹是否存在
void CheckDir(std::string strDirPath);

void InstallFile(const char* szPath);

SYSTEMVER GetSystemVersion();
VERSION CheckSytemNumber();

bool DownloadUrl(const char* url, const char* strSaveFile);

void InstallFile(char* szPath);

// url  编码和解码
char dec2hexChar(short int n);
short int hexChar2dec(char c);
string escapeURL(const string &URL);
string deescapeURL(const string &URL);