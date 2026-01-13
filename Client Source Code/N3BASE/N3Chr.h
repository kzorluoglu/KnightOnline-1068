// N3Chr.h: interface for the CN3Chr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3Chr_h__INCLUDED_)
#define AFX_N3Chr_h__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786)

#include "N3TransformCollision.h"
#include "N3AnimControl.h"
#include "N3Skin.h"
#include "N3Cloak.h"
#include "N3PMeshInstance.h"
#include "N3Texture.h"
#include "N3Joint.h"
#include <list>
#include <vector>

const int MAX_CHR_JOINT = 256;
const int MAX_CHR_LOD = 4; // 4 �ܰ� Level Of Detail
const int MAX_CHR_LOD_DELTA = 4;

class CN3CPartSkins : public CN3BaseFileAccess
{
	friend class CN3Chr;
	friend class CN3CPart;
protected:
	CN3Skin			m_Skins[MAX_CHR_LOD];

public:
	bool			Load(HANDLE hFile);
#ifdef _N3TOOL
	bool			Save(HANDLE hFile);
#endif // end of _N3TOOL

	void Release();
	CN3CPartSkins();
	virtual ~CN3CPartSkins();
};

class CN3CPart : public CN3BaseFileAccess
{
	friend class CN3Chr;
public:
	__Material			m_Mtl;
	__Material			m_MtlOrg;
	DWORD				m_dwReserved;


	bool				m_bVisible;
protected:
	CN3Texture*			m_pTexRef;
	CN3Texture*			m_pTexOverlapRef; // ���� ��ĥ�� �ؽ�ó
//	std::vector<class CN3AnimatedTexture*> m_AnimatedTextures;
	CN3CPartSkins*		m_pSkinsRef;
public:
	bool			Load(HANDLE hFile);
	void			Render(int nLOD);

	CN3Texture*		Tex() { return m_pTexRef; }
	CN3Texture*		TexSet(const std::string& szFN) { s_MngTex.Delete(&m_pTexRef); m_pTexRef = s_MngTex.Get(szFN, true, s_Options.iTexLOD_Chr); return m_pTexRef; }
	void			TexSet(CN3Texture* pTex) { s_MngTex.Delete(&m_pTexRef); m_pTexRef = pTex; }
	
	CN3Texture*		TexOverlap() { return m_pTexOverlapRef; }  // ���� ��ĥ�� �ؽ�ó
	CN3Texture*		TexOverlapSet(const std::string& szFN);
	void			TexOverlapSet(CN3Texture* pTex);
	
	CN3Skin*		Skin(int nLOD)
	{
		if(m_pSkinsRef && nLOD >= 0 && nLOD < MAX_CHR_LOD)
			return &(m_pSkinsRef->m_Skins[nLOD]);
		else
			return NULL;
	}
	CN3CPartSkins*	Skins() { return m_pSkinsRef; }
	CN3CPartSkins*	SkinsSet(const std::string& szFN);

#ifdef _N3TOOL
	void			RenderSelected(int nLOD);
	bool			Save(HANDLE hFile);
#endif // end of _N3TOOL

	void Release();
	
	CN3CPart();
	virtual ~CN3CPart();
};

enum e_PlugType{PLUGTYPE_NORMAL=0, PLUGTYPE_CLOAK, PLUGTYPE_MAX=10, PLUGTYPE_UNDEFINED=0xffffffff};
class CN3CPlugBase : public CN3BaseFileAccess
{
	friend class CN3Chr;
public:
	e_PlugType			m_ePlugType;
	bool				m_bVisible;
	int					m_nJointIndex; // �ٴ� Mesh �� Joint Index.. ��� ����Ʈ�� �ٴ���.. �װ��� �����δ�...
	__Vector3			m_vPosition; // �ٴ� Mesh �� Offset Vector.
	__Vector3			m_vScale; // �ٴ� Mesh �� ������.
	__Material			m_Mtl;
	__Matrix44			m_Matrix;	

protected:
	CN3PMeshInstance	m_PMeshInst; // Progressive Mesh Instance
	CN3Texture*			m_pTexRef; // Texture Reference Pointer
	CN3Texture*			m_pTexOverlapRef; // ���� ��ĥ�� Texture Reference Pointer
//		__Vector3 m_vRotation; // �ٴ� Mesh �� Rotation��.
	__Matrix44			m_MtxRot; // Rotation Matrix;

public:
	CN3CPlugBase();
	virtual ~CN3CPlugBase();
	static e_PlugType	GetPlugTypeByFileName(const std::string& szFN);

