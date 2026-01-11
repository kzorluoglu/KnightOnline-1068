// N3UIBase.h: interface for the CN3UIBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3UI_H__5F1578F8_8476_4647_8C53_E22A5F6184FE__INCLUDED_)
#define AFX_N3UI_H__5F1578F8_8476_4647_8C53_E22A5F6184FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786)

#include "N3BaseFileAccess.h"
#include "N3UIDef.h"
#include <DInput.h>
#include <list>
#include <set>
#include <string>

class CN3UIBase;
using namespace std;
typedef list<CN3UIBase*>			UIList;
typedef UIList::iterator			UIListItor;
typedef UIList::const_iterator		UIListItorConst;
typedef UIList::reverse_iterator	UIListReverseItor;

class CN3UIImage;
class CN3UIString;
class CN3UIButton;
class CN3UIStatic;
class CN3UIProgress;
class CN3UITrackBar;
class CN3UIScrollBar;
class CN3UIEdit;
class CN3UITooltip;
class CN3UIArea;
class CN3SndObj;
class CN3UIList;

class CN3UIBase : public CN3BaseFileAccess
{
//friend class CN3IME;
#ifdef _N3TOOL
friend class CN3UIBase;
friend class CHierarchyView;	// ������ child list�� �����ϱ� ���ؼ�.
friend class CPropertyView;	// ������ �� �������� �����ϱ� ���ؼ� 
friend class CUIEView;	// ������ child list�� �����ϱ� ���ؼ�.
#endif

public:
	std::string m_szID;			// UI id
	std::string	m_szToolTip;	// tooltip text
	void		SetID(LPCTSTR pszID) {m_szID = pszID;}
	const std::string GetID() const {return m_szID;}
	void		SetTooltipText(LPCTSTR pszTT) {m_szToolTip = pszTT;}

	static CN3UITooltip*	s_pTooltipCtrl;		// tool tip
	
	CN3UIBase*	m_pChildUI;		// UI �μ��� �ƴ϶� �ٸ� UI�� �ڽ����� ���´�..
	CN3UIBase*	m_pParentUI;

protected:
	static std::string	s_szStringTmp;		// �ӽ� ���ڿ�.. �����͸� �ѱ�� ���ؼ��̴�..

	int			m_iChildID;
	CN3UIBase*	m_pParent;		// parent pointer
	UIList		m_Children;		// children pointer list
	eUI_TYPE	m_eType;		// UI Type - button, image .....
	eUI_STATE	m_eState;		// UI state
	DWORD		m_dwStyle;		// style
	DWORD		m_dwReserved;	// ��Ÿ �ӽ÷� �ְ� ���� ������ ������ �ȴ�.

	RECT		m_rcRegion;		// UI - screen coordinates (screen : main window client area) ���� : �θ� ���� �����ǥ�� �ƴϴ�.
	RECT		m_rcMovable;	// UI�� �巡�� �Ͽ� �����̰� �� �� �ִ� ���� - (screen : main window client area)           ~~~~~~~

	bool		m_bVisible;		// ȭ�鿡 ���̴°� (�θ� ������ ������ �ڽĵ��� render ���� �ʴ´�.)
	CN3SndObj*	m_pSnd_OpenUI;	// UI�� ȭ�鿡 ���̴� ���� ���� �Ҹ�
	CN3SndObj*	m_pSnd_CloseUI;	// UI�� ȭ�鿡�� ������� ���� ���� �Ҹ�

	static CN3UIEdit*		s_pFocusedEdit;		// ���� ��Ŀ���� ������ �ִ� Edit, NULL�̸� �ƹ��� ��Ŀ���� ������ ���� �ʴ�.
	
public:
	CN3UIBase();
	virtual ~CN3UIBase();

// Attributes
public:
	eUI_TYPE		UIType() const { return m_eType; }
	eUI_STATE 		GetState() const { return m_eState; }
	bool			IsVisible()	const { return m_bVisible; }
	RECT			GetRegion() const { return m_rcRegion; }
	void			SetMoveRect(const RECT& Rect) { m_rcMovable = Rect; }
	RECT			GetMoveRect() { return m_rcMovable; }
	void			SetReserved(DWORD dwReserved) {m_dwReserved = dwReserved;}
	DWORD			GetReserved() const {return m_dwReserved;}
	CN3UIBase*		GetParent() const {return m_pParent;}
	static CN3UIEdit*	GetFocusedEdit() {return s_pFocusedEdit;}
	static CN3UITooltip*	GetTooltipCtrl() {return s_pTooltipCtrl;}
	DWORD			GetStyle()	{return m_dwStyle;}

