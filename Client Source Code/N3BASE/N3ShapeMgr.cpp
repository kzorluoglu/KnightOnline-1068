// N3ShapeMgr.cpp: implementation of the CN3ShapeMgr class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _N3GAME
#include "../WarFare/StdAfx.h"
#include "../WarFare/GameProcedure.h"
#ifndef _REPENT
#include "../WarFare/UILoading.h"
#endif	// _REPENT
#else
#include "StdAfxBase.h"
#endif // end of #ifndef _N3GAME

#include "N3ShapeMgr.h"

#ifndef _3DSERVER
#include "N3ShapeExtra.h"
#endif // end of #ifndef _3DSERVER

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CN3ShapeMgr::CN3ShapeMgr()
{
	m_fMapWidth = 0.0f;
	m_fMapLength = 0.0f;
	m_nCollisionFaceCount = 0;
	m_pvCollisions = NULL;
	memset(m_pCells, 0, sizeof(m_pCells));
}

CN3ShapeMgr::~CN3ShapeMgr()
{
#ifndef _3DSERVER
	int iSC = m_Shapes.size();
	for(int i = 0; i < iSC; i++) delete m_Shapes[i];
	m_Shapes.clear();
#endif // end of #ifndef _3DSERVER

	delete [] m_pvCollisions; m_pvCollisions = NULL;
	for(int z = 0; z < MAX_CELL_MAIN; z++)
	{
		for(int x = 0; x < MAX_CELL_MAIN; x++)
		{
			delete m_pCells[x][z];
		}
	}
}

void CN3ShapeMgr::Release()
{
#ifndef _3DSERVER
	this->ReleaseShapes();
#endif // end of #ifndef _3DSERVER

	m_fMapWidth = 0.0f;
	m_fMapLength = 0.0f;
	m_nCollisionFaceCount = 0;
	delete [] m_pvCollisions; m_pvCollisions = NULL;
	for(int z = 0; z < MAX_CELL_MAIN; z++)
	{
		for(int x = 0; x < MAX_CELL_MAIN; x++)
		{
			delete m_pCells[x][z];
			m_pCells[x][z] = NULL;
		}
	}
	memset(m_pCells, 0, sizeof(MAX_CELL_MAIN));

#ifndef _3DSERVER
	m_ShapesToRender.clear();
#endif // end of #ifndef _3DSERVER

#ifdef _N3TOOL
	m_CollisionExtras.clear(); // �߰��� ���� �浹üũ ������
#endif // end of #ifedef _N3TOOL
}

#ifndef _3DSERVER
void CN3ShapeMgr::ReleaseShapes()
{
	int iSC = m_Shapes.size();
	for(int i = 0; i < iSC; i++) delete m_Shapes[i];
	m_Shapes.clear();
	m_ShapesHaveID.clear();
	m_ShapesToRender.clear();
}
#endif // end of #ifndef _3DSERVER

#ifndef _3DSERVER
bool CN3ShapeMgr::Load(HANDLE hFile)
{
	DWORD dwRWC;
	int nL = 0;

	if(false == LoadCollisionData(hFile)) return false;

#ifdef _N3GAME
#ifndef _REPENT
	CUILoading* pUILoading = CGameProcedure::s_pUILoading; // �ε���..
#endif
#endif // end of #ifndef _N3GAME

	char szBuff[128];
	int iSC = 0;
	if(!m_Shapes.empty()) ReleaseShapes();
	m_ShapesHaveID.clear();

	ReadFile(hFile, &iSC, 4, &dwRWC, NULL); // Shape Count
	if(iSC > 0)
	{
		CN3Shape* pShape = NULL;
		m_Shapes.reserve(iSC);
		DWORD dwType = 0;
		for(int i = 0; i < iSC; i++)
		{
			ReadFile(hFile, &dwType, 4, &dwRWC, NULL); // Shape Type
			if(dwType & OBJ_SHAPE_EXTRA) pShape = new CN3ShapeExtra(); // ������ Ȯ��� Object �� �����..
			else pShape = new CN3Shape();
			m_Shapes.push_back(pShape);

			// pShape->m_iEventID; ���ε� ����Ʈ 100~, 200~ ���� 1100~, 1200~ ���� 2100~, 2200~
			// pShape->m_iEventType; 0-���ε�����Ʈ 1-����(�¿쿭��) 2-����(���Ͽ���) 3-����(���ϴ��) 4-���(����, �Ⱥ���)
			// pShape->m_iNPC_ID; ������ Object ID
			// pShape->m_iNPC_Status; toggle 0, 1
			
			pShape->Load(hFile);
			if(pShape->m_iEventID) //  ID �� �ִ� ������Ʈ ... NPC �� ���� �ִ�..
			{
				m_ShapesHaveID.push_back(pShape);
				pShape->MakeCollisionMeshByParts(); // ���� ��� �״��... �浹 �޽ø� �����...

				TRACE("  Load OBject Event : ID(%d) Type(%d) CtrlID(%d) Status(%d)\n",
					pShape->m_iEventID, pShape->m_iEventType, pShape->m_iNPC_ID, pShape->m_iNPC_Status);

				if(	1 == pShape->m_iEventType || // �¿쿭������,
					2 == pShape->m_iEventType ||  // ���Ͽ�������
					3 == pShape->m_iEventType )  // ���� ����
				{
					pShape->m_bVisible = false;
				}
				else pShape->m_bVisible = true;
			}
#ifdef _N3GAME
			
			// ���� �ڵ�... ���� ���� ����..
//			if(dwType & OBJ_SHAPE_EXTRA)
//			{
//				CN3ShapeExtra* pSE = (CN3ShapeExtra*)pShape; // ������ Ȯ��� Object �� �����..
//				pSE->RotateTo(0, __Vector3(0,1,0), 80, 1, true); // �ٷ� ����.
//				pSE->RotateTo(1, __Vector3(0,1,0), -80, 1, true); // �ٷ� ����.
//			}

			if(!(i%64))
			{
#ifdef _REPENT
				CGameProcedure::RenderLoadingBar(80 + 15 * i / iSC);
#else
				int iLoading = (i+1) * 100 / iSC;
				sprintf(szBuff, "Loading Objects... %d %%", iLoading);
				pUILoading->Render(szBuff, iLoading);
#endif
			}
#endif // end of #ifndef _N3GAME
		}
	}

	return true;
}
#endif // end of #ifndef _3DSERVER

bool CN3ShapeMgr::LoadCollisionData(HANDLE hFile)
{
	DWORD dwRWC;

	ReadFile(hFile, &m_fMapWidth, 4, &dwRWC, NULL); // Shape Count
	ReadFile(hFile, &m_fMapLength, 4, &dwRWC, NULL); // Shape Count
	this->Create(m_fMapWidth, m_fMapLength);

	// �浹 üũ ������ ������ �б�..
	ReadFile(hFile, &m_nCollisionFaceCount, 4, &dwRWC, NULL);
	delete [] m_pvCollisions; m_pvCollisions = NULL;
	if(m_nCollisionFaceCount > 0)
	{
		m_pvCollisions = new __Vector3[m_nCollisionFaceCount * 3];
		ReadFile(hFile, m_pvCollisions, sizeof(__Vector3) * m_nCollisionFaceCount * 3, &dwRWC, NULL);
	}

	// Cell Data ����.
	BOOL bExist = FALSE;
	int z = 0;
	for(float fZ = 0.0f; fZ < m_fMapLength; fZ += CELL_MAIN_SIZE, z++)
	{
		int x = 0;
		for(float fX = 0.0f; fX < m_fMapWidth;  fX += CELL_MAIN_SIZE, x++)
		{
			delete m_pCells[x][z]; m_pCells[x][z] = NULL;

			ReadFile(hFile, &bExist, 4, &dwRWC, NULL); // �����Ͱ� �ִ� ������ ����..
			if(FALSE == bExist) continue;

			m_pCells[x][z] = new __CellMain;
			m_pCells[x][z]->Load(hFile);
		}
	}

	return true;
}

