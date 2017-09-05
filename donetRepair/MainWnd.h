// MainWnd.h

#pragma once
//#include "DownloadFile.h"
#include "CommonFunc.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "ChildWnd.h"
#include "EasyLog.h"

using namespace std;

#define TIMER_ID_PROGRESS (1000)
#define MSG_HIDE_WINDOW WM_USER + 101

struct stInstaller
{
	bool bCheck;
	bool bInstall;
	std::string strCaption;   //��ʾ�ı�����
	std::string strUrl;       //�ļ�����������
	std::string strFileName;  //�洢�ļ�������
	CButtonUI *pCheckBox;
	CLabelUI *pLabel;
};

struct Timer
{
	bool bStart;
	int  nLapse;
};

struct stUserSelect
{
	int nDownloadIndex;
	int nDownloadFileSize;
	bool bDownload;
	bool bOver;
	bool bOptSuc;  //���������Ƿ����
	Timer tm;      //��ʱ��
	std::string strUrl;
	std::string strFilePath;
	CButtonUI *ptopSplit; //�ָ���
	CLabelUI *pnetText; //net �ı�����
	CLabelUI *psysText; //ϵͳ֧�� �ı�����

	CButtonUI *pDownnload; //���ذ�ť
	CButtonUI *pOpenFile; //���ļ���ť 
	CButtonUI *pOpenDir;  //���ļ���
	CProgressUI *pProgress; //���ؽ�����
	CLabelUI *plabelOver; //�������
	CLabelUI *plabelOverPic; //������ɵ�ͼƬ 
	CLabelUI *plabelSpeed; //�����ٶ�
	CButtonUI *pUrlTip;  //Url��ʾ

	CLabelUI *plabelPercent; //�ٷֺ�
	CLabelUI *plabelHund;    //��λ
	CLabelUI *plabelDec;     //ʮλ
	CLabelUI *plabelUnit;    //��λ
};

struct stNumber
{
	int nHund = 0;   //��λ
	int nDec = 0;    //ʮλ
	int nUnit = 0;   //��λ
};

struct stRect
{
	RECT rc1;
	RECT rc2;
	RECT rc3;
};

class CMainWnd : public WindowImplBase
{
public:
	CMainWnd();
	CDuiString GetSkinFile();
	LPCTSTR GetWindowClassName() const ;
	
	void InitWindow();
	void Notify(TNotifyUI& msg);
	void OnFinalMessage(HWND hWnd);

	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	void Init();
	void ShowFirstUI(bool bShow);
	void ShowSecondUI(bool bShow);
	void UpdataDownloadBtnShow(int nIndex); //�������ذ�ť����ʾ
	void UpdataDownloadProShow(int nIndex); //���½�������ʾ
	void UpdataOverShow(int nIndex); //����������ɺ����ʾ
	void ShowPauseWnd();
	void ShowNoFileWnd();
	void CleanFile();
	bool CheckNet();
	void GetInstallerHttpInfo(_TCHAR* url, vector<stInstaller>& vecIns);  //��ȡhttp�����Ϣ

	void EnableWnd();   //ʹ�ؼ��
	void UnEnableWnd(); //ʹ�ؼ����ܻ


protected:
	stRect m_rcRect;

	CVerticalLayoutUI *m_pLayout;
	CButtonUI   *m_pBtnNext;

	CLabelUI    *m_pLabelLogo;
	CButtonUI   *m_pBtnClose1;
	CButtonUI   *m_pBtnClose2;
	CLabelUI    *m_pLabelTopBkTip;
	CButtonUI   *m_pBtnMin;
	CLabelUI    *m_pLabelTopTextBk;
	
	CLabelUI   *m_pLabeltopText1;
	CLabelUI   *m_pLabeltopText2;
	CLabelUI   *m_pLabeltopText3;
	CLabelUI   *m_pLabeltopText4;

	CButtonUI   *m_pBtntopSplit1;
	CButtonUI   *m_pBtntopSplit2;
	CButtonUI   *m_pBtntopSplit3;
	CButtonUI   *m_pBtntopSplit4;
	CButtonUI   *m_pBtntopSplit5;
	CButtonUI   *m_pBtntopSplit6;

	CLabelUI   *m_pLabelnetText1;
	CLabelUI   *m_pLabelnetText2;
	CLabelUI   *m_pLabelnetText3;
	CLabelUI   *m_pLabelnetText4;
	CLabelUI   *m_pLabelnetText5;
	CLabelUI   *m_pLabelnetText6;