	virtual bool	Load(HANDLE hFile);
#ifdef _N3TOOL
	virtual bool	Save(HANDLE hFile);
#endif // end of _N3TOOL

	virtual void Render(const __Matrix44& mtxParent, const __Matrix44& mtxJoint);
	virtual void Release();

	void ReCalcMatrix();

	__Vector3& Position() { return m_vPosition; }
	__Matrix44& Rotation() { return m_MtxRot; }
	__Vector3& Scale() { return m_vScale; }

	void PositionSet(const __Vector3& vPos) { m_vPosition = vPos; this->ReCalcMatrix(); }
	void RotationSet(const __Matrix44& mtx) { m_MtxRot = mtx; this->ReCalcMatrix(); }
	void ScaleSet(const __Vector3& vScale2) { m_vScale = vScale2; this->ReCalcMatrix(); }
	
	CN3Texture* Tex() { return m_pTexRef; }
	CN3Texture* TexSet(const std::string& szFN) { s_MngTex.Delete(&m_pTexRef); m_pTexRef = s_MngTex.Get(szFN, true, s_Options.iTexLOD_Chr); return m_pTexRef; }
	void		TexSet(CN3Texture* pTex) { s_MngTex.Delete(&m_pTexRef); m_pTexRef = pTex; }

	CN3Texture* TexOverlap() { return m_pTexOverlapRef; }  // ���� ��ĥ�� �ؽ�ó
	CN3Texture* TexOverlapSet(const std::string& szFN);
	void		TexOverlapSet(CN3Texture* pTex);
	
	CN3PMesh* PMesh() { return m_PMeshInst.GetMesh(); }
	CN3PMeshInstance* PMeshInst() { return &m_PMeshInst; }
	void PMeshSet(const std::string& szFN);
};

const int MAX_PLUG_FX_POSITION = 5;
const int MAX_FXTAIL = 3;
enum e_PlugAttr{PLUGATTR_FIRE=0, PLUGATTR_ICE, PLUGATTR_LIGHTENNING, PLUGATTR_POISON, PLUGATTR_TOTAL};
class CN3CPlug: public CN3CPlugBase
{
public:
	bool			m_bRenderTrace;
	int 			m_nTraceStep; // ������ ����..
	D3DCOLOR		m_crTrace; // ���� ����.. �������̸� ����..
	float			m_fTrace0; // ���� ��ġ..
	float			m_fTrace1; // ���� ��ġ..

	//fx.......
	CN3PMeshInstance			m_PMeshInstFX; // FX �� �� PMesh Instance
	class CN3FXBundle*			m_pFXMainBundle;
	class CN3FXBundle*			m_pFXTailBundle[MAX_FXTAIL];
	class CN3FXPartBillBoard*	m_pFXPart;
	std::string					m_strFXMainName;
	std::string					m_strFXTailName;
	
public:
	virtual bool	Load(HANDLE hFile);
#ifdef _N3TOOL
	virtual bool	Save(HANDLE hFile);
	void			ImportPMesh(const std::string& szFileName);
	void			RenderFXLines(const __Matrix44& mtxParent, const __Matrix44& mtxJoint); // FX �� ���� ���� �׷��ش�.
#endif // end of _N3TOOL
	virtual void	Render(const __Matrix44& mtxParent, const __Matrix44& mtxJoint);
	virtual void	Release();

	void RenderFX(const __Matrix44& mtxParent, const __Matrix44& mtxJoint);
	void InitFX(std::string& szFXMain, std::string& szFXTail, D3DCOLOR TraceCR = 0xffffffff);

public:
	CN3CPlug();
	virtual ~CN3CPlug();
};