#ifdef _N3TOOL
bool CN3ShapeMgr::Save(HANDLE hFile)
{
	if(false == SaveCollisionData(hFile)) return false;

#ifndef _3DSERVER
	DWORD dwRWC;
	int iSC = m_Shapes.size();
	
	WriteFile(hFile, &iSC, 4, &dwRWC, NULL); // Shape Count
	DWORD dwType = 0;
	for(int i = 0; i < iSC; i++)
	{
		dwType = m_Shapes[i]->Type();
		if(m_Shapes[i]->m_iEventID || m_Shapes[i]->m_iEventType || m_Shapes[i]->m_iNPC_ID || m_Shapes[i]->m_iNPC_Status)
			dwType |= OBJ_SHAPE_EXTRA; // NPC ID �� ������.. Ȯ�� Shape �� ...

		WriteFile(hFile, &dwType, 4, &dwRWC, NULL); // Shape Type
		m_Shapes[i]->CollisionMeshSet(""); // �浹 �޽ô� �����ش�..
		m_Shapes[i]->ClimbMeshSet(""); // �浹 �޽ô� �����ش�..
		m_Shapes[i]->Save(hFile);
	}
#endif // end of #ifndef _3DSERVER
	return true;
}
#endif // end of _N3TOOL

#ifdef _N3TOOL
bool CN3ShapeMgr::SaveCollisionData(HANDLE hFile)
{
	DWORD dwRWC;

	WriteFile(hFile, &m_fMapWidth, 4, &dwRWC, NULL); // �� ���� ���� ���� �ʺ�
	WriteFile(hFile, &m_fMapLength, 4, &dwRWC, NULL); // �� ���� ���� ���� ����

	// �浹 üũ ������ ������ ����..
	WriteFile(hFile, &m_nCollisionFaceCount, 4, &dwRWC, NULL);
	if(m_nCollisionFaceCount > 0)
	{
		WriteFile(hFile, m_pvCollisions, sizeof(__Vector3) * m_nCollisionFaceCount * 3, &dwRWC, NULL);
	}

	// Cell Data ����.
	int z = 0;
	for(float fZ = 0.0f; fZ < m_fMapLength; fZ += CELL_MAIN_SIZE, z++)
	{
		int x = 0;
		for(float fX = 0.0f; fX < m_fMapWidth; fX += CELL_MAIN_SIZE, x++)
		{
			BOOL bExist = FALSE;
			if(m_pCells[x][z]) bExist = TRUE;
			WriteFile(hFile, &bExist, 4, &dwRWC, NULL); // �����Ͱ� �ִ� ������ ����..

			if(NULL == m_pCells[x][z]) continue;

			m_pCells[x][z]->Save(hFile);
		}
	}

	return true;
}
#endif // end of _N3TOOL

bool CN3ShapeMgr::Create(float fMapWidth, float fMapLength) // ���� �ʺ�� ���̸� ���� ������ �ִ´�..
{
	if(	fMapWidth <= 0.0f || fMapWidth > MAX_CELL_MAIN * CELL_MAIN_SIZE ||
		fMapLength <= 0.0f || fMapLength > MAX_CELL_MAIN * CELL_MAIN_SIZE )
	{
		return false;
	}

	m_fMapWidth = fMapWidth;
	m_fMapLength = fMapLength;

	return true;
}

