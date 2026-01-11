// N3Chr.h: interface for the CN3Chr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3Chr_h__INCLUDED_)
#define AFX_N3Chr_h__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3Texture.h"
#include "N3TransformCollision.h"
#include "N3Joint.h"
#include "N3Skin.h"
#include "N3IMesh.h"
#include "N3PMeshInstance.h"
#include "N3LinkedList.h"
#include "N3AnimControl.h"

const int MAX_CHR_JOINT = 256;
const int MAX_CHR_LOD = 4; // 4 �ܰ� Level Of Detail

enum e_PartType	{ PART_HAIR = 0, PART_FACE, PART_UPPER, PART_LOWER, PART_HAND, PART_FOOT, PART_UNKNOWN = 0xffffffff };
enum e_PlugType { PLUG_RIGHTHAND = 0, PLUG_LEFTHAND, PLUG_TWOHAND, PLUG_QUIVER, PLUG_UNKNOWN = 0xffffffff };

class CN3CPart : public CN3Base
{
	friend class CN3Chr;
public:
	char		m_szID[64]; // ���� �̸��� �ƴ� ������ Part �̸�..
	__Material	m_Mtl;
	e_PartType	m_Type;
protected:
	CN3Texture* m_pTexRef;
	CN3IMesh	m_IMeshes[MAX_CHR_LOD];
	CN3Skin		m_Skins[MAX_CHR_LOD];
public:
	bool Load(HANDLE hFile);
	bool Save(HANDLE hFile);

	void Render(int nLOD);
	CN3Texture* Tex() { return m_pTexRef; }
	void TexSet(const char* szFN);
	CN3IMesh* Mesh(int nLOD) { if(nLOD >= 0 && nLOD < MAX_CHR_LOD) return &m_IMeshes[nLOD]; else return NULL; }
	CN3Skin* Skin(int nLOD) { if(nLOD >= 0 && nLOD < MAX_CHR_LOD) return &m_Skins[nLOD]; else return NULL; }

	void Release();
	
	CN3CPart();
	virtual ~CN3CPart();
};

class CN3CPlug : public CN3Base
{
	friend class CN3Chr;
public:
	e_PlugType	m_Type;
	int m_nJointIndex; // �ٴ� Mesh �� Joint Index.. ��� ����Ʈ�� �ٴ���.. �װ��� �����δ�...
	__Material m_Mtl;
	__Matrix44 m_Matrix;

	int 		m_nTraceStep; // ������ ����..
	D3DCOLOR	m_crTrace; // ���� ����.. �������̸� ����..
	float		m_fTrace0; // ���� ��ġ..
	float		m_fTrace1; // ���� ��ġ..

protected:
	CN3PMeshInstance	m_PMeshInst; // Progressive Mesh Instance
	CN3Texture*			m_pTexRef; // Texture Reference Pointer
	__Vector3	m_vPosition; // �ٴ� Mesh �� Offset Vector.
//		__Vector3 m_vRotation; // �ٴ� Mesh �� Rotation��.
	__Matrix44	m_MtxRot; // Rotation Matrix;
	__Vector3	m_vScale; // �ٴ� Mesh �� ������.

public:
	bool Load(HANDLE hFile);
	bool Save(HANDLE hFile);

	void Render(__Matrix44& mtxParent, __Matrix44& mtxJoint);
	void RecalcMatrix();

	__Vector3& Position() { return m_vPosition; }
	__Matrix44& Rotation() { return m_MtxRot; }
	__Vector3& Scale() { return m_vScale; }

	void PositionSet(__Vector3& vPos) { m_vPosition = vPos; this->RecalcMatrix(); }
	void RotationSet(__Matrix44& mtx) { m_MtxRot = mtx; this->RecalcMatrix(); }
	void ScaleSet(__Vector3& vScale2) { m_vScale = vScale2; this->RecalcMatrix(); }
	
	CN3Texture* Tex() { return m_pTexRef; }
	void TexSet(const char* szFN);
	
	CN3PMesh* PMesh() { return m_PMeshInst.GetMesh(); }
	CN3PMeshInstance* PMeshInst() { return &m_PMeshInst; }
	void PMeshSet(const char* szFN);
	
	void Release();

	CN3CPlug();
	virtual ~CN3CPlug();
};

const int MAX_PLUG_TRACE = 2; // �ִ� �ΰ��� ���� ������ �����..
const int MAX_PLUG_TRACE_VERTEX = 24; // ���� ���� ���� ��.. �� 8 ���δ� �ܻ��� 3�ܰ�� ���´�..

class CN3Chr : public CN3TransformCollision
{
public:
	static float s_fDistLODs[MAX_CHR_LOD];
	static CN3Mesh* s_pBoxMesh;

public:
	int		m_nLOD; // Level Of Detail - ������ �����Ҽ� �ֵ��� �Ѵ�..
	float	m_fFrmPrev; // ������ Frame

protected:
	CN3LinkedList<CN3CPart*>	m_Parts; // �� ĳ������ �κк� Data Pointer List
	CN3LinkedList<CN3CPlug*>	m_Plugs; // �� ĳ���Ϳ� ���̴� ������� Data Pointer List
	CN3Joint*	m_pJointRef; // �Ѱ��� ���븸�� ����..
	CN3Joint*	m_pJointBlend; // �Ѱ��� ���븸�� ����..
	__Matrix44*	m_pMtxs; // ����Ʈ�� ���� ���
	__Matrix44*	m_pMtxInverses; // ����Ʈ�� ���� �����
	
//	CN3Skin*	m_pSkinCollision;
	
