// N3EffectInstance.h: interface for the CN3EffectInstance class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3EFFECTINSTANCE_H__0F7C2231_1D9E_44A5_BB6F_E614291D90FB__INCLUDED_)
#define AFX_N3EFFECTINSTANCE_H__0F7C2231_1D9E_44A5_BB6F_E614291D90FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3Transform.h"
#include "N3Effect.h"

class CN3Shape;

class CN3EffectPartInstance : public CN3Base
{
protected:
	CN3EffectPart* m_pEffectPartRef; // Effect Part Class Reference Pointer

public:
	BOOL	m_nTexCycleCount; // Texture Animation �� ���ư� Ƚ��.
	int		m_nTexIndexCur; // ���� Texture Index;
	DWORD	m_dwTickPrev; // Texture Frame �����

	float		m_fGravityCur; // ���� �߷¼ӵ�...
	__Vector3*	m_pVDeltas; // Instance ���� �ִ� ���� Move Vector
	__Vector3*	m_pVPosCurs; // Instance ���� �ִ� Random

	float*		m_pfGravityCurs; // ��ƼŬ�϶� ���� �߷¼ӵ�...

	int*		m_pnTexIndices;
	int*		m_pnTexCycles;
	DWORD*		m_pdwTexAlphaFactors;
	DWORD*		m_pdwTickPrevs;
	bool*		m_bDeads;

	CN3Shape*	m_pShapeRef;

protected:
	void	GenerateRandomPositions();

public:
	CN3EffectPart* EffectPart() { return m_pEffectPartRef; }
	void	EffectSet(CN3EffectPart* pEffect);
	void	Tick(DWORD dwTick, __Vector3& vDir);
	void	Render(__Vector3& vPos);
	int		TexCycleCount() { return m_nTexCycleCount; }

	void	Release();
	CN3EffectPartInstance();
	~CN3EffectPartInstance();
};

class CN3EffectInstance : public CN3Transform
{
public:
	CN3Effect* m_pEffectRef; // Effect Reference Pointer
	DWORD	m_dwTickPrev0, m_dwTickPrev1; // Trigger Timer
	BOOL	m_bDontRender;

	// key for prepare..
	__Vector3	m_vDir;
	__Vector3	m_vStart;
	__Vector3	m_vPosCur;
	__Vector3	m_vPosPrev;

	float		m_fTime;

protected:
	CN3EffectPartInstance	m_PartInst0, m_PartInst1;
	int				m_nTickStep; // ���� �ܰ�

public:
	void Create(CN3Effect* pEffect);
	void Tick(DWORD dwTick);
	int TickStep() { return m_nTickStep; }
	void Render();
	void MakeBoard(__VertexXyzColorT1 *pVs, float fSize, D3DCOLOR Color, __Vector3& vOffset, __Vector3& vRandom);

	void Release();
	CN3EffectInstance();
	virtual ~CN3EffectInstance();

};

#endif // !defined(AFX_N3EFFECTINSTANCE_H__0F7C2231_1D9E_44A5_BB6F_E614291D90FB__INCLUDED_)