#ifdef _N3TOOL
void CN3ShapeMgr::GenerateCollisionData()
{
	int nFC = 0, iSC = m_Shapes.size();
	for(int i = 0; i < iSC; i++) // Shape �� �ִ� �浹 �޽� ��ŭ ����.
	{
		CN3VMesh* pVM = m_Shapes[i]->CollisionMesh();
		if (NULL == pVM) continue;
		int nIC = pVM->IndexCount();
		if(nIC > 0)
		{
			nFC += nIC/3;
		}
		else 
		{
			int nVC = pVM->VertexCount();
			nFC += nVC/3;
		}
	}

	nFC += m_CollisionExtras.size() / 3; // �߰� �浹 ������..

	if(nFC <= 0) return;
	
	m_nCollisionFaceCount = nFC;
	delete [] m_pvCollisions; m_pvCollisions = NULL;
	m_pvCollisions = new __Vector3[nFC * 3]; // �浹 ������ ����
	memset(m_pvCollisions, 0, sizeof(__Vector3) * nFC * 3);

	int nCPC = 0; // Collision Polygon Count
	for(int i = 0; i < iSC; i++) // Shape �� �ִ� �浹 �޽� ��ŭ ������ ����..
	{
		CN3VMesh* pVMesh = m_Shapes[i]->CollisionMesh();
		if (NULL == pVMesh) continue;
		__Vector3 *pVSrc = pVMesh->Vertices();
		int nIC = pVMesh->IndexCount();
		if(nIC > 0)
		{
			WORD* pwIs = pVMesh->Indices();
			for(int j = 0; j < nIC; j++)
			{
				m_pvCollisions[nCPC++] = pVSrc[pwIs[j]] * m_Shapes[i]->m_Matrix; // ���� ��ġ�̴�.
			}
		}
		else 
		{
			int nVC = pVMesh->VertexCount();
			for(int j = 0; j < nVC; j++)
			{
				m_pvCollisions[nCPC++] = pVSrc[j] * m_Shapes[i]->m_Matrix; // ���� ��ġ�̴�.
			}
		}
	}

	// �߰� �浹 ������ �ֱ�..
	it_Vector3 it = m_CollisionExtras.begin(), itEnd = m_CollisionExtras.end();
	for(; it != itEnd; it++)
	{
		m_pvCollisions[nCPC++] = *it;
	}

	if(nCPC != (nFC * 3))
	{
		__ASSERT(0, "�浹 üũ �������� �������� �� ������ �ٸ��ϴ�.");
		this->Release();
		return;
	}

	// �� ������ ���� ���� �ִ��� Ȥ�� ���ԵǾ� �ִ��� ��� �Ǵ��ؼ� �ε��� ������ �Ѵ�.
	int xSMax = (int)(m_fMapWidth / CELL_SUB_SIZE);
	int zSMax = (int)(m_fMapLength / CELL_SUB_SIZE);
	for(int i = 0; i < nFC; i++)
	{
		__Vector3 vEdge[3][2];

		vEdge[0][0] = m_pvCollisions[i*3];
		vEdge[0][1] = m_pvCollisions[i*3+1];
		vEdge[1][0] = m_pvCollisions[i*3+1];
		vEdge[1][1] = m_pvCollisions[i*3+2];
		vEdge[2][0] = m_pvCollisions[i*3+2];
		vEdge[2][1] = m_pvCollisions[i*3];

		for(int j = 0; j < 3; j++) // ���� �ִ� �޽� ��ŭ ����...
		{
			// ������ ���ϰ�..
			int xx1 = 0, xx2 = 0, zz1 = 0, zz2 = 0;

			if(vEdge[j][0].x < vEdge[j][1].x) 
			{
				xx1 = (int)(vEdge[j][0].x / CELL_SUB_SIZE) - 1;
				xx2 = (int)(vEdge[j][1].x / CELL_SUB_SIZE) + 1;
			}
			else
			{
				xx1 = (int)(vEdge[j][1].x / CELL_SUB_SIZE) - 1;
				xx2 = (int)(vEdge[j][0].x / CELL_SUB_SIZE) + 1;
			}

			if(xx1 < 0) xx1 = 0;
			if(xx1 >= xSMax) xx1 = xSMax - 1;
			if(xx2 < 0) xx2 = 0;
			if(xx2 >= xSMax) xx2 = xSMax - 1;

			if(vEdge[j][0].z < vEdge[j][1].z)
			{
				zz1 = (int)(vEdge[j][0].z / CELL_SUB_SIZE) - 1;
				zz2 = (int)(vEdge[j][1].z / CELL_SUB_SIZE) + 1;
			}
			else
			{
				zz1 = (int)(vEdge[j][1].z / CELL_SUB_SIZE) - 1;
				zz2 = (int)(vEdge[j][0].z / CELL_SUB_SIZE) + 1;
			}

			if(zz1 < 0) zz1 = 0;
			if(zz1 >= zSMax) zz1 = zSMax - 1;
			if(zz2 < 0) zz2 = 0;
			if(zz2 >= zSMax) zz2 = zSMax - 1;

			for(int z = zz1; z <= zz2; z++) // ������ŭ ó��..
			{
				float fZMin = (float)(z * CELL_SUB_SIZE);
				float fZMax = (float)((z+1) * CELL_SUB_SIZE);
				for(int x = xx1; x <= xx2; x++)
				{
					float fXMin = (float)(x * CELL_SUB_SIZE);
					float fXMax = (float)((x+1) * CELL_SUB_SIZE);

					// Cohen thuderland algorythm
					DWORD dwOC0 = 0, dwOC1 = 0; // OutCode 0, 1
					if(vEdge[j][0].z > fZMax) dwOC0 |= 0xf000;
					if(vEdge[j][0].z < fZMin) dwOC0 |= 0x0f00;
					if(vEdge[j][0].x > fXMax) dwOC0 |= 0x00f0;
					if(vEdge[j][0].x < fXMin) dwOC0 |= 0x000f;
					if(vEdge[j][1].z > fZMax) dwOC1 |= 0xf000;
					if(vEdge[j][1].z < fZMin) dwOC1 |= 0x0f00;
					if(vEdge[j][1].x > fXMax) dwOC1 |= 0x00f0;
					if(vEdge[j][1].x < fXMin) dwOC1 |= 0x000f;
					
					bool bWriteID = false;
					if(dwOC0 & dwOC1) bWriteID = false; // �� ������ ���� ���� �ܺο� �ִ�.
					else if(dwOC0 == 0 && dwOC1 == 0) bWriteID = true;// ������ �簢�� ���ο� ����
					else if((dwOC0 == 0 && dwOC1 != 0) || (dwOC0 != 0 && dwOC1 == 0)) bWriteID = true;// ���� ������ ���� ���ο� ������ �ܺο� ����.
					else if((dwOC0 & dwOC1) == 0) // �� �L�� ��� �� �ܺο� ������ �Ǵ��� �ٽ� �ؾ� �Ѵ�.
					{
						float fXCross = vEdge[j][0].x + (fZMax - vEdge[j][0].z) * (vEdge[j][1].x - vEdge[j][0].x) / (vEdge[j][1].z - vEdge[j][0].z); // ���� ������ �������� ����ϰ�..
						if(fXCross < fXMin) bWriteID = false; // ������ �ܰ��� �ִ�.
						else bWriteID = true; // ��ó�ִ�.
					}

					if(false == bWriteID) // ���� �ɸ��� ���ٸ�... ������ ������ ��Ƽ� �浹���� üũ�Ѵ�.
					{
						__Vector3 vPos, vDir;
						vDir.Set(0, -1.0f, 0);
						float zz3 = z * CELL_SUB_SIZE, zz4 = (z+1) * CELL_SUB_SIZE;
						float xx3 = x * CELL_SUB_SIZE, xx4 = (x+1) * CELL_SUB_SIZE;
						for(float fZ = zz3; fZ <= zz4 && false == bWriteID; fZ += 0.25f)
						{
							for(float fX = xx3; fX <= xx4 && false == bWriteID; fX += 0.25f)
							{
								vPos.Set(fX, 10000.0f, fZ);
								if(::_IntersectTriangle(vPos, vDir, m_pvCollisions[i*3], m_pvCollisions[i*3+1], m_pvCollisions[i*3+2])) // ������ �浹 üũ..
								{
									bWriteID = true; // �浹 ������ �ε����� ���� �����..
								}
							}
						}
					}
					
					if(false == bWriteID) continue; // �浹 ������ ���� ����..
					
					// �浹 ������ ��� �Ѵ�..
					int nX = x / CELL_MAIN_DEVIDE;
					int nZ = z / CELL_MAIN_DEVIDE;
					if(nX < 0 || nX >= MAX_CELL_MAIN || nZ < 0 && nZ >= MAX_CELL_MAIN) continue;

					if(NULL == m_pCells[nX][nZ])
					{
						m_pCells[nX][nZ] = new __CellMain;
					}

					int nXSub = x%CELL_MAIN_DEVIDE;
					int nZSub = z%CELL_MAIN_DEVIDE;

					__CellSub* pSubCell = &(m_pCells[nX][nZ]->SubCells[nXSub][nZSub]);
					int nCCPC = pSubCell->nCCPolyCount; // Collision Check Polygon Count

					bool bOverlapped = false;
					for(int k = 0; k < nCCPC; k++) // �ߺ� �Ǵ��� üũ
					{
						if(i * 3 == pSubCell->pdwCCVertIndices[k*3]) 
						{
							bOverlapped = true;
							break;
						}
					}

					if(true == bOverlapped) continue; // ��ģ�� �ִ��� üũ
					// �ߺ��Ȱ� ������..
					if(0 == nCCPC) // ÷ ���� �Ǿ�����..
					{
						pSubCell->pdwCCVertIndices = new DWORD[768];
						memset(pSubCell->pdwCCVertIndices, 0, 768 * 4);
					}
//					else // �̹� ������..
//					{
//						DWORD* pwBack = pSubCell->pdwCCVertIndices;
//						pSubCell->pdwCCVertIndices = new DWORD[(nCCPC+1)*3];
//						memcpy(pSubCell->pdwCCVertIndices, pwBack, nCCPC * 3 * 4); // �������� �ϳ��� �������̰� ���� �ε����̹Ƿ�..
//						delete [] pwBack;
//					}
					
					if(nCCPC >= 256)
					{
						__ASSERT(0, "�浹 üũ ������ ���� �ʹ� �����ϴ�");
						continue;
					}

					pSubCell->pdwCCVertIndices[nCCPC*3+0] = i * 3 + 0; // �ε��� ����..
					pSubCell->pdwCCVertIndices[nCCPC*3+1] = i * 3 + 1; // �ε��� ����..
					pSubCell->pdwCCVertIndices[nCCPC*3+2] = i * 3 + 2; // �ε��� ����..
					pSubCell->nCCPolyCount++; // Collision Check Polygon Count �� �ø���.
				} // end of for(int x = xx1; x <= xx2; x++)
			} // end of for(int z = zz1; z <= zz2; z++) // ������ŭ ó��..
		} // end of for(int j = 0; j < 3; j++) // ���� �ִ� �޽� ��ŭ ����...
	}
}
#endif // end of _N3TOOL