	__VertexColor	m_vPlugTraces[MAX_PLUG_TRACE][MAX_PLUG_TRACE_VERTEX]; // ���� ����
	bool			m_bRenderPlugTraces[MAX_PLUG_TRACE];
	DWORD			m_dwTickPlugPrevs[MAX_PLUG_TRACE];
	
	CN3AnimControl* m_pAniCtrlRef; // Animation Control Reference Pointer
	CN3AnimControl::__AnimData* m_pAniCur;

	int		m_nAniCur; // ���� ���ϸ��̼�
	int		m_nAniPrev; // ���� ���ϸ��̼�..
	int		m_nAniLoopCountCur; // ���ϸ��̼� ���� ī��Ʈ..

	float	m_fFrmCurUpper; // ���� ������ 0 - ��ü
	float	m_fFrmCurLower; // ���� ������ 1 - ��ü

	float m_fFrmBlendUpper;
	float m_fFrmBlendLower;
	float m_fBlendTimeCur; // ���� ������ �ð�..
	float m_fBlendTime; // �������� �ð�. (�ʴ���)

	__Vector3	m_vCollisionBoxes[8]; // �浹 üũ�� ���� ������
	WORD		m_wCollisionIndices[36]; // �浹 üũ�� ���� ������ �ε���

public:
	bool		CheckCollisionByBox(int xScreen, int yScreen);
	bool		CheckCollisionByBox(__Vector3& v0, __Vector3& v1, __Vector3 &vCollision, __Vector3 &vNormal);
	void		RemakePlugTracePolygons();
	__Matrix44 * MatrixGet(int nJointIndex) { return &m_pMtxs[nJointIndex]; }

//	void		CollisionSkinSet(const char* szFN);
//	CN3Skin*	CollisionSkin() { return m_pSkinCollision; }

	void		PartDelete(int nPart);
	int 		PartAdd();
	int			PartCount() { return m_Parts.Count(); }
	CN3CPart*	Part(int nPart) { if(nPart < 0 || nPart >= m_Parts.Count()) return NULL; return m_Parts.Get(nPart); }
	void		PartSet(int nPart, const char* szFN);

	void		PlugDelete(int nPlug);
	int			PlugAdd();
	int			PlugCount() { return m_Plugs.Count(); }
	CN3CPlug*	Plug(int nPlug) { if(nPlug < 0 || nPlug >= m_Plugs.Count()) return NULL; return m_Plugs.Get(nPlug); }
	void		PlugSet(int nPart, const char* szFN);

	void	Tick(float fFrm = FRAME_SELFPLAY);

	int		LODGet() { return m_nLOD; }

	CN3Joint*	Joint() { return m_pJointRef; }
	void		JointSet(const char* szFN);

	// Animation ���� �Լ�
	void	AniDefaultSet();
	CN3AnimControl* AniCtrl() { return m_pAniCtrlRef; }
	void	AniCtrlSet(const char* szFN);
	int		AniCurGet() { return m_nAniCur; }
	int		AniCurSet(int nAni, int nLoopCount = 0, float fBlendTime = 0.5f); // Animation ��ȣ, ���� ī��Ʈ(0�̸� ���ѷ���, 1�̻��̸� ī��Ʈ ��ŭ �����ϰ� ���� �ɷ� ���ư���.), �������ϴ� �ð�(�ʴ���)
//	int		AniCurSet(const char* szName, int nLoopCount);
	bool	IsLooping() { if(m_nAniLoopCountCur == 0) return true; else return false; }
	float	FrmCurUpper() { return m_fFrmCurUpper; }
	float	FrmCurLower() { return m_fFrmCurLower; }
	float	AniBlendDelta() { if(m_fBlendTime == 0) return 0.0f; return m_fBlendTimeCur / m_fBlendTime; }
	float	AniDeltaUpper()
	{
		if(NULL == m_pAniCur) return 0.0f;
		return (m_fFrmCurUpper - m_pAniCur->fFrmStartUpper)/(m_pAniCur->fFrmEndUpper - m_pAniCur->fFrmStartUpper); 
	}
	float	AniDeltaLower()
	{ 
		if(NULL == m_pAniCur) return 0.0f;
		return (m_fFrmCurLower - m_pAniCur->fFrmStartLower)/(m_pAniCur->fFrmEndLower - m_pAniCur->fFrmStartLower);
	}

	void Init();
	void BuildMesh();
	void Render(float fFrm);
	void Render2(float fFrm);

	bool Save(HANDLE hFile);
	bool Load(HANDLE hFile);

	void Release();
	CN3Chr();
	virtual ~CN3Chr();

};

#endif // !defined(AFX_N3Chr_h__INCLUDED_)
