// ADMonSetup.cpp : Defines the entry point for the application.
//

#include "StdAfx.h"
#include "donetRepair.h"
#include "MainWnd.h"

//#include <GdiPlus.h>
//#pragma comment( lib, "GdiPlus.lib" )
//using namespace Gdiplus;

//----------------------------------------------------------------------------------------------------------------------------------------

//������ڴ����øú��������ڴ�й¶���
//�����˳�ǰ����_CrtDumpMemoryLeaks();�����ɲ鿴�ڴ�й¶��ID�ţ���ID�ſ���ΪBreakpoint�Ĳ���������ʱ��������Ӧλ���Զ��ϵ�
inline  void  EnableMemLeakCheck(int Breakpoint = 0){
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)  |  _CRTDBG_LEAK_CHECK_DF);
	if(Breakpoint)
		_CrtSetBreakAlloc ( 1912 );
}

#ifdef _DEBUG
#define  new   new(_NORMAL_BLOCK, __FILE__, __LINE__) 
#endif 
#define _CRT_SECURE_NO_DEPRECATE

//----------------------------------------------------------------------------------------------------------------------------------------

void InitResource()
{	
	// ��Դ����
	CPaintManagerUI::SetResourceType(UILIB_ZIPRESOURCE);
	// ��Դ·��
	CDuiString strResourcePath = CPaintManagerUI::GetInstancePath();
	// ������Դ
	switch(CPaintManagerUI::GetResourceType())
	{
	case UILIB_FILE:
		{
			strResourcePath += _T("skin\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			// ������Դ������
			CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
			break;
		}
	case UILIB_RESOURCE:
		{
			strResourcePath += _T("skin\\ADMonSetup\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			// ������Դ������
			CResourceManager::GetInstance()->LoadResource(_T("IDR_RES"), _T("xml"));
			break;
		}
	case UILIB_ZIP:
		{
			strResourcePath += _T("skin\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			CPaintManagerUI::SetResourceZip(_T("ADMonSetup.zip"), true);
			// ������Դ������
			CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
			break;
		}
	case UILIB_ZIPRESOURCE:
		{
			HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), _T("IDR_ZIPRES1"), _T("ZIPRES"));
			if( hResource != NULL ) {
				DWORD dwSize = 0;
				HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
				if( hGlobal != NULL ) {
					dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
					if( dwSize > 0 ) {
						CPaintManagerUI::SetResourceZip((LPBYTE)::LockResource(hGlobal), dwSize);
						// ������Դ������
						CResourceManager::GetInstance()->LoadResource(_T("DlgMain.xml"), NULL);
					}
				}
				::FreeResource(hResource);
			}
		}
		break;
	}
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	EnableMemLeakCheck();


	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

	HWND hWnd = FindWindow(NULL, _T(".NET���ع���"));
	if (hWnd)
		return 0;

	CPaintManagerUI::SetInstance(hInstance);
	InitResource();


	CMainWnd* pFrame = new CMainWnd();
	if(pFrame == NULL)
		return 0;

	pFrame->Create(NULL,_T(".NET���ع���"), UI_WNDSTYLE_EX_FRAME | WS_EX_WINDOWEDGE | WS_EX_ACCEPTFILES, 0L, 0, 0, 536, 355);
	pFrame->CenterWindow();
	::ShowWindow(*pFrame, SW_SHOW);
	pFrame->SetIcon(IDI_ICON2);
	CPaintManagerUI::MessageLoop();

	::CoUninitialize();

	_CrtDumpMemoryLeaks();
	return 0;
}
