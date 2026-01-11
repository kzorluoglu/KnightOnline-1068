// N3ShapeMgr.cpp: implementation of the CN3ShapeMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfxBase.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "N3ShapeMgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CN3ShapeMgr::CN3ShapeMgr()
{
	m_nShapeCount = 0;
	memset(m_pShapes, 0, sizeof(m_pShapes));

	m_nCollisionFaceCount = 0;
	m_pvCollisions = NULL;

	memset(m_pCells, 0, sizeof(m_pCells));
}

CN3ShapeMgr::~CN3ShapeMgr()
{
	this->Release();
}

void CN3ShapeMgr::Release()
{
	this->ReleaseShapes();

	m_nCollisionFaceCount = 0;
	delete [] m_pvCollisions; m_pvCollisions = NULL;

	for(int z = 0; z < MAX_CELL_MAIN; z++)
	{
		for(int x = 0; x < MAX_CELL_MAIN; x++)
		{
			delete m_pCells[x][z];
		}
	}
	memset(m_pCells, 0, sizeof(MAX_CELL_MAIN));

	m_ShapesToRender.Release();
}

void CN3ShapeMgr::ReleaseShapes()
{
	for(int i = 0; i < m_nShapeCount; i++)
	{
		delete m_pShapes[i];
	}
	m_nShapeCount = 0;
	memset(m_pShapes, 0, sizeof(m_pShapes));
}

bool CN3ShapeMgr::Load(HANDLE hFile)
{
	DWORD dwRWC;
	int nL = 0;

	if(m_nShapeCount > 0) ReleaseShapes();

	__Vector3 v;
	__Quaternion qt;
	char szFN[512];
	ReadFile(hFile, &m_nShapeCount, 4, &dwRWC, NULL); // Shape Count
	for(int i = 0; i < m_nShapeCount; i++)
	{
		ReadFile(hFile, &nL, 4, &dwRWC, NULL); // Mesh FileName
		__ASSERT(nL > 0, "Error : Shape has null name");
		if(nL <= 0) continue;

		ReadFile(hFile, szFN, nL, &dwRWC, NULL); szFN[nL] = NULL; // Shape ���� �̸�..

		if(m_pShapes[i]) delete m_pShapes[i];
		m_pShapes[i] = new CN3Shape();
		m_pShapes[i]->LoadFromFile(szFN);

		ReadFile(hFile, &v, sizeof(__Vector3), &dwRWC, NULL); // ��ġ
		m_pShapes[i]->PosSet(v);
		ReadFile(hFile, &qt, sizeof(__Quaternion), &dwRWC, NULL); // ȸ��
		m_pShapes[i]->RotSet(qt);
		ReadFile(hFile, &v, sizeof(__Vector3), &dwRWC, NULL); // ������
		m_pShapes[i]->ScaleSet(v);
	}

	// �浹 üũ ������ ������ ����..
	ReadFile(hFile, &m_nCollisionFaceCount, 4, &dwRWC, NULL);
	delete [] m_pvCollisions; m_pvCollisions = NULL;
	if(m_nCollisionFaceCount > 0)
	{
		m_pvCollisions = new __Vector3[m_nCollisionFaceCount * 3];
		ReadFile(hFile, m_pvCollisions, sizeof(__Vector3) * m_nCollisionFaceCount * 3, &dwRWC, NULL);
	}

	// Cell Data ����.
	BOOL bExist = FALSE;
	for(int z = 0; z < MAX_CELL_MAIN; z++)
	{
		for(int x = 0; x < MAX_CELL_MAIN; x++)
		{
			if(m_pCells[x][z]) delete m_pCells[x][z];
			m_pCells[x][z] = NULL;

			ReadFile(hFile, &bExist, 4, &dwRWC, NULL); // �����Ͱ� �ִ� ������ ����..
			if(FALSE == bExist) continue;

			m_pCells[x][z] = new __CellMain;
			m_pCells[x][z]->Load(hFile);

//			TRACE("ShapeMgr : MainCell(%d, %d)\n", x, z);
		}
	}

	return true;
}

