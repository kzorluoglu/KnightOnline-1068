// ProgressBar.cpp : implementation file
//

#include "stdafx.h"
#include "n3me.h"
#include "ProgressBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressBar

CProgressBar::CProgressBar()
{
	m_nSize = 0;
}

CProgressBar::~CProgressBar()
{
	Clear();
}


BEGIN_MESSAGE_MAP(CProgressBar, CProgressCtrl)
	//{{AFX_MSG_MAP(CProgressBar)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressBar message handlers

BOOL CProgressBar::Create(LPCTSTR strMessage, int nSize, int MaxValue)
{
	if(NULL == m_hWnd)
	{
		// ���¹ٸ� ����
		CStatusBar * pStatusBar = GetStatusBar();
		if (pStatusBar == NULL) return FALSE;

		// ���¹� ���� ���α׷��� ��Ʈ�ѻ���
		if(!CProgressCtrl::Create(WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), pStatusBar, 1)) return FALSE;
	}

	// ���α׷��� ��Ʈ���� ������ ���� ����
	SetRange(0, MaxValue);
	SetStep(1);

	m_strMessage = strMessage;
	m_nSize = nSize;

	// ��ġ�� ũ�� ����
	Resize();
	return TRUE;
}

CStatusBar* CProgressBar::GetStatusBar()
{
	CFrameWnd* pFrame = (CFrameWnd*)AfxGetMainWnd();
	if (pFrame == NULL || pFrame->IsKindOf(RUNTIME_CLASS(CFrameWnd)) == FALSE) return NULL;

	CStatusBar* pBar = (CStatusBar*)pFrame->GetMessageBar();
	if(pBar == NULL || pBar->IsKindOf(RUNTIME_CLASS(CStatusBar)) == FALSE) return NULL;

	return pBar;
}

void CProgressBar::Resize()
{
	CStatusBar* pStatusBar = GetStatusBar();
	if (pStatusBar == NULL) return;

	// �ؽ�Ʈ ���
	if (::IsWindow(m_hWnd) && IsWindowVisible())
	{
		pStatusBar->SetWindowText(m_strMessage);
		pStatusBar->UpdateWindow();
	}

	// �ؽ�Ʈ�� �����ϴ� ���� ���
	CClientDC dc(pStatusBar);
	CFont* pOldFont = dc.SelectObject(pStatusBar->GetFont());
	CSize size = dc.GetTextExtent(m_strMessage);
	int margin = dc.GetTextExtent(_T(" ")).cx*2;
	dc.SelectObject(pOldFont);

	// ���α׷��� ��Ʈ���� �׷��� ���� ���
	CRect rc;
	pStatusBar->GetItemRect(0, rc);
	rc.left = size.cx + 2*margin;
	rc.right = rc.left + (rc.right-rc.left)*m_nSize/100;
	if (rc.right < rc.left) rc.right = rc.left;

	// ���¹� ���Ϸ� 10%�� ������ ��
	int Height = rc.bottom - rc.top;
	rc.bottom -= Height/10;
	rc.top += Height/10;

	// ���α׷��� ��Ʈ���� ��ġ�� ũ�⸦ ������
	if (::IsWindow(m_hWnd) && (rc != m_Rect)) MoveWindow(&rc);
	m_Rect = rc;
}

void CProgressBar::Clear()
{
	ModifyStyle(WS_VISIBLE, 0);
	CString str;
	str.LoadString(AFX_IDS_IDLEMESSAGE);
	CStatusBar* pStatusBar = GetStatusBar();
	if(pStatusBar) pStatusBar->SetWindowText(str);
}


BOOL CProgressBar::OnEraseBkgnd(CDC* pDC) 
{
	Resize();
	
	return CProgressCtrl::OnEraseBkgnd(pDC);
}