class CN3CPlug_Cloak : public CN3CPlugBase
{
public:
	CN3CPlug_Cloak();
	virtual ~CN3CPlug_Cloak();	

public:
	virtual bool Load(HANDLE hFile);
#ifdef _N3TOOL
	virtual bool Save(HANDLE hFile);
#endif // end of _N3TOOL
	virtual void Render(const __Matrix44& mtxParent, const __Matrix44& mtxJoint);
	virtual void Release();

	void	SetLOD(int nLOD);
#ifdef _N3GAME
	CN3Cloak*	GetCloak(){return &m_Cloak;}
#endif
protected:
#ifdef _N3GAME
	CN3Cloak		m_Cloak;
#endif
};

const int MAX_CHR_ANI_PART = 2; // 0 - ��ü, 1 - ��ü ::: �������� ����� ����� ���ϸ��̼� ����..
const int MAX_PLUG_TRACE = 2; // �ִ� �ΰ��� ���� ������ �����..
const int MAX_PLUG_TRACE_VERTEX = 64; // ���� ���� ���� ��.. �� 8 ���δ� �ܻ��� 3�ܰ�� ���´�..
  
typedef std::vector<CN3CPart*>::iterator	it_CPart;
typedef std::vector<CN3CPlug*>::iterator	it_CPlug;

class CN3Chr : public CN3TransformCollision
{
	friend class CPlayerBase;
	friend class CPlayerMySelf;

public:
	int							m_nLOD; // Level Of Detail - ������ �����Ҽ� �ֵ��� �Ѵ�..
	int							m_nLOD_0; // Level Of Detail - 0���� �������� ������ ���� LOD ������ �����Ҽ� �ֵ��� �Ѵ�..

protected:
	static int s_iLODDelta; // LOD ��꿡 �ʿ��� �ε���..

	CN3Joint*					m_pRootJointRef; // �Ѱ��� ���븸�� ����..
	
	std::vector<CN3Joint*>		m_JointRefs; // �� ����Ʈ�� ���� ������
	std::vector<__Matrix44>		m_MtxJoints; // �� ����Ʈ�� ���.. �����ͷ� ���� ���� ������ �� ĳ���͸��� ���� ���ϸ��̼��� �Ǳ� �����̴�..
	std::vector<__Matrix44>		m_MtxInverses; // ����Ʈ�� ���� �����

	std::vector<CN3CPart*>		m_Parts; // �� ĳ������ �κк� Data Pointer List
	std::vector<CN3CPlug*>		m_Plugs; // �� ĳ���Ϳ� ���̴� ������� Data Pointer List
	std::vector<__VertexColor*>	m_vTraces; // Plug Trace Polygon
	class CN3FXPlug*			m_pFXPlug;	// ĳ���Ϳ� FX�� ���̴� ��.

	int							m_nJointPartStarts[MAX_CHR_ANI_PART]; // ����Ʈ�� �Ϻκ��� ���� ���ϸ��̼� �Ǿ� �Ѵٸ�.. ����Ʈ �ε��� ���� ��ȣ
	int							m_nJointPartEnds[MAX_CHR_ANI_PART]; // ����Ʈ�� �Ϻκ��� ���� ���ϸ��̼� �Ǿ� �Ѵٸ�.. ����Ʈ �ε��� �� ��ȣ
	
//	CN3Skin*	m_pSkinCollision;
	
	CN3AnimControl*				m_pAniCtrlRef; // Animation Control Reference Pointer

	struct __FrmCtrl
	{
		__AnimData*		pAniData;	// ���� ���ϸ��̼� ������ ������..

		int		iAni;					// ���� ���ϸ��̼�
		bool	bOnceAndFreeze;			// �ѹ��� �ϰ� ����� �ϴ°�??
		bool	bProcessingDelayNow;	// ���� �����ð��� ó���ϴ°�??
		float	fFrmCur;				// ���� ������
		float	fFrmPrev;				// �ֱ� ������
		int		iAniLoop;				// ���ϸ��̼��� ������??
		float	fBlendFrm;				// ���� ���ϸ��̼��� ���������Ӱ� �������� ������
		float	fBlendTimeCur;			// ���� ������ �ð�..
		float	fBlendTime;				// �������� �ð�. (�ʴ���)
		float	fFreezeTime;			// ���� �ð�..

