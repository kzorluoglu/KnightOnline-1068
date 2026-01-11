// N3Shape.h: interface for the C3DObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3Shape_h__INCLUDED_)
#define AFX_N3Shape_h__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786)

#include "N3TransformCollision.h"
#include "N3PMeshInstance.h"
#include "N3Texture.h"

#include <vector>

typedef std::vector<CN3Texture*> it_pTex;

class CN3SPart : public CN3BaseFileAccess
{
	friend class CN3Shape;
public:
	__Vector3	m_vPivot; // Local ��
	__Matrix44	m_Matrix; // World Matrix.. Shape Loading �� �̸� ����ؾ� ����..		
	BOOL		m_bOutOfCameraRange; // Camera ���� �ٱ��� ����...

	__Material	m_Mtl; // Material
	float		m_fTexFPS; // Texture Animation Interval;

//	__Vector3	m_vWindFactorCur;		// ���� �ٶ� �δ� ��.. �̰����� ȸ���� ��Ų��..
//	__Vector3	m_vWindFactorToReach;	// �ٶ� �δ� ��..
	float		m_fTimeToSetWind;		// �ٶ� �δ� ���� �ٲٱ� ���� �ð�..
	float		m_fWindFactorToReach;	// ���� �ٶ� �δ� ��.. �̰����� ȸ���� ��Ų��..
	float		m_fWindFactorCur;		// �ٶ� �δ� ��..

protected:
	std::vector<CN3Texture*>	m_TexRefs; // Texture Reference Pointers
	CN3PMeshInstance			m_PMInst; // Progressive Mesh Instance

	float	m_fTexIndex; // Current Texture Index.. Animation ��ų�� �ʿ��� �ε����̴�.. float �� �ؼ� �ؽ�ó ���ϸ��̼� �����Ѵ�.

public:
	virtual bool Load(HANDLE hFile);
#ifdef _N3TOOL
	virtual bool Save(HANDLE hFile);
#endif // end of _N3TOOL
	
	int			TexCount() { return m_TexRefs.size(); }
	CN3Texture* Tex(int iIndex)
	{
		if(iIndex < 0 || iIndex >= m_TexRefs.size()) return NULL;
		return m_TexRefs[iIndex];
	}
	void		TexAlloc(int m_nCount);
	CN3Texture*	TexSet(int iIndex, const std::string& szFN)
	{
		if(iIndex < 0 || iIndex >= m_TexRefs.size()) return NULL;
		s_MngTex.Delete(&m_TexRefs[iIndex]);
		m_TexRefs[iIndex] = s_MngTex.Get(szFN, true, s_Options.iTexLOD_Shape);
		return m_TexRefs[iIndex];
	}
	void	TexSet(int iIndex, CN3Texture* pTex)
	{
		if(iIndex < 0 || iIndex >= m_TexRefs.size()) return;
		s_MngTex.Delete(&m_TexRefs[iIndex]);
		m_TexRefs[iIndex] = pTex;
	}


	CN3PMeshInstance*	MeshInstance() { return &m_PMInst; } 
	CN3PMesh*			Mesh() { return m_PMInst.GetMesh(); }
	void				MeshSet(const std::string& szFN);

	void ReCalcMatrix(const __Matrix44& mtxParent) { m_Matrix.Identity(); m_Matrix.PosSet(m_vPivot); m_Matrix *= mtxParent; }

	void Tick(const __Matrix44& mtxParent, const __Quaternion& qRot, float fScale); // �θ� ��� �� Shape ���, ȸ�����ʹϾ� �� �ִ´�.
	void Render();
#ifdef _N3TOOL
	void RenderSelected(bool bWireFrame);
	void RenderAxis();
#endif // end of _N3TOOL

	__Vector3 Min() { if(m_PMInst.GetMesh()) return m_PMInst.GetMesh()->Min() * m_Matrix; else return __Vector3(0,0,0); } // ���� ���� �ּҰ�
	__Vector3 Max() { if(m_PMInst.GetMesh()) return m_PMInst.GetMesh()->Max() * m_Matrix; else return __Vector3(0,0,0); } // ���� ���� �ִ밪
	float	Radius() { if(m_PMInst.GetMesh()) return m_PMInst.GetMesh()->Radius(); else return 0.0f; }

	virtual void	Release();
	
