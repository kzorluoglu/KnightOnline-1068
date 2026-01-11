// N3Skin.h: interface for the CN3Skin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3Skin_h__INCLUDED_)
#define AFX_N3Skin_h__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3IMesh.h"

struct __VertexSkinned
{
	__Vector3	vOrigin; // ���� �� ��ġ..
	int			nAffect; // ���� ��� ������ �ִ���..
	int*		pnJoints; // ���� �ε���
	float*		pfWeights; // ����ġ

	__VertexSkinned() { memset(this, 0, sizeof(__VertexSkinned)); }
	~__VertexSkinned() { delete [] pnJoints; delete [] pfWeights; }
};

class CN3Skin : public CN3IMesh
{
protected:
#ifdef _N3TOOL
	struct __Weight
	{
		int nIndex;
		float fWeight;
	};
	static int SortWeightsProc(const void* pArg1, const void* pArg2);
#endif // end of _N3TOOL
	
	__VertexSkinned*	m_pSkinVertices;

public:
	__VertexSkinned*	SkinVertices() { return m_pSkinVertices; }
	bool				Create(int nFC, int nVC, int nUVC);
	bool				Load(HANDLE hFile);

	bool				CheckCollisionPrecisely(const __Vector3 &vPos, const __Vector3 &vDir, __Vector3* pvPick);

#ifdef _N3TOOL
	bool				Save(HANDLE hFile);
	void				RecalcWeight();		// ����Ʈ�� �ٽ� ����ϱ�
#endif // end of _N3TOOL
	
	void Release();
	CN3Skin();
	virtual ~CN3Skin();

};

#endif // !defined(AFX_N3Skin_h__INCLUDED_)