		void Init()
		{
			pAniData = NULL;
			iAni = -1;						// ���� ���ϸ��̼�
			bOnceAndFreeze = false;			// ���ư� ���ϸ��̼�..
			bProcessingDelayNow = false;	// ���� �����ð��� ó���ϴ°�??

			fFrmCur = 0;
			fFrmPrev = 0;					// �ֱ� ������
			iAniLoop = 0;					// ���ϸ��̼��� �����°�?
			fBlendFrm = 0;					// �������� ���ϸ��̼� ������ ������..
			fBlendTimeCur = 0;				// ���� ������ �ð�..
			fBlendTime = 0;					// �������� �ð�. (�ʴ���)
		}
		__FrmCtrl()
		{
			__FrmCtrl::Init();
		}
	};
	__FrmCtrl		m_FrmCtrl;
	__FrmCtrl		m_FrmCtrlUpper;	// ��ü��...
	float			m_fAniSpeedDelta; // ���ϸ��̼� �ӵ� ���� ���� 1 �̺���, �� ũ�� ��������..

public:
	int				CheckCollisionPrecisely(const __Vector3& vPos, const __Vector3& vDir, __Vector3* pvPick = NULL);
	int				CheckCollisionPrecisely(int ixScreen, int iyScreen, __Vector3* pvPick = NULL);
	static void		LODDeltaSet(int iLODDelta) { if(s_iLODDelta >= 0 && iLODDelta <= 3) s_iLODDelta = iLODDelta; }
	static int 		LODDelta() { return s_iLODDelta; }

	void			FindMinMax(); // �ִ� �ּҰ��� ã�´�.
	int				JointPartStart(int nAniPart) { if(nAniPart < 0 || nAniPart >= MAX_CHR_ANI_PART) return -1; return m_nJointPartStarts[nAniPart]; }
	int				JointPartEnd(int nAniPart) { if(nAniPart < 0 || nAniPart >= MAX_CHR_ANI_PART) return -1; return m_nJointPartEnds[nAniPart]; }
	void			JointPartSet(int nAniPart, int nJS, int nJE);

	const __Matrix44*	MatrixGet(int nJointIndex) const
	{
		if((int)m_MtxJoints.size() > nJointIndex && nJointIndex >=0) 
			return &(m_MtxJoints[nJointIndex]);
		return NULL;
	}

//	void		CollisionSkinSet(const std::string& szFN);
//	CN3Skin*	CollisionSkin() { return m_pSkinCollision; }

	void		PartDelete(int iIndex);
	void		PartAlloc(int nCount);
	int			PartCount() { return m_Parts.size(); }
	CN3CPart*	PartSet(int iIndex, const std::string& szFN);
	CN3CPart*  	PartAdd() { CN3CPart* pPart = new CN3CPart(); m_Parts.push_back(pPart); return pPart; }
	CN3CPart*	Part(int iIndex) { if(iIndex < 0 || iIndex >= (int)m_Parts.size()) return NULL; return m_Parts[iIndex]; }

	void		PlugDelete(int iIndex);
	void		PlugAlloc(int nCount);
	int			PlugCount() { return m_Plugs.size(); }
	CN3CPlug*	PlugSet(int iIndex, const std::string& szFN);
	CN3CPlug*	PlugAdd(e_PlugType eType=PLUGTYPE_NORMAL) { CN3CPlug* pPlug = new CN3CPlug(); m_Plugs.push_back(pPlug); return pPlug; }
	CN3CPlug*	Plug(int iIndex) { if(iIndex < 0 || iIndex >= (int)m_Plugs.size()) return NULL; return m_Plugs[iIndex]; }

	void		Tick(float fFrm = FRAME_SELFPLAY);
	void		TickAnimationFrame();
	void		TickJoints();
	void		TickPlugs(float fLOD);
	void		RemakePlugTracePolygons();

	float		Height() { return m_vMax.y * m_vScale.y; } // �������� ������ Ű ...
	float		Radius() { return m_fRadius * m_vScale.y; } // �������� ������ �ʺ�...

	CN3Joint*	Joint() { return m_pRootJointRef; }
	void		JointSet(const std::string& szFN);