	CN3SPart();
	virtual ~CN3SPart();

#ifdef _USE_VERTEXBUFFER
	void			PartialRender(int iCount, LPDIRECT3DINDEXBUFFER9 pIB);
#else
	void			PartialRender(int iCount, WORD* pIndices);
#endif
};

typedef std::vector<CN3SPart*>::iterator it_SPart;

class CN3Shape : public CN3TransformCollision
{
public:
	int		m_iBelong;			// �Ҽ� - 0:�Ҽ� ���� 1:������ 2:ī�罺 3:?? ....
	int		m_iEventID;			// Event ID
	int		m_iEventType;		// Event Type
	int		m_iNPC_ID;			// NPC �� ���� ������Ʈ�� ��� NPC ID
	int		m_iNPC_Status;		// NPC �� ���� ������Ʈ�� ��� Default Status

	bool	m_bDontRender; // ī�޶� �Ÿ��� ���� ���÷��װ� �����Ǹ� ���������� �ʴ´�..
	bool	m_bVisible;	// .. 

	std::vector<CN3SPart*>	m_Parts; // Part Data Pointer Linked List

public:
#ifdef _N3TOOL
	bool			SaveToSameFolderAndMore(const std::string& szFullPath, const std::string& szRelativePath);
	bool			SaveToSameFolder(const std::string& szFullPath);
	void			RemoveRenderFlags(int nFlags = -1);
	void			MakeDefaultMaterial();
#endif // end of _N3TOOL
	int				CheckCollisionPrecisely(bool bIgnoreBoxCheck, int ixScreen, int iyScreen, __Vector3* pVCol = NULL, __Vector3* pVNormal = NULL); // �����ϰ� ������ ������ üũ - ���� �ڽ� üũ�� �ٽ� ���� üũ..
	int				CheckCollisionPrecisely(bool bIgnoreBoxCheck, const __Vector3& vPos, const __Vector3& vDir, __Vector3* pVCol = NULL, __Vector3* pVNormal = NULL); // �����ϰ� ������ ������ üũ - ���� �ڽ� üũ�� �ٽ� ���� üũ..
	bool			MakeCollisionMeshByParts();  // �浹 �޽ø� �ڽ� ���·� �ٽ� �����...
	bool			MakeCollisionMeshByPartsDetail();  // ���� ��� �״��... �浹 �޽ø� �����...

	void			FindMinMax();
	virtual void	ReCalcMatrix();
	void			ReCalcPartMatrix();
	virtual void	Tick(float fFrm = FRAME_SELFPLAY);
	virtual void	Render();

	CN3SPart*		Part(int iIndex) { if(iIndex < 0 || iIndex >= m_Parts.size()) return NULL; return m_Parts[iIndex]; }
	CN3SPart*		PartAdd() { CN3SPart* pPart = new CN3SPart(); m_Parts.push_back(pPart); return pPart; }
	int				PartCount() { return m_Parts.size(); }
	void			PartDelete(int iIndex);
	
	bool			Load(HANDLE hFile);
#ifdef _N3TOOL
	bool			Save(HANDLE hFile);
	void			RenderSelected(bool bWireFrame);
	void			RenderSelected(int iPart, bool bWireFrame);
	bool			IsPMeshProcessed();
#endif // end of _N3TOOL

	virtual void	Release();
	CN3Shape();
	virtual ~CN3Shape();

//	By : Ecli666 ( On 2002-08-06 ���� 4:33:04 )
//
	void			SetMaxLOD();
	__Matrix44	GetPartMatrix(int iPartIndex);
#ifdef _USE_VERTEXBUFFER
	void			PartialRender(int iPartIndex, int iCount, LPDIRECT3DINDEXBUFFER9 pIB);
#else
	void			PartialRender(int iPartIndex, int iCount, WORD* pIndices);
#endif
	int				GetIndexbufferCount(int iPartIndex);
	int				GetIndexByiOrder(int iPartIndex, int iOrder);
__Vector3		GetVertexByIndex(int iPartIndex, int iIndex); 
	int				GetColIndexbufferCount();
	int				GetColIndexByiOrder(int iOrder);
__Vector3	  GetColVertexByIndex(int iIndex); 
	void			PartialColRender(int iCount, int* piIndices);
	void			PartialGetCollision(int iIndex, __Vector3& vec);
	bool			LoadTransformOnly(HANDLE hFile);
//	~(By Ecli666 On 2002-08-06 ���� 4:33:04 )
};

#endif // !defined(AFX_N3Shape_h__INCLUDED_)
