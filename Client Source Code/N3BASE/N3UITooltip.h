// N3UITooltip.h: interface for the CN3UITooltip class.
// CN3UIBase�� EnableTooltip() �� DestroyTooltip()�Լ��� �Ʒ� �׸���� �������ݴϴ�.
// 1. tooltip�� �θ� null�� ���ְ� ���� ���� �ؾ� �մϴ�.
// 2. Save/Load�� ���� �����մϴ�.
//
// �Ʒ� �׸���� ���� �׸� �ܿ��� ó�����־�� �� �͵��Դϴ�.
// 1. Render�� ��� UI�� �� �׸��� Render�ؾ� �մϴ�.
// 2. MouseProc�Լ��� ȣ������� �մϴ�.
// 3. Tick�Լ��� ȣ������� �մϴ�.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3UITOOLTIP_H__7085B857_C8EE_410D_AB27_5332D26DF01A__INCLUDED_)
#define AFX_N3UITOOLTIP_H__7085B857_C8EE_410D_AB27_5332D26DF01A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3UIStatic.h"

class CN3UITooltip : public CN3UIStatic
{
public:
	CN3UITooltip();
	virtual ~CN3UITooltip();

// Attributes
public:
protected:
	float			m_fHoverTime;	// ���콺�� �Ѱ����� �����Ͽ� �ִ� �ð�(����)
	bool			m_bSetText;		// �̹� text�� �����Ǿ��°�?
	POINT			m_ptCursor;		// Ŀ���� ��ġ

// Operations
public:
	void			SetText(const std::string& szText);
	virtual void	Release();
	virtual void	Tick();
	virtual void	Render();
	virtual DWORD	MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);
protected:

};

#endif // !defined(AFX_N3UITOOLTIP_H__7085B857_C8EE_410D_AB27_5332D26DF01A__INCLUDED_)
