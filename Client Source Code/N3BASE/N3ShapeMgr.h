// N3ShapeMgr.h: interface for the CN3ShapeMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3SHAPEMGR_H__36456F66_1D60_4589_A5D9_70B94C2C3127__INCLUDED_)
#define AFX_N3SHAPEMGR_H__36456F66_1D60_4589_A5D9_70B94C2C3127__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786)

#ifdef _3DSERVER
#include "My_3DStruct.h"
#else
#include "N3BaseFileAccess.h"
#endif // end of #ifndef _3DSERVER


const int CELL_MAIN_DEVIDE = 4; // ���μ��� 4 X 4 �� ���꼿�� ������..
const int CELL_SUB_SIZE = 4; // 4 Meter �� ���꼿�� �������̴�..
const int CELL_MAIN_SIZE = CELL_MAIN_DEVIDE * CELL_SUB_SIZE; // ���μ� ũ��� ���꼿���� X ���꼿 ũ���̴�.
const int MAX_CELL_MAIN = 4096 / CELL_MAIN_SIZE; // ���μ��� �ִ� ������ ����ũ�� / ���μ�ũ�� �̴�.
const int MAX_CELL_SUB = MAX_CELL_MAIN * CELL_MAIN_DEVIDE; // ���꼿 �ִ� ������ ���μ� * ���μ������� �̴�.