	// Animation ���� �Լ�
#ifdef _N3TOOL
	void		AniDefaultSet();
#endif // #ifdef _N3TOOL
	CN3AnimControl* AniCtrl() { return m_pAniCtrlRef; }
	void		AniCtrlSet(const std::string& szFN);
	int			AniIndexCur() { return m_FrmCtrl.iAni; }
	int			AniCurSet(	int iAni,							// Animation ��ȣ,
							bool bOnceAndFreeze = false,		// �ѹ��� ���� ���߾�� �ϴ°�??
							float fBlendTime = FLT_MIN,			// �������ϴ� �ð�(�ʴ���), 
							float fFreezeTime = 0, 				// ����ð�...
							bool bStopUpperAnimation = true);	// ��ü ���ϸ��̼��� ������.. ���ߵ���
	void		AniUpperSet(int nAni, float fFreezeTime = 0);
	void		AniFixToLastFrame( int iAni ); // ������ ���������� ���� ���� ������.

	bool		IsAnimEnd(); // �ѹ� �ϰ� ���ߴ� ���ϸ��̼� �ϰ�� .. ��������?? Ȥ�� �ѹ��ϰ� ���� �������� ���ư��� ���ϸ��̼��� ��� ���ư�����?

	float		FrmCur() { return m_FrmCtrl.fFrmCur; }
	float		FrmPrev() { return m_FrmCtrl.fFrmPrev; }
	float		AniBlendDelta();
	float		AniSpeedDelta() { return m_fAniSpeedDelta; } // ���ϸ��̼� �ӵ� ���� ���� 1 �̺���, �� ũ�� ��������..
	void		AniSpeedDeltaSet(float fDelta) { if(fDelta > 0.1f && fDelta < 10.0f) m_fAniSpeedDelta = fDelta; } // ���ϸ��̼� �ӵ� ���� ���� 1 �̺���, �� ũ�� ��������..

	// Sound Player ���� �Լ�
	bool		NeedPlaySound0();
	bool		NeedPlaySound1();

	// ��¥ Ÿ���� ���� �Լ�..
	bool		NeedStrike0();
	bool		NeedStrike1();

	// ������ �ٽ� ���۵Ǵ� Ÿ�̹��ΰ�?
	bool		IsLoopingAgain();
	
//////////////////////////////////////////////////
//	Coded (By Dino On 2002-10-10 ���� 2:35:32 )
//	FXPlug
	class CN3FXPlug*	FXPlugSet(const std::string& strFN);
	class CN3FXPlug*	FXPlugCreate();
	class CN3FXPlug*	FXPlugGet() {return m_pFXPlug;}
	void				FXPlugDelete();
//	End Of Code (By Dino On 2002-10-10 ���� 2:35:32 )
//////////////////////////////////////////////////

	void		Init();
	void		BuildMesh();
	void		BuildMesh(int nLOD);
	void		Render();

	__AnimData* AniDataCur();
#ifdef _N3TOOL	
	__AnimData* AniDataUpper();
	bool		Save(HANDLE hFile);
#endif // end of _N3TOOL
	bool		Load(HANDLE hFile);

	void		Release();
	
	CN3Chr();
	virtual ~CN3Chr();

};

inline bool CN3Chr::IsAnimEnd() // �ѹ� �ϰ� ���ߴ� ���ϸ��̼� �ϰ�� .. ��������?? Ȥ�� �ѹ��ϰ� ���� �������� ���ư��� ���ϸ��̼��� ��� ���ư�����?
{
	if(m_FrmCtrlUpper.pAniData)
	{
		if(m_FrmCtrlUpper.iAniLoop > 0)	return true;
		return false;
	}
	else
	{
		if(m_FrmCtrl.iAniLoop > 0) return true;
		return false;
	}
}

inline float CN3Chr::AniBlendDelta()
{
	if(m_FrmCtrl.fBlendTime == 0) return 0.0f;
	return m_FrmCtrl.fBlendTimeCur / m_FrmCtrl.fBlendTime;
}

