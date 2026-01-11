// PondMesh.h: interface for the CPondMesh class.
//
// ���� ����ū �簢���� ������ ���� �����̵� ������ ���� (4m ���� ������ �Ÿ�)
// client���� ����
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PONDMESH_H__314B2191_7B76_461C_AFD2_FDBC105914FC__INCLUDED_)
#define AFX_PONDMESH_H__314B2191_7B76_461C_AFD2_FDBC105914FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_PONDMESH_LINE		200
#define MAX_PONDMESH_VERTEX		200*4
//#define MAX_PONDMESH_INDEX		199*18
//#define INDEXBUFFER_PERLINE		18

#include "../N3Base/N3BaseFileAccess.h"
#include <list>

class CLyTerrain;

class CPondMesh : public CN3BaseFileAccess
{
public:
	//	�����Լ�
	CN3Texture*		TexGet() const {return m_pTexture;}
	int				VertexCount() const {return m_iVC;}
	int				RectVCount() const {return m_iRectVC;}
	int				IndexCount() const {return m_iIC;}
	int				LastVertexCount() const {return GetWaterScaleWidht();}

	void			Rotation(__Matrix44 mRot,__Vector3 vCenter);

protected:
	//	���õ� ������
	typedef struct __SELECT_PO
	{
		int		ix;	//	m_iWaterScaleWidth ���� ����
		int		iz;	//	m_iWaterScaleHeight ���� ����
	};

	typedef std::list<__SELECT_PO*>::iterator it_SelVtx;
	std::list<__SELECT_PO*>			m_vSelect;	//	���õ� ������ ����

	//	�⺻ ����Ÿ
	int				m_iPondID;
	float			m_fWaterHeight;	//	������
	int				m_iWaterScaleWidth;	//	���ι����� ���� ����
	int				m_iWaterScaleHeight;//	���ι����� ���� ����
	DWORD			m_dwPondAlpha;		//	���� ����
	float			m_fTU;
	float			m_fTV;
	float			m_fWaterScaleX,m_fWaterScaleZ;	//	���������� ������ �Ÿ�

	int				m_iBackUpWidht;
	int				m_iBackUpHeight;

	__Vector3		m_pVertices[MAX_PONDMESH_VERTEX];	//	����,��������� ���̴� ��
	__VertexXyzT2	m_pViewVts[MAX_PONDMESH_VERTEX];	//	ȭ�鿡 �ѷ����� ��
	WORD*			m_pdwIndex;//[MAX_PONDMESH_LINE*MAX_PONDMESH_LINE*6];
	int				m_iIC;					// Index Buffer Count.
	int				m_iVC;					// Vertex Count.
	CN3Texture*		m_pTexture;

	BOOL			m_bUVState;			//	TRUE ���� �����Ͽ�, FALSE �����ϰ� (uv����)
	
	//	���������� ���� ����Ÿ
	__Vector3		m_vDrawBox[4];		//	ȭ��� ���� ����(���������� �ð��������)
	__VertexXyzColor m_ViewRect[5];		//	ȭ�鿡 �׸���
	int				m_iRectVC;
	__VertexXyzT2	m_pRectVts[100];	//	�����ٿ� �׷��� ����

	__Vector3		m_vSelectBox[2];	//	������ ���������� ������ ����(������ ȭ�鿵���� �������)

	//	�������� ����
	CLyTerrain*		m_pTerrain;

public:
	CPondMesh();
	virtual ~CPondMesh();

	virtual void	Release();
	virtual bool	Load(HANDLE hFile);
	virtual bool	Load1000(HANDLE hFile);
	virtual bool	Load1001(HANDLE hFile);
	virtual bool	Save(HANDLE hFile);
	void			Render();
	void			RenderVertexPoint();	// �ߺ��̰� ���� �ٽ� �׸���

	__Vector3		GetCenter();
	float			SettingDrawPos(__Vector3 vPos1,__Vector3 vPos2,__Vector3 vPos3,__Vector3 vPos4,int iLinePosNum);
	__VertexXyzT2*	const GetVertex(int iIndex) {if (iIndex<0 || iIndex>=m_iVC) return NULL; return m_pViewVts+iIndex;}
	__VertexXyzT2*	const GetRcVertex(int iIndex) {if (iIndex<0 || iIndex>=m_iRectVC) return NULL; return m_pRectVts+iIndex;}