#ifdef _3DSERVER
class CN3ShapeMgr
#else
#include <list>
#include <vector>
typedef std::list<class CN3Shape*>::iterator	it_Shp;
typedef std::list<__Vector3>::iterator			it_Vector3;
class CN3ShapeMgr : public CN3BaseFileAccess
#endif // end of #ifndef _3DSERVER
{
public:
	struct __CellSub // ���� �� ������
	{
		int 	nCCPolyCount; // Collision Check Polygon Count
		DWORD*	pdwCCVertIndices; // Collision Check Polygon Vertex Indices - wCCPolyCount * 3 ��ŭ �����ȴ�.

		void Load(HANDLE hFile)
		{
			DWORD dwRWC = 0;
			ReadFile(hFile, &nCCPolyCount, 4, &dwRWC, NULL);
			if(nCCPolyCount > 0)
			{
				if(pdwCCVertIndices) delete [] pdwCCVertIndices;
				pdwCCVertIndices = new DWORD[nCCPolyCount * 3];
				__ASSERT(pdwCCVertIndices, "New memory failed");
				ReadFile(hFile, pdwCCVertIndices, nCCPolyCount * 3 * 4, &dwRWC, NULL);
//#if _DEBUG				
//				static char szTrace[256];
//				sprintf(szTrace, "CollisionCheckPolygon : %d\n", nCCPolyCount);
//				OutputDebugString(szTrace);
//#endif
			}
		}
#ifdef _N3TOOL
		void Save(HANDLE hFile)
		{
			DWORD dwRWC = 0;
			WriteFile(hFile, &nCCPolyCount, 4, &dwRWC, NULL);
			if(nCCPolyCount > 0)
				WriteFile(hFile, pdwCCVertIndices, nCCPolyCount * 3 * 4, &dwRWC, NULL);
		}
#endif // end of _N3TOOL

		__CellSub() { memset(this, 0, sizeof(__CellSub)); }
		~__CellSub() { delete [] pdwCCVertIndices; }
	};

	struct __CellMain // �⺻ �� ������
	{
		int		nShapeCount; // Shape Count;
		WORD*	pwShapeIndices; // Shape Indices
		__CellSub SubCells[CELL_MAIN_DEVIDE][CELL_MAIN_DEVIDE];

		void Load(HANDLE hFile)
		{
			DWORD dwRWC = 0;
			ReadFile(hFile, &nShapeCount, 4, &dwRWC, NULL);
			if(nShapeCount > 0)
			{
				if(pwShapeIndices) delete [] pwShapeIndices;
				pwShapeIndices = new WORD[nShapeCount];
				ReadFile(hFile, pwShapeIndices, nShapeCount * 2, &dwRWC, NULL);
			}
			for(int z = 0; z < CELL_MAIN_DEVIDE; z++)
			{
				for(int x = 0; x < CELL_MAIN_DEVIDE; x++)
				{
					SubCells[x][z].Load(hFile);
				}
			}
		}

#ifdef _N3TOOL
		void Save(HANDLE hFile)
		{
			DWORD dwRWC = 0;
			WriteFile(hFile, &nShapeCount, 4, &dwRWC, NULL);
			if(nShapeCount > 0) WriteFile(hFile, pwShapeIndices, nShapeCount * 2, &dwRWC, NULL);
			for(int z = 0; z < CELL_MAIN_DEVIDE; z++)
			{
				for(int x = 0; x < CELL_MAIN_DEVIDE; x++)
				{
					SubCells[x][z].Save(hFile);
				}
			}
		}
#endif // end of _N3TOOL
		
		__CellMain() { nShapeCount = 0; pwShapeIndices = NULL; }
		~__CellMain() { delete [] pwShapeIndices; }
	};

	__Vector3* 				m_pvCollisions;

protected:
#ifndef _3DSERVER
	std::vector<CN3Shape*>	m_Shapes;			// ����Ʈ�� �� ���� ������... �迭�� �ξ� ȿ�����̱� �����̴�.
	std::list<CN3Shape*>	m_ShapesToRender;	// Tick �� ȣ���ϸ� �������� �͸� �߸���..
	std::list<CN3Shape*>	m_ShapesHaveID;		// ID �� ���� �־� NPC �� �ɼ� �ִ� Shapes....
#endif // end of #ifndef _3DSERVER
	
	float					m_fMapWidth;	// �� �ʺ�.. ���� ����
	float					m_fMapLength;	// �� ����.. ���� ����
	int						m_nCollisionFaceCount;
	__CellMain*				m_pCells[MAX_CELL_MAIN][MAX_CELL_MAIN];

#ifdef _N3TOOL
	std::list<__Vector3>	m_CollisionExtras; // �߰��� ���� �浹üũ ������
#endif // end of #ifedef _N3TOOL

public:
#ifndef _3DSERVER
	CN3Shape* ShapeGetByID(int iID); // ���� ID �� ���� ������Ʈ... NPC �� ���� �ִ� ������Ʈ�� �˻��ؼ� �����ش�..
	CN3Shape* Pick(int iXScreen, int iYScreen, bool bMustHaveEvent, __Vector3* pvPick = NULL); // ��ġ�� �����ش�..
	CN3Shape* PickMovable(int iXScreen, int iYScreen, __Vector3* pvPick);
#endif // end of #ifndef _3DSERVER
	void SubCell(const __Vector3& vPos, __CellSub** ppSubCell);
	__CellSub* SubCell(float fX, float fZ) // �ش� ��ġ�� �� �����͸� �����ش�.
	{
		int x = (int)(fX / CELL_MAIN_SIZE);
		int z = (int)(fZ / CELL_MAIN_SIZE);
		
		__ASSERT(x >= 0 && x < MAX_CELL_MAIN && z >= 0 && z < MAX_CELL_MAIN, "Invalid cell number");
		if(x < 0 || x >= MAX_CELL_MAIN || z < 0 || z >= MAX_CELL_MAIN) return NULL;
		if(NULL == m_pCells[x][z]) return NULL;

		int xx = (((int)fX)%CELL_MAIN_SIZE)/CELL_SUB_SIZE;
		int zz = (((int)fZ)%CELL_MAIN_SIZE)/CELL_SUB_SIZE;
		
		return &(m_pCells[x][z]->SubCells[xx][zz]);
	}
	float		GetHeightNearstPos(const __Vector3& vPos, float fDist, __Vector3* pvNormal = NULL);  // ���� ����� ������ �����ش�. ������ -FLT_MAX �� �����ش�.
	float		GetHeight(float fX, float fZ, __Vector3* pvNormal = NULL);  // ���� �������� ���� ���� ���� �����ش�. ������ -FLT_MAX �� �����ش�.
	int			SubCellPathThru(const __Vector3& vFrom, const __Vector3& vAt, int iMaxSubCell, __CellSub** ppSubCells); // ���� ���̿� ��ģ �������� �����ش�..
	float		Width() { return m_fMapWidth; } // ���� �ʺ�. ������ �����̴�.
	float		Height() { return m_fMapWidth; } // ���� �ʺ�. ������ �����̴�.

#ifndef _3DSERVER
	void		ReleaseShapes();
	void		RenderCollision(__Vector3& vPos); // ���� ��ġ�� �ִ� �浹 �޽ø� �׷��ش�.. ������...
	void		Tick();
	void		Render();
	bool		Load(HANDLE hFile);
	bool		CheckCollisionCamera(__Vector3& vEye, const __Vector3& vAt, float fNP);
	static int SortByCameraDistance(const void* pArg1, const void* pArg2);
#endif // end of #ifndef _3DSERVER

	bool		CheckCollision(	const __Vector3& vPos,			// �浹 ��ġ
								const __Vector3& vDir,			// ���� ����
								float fSpeedPerSec,				// �ʴ� �����̴� �ӵ�
								__Vector3* pvCol = NULL,		// �浹 ����
								__Vector3* pvNormal = NULL,		// �浹�Ѹ��� ��������
								__Vector3* pVec = NULL);		// �浹�� �� �� ������ __Vector3[3]

	bool		Create(float fMapWidth, float fMapLength); // ���� �ʺ�� ���̸� ���� ������ �ִ´�..
	bool		LoadCollisionData(HANDLE hFile);

#ifdef _N3TOOL
	void		MakeMoveTable(short** pMoveArray);	//�������� shape�� �ִ� Ÿ���� 1, ���� Ÿ���� 0���� ������ ���̺��� �����.
	int			Add(CN3Shape* pShape);
	bool		AddCollisionTriangle(const __Vector3& v1, const __Vector3& v2, const __Vector3& v3);
	void		GenerateCollisionData();
	bool		Save(HANDLE hFile);
	bool		SaveCollisionData(HANDLE hFile);
#endif // end of _N3TOOL
	
	void Release();
	CN3ShapeMgr();
	virtual ~CN3ShapeMgr();

};

#endif // !defined(AFX_N3SHAPEMGR_H__36456F66_1D60_4589_A5D9_70B94C2C3127__INCLUDED_)