#ifdef _N3TOOL
int CN3ShapeMgr::Add(CN3Shape *pShape)
{
	if(NULL == pShape) return -1;
	__Vector3 vPos = pShape->Pos();
	int nX = (int)(vPos.x / CELL_MAIN_SIZE);
	int nZ = (int)(vPos.z / CELL_MAIN_SIZE);
	if(nX < 0 || nX >= MAX_CELL_MAIN || nZ < 0 || nZ >= MAX_CELL_MAIN) 
	{
		__ASSERT(0, "CN3ShapeMgr::Add - Shape Add Failed. Check position");
		return -1;
	}

	pShape->SaveToFile(); // ���Ϸ� �����ϰ�..
	CN3Shape* pShapeAdd = new CN3Shape();
	if(false == pShapeAdd->LoadFromFile(pShape->FileName())) // �� ������ ���� ����
	{
		delete pShapeAdd;
		return -1;
	}

	if(NULL == m_pCells[nX][nZ])
	{
		m_pCells[nX][nZ] = new __CellMain;
	}

	int iSC = m_pCells[nX][nZ]->nShapeCount;
	if(0 == iSC) // ÷ ���� �Ǿ�����..
	{
		m_pCells[nX][nZ]->pwShapeIndices = new WORD[iSC+1];
	}
	else // �̹� ������..
	{
		WORD* pwBack = m_pCells[nX][nZ]->pwShapeIndices;
		m_pCells[nX][nZ]->pwShapeIndices = new WORD[iSC+1];
		memcpy(m_pCells[nX][nZ]->pwShapeIndices, pwBack, iSC * 2);
		delete [] pwBack;
	}
	
	m_pCells[nX][nZ]->pwShapeIndices[iSC] = m_Shapes.size(); // �ε��� ����..

	m_Shapes.push_back(pShapeAdd); // �߰� �Ѵ�..
	m_pCells[nX][nZ]->nShapeCount++; // Shape Count �� �ø���.

	return m_Shapes.size() - 1;
}
#endif // end of _N3TOOL

#ifdef _N3TOOL
bool CN3ShapeMgr::AddCollisionTriangle(const __Vector3& v1, const __Vector3& v2, const __Vector3& v3)
{
	int count = m_CollisionExtras.size();
	m_CollisionExtras.push_back(v1); // �߰��� ���� �浹üũ ������
	m_CollisionExtras.push_back(v2); // �߰��� ���� �浹üũ ������
	m_CollisionExtras.push_back(v3); // �߰��� ���� �浹üũ ������

	if((count+3)==m_CollisionExtras.size()) return true;

	return false;
}
#endif // end of _N3TOOL

#ifdef _N3TOOL
void CN3ShapeMgr::MakeMoveTable(short** pMoveArray)
{
	int ArraySize = (MAX_CELL_MAIN * CELL_MAIN_DEVIDE) + 1;

	for(int bx=0; bx<MAX_CELL_MAIN; bx++)
	{
		for(int bz=0; bz<MAX_CELL_MAIN; bz++)
		{
			if(m_pCells[bx][bz])
			{
				for(int sx=0; sx<CELL_MAIN_DEVIDE; sx++)
				{
					for(int sz=0; sz<CELL_MAIN_DEVIDE; sz++)
					{
						if(m_pCells[bx][bz]->SubCells[sx][sz].nCCPolyCount>0)
						{
							int ix, iz;
							ix = (bx * CELL_MAIN_DEVIDE) + sx;
							iz = (bz * CELL_MAIN_DEVIDE) + sz;
							pMoveArray[ix][iz] = 0;
						}
					} //for(int sz=0; sz<CELL_MAIN_DEVIDE; sz++)
				} //for(int sx=0; sx<CELL_MAIN_DEVIDE; sx++)
			} //if(m_pCells[bx][bz] && m_pCells[bx][bz]->nShapeCount>0)
		} //for(int bz=0; bz<MAX_CELL_MAIN; bz++)
	} //for(int bx=0; bx<MAX_CELL_MAIN; bx++)
}
#endif // end of _N3TOOL

#ifndef _3DSERVER
void CN3ShapeMgr::Tick()
{
	int xMainS = (int)((s_CameraData.vEye.x - s_CameraData.fFP) / CELL_MAIN_SIZE); if(xMainS < 0) xMainS = 0;
	int xMainE = (int)((s_CameraData.vEye.x + s_CameraData.fFP) / CELL_MAIN_SIZE); if(xMainE > MAX_CELL_MAIN) xMainE = MAX_CELL_MAIN;
	int zMainS = (int)((s_CameraData.vEye.z - s_CameraData.fFP) / CELL_MAIN_SIZE); if(zMainS < 0) zMainS = 0;
	int zMainE = (int)((s_CameraData.vEye.z + s_CameraData.fFP) / CELL_MAIN_SIZE); if(zMainE > MAX_CELL_MAIN) zMainE = MAX_CELL_MAIN;
	
	__CellMain* pCellCur = NULL;
	int i = 0, iShp = 0, iSIndex = 0, iSC = m_Shapes.size();;
	m_ShapesToRender.clear();
	// ������ ����Ʈ ����..
	
	for(int zM = zMainS; zM < zMainE; zM++)
	{
		for(int xM = xMainS; xM < xMainE; xM++)
		{
			pCellCur = m_pCells[xM][zM];
			if(NULL == pCellCur) continue;

			int iSCC = pCellCur->nShapeCount;
			for(int i = 0; i < iSCC; i++)
			{
				iSIndex = pCellCur->pwShapeIndices[i];
				__ASSERT(iSIndex >= 0 && iSIndex < iSC, "Shape Index is invalid");

				m_Shapes[iSIndex]->Tick();
				if(m_Shapes[iSIndex]->m_bDontRender) continue;

				m_ShapesToRender.push_back(m_Shapes[iSIndex]);
			}
		}
	}
}
#endif // end of #ifndef _3DSERVER

#ifndef _3DSERVER
void CN3ShapeMgr::Render()
{
	CN3Shape* pShpCur = NULL;
	it_Shp it = m_ShapesToRender.begin(), itEnd = m_ShapesToRender.end();
	for(; it != itEnd; it++)
	{
		pShpCur = *it;
		__ASSERT(pShpCur, "Shape pointer is null!!!");

		pShpCur->Render();
#if _DEBUG
		pShpCur->RenderCollisionMesh();
#endif
	}
}
#endif // end of #ifndef _3DSERVER