// Sound Player ���� �Լ�
inline bool	CN3Chr::NeedPlaySound0()
{
	if(NULL == m_FrmCtrl.pAniData) return false;
	
	if(	m_FrmCtrlUpper.pAniData)
	{
		if(m_FrmCtrlUpper.pAniData->fFrmSound0 <= 0) return false;
		if(	m_FrmCtrlUpper.fFrmPrev < m_FrmCtrlUpper.pAniData->fFrmSound0 &&
			m_FrmCtrlUpper.fFrmCur >= m_FrmCtrlUpper.pAniData->fFrmSound0 ) return true;
		return false;
	}

	if(m_FrmCtrl.pAniData->fFrmSound0 <= 0) return false;
	if(	m_FrmCtrl.fFrmPrev < m_FrmCtrl.pAniData->fFrmSound0 &&
		m_FrmCtrl.fFrmCur >= m_FrmCtrl.pAniData->fFrmSound0 ) return true;

	return false;
}

inline bool CN3Chr::IsLoopingAgain()	// ������ �ٽ� ���۵Ǵ� Ÿ�̹��ΰ�?
{
	if(NULL == m_FrmCtrl.pAniData) return false;

	if(	m_FrmCtrlUpper.pAniData)
	{
		if (m_FrmCtrlUpper.iAniLoop <= 0) return false;
		if(	m_FrmCtrlUpper.fFrmPrev > m_FrmCtrlUpper.fFrmCur) return true;
	}

	if(m_FrmCtrl.iAniLoop <= 0) return false;	// ������ �ƴϴ�.
	if(	m_FrmCtrl.fFrmPrev > m_FrmCtrl.fFrmCur) return true;

	return false;
}

inline bool CN3Chr::NeedPlaySound1()
{
	if(NULL == m_FrmCtrl.pAniData) return false;

	if(	m_FrmCtrlUpper.pAniData)
	{
		if(m_FrmCtrlUpper.pAniData->fFrmSound1 <= 0) return false;
		if(	m_FrmCtrlUpper.fFrmPrev < m_FrmCtrlUpper.pAniData->fFrmSound1 &&
			m_FrmCtrlUpper.fFrmCur >= m_FrmCtrlUpper.pAniData->fFrmSound1 ) return true;
		return false;
	}

	if(m_FrmCtrl.pAniData->fFrmSound1 <= 0) return false;
	if(	m_FrmCtrl.fFrmPrev < m_FrmCtrl.pAniData->fFrmSound1 &&
		m_FrmCtrl.fFrmCur >= m_FrmCtrl.pAniData->fFrmSound1 ) return true;

	return false;
}

inline bool	CN3Chr::NeedStrike0()
{
	if(NULL == m_FrmCtrl.pAniData) return false;

	if(	m_FrmCtrlUpper.pAniData)
	{
		if(m_FrmCtrlUpper.pAniData->fFrmStrike0 <= 0) return false;
		if(	m_FrmCtrlUpper.fFrmPrev < m_FrmCtrlUpper.pAniData->fFrmStrike0 &&
			m_FrmCtrlUpper.fFrmCur >= m_FrmCtrlUpper.pAniData->fFrmStrike0 ) return true;
		return false;
	}

	if(m_FrmCtrl.pAniData->fFrmStrike0 <= 0) return false;
	if(	m_FrmCtrl.fFrmPrev < m_FrmCtrl.pAniData->fFrmStrike0 &&
		m_FrmCtrl.fFrmCur >= m_FrmCtrl.pAniData->fFrmStrike0 ) return true;

	return false;
}

inline bool	CN3Chr::NeedStrike1()
{
	if(NULL == m_FrmCtrl.pAniData) return false;

	if(	m_FrmCtrlUpper.pAniData)
	{
		if(m_FrmCtrlUpper.pAniData->fFrmStrike1 <= 0) return false;
		if(	m_FrmCtrlUpper.fFrmPrev < m_FrmCtrlUpper.pAniData->fFrmStrike1 &&
			m_FrmCtrlUpper.fFrmCur >= m_FrmCtrlUpper.pAniData->fFrmStrike1 ) return true;
		return false;
	}

	if(m_FrmCtrl.pAniData->fFrmStrike1 <= 0) return false;
	if(	m_FrmCtrl.fFrmPrev < m_FrmCtrl.pAniData->fFrmStrike1 &&
		m_FrmCtrl.fFrmCur >= m_FrmCtrl.pAniData->fFrmStrike1 ) return true;

	return false;
}

#endif // !defined(AFX_N3Chr_h__INCLUDED_)
