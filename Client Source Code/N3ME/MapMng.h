// MapMng.h: interface for the CMapMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPMNG_H__995D141A_97C5_4CE7_B100_D729B35E9959__INCLUDED_)
#define AFX_MAPMNG_H__995D141A_97C5_4CE7_B100_D729B35E9959__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Cursor Mode
enum {CM_SELECT = 1, CM_OBJECT_BRUSH = 2, CM_POS = 3, CM_ROTATE = 4, CM_SCALE = 5,
 CM_EDIT_TERRAIN = 6, CM_SMOOTH_TERRAIN = 7, CM_SET_DTEX=8, CM_EDIT_RIVER=9,
 CM_EDIT_LIGHTMAP=10, CM_MAKE_NPCPATH=11, CM_MAKE_WALL=12, CM_EDIT_EVENT=13,
 CM_EDIT_POND=14, CM_FLAT_TERRAIN=15, CM_EDIT_REGENUSER=16, CM_EDIT_SOUND=17,
 CM_EDIT_SEED = 18, CM_EDIT_LIGHT = 19 };

const int NUM_UNIT_LIGHT = 2;

#include <afxtempl.h>
#include "../N3Base/N3Base.h"
#include "PosDummy.h"
#include "RotDummy.h"
#include "ScaleDummy.h"
#include "RiverMng.h"
#include "PondMng.h"
#include "LyTerrain.h"
#include "RegenUser.h"
#include "SowSeedMng.h"

class CMainFrame;
class CN3Scene;
class CN3Camera;
class CN3Light;
//class CDlgSceneGraph;
class CDlgShapeList;
class CBrushDlg;
class CN3Transform;
class CN3TransformCollision;
class CDlgBase;
class CN3Base;
class CNPCPathMgr;
class CWallMgr;
class CN3ShapeMgr;
class CEventMgr;
class CRegenUser;
class CWarpMgr;
class CSoundMgr;
class CLightObjMgr;

class CMapMng : public CN3Base
{
public:
	CMapMng(CMainFrame* pMainFrm);
	virtual ~CMapMng();

// Attributes
public:

	CN3Transform*	GetSelOutputObj(int iIndex) const 
	{
		if (iIndex < 0 || iIndex >= m_SelOutputObjArray.GetSize() )
			return NULL;
		return m_SelOutputObjArray.GetAt(iIndex);
	}
	int				GetSelOutputObjCount() const {return m_SelOutputObjArray.GetSize();};
	CLyTerrain*		GetTerrain() const { return m_pTerrain;}
	CWnd*			GetDlgSourceList() const {return (CWnd*)m_pDlgSourceList;}
	CWnd*			GetDlgOutputList() const {return (CWnd*)m_pDlgOutputList;}
	CWnd*			GetDlgBase() const {return (CWnd*)m_pDlgBase;}
	CN3Scene*       GetSceneOutput() { return m_pSceneOutput;}
	D3DSHADEMODE	m_ShadeMode;
	D3DFILLMODE		m_FillMode;
	bool			m_bViewWireFrame; // ���õ� ������Ʈ�� ���̾� ���������� ���� �Ѵ�..
	bool			m_bRenderAxisAndGrid;			// ��� �׸��带 �׸��� �ȱ׸��� ����..

	int				m_iZoneID; // �� ���̵�...
	bool			m_bHideObj;
	bool			m_bLoadingComplete; // �̰� ���̾�߸� �������� �Ѵ�..

	CMainFrame*			m_pMainFrm;						// Main ������
	CSowSeedMng			m_SowSeedMng;
	CDlgShapeList*		m_pDlgSourceList;			// Object ����� ������ ���̾˷α�
	CN3Transform*		GetSelSourceObj(){ return m_pSelSourceObj;}

protected:
	enum	ENUM_EDIT_STATE {eEDIT_NOT=0,eEDIT_COPY,eEDIT_PASTE,eEDIT_NUM};

