// N3FXPMeshInstance.cpp: implementation of the CN3FXPMeshInstance class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfxBase.h"
#include "N3FXPMeshInstance.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CN3FXPMeshInstance::CN3FXPMeshInstance()
{
	m_pFXPMesh			= NULL;
	m_pIndices			= NULL;
	m_pColorVertices	= NULL;
	m_iNumVertices		= 0;
	m_iNumIndices		= 0;
	m_pCollapseUpTo		= NULL;
}

CN3FXPMeshInstance::CN3FXPMeshInstance(CN3FXPMesh* pN3FXPMesh)
{
	m_pFXPMesh			= NULL;
	m_pIndices			= NULL;
	m_pColorVertices	= NULL;
	m_iNumVertices		= 0;
	m_iNumIndices		= 0;
	m_pCollapseUpTo		= NULL;

	CN3FXPMeshInstance::Create(pN3FXPMesh);
}

CN3FXPMeshInstance::CN3FXPMeshInstance(const std::string& szFN)
{
	m_pFXPMesh			= NULL;
	m_pIndices			= NULL;
	m_pColorVertices	= NULL;
	m_iNumVertices		= 0;
	m_iNumIndices		= 0;
	m_pCollapseUpTo		= NULL;

	this->Create(szFN);
}

CN3FXPMeshInstance::~CN3FXPMeshInstance()
{
	if (m_pColorVertices)	{ delete[] m_pColorVertices;m_pColorVertices = NULL;}
	if (m_pIndices)			{ delete[] m_pIndices;m_pIndices = NULL;}

	s_MngFXPMesh.Delete(&m_pFXPMesh);				//���۷��� ī��Ʈ�� ���̱� ����
}

void CN3FXPMeshInstance::Release()
{
	if (m_pColorVertices)	{ delete[] m_pColorVertices;m_pColorVertices = NULL;}
	if (m_pIndices)			{ delete[] m_pIndices;m_pIndices = NULL;}

	s_MngFXPMesh.Delete(&m_pFXPMesh);				//���۷��� ī��Ʈ�� ���̱� ����

	m_pCollapseUpTo = NULL;
	m_iNumVertices = 0;
	m_iNumIndices = 0;
}

bool CN3FXPMeshInstance::Create(CN3FXPMesh* pN3FXPMesh)
{
	if(pN3FXPMesh == NULL)
	{
		CN3FXPMeshInstance::Release();
		return false;
	}

	m_pFXPMesh = pN3FXPMesh;
	if(m_pFXPMesh == NULL)
	{
		__ASSERT(m_pFXPMesh, "Failed new CN3FXPMesh");
		return false;
	}

	m_szName = pN3FXPMesh->m_szName;

	int iMaxNumVertices = m_pFXPMesh->GetMaxNumVertices();
	if (iMaxNumVertices>0)
	{
		if(m_pColorVertices) { delete[] m_pColorVertices;m_pColorVertices = NULL;}
		m_pColorVertices = new __VertexXyzColorT1[iMaxNumVertices];
		__ASSERT(m_pColorVertices, "Failed to create Vertex buffer");
		CopyMemory(m_pColorVertices, m_pFXPMesh->m_pColorVertices, iMaxNumVertices * sizeof(__VertexXyzColorT1));
	}

	// And setup my instance-specific data.
	// We start with the lowest level of detail.
	int iMaxNumIndices = m_pFXPMesh->GetMaxNumIndices();
	if (iMaxNumIndices>0)
	{
		if(m_pIndices) { delete[] m_pIndices;m_pIndices = NULL;}
		m_pIndices = new WORD[m_pFXPMesh->m_iMaxNumIndices];
		__ASSERT(m_pIndices, "Failed to create index buffer");
		CopyMemory(m_pIndices, m_pFXPMesh->m_pIndices, m_pFXPMesh->m_iMaxNumIndices * sizeof(WORD));
	}

	// lowest level of detail right now.
	m_iNumVertices = m_pFXPMesh->GetMinNumVertices();
	m_iNumIndices  = m_pFXPMesh->GetMinNumIndices();

	// So far, we're at the last collapse
	m_pCollapseUpTo = m_pFXPMesh->m_pCollapses;

	return true;
}

bool CN3FXPMeshInstance::Create(const std::string& szFN)
{
	if (m_pFXPMesh && m_pFXPMesh->FileName() == szFN) return true;	// ���� �̸��� ������ ���� ������ �ʰ� ��������
	this->Release();

	CN3FXPMesh* pN3FXPMesh = s_MngFXPMesh.Get(szFN);
	return this->Create(pN3FXPMesh);
}