bool CN3ShapeMgr::Save(HANDLE hFile)
{
	DWORD dwRWC;
	int nL = 0;

	__Vector3 v;
	__Quaternion qt;
	WriteFile(hFile, &m_nShapeCount, 4, &dwRWC, NULL); // Shape Count
	for(int i = 0; i < m_nShapeCount; i++)
	{
		nL = lstrlen(m_pShapes[i]->FileName());
		WriteFile(hFile, &nL, 4, &dwRWC, NULL); // Mesh FileName
		if(nL <= 0) continue;
		
		WriteFile(hFile, m_pShapes[i]->FileName(), nL, &dwRWC, NULL); // Shape ���� �̸�..
		v = m_pShapes[i]->Pos();
		WriteFile(hFile, &v, sizeof(__Vector3), &dwRWC, NULL); // ��ġ
		qt = m_pShapes[i]->Rot();
		WriteFile(hFile, &qt, sizeof(__Quaternion), &dwRWC, NULL); // ȸ��
		v = m_pShapes[i]->Scale();
		WriteFile(hFile, &v, sizeof(__Vector3), &dwRWC, NULL); // ������
	}

	// �浹 üũ ������ ������ ����..
	WriteFile(hFile, &m_nCollisionFaceCount, 4, &dwRWC, NULL);
	if(m_nCollisionFaceCount > 0)
	{
		WriteFile(hFile, m_pvCollisions, sizeof(__Vector3) * m_nCollisionFaceCount * 3, &dwRWC, NULL);
	}

	// Cell Data ����.
	for(int z = 0; z < MAX_CELL_MAIN; z++)
	{
		for(int x = 0; x < MAX_CELL_MAIN; x++)
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

void CN3ShapeMgr::Create(int nCellWidthCount, int nCellLengthCount) // Cell ������ �ִ´�. 
{
	if(nCellWidthCount <= 0 || nCellLengthCount <= 0) return;

	int nW = nCellWidthCount / (CELL_MAIN_DEVIDE * CELL_SUB_SIZE) + 1;
	int nH = nCellLengthCount / (CELL_MAIN_DEVIDE * CELL_SUB_SIZE) + 1;

	__ASSERT(nW <= MAX_CELL_MAIN && nH <= MAX_CELL_MAIN, "Cell count is too large");
}

void CN3ShapeMgr::GenerateCollisionData()
{
	int nFC = 0;
	for(int i = 0; i < m_nShapeCount; i++) // Shape �� �ִ� �浹 �޽� ��ŭ ����.
	{
		CN3VMesh* pVM = m_pShapes[i]->CollisionMesh();
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

	if(nFC <= 0) return;
	
	m_nCollisionFaceCount = nFC;
	delete [] m_pvCollisions; m_pvCollisions = NULL;
	m_pvCollisions = new __Vector3[nFC * 3]; // �浹 ������ ����
	memset(m_pvCollisions, 0, sizeof(__Vector3) * nFC * 3);

	int nCPC = 0; // Collision Polygon Count
	for(i = 0; i < m_nShapeCount; i++) // Shape �� �ִ� �浹 �޽� ��ŭ ������ ����..
	{
		CN3VMesh* pVMesh = m_pShapes[i]->CollisionMesh();
		if (NULL == pVMesh) continue;
		__Vector3 *pVSrc = pVMesh->Vertices();
		int nIC = pVMesh->IndexCount();
		if(nIC > 0)
		{
			WORD* pwIs = pVMesh->Indices();
			for(int j = 0; j < nIC; j++)
			{
				m_pvCollisions[nCPC++] = pVSrc[pwIs[j]] * m_pShapes[i]->m_Matrix; // ���� ��ġ�̴�.
			}
		}
		else 
		{
			int nVC = pVMesh->VertexCount();
			for(int j = 0; j < nVC; j++)
			{
				m_pvCollisions[nCPC++] = pVSrc[j] * m_pShapes[i]->m_Matrix; // ���� ��ġ�̴�.
			}
		}
	}

	// �� ������ ���� ���� �ִ��� Ȥ�� ���ԵǾ� �ִ��� ��� �Ǵ��ؼ� �ε��� ������ �Ѵ�.
	for(i = 0; i < nFC; i++)
	{
		__Vector3 vEdge[3][2];

		vEdge[0][0] = m_pvCollisions[i*3];
		vEdge[0][1] = m_pvCollisions[i*3+1];
		vEdge[1][0] = m_pvCollisions[i*3+1];
		vEdge[1][1] = m_pvCollisions[i*3+2];
		vEdge[2][0] = m_pvCollisions[i*3+2];
		vEdge[2][1] = m_pvCollisions[i*3];

		// ���� �ִ� �޽� ��ŭ ����...
		for(int j = 0; j < 3; j++)
		{
			// ������ ���ϰ�..
			int xx1 = 0, xx2 = 0, zz1 = 0, zz2 = 0;

			if(vEdge[j][0].x < vEdge[j][1].x) 
			{
				xx1 = (int)(vEdge[j][0].x / CELL_SUB_SIZE);
				xx2 = (int)(vEdge[j][1].x / CELL_SUB_SIZE);
			}
			else
			{
				xx1 = (int)(vEdge[j][1].x / CELL_SUB_SIZE);
				xx2 = (int)(vEdge[j][0].x / CELL_SUB_SIZE);
			}
			

			if(vEdge[j][0].z < vEdge[j][1].z)
			{
				zz1 = (int)(vEdge[j][0].z / CELL_SUB_SIZE);
				zz2 = (int)(vEdge[j][1].z / CELL_SUB_SIZE);
			}
			else
			{
				zz1 = (int)(vEdge[j][1].z / CELL_SUB_SIZE);
				zz2 = (int)(vEdge[j][0].z / CELL_SUB_SIZE);
			}

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


					if(bWriteID) // �浹 ������ ��� �Ѵ�..
					{
						int nX = x / CELL_MAIN_DEVIDE;
						int nZ = z / CELL_MAIN_DEVIDE;
						__ASSERT(nX >= 0 && nX < MAX_CELL_MAIN && nZ >= 0 && nZ < MAX_CELL_MAIN, "Shape Add Failed. Invalid position");
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
							if(i * 3 == pSubCell->pwCCVertIndices[k*3]) 
							{
								bOverlapped = true;
								break;
							}
						}

						if(false == bOverlapped) // �ߺ��Ȱ� ������..
						{
							if(0 == nCCPC) // ÷ ���� �Ǿ�����..
							{
								pSubCell->pwCCVertIndices = new WORD[(nCCPC+1)*3];
							}
							else // �̹� ������..
							{
								WORD* pwBack = pSubCell->pwCCVertIndices;
								pSubCell->pwCCVertIndices = new WORD[(nCCPC+1)*3];
								memcpy(pSubCell->pwCCVertIndices, pwBack, nCCPC * 3 * 2); // �������� �ϳ��� �������̰� ���� �ε����̹Ƿ�..
								delete [] pwBack;
							}
							
							pSubCell->pwCCVertIndices[nCCPC*3+0] = i * 3 + 0; // �ε��� ����..
							pSubCell->pwCCVertIndices[nCCPC*3+1] = i * 3 + 1; // �ε��� ����..
							pSubCell->pwCCVertIndices[nCCPC*3+2] = i * 3 + 2; // �ε��� ����..

							pSubCell->nCCPolyCount++; // Collision Check Polygon Count �� �ø���.
						}
					}
				}
			}
		}
	}
}