	CLightObjMgr*		m_pLightObjMgr;	
	CSoundMgr*			m_pSoundMgr;
	CWarpMgr*			m_pWarpMgr;
	CRegenUser*			m_pRegenUser;
	CEventMgr*			m_pEventMgr;				//�� �̺�Ʈ ����...
	CWallMgr*			m_pWall;					//�� �ܰ� ���� ���ϴ� ������..
	CNPCPathMgr*		m_pNPCPath;					// NPC �� ����� Ŭ����..
	CLyTerrain*			m_pTerrain;						// ����
	CN3Scene*			m_pSceneSource;					// source object ��Ͽ� ������ �͵��� ���� Scene
	CN3Scene*			m_pSceneOutput;					// �ʿ� ��ġ�� Object�� ������ Scene
	
	//CDlgSceneGraph*		m_pDlgSourceList;			// Object ����� ������ ���̾˷α�
	//CDlgSceneGraph*		m_pDlgOutputList;			// �ʿ� ��ġ�� Object�� ������ ���̾˷α�
	CDlgShapeList*		m_pDlgOutputList;			// �ʿ� ��ġ�� Object�� ������ ���̾˷α�
	CDlgBase*			m_pDlgBase;						// ��ü ������� ���� ��ȭ����..
	CBrushDlg*			m_pBrushDlg;					// ���� ���� �귯�� �ɼ��������ִ� ��ȭ����
	

	CN3Transform*		m_pSelSourceObj;				// source object ��Ͽ��� ������ ��
//	CTypedPtrArray<CPtrArray, CN3TransformCollision*> m_SelOutputObjArray;	// �ʿ� ��ġ�� Object �� ������ �͵�
	CTypedPtrArray<CPtrArray, CN3Transform*>	m_SelOutputObjArray;	// �ʿ� ��ġ�� Object �� ������ �͵�

	ENUM_EDIT_STATE		m_eSelObjBackState;		//	�ӽú���(ctrl+c)�� ����ǥ��
	CTypedPtrArray<CPtrArray, CN3Transform*>	m_SelOutputObjBack;	// �ʿ� ��ġ�� Object �� ������ �͵� ������ ����

	int					m_CursorMode;					// ���콺�� ��� ��� ����
	BOOL				m_bRenderSelObj;				// source object ��Ͽ��� ���õ� ���� �׸��� �ȱ׸��� ����

	CTransDummy*		m_pDummy;						// ��ü�� �̵� ȸ�� Ȯ��/��� �ϴ� ����� ����ϴ� Ŭ����
	CPosDummy			m_PosDummy;						// �̵�
	CRotDummy			m_RotDummy;						// ȸ��
	CScaleDummy			m_ScaleDummy;					// Ȯ��/���
	CRiverMng			m_RiverMng;						// ���� ��������
	CPondMng			m_PondMng;						// ���� ��������

	RECT				m_rcSelDrag;					// �巡�� ����

