// N3ColorChange.h: interface for the CN3ColorChange class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3COLORCHANGE_H__6A4576D3_1880_40A4_AED9_39B306508D0C__INCLUDED_)
#define AFX_N3COLORCHANGE_H__6A4576D3_1880_40A4_AED9_39B306508D0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "N3Base.h"

class CN3ColorChange  : public CN3Base
{
public:
	CN3ColorChange();
	virtual ~CN3ColorChange();

public:
	D3DCOLOR	GetCurColor() const {return m_CurColor;}
	D3DCOLOR	GetNextColor() const {return m_NextColor;}
	void		ChangeColor(D3DCOLOR color, float fSec = 0.0f);
	void		Tick();
	void		SetPercentage(float fPercentage);
protected:
	D3DCOLOR	m_CurColor;	// ���� ��.
	D3DCOLOR	m_PrevColor;	// ��ȭ�ϱ� �� ��.
	D3DCOLOR	m_NextColor;	// ���������� ��ȭ�� ��.
	float		m_fPercentage;	// ���� ��ȭ�� ����(0.0 ~ 1.0)
	float		m_fRate;		// �ʴ� ��ȭ��
	float		m_fTempSec;		// �ӽ� �ð� ���� ����
};

class CN3DeltaChange  : public CN3Base
{
public:
	CN3DeltaChange();
	virtual ~CN3DeltaChange();

public:
	float		GetCurDelta() const {return m_fCurDelta;}
	float		GetNextDelta() const {return m_fNextDelta;}
	void		ChangeDelta(float fDelta, float fSec = 0.0f);
	void		Tick();
	void		SetPercentage(float fPercentage);
protected:
	float		m_fCurDelta;	// ���� ��.
	float		m_fPrevDelta;	// ��ȭ�ϱ� �� ��.
	float		m_fNextDelta;	// ���������� ��ȭ�� ��.

	float		m_fPercentage;	// ���� ��ȭ�� ����(0.0 ~ 1.0)
	float		m_fRate;		// �ʴ� ��ȭ��
	float		m_fTempSec;		// �ӽ� �ð� ���� ����
};

#endif // !defined(AFX_N3COLORCHANGE_H__6A4576D3_1880_40A4_AED9_39B306508D0C__INCLUDED_)
