// N3UIEdit.h: interface for the CN3UIEdit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3UIEDIT_H__91BCC181_3AA5_4CD4_8D33_06D5D96F4F26__INCLUDED_)
#define AFX_N3UIEDIT_H__91BCC181_3AA5_4CD4_8D33_06D5D96F4F26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3UIStatic.h"

class CN3UIEdit : public CN3UIStatic  
{
//	friend class CN3IME;
public:
	CN3UIEdit();
	virtual ~CN3UIEdit();
// class
protected:
	class CN3Caret
	{
	public:
		CN3Caret();
		virtual ~CN3Caret();
		void	SetPos(int x, int y);
		void	MoveOffset(int iOffsetX, int iOffsetY);
		void	SetSize(int iSize);
		void	SetColor(D3DCOLOR color);
		void	Render(LPDIRECT3DDEVICE9 lpD3DDev);
		void	InitFlckering();		// ������ �ʱ�ȭ..
		BOOL	m_bVisible;				// ���̳�
	protected:
		int		m_iSize;				// caret�� pixel ũ��
		float	m_fFlickerTimePrev;	// �����̱� ���� �ð�..
		bool	m_bFlickerStatus;
//		POINT	m_ptPos;				// caret�� pixel ��ǥ
		__VertexTransformedColor	m_pVB[2];	// vertex ����
	};

// Attributes
public:
/*
	static bool		AddEdit(CN3UIEdit* pEdit);
	static bool		DeleteEdit(CN3UIEdit* pEdit);
	static CN3UIEdit* GetEdit(HWND hWnd);
	static HWND		s_hWndParent;

	LRESULT APIENTRY EditWndProcFuncPointer(HWND hWnd, WORD Message, WPARAM wParam, LPARAM lParam);
	static LRESULT	APIENTRY EditWndProc(HWND hWnd, WORD Message, WPARAM wParam, LPARAM lParam);
	static			void SetParentWindowHwnd(HWND hWnd) { s_hWndParent = hWnd; }
*/
	static HWND		s_hWndEdit, s_hWndParent;
	static WNDPROC	s_lpfnEditProc;
	static char		s_szBuffTmp[512];

	static void				SetImeStatus(POINT ptPos, bool bOpen);
	static BOOL				CreateEditWindow(HWND hParent, RECT rect);
	static LRESULT APIENTRY EditWndProc(HWND hWnd, WORD Message, WPARAM wParam, LPARAM lParam);
	static void				UpdateTextFromEditCtrl();
	static void				UpdateCaretPosFromEditCtrl();

protected:
	static CN3Caret		s_Caret;
	UINT				m_nCaretPos;		// ���� ������ġ(byte����)
	int					m_iCompLength;		// ���� �������� ������ byte�� 0�̸� �������� �ƴϴ�.
	int					m_iMaxStrLen;		// ���� �ִ� �۾��� �ִ� ����
	std::string			m_szPassword;		// password buffer

	CN3SndObj*			m_pSnd_Typing;		// Ÿ���� �� �� ���� �Ҹ�
// Operations
public:
	virtual const std::string&	GetString();
	virtual void				SetString(const std::string& szString);

	virtual bool		Load(HANDLE hFile);
	virtual void		Render();
	virtual void		Release();
	virtual void		SetVisible(bool bVisible);
	virtual DWORD		MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);
	virtual BOOL		MoveOffset(int iOffsetX, int iOffsetY);		// ��ġ ����(chilren�� ��ġ�� ���� �ٲپ��ش�. caret��ġ�� ���� �ٲپ���.)
	void				KillFocus();			// ��Ŀ���� ���ش�.
	bool				SetFocus();				// ��Ŀ���� �ش�.
	bool				HaveFocus() const {return (this == s_pFocusedEdit);}
	void				SetCaretPos(UINT nPos);	//���° ����Ʈ�� �ִ��� �����Ѵ�.
	void				SetMaxString(int iMax);		// �ִ� �۾� ���� �����ش�.
protected:
	BOOL				IsHangulMiddleByte( const char* lpszStr, int iPos );	// �ѱ��� 2��° ����Ʈ �����ΰ�?

#ifdef _N3TOOL
public:	
	virtual void		operator = (const CN3UIEdit& other);
	virtual bool		Save(HANDLE hFile);
	void				SetSndTyping(const std::string& strFileName);
	std::string			GetSndFName_Typing() const;
#endif
};

#endif // !defined(AFX_N3UIEDIT_H__91BCC181_3AA5_4CD4_8D33_06D5D96F4F26__INCLUDED_)