void CN3FXPMeshInstance::SetLODByNumVertices(int iNumVertices)
{
	if(m_pCollapseUpTo == NULL) return;

	int iDiff = iNumVertices - m_iNumVertices;

	if(iDiff == 0)
	{
		return;
	}
	else if (iDiff > 0)
	{
		while(iNumVertices > m_iNumVertices)
		{
			if (m_pCollapseUpTo->NumVerticesToLose + m_iNumVertices > iNumVertices) break;		// ������ ���� �ڵ�..
			if (SplitOne() == false) break;
		}
	}
	else if (iDiff < 0)
	{
		iDiff = -iDiff;
		while(iNumVertices < m_iNumVertices)
		{
			if (CollapseOne() == false) break;
		}
	}

	while(m_pCollapseUpTo->bShouldCollapse)
	{
		if (SplitOne() == false) break;
	}
}

void CN3FXPMeshInstance::SetLOD(float value)
{
#define _USE_LODCONTROL_VALUE
#ifdef _USE_LODCONTROL_VALUE
	// value�� distance * FOV�̴�.
	if (m_pFXPMesh == NULL ) return;

	if (m_pFXPMesh->m_iLODCtrlValueCount == 0)
	{	// LODCtrlValue�� ������ ��� �׸���.
		SetLODByNumVertices(0x7fffffff);
		return;
	}
	
	__ASSERT(m_pFXPMesh->m_pLODCtrlValues, "LOD control value is NULL!");

	CN3PMesh::__LODCtrlValue* pTmpLODCV = m_pFXPMesh->m_pLODCtrlValues + m_pFXPMesh->m_iLODCtrlValueCount-1;

	if (value < m_pFXPMesh->m_pLODCtrlValues[0].fDist)
	{		// �ּ� ����ġ���� �����Ƿ� ���� ���� ������ �׸���.
		SetLODByNumVertices(m_pFXPMesh->m_pLODCtrlValues[0].iNumVertices);
	}
	else if ( pTmpLODCV->fDist < value)
	{		// �ִ� ����ġ���� �ָ� �����Ƿ� ���� ���� ������ �׸���.
		SetLODByNumVertices(pTmpLODCV->iNumVertices);
	}
	else
	{		// �߰� ���� �°� ������ �� ���� �׸���.
		for (int i=1; i< m_pFXPMesh->m_iLODCtrlValueCount; ++i)
		{
			if (value < m_pFXPMesh->m_pLODCtrlValues[i].fDist)
			{
				CN3PMesh::__LODCtrlValue* pHiValue = m_pFXPMesh->m_pLODCtrlValues + i;
				CN3PMesh::__LODCtrlValue* pLowValue = pHiValue - 1;
				float fVertices = (pHiValue->iNumVertices - pLowValue->iNumVertices)*
									(value - pLowValue->fDist)/(pHiValue->fDist - pLowValue->fDist);
				SetLODByNumVertices(pLowValue->iNumVertices + (int)fVertices);
				break;
			}
		}
	}
#else
	// value�� distance * FOV�̴�.
	if (m_pCollapseUpTo == NULL || m_pFXPMesh == NULL) return;

	const int iLODCtrlValueCount = 5;
	__PMLODCtrlValue LODCtrlValues[iLODCtrlValueCount];
	__PMLODCtrlValue* pTmpLODCV = &(LODCtrlValues[iLODCtrlValueCount-1]);

	int iMaxNumVertices = m_pFXPMesh->GetMaxNumVertices();
	int iMinNumVertices = m_pFXPMesh->GetMinNumVertices();
	int iDiff = iMaxNumVertices - iMinNumVertices;
	
	float fVolume = m_pFXPMesh->GetVolume();
	float fD = (sqrtf(fVolume)*3.0f) / (value * 1.0f);
	fD = 1.0f;
//	float fD = fVolume/(value*13.0f) * (400.0f/(float)iMaxNumVertices);
	if(fD > 1.0f) SetLODByNumVertices(iMaxNumVertices);
	else if(fD < 0.1f) SetLODByNumVertices(iMinNumVertices);
	else SetLODByNumVertices(iMinNumVertices + (int)(iDiff * fD));

#endif
}

bool CN3FXPMeshInstance::CollapseOne()
{
	if (m_pCollapseUpTo <= m_pFXPMesh->m_pCollapses) return false;

	m_pCollapseUpTo--;

	m_iNumIndices -= m_pCollapseUpTo->NumIndicesToLose;
	
	for (	int *i = m_pFXPMesh->m_pAllIndexChanges + m_pCollapseUpTo->iIndexChanges;
			i < m_pFXPMesh->m_pAllIndexChanges + m_pCollapseUpTo->iIndexChanges + m_pCollapseUpTo->NumIndicesToChange;
			i++)
	{
		m_pIndices[*i] = m_pCollapseUpTo->CollapseTo;
	}

	m_iNumVertices -= m_pCollapseUpTo->NumVerticesToLose;

	return true;
}