int CN3ShapeMgr::Add(CN3Shape *pShape)
{
	if(NULL == pShape) return -1;

	m_pShapes[m_nShapeCount] = new CN3Shape();
	if(false == m_pShapes[m_nShapeCount]->LoadFromFile(pShape->FileName()))
	{
		delete m_pShapes[m_nShapeCount];
		m_pShapes[m_nShapeCount] = NULL;
		return -1;
	}

	m_pShapes[m_nShapeCount]->ScaleSet(pShape->Scale());
	m_pShapes[m_nShapeCount]->RotSet(pShape->Rot());
	m_pShapes[m_nShapeCount]->PosSet(pShape->Pos());

	__Vector3 vPos = pShape->Pos();
	int nX = (int)(vPos.x / (CELL_MAIN_DEVIDE * CELL_SUB_SIZE));
	int nZ = (int)(vPos.z / (CELL_MAIN_DEVIDE * CELL_SUB_SIZE));

	__ASSERT(nX >= 0 && nX < MAX_CELL_MAIN && nZ >= 0 && nZ < MAX_CELL_MAIN, "Shape Add Failed. Invalid position");
	if(NULL == m_pCells[nX][nZ])
	{
		m_pCells[nX][nZ] = new __CellMain;
	}

	int nSC = m_pCells[nX][nZ]->nShapeCount;
	if(0 == nSC) // ÷ ���� �Ǿ�����..
	{
		m_pCells[nX][nZ]->pwShapeIndices = new WORD[nSC+1];
	}
	else // �̹� ������..
	{
		WORD* pwBack = m_pCells[nX][nZ]->pwShapeIndices;
		m_pCells[nX][nZ]->pwShapeIndices = new WORD[nSC+1];
		memcpy(m_pCells[nX][nZ]->pwShapeIndices, pwBack, nSC * 2);
		delete [] pwBack;
	}
	
	m_pCells[nX][nZ]->pwShapeIndices[nSC] = m_nShapeCount; // �ε��� ����..

	m_nShapeCount++; // Shape Count �� �ø���.
	m_pCells[nX][nZ]->nShapeCount++; // Shape Count �� �ø���.

	return m_nShapeCount - 1;
}