bool CN3ShapeMgr::CheckCollision(	const __Vector3& vPos,		// �浹 ��ġ
									const __Vector3& vDir,		// ���� ����
									float fSpeedPerSec,			// �ʴ� �����̴� �ӵ�
									__Vector3* pvCol,			// �浹 ����
									__Vector3* pvNormal,		// �浹�Ѹ��� ��������
									__Vector3* pVec)			// �浹�� �� �� ������ __Vector3[3]
{
	if(fSpeedPerSec <= 0) return false; // �����̴� �ӵ��� ���ų� �ݴ�� �����̸� �Ѿ��..
	static __CellSub* ppCells[128];
	__Vector3 vPosNext = vPos + (vDir * fSpeedPerSec); // ���� ��ġ
	
	int iSubCellCount = 0;
	if(fSpeedPerSec < 4.0f) 
	{
		__Vector3 vPos2 = vPos + (vDir * 4.0f);
		iSubCellCount = this->SubCellPathThru(vPos, vPos2, 128, ppCells); // ����ϴ� ���꼿�� �����´�..
	}
	else
	{
		iSubCellCount = this->SubCellPathThru(vPos, vPosNext, 128, ppCells); // ����ϴ� ���꼿�� �����´�..
	}
	if(iSubCellCount <= 0) return false; // ���� ����.

	__Vector3 vColTmp(0,0,0);
	int nIndex0, nIndex1, nIndex2;
	static float fT, fU, fV;
	float fDistTmp = FLT_MAX, fDistClosest = FLT_MAX;

	for ( int i = 0; i < iSubCellCount; i++ )
	{
		if ( ppCells[i]->nCCPolyCount <= 0 ) continue;

		for ( int j = 0; j < ppCells[i]->nCCPolyCount; j++ )
		{
			nIndex0 = ppCells[i]->pdwCCVertIndices[j*3];
			nIndex1 = ppCells[i]->pdwCCVertIndices[j*3+1];
			nIndex2 = ppCells[i]->pdwCCVertIndices[j*3+2];
			
			if(false == ::_IntersectTriangle(vPos, vDir, m_pvCollisions[nIndex0], m_pvCollisions[nIndex1], m_pvCollisions[nIndex2], fT, fU, fV, &vColTmp)) continue;
			if(false == ::_IntersectTriangle(vPosNext, vDir, m_pvCollisions[nIndex0], m_pvCollisions[nIndex1], m_pvCollisions[nIndex2]))
			{
				fDistTmp = (vPos - vColTmp).Magnitude(); // �Ÿ��� �纸��..
				if(fDistTmp < fDistClosest) 
				{
					fDistClosest = fDistTmp;
					// �浹�̴�..
					if(pvCol) *pvCol = vColTmp;
					if(pvNormal)
					{
						(*pvNormal).Cross(m_pvCollisions[nIndex1] - m_pvCollisions[nIndex0], 
							m_pvCollisions[nIndex2] - m_pvCollisions[nIndex0]);
						(*pvNormal).Normalize();
					}
					if ( pVec )
					{
						pVec[0] = m_pvCollisions[nIndex0];
						pVec[1] = m_pvCollisions[nIndex1];
						pVec[2] = m_pvCollisions[nIndex2];
					}
				}
			}
		}
	}
	
	if(fDistClosest != FLT_MAX)
		return true;

#ifndef _3DSERVER
	else
	{
		it_Shp it = m_ShapesToRender.begin(), itEnd = m_ShapesToRender.end(); // ���� ���̴°͸� ������� �ؼ�...
		int iSC = m_ShapesToRender.size();	

		if(iSC > 0)
		{
			// �Ÿ������� ����..
			CN3Shape* pShape = NULL;
			std::vector<CN3Shape*> Shapes;
			for(int i = 0; it != itEnd; it++, i++)
			{
				pShape = *it;
				if(NULL == pShape->CollisionMesh()) continue;
				if((pShape->Pos() - vPos).Magnitude() > pShape->Radius() * 2) continue; // �ָ� �������Ÿ� ��������..

				Shapes.push_back(pShape);
			}

			if(Shapes.empty()) return false;

			iSC = Shapes.size();
			if(iSC > 1) qsort(&(Shapes[0]), iSC, 4, SortByCameraDistance); // ī�޶� �Ÿ��� ���� �����ϰ�..

			CN3VMesh* pVMesh = NULL;
			for(int i = 0; i < iSC; i++)
			{
				pVMesh = Shapes[i]->CollisionMesh();
				if(true == pVMesh->CheckCollision(Shapes[i]->m_Matrix, vPos, vPosNext, pvCol, pvNormal)) return true;
			}

			return false;
		}
		else
		{
			return false;
		}
	}
#else
	return false;
#endif // end of #ifndef _3DSERVER
}

#ifndef _3DSERVER
bool CN3ShapeMgr::CheckCollisionCamera(__Vector3& vEyeResult, const __Vector3& vAt, float fNP)
{
	__Vector3 vDir = vEyeResult - vAt;
	float fD = vDir.Magnitude();
	vDir.Normalize();
	__Vector3 vCol(0,0,0);
	if(false == this->CheckCollision(vAt, vDir, fD, &vCol)) return false;

	float fDelta = (vEyeResult - vCol).Magnitude(); // �浹���� ó�ٺ��� �Ÿ��� �纸��..
	if(fDelta < fNP * 2.0f) return false; // �ʹ� ������ ������ ���ư���..
	
	vEyeResult -= vDir * fDelta;
	return true;
}
#endif // end of #ifndef _3DSERVER

#ifndef _3DSERVER
void CN3ShapeMgr::RenderCollision(__Vector3 &vPos)
{
	int x = (int)(vPos.x / CELL_MAIN_SIZE);
	int z = (int)(vPos.z / CELL_MAIN_SIZE);

	__CellSub* ppCell[9]; memset(ppCell, 0, sizeof(ppCell));
	this->SubCell(vPos, ppCell);

	__Matrix44 mtxWorld;
	mtxWorld.Identity();
	s_lpD3DDev->SetTransform(D3DTS_WORLD, &mtxWorld);
	
	DWORD dwFillPrev;
	s_lpD3DDev->GetRenderState(D3DRS_FILLMODE, &dwFillPrev);
	s_lpD3DDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	DWORD dwLight;
	s_lpD3DDev->GetRenderState(D3DRS_LIGHTING, &dwLight);
	s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);
//	DWORD dwZ;
//	s_lpD3DDev->GetRenderState(D3DRS_ZENABLE, &dwZ);
//	s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	s_lpD3DDev->SetTexture(0, NULL);

	for(int i = 0; i < 9; i++ )
	{
		if(NULL == ppCell[i] || ppCell[i]->nCCPolyCount <= 0) continue;
		int nFC = ppCell[i]->nCCPolyCount;
		int n0, n1, n2;
		__VertexColor vCols[4];
		__VertexColor vNormalDir[2];
		__Vector3 vDir;
		for(int j = 0; j < nFC; j++)
		{
			n0 = ppCell[i]->pdwCCVertIndices[j*3+0];
			n1 = ppCell[i]->pdwCCVertIndices[j*3+1];
			n2 = ppCell[i]->pdwCCVertIndices[j*3+2];
			
			vCols[0].Set(m_pvCollisions[n0], 0xffff0000);
			vCols[1].Set(m_pvCollisions[n1], 0xff00ff00);
			vCols[2].Set(m_pvCollisions[n2], 0xff0000ff);
//			vCols[3] = vCols[0]; vCols[3].color = 0xffffffff;

			vDir.Cross((m_pvCollisions[n1] - m_pvCollisions[n0]), (m_pvCollisions[n2] - m_pvCollisions[n1])); vDir.Normalize();
			vNormalDir[0] = (m_pvCollisions[n0] + m_pvCollisions[n1] + m_pvCollisions[n2])/3.0f;
			vNormalDir[1] = vNormalDir[0] + vDir;
			vNormalDir[0].color = 0xffff0000;
			vNormalDir[1].color = 0xffffffff;
			
			s_lpD3DDev->SetFVF(FVF_CV);
			s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 1, vCols, sizeof(__VertexColor));
			s_lpD3DDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, vNormalDir, sizeof(__VertexColor));
		}
	}

	s_lpD3DDev->SetRenderState(D3DRS_FILLMODE, dwFillPrev);
	s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, dwLight);
//	s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, dwZ);
}
#endif // end of #ifndef _3DSERVER

#ifndef _3DSERVER
CN3Shape* CN3ShapeMgr::Pick(int iXScreen, int iYScreen, bool bMustHaveEvent, __Vector3* pvPick)
{
	if(m_ShapesToRender.empty()) return NULL;

	__Vector3 vPos, vDir;
	::_Convert2D_To_3DCoordinate(iXScreen, iYScreen, s_CameraData.mtxView, s_CameraData.mtxProjection, s_CameraData.vp, vPos, vDir);

	it_Shp it = m_ShapesToRender.begin(), itEnd = m_ShapesToRender.end(); // ���� ���̴°͸� ������� �ؼ�...
	int iSC = m_ShapesToRender.size();

	// �Ÿ������� ����..
	std::vector<CN3Shape*> Shapes(iSC, NULL);
	for(int i = 0; it != itEnd; it++, i++) { Shapes[i] = *it; }
	qsort(&(Shapes[0]), iSC, 4, SortByCameraDistance);

	for(int i = 0; i < iSC; i++)
	{
		if(bMustHaveEvent && Shapes[i]->m_iEventID <= 0) continue; // �̺�Ʈ�� �־�� �Ѵٸ�...
		if(Shapes[i]->CheckCollisionPrecisely(false, vPos, vDir, pvPick) >= 0)
			return Shapes[i];
	}

	return NULL;
}
#endif // end of #ifndef _3DSERVER

