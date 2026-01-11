// N3VMesh.h: interface for the C3DObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3VMesh_h__INCLUDED_)
#define AFX_N3VMesh_h__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3Base.h"

class CN3IMesh;

class CN3VMesh : public CN3Base
{
protected:
	__Vector3*	m_pVertices; // �� ����
	int		m_nVC; // �� ����

	WORD*	m_pwIndices; // Index...
	int		m_nIC; // Index Count

	__Vector3 m_vCenter; // Mesh Vertices �� �߰���..
	float m_fRadius; // ������

public:
	bool Import(CN3IMesh *pIMesh);
	bool CheckCollision(const __Matrix44* pMtxWorld, int xScreen, int yScreen); // Screen ��ǥ�� Local ��ǥ�� ���� �浹 üũ ����..
	bool CheckCollision(const __Matrix44* pMtxWorld, __Vector3& v0, __Vector3& v1, __Vector3 &vCollision, __Vector3 &vNormal); // Local ��ǥ�� ���� �浹 üũ ����..
	float Radius() { return m_fRadius; }
	__Vector3 Center() { return m_vCenter; }
	void CalcRadiusAndCenter();
	void Render(D3DCOLOR crLine);
	void CreateVertices(int nVC);
	void CreateIndex(int nIC);
	void CreateCube(const __Vector3& vMin, const __Vector3& vMax);
	
	int			VertexCount() { return m_nVC; }
	__Vector3*	Vertices() { return m_pVertices; }

	int		IndexCount() { return m_nIC; }
	WORD*	Indices() { return m_pwIndices; }

	bool	Load(HANDLE hFile);
	bool	Save(HANDLE hFile);
	
	void	Release();

	CN3VMesh();
	virtual ~CN3VMesh();
};

#endif // !defined(AFX_N3VMesh_h__INCLUDED_)
