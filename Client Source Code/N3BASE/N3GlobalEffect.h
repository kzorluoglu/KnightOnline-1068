// N3GlobalEffect.h: interface for the CN3GlobalEffect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3GLOBALEFFECT_H__01468E41_4EE1_4893_8886_E57EE2491007__INCLUDED_)
#define AFX_N3GLOBALEFFECT_H__01468E41_4EE1_4893_8886_E57EE2491007__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3Transform.h"
const DWORD GETYPE_RAIN = 0x00000001;
const DWORD GETYPE_SNOW = 0x00000002;

class CN3GlobalEffect : public CN3Transform  
{
public:
	CN3GlobalEffect();
	virtual ~CN3GlobalEffect();

// Attributes
public:
	void						SetActive(BOOL bActive) {m_bActive = bActive;}
protected:
	int							m_iVC;
	int							m_iIC;
	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	LPDIRECT3DINDEXBUFFER9		m_pIB;
	DWORD						m_dwEffectType;
	BOOL						m_bActive;
	float						m_fFadeTime; // �̽ð� ���� ���� ��ǥ�� �縸ŭ ��ƼŬ�� ���� �þ��..
	float						m_fFadeTimeCur; // �����ð�..
	int							m_iFadeMode; // 1 - FadeIn 0... -1 FadeOut

// Operations
public:
	virtual bool	NeedDelete() { if(m_iFadeMode < 0 && m_fFadeTimeCur >= m_fFadeTime) return true; return false; }
	virtual void	FadeSet(float fTimeToFade, bool bFadeIn);
	virtual void	Release();
	virtual void	Tick();
	virtual void	Render(__Vector3& vPos);
protected:

};

#endif // !defined(AFX_N3GLOBALEFFECT_H__01468E41_4EE1_4893_8886_E57EE2491007__INCLUDED_)
