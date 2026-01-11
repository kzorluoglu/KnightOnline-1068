// N3UITrackBar.h: interface for the CN3UITrackBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3UITRACKBAR_H__EB496B74_468B_4D7B_89A9_D3A5A1A3E538__INCLUDED_)
#define AFX_N3UITRACKBAR_H__EB496B74_468B_4D7B_89A9_D3A5A1A3E538__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3UIBase.h"

class CN3UITrackBar : public CN3UIBase  
{
public:
	CN3UITrackBar();
	virtual ~CN3UITrackBar();

// Attributes
public:
	enum eIMAGE_TYPE{IMAGETYPE_BKGND=0, IMAGETYPE_THUMB, NUM_IMAGETYPE};
protected:
	CN3UIImage*		m_pBkGndImageRef;		// ��� �̹��� referance (�޸� �Ҵ��� children list�� ����)
	CN3UIImage*		m_pThumbImageRef;		// ��� �巹�� �Ͽ� �ű� �� �ִ� �̹��� referance

	int				m_iMaxPos;									// �ִ�
	int				m_iMinPos;									// �ּ�
	int				m_iCurPos;									// ���� ��
	int				m_iPageSize;								// page���� �̵��Ҷ� �̵���
// Operations
public:
	virtual void	Release();
	virtual bool	Load(HANDLE hFile);
	virtual void	SetRegion(const RECT& Rect);
	virtual DWORD	MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);
//	virtual void	Render();

	void			SetRange(int iMin, int iMax);
	void			SetRangeMax(int iMax) {SetRange(m_iMinPos, iMax);}
	void			SetRangeMin(int iMin) {SetRange(iMin, m_iMaxPos);}
	void			SetCurrentPos(int iPos);
	int				GetPos() const {return m_iCurPos;}
	void			SetPageSize(int iSize) {m_iPageSize = iSize;}
	int				GetPageSize() const {return m_iPageSize;}
	int				GetMaxPos() const {return m_iMaxPos;}
	int				GetMinPos() const {return m_iMinPos;}
protected:
	void			UpdateThumbPos();							// m_iCurPos�� ����Ͽ� Thumb��ġ �ٽ� ����Ͽ� �ٲٱ�
	void			UpDownThumbPos(int iDiff);					// Thumb��ġ�� �Ʒ� ���� iDiff pixel��ŭ ������ �� m_iCurPos �����ϱ�

#ifdef _N3TOOL
// tool���� ���̴� �Լ�
public:
	virtual void	operator = (const CN3UITrackBar& other);
	void			CreateImages();			// �̹��� ����
	void			DeleteBkImage();		// ����̹��� ����
	CN3UIImage*		GetBkGndImgRef() const {return m_pBkGndImageRef;}
	CN3UIImage*		GetThumbImgRef() const {return m_pThumbImageRef;}
#endif
};

#endif // !defined(AFX_N3UITRACKBAR_H__EB496B74_468B_4D7B_89A9_D3A5A1A3E538__INCLUDED_)