#ifndef _3DSERVER
CN3Shape* CN3ShapeMgr::PickMovable(int iXScreen, int iYScreen, __Vector3* pvPick)
{
	if(m_ShapesToRender.empty()) return NULL;

	__Vector3 vPos, vDir;
	::_Convert2D_To_3DCoordinate(iXScreen, iYScreen, s_CameraData.mtxView, s_CameraData.mtxProjection, s_CameraData.vp, vPos, vDir);

	it_Shp it = m_ShapesToRender.begin(), itEnd = m_ShapesToRender.end(); // ���� ���̴°͸� ������� �ؼ�...
	int iSC = m_ShapesToRender.size();

	// �Ÿ������� ����..
	std::vector<CN3Shape*> Shapes(iSC, NULL);
	for(int i = 0; it != itEnd; it++, i++) { Shapes[i] = *it; }
	qsort(&(Shapes[0]), iSC, 4, SortByCameraDistance);

	for(int i = 0; i < iSC; i++)
	{
		if(Shapes[i]->CheckCollisionPrecisely(false, vPos, vDir, pvPick) >= 0)
			return Shapes[i];
	}

	return NULL;
}
#endif // end of #ifndef _3DSERVER

#ifndef _3DSERVER
CN3Shape* CN3ShapeMgr::ShapeGetByID(int iID)
{
	CN3Shape* pShape = NULL;
	it_Shp it = m_ShapesHaveID.begin(), itEnd = m_ShapesHaveID.end();
	for(; it != itEnd; it++)
	{
		pShape = *it;
		if(iID == pShape->m_iEventID) return pShape;
	}

	return NULL;
}
#endif // end of #ifndef _3DSERVER

#ifndef _3DSERVER
int CN3ShapeMgr::SortByCameraDistance(const void* pArg1, const void* pArg2)
{
	CN3Shape* pShape1 = *((CN3Shape**)pArg1);
	CN3Shape* pShape2 = *((CN3Shape**)pArg2);

	float fDist1 = (CN3Base::s_CameraData.vEye - pShape1->Pos()).Magnitude();
	float fDist2 = (CN3Base::s_CameraData.vEye - pShape2->Pos()).Magnitude();

	if(fDist1 < fDist2) return -1; // ������ true;
	else if(fDist1 > fDist2) return 1;
	else return 0;
}
#endif // end of #ifndef _3DSERVER

int CN3ShapeMgr::SubCellPathThru(const __Vector3& vFrom, const __Vector3& vAt, int iMaxSubCell, __CellSub** ppSubCells) // ���� ���̿� ��ģ �������� �����ش�..
{
	if(NULL == ppSubCells) return 0;

	// ������ ���ϰ�..
	int xx1 = 0, xx2 = 0, zz1 = 0, zz2 = 0;

	if(vFrom.x < vAt.x) { xx1 = (int)(vFrom.x / CELL_SUB_SIZE); xx2 = (int)(vAt.x / CELL_SUB_SIZE); }
	else { xx1 = (int)(vAt.x / CELL_SUB_SIZE); 	xx2 = (int)(vFrom.x / CELL_SUB_SIZE); }

	if(vFrom.z < vAt.z)	{ zz1 = (int)(vFrom.z / CELL_SUB_SIZE); zz2 = (int)(vAt.z / CELL_SUB_SIZE); }
	else { zz1 = (int)(vAt.z / CELL_SUB_SIZE); zz2 = (int)(vFrom.z / CELL_SUB_SIZE); }

	bool bPathThru;
	float fZMin, fZMax, fXMin, fXMax;
	int iSubCellCount = 0;
	for(int z = zz1; z <= zz2; z++) // ������ŭ ó��..
	{
		fZMin = (float)(z * CELL_SUB_SIZE);
		fZMax = (float)((z+1) * CELL_SUB_SIZE);
		for(int x = xx1; x <= xx2; x++)
		{
			fXMin = (float)(x * CELL_SUB_SIZE);
			fXMax = (float)((x+1) * CELL_SUB_SIZE);

			// Cohen thuderland algorythm
			DWORD dwOC0 = 0, dwOC1 = 0; // OutCode 0, 1
			if(vFrom.z > fZMax) dwOC0 |= 0xf000;
			if(vFrom.z < fZMin) dwOC0 |= 0x0f00;
			if(vFrom.x > fXMax) dwOC0 |= 0x00f0;
			if(vFrom.x < fXMin) dwOC0 |= 0x000f;
			if(vAt.z > fZMax) dwOC1 |= 0xf000;
			if(vAt.z < fZMin) dwOC1 |= 0x0f00;
			if(vAt.x > fXMax) dwOC1 |= 0x00f0;
			if(vAt.x < fXMin) dwOC1 |= 0x000f;
			
			bPathThru = false;
			if(dwOC0 & dwOC1) bPathThru = false; // �� ������ ���� ���� �ܺο� �ִ�.
			else if(dwOC0 == 0 && dwOC1 == 0) bPathThru = true;// ������ �簢�� ���ο� ����
			else if((dwOC0 == 0 && dwOC1 != 0) || (dwOC0 != 0 && dwOC1 == 0)) bPathThru = true;// ���� ������ ���� ���ο� ������ �ܺο� ����.
			else if((dwOC0 & dwOC1) == 0) // �� �L�� ��� �� �ܺο� ������ �Ǵ��� �ٽ� �ؾ� �Ѵ�.
			{
				float fXCross = vFrom.x + (fZMax - vFrom.z) * (vAt.x - vFrom.x) / (vAt.z - vFrom.z); // ���� ������ �������� ����ϰ�..
				if(fXCross < fXMin) bPathThru = false; // ������ �ܰ��� �ִ�.
				else bPathThru = true; // ��ó�ִ�.
			}

			if(false == bPathThru) continue;

			// �浹 ������ ��� �Ѵ�..
			int nX = x / CELL_MAIN_DEVIDE;
			int nZ = z / CELL_MAIN_DEVIDE;
			if(nX < 0 || nX >= MAX_CELL_MAIN || nZ < 0 && nZ >= MAX_CELL_MAIN) continue; // ���μ��ٱ��� ���� ��������.
			if(NULL == m_pCells[nX][nZ]) continue; // ���μ��� ���̸� ��������..

			int nXSub = x%CELL_MAIN_DEVIDE;
			int nZSub = z%CELL_MAIN_DEVIDE;

			ppSubCells[iSubCellCount] = &(m_pCells[nX][nZ]->SubCells[nXSub][nZSub]);
			iSubCellCount++;

			if(iSubCellCount >= iMaxSubCell) return iMaxSubCell;
		} // end of for(int x = xx1; x <= xx2; x++)
	} // end of for(int z = zz1; z <= zz2; z++) // ������ŭ ó��..

	return iSubCellCount; // ��ģ �� ������ �����ֱ�..
}

