// N3UIString.h: interface for the CN3UIString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3UISTRING_H__E3559B01_72AE_4651_804D_B96D22738ED8__INCLUDED_)
#define AFX_N3UISTRING_H__E3559B01_72AE_4651_804D_B96D22738ED8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "N3UIBase.h"
#include "DFont.h"

class CDFont;
class CN3UIString : public CN3UIBase  
{
	friend class CN3UIEdit;
#ifdef _N3TOOL
friend class CPropertyView;	// ������ �� �������� �����ϱ� ���ؼ� 
#endif

protected:
	CDFont*			m_pDFont;			// DFont(���� ���ڸ� ȭ�鿡 ����ִ� Ŭ������)
	POINT			m_ptDrawPos;		// ���� ȭ�鿡 ǥ�õ� ������ ���� ���� ��� ��ǥ
	std::string 	m_szString;			// string buffer
	D3DCOLOR		m_Color;			// ���� ��
	int				m_iLineCount;		// ���� ���õ� �۾����� line��
	std::vector<int>	m_NewLineIndices;	// multiline�ϰ�� ���ο� ������ �ε�����
	int				m_iStartLine;		// multiline�ϰ�� �۾��� ������ ���� ���ι�ȣ

public:
	CN3UIString();
	virtual ~CN3UIString();

// Attributes
public:
	void				SetColor(D3DCOLOR color) { m_Color = color; }
	D3DCOLOR			GetColor() const { return m_Color; }
	const std::string&	GetString() { return m_szString; }
	int					GetLineCount() const {return m_iLineCount;}
	int					GetStartLine() const {return m_iStartLine;}
	int					GetStringRealWidth(int iNum);

	virtual	DWORD	MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);
	virtual void	Render();
	virtual void	Release();	
	virtual void	Init(CN3UIBase* pParent);
	virtual BOOL	MoveOffset(int iOffsetX, int iOffsetY);// �۾���� ��ġ�� �ٲ�� �ش�.
	virtual bool	Load(HANDLE hFile);
	void			ClearOnlyStringBuffer() { m_szString = ""; }	// string ���۸� �����.
	void			SetStartLine(int iLine);	// multiline�ϰ�� �����ϴ� ���� �����ϱ�

	virtual void	operator = (const CN3UIString& other);

#ifdef _N3TOOL
	virtual bool	Save(HANDLE hFile);
	virtual void	ChangeFont(const std::string& szFont);
#endif
	
	virtual void	SetRegion(const RECT& Rect);
	virtual void	SetStyle(DWORD dwStyle);
	virtual void	SetStyle(DWORD dwType, DWORD dwStyleEx);

	virtual void	SetString(const std::string& szString);
	virtual void	SetStringAsInt(int iVal);
	void			SetString_NoWordWrap(const std::string& szString);	// ���� ���� ���� �ʴ´�.
	virtual void	SetFont(const std::string& szFontName, DWORD dwHeight, BOOL bBold, BOOL bItalic); // dwHeight�� point size�̴�.
	BOOL			GetTextExtent(const std::string& szString, int iStrLen, SIZE* pSize )
	{
		if (m_pDFont) return m_pDFont->GetTextExtent(szString, iStrLen, pSize);
		return FALSE;
	}
	DWORD				GetFontColor() const {if (m_pDFont) return m_pDFont->GetFontColor(); return 0xffffffff;}
	const std::string&	GetFontName() const {if (m_pDFont) return m_pDFont->GetFontName(); return s_szStringTmp;} 
	DWORD				GetFontHeight() const {if (m_pDFont) return m_pDFont->GetFontHeight(); return 0;} 
	DWORD				GetFontFlags() const {if (m_pDFont) return m_pDFont->GetFontFlags(); return 0;}
protected:
	void				WordWrap();		// wordwrap
};

#endif // !defined(AFX_N3UISTRING_H__E3559B01_72AE_4651_804D_B96D22738ED8__INCLUDED_)
