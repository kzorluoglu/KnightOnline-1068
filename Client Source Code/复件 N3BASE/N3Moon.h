// N3Moon.h: interface for the CN3Moon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3MOON_H__A89DCBCF_C959_4D36_92C1_E138448717D9__INCLUDED_)
#define AFX_N3MOON_H__A89DCBCF_C959_4D36_92C1_E138448717D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3Base.h"
#include <string>

class CN3Texture;
class CN3Moon : public CN3Base  
{
#ifdef _N3TOOL
	friend class CN3SkyMng;
#endif

public:
	CN3Moon();
	virtual ~CN3Moon();
// Attributes
public:
	int						GetMoonPhaseIndex() const {return m_iMoonPhaseIndex;}
protected:	
	__VertexTransformed		m_pVertices[4];
	CN3Texture				*m_pTexture;
	float					m_fCurRadian;		// ���� ���� ȸ����ġ
	float					m_fMoonRatio;		// ���� pixel������
	int						m_iMoonPhaseIndex;	// ���� ��� �ε��� 0~23;

// Operations
public:
	void	Init(const std::string& szTexFN);
	void	SetMoonPhase(int iIndex);	// ���� ��� ����
	void	SetCurAngle(float fAngle)	{m_fCurRadian = D3DXToRadian(fAngle);}	// ���� ��������

	void	Render(__Matrix44& matView, __Matrix44& matProj);
	void	Release();
};

#endif // !defined(AFX_N3MOON_H__A89DCBCF_C959_4D36_92C1_E138448717D9__INCLUDED_)