void CN3ShapeMgr::Tick()
{
	int xMainS = (int)((s_CameraData.vEye.x - s_CameraData.fFP/2.0f) / CELL_MAIN_SIZE); if(xMainS < 0) xMainS = 0;
	int xMainE = (int)((s_CameraData.vEye.x + s_CameraData.fFP/2.0f) / CELL_MAIN_SIZE); if(xMainS > MAX_CELL_MAIN) xMainE = MAX_CELL_MAIN;
	int zMainS = (int)((s_CameraData.vEye.z - s_CameraData.fFP/2.0f) / CELL_MAIN_SIZE); if(zMainS < 0) zMainS = 0;
	int zMainE = (int)((s_CameraData.vEye.z + s_CameraData.fFP/2.0f) / CELL_MAIN_SIZE); if(zMainS > MAX_CELL_MAIN) zMainE = MAX_CELL_MAIN;
	
	__CellMain* pCellCur = NULL;
	CN3Shape* pShpCur = NULL;
	int nShp = 0, nSI = 0;
	m_ShapesToRender.Release();
	// ������ ����Ʈ ����..
	
	for(int zM = zMainS; zM < zMainE; zM++)
	{
		for(int xM = xMainS; xM < xMainE; xM++)
		{
			pCellCur = m_pCells[xM][zM];
			if(NULL == pCellCur) continue;

			int nSC = pCellCur->nShapeCount;
			for(nShp = 0; nShp < nSC; nShp++)
			{
				nSI = pCellCur->pwShapeIndices[nShp];
				__ASSERT(nSI >= 0 && nSI < m_nShapeCount, "Shape Index is invalid");
				pShpCur = m_pShapes[nSI];
				__ASSERT(pShpCur, "Shape pointer is null!!!");

				pShpCur->Tick();
				if(pShpCur->GetDontRender()) continue;

				m_ShapesToRender.Add(pShpCur);
			}
		}
	}
}

void CN3ShapeMgr::Render()
{
	CN3Shape* pShpCur = NULL;
	for(void* pPos = m_ShapesToRender.Position(0); pPos; m_ShapesToRender.Next(pPos))
	{
		pShpCur = m_ShapesToRender.Get(pPos);
		__ASSERT(pShpCur, "Shape pointer is null!!!");

		pShpCur->Render();
	}
}

bool CN3ShapeMgr::CheckCollision(__Vector3 &vPos, __Vector3 &vPosNext, __Vector3 *pvCol, __Vector3 *pvNormal)
{
	__CellSub* pCell = this->SubCell(vPos);
	if(NULL == pCell || pCell->nCCPolyCount <= 0) return false;

	__Vector3 vDir = vPosNext - vPos;

	bool bCol0, bCol1;

	int nIndex0, nIndex1, nIndex2;
	for(int i = 0; i < pCell->nCCPolyCount; i++)
	{
		nIndex0 = pCell->pwCCVertIndices[i*3];
		nIndex1 = pCell->pwCCVertIndices[i*3+1];
		nIndex2 = pCell->pwCCVertIndices[i*3+2];
		bCol0 = ::IntersectTriangle(vPos, vDir, m_pvCollisions[nIndex0], m_pvCollisions[nIndex1], m_pvCollisions[nIndex2]); // ������ġ�� ���⿡�� �浹�� �ְ�..
		if(bCol0)
		{
			bCol1 = ::IntersectTriangle(vPosNext, vDir, m_pvCollisions[nIndex0], m_pvCollisions[nIndex1], m_pvCollisions[nIndex2]); // ���� ��ġ�� ���࿡�� �浹�� ������..
			if(bCol1)
			{
				if(pvCol)
				{
					float fT, fU, fV;
					::IntersectTriangle(vPosNext, vDir, m_pvCollisions[nIndex0], m_pvCollisions[nIndex1], m_pvCollisions[nIndex2], fT, fU, fV, pvCol); // ���� ��ġ�� ���࿡�� �浹�� ������..
				}
				if(pvNormal)
				{
					(*pvNormal).Cross(m_pvCollisions[nIndex1] - m_pvCollisions[nIndex0], m_pvCollisions[nIndex2] - m_pvCollisions[nIndex1]);
				}
				return true;
			}
		}
	}

	return false;
}

void CN3ShapeMgr::RenderCollision(__Vector3 &vPos)
{
	int x = (int)(vPos.x / CELL_MAIN_SIZE);
	int z = (int)(vPos.z / CELL_MAIN_SIZE);

	__CellSub* pSubCell = this->SubCell(vPos);
	if(NULL == pSubCell || pSubCell->nCCPolyCount <= 0) return;

	__Matrix44 mtxWorld;
	mtxWorld.Identity();
	s_lpD3DDev->SetTransform(D3DTS_WORLD, &mtxWorld);
	
	DWORD dwFillPrev;
	s_lpD3DDev->GetRenderState(D3DRS_FILLMODE, &dwFillPrev);

	s_lpD3DDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	s_lpD3DDev->SetTexture(0, NULL);

	int nFC = pSubCell->nCCPolyCount;
	WORD* pwIndices = pSubCell->pwCCVertIndices;
	s_lpD3DDev->SetFVF(D3DFVF_XYZ);
	s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_nCollisionFaceCount * 3, nFC, pwIndices, D3DFMT_INDEX16, m_pvCollisions, sizeof(__Vector3));

	s_lpD3DDev->SetRenderState(D3DRS_FILLMODE, dwFillPrev);
}
