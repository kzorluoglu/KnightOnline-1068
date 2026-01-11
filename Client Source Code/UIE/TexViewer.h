#if !defined(AFX_TEXVIEWER_H__E7AB26A3_E760_421C_B0CE_E2118D0FB88A__INCLUDED_)
#define AFX_TEXVIEWER_H__E7AB26A3_E760_421C_B0CE_E2118D0FB88A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TexViewer.h : header file
//
#define UM_UPDATE_INFO	((WM_USER)+101)
/////////////////////////////////////////////////////////////////////////////
// CTexViewer window
#define MAX_IMAGETYPE		200
class CN3Texture;
class CTexViewer : public CWnd
{
// Construction
public:
	CTexViewer();

// Attributes
public:
	enum	eEDITMODE {EDITMODE_SELECT=0, EDITMODE_ZOOM, EDITMODE_HAND, NUM_EDITMODE};
	eEDITMODE		GetEditMode() const {return m_eEditMode;}
	CRect			GetSelectedRect() const {return m_rcSelectedRect;}
	CSize			GetTexSize() const {return m_TexSize;}
protected:
	CN3Texture*		m_pTex;				// texture
	CSize			m_TexSize;			// texture size
	float			m_fScale;			// ȭ�� ����
	CPoint			m_ptLeftTopInImage;	// �� ������ ���� ��ܿ� ���̴� texture�� ���� ��� ��ǥ
	eEDITMODE		m_eEditMode;		// ���� � ���� ��Ȳ����.(��, ��������, zoom in/out...)
	CRect			m_rcSelectedRect;	// ���õ� �簢��(image��ǥ ����)
	BOOL			m_bDrag;			// �巡�� ���ΰ�?
	CPen			m_WhiteDashPen;		// ��� ����
	CPoint			m_ptMouseOld;		// ���콺�� ���� ���� ���
	BOOL			m_bDeselect;		// deselect �� ���ΰ�?
	CPoint			m_ptClickOffset;	// ���� ������ �����̷��� �Ҷ� click���������
										// ����â�� Ŭ������ �����ǥ(���� ����lefttop 0,0 ����)(image pixel��ǥ��)
										
	enum	eDRAGTYPE {DRAGTYPE_NONE=0, DRAGTYPE_MOVE, DRAGTYPE_LEFT, DRAGTYPE_RIGHT,
					DRAGTYPE_TOP, DRAGTYPE_BOTTOM, DRAGTYPE_LEFTTOP, DRAGTYPE_RIGHTTOP,
					DRAGTYPE_LEFTBOTTOM, DRAGTYPE_RIGHTBOTTOM, DRAGTYPE_SELECT};
	eDRAGTYPE		m_eDragType;			// Drag ����

	// Ŀ��
	HCURSOR			m_hCursorSelect;
	HCURSOR			m_hCursorZoomIn;
	HCURSOR			m_hCursorZoomOut;
	HCURSOR			m_hCursorHand;

	HCURSOR			m_hCursorSizeAll;
	HCURSOR			m_hCursorSizeWE;
	HCURSOR			m_hCursorSizeNS;
	HCURSOR			m_hCursorSizeNWSE;
	HCURSOR			m_hCursorSizeNESW;

	// image type����
	int				m_iImageTypeCount;								// ���õǾ�� �� �̹��� ������ ��
	CRect			m_ImageRects[MAX_IMAGETYPE];					// m_iImageTypeCount��ŭ�� ImageRect
	int				m_iCurSelectedImage;							// ���� ���õ� ImageType
// Operations
public:
	void			Release();
	BOOL			Zoom(BOOL bZoomIn);		// in : Ȯ��, out : ���
	BOOL			Zoom(float fScale);		// f��� Zoom �ϱ� 
	void			Render();				// texture render�ϱ�
	void			SetTexture(LPCTSTR pszFName);	// texture ����
	eEDITMODE		SetEditMode(eEDITMODE eMode);	// mode �ٲٱ� (zoom, hand, select) �����ϸ� ���� mode�� �����ش�.
	void			SetLeftTopInImage(CPoint ptLeftTop);	// �̹����� ���� ��� ��ǥ �ٲٱ�
	BOOL			GetSelectedUVRect(struct __FLOAT_RECT* pFRect) const;	// ���� ���õ� UV��ǥ ���
	void			SetSelectedUVRect(const struct __FLOAT_RECT* pFRect);	// ���� ���õ� UV��ǥ �ֱ�

	// image type����
	void			SetImageTypeCount(int iCount) {m_iImageTypeCount = iCount;}	// image type ���� ���ϱ�
	BOOL			SetImageTypeIndex(int iIndex);	// zero base ���õ� image type���ϱ�
	CRect			GetImageRect(int iIndex);
	BOOL			AutoMultiRectSelect(BOOL bHorizon, CString& strErrMsg);
protected:
	BOOL			ScreenToImage(POINT	*pPoint);	// screen��ǥ�� image��ǥ��
	BOOL			ScreenToImage(RECT* pRect);		// screen��ǥ�� image��ǥ��
	BOOL			ImageToScreen(POINT	*pPoint);	// image��ǥ�� screen��ǥ��
	BOOL			ImageToScreen(RECT* pRect);		// image��ǥ�� screen��ǥ��
	eDRAGTYPE		CheckDragType(CRect rcSel, CPoint point);
	void			ProcessDrag(CPoint point);		// ���� ������ ��� ó���ϴ� ��ƾ
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTexViewer)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTexViewer();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTexViewer)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXVIEWER_H__E7AB26A3_E760_421C_B0CE_E2118D0FB88A__INCLUDED_)
