// PondMng.h: interface for the CPondMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PONDMNG_H__B71BF3BD_5F4F_4E5B_9F9F_B8BD45E2685B__INCLUDED_)
#define AFX_PONDMNG_H__B71BF3BD_5F4F_4E5B_9F9F_B8BD45E2685B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../N3Base/N3BaseFileAccess.h"
#include "VtxPosDummy.h"
#include <afxtempl.h>
#include "PondMesh.h"

enum {PCM_NONE=0, PCM_SELECT=1, PCM_CREATE=2, PCM_SELECTING};

//class CPondMesh;
class CMainFrame;
class CDlgPondProperty;

#include <list>
typedef std::list<CPondMesh*>::iterator it_PondMesh;
typedef std::list<__Vector3*>::iterator it_SelVtxBak;

class CPondMng  : public CN3BaseFileAccess
{
public:
	CPondMng(CMainFrame* pMainFrm);
	virtual ~CPondMng();

	struct __VertexPond
	{
	public:
		float x,y,z;
		float nx,ny,nz;
		D3DCOLOR	color;
		float u,v, u2,v2;

		void Set(__VertexXyzT2& __Vtx,float snx, float sny, float snz,DWORD dwColor) 
		{
			x = __Vtx.x, y = __Vtx.y, z = __Vtx.z;
			u = __Vtx.tu, v = __Vtx.tv;
			u2 = __Vtx.tu2, v2 = __Vtx.tv2;

			nx = snx, ny = sny, nz = snz;
			color = dwColor;
		}
	};

// Attributes
public:
	CPondMesh*	GetSelPond();
	int			GetDrawPondNum() {return m_PondMeshes.size();}
	it_PondMesh	GetDrawPond();
	int GetPCursorMode() const {return m_PCursorMode;}
protected:
	std::list<CPondMesh*>			m_PondMeshes;				// Pond Mesh list
	std::list<CPondMesh*>			m_pSelPonds;				// ���õ� PondMesh
	CTypedPtrArray<CPtrArray, __VertexXyzT2*>	m_SelVtxArray;	// ���õ� ����
	std::list<__Vector3*>			m_SelVtxBakArray;
	CVtxPosDummy					m_VtxPosDummy;				// ���� �����ϼ� �ִ� dummy object
	BOOL							m_bEditMode;				// ������ ��������ΰ�?
	BOOL							m_bChooseGroup;				// ������ �׷����� �����ϴ°�
	BOOL							m_bChooseEditPond;			// ���� ���� ������������ �����Ұ��ΰ�
	BOOL							m_bMovePond;				// ������ ������ ��°�� ������ ���ΰ�
	int								m_PCursorMode;				// ������ �߰�?�ϴ� ���ΰ� ���� �����ϴ� ���ΰ�?

	CMainFrame*						m_pMainFrm;					// mainframe pointer
	CDlgPondProperty*				m_pDlgProperty;				// property dialog

	RECT							m_rcSelDrag;				// �巡�� ����
	__VertexXyzColor				m_CreateLine[5];			// ���� ó�� �����Ҷ� ���̴� ��

	__Vector3						m_vPondsCenter;				//	����(��)�� �߰���

	BOOL							m_bShift;

// Operations
public:
	void			Tick();
	void			Render();
	virtual void	Release();
	virtual bool	Load(HANDLE hFile);
	virtual bool	Save(HANDLE hFile);

	BOOL			MouseMsgFilter(LPMSG pMsg);								// Mouse �޼��� ó��
	void			MakeGameFiles(HANDLE hFile, float fSize);				// Game file�� ����

	CPondMesh*		CreateNewPondMesh();									// �������� ������(�簢��)���� ���ο� ������ �߰��Ѵ�.
	
	BOOL			SetPondID(CPondMesh* pPondMesh, int iPondID);			// ������ ID�� ����
	void			SetEditMode(BOOL bEditMode);							// ���� EditMode����
	CPondMesh*		GetPondMesh(int iPondID);								// PondMesh ���

	void			MainInvalidate();	// ȭ�� ����� �ٽ� ���

	//	CDlgPondProperty���� ���� �Լ���
	void			RemovePondMesh(int iPondID);							// ���õ� ������ �����Ѵ�.
	void			GoPond(int iPondID);									// ���õ� �������� ���ϴ�.
	void			StationPond();											// ���õ� ������ ������ �ٽ� ��ġ
	void			MovePond();												// ���õ� ������ ��° �ű�� �Ѵ�

	BOOL			GetChooseGroup() {return m_bChooseGroup;}
	BOOL			GetChooseEditPond() {return m_bChooseEditPond;}
	void			ChooseGroupPond() {m_bChooseGroup^=1;}
	void			ChooseEditPond() {m_bChooseEditPond^=1;}

	//	���߿� ��������
	void			ReCalcSelectedVertex();									// ������ �����Ͽ� ���� �ٽ� ����
	void			ReCalcUV();												// ���õ� ������ UV��ǥ �ٽ� ���.
protected:
	void			SetSelPond(CPondMesh* pPondMesh,BOOL bChooseGroup = FALSE);					// PondMesh �����ϱ�
	BOOL			SelectVtxByDragRect(RECT* pRect, BOOL bAdd,BOOL bSelectPond=FALSE);
	void			ClearSelectRcAllPond();	// ��� ������ ������ ������ �ӽÿ��� �ʱ�ȭ �ϱ�

	void			SelPondRelease();
	void			SelPondDelete(CPondMesh* pPondMesh);
	void			SetSelPonds(CPondMesh* pPondMesh);
	void			SetRotatePonds(float fMove);

	void			SetVtxCenter();

	void			VtxBackupRelease();
	void			SetVtxBackup();
	void			ReSetVtxBackup();

	void			ReSetDrawRect(__Vector3 vStrPos,__Vector3 vEndPos);	//	������ �׸������� ���� ������
	void			InputDummyMovePos(__Vector3 vMovePos);	//	���̰� ������ ��ŭ ������ ������ �Է�
};

#endif // !defined(AFX_PONDMNG_H__B71BF3BD_5F4F_4E5B_9F9F_B8BD45E2685B__INCLUDED_)
