// N3Shape.h: interface for the C3DObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3Shape_h__INCLUDED_)
#define AFX_N3Shape_h__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3TransformCollision.h"
#include "N3Texture.h"
#include "N3PMesh.h"
#include "N3PMeshInstance.h"

#include "N3LinkedList.h"

class CN3Shape : public CN3TransformCollision
{
public:
	class CPart : public CN3Base
	{
		friend CN3Shape;
	public:
		__Vector3	m_vPivot; // Local ��
		__Matrix44	m_Matrix; // World Matrix.. Shape Loading �� �̸� ����ؾ� ����..		
		BOOL		m_bOutOfCameraRange; // Camera ���� �ٱ��� ����...

		__Material	m_Mtl; // Material
		float		m_fTexFPS; // Texture Animation Interval;

	protected:
		int					m_nTexCount; // Texture Count
		CN3Texture**		m_ppTexRefs; // Texture Reference Pointers
		CN3PMeshInstance	m_PMInst; // Progressive Mesh Instance

		int		m_nTexIndex; // Current Texture Index.. Animation ��ų�� �ʿ��� �ε����̴�..
		DWORD	m_dwTickPrev; // ���� Tick Count...

		__Vector3	m_vMin, m_vMax; // �ּ� �ִ���.. ���� ������� �����̴�..
		float		m_fRadius; // �ݰ�..

	public:
		bool	Save(HANDLE hFile);
		virtual bool	Load(HANDLE hFile);
		void	FindMinMax();
		int			TexCount() { return m_nTexCount; }
		CN3Texture* Tex(int index) { if(index < 0 || index >= m_nTexCount) return NULL; return m_ppTexRefs[index]; }
		void		TexAlloc(int m_nCount);
		void		TexSet(int index, const char* szFN);

		CN3PMeshInstance*	MeshInstance() { return &m_PMInst; } 
		CN3PMesh*			Mesh() { return m_PMInst.GetMesh(); }
		void				MeshSet(const char* szFN);

		void RecalcMatrix(__Matrix44& mtxParent) { m_Matrix.Identity(); m_Matrix.PosSet(m_vPivot); m_Matrix *= mtxParent; }

		void Tick(DWORD dwTime); // timeGetTime ���� ���� ���� ������ Texture Animation �� ��Ʈ�� �Ѵ�..
		void Render();
		void RenderSelected();
		void RenderAxis();

		__Vector3 Min() { return m_vMin; }
		__Vector3 Max() { return m_vMax; }
		float Radius() { return m_fRadius; }

		void	Release();
		
		CPart();
		virtual ~CPart();
	};
public:
	int		m_nBelong; // �Ҽ� - 0:�Ҽ� ���� 1:������ 2:ī�罺 3:?? ....

protected:
	BOOL	m_bDontRender; // ī�޶� �Ÿ��� ���� ���÷��װ� �����Ǹ� ���������� �ʴ´�..

	CN3LinkedList<CPart*>	m_Parts; // Part Data Pointer Linked List

public:
	bool CheckPreciseCollision(__Vector3& vPos, __Vector3& vDir);  // �����ϰ� ������ ������ üũ
	void FindMinMax();
	virtual void	ReCalcMatrix();
	void			ReCalcPartMatrix();
	virtual void	Tick(float fFrm = FRAME_SELFPLAY);
	virtual void	Render();

	void RenderSelected();
	void RenderSelected(int nPart);

	int		PartCount() { return m_Parts.Count(); }
	int		PartAdd();
	void	PartDelete(int nPart);
	CPart*	Part(int nPart) { if(nPart < 0 || nPart >= m_Parts.Count()) return NULL; return m_Parts.Get(nPart); }
	
	bool Load(HANDLE hFile);
	bool Save(HANDLE hFile);

	virtual void	Release();
	CN3Shape();
	virtual ~CN3Shape();

	BOOL GetDontRender() { return m_bDontRender; }
};

#endif // !defined(AFX_N3Shape_h__INCLUDED_)
