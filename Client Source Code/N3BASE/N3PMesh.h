// N3PMesh.h: interface for the CN3PMesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3PMesh_h__INCLUDED_)
#define AFX_N3PMesh_h__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define _USE_VERTEXBUFFER			// �����ϸ� VertexBuffer�� IndexBuffer�� ����Ѵ�.

#include "N3BaseFileAccess.h"

class CN3PMesh : public CN3BaseFileAccess
{
friend class CN3FXPMeshInstance;
friend class CN3PMeshInstance;
friend class CN3PMeshCreate;
friend class CPMeshEditor;			// N3Viewer�� Ŭ����

public:
	struct  __LODCtrlValue
	{
		float	fDist;
		int		iNumVertices;
	};

	// A single edge collapse / vertex split operation.
	struct __EdgeCollapse
	{
	//	float Value;
		int NumIndicesToLose, NumIndicesToChange, NumVerticesToLose;
		//int *IndexChanges;
		int iIndexChanges;	// ������ ��� n��° ���� (0, 1, 2,...)
		int CollapseTo;
		bool	bShouldCollapse;	// ���⼭ ����/�����⸦ ���߸� ������ �Ը���. �����ܰ踦 �� �����ؾ���.
	};

protected:

	//int m_iNumMaterials;
	int m_iNumCollapses;			// �ﰢ�� ���� ����Ʈ
	int m_iTotalIndexChanges;
	int *m_pAllIndexChanges; // All the index changes

	// The mesh is an array of materials, and an edge collapse list. The mesh
	// is saved in its lowest level of detail

	__EdgeCollapse	*m_pCollapses;

	// Mesh ����
#ifdef _USE_VERTEXBUFFER
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;
#else
	__VertexT1*		m_pVertices;
	__VertexT2*		m_pVertices2;
	WORD*			m_pIndices;
#endif
	int m_iMaxNumVertices, m_iMaxNumIndices;
	int m_iMinNumVertices, m_iMinNumIndices;

	__Vector3	m_vMin, m_vMax; // �ִ� �ּҰ�..
	float		m_fRadius; // ������

	// LOD ���� ����..(�Ÿ��� ���� Vertex�� ���� ������ �� �ִ�.)
	int					m_iLODCtrlValueCount;
	__LODCtrlValue*		m_pLODCtrlValues;
protected:
#ifdef _N3TOOL
	static int SortByDistance(const void *pArg1, const void *pArg2);
#endif // end of _N3TOOL

public:
#ifdef _N3TOOL
	void ReGenerateSharpNormal();
	void ReGenerateSmoothNormal();
	int LODCtrlCount() { return m_iLODCtrlValueCount; }
	void LODCtrlSet(__LODCtrlValue* pLODCtrls, int nCount);
	__LODCtrlValue* LODCtrlGet(int index) { if(index < 0 || index >= m_iLODCtrlValueCount) return NULL; return &m_pLODCtrlValues[index]; }
	int CollapsesCount() { return m_iNumCollapses; }
#endif // end of _N3TOOL

//	int					GetLODCtrlValueCount() const {return m_iLODCtrlValueCount;}
//	__PMLODCtrlValue*	GetLODCtrlValue(int iIndex) const {if (iIndex>=0&&m_iLODCtrlValueCount>iIndex)return m_pLODCtrlValues+iIndex; return NULL;}
//	void				SetLODCtrlValues(__PMLODCtrlValue* pValues) {m_pLODCtrlValues = pValues;}

	__Vector3 Min() { return m_vMin; } // ���� ����� �������� ���������� �ּҰ��̴�.
	__Vector3 Max() { return m_vMax; } // ���� ����� �������� ���������� �ִ밪�̴�.

	void Release();
	HRESULT Create(int iNumVertices, int iNumIndices);
	HRESULT GenerateSecondUV();
	
	bool Load(HANDLE hFile);
#ifdef _N3TOOL
	bool Save(HANDLE hFile);
#endif // end of _N3TOOL
	
	void FindMinMax();

	float Radius() { return m_fRadius; }
	int	GetMinNumVertices() const {return m_iMinNumVertices;};
	int	GetMaxNumVertices() const {return m_iMaxNumVertices;};
	int GetMaxNumIndices() const {return m_iMaxNumIndices;};
	int GetMinNumIndices() const {return m_iMinNumIndices;};

#ifdef _USE_VERTEXBUFFER
	LPDIRECT3DVERTEXBUFFER9	GetVertexBuffer() const {return m_pVB;}
	LPDIRECT3DINDEXBUFFER9	GetIndexBuffer() const {return m_pIB;}
#else
	__VertexT1*		GetVertices() const { return m_pVertices; };
	__VertexT2*		GetVertices2() const { return m_pVertices2; };
	WORD*			GetIndices() const { return m_pIndices;};		// ����ε� Index�� �ƴ� 
	// (����� �� �ε����� �������� N3PMeshInstance�� ������ LOD������ �ε������� ������ �ȴ�.)
#endif

#ifdef _N3TOOL
	void CopyMesh(CN3PMesh* pSrcPMesh);
#endif // end of _N3TOOL

	CN3PMesh();
	virtual ~CN3PMesh();
};

#endif // !defined(AFX_N3PMesh_h__INCLUDED_)