float CN3ShapeMgr::GetHeightNearstPos(const __Vector3 &vPos, float fDist, __Vector3* pvNormal) // ���� ����� ���̰��� �����ش�. ������ -FLT_MAX �� �����ش�.
{
	__CellSub* pCell = this->SubCell(vPos.x, vPos.z); // ���꼿�� �����´�..
	if(NULL == pCell || pCell->nCCPolyCount <= 0) return -FLT_MAX; // ���� ����.

	__Vector3 vPosV = vPos; vPosV.y = 5000.0f; // ����⿡ ��ġ�� �ϰ�..
	__Vector3 vDir(0,-1, 0); // ���� ���� ����
	__Vector3 vColTmp(0,0,0); // ���������� ���� ����� �浹 ��ġ..

	int nIndex0, nIndex1, nIndex2;
	float fT, fU, fV;
	float fNearst = FLT_MAX, fMinTmp = 0, fHeight = -FLT_MAX;		// �ϴ� �ּҰ��� ū������ ���..

	for ( int i = 0; i < pCell->nCCPolyCount; i++ )
	{
		nIndex0 = pCell->pdwCCVertIndices[i*3];
		nIndex1 = pCell->pdwCCVertIndices[i*3+1];
		nIndex2 = pCell->pdwCCVertIndices[i*3+2];
		
		// �浹�� ���� ������..
		if(true == ::_IntersectTriangle(vPosV, vDir, m_pvCollisions[nIndex0], m_pvCollisions[nIndex1], m_pvCollisions[nIndex2], fT, fU, fV, &vColTmp))
		{
			fMinTmp = (vColTmp - vPos).Magnitude();
			if(fMinTmp < fNearst) // ���� ����� �浹 ��ġ�� ã�� ���� �ڵ�..
			{
				fNearst = fMinTmp;
				fHeight = vColTmp.y; // ���̰�.

				if(pvNormal)
				{
					pvNormal->Cross(m_pvCollisions[nIndex1] - m_pvCollisions[nIndex0], m_pvCollisions[nIndex2] - m_pvCollisions[nIndex0]);
					pvNormal->Normalize();
				}
			}
		}
	}

	return fHeight;
}

float CN3ShapeMgr::GetHeight(float fX, float fZ, __Vector3* pvNormal) // ���� ���� ���� �����ش�.. ������ -FLT_MAX���� �����ش�.
{
	__CellSub* pCell = this->SubCell(fX, fZ); // ���꼿�� �����´�..
	if(NULL == pCell || pCell->nCCPolyCount <= 0) return -FLT_MAX; // ���� ����.

	__Vector3 vPosV(fX, 5000.0f, fZ); // ����⿡ ��ġ�� �ϰ�..
	__Vector3 vDir(0,-1, 0); // ���� ���� ����
	__Vector3 vColTmp(0,0,0); // ���������� ���� ����� �浹 ��ġ..

	int nIndex0, nIndex1, nIndex2;
	float fT, fU, fV;
	float fMaxTmp = -FLT_MAX;;

	for ( int i = 0; i < pCell->nCCPolyCount; i++ )
	{
		nIndex0 = pCell->pdwCCVertIndices[i*3];
		nIndex1 = pCell->pdwCCVertIndices[i*3+1];
		nIndex2 = pCell->pdwCCVertIndices[i*3+2];
		
		// �浹�� ���� ������..
		if(true == ::_IntersectTriangle(vPosV, vDir, m_pvCollisions[nIndex0], m_pvCollisions[nIndex1], m_pvCollisions[nIndex2], fT, fU, fV, &vColTmp))
		{
			if(vColTmp.y > fMaxTmp)
			{
				fMaxTmp = vColTmp.y;
				if(pvNormal)
				{
					pvNormal->Cross(m_pvCollisions[nIndex1] - m_pvCollisions[nIndex0], m_pvCollisions[nIndex2] - m_pvCollisions[nIndex0]);
					pvNormal->Normalize();
				}
			}
		}
	}

	return fMaxTmp;
}