	void			SetUIType(eUI_TYPE eUIType) { m_eType = eUIType; }	// by ecli666 ���� ��� �ֱ� �����Ƽ�.. ^^
// Operations
public:	
	bool			IsIn(int x, int y);
	void			AddChild(CN3UIBase* pChild) { m_Children.push_front(pChild); }
	void			RemoveChild(CN3UIBase* pChild); // �ڽ� ����Ʈ���� �����͸� �������� ������ delete ������ �ʴ´�.
	void			SetParent(CN3UIBase* pParent);	// �θ� �ٲ۴�.
	int				GetWidth() { return m_rcRegion.right - m_rcRegion.left; }
	int				GetHeight() { return m_rcRegion.bottom - m_rcRegion.top; }
	POINT			GetPos() const;
	virtual void	SetPos(int x, int y);	// ��ġ ����(chilren�� ��ġ�� ���� �ٲپ��ش�.) ���������� MoveOffset�Լ��� �θ���.
	void			SetPosCenter();	// ȭ�� ������� ���߾��ش�..(chilren�� ��ġ�� ���� �ٲپ��ش�.) ���������� MoveOffset�Լ��� �θ���.
	CN3UIBase*		GetChildByID(const std::string& szID);

	virtual void	SetRegion(const RECT& pRect) { m_rcRegion = pRect; }	// ���� ����
	virtual BOOL	MoveOffset(int iOffsetX, int iOffsetY);	// offset��ŭ �̵����ش�.(region, children, move rect �̵�)
	virtual void	SetSize(int iWidth, int iHeight);	// ũ�� ����
	virtual void	SetState(eUI_STATE eState) { m_eState = eState; }
	virtual void	SetStyle(DWORD dwStyle) {m_dwStyle = dwStyle;}	// style����
	virtual void	SetVisible(bool bVisible);

	virtual void	SetVisibleWithNoSound(bool bVisible, bool bWork = false, bool bReFocus = false);

	virtual void	CallBackProc(int iID, DWORD dwFlag);
	virtual void	ShowWindow(int iID = -1, CN3UIBase* pParent = NULL);
	virtual bool	Load(HANDLE hFile);
	virtual bool	ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg); // �޽����� �޴´�.. ������, msg
	virtual DWORD	MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);
	virtual void	Tick();
	virtual void	Render();
	virtual void	Release(); // �ڽ� �����ͱ��� delete �Ѵ�..
	virtual void	Init(CN3UIBase* pParent);
	virtual bool	OnKeyPress(int iKey) { return false; }
	virtual bool	OnKeyPressed(int iKey) { return false; }

	static	bool	EnableTooltip(const std::string& szFN);	// tooltip UI�� �ʱ�ȭ ���ش�.
	static	void	DestroyTooltip();	// tooltip ui�� ���õ� ���� �������ش�.

	int				GetChildrenCount() { return m_Children.size(); }
	CN3UIBase*		GetChildByIndex(int iIndex)
	{
		if(iIndex < 0 || iIndex >= m_Children.size()) return NULL;
		UIListItor it = m_Children.begin();
		for(int i = 0; i < iIndex; it++, i++);
		return *it;
	}
	virtual void	operator = (const CN3UIBase& other);

protected:

#ifdef _N3TOOL
public:
	virtual bool	Save(HANDLE hFile);
	virtual void	ChangeImagePath(const std::string& szPathOld, const std::string& szPathNew);
	virtual void	ChangeFont(const std::string& szFont);
	virtual void	GatherImageFileName(std::set<std::string>& setImgFile);
	
	void			ResizeAutomaticalyByChild();
	int				IsMyChild(CN3UIBase* pUI);
	bool			SwapChild(CN3UIBase* pChild1, CN3UIBase* pChild2);
	
	bool			MoveToLower(CN3UIBase* pChild);
	bool			MoveToUpper(CN3UIBase* pChild);
	bool			MoveToLowest(CN3UIBase* pChild);
	bool			MoveToHighest(CN3UIBase* pChild);
	
	void			ArrangeZOrder();
	RECT			GetMoveRect() const {return m_rcMovable;}
	void			SetMoveRect(RECT rc) {m_rcMovable = rc;}
	void			SetSndOpen(const std::string& strFileName);
	void			SetSndClose(const std::string& strFileName);
	std::string		GetSndFName_OpenUI() const;
	std::string		GetSndFName_CloseUI() const;

	virtual bool	ReplaceAllTextures(const std::string& strFind, const std::string& strReplace);
#endif
};


#endif // !defined(AFX_N3UI_H__5F1578F8_8476_4647_8C53_E22A5F6184FE__INCLUDED_)
