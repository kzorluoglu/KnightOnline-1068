// N3Skin.h: interface for the CN3Skin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3Skin_h__INCLUDED_)
#define AFX_N3Skin_h__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3Base.h"

struct __VertexSkinned
{
	__Vector3	vOrigin; // ���� �� ��ġ..
	int			nAffect; // ���� ��� ������ �ִ���..
	int*		pnJoints; // ���� �ε���
	float*		pfWeights; // ����ġ

	__VertexSkinned() { memset(this, 0, sizeof(__VertexSkinned)); }
	~__VertexSkinned() { delete [] pnJoints; delete [] pfWeights; }
};

class CN3Skin : public CN3Base  
{
protected:
	struct __Weight
	{
		int nIndex;
		float fWeight;
	};

	static int SortWeightsProc(const void* pArg1, const void* pArg2);
	
	int		m_nVertexCount; // �� ����
	__VertexSkinned* m_pVertices;


public:
	int		VertexCount() { return m_nVertexCount; }
	__VertexSkinned* Vertices() { return m_pVertices; }
	void	Alloc(int nCount);
	
	bool	Save(HANDLE hFile);
	bool	Load(HANDLE hFile);
	
	void	Release();
	CN3Skin();
	virtual ~CN3Skin();

};

#endif // !defined(AFX_N3Skin_h__INCLUDED_)