void CN3ShapeMgr::SubCell(const __Vector3& vPos, __CellSub** ppSubCell)			// �ش� ��ġ�� �� �����͸� �����ش�.
{
	int x = (int)(vPos.x / CELL_MAIN_SIZE);
	int z = (int)(vPos.z / CELL_MAIN_SIZE);
	
	__ASSERT(x >= 0 && x < MAX_CELL_MAIN && z >= 0 && z < MAX_CELL_MAIN, "Invalid cell number");
	if(x < 0 || x >= MAX_CELL_MAIN || z < 0 || z >= MAX_CELL_MAIN) return;

	int xx = (((int)vPos.x)%CELL_MAIN_SIZE)/CELL_SUB_SIZE;			// 2, 3, 4
	int zz = (((int)vPos.z)%CELL_MAIN_SIZE)/CELL_SUB_SIZE;			// 1, 0, 5
																	// 8, 7, 6	
	for ( int i = 0; i < 9; i++ )
	{
		switch( i )
		{
			case 0:
				if ( m_pCells[x][z] != NULL )
					ppSubCell[i] = &(m_pCells[x][z]->SubCells[xx][zz]);
				else
					ppSubCell[i] = NULL;
				break;

			case 1:
				if ( (x == 0) && (xx == 0) )
				{
					ppSubCell[i] = NULL;
					break;
				}

				if ( (x != 0) && (xx == 0) )
				{
					if ( m_pCells[x-1][z] != NULL )
						ppSubCell[i] = &(m_pCells[x-1][z]->SubCells[CELL_MAIN_DEVIDE-1][zz]);
					else
						ppSubCell[i] = NULL;
					break;
				}

				if ( m_pCells[x][z] != NULL )
					ppSubCell[i] = &(m_pCells[x][z]->SubCells[xx-1][zz]);
				else
					ppSubCell[i] = NULL;
				break;

			case 2:
				if ( (x == 0) && (xx == 0) )
				{
					ppSubCell[i] = NULL;
					break;
				}

				if ( (z == (CELL_MAIN_SIZE-1)) && ( zz == (CELL_MAIN_DEVIDE-1) ) )
				{
					ppSubCell[i] = NULL;
					break;
				}

				if ( (x != 0) && (xx == 0) )											// x ����, z ����.
				{
					if ( (z != (MAX_CELL_MAIN-1)) && ( zz == (CELL_MAIN_DEVIDE-1) ) )
						if ( m_pCells[x-1][z+1] != NULL )
							ppSubCell[i] = &(m_pCells[x-1][z+1]->SubCells[CELL_MAIN_DEVIDE-1][0]);
						else
							ppSubCell[i] = NULL;
					else
						if ( m_pCells[x-1][z] != NULL )
							ppSubCell[i] = &(m_pCells[x-1][z]->SubCells[CELL_MAIN_DEVIDE-1][zz+1]);
						else
							ppSubCell[i] = NULL;
					break;
				}

				if ( (z != (MAX_CELL_MAIN-1)) && (zz == (CELL_MAIN_DEVIDE-1) ) )		// x ����, z ����.
				{
					if ( (x != 0) && (xx == 0) )
						if ( m_pCells[x-1][z+1] != NULL )
							ppSubCell[i] = &(m_pCells[x-1][z+1]->SubCells[CELL_MAIN_DEVIDE-1][0]);	
						else
							ppSubCell[i] = NULL;
					else
						if ( m_pCells[x][z+1] != NULL )
							ppSubCell[i] = &(m_pCells[x][z+1]->SubCells[xx-1][0]);	
						else
							ppSubCell[i] = NULL;
					break;
				}
							
				if ( m_pCells[x][z] != NULL )
					ppSubCell[i] = &(m_pCells[x][z]->SubCells[xx-1][zz+1]);						
				else
					ppSubCell[i] = NULL;					
					break;

			case 3:
				if ( (z == (MAX_CELL_MAIN-1)) && (zz == (CELL_MAIN_DEVIDE-1)) )			// z ����.
				{
					ppSubCell[i] = NULL;
					break;
				}

				if ( (z != (MAX_CELL_MAIN-1)) && (zz == (CELL_MAIN_DEVIDE-1)) )
				{
					if ( m_pCells[x-1][z] != NULL )
						ppSubCell[i] = &(m_pCells[x-1][z]->SubCells[xx][0]);
					else
						ppSubCell[i] = NULL;
					break;
				}

				if ( m_pCells[x][z] != NULL )
					ppSubCell[i] = &(m_pCells[x][z]->SubCells[xx][zz+1]);
				else
					ppSubCell[i] = NULL;					
				break;

			case 4:
				if ( (x == (MAX_CELL_MAIN-1)) && (xx == (CELL_MAIN_DEVIDE-1)) )			// x ����, z ����.
				{
					ppSubCell[i] = NULL;
					break;
				}

				if ( (z == (MAX_CELL_MAIN-1)) && ( zz == (CELL_MAIN_DEVIDE-1)) )
				{
					ppSubCell[i] = NULL;
					break;
				}

				if ( (x != (MAX_CELL_MAIN-1)) && (xx == (CELL_MAIN_DEVIDE-1)) )
				{
					if ( (z != (MAX_CELL_MAIN-1)) && ( zz == (CELL_MAIN_DEVIDE-1)) )
						if ( m_pCells[x+1][z+1] != NULL )
							ppSubCell[i] = &(m_pCells[x+1][z+1]->SubCells[0][0]);
						else
							ppSubCell[i] = NULL;
					else
						if ( m_pCells[x+1][z] != NULL )
							ppSubCell[i] = &(m_pCells[x+1][z]->SubCells[0][zz+1]);
						else
							ppSubCell[i] = NULL;
					break;
				}

				if ( (z != (MAX_CELL_MAIN-1)) && (zz == (CELL_MAIN_DEVIDE-1)) )
				{
					if ( (x != (MAX_CELL_MAIN-1)) && (xx == (CELL_MAIN_DEVIDE-1)) )
						if ( m_pCells[x+1][z+1] != NULL )
							ppSubCell[i] = &(m_pCells[x+1][z+1]->SubCells[0][0]);	
						else
							ppSubCell[i] = NULL;
					else
						if ( m_pCells[x][z+1] != NULL )
							ppSubCell[i] = &(m_pCells[x][z+1]->SubCells[xx+1][0]);	
						else
							ppSubCell[i] = NULL;
					break;
				}

				if ( m_pCells[x][z] != NULL )								
					ppSubCell[i] = &(m_pCells[x][z]->SubCells[xx+1][zz+1]);						
				else
					ppSubCell[i] = NULL;					
				break;

			case 5:																		// x ����.
				if ( (x == (MAX_CELL_MAIN-1)) && (xx == (CELL_MAIN_DEVIDE-1)) )
				{
					ppSubCell[i] = NULL;
					break;
				}

				if ( (x != (MAX_CELL_MAIN-1)) && (xx == (CELL_MAIN_DEVIDE-1)) )
				{
					if ( m_pCells[x+1][z] != NULL )
						ppSubCell[i] = &(m_pCells[x+1][z]->SubCells[0][zz]);
					else
						ppSubCell[i] = NULL;
					break;
				}

				if ( m_pCells[x][z] != NULL )								
					ppSubCell[i] = &(m_pCells[x][z]->SubCells[xx+1][zz]);
				else
					ppSubCell[i] = NULL;					
				break;

			case 6:																		// x ����. z ����.		
				if ( (x == (MAX_CELL_MAIN-1)) && (xx == (CELL_MAIN_DEVIDE-1)) )
				{
					ppSubCell[i] = NULL;
					break;
				}

				if ( (z == 0) && (zz == 0) )
				{
					ppSubCell[i] = NULL;
					break;
				}

				if ( (x != (MAX_CELL_MAIN-1)) && (xx == (CELL_MAIN_DEVIDE-1)) )			
				{
					if ( (z != 0) && (zz == 0) )
						if ( m_pCells[x+1][z-1] != NULL )								
							ppSubCell[i] = &(m_pCells[x+1][z-1]->SubCells[0][CELL_MAIN_DEVIDE-1]);
						else
							ppSubCell[i] = NULL;
					else
						if ( m_pCells[x+1][z] != NULL )								
							ppSubCell[i] = &(m_pCells[x+1][z]->SubCells[0][zz-1]);
						else
							ppSubCell[i] = NULL;
					break;
				}

				if ( (z != 0) && (zz == 0) )
				{
					if ( (x != (CELL_MAIN_SIZE-1)) && (xx == (CELL_MAIN_DEVIDE-1) ) )
						if ( m_pCells[x+1][z-1] != NULL )								
							ppSubCell[i] = &(m_pCells[x+1][z-1]->SubCells[0][CELL_MAIN_DEVIDE-1]);
						else
							ppSubCell[i] = NULL;
					else
						if ( m_pCells[x][z-1] != NULL )								
							ppSubCell[i] = &(m_pCells[x][z-1]->SubCells[xx+1][CELL_MAIN_DEVIDE-1]);	
						else
							ppSubCell[i] = NULL;
					break;
				}

				if ( m_pCells[x][z] != NULL )								
					ppSubCell[i] = &(m_pCells[x][z]->SubCells[xx+1][zz-1]);						
				else
					ppSubCell[i] = NULL;					
				break;

			case 7:																		// z ����.
				if ( (z == 0) && (zz == 0) )	
				{
					ppSubCell[i] = NULL;
					break;
				}

				if ( (z != 0) && (zz == 0) )
				{
					if ( m_pCells[x][z-1] != NULL )								
						ppSubCell[i] = &(m_pCells[x][z-1]->SubCells[xx][CELL_MAIN_DEVIDE-1]);
					else
						ppSubCell[i] = NULL;					
					break;
				}

				if ( m_pCells[x][z] != NULL )								
					ppSubCell[i] = &(m_pCells[x][z]->SubCells[xx][zz-1]);
				else
					ppSubCell[i] = NULL;					
				break;

			case 8:																		// x ����, z ����.
				if ( (x == 0) && (xx == 0) )
				{
					ppSubCell[i] = NULL;
					break;
				}

				if ( (z == 0) && (zz == 0) )
				{
					ppSubCell[i] = NULL;
					break;
				}

				if ( (x != 0) && (xx == 0) )
				{
					if ( (z != 0) && (zz == 0) )
						if ( m_pCells[x-1][z-1] != NULL )								
							ppSubCell[i] = &(m_pCells[x-1][z-1]->SubCells[CELL_MAIN_DEVIDE-1][CELL_MAIN_DEVIDE-1]);
						else
							ppSubCell[i] = NULL;
					else
						if ( m_pCells[x-1][z] != NULL )								
							ppSubCell[i] = &(m_pCells[x-1][z]->SubCells[CELL_MAIN_DEVIDE-1][zz-1]);
						else
							ppSubCell[i] = NULL;
					break;
				}

				if ( (z != 0) && (zz == 0) )
				{
					if ( (x != 0) && (xx == 0) )
						if ( m_pCells[x-1][z-1] != NULL )								
							ppSubCell[i] = &(m_pCells[x-1][z-1]->SubCells[CELL_MAIN_DEVIDE-1][CELL_MAIN_DEVIDE-1]);
						else
							ppSubCell[i] = NULL;
					else
						if ( m_pCells[x][z-1] != NULL )								
							ppSubCell[i] = &(m_pCells[x][z-1]->SubCells[xx-1][CELL_MAIN_DEVIDE-1]);	
						else
							ppSubCell[i] = NULL;
					break;
				}
							
				if ( m_pCells[x][z] != NULL )								
					ppSubCell[i] = &(m_pCells[x][z]->SubCells[xx-1][zz-1]);						
				else
					ppSubCell[i] = NULL;
				break;
		}	// switch
	}	// for 
}