	BOOL			SetTextureName(LPCTSTR pszFName);

public:
	//	���� �߰��Ѱų� ��ģ��
	void			SetTerrain(CLyTerrain* pCTerrain) {m_pTerrain = pCTerrain;}

	void			MakeDrawRect(__Vector3* pvPos);	//	ȭ�鿡 ���� ������ �����׵θ� �����
	void			UpdateDrawPos();		//	�����ٿ� �׷��� ���� 

	void			UpdateWaterHeight();	//	������ ���� ���̷� ����
	void			UpdateMovePos(__Vector3 vMovingPos);	//	�̵�

	void			MakePondPos();	//	���� �ڽ����� ������ �����
	void			MakeIndex();
	void			ReCalcUV();		//	���� ������� �����ϰ� �Ѹ���
	void			ReCalcVexUV();	//	���� �����Ͽ� �Ѹ���

	void			EstimatePos();	//	���� ũ�⿡ ���� ������ ������ ���Ƿ� ����////

	void			ClearSelectPos();	//	�������� ���� ������ �ʱ�ȭ
	void			InputSelectPos(float fX,float fY,float fZ,int iVC=-1);	//	������ ���鿡 ���� �ڷ��Է�
	BOOL			InputDummyMovingPos(__Vector3 vDummyMovingPos,BOOL bMovePond);	//	���̰� ���������� ���������� ó���ϱ� ����

protected:
	void			MovingPos();
	void			CalcuWidth(int iSx,int iSy,int iEx=-1,int iEy=-1);	//	ó�� �¿���� ����� �Ѵ�
	void			SetAllPos(int iSx,int iSy,int iEx=-1,int iEy=-1);	//	ó�� ���������ܿ� ���� ��ǥ ���
	void			ReInputBackPos();	//	���� �ű���ǥ�� ����� �־��� 

public:	//	���̾�α� ���ڿ��� ���̴� �Լ�
	void			SetPondID(int iPondID)				{m_iPondID = iPondID;}
	void			SetWaterHeight(float fHeight);		
	void			SetWaterScaleWidht(int iWidhtNum)	{m_iWaterScaleWidth = iWidhtNum;	UpdateDrawPos();MakePondPos();}
	void			SetWaterScaleHeight(int iHeightNum) {m_iWaterScaleHeight = iHeightNum;	UpdateDrawPos();MakePondPos();}
	void			SetAlphaFactor(DWORD dwAlpha)		{m_dwPondAlpha = dwAlpha;}
	void			SetTU(float fTU)					{m_fTU = fTU;ReCalcUV();}
	void			SetTV(float fTV)					{m_fTV = fTV;ReCalcUV();}
	void			SetLeft(float fLeft);
	void			SetTop(float fTop);
	void			SetRight(float fRight);
	void			SetBottom(float fBottom);
	void			SetChangUVState()		{m_bUVState^=1;}

	int				GetPondID()				const {return m_iPondID;}
	float			GetWaterHeight()		const {return m_fWaterHeight;}
	int				GetWaterScaleWidht()	const {return m_iWaterScaleWidth;}
	int				GetWaterScaleHeight()	const {return m_iWaterScaleHeight;}
	DWORD			GetAlphaFactor()		const {return m_dwPondAlpha;}
	float			GetTU()					const {return m_fTU;}
	float			GetTV()					const {return m_fTV;}
	float			GetLeft()				const {return m_vDrawBox[0].x;}
	float			GetTop()				const {return m_vDrawBox[0].z;}
	float			GetRight()				const {return m_vDrawBox[2].x;}
	float			GetBottom()				const {return m_vDrawBox[2].z;}
	float			GetWaterWidthLength()	const {return m_fWaterScaleX;}
	float			GetWaterHeightLength()	const {return m_fWaterScaleZ;}
	BOOL			GetChangUVState()		const {return m_bUVState;}
};

#endif // !defined(AFX_PONDMESH_H__314B2191_7B76_461C_AFD2_FDBC105914FC__INCLUDED_)
