#pragma once
class ChildWnd : public WindowImplBase
{
public:
	ChildWnd();
	~ChildWnd();

	CDuiString GetSkinFile();
	LPCTSTR GetWindowClassName() const;

	void InitWindow();
	void Notify(TNotifyUI& msg);
	void OnFinalMessage(HWND hWnd);

	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void Init();
	void FitstBkShow(bool bShow);
	void SecondBkShow(bool bShow);
	void CloseWnd();

protected:
	CButtonUI  *m_pcloseBtn;

	CLabelUI   *m_pfirstLabell;
	CLabelUI   *m_pfirstLabel2;
	CButtonUI  *m_pfirstBtn1;
	CButtonUI  *m_pfirstBtn2;

	CLabelUI   *m_psecondLabel1;
	CLabelUI   *m_psecondLabel2;
	CLabelUI   *m_psecondLabel3;
	CButtonUI  *m_psecondBtn1;
};