	CLabelUI   *m_pLabelsysText1;
	CLabelUI   *m_pLabelsysText2;
	CLabelUI   *m_pLabelsysText3;
	CLabelUI   *m_pLabelsysText4;
	CLabelUI   *m_pLabelsysText5;
	CLabelUI   *m_pLabelsysText6;

	CLabelUI   *m_pLabeltipLast1;
	CLabelUI   *m_pLabeltipPeiTu;
	CLabelUI   *m_pLabeltipLast2;
	CLabelUI   *m_pLabeltipLast3;
	CLabelUI   *m_pLabeltipLast4;

	CButtonUI   *m_pBtnDownload1;
	CButtonUI   *m_pBtnDownload2;
	CButtonUI   *m_pBtnDownload3;
	CButtonUI   *m_pBtnDownload4;
	CButtonUI   *m_pBtnDownload5;
	CButtonUI   *m_pBtnDownload6;

	CButtonUI   *m_pLabeltipText1;
	CButtonUI   *m_pLabeltipText2;
	CButtonUI   *m_pLabeltipText3;
	CButtonUI   *m_pLabeltipText4;
	CButtonUI   *m_pLabeltipText5;
	CButtonUI   *m_pLabeltipText6;

	CButtonUI   *m_pBtnOpenFile1;
	CButtonUI   *m_pBtnOpenFile2;
	CButtonUI   *m_pBtnOpenFile3;
	CButtonUI   *m_pBtnOpenFile4;
	CButtonUI   *m_pBtnOpenFile5;
	CButtonUI   *m_pBtnOpenFile6;

	CButtonUI   *m_pBtnOpenDir1;
	CButtonUI   *m_pBtnOpenDir2;
	CButtonUI   *m_pBtnOpenDir3;
	CButtonUI   *m_pBtnOpenDir4;
	CButtonUI   *m_pBtnOpenDir5;
	CButtonUI   *m_pBtnOpenDir6;

	CProgressUI *m_pProgress1;
	CProgressUI *m_pProgress2;
	CProgressUI *m_pProgress3;
	CProgressUI *m_pProgress4;
	CProgressUI *m_pProgress5;
	CProgressUI *m_pProgress6;

	CLabelUI   *m_pLabelSpeed1;
	CLabelUI   *m_pLabelSpeed2;
	CLabelUI   *m_pLabelSpeed3;
	CLabelUI   *m_pLabelSpeed4;
	CLabelUI   *m_pLabelSpeed5;
	CLabelUI   *m_pLabelSpeed6;

	CLabelUI   *m_pLabelOver1;
	CLabelUI   *m_pLabelOver2;
	CLabelUI   *m_pLabelOver3;
	CLabelUI   *m_pLabelOver4;
	CLabelUI   *m_pLabelOver5;
	CLabelUI   *m_pLabelOver6;

	CLabelUI   *m_pLabelOverPic1;
	CLabelUI   *m_pLabelOverPic2;
	CLabelUI   *m_pLabelOverPic3;
	CLabelUI   *m_pLabelOverPic4;
	CLabelUI   *m_pLabelOverPic5;
	CLabelUI   *m_pLabelOverPic6;

	CLabelUI   *m_pLabelPercent1;
	CLabelUI   *m_pLabelPercent2;
	CLabelUI   *m_pLabelPercent3;
	CLabelUI   *m_pLabelPercent4;
	CLabelUI   *m_pLabelPercent5;
	CLabelUI   *m_pLabelPercent6;

	CLabelUI   *m_pLabelHund1;
	CLabelUI   *m_pLabelHund2;
	CLabelUI   *m_pLabelHund3;
	CLabelUI   *m_pLabelHund4;
	CLabelUI   *m_pLabelHund5;
	CLabelUI   *m_pLabelHund6;

	CLabelUI   *m_pLabelDec1;
	CLabelUI   *m_pLabelDec2;
	CLabelUI   *m_pLabelDec3;
	CLabelUI   *m_pLabelDec4;
	CLabelUI   *m_pLabelDec5;
	CLabelUI   *m_pLabelDec6;

	CLabelUI   *m_pLabelUnit1;
	CLabelUI   *m_pLabelUnit2;
	CLabelUI   *m_pLabelUnit3;
	CLabelUI   *m_pLabelUnit4;
	CLabelUI   *m_pLabelUnit5;
	CLabelUI   *m_pLabelUnit6;

	ChildWnd   *m_pChildWnd;

	CButtonUI *m_pcheckBox1;
	CButtonUI *m_pcheckBox2;
	CButtonUI *m_pcheckBox3;

	CButtonUI* m_plabelIns1;
	CButtonUI* m_plabelIns2;
	CButtonUI* m_plabelIns3;
};

extern int g_nWndStatus;