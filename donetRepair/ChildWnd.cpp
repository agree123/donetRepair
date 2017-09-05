#include "stdafx.h"
#include "ChildWnd.h"
#include "MainWnd.h"



ChildWnd::ChildWnd()
{
}


ChildWnd::~ChildWnd()
{
}

DuiLib::CDuiString ChildWnd::GetSkinFile()
{
	try
	{
		return CDuiString(_T("DlgChild.xml"));
	}
	catch (...)
	{
		throw "CMainWnd::GetSkinFile";
	}
}

LPCTSTR ChildWnd::GetWindowClassName() const
{
	try
	{
		return _T(".NETÐÞ¸´¹¤¾ß");
	}
	catch (...)
	{
		throw "CChildWnd::GetWindowClassName";
	}
}

void ChildWnd::InitWindow()
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


void ChildWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("click"))
	{
		if (msg.pSender == m_pcloseBtn)
		{
			ShowWindow(SW_HIDE);
			g_nWndStatus = 1;
		}

		if (msg.pSender == m_pfirstBtn1)
		{
			this->Close();
		}

		if (msg.pSender == m_pfirstBtn2)
		{
			ShowWindow(SW_HIDE);
			g_nWndStatus = 1;
		}

		if (msg.pSender == m_psecondBtn1)
		{
			ShowWindow(SW_HIDE);
			g_nWndStatus = 1;
		}
	}

	WindowImplBase::Notify(msg);
}


void ChildWnd::OnFinalMessage(HWND hWnd)
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


LRESULT ChildWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes = 0;
	CPoint* p = NULL;

	switch (uMsg)
	{
	case WM_TIMER:  break;
	case WM_NCLBUTTONDBLCLK: break;
	case WM_CLOSE:  break;
	default: break;
	}

	return lRes;
}

void ChildWnd::Init()
{
	m_pcloseBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnClose"))); 

	m_pfirstLabell = static_cast<CLabelUI*>(m_pm.FindControl(_T("firstLabell")));
	m_pfirstLabel2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("firstLabel2")));
	m_pfirstBtn1 = static_cast<CButtonUI*>(m_pm.FindControl(_T("firstBtn1")));
	m_pfirstBtn2 = static_cast<CButtonUI*>(m_pm.FindControl(_T("firstBtn2")));

	m_psecondLabel1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("secondLabell")));
	m_psecondLabel2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("secondLabel2")));
	m_psecondLabel3 = static_cast<CLabelUI*>(m_pm.FindControl(_T("secondLabel3")));
	m_psecondBtn1 = static_cast<CButtonUI*>(m_pm.FindControl(_T("secondBtn1")));

}

void ChildWnd::FitstBkShow(bool bShow)
{
	m_pfirstLabell->SetVisible(bShow);
	m_pfirstLabel2->SetVisible(bShow);
	m_pfirstBtn1->SetVisible(bShow);
	m_pfirstBtn2->SetVisible(bShow);
}

void ChildWnd::SecondBkShow(bool bShow)
{
	m_psecondLabel1->SetVisible(bShow);
	m_psecondLabel2->SetVisible(bShow);
	m_psecondLabel3->SetVisible(bShow);
	m_psecondBtn1->SetVisible(bShow);
}

void ChildWnd::CloseWnd()
{
	this->Close();
}