bool CN3FXPMeshInstance::SplitOne()
{
	if (m_pCollapseUpTo >= m_pFXPMesh->m_pCollapses + m_pFXPMesh->m_iNumCollapses) return false; // �̷��� �ϸ� ������ �ϳ��� ���� ������ �ȴ�..
	// ������ �̷��� �ٽ� �ϴ� ������ �Ʒ� �ڵ�� �ϸ� ������ �������� ���� �׷����� �ʴ´�.
	// �̷��� �ص� ������ �� �ֵ��� ����ڵ带 �־���. m_pFXPMesh->m_pCollapses �� �Ҵ��Ҷ� 1�� �� �Ҵ��ϰ� ������ �����͸� �ʱⰪ���� �־���.
//	if (m_pCollapseUpTo >= m_pFXPMesh->m_pCollapses + m_pFXPMesh->m_iNumCollapses - 1) return false; // �̰� �����̴�..

	m_iNumIndices  += m_pCollapseUpTo->NumIndicesToLose;
	m_iNumVertices += m_pCollapseUpTo->NumVerticesToLose;

	if(m_pFXPMesh->m_pAllIndexChanges)
	{
		for (	int *i = m_pFXPMesh->m_pAllIndexChanges + m_pCollapseUpTo->iIndexChanges;
				i < m_pFXPMesh->m_pAllIndexChanges + m_pCollapseUpTo->iIndexChanges + m_pCollapseUpTo->NumIndicesToChange;
				i++)
		{
			m_pIndices[*i] = m_iNumVertices - 1;
		}
	}

	m_pCollapseUpTo++;
	return true;
}

void CN3FXPMeshInstance::Render()
{
	if (m_pFXPMesh == NULL) return;
	s_lpD3DDev->SetFVF(FVF_VNT1);

	const int iPCToRender = 1000;	// primitive count to render
	if(m_iNumIndices > 3)
	{
		int iPC = m_iNumIndices / 3;

		int iLC = iPC / iPCToRender;
		int i;
		for (i=0; i<iLC; ++i)
		{
			s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_iNumVertices, iPCToRender, m_pIndices + i*iPCToRender*3, D3DFMT_INDEX16, m_pColorVertices, sizeof(__VertexXyzColorT1));
		}

		int iRPC = iPC%iPCToRender;
		if(iRPC > 0) s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_iNumVertices, iRPC, m_pIndices + i*iPCToRender*3, D3DFMT_INDEX16, m_pColorVertices, sizeof(__VertexXyzColorT1));
	}
}

void CN3FXPMeshInstance::RenderTwoUV()
{
	if(NULL == m_pFXPMesh) return;
	if(NULL == m_pFXPMesh->GetVertices2())
	{
		m_pFXPMesh->GenerateSecondUV(); // �ι�° UV �� ���� ���� �����..
	}
	if(NULL == m_pFXPMesh->GetVertices2()) return;
	
	s_lpD3DDev->SetFVF(FVF_VNT2);

	const int iPCToRender = 1000;	// primitive count to render
	if(m_iNumIndices > 3)
	{
		int iPC = m_iNumIndices / 3;

		int iLC = iPC / iPCToRender;
		int i;
		for (i=0; i<iLC; ++i)
		{
			s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_iNumVertices, iPCToRender, m_pIndices + i*iPCToRender*3, D3DFMT_INDEX16, m_pFXPMesh->m_pVertices2, sizeof(__VertexT2));
		}

		int iRPC = iPC%iPCToRender;
		if(iRPC > 0) s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_iNumVertices, iRPC, m_pIndices + i*iPCToRender*3, D3DFMT_INDEX16, m_pFXPMesh->m_pVertices2, sizeof(__VertexT2));
	}
}

#ifdef _USE_VERTEXBUFFER
LPDIRECT3DVERTEXBUFFER9	CN3FXPMeshInstance::GetVertexBuffer() const
{
	if (m_pFXPMesh == NULL) return NULL;
	return m_pFXPMesh->GetVertexBuffer();
}

#else
__VertexXyzColorT1*	CN3FXPMeshInstance::GetVertices() const
{
	return m_pColorVertices;
}
#endif



void CN3FXPMeshInstance::SetColor(DWORD dwColor)
{
	if (m_pColorVertices == NULL)	return;
	if (m_iNumVertices <= 0)		return;

	for(int i=0;i<m_iNumVertices;i++)
	{
		m_pColorVertices[i].color = dwColor;
	}
}