	struct __Sort										// Object picking�Ҷ� �����ϴ� ����ü
	{
		CN3TransformCollision*	pObj;
		float		fCamDist;
		__Vector3	vMin, vMax;
	};
// Operations
public:
	void	SetLight(float fLgt);
	void	ActWarpMgr();
	bool	IsWarpMgrActive();
	void	DeleteSelectedSourceObjects();
	void	DeleteOverlappedObjects();
	void	DeleteUnusedFiles();
	void	SetZoneID(int id) { m_iZoneID = id; if(m_pTerrain) m_pTerrain->m_iZoneID = id; }	//���� �� ���̵� ����.
	void	ImportShape(const char* szFullPath);
	void	MakeTerrainMovableAttr(CN3ShapeMgr* pShapeMgr);		//�������� ���� �ִ� Ÿ�ϰ� �� �� ���� Ÿ���� �����ض�..
	void	ImportPostDataFromScene(const char* szFileName); // Scene ���� ������Ʈ ��ġ�Ȱ� �ҷ��´�..
	void	UpDateFP(); 
	void	Tick();
	void	Render();
	void	Release();
	void	SavePartition(float x, float z, float width);
	void	SaveToFile(LPCTSTR lpszPathName);			// Map ���� ����
	void	LoadFromFile(LPCTSTR lpszPathName);			// Map ���� �ҷ�����
	BOOL	MouseMsgFilter(LPMSG pMsg);			// ���콺�� ���
	void	ImportTerrain(const char* szMeshFN);			// VMesh ���Ͽ��� ���� ������ �о����
	void	ImportTerrainHeight(const char* szMeshFN);		// VMesh ���Ͽ��� ������ ���̰��� �о����..
	void	MakeGameFiles(LPCTSTR lpszPathName, float fSize = 128.0f);	// ���� �����ͷ� ��ȯ�ϱ�
	void	MakeServerDataFiles(LPCTSTR lpszPathName);
	void	SelectObject(CN3Base* pObj, BOOL IsSourceObj, BOOL bAdd = FALSE);	// ��ü�� �����Ѵ�.
	void	RenderObjectToWindow(CN3TransformCollision* pObj, HWND hWnd);	// Ư�� �����쿡 Object�� �׷��ش�.
	void	SetCursorMode(int iMode);						// ���콺Ŀ���� �̿����� �ٲ۴�.
	int		GetCursorMode() const {return m_CursorMode;}	// ���콺 Ŀ���� �̿����� ��´�.
	void	Invalidate();									// View ȭ�� ����
	void	FocusSelObj();									// ���õ� ��ü�� ���ͽ��� �ű��.
	void	FocusAll();										// ��ü ���� ���� �ֵ��� �Ѵ�.
	void	FocusAt(__Vector3 v);							// ������ ������ ��Ŀ�� �Ű�.
	void	DropSelObjToTerrain();							// ������ ��ü�� ������ ���δ�.(Y���� ����)
	void	DeleteSelObjectFromOutputScene();			// OutputScene���� ������ ��ü�� �����.
	void	RenderDragRect(RECT* rc);					// �巡�� ������ �׸���.
	void	UpdateAll();								// source �� output dialog�� update��Ų��.

	void	LoadObjectPostData(LPCTSTR lpszFileName);		// Shape ��ġ������ text���Ͽ��� �о�´�.
	void	SaveObjectPostData(LPCTSTR lpszFileName);		// Shape ��ġ������ text���Ϸ� �����Ѵ�.
	void	SaveObjectPostDataPartition(LPCTSTR lpszFileName, float psx, float psz, float width);
	void	RenderGrid(float fGridSize, float fMaxDistance);	// �ʿ� ������������Grid�� �׷��ش�
	void	OnSelChanged();								// ������ ��ü�� �ٲ������ ���ٰ͵�
	void	SelectObjectByDragRect(RECT* pRect, BOOL bAdd);	// �巡�� �ؼ� ��ü �����ϱ�
	void	LoadSourceObjects();						// Folder ���� Source Object �� �о�´�.
	CN3Transform* AddChr(CN3Scene* pDestScene, const std::string& szFN, BOOL bGenerateChainNumber);		// Ư��Scene�� ĳ���� ��ü�� ������ �߰�
	CN3Transform* AddShape(CN3Scene* pDestScene, const std::string& szFN, BOOL bGenerateChainNumber);	// Ư��Scene�� Shape ��ü�� ������ �߰�
	CN3Transform* AddObjectToOutputScene(CN3Transform* pObj);	// �ҽ���Ͽ��� ������ Object�� ������ OutputScene���� �����ؼ� �־��ش�.
	CN3Camera* CameraGet();

	CN3Base*	Pick(POINT point, int* pnPart);												// ��ü picking �Լ�
	static int SortByCameraDistance(const void *pArg1, const void *pArg2);	// ī�޶� �Ÿ��� ���� �����Լ� qsort�� �̿�

//	BOOL	CameraMove(LPMSG pMsg);		// ī�޶� �̵��� ���� �޼��� ó��
	BOOL	GetObjectMinMax(CN3Transform* pObj, __Vector3& vMin, __Vector3& vMax);	// ��ü�� �ִ��ּ��� ã��

	CStatusBar* GetStatusBar();

	void	SetEditState(ENUM_EDIT_STATE eEditStat);	//	��ο��ؼ� ������ ������Ʈ�� edit
};

#endif // !defined(AFX_MAPMNG_H__995D141A_97C5_4CE7_B100_D729B35E9959__INCLUDED_)

