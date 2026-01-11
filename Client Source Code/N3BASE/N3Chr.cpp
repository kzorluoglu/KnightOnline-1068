// N3Chr.cpp: implementation of the CN3Chr class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfxBase.h"
#include "N3SndObj.h"
#include "N3Chr.h"
#include "N3SkyMng.h"
#include "N3Sun.h"
#include "N3FXBundle.h"
#include "N3FXPartBase.h"
#include "N3FXPartBillBoard.h"
#include "N3FXPlug.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const float CHR_LOD_CALCULATION_VALUES[MAX_CHR_LOD_DELTA][MAX_CHR_LOD] =
{
//	{ 6.0f, 18.0f, 32.0f, 128.0f },
//	{ 4.0f, 12.0f, 21.0f, 85.0f },
//	{ 2.66f, 8.0f, 14.21f, 56.8f },
//	{ 1.77f, 5.33f, 9.48f, 37.9f }
	{ 8.0f, 16.0f, 48.0f, 128.0f },
	{ 4.0f, 8.0f, 24.0f, 64.0f },
	{ 2.0f, 4.0f, 12.0f, 32.0f },
	{ 1.0f, 3.0f, 5.0f, 7.0f }
//	{ 1.0f, 2.0f, 6.0f, 16.0f }
};

CN3CPartSkins::CN3CPartSkins()
{
	m_dwType |= OBJ_CHARACTER_PART_SKINS;
}

CN3CPartSkins::~CN3CPartSkins()
{
}

void CN3CPartSkins::Release()
{
	for(int i = 0; i < MAX_CHR_LOD; i++)
	{
		m_Skins[i].Release();
	}
}

bool CN3CPartSkins::Load(HANDLE hFile)
{
	CLogWriter::Write("CN3CPartSkins::Load begin");
	try {
		CN3BaseFileAccess::Load(hFile);
		CLogWriter::Write("CN3CPartSkins::Load after base load");
	} catch (...) {
		CLogWriter::Write("CN3CPartSkins::Load base load exception");
		return false;
	}
	
	for(int i = 0; i < MAX_CHR_LOD; i++)
	{
		try {
			CLogWriter::Write("CN3CPartSkins::Load loading LOD %d", i);
			m_Skins[i].Load(hFile);
			CLogWriter::Write("CN3CPartSkins::Load LOD %d loaded successfully", i);
		} catch (...) {
			CLogWriter::Write("CN3CPartSkins::Load LOD %d exception - skipping", i);
			m_Skins[i].Release();
		}
	}
	CLogWriter::Write("CN3CPartSkins::Load end");
	return true;
}

#ifdef _N3TOOL
bool CN3CPartSkins::Save(HANDLE hFile)
{
	CN3BaseFileAccess::Save(hFile);
	for(int i = 0; i < MAX_CHR_LOD; i++)
	{
		m_Skins[i].Save(hFile);
	}

	return true;
}
#endif // end of _N3TOOL

////////////////////////////////
// Part, Plug....
CN3CPart::CN3CPart()
{
	m_dwType |= OBJ_CHARACTER_PART;
	m_pTexRef = NULL;
	m_pTexOverlapRef = NULL;
	m_pSkinsRef = NULL;

	m_bVisible = true;
	m_dwReserved = 0;
	m_Mtl.Init();
	m_MtlOrg.Init();
};

CN3CPart::~CN3CPart()
{
	s_MngTex.Delete(&m_pTexRef);
	s_MngTex.Delete(&m_pTexOverlapRef);
	s_MngSkins.Delete(&m_pSkinsRef);
};

void CN3CPart::Release()
{
	m_bVisible = true;
	m_dwReserved = 0;
	m_Mtl.Init();
	m_MtlOrg.Init();

	s_MngTex.Delete(&m_pTexRef);
	s_MngTex.Delete(&m_pTexOverlapRef);
	m_pTexRef = NULL;
	m_pTexOverlapRef = NULL;
	s_MngSkins.Delete(&m_pSkinsRef);

	CN3BaseFileAccess::Release();
}

CN3CPartSkins* CN3CPart::SkinsSet(const std::string& szFN)
{
	s_MngSkins.Delete(&m_pSkinsRef);
	m_pSkinsRef = s_MngSkins.Get(szFN, TRUE);

	return m_pSkinsRef;
}

bool CN3CPart::Load(HANDLE hFile)
{
	CLogWriter::Write("CN3CPart::Load begin");
	CN3BaseFileAccess::Load(hFile);
	CLogWriter::Write("CN3CPart::Load after base load, name=%s", m_szName.c_str());

	DWORD dwRWC = 0;
	int nL = 0;
	char szFN[256] = "";

	ReadFile(hFile, &m_dwReserved, 4, &dwRWC, NULL);
	ReadFile(hFile, &m_MtlOrg, sizeof(__Material), &dwRWC, NULL);
	m_Mtl = m_MtlOrg;
	CLogWriter::Write("CN3CPart::Load after material");

	ReadFile(hFile, &nL, 4, &dwRWC, NULL);
	CLogWriter::Write("CN3CPart::Load texture filename length=%d", nL);
	if(nL > 0 && nL < 256) 
	{
		ReadFile(hFile, szFN, nL, &dwRWC, NULL); szFN[nL] = NULL;
		CLogWriter::Write("CN3CPart::Load texture=%s", szFN);
		// Skip loading problematic dxt files (only reye.dxt is compatible with modern drivers)
		try {
			this->TexSet(szFN);
		} catch (...) {
			CLogWriter::Write("CN3CPart::Load texture exception (load failed) for %s", szFN);
		}
	}

	ReadFile(hFile, &nL, 4, &dwRWC, NULL);
	CLogWriter::Write("CN3CPart::Load skins filename length=%d", nL);
	if(nL > 0 && nL < 256) 
	{
		ReadFile(hFile, szFN, nL, &dwRWC, NULL); szFN[nL] = NULL;
		CLogWriter::Write("CN3CPart::Load skins=%s", szFN);
		try {
			s_MngSkins.Delete(&m_pSkinsRef);
			m_pSkinsRef = s_MngSkins.Get(szFN);
			if(m_pSkinsRef) {
				CLogWriter::Write("CN3CPart::Load skins loaded successfully");
			} else {
				CLogWriter::Write("CN3CPart::Load skins load FAILED");
			}
		} catch (...) {
			CLogWriter::Write("CN3CPart::Load skins exception caught - skipping this part's skins");
			m_pSkinsRef = NULL;
		}
	}
	CLogWriter::Write("CN3CPart::Load end");

	return true;
}

#ifdef _N3TOOL
bool CN3CPart::Save(HANDLE hFile)
{
	CN3BaseFileAccess::Save(hFile);

	DWORD dwRWC = 0;
	int nL = 0;

	WriteFile(hFile, &m_dwReserved, 4, &dwRWC, NULL);
	WriteFile(hFile, &m_Mtl, sizeof(__Material), &dwRWC, NULL);
	
	if(m_pTexRef) nL = m_pTexRef->FileName().size();
	else nL = 0;
	WriteFile(hFile, &nL, 4, &dwRWC, NULL);
	if(nL > 0) WriteFile(hFile, m_pTexRef->FileName().c_str(), nL, &dwRWC, NULL);

	if(m_pSkinsRef) nL = m_pSkinsRef->FileName().size();
	else nL = 0;
	WriteFile(hFile, &nL, 4, &dwRWC, NULL);
	if(nL > 0) WriteFile(hFile, m_pSkinsRef->FileName().c_str(), nL, &dwRWC, NULL);

	return true;
}
#endif // end of _N3TOOL

void CN3CPart::Render(int nLOD)
{
	if(false == m_bVisible) return;
	if(nLOD < 0 || nLOD >= MAX_CHR_LOD) return;
	if(NULL == m_pSkinsRef) return;
	if(m_pSkinsRef->m_Skins[nLOD].VertexCount() <= 0) return;

#ifdef _DEBUG
	CN3Base::s_RenderInfo.nChr_Part++; // Rendering Information Update...
	CN3Base::s_RenderInfo.nChr_Polygon += m_pSkinsRef->m_Skins[nLOD].FaceCount();
#endif

	static DWORD dwAlpha, dwFog, dwCull;
	if(m_Mtl.nRenderFlags & RF_ALPHABLENDING) // Alpha ���?
	{
		s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlpha);
		if(TRUE != dwAlpha) s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND,   m_Mtl.dwSrcBlend);
		s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND,  m_Mtl.dwDestBlend);
	}
	if(m_Mtl.nRenderFlags & RF_NOTUSEFOG) // Fog ����..
	{
		s_lpD3DDev->GetRenderState(D3DRS_FOGENABLE, &dwFog);
		if(TRUE == dwFog) s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
	if(m_Mtl.nRenderFlags & RF_DOUBLESIDED) // Render Flags - 
	{
		s_lpD3DDev->GetRenderState(D3DRS_CULLMODE, &dwCull);
		s_lpD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	}

	s_lpD3DDev->SetMaterial(&m_Mtl);
	LPDIRECT3DTEXTURE9 lpTex = NULL;
	bool bUseTwoUV = false;
	if(m_pTexRef) lpTex = m_pTexRef->Get();
	if(lpTex)
	{
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		s_lpD3DDev->SetTexture(0, lpTex);
		
		m_pTexRef->UpdateRenderInfo();

		if(m_pTexOverlapRef && m_pTexOverlapRef->Get())
		{
			bUseTwoUV = true;
			s_lpD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
			s_lpD3DDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
			s_lpD3DDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
			s_lpD3DDev->SetTexture(1, m_pTexOverlapRef->Get());

			m_pTexOverlapRef->UpdateRenderInfo();
		}
	}
	else
	{
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		s_lpD3DDev->SetTexture(0, NULL);
	}

	m_pSkinsRef->m_Skins[nLOD].Render(bUseTwoUV);

	if(bUseTwoUV)
	{
		s_lpD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		s_lpD3DDev->SetTexture(1, NULL);
	}

	if((m_Mtl.nRenderFlags & RF_ALPHABLENDING) && FALSE == dwAlpha)		s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	if((m_Mtl.nRenderFlags & RF_NOTUSEFOG) && TRUE == dwFog) 			s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, TRUE); // �Ȱ� �������?�ʴ´�..
	if((m_Mtl.nRenderFlags & RF_DOUBLESIDED) && D3DCULL_NONE != dwCull) s_lpD3DDev->SetRenderState(D3DRS_CULLMODE, dwCull);
}

CN3Texture* CN3CPart::TexOverlapSet(const std::string& szFN)
{
	if(	m_pTexOverlapRef && 0 == lstrcmpi(m_pTexOverlapRef->FileName().c_str(), szFN.c_str()))
		return m_pTexOverlapRef;

	s_MngTex.Delete(&m_pTexOverlapRef);
	m_pTexOverlapRef = s_MngTex.Get(szFN, true, s_Options.iTexLOD_Chr);
	return m_pTexOverlapRef;
}

void CN3CPart::TexOverlapSet(CN3Texture* pTex)
{
	if(pTex == m_pTexOverlapRef) return;
	
	s_MngTex.Delete(&m_pTexOverlapRef);
	m_pTexOverlapRef = pTex;
}


#ifdef _N3TOOL
void CN3CPart::RenderSelected(int nLOD)
{
	if(nLOD < 0 || nLOD > MAX_CHR_LOD) return;
	if(NULL == m_pSkinsRef) return;

	m_pSkinsRef->m_Skins[nLOD].RenderSelected();
}
#endif // end of _N3TOOL

CN3CPlugBase::CN3CPlugBase()
{
	m_dwType |= OBJ_CHARACTER_PLUG;

	m_ePlugType = PLUGTYPE_NORMAL;
	m_nJointIndex = 0;

	m_pTexRef = NULL;
	m_pTexOverlapRef = NULL;

	m_bVisible = true;
	m_Mtl.Init();
	m_vPosition.Zero();
	m_MtxRot.Identity();
	m_vScale.Set(1,1,1);
	m_Matrix.Identity();	
};

CN3CPlugBase::~CN3CPlugBase()
{
	s_MngTex.Delete(&m_pTexRef);
	s_MngTex.Delete(&m_pTexOverlapRef);
}

void CN3CPlugBase::Release()
{
	m_ePlugType = PLUGTYPE_NORMAL;
	m_nJointIndex = 0;
	m_PMeshInst.Release();

	s_MngTex.Delete(&m_pTexRef);
	s_MngTex.Delete(&m_pTexOverlapRef);

	m_Mtl.Init();
	m_vPosition.Zero();
	m_MtxRot.Identity();
	m_vScale.Set(1,1,1);
	m_Matrix.Identity();	

	CN3BaseFileAccess::Release();
}

void CN3CPlugBase::ReCalcMatrix()
{
	m_Matrix.Scale(m_vScale);
	m_Matrix *= m_MtxRot;
	m_Matrix.PosSet(m_vPosition * m_vScale);

//	m_Matrix *= m_MtxRot;
//	m_Matrix.PosSet(m_vPosition);
//
//	__Matrix44 mtxScale; // �Ϻη� ������ ���� ���߿� �����ϵ��� �ٲپ���...
//	mtxScale.Scale(m_vScale);
//	m_Matrix *= mtxScale;
}

void CN3CPlugBase::PMeshSet(const std::string& szFN)
{
	m_PMeshInst.Create(szFN);
}

void CN3CPlugBase::Render(const __Matrix44& mtxParent, const __Matrix44& mtxJoint)
{
	if(false == m_bVisible) return;
	if(m_PMeshInst.GetNumVertices() <= 0) return;
	
#ifdef _DEBUG
	CN3Base::s_RenderInfo.nChr_Plug++; // Rendering Information Update...
	CN3Base::s_RenderInfo.nChr_Plug_Polygon += m_PMeshInst.GetNumIndices() / 3; // Rendering Information Update...
#endif

	DWORD dwAlpha = 0, dwFog = 0, dwCull = 0;
	if(m_Mtl.nRenderFlags & RF_ALPHABLENDING) // Alpha ���?
	{
		s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlpha);
		if(TRUE != dwAlpha) s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND,   m_Mtl.dwSrcBlend);
		s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND,  m_Mtl.dwDestBlend);
	}
	if(m_Mtl.nRenderFlags & RF_NOTUSEFOG) // Fog ����..
	{
		s_lpD3DDev->GetRenderState(D3DRS_FOGENABLE, &dwFog);
		if(TRUE == dwFog) s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
	if(m_Mtl.nRenderFlags & RF_DOUBLESIDED) // Render Flags - 
	{
		s_lpD3DDev->GetRenderState(D3DRS_CULLMODE, &dwCull);
		s_lpD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	}

	static __Matrix44 mtx;
	mtx = m_Matrix;
	mtx *= mtxJoint;
	mtx *= mtxParent;
	s_lpD3DDev->SetTransform(D3DTS_WORLD, &mtx);

	s_lpD3DDev->SetMaterial(&m_Mtl);
	LPDIRECT3DTEXTURE9 lpTex = NULL;
	bool bUseTwoUV = false;
	if(m_pTexRef) lpTex = m_pTexRef->Get();
	if(lpTex)
	{
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		s_lpD3DDev->SetTexture(0, lpTex);
		m_pTexRef->UpdateRenderInfo();

		if(m_pTexOverlapRef && m_pTexOverlapRef->Get())
		{
			bUseTwoUV = true;

			s_lpD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
			s_lpD3DDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			s_lpD3DDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
			s_lpD3DDev->SetTexture(1, m_pTexOverlapRef->Get());
			m_pTexOverlapRef->UpdateRenderInfo();
		}
	}
	else
	{
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		s_lpD3DDev->SetTexture(0, NULL);
	}

	if((m_Mtl.nRenderFlags & RF_ALPHABLENDING) && FALSE == dwAlpha)	s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	if((m_Mtl.nRenderFlags & RF_NOTUSEFOG) && TRUE == dwFog) 		s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, TRUE); // �Ȱ� �������?�ʴ´�..
	if((m_Mtl.nRenderFlags & RF_DOUBLESIDED) && D3DCULL_NONE != dwCull) 		s_lpD3DDev->SetRenderState(D3DRS_CULLMODE, dwCull);

	if(bUseTwoUV) m_PMeshInst.RenderTwoUV();
	else m_PMeshInst.Render();

	if(bUseTwoUV) // �ؽ�ó �������� �ΰ��� �������Ѵ�...!!
	{
		s_lpD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		s_lpD3DDev->SetTexture(1, NULL);
	}
}

CN3Texture* CN3CPlugBase::TexOverlapSet(const std::string& szFN)
{
//	if(m_pTexOverlapRef && m_pTexOverlapRef->FileName() == szFN) return m_pTexOverlapRef;
	if(	m_pTexOverlapRef && 0 == lstrcmpi(m_pTexOverlapRef->FileName().c_str(), szFN.c_str()))
		return m_pTexOverlapRef;

	s_MngTex.Delete(&m_pTexOverlapRef);
	m_pTexOverlapRef = s_MngTex.Get(szFN, true, s_Options.iTexLOD_Chr);
	return m_pTexOverlapRef;
}

void CN3CPlugBase::TexOverlapSet(CN3Texture* pTex)
{
	if(pTex == m_pTexOverlapRef) return;
	
	s_MngTex.Delete(&m_pTexOverlapRef);
	m_pTexOverlapRef = pTex;
}

bool CN3CPlugBase::Load(HANDLE hFile)
{
	CN3BaseFileAccess::Load(hFile);

	DWORD dwRWC = 0;
	int nL = 0;
	char szFN[512] = "";

	ReadFile(hFile, &m_ePlugType, 4, &dwRWC, NULL); // Plug Type
//#ifdef _N3TOOL
	if (m_ePlugType > PLUGTYPE_MAX)
	{
		m_ePlugType = PLUGTYPE_NORMAL;
	}
//#endif
	ReadFile(hFile, &m_nJointIndex, 4, &dwRWC, NULL); // Plug Joint Index

	ReadFile(hFile, &m_vPosition, sizeof(m_vPosition), &dwRWC, NULL);
	ReadFile(hFile, &m_MtxRot, sizeof(m_MtxRot), &dwRWC, NULL);
	ReadFile(hFile, &m_vScale, sizeof(m_vScale), &dwRWC, NULL);

	ReadFile(hFile, &m_Mtl, sizeof(__Material), &dwRWC, NULL); // ����

	ReadFile(hFile, &nL, 4, &dwRWC, NULL);
	if(nL > 0)
	{
		ReadFile(hFile, szFN, nL, &dwRWC, NULL); szFN[nL] = NULL;
		this->PMeshSet(szFN);
	}

	ReadFile(hFile, &nL, 4, &dwRWC, NULL);
	if(nL > 0)
	{
		ReadFile(hFile, szFN, nL, &dwRWC, NULL); szFN[nL] = NULL;
		this->TexSet(szFN);
	}	

	this->ReCalcMatrix(); // ���?���?..

	return 0;
}

#ifdef _N3TOOL
bool CN3CPlugBase::Save(HANDLE hFile)
{
	CN3BaseFileAccess::Save(hFile);

	DWORD dwRWC = 0;
	int nL = 0;

	WriteFile(hFile, &m_ePlugType, 4, &dwRWC, NULL); // Plug Type
	WriteFile(hFile, &m_nJointIndex, 4, &dwRWC, NULL); // Plug Joint Index

	WriteFile(hFile, &m_vPosition, sizeof(m_vPosition), &dwRWC, NULL);
	WriteFile(hFile, &m_MtxRot, sizeof(m_MtxRot), &dwRWC, NULL);
	WriteFile(hFile, &m_vScale, sizeof(m_vScale), &dwRWC, NULL);

	WriteFile(hFile, &m_Mtl, sizeof(__Material), &dwRWC, NULL); // ����

	nL = 0;
	CN3PMesh* pPMesh = m_PMeshInst.GetMesh();
	if(pPMesh) nL = pPMesh->FileName().size();
	WriteFile(hFile, &nL, 4, &dwRWC, NULL);
	if(nL > 0) WriteFile(hFile, pPMesh->FileName().c_str(), nL, &dwRWC, NULL);

	nL = 0;
	if(m_pTexRef) nL = m_pTexRef->FileName().size();
	WriteFile(hFile, &nL, 4, &dwRWC, NULL);
	if(nL > 0) WriteFile(hFile, m_pTexRef->FileName().c_str(), nL, &dwRWC, NULL);

	return 0;
}
#endif // end of _N3TOOL

e_PlugType CN3CPlugBase::GetPlugTypeByFileName(const std::string& szFN)
{
	if(szFN.empty()) return PLUGTYPE_UNDEFINED;
	// berserk
	// �ϴ� Ȯ���ڷ� �����Ѵ�. ���� ���� ���?������ ������.. N3CPlug, N3CPlug_Cloak
	int nL = szFN.size();
	if (szFN[nL-2] == 'u' && szFN[nL-1] == 'g')
	{	// PLUGTYPE_NORMAL
		return PLUGTYPE_NORMAL;
	}
	else if (szFN[nL-2] == 'a' && szFN[nL-1] == 'k')
	{
		return PLUGTYPE_CLOAK;
	}
	else
	{
		return PLUGTYPE_UNDEFINED;
	}
}


CN3CPlug::CN3CPlug()
{
	m_ePlugType = PLUGTYPE_NORMAL;

	m_bRenderTrace = false;
	m_nTraceStep = 0; // ���� �Ҹ� �ð�..
	m_crTrace = 0xfffffff; // ���� ����.. �������̸� ����..
	m_fTrace0 = 0; // ���� ��ġ..
	m_fTrace1 = 0; // ���� ��ġ..

	m_pFXMainBundle = NULL;
	m_pFXPart = NULL;

	for(int i=0;i<MAX_FXTAIL;i++) m_pFXTailBundle[i] = NULL;

//	m_strFXName[PLUGATTR_FIRE] = "fx//fire_sword0_1.fxb";
//	m_strFXName[PLUGATTR_ICE] = "fx//ice_sword0_1.fxb";
//	m_strFXName[PLUGATTR_LIGHTENNING] = "fx//poison_sword0_1.fxb";
//	m_strFXName[PLUGATTR_POISON] = "fx//fire_sword0_1.fxb";
}

CN3CPlug::~CN3CPlug()
{
	if(m_pFXMainBundle) { delete m_pFXMainBundle; m_pFXMainBundle = NULL; }

	for(int i=0;i<MAX_FXTAIL;i++)
	{
		if(m_pFXTailBundle[i]) { delete m_pFXTailBundle[i]; m_pFXTailBundle[i] = NULL; }
	}
}

void CN3CPlug::Release()
{
	CN3CPlugBase::Release();

	m_bRenderTrace = false;
	m_nTraceStep = 0; // ���� �Ҹ� �ð�..
	m_crTrace = 0xffffffff; // ���� ����.. �������̸� ����..
	m_fTrace0 = 0; // ���� ��ġ..
	m_fTrace1 = 0; // ���� ��ġ..

	m_PMeshInstFX.Release(); // FX �� �� PMesh Instance

	if(m_pFXMainBundle) { delete m_pFXMainBundle; m_pFXMainBundle = NULL; }

	for(int i=0;i<MAX_FXTAIL;i++)
	{
		if(m_pFXTailBundle[i]) { delete m_pFXTailBundle[i]; m_pFXTailBundle[i] = NULL; }
	}
}

bool CN3CPlug::Load(HANDLE hFile)
{
	CN3CPlugBase::Load(hFile);
	DWORD dwRWC = 0;

	ReadFile(hFile, &m_nTraceStep, 4, &dwRWC, NULL); // ���� ����..
	if(m_nTraceStep > 0)
	{
		ReadFile(hFile, &m_crTrace, 4, &dwRWC, NULL); // ���� ����.. �������̸� ����..
		ReadFile(hFile, &m_fTrace0, 4, &dwRWC, NULL); // ���� ��ġ..
		ReadFile(hFile, &m_fTrace1, 4, &dwRWC, NULL); // ���� ��ġ..
	}
	else m_nTraceStep = 0;

	int iUseVMesh = 0;
	ReadFile(hFile, &iUseVMesh, 4, &dwRWC, NULL); // �޽ø� ���°�??
	if(iUseVMesh)
	{
		CN3PMesh* pPMesh = new CN3PMesh();
		pPMesh->Load(hFile);
		static int iSN = 0;
		char szFNTmp[256]; sprintf(szFNTmp, "Temp_Plug_%d.N3PMesh", iSN++);
		pPMesh->FileNameSet(szFNTmp);
		s_MngPMesh.Add(pPMesh);
		m_PMeshInstFX.Create(pPMesh); // FX �� �� PMesh Instance
	}

	m_strFXMainName = "";
	m_strFXTailName = "";
	InitFX(m_strFXMainName, m_strFXTailName, 0xffffffff);
	
	return true;
}

void CN3CPlug::InitFX(std::string& szFXMain, std::string& szFXTail, D3DCOLOR TraceCR)
{
	if(szFXMain.empty())
	{
		if(m_pFXMainBundle) delete m_pFXMainBundle;
		m_pFXMainBundle = NULL;
		m_strFXMainName = szFXMain;
	}
	else if(!m_pFXMainBundle || szFXMain!=m_pFXMainBundle->FileName())
	{
		m_strFXMainName = szFXMain;
		if(m_pFXMainBundle) delete m_pFXMainBundle;
		m_pFXMainBundle = new CN3FXBundle;
		if(!m_pFXMainBundle->LoadFromFile(m_strFXMainName.c_str()))
		{
			delete m_pFXMainBundle;
			m_pFXMainBundle = NULL;
			m_strFXMainName = "";

			m_crTrace = 0xffffffff;
		}
		else
		{
			m_pFXMainBundle->Trigger(0, -1, -1);
			m_pFXPart = (CN3FXPartBillBoard*)m_pFXMainBundle->GetPart(0);
			m_pFXPart->m_fCurrLife = (float)(rand() % 1000) / 100.0f;

			m_crTrace = TraceCR;
		}
	}

	if(szFXTail.empty())
	{
		m_strFXTailName = szFXTail;
		for(int i=0;i<MAX_FXTAIL;i++)
		{
			if(m_pFXTailBundle[i]) delete m_pFXTailBundle[i];
			m_pFXTailBundle[i] = NULL;
		}
	}
	else if(!m_pFXTailBundle[0] || szFXTail!=m_pFXTailBundle[0]->FileName())
	{
		m_strFXTailName = szFXTail;
		if(m_pFXTailBundle[0]) delete m_pFXTailBundle[0];
		m_pFXTailBundle[0] = new CN3FXBundle;
		if(!m_pFXTailBundle[0]->LoadFromFile(m_strFXTailName.c_str()))
		{
			delete m_pFXTailBundle[0];
			m_pFXTailBundle[0] = NULL;
			m_strFXTailName = "";
			return;			
		}
		else
		{
			CN3PMesh* pMesh = m_PMeshInstFX.GetMesh();
			__Vector3 vInterval(1.0f,1.0f,1.0f);
			if(pMesh) vInterval = m_PMeshInstFX.GetMesh()->Max() - m_PMeshInstFX.GetMesh()->Min();
			m_pFXTailBundle[0]->Trigger(0, -1, -1);
			for(int i=1;i<MAX_FXTAIL;i++)
			{
				if(m_pFXTailBundle[i]) delete m_pFXTailBundle[i];
				m_pFXTailBundle[i] = new CN3FXBundle;
				m_pFXTailBundle[0]->Duplicate(m_pFXTailBundle[i]);
				m_pFXTailBundle[i]->Trigger(0, -1, -1);
				m_pFXTailBundle[i]->m_fTargetScale = (vInterval.z + vInterval.y) * 0.7f;
			}
		}
	}
}

#ifdef _N3TOOL
bool CN3CPlug::Save(HANDLE hFile)
{
	CN3CPlugBase::Save(hFile);
	DWORD dwRWC = 0;

	WriteFile(hFile, &m_nTraceStep, 4, &dwRWC, NULL); // ���� ����..
	if(m_nTraceStep > 0 && m_nTraceStep <= MAX_PLUG_TRACE_VERTEX/2 - 1)
	{
		WriteFile(hFile, &m_crTrace, 4, &dwRWC, NULL); // ���� ����.. �������̸� ����..
		WriteFile(hFile, &m_fTrace0, 4, &dwRWC, NULL); // ���� ��ġ..
		WriteFile(hFile, &m_fTrace1, 4, &dwRWC, NULL); // ���� ��ġ..
	}
	else m_nTraceStep = 0;
	
	int iUseVMesh = (m_PMeshInstFX.GetMesh()) ? true : false; // �޽�.. FX�� ����..
	WriteFile(hFile, &iUseVMesh, 4, &dwRWC, NULL); // ��ġ ���� �޽ø� ���°�??
	if(iUseVMesh) m_PMeshInstFX.GetMesh()->Save(hFile);

	return 0;
}
#endif

#ifdef _N3TOOL
void CN3CPlug::ImportPMesh(const std::string& szFileName)
{
	if(szFileName.empty()) return;
	m_PMeshInstFX.Release();
	m_PMeshInstFX.Create(szFileName);
}
#endif

#ifdef _N3TOOL
void CN3CPlug::RenderFXLines(const __Matrix44& mtxParent, const __Matrix44& mtxJoint) // FX ���?���� ���� �׷��ش�.
{
	static __Matrix44 mtx;
	mtx = m_Matrix;
	mtx *= mtxJoint;
	mtx *= mtxParent;
	s_lpD3DDev->SetTransform(D3DTS_WORLD, &mtx);

	DWORD dwCull;
	s_lpD3DDev->GetRenderState(D3DRS_CULLMODE, &dwCull);
	s_lpD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_PMeshInstFX.SetLOD((s_CameraData.vEye - mtx.Pos()).Magnitude() * s_CameraData.fFOV);
	m_PMeshInstFX.Render();
	s_lpD3DDev->SetRenderState(D3DRS_CULLMODE, dwCull);
}
#endif

void CN3CPlug::Render(const __Matrix44& mtxParent, const __Matrix44& mtxJoint)
{
	if(false == m_bVisible) return;

	CN3CPlugBase::Render(mtxParent, mtxJoint);

	//fx render....
	RenderFX(mtxParent, mtxJoint);
}

void CN3CPlug::RenderFX(const __Matrix44& mtxParent, const __Matrix44& mtxJoint)
{
	if(!m_pFXMainBundle || !m_pFXPart) return;
	if(!m_PMeshInstFX.GetVertices()) return;
	__VertexT1* pvAxis = m_PMeshInstFX.GetVertices();
	//if(m_pFXMainBundle->FileName() != m_strFXMainName) InitFX(m_strFXMainName, m_strFXTailName);

	m_pFXMainBundle->Tick();

	static __Matrix44 mtx;
	//mtx = m_Matrix;
	//mtx *= mtxJoint;
	//mtx *= mtxParent;

	//tail...
	/////////////////////////////////////////////////////////////////////
	//
	__Vector3 vMax = m_PMeshInstFX.GetMesh()->Max();
	__Vector3 vMin = m_PMeshInstFX.GetMesh()->Min();
	__Vector3 vInterval = vMax - vMin;
	__Vector3 vTmp;
	float fHeight = vMax.y - vMin.y;
	if(m_pFXTailBundle[0])
	{
		mtx.Identity();
		mtx *= m_Matrix;
		mtx *= mtxJoint;
		mtx *= mtxParent;

		for(int i=1;i<MAX_FXTAIL;i++)
		{
			if(m_pFXTailBundle[i])
			{
				vTmp.Set( vMin.x + (vInterval.x * 0.25f) + (vInterval.x * (float)(rand()%50) / 100.0f),
					vMin.y + (vInterval.y * 0.25f) + (vInterval.y * (float)(rand()%50) / 100.0f),
					vMin.z + (vInterval.z * 0.25f) + (vInterval.z * (float)(rand()%50) / 100.0f));
				
				m_pFXTailBundle[i]->m_vPos = vTmp * mtx;
				m_pFXTailBundle[i]->Tick();
				m_pFXTailBundle[i]->Render();
			}
		}
	}
	//
	/////////////////////////////////////////////////////////////////////

	//main...
	/////////////////////////////////////////////////////////////////////
	///
	if(m_pFXPart->m_dwRenderFlag & RF_ALPHABLENDING) // Alpha ���?
	{
		int iTexIdx = m_pFXPart->m_iTexIdx;
		mtx = m_Matrix;
		mtx *= mtxJoint;
		mtx *= mtxParent;

		float fCameraDist = (s_CameraData.vEye - mtx.Pos()).Magnitude();

		m_PMeshInstFX.SetLOD(fCameraDist*s_CameraData.fFOV);
		
		__AlphaPrimitive* pAP = s_AlphaMgr.Add();
		if(pAP)
		{
			pAP->bUseVB				= FALSE;
			pAP->dwBlendDest		= m_pFXPart->m_dwDestBlend;
			pAP->dwBlendSrc			= m_pFXPart->m_dwSrcBlend;
			pAP->dwFVF				= FVF_VNT1;
			pAP->dwPrimitiveSize	= sizeof(__VertexT1);
			pAP->fCameraDistance	= fCameraDist + fHeight;

			pAP->lpTex				= NULL;
			if(m_pFXPart->m_ppRefTex[iTexIdx])
				pAP->lpTex = m_pFXPart->m_ppRefTex[iTexIdx]->Get();
			
			pAP->ePrimitiveType		= D3DPT_TRIANGLELIST;
			pAP->nPrimitiveCount	= m_PMeshInstFX.GetNumIndices() / 3;
			pAP->nRenderFlags		= m_pFXPart->m_dwRenderFlag;
			pAP->nVertexCount		= m_PMeshInstFX.GetNumVertices();
			pAP->MtxWorld			= mtx;
			pAP->pVertices			= m_PMeshInstFX.GetVertices();
			pAP->pwIndices			= m_PMeshInstFX.GetIndices();
		}

		float fArg1 = m_pFXMainBundle->m_fLife * 1.2f;
		float fArg2 = (0.07f * (fArg1 - (int)fArg1)) - 0.035f;
		__Vector3 vArg2 = pvAxis[0].n * fArg2;

		mtx.Identity();
		mtx.RotationY(-__PI / 36.0f);
		mtx += vArg2;
		mtx *= m_Matrix;
		mtx *= mtxJoint;
		mtx *= mtxParent;

		pAP = s_AlphaMgr.Add();
		if(pAP)
		{
			pAP->bUseVB				= FALSE;
			pAP->dwBlendDest		= m_pFXPart->m_dwDestBlend;
			pAP->dwBlendSrc			= m_pFXPart->m_dwSrcBlend;
			pAP->dwFVF				= FVF_VNT1;
			pAP->dwPrimitiveSize	= sizeof(__VertexT1);
			pAP->fCameraDistance	= (s_CameraData.vEye - mtx.Pos()).Magnitude() + fHeight;

			pAP->lpTex				= NULL;
			iTexIdx = (iTexIdx+1) % m_pFXPart->m_iNumTex;
			if(m_pFXPart->m_ppRefTex[iTexIdx])
				pAP->lpTex = m_pFXPart->m_ppRefTex[iTexIdx]->Get();
			
			pAP->ePrimitiveType		= D3DPT_TRIANGLELIST;
			pAP->nPrimitiveCount	= m_PMeshInstFX.GetNumIndices() / 3;
			pAP->nRenderFlags		= m_pFXPart->m_dwRenderFlag;
			pAP->nVertexCount		= m_PMeshInstFX.GetNumVertices();
			pAP->MtxWorld			= mtx;
			pAP->pVertices			= m_PMeshInstFX.GetVertices();
			pAP->pwIndices			= m_PMeshInstFX.GetIndices();
		}

		mtx.Identity();
		mtx.RotationY(__PI / 36.0f);
		mtx -= vArg2;
		mtx *= m_Matrix;
		mtx *= mtxJoint;
		mtx *= mtxParent;

		pAP = s_AlphaMgr.Add();
		if(pAP)
		{
			pAP->bUseVB			= FALSE;
			pAP->dwBlendDest		= m_pFXPart->m_dwDestBlend;
			pAP->dwBlendSrc		= m_pFXPart->m_dwSrcBlend;
			pAP->dwFVF				= FVF_VNT1;
			pAP->dwPrimitiveSize	= sizeof(__VertexT1);
			pAP->fCameraDistance	= (s_CameraData.vEye - mtx.Pos()).Magnitude() + fHeight;

			pAP->lpTex				= NULL;
			iTexIdx = (iTexIdx+1) % m_pFXPart->m_iNumTex;
			if(m_pFXPart->m_ppRefTex[iTexIdx])
				pAP->lpTex = m_pFXPart->m_ppRefTex[iTexIdx]->Get();
			
			pAP->ePrimitiveType	= D3DPT_TRIANGLELIST;
			pAP->nPrimitiveCount	= m_PMeshInstFX.GetNumIndices() / 3;
			pAP->nRenderFlags		= m_pFXPart->m_dwRenderFlag;
			pAP->nVertexCount		= m_PMeshInstFX.GetNumVertices();
			pAP->MtxWorld			= mtx;
			pAP->pVertices			= m_PMeshInstFX.GetVertices();
			pAP->pwIndices			= m_PMeshInstFX.GetIndices();
		}
		/*
		fArg1 = m_pFXMainBundle->m_fLife * 1.2f;
		fArg2 = (0.07f * (fArg1 - (int)fArg1)) - 0.035f;
		vArg2 = pvAxis[0].n * fArg2;

		mtx.Identity();
		mtx.RotationY(-__PI / 72.0f);
		mtx.Scale(0.5f, 0.5f, 0.5f);
		mtx.PosSet(0.0f, m_PMeshInstFX.GetMesh()->Max().y * 0.6f, 0.0f);
		mtx += vArg2;
		mtx *= m_Matrix;
		mtx *= mtxJoint;
		mtx *= mtxParent;

		pAP = s_AlphaMgr.Add();
		if(pAP)
		{
			pAP->bUseVB			= FALSE;
			pAP->dwBlendDest		= m_pFXPart->m_dwDestBlend;
			pAP->dwBlendSrc		= m_pFXPart->m_dwSrcBlend;
			pAP->dwFVF				= FVF_VNT1;
			pAP->dwPrimitiveSize	= sizeof(__VertexT1);
			pAP->fCameraDistance	= (s_CameraData.vEye - mtx.Pos()).Magnitude() + fHeight;

			pAP->lpTex				= NULL;
			iTexIdx = (iTexIdx+1) % m_pFXPart->m_iNumTex;
			if(m_pFXPart->m_ppRefTex[iTexIdx])
				pAP->lpTex = m_pFXPart->m_ppRefTex[iTexIdx]->Get();
			
			pAP->ePrimitiveType	= D3DPT_TRIANGLELIST;
			pAP->nPrimitiveCount	= m_PMeshInstFX.GetNumIndices() / 3;
			pAP->nRenderFlags		= m_pFXPart->m_dwRenderFlag;
			pAP->nVertexCount		= m_PMeshInstFX.GetNumVertices();
			pAP->MtxWorld			= mtx;
			pAP->pVertices			= m_PMeshInstFX.GetVertices();
			pAP->pwIndices			= m_PMeshInstFX.GetIndices();
		}

		mtx.Identity();
		mtx.RotationY(__PI / 72.0f);
		mtx.Scale(0.5f, 0.5f, 0.5f);
		mtx.PosSet(0.0f, m_PMeshInstFX.GetMesh()->Max().y * 0.5f, 0.0f);
		mtx -= vArg2;
		mtx *= m_Matrix;
		mtx *= mtxJoint;
		mtx *= mtxParent;

		pAP = s_AlphaMgr.Add();
		if(pAP)
		{
			pAP->bUseVB			= FALSE;
			pAP->dwBlendDest		= m_pFXPart->m_dwDestBlend;
			pAP->dwBlendSrc		= m_pFXPart->m_dwSrcBlend;
			pAP->dwFVF				= FVF_VNT1;
			pAP->dwPrimitiveSize	= sizeof(__VertexT1);
			pAP->fCameraDistance	= (s_CameraData.vEye - mtx.Pos()).Magnitude() + fHeight;

			pAP->lpTex				= NULL;
			iTexIdx = (iTexIdx+1) % m_pFXPart->m_iNumTex;
			if(m_pFXPart->m_ppRefTex[iTexIdx])
				pAP->lpTex = m_pFXPart->m_ppRefTex[iTexIdx]->Get();
			
			pAP->ePrimitiveType	= D3DPT_TRIANGLELIST;
			pAP->nPrimitiveCount	= m_PMeshInstFX.GetNumIndices() / 3;
			pAP->nRenderFlags		= m_pFXPart->m_dwRenderFlag;
			pAP->nVertexCount		= m_PMeshInstFX.GetNumVertices();
			pAP->MtxWorld			= mtx;
			pAP->pVertices			= m_PMeshInstFX.GetVertices();
			pAP->pwIndices			= m_PMeshInstFX.GetIndices();
		}
		*/
	}
	///
	////////////////////////////////////////////////////////////////////////
	return;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// CN3cPlug_Cloak Codes Start here
CN3CPlug_Cloak::CN3CPlug_Cloak()
{
	CN3CPlugBase::CN3CPlugBase();
}

CN3CPlug_Cloak::~CN3CPlug_Cloak()
{
	Release();
}

void CN3CPlug_Cloak::Release()
{
	CN3CPlugBase::Release();
}

bool CN3CPlug_Cloak::Load(HANDLE hFile)
{
	CN3CPlugBase::Load(hFile);
#ifdef _N3GAME
	m_Cloak.Init(this);
#endif
	return 0;
}

#ifdef _N3TOOL
bool CN3CPlug_Cloak::Save(HANDLE hFile)
{
	CN3CPlugBase::Save(hFile);
	return 0;
}
#endif
void CN3CPlug_Cloak::Render(const __Matrix44& mtxParent, const __Matrix44& mtxJoint)
{
#ifdef _N3TOOL
	CN3CPlugBase::Render(mtxParent, mtxJoint);
#endif
#ifdef _N3GAME
	static __Matrix44 mtx;
	mtx = m_Matrix;
	mtx *= mtxJoint;
	mtx *= mtxParent;
	m_Cloak.Render(mtx);
#endif
}

void CN3CPlug_Cloak::SetLOD(int nLOD)
{
#ifdef _N3GAME
	m_Cloak.SetLOD(nLOD);
#endif
}
// CN3cPlug_Cloak Codes End here
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// Part, Plug....
////////////////////////////////
int CN3Chr::s_iLODDelta = 0; // LOD ���?�ʿ��� �ε���..

CN3Chr::CN3Chr()
{
	m_dwType |= OBJ_CHARACTER;

	m_pRootJointRef = NULL;
	m_pRootJointRef = NULL;

	m_pAniCtrlRef = NULL;
	m_pMeshCollision = NULL;
	m_pFXPlug = NULL;

//	m_pSkinCollision = NULL;
	m_fAniSpeedDelta = 1.0f;			// ���ϸ��̼� �ӵ� ���� ���� 1 �̺���, �� ũ�� ��������..

	this->Release();
}

CN3Chr::~CN3Chr()
{
	s_MngJoint.Delete(&m_pRootJointRef);

	int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++) delete m_Parts[i];
	m_Parts.clear();

	iPC = m_Plugs.size();
	for(int i = 0; i < iPC; i++) delete m_Plugs[i];
	m_Plugs.clear();

	for(int i = 0; i < m_vTraces.size(); i++) delete m_vTraces[i];
	m_vTraces.clear();

	// Animation Control
	s_MngAniCtrl.Delete(&m_pAniCtrlRef);

	// �浹 �޽ô� ĳ���Ϳ����� �����?.
	delete m_pMeshCollision; m_pMeshCollision = NULL;

	delete m_pFXPlug;
}

void CN3Chr::Release()
{
	m_nLOD = 0;

	s_MngJoint.Delete(&m_pRootJointRef);

	m_JointRefs.clear();
	m_MtxJoints.clear();
	m_MtxInverses.clear();

	int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++) delete m_Parts[i];
	m_Parts.clear();

	iPC = m_Plugs.size();
	for(int i = 0; i < iPC; i++) delete m_Plugs[i];
	m_Plugs.clear();

	for(int i = 0; i < m_vTraces.size(); i++) delete m_vTraces[i];
	m_vTraces.clear();

//	s_MngSkin.Delete(m_pSkinCollision);
//	m_pSkinCollision = NULL;

	// Animation Control
	s_MngAniCtrl.Delete(&m_pAniCtrlRef);

	for(int i = 0; i < MAX_CHR_ANI_PART; i++)
	{
		m_nJointPartStarts[i] = -1; // ����Ʈ�� �Ϻκ��� ���� ���ϸ��̼� �Ǿ� �Ѵٸ�.. ����Ʈ �ε��� ���� ��ȣ
		m_nJointPartEnds[i] = -1; // ����Ʈ�� �Ϻκ��� ���� ���ϸ��̼� �Ǿ� �Ѵٸ�.. ����Ʈ �ε��� �� ��ȣ

		m_FrmCtrl.Init();
		m_FrmCtrlUpper.Init();
	}

	m_fAniSpeedDelta = 1.0f;			// ���ϸ��̼� �ӵ� ���� ���� 1 �̺���, �� ũ�� ��������..

	// �浹 �޽ô� ĳ���Ϳ����� �����?.
	delete m_pMeshCollision; m_pMeshCollision = NULL;

	if (m_pFXPlug) {delete m_pFXPlug; m_pFXPlug = NULL;}

	CN3TransformCollision::Release();
}

bool CN3Chr::Load(HANDLE hFile)
{
	CLogWriter::Write("N3Chr::Load begin");
	if(m_pRootJointRef != NULL) CN3Chr::Release();

	CN3TransformCollision::Load(hFile);
	CLogWriter::Write("N3Chr::Load after TransformCollision::Load");

	DWORD dwRWC = 0;
	
	int nL = 0;
	char szFN[512] = "";

	ReadFile(hFile, &nL, 4, &dwRWC, NULL);
	CLogWriter::Write("N3Chr::Load joint filename length=%d", nL);
	if(nL > 0 && nL < 512) {
		ReadFile(hFile, szFN, nL, &dwRWC, NULL); szFN[nL] = NULL;
		CLogWriter::Write("N3Chr::Load joint file=%s", szFN);
		this->JointSet(szFN); // ���� ����..
		CLogWriter::Write("N3Chr::Load after JointSet");
	} else {
		CLogWriter::Write("N3Chr::Load invalid joint filename length");
		return false;
	}

	// Part Allocation, Loading .. 
	int iPC = 0;
	ReadFile(hFile, &iPC, 4, &dwRWC, NULL);
	CLogWriter::Write("N3Chr::Load part count=%d", iPC);
	if(iPC < 0 || iPC > 100) {
		CLogWriter::Write("N3Chr::Load invalid part count");
		return false;
	}
	this->PartAlloc(iPC);
	for(int i = 0; i < iPC; i++)
	{
		nL = 0;
		ReadFile(hFile, &nL, 4, &dwRWC, NULL);
		if(nL > 0 && nL < 512)
		{
			ReadFile(hFile, szFN, nL, &dwRWC, NULL); szFN[nL] = NULL;
			CLogWriter::Write("N3Chr::Load part[%d]=%s", i, szFN);
			m_Parts[i]->LoadFromFile(szFN);
		}
	}
	CLogWriter::Write("N3Chr::Load all parts loaded");
	
	// Plug Allocation, Loading .. 
	m_Plugs.clear();
	ReadFile(hFile, &iPC, 4, &dwRWC, NULL);
	CLogWriter::Write("N3Chr::Load plug count=%d", iPC);
	if(iPC < 0 || iPC > 100) {
		CLogWriter::Write("N3Chr::Load invalid plug count");
		return false;
	}
	this->PlugAlloc(iPC);
	for(int i = 0; i < iPC; i++)
	{
		nL = 0;
		ReadFile(hFile, &nL, 4, &dwRWC, NULL);
		if(nL > 0 && nL < 512)
		{
			ReadFile(hFile, szFN, nL, &dwRWC, NULL); szFN[nL] = NULL;
			CLogWriter::Write("N3Chr::Load plug[%d]=%s", i, szFN);
			// Skip loading plugs - they cause DirectX driver crashes on modern GPUs
			CLogWriter::Write("N3Chr::Load plug[%d] skipped - incompatible with modern drivers", i);
			continue;

//			CN3CPlugBase* pPlug = NULL;
//			e_PlugType eType = CN3CPlugBase::GetPlugTypeByFileName(szFN);
//			if (eType == PLUGTYPE_NORMAL)			
//			{	// PLUGTYPE_NORMAL
//				pPlug = (CN3CPlugBase*) new CN3CPlug();
//				if(false == pPlug->LoadFromFile(szFN))
//				{
//					delete pPlug;
//					continue;
//				}
//			}
//			else if (eType == PLUGTYPE_CLOAK)
//			{	// PLUGTYPE_CLOAK
//				pPlug = (CN3CPlugBase*) new CN3CPlug_Cloak();
//				if(false == pPlug->LoadFromFile(szFN))
//				{
//					delete pPlug;
//					continue;
//				}
//			}
//			else
//			{
//				__ASSERT(0, "Undefined plug type");
//			}
//			m_Plugs.push_back(pPlug);
		}
	}

	CLogWriter::Write("N3Chr::Load all plugs loaded");
	// Animation Control..
	nL = 0;
	ReadFile(hFile, &nL, 4, &dwRWC, NULL);
	CLogWriter::Write("N3Chr::Load anim filename length=%d", nL);
	if(nL > 0 && nL < 512)
	{
		ReadFile(hFile, szFN, nL, &dwRWC, NULL); szFN[nL] = NULL;
		CLogWriter::Write("N3Chr::Load anim file=%s", szFN);
		try {
			this->AniCtrlSet(szFN);
			CLogWriter::Write("N3Chr::Load after AniCtrlSet");
		} catch (...) {
			CLogWriter::Write("N3Chr::Load anim loading exception - animation disabled");
		}
	}

	ReadFile(hFile, m_nJointPartStarts, sizeof(m_nJointPartStarts), &dwRWC, NULL);
	ReadFile(hFile, m_nJointPartEnds, sizeof(m_nJointPartEnds), &dwRWC, NULL);
    CLogWriter::Write("N3Chr::Load JointParts loaded");

	nL = 0;
	ReadFile(hFile, &nL, sizeof(nL), &dwRWC, NULL);
	if (nL > 0)
	{
		ReadFile(hFile, szFN, nL, &dwRWC, NULL); szFN[nL] = NULL;
        CLogWriter::Write("N3Chr::Load FXPlug file=%s", szFN);
		FXPlugSet(szFN);
        CLogWriter::Write("N3Chr::Load FXPlug loaded");
	}
	
    CLogWriter::Write("N3Chr::Load about to call Init");
	this->Init();
    CLogWriter::Write("N3Chr::Load Init done");

	return true;
}

__AnimData* CN3Chr::AniDataCur()
{
	if(NULL == m_pAniCtrlRef) return NULL;
	return m_pAniCtrlRef->DataGet(m_FrmCtrl.iAni);
}

#ifdef _N3TOOL
__AnimData* CN3Chr::AniDataUpper()
{
	if(NULL == m_pAniCtrlRef) return NULL;
	return m_pAniCtrlRef->DataGet(m_FrmCtrlUpper.iAni);
}

bool CN3Chr::Save(HANDLE hFile)
{
	CN3TransformCollision::Save(hFile);

	DWORD dwRWC = 0;
	int nL = 0;

	// ���� ���� �̸� ���ֱ�..
	if(m_pRootJointRef) nL = m_pRootJointRef->FileName().size();
	WriteFile(hFile, &nL, 4, &dwRWC, NULL);
	if(nL > 0) WriteFile(hFile, m_pRootJointRef->FileName().c_str(), nL, &dwRWC, NULL);

	// ������ ���� Part Data�� �ɷ�����..
	std::vector<CN3CPart*> PartsTmp = m_Parts;
	m_Parts.clear();
	m_Parts.reserve(64);

	it_CPart it = PartsTmp.begin(), itEnd = PartsTmp.end();
	CN3CPart* pPart = NULL;
	for(; it != itEnd; it++)
	{
		pPart = *it;
		
		bool bHaveData = false;
		if(pPart->m_pSkinsRef)
		{
			for(int j = 0; j < MAX_CHR_LOD; j++)
			{
				if(pPart->m_pSkinsRef->m_Skins[j].VertexCount() > 0)
				{
					bHaveData = true;
					break;
				}
			}
		}

		if(bHaveData) // ���� �����Ͱ� ������..
		{
			m_Parts.push_back(pPart);
		}
		else // ������..
		{
			delete pPart; // �����?.
			*it = NULL;
		}
	}

	// ���� ����..
	int iPC = m_Parts.size();
	WriteFile(hFile, &iPC, 4, &dwRWC, NULL);
	for(int i = 0; i < iPC; i++)
	{
		nL = m_Parts[i]->FileName().size();
		if(nL <= 0)
		{
			char szFNTmp[256];
			wsprintf(szFNTmp, "%s_Default%d.N3CPart", m_szName.c_str(), i);
			m_Parts[i]->FileNameSet(szFNTmp);
		}
		nL = m_Parts[i]->FileName().size();
		WriteFile(hFile, &nL, 4, &dwRWC, NULL);
		WriteFile(hFile, m_Parts[i]->FileName().c_str(), nL, &dwRWC, NULL);
		m_Parts[i]->SaveToFile();
	}
	
	iPC = m_Plugs.size();
	WriteFile(hFile, &iPC, 4, &dwRWC, NULL);
	for(int i = 0; i < iPC; i++)
	{
		nL = m_Plugs[i]->FileName().size();
		if(nL <= 0)
		{
			char szFNTmp[256];
			wsprintf(szFNTmp, "%s_Default%d.N3CPlug", m_szName.c_str(), i);
			m_Plugs[i]->FileNameSet(szFNTmp);
			i++;
		}

		nL = m_Plugs[i]->FileName().size();
		WriteFile(hFile, &nL, 4, &dwRWC, NULL);
		WriteFile(hFile, m_Plugs[i]->FileName().c_str(), nL, &dwRWC, NULL);
		m_Plugs[i]->SaveToFile();
	}
	
//	nL = 0;
//	if(m_pSkinCollision) nL = m_pSkinCollision->m_szName.size();
//	WriteFile(hFile, &nL, 4, &dwRWC, NULL);
//	if(nL > 0) WriteFile(hFile, m_pSkinCollision->m_szName.c_str(), nL, &dwRWC, NULL);
	
	// Animation Control..
	nL = 0;
	if(m_pAniCtrlRef) nL = m_pAniCtrlRef->FileName().size();
	WriteFile(hFile, &nL, 4, &dwRWC, NULL);
	if(nL > 0)
	{
		WriteFile(hFile, m_pAniCtrlRef->FileName().c_str(), nL, &dwRWC, NULL);
	}

	WriteFile(hFile, m_nJointPartStarts, sizeof(m_nJointPartStarts), &dwRWC, NULL); // ����Ʈ�� �Ϻκ��� ���� ���ϸ��̼� �Ǿ� �Ѵٸ�.. ����Ʈ �ε��� ���� ��ȣ
	WriteFile(hFile, m_nJointPartEnds, sizeof(m_nJointPartEnds), &dwRWC, NULL); // ����Ʈ�� �Ϻκ��� ���� ���ϸ��̼� �Ǿ� �Ѵٸ�.. ����Ʈ �ε��� �� ��ȣ

//////////////////////////////////////////////////
//	Coded (By Dino On 2002-10-11 ���� 2:19:11 )
//	FXPlug
	nL = 0;
	if (m_pFXPlug) nL = m_pFXPlug->FileName().size();
	WriteFile(hFile, &nL, sizeof(nL), &dwRWC, NULL);
	if (nL>0)
	{
		WriteFile(hFile, m_pFXPlug->FileName().c_str(), nL, &dwRWC, NULL);
		m_pFXPlug->SaveToFile();
	}
//	End Of Code (By Dino On 2002-10-11 ���� 2:19:11 )
//////////////////////////////////////////////////

	return true;
}
#endif // end of _N3TOOL

void CN3Chr::Tick(float fFrm)
{
	if(NULL == m_pRootJointRef)
	{
		m_nLOD = -1;
		return;
	}

	// ī�޶��?�ָ� �������� ��������..
	float fDist = (m_vPos - s_CameraData.vEye).Magnitude();
	if(fDist > s_CameraData.fFP + m_fRadius * 2.0f)
	{
		m_nLOD = -1;
		return;
	}

	// LOD ����...
	m_nLOD = MAX_CHR_LOD;
	float fLOD = fDist * s_CameraData.fFOV / (m_fRadius * m_vScale.x); // ��ġ�� ����ϰ�?�Ѵ�..
	for(int i = 0; i < MAX_CHR_LOD; i++)
	{
		if(fLOD < CHR_LOD_CALCULATION_VALUES[0][i]) // �ϴ� ���� ū���� �������� �ϰ� LOD ���� ���� ����..... Render ���� �����Ѵ�....
		{
			m_nLOD = i;
			break;
		}
	}
	if(m_nLOD < 0 && m_nLOD >= MAX_CHR_LOD) return; // LOD ���̸� ������.

	static __Vector3 vPos2;
	vPos2 = m_vPos;
	vPos2.y += 1.8f;
	float fOffset = m_fRadius * 3.0f + 2.0f;
	if(	s_CameraData.IsOutOfFrustum(m_vPos, fOffset) &&
		s_CameraData.IsOutOfFrustum(vPos2, fOffset) ) // ī�޶� ����?�ٱ��̸� ��������.. �� ������ �Ӹ� ������ ����..
	{
		m_nLOD = -1;
		return;
	}

	m_FrmCtrl.fFrmPrev = m_FrmCtrl.fFrmCur; // ������ ���ϸ��̼� �������� �����?����..
	m_FrmCtrlUpper.fFrmPrev = m_FrmCtrlUpper.fFrmCur;

	if(fFrm == FRAME_SELFPLAY) this->TickAnimationFrame();
	else
	{
		int iJC = m_JointRefs.size();
		for(int i = 0; i < iJC; i++) // �� �ܼ��� ����Ʈ�� Tick ���ְ� ������..
		{
			m_JointRefs[i]->TickAnimationKey(fFrm);
			m_JointRefs[i]->ReCalcMatrix();
			m_MtxJoints[i] = m_JointRefs[i]->m_Matrix;
		}

		m_FrmCtrl.fFrmCur = fFrm;
		m_FrmCtrl.fBlendFrm = fFrm;
		m_FrmCtrl.fBlendTime = 0;
	}

	if(!m_Plugs.empty()) this->TickPlugs(fLOD); // ���� ���� Tick

//////////////////////////////////////////////////
//	Coded (By Dino On 2002-10-11 ���� 11:21:21 )
//	FXPlug
	if (m_pFXPlug) m_pFXPlug->Tick(this);
//	End Of Code (By Dino On 2002-10-11 ���� 11:21:21 )
//////////////////////////////////////////////////

}

void CN3Chr::TickAnimationFrame()
{
	if(NULL == m_FrmCtrl.pAniData) return; // ���ϸ��̼� ������ �� ����!!

	// �������̰� �����ð��� ������..
	if(	(m_FrmCtrl.pAniData->iBlendFlags & 1) &&
		m_FrmCtrl.iAniLoop > 0 && 
		m_FrmCtrl.bProcessingDelayNow && 
		m_FrmCtrl.fBlendTime == 0)
	{
		m_FrmCtrl.fBlendTime = m_FrmCtrl.pAniData->fTimeBlend;
		m_FrmCtrl.fBlendTimeCur = 0.0f;
		m_FrmCtrl.fBlendFrm = m_FrmCtrl.pAniData->fFrmEnd;
		m_FrmCtrl.fFrmCur = m_FrmCtrl.pAniData->fFrmStart;
		m_FrmCtrl.iAniLoop = 0;
	}

	float fDelta = s_fSecPerFrm * m_fAniSpeedDelta;			// ���ϸ��̼� �ӵ� ���� ���� 1 �̺���, �� ũ�� ��������..

	if(0.0f != m_FrmCtrl.fBlendTime) // Blending �� ���ϸ��̼��� ������..
	{
		m_FrmCtrl.fBlendTimeCur += fDelta;
		if(m_FrmCtrl.fBlendTimeCur > m_FrmCtrl.fBlendTime) // Blending �� ������.
		{
			m_FrmCtrl.fBlendTime = 0.0f;
			m_FrmCtrl.fBlendTimeCur = 0.0f;
			m_FrmCtrl.bProcessingDelayNow = false; // ����ó�� ��..
		}
	}
	else // ������ �Ұ� ������..
	{
		m_FrmCtrl.fFrmCur += m_FrmCtrl.pAniData->fFrmPerSec * fDelta; // Frame Tick
		if(m_FrmCtrl.fFrmCur < m_FrmCtrl.pAniData->fFrmStart) m_FrmCtrl.fFrmCur = m_FrmCtrl.pAniData->fFrmStart;
		if(m_FrmCtrl.fFrmCur > m_FrmCtrl.pAniData->fFrmEnd) // ���ϸ��̼��� �ѹ� ��ȯ�ϸ�..
		{
			if(m_FrmCtrl.fFreezeTime > 0) // �����?�ȴٸ�..
			{
				m_FrmCtrl.fFrmCur = m_FrmCtrl.pAniData->fFrmEnd; // ���ϸ��̼� �������� ������ ���������� �ϰ�..
				m_FrmCtrl.fFreezeTime -= fDelta; // ���� �ð�..
				if(m_FrmCtrl.fFreezeTime < 0)
				{
					m_FrmCtrl.fFreezeTime = 0; // ���ߴ� �ð� �ٽ� ����..
					m_FrmCtrl.iAniLoop++; // ���ϸ��̼� �ѹ� �ߴ�~
				}
			}
			else
			{
				m_FrmCtrl.fFreezeTime = 0;
				m_FrmCtrl.iAniLoop++;

				if(m_FrmCtrl.bOnceAndFreeze) m_FrmCtrl.fFrmCur = m_FrmCtrl.pAniData->fFrmEnd; // �ѹ� ���ϸ��̼� �ϰ� �����?�Ѵٸ�.. �� ���� �ִ´�..
				else if(m_FrmCtrl.pAniData->iBlendFlags & 1) // ���� �����ð��� ������..
				{
					m_FrmCtrl.fFrmCur = m_FrmCtrl.pAniData->fFrmEnd;
					m_FrmCtrl.bProcessingDelayNow = true; // ���� ó��.. ����..
				}
				else
				{
					float fFrmDiff = m_FrmCtrl.pAniData->fFrmEnd - m_FrmCtrl.pAniData->fFrmStart;
					if(fFrmDiff > 0) m_FrmCtrl.fFrmCur -= m_FrmCtrl.pAniData->fFrmEnd - m_FrmCtrl.pAniData->fFrmStart;
					else m_FrmCtrl.fFrmCur = m_FrmCtrl.pAniData->fFrmStart;
				}
			}
		}
	} // end of else // ������ �Ұ� ������..

	if(m_FrmCtrlUpper.pAniData) // ����ü�� �и��ؾ� �ϸ�.. // ���� ó���ؾ� �Ѵٸ�..
	{
		if(0.0f != m_FrmCtrlUpper.fBlendTime) // Blending �� ���ϸ��̼��� ������..
		{
			m_FrmCtrlUpper.fBlendTimeCur += fDelta;
			if(	m_FrmCtrlUpper.fBlendTimeCur > m_FrmCtrlUpper.fBlendTime) // Blending �� ������.
			{
				if( m_FrmCtrlUpper.iAniLoop > 0) // ������ ��.. ���� ��ü�θ� ���ϸ��̼� �����?
					m_FrmCtrlUpper.Init(); // ��ü ���ϸ��̼� ��!!				
				else
					m_FrmCtrlUpper.fBlendTime = 0;
			}
		}
		else // ������ �Ұ� ������..
		{
			m_FrmCtrlUpper.fFrmCur += m_FrmCtrlUpper.pAniData->fFrmPerSec * fDelta; // Frame Tick
			if(m_FrmCtrlUpper.fFrmCur < m_FrmCtrlUpper.pAniData->fFrmStart) m_FrmCtrlUpper.fFrmCur = m_FrmCtrlUpper.pAniData->fFrmStart;
			if(m_FrmCtrlUpper.fFrmCur > m_FrmCtrlUpper.pAniData->fFrmEnd) // ���ϸ��̼��� �ѹ� ��ȯ�ϸ�..
			{
				if(m_FrmCtrlUpper.fFreezeTime > 0) // �����?�ȴٸ�..
				{
					m_FrmCtrlUpper.fFrmCur = m_FrmCtrlUpper.pAniData->fFrmEnd; // ���ϸ��̼� �������� ������ ���������� �ϰ�..

					m_FrmCtrlUpper.fFreezeTime -= fDelta; // ���� �ð�..
					if(m_FrmCtrlUpper.fFreezeTime < 0) m_FrmCtrlUpper.fFreezeTime = 0;
				}
				else // �����?����.. ��ü ���ϸ��̼����� �������� �Ѵ�.
				{
					m_FrmCtrlUpper.iAniLoop++; // �̷��� ���ϸ��̼��� ������.. ��ü ���ϸ��̼����� �������� �ȴ�.
					m_FrmCtrlUpper.fBlendFrm = m_FrmCtrlUpper.pAniData->fFrmEnd;
					m_FrmCtrlUpper.fBlendTime = m_FrmCtrlUpper.pAniData->fTimeBlend;
					m_FrmCtrlUpper.fBlendTimeCur = 0;
				}
			}
		} // end of else // ������ �Ұ� ������..
	}
}

void CN3Chr::TickJoints()
{
	if(NULL == m_FrmCtrlUpper.pAniData) // ����ü ��°�� ó��..
	{
		int iJC = m_JointRefs.size();
		for(int i = 0; i < iJC; i++)
		{
			if(m_FrmCtrl.fBlendTime > 0)
			{
				float fBlendFactor = m_FrmCtrl.fBlendTimeCur / m_FrmCtrl.fBlendTime;
				m_JointRefs[i]->ReCalcMatrixBlended(m_FrmCtrl.fFrmCur, m_FrmCtrl.fBlendFrm, fBlendFactor); // Joint Animation Blending...
			}
			else
			{
				m_JointRefs[i]->TickAnimationKey(m_FrmCtrl.fFrmCur);
				m_JointRefs[i]->ReCalcMatrix();
			}
			m_MtxJoints[i] = m_JointRefs[i]->m_Matrix;
		}
	}
	else // ����ü ���� ó���ؾ� �Ѵٸ�..
	{
		__FrmCtrl* pFrmCtrls[2] = { &m_FrmCtrl, &m_FrmCtrlUpper };
		for(int i = 0; i < MAX_CHR_ANI_PART; i++)
		{
			// ��ü�ϰ��?��Ʈ ����Ʈ �۵�...
			if(0 == i)
			{
				if(pFrmCtrls[i]->fBlendTime > 0) // Motion Blending �� �ʿ��ϸ�...
				{
					float fBlendFactor = pFrmCtrls[i]->fBlendTimeCur / pFrmCtrls[i]->fBlendTime;
					m_pRootJointRef->ReCalcMatrixBlended(pFrmCtrls[i]->fFrmCur, pFrmCtrls[i]->fBlendFrm, fBlendFactor); // Joint Animation Blending...// ��ü �ϰ��?
				}
				else
				{
					m_pRootJointRef->TickAnimationKey(pFrmCtrls[i]->fFrmCur);
					m_pRootJointRef->ReCalcMatrix();
				}
				m_MtxJoints[i] = m_pRootJointRef->m_Matrix;
			}

			if(m_nJointPartStarts[i] > 0)
			{
				float fBlendFactor = pFrmCtrls[i]->fBlendTimeCur / pFrmCtrls[i]->fBlendTime;
				for(int j = m_nJointPartStarts[i]; j <= m_nJointPartEnds[i]; j++)
				{
					if(pFrmCtrls[i]->fBlendTime > 0)
						m_JointRefs[j]->ReCalcMatrixBlended(pFrmCtrls[i]->fFrmCur, pFrmCtrls[i]->fBlendFrm, fBlendFactor); // Joint Animation Blending...
					else
					{
						m_JointRefs[j]->TickAnimationKey(pFrmCtrls[i]->fFrmCur); // 
						m_JointRefs[j]->ReCalcMatrix();
					}

					m_MtxJoints[j] = m_JointRefs[j]->m_Matrix;
				}
			}
		}
	}
}

void CN3Chr::TickPlugs(float fLOD)
{
	if(NULL == this->m_pRootJointRef) return;

	CN3CPlug*	pPlug = NULL;
	
	float fFrmCur = m_FrmCtrl.fFrmCur;
//	if(m_FrmCtrlUpper.pAniData) fFrmCur = m_FrmCtrlUpper.fFrmCur; // ��ü ���ϸ��̼��� ������...

	int iPC = m_Plugs.size();
	for(int i = 0; i < iPC; i++)
	{
		pPlug = m_Plugs[i];
		pPlug->m_PMeshInst.SetLOD(fLOD);
		if (pPlug->m_ePlugType== PLUGTYPE_NORMAL)
		{
			__AnimData* pAniData = m_FrmCtrlUpper.pAniData;
			if(NULL == pAniData) pAniData = m_FrmCtrl.pAniData;
			if(NULL == pAniData) continue;
			if(pPlug->m_nTraceStep <= 0) continue;
			
			pPlug->m_bRenderTrace = false;
			if(!(pAniData->fFrmPlugTraceEnd - pAniData->fFrmPlugTraceStart)) continue; // ���� �������� ���۰� ���� ���̰� ������..
			
			if(	pPlug->m_nTraceStep > 0 && 
				fFrmCur >= pAniData->fFrmPlugTraceStart &&
				fFrmCur <= pAniData->fFrmPlugTraceEnd) // ���� �������� �ȿ� ������..
			{
				pPlug->m_bRenderTrace = true;

				int iJTmp = 0;
				float fFrmTmp = fFrmCur;
				__Vector3	vTrace0, vTrace1;
				D3DCOLOR crTraceU = pPlug->m_crTrace;
				D3DCOLOR crTraceL = pPlug->m_crTrace;

				for(int j = 0, k = pPlug->m_nTraceStep; j < pPlug->m_nTraceStep; j++, k--) // �������� �����?.
				{
					fFrmTmp = fFrmCur - (j*0.2f);
					iJTmp = 0;
					if(fFrmTmp < pAniData->fFrmStart) fFrmTmp = pAniData->fFrmStart;
					
					m_pRootJointRef->Tick(fFrmTmp);

					vTrace0.Set(0,pPlug->m_fTrace0, 0);
					vTrace1.Set(0,pPlug->m_fTrace1, 0);

					vTrace0 *= pPlug->m_Matrix;
					vTrace0 *= m_JointRefs[pPlug->m_nJointIndex]->m_Matrix;
					vTrace1 *= pPlug->m_Matrix;
					vTrace1 *= m_JointRefs[pPlug->m_nJointIndex]->m_Matrix;

					crTraceU =	(pPlug->m_crTrace & 0xff000000) |
								((((pPlug->m_crTrace & 0x00ff0000) >> 16) * k / pPlug->m_nTraceStep) << 16) |
								((((pPlug->m_crTrace & 0x0000ff00) >> 8) * k / pPlug->m_nTraceStep) << 8) |
								((pPlug->m_crTrace & 0x000000ff) * k / pPlug->m_nTraceStep);
					crTraceL =	(crTraceU & 0xff000000) | ((crTraceU & 0x00fcfcfc) >> 2);

					m_vTraces[i][j*2+0].Set(vTrace0, crTraceL);
					m_vTraces[i][j*2+1].Set(vTrace1, crTraceU);
				}
			}
		}
//		else if (pPlug->m_ePlugType == PLUGTYPE_CLOAK)
//		{	// PLUGTYPE_CLOAK
//#ifdef _N3GAME
//			pPlugCloak = (CN3CPlug_Cloak*)pPlug;
//			pPlugCloak->GetCloak()->Tick(m_nLOD);
//#endif
//		}		
	}
}

void CN3Chr::RemakePlugTracePolygons()
{
	for(int i = 0; i < m_vTraces.size(); i++) delete m_vTraces[i];
	m_vTraces.clear();

	if(m_Plugs.empty()) return;

	int iPC = m_Plugs.size();
	m_vTraces.assign(iPC, NULL);
	for(int i = 0; i < iPC; i++)
	{
		int iTS = m_Plugs[i]->m_nTraceStep;
		if(iTS <= 0) continue;

		m_vTraces[i] = new __VertexColor[iTS * 2];
	}
}

void CN3Chr::Render()
{
	if(NULL == m_pRootJointRef) return;
	if(m_nLOD < 0 || m_nLOD >= MAX_CHR_LOD) return;


#ifdef _DEBUG
	CN3Base::s_RenderInfo.nChr++; // Rendering Information Update...
#endif



	// LOD ���� ����...
	if(s_iLODDelta > 0)
	{
		m_nLOD = MAX_CHR_LOD;
		float fDist = (m_vPos - s_CameraData.vEye).Magnitude();
		float fLOD = fDist * s_CameraData.fFOV / (m_fRadius * m_vScale.x); // ��ġ�� ����ϰ�?�Ѵ�..
		for(int i = 0; i < MAX_CHR_LOD; i++)
		{
			if(fLOD < CHR_LOD_CALCULATION_VALUES[s_iLODDelta][i]) // �ϴ� ���� ū���� �������� �ϰ� LOD ���� ���� ����..... Render ���� �����Ѵ�....
			{
				m_nLOD = i;
				break;
			}
		}
		if(m_nLOD >= MAX_CHR_LOD) m_nLOD = MAX_CHR_LOD - 1; // LOD ���̸� ... 
	}







	s_lpD3DDev->SetTransform(D3DTS_WORLD, &m_Matrix);
	this->TickJoints(); // ����Ʈ ��ĵ�?���?..
	this->BuildMesh(); // ��Ŀ�?���� ����ġ ���?.

	int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++)
	{
		m_Parts[i]->Render(m_nLOD);
	}

	// Plug - ���̴� �κ� Rendering
	CN3CPlug* pPlug = NULL;
	iPC = m_Plugs.size();
	for(int i = 0; i < iPC; i++)
	{
		pPlug = m_Plugs[i];
		
		if(false == pPlug->m_bVisible) continue;

		pPlug->Render(m_Matrix, m_MtxJoints[pPlug->m_nJointIndex]);

		////////////////////////////////////////////////////
		// �˱� �׸���...
		if (pPlug->m_ePlugType == PLUGTYPE_NORMAL)
		{
			if(pPlug->m_nTraceStep <= 1) continue;
			if(false == pPlug->m_bRenderTrace) continue;

			__AlphaPrimitive* pAP = s_AlphaMgr.Add();
			if(pAP)
			{
				pAP->bUseVB				= FALSE;
				pAP->dwBlendDest		= D3DBLEND_ONE;
#ifdef _REPENT
				pAP->dwBlendSrc			= D3DBLEND_ONE;
#else
				pAP->dwBlendSrc			= D3DBLEND_SRCCOLOR;
#endif
				pAP->dwFVF				= FVF_CV;
				pAP->dwPrimitiveSize	= sizeof(__VertexColor);
				pAP->fCameraDistance	= (s_CameraData.vEye - m_Matrix.Pos()).Magnitude();
				pAP->lpTex				= NULL;
				pAP->ePrimitiveType		= D3DPT_TRIANGLESTRIP;
				pAP->nPrimitiveCount	= (pPlug->m_nTraceStep-1)*2;
				pAP->nRenderFlags		= RF_DOUBLESIDED | RF_DIFFUSEALPHA | RF_NOTUSELIGHT | RF_NOTZWRITE;
				pAP->nVertexCount		= (pPlug->m_nTraceStep)*2;
				pAP->MtxWorld			= m_Matrix;
				pAP->pVertices			= m_vTraces[i];
				pAP->pwIndices			= NULL;
			}
		}
		else if (pPlug->m_ePlugType == PLUGTYPE_CLOAK)
		{
		}
		// �˱� �׸���...
		////////////////////////////////////////////////////
	}

//////////////////////////////////////////////////
//	Coded (By Dino On 2002-10-11 ���� 11:20:19 )
//	FXPlug
	if (m_pFXPlug) m_pFXPlug->Render();
//	End Of Code (By Dino On 2002-10-11 ���� 11:20:19 )
//////////////////////////////////////////////////
}


void CN3Chr::BuildMesh()
{
	if(m_nLOD < 0 || m_nLOD >= MAX_CHR_LOD) return;
	if(m_MtxJoints.empty() || m_MtxInverses.empty()) return;
	__ASSERT(m_pRootJointRef, "Joint pointer is NULL!");

	float fWeight = 0;
	int nJIndex = 0, nAffect = 0;

	int i = 0, j = 0, k = 0, nVC = 0;
	CN3IMesh* pIMesh = NULL;
	CN3Skin* pSkin = NULL;

	__Matrix44* pMtxJs = &(m_MtxJoints[0]);
	__Matrix44* pMtxJIs = &(m_MtxInverses[0]);

	int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++)
	{
		pSkin = m_Parts[i]->Skin(m_nLOD);
		if(NULL == pSkin) continue;

		__VertexXyzNormal* pVDest = pSkin->Vertices();
		__VertexSkinned* pVSrc = pSkin->SkinVertices();
		if(NULL == pVDest || NULL == pVSrc) continue;

		nVC = pSkin->VertexCount();
		__Vector3 vFinal;
		int nAffect = 0;
		float* pfWeights = NULL;
		for(j = 0; j < nVC; j++) // j < m_nBoneVertices �� ����..
		{

			nAffect = pVSrc[j].nAffect;
			if(1 == nAffect)
			{
				// ���� ����...
				nJIndex = pVSrc[j].pnJoints[0];
				pVDest[j] = (pVSrc[j].vOrigin * pMtxJIs[nJIndex]) * pMtxJs[nJIndex];
			}
			else if(nAffect > 1) 
			{
				vFinal.Zero();
				pfWeights = pVSrc[j].pfWeights;
				for(k = 0; k < nAffect; k++)
				{
					nJIndex = pVSrc[j].pnJoints[k];
					vFinal += ((pVSrc[j].vOrigin * pMtxJIs[nJIndex]) * pMtxJs[nJIndex]) * pfWeights[k];
				}
				pVDest[j] = vFinal;
			}
		}
	}

/*	if(m_pMeshCollision && m_pSkinCollision)
	{
		__ASSERT(m_pMeshCollision->IndexCount() > 0, "Indices count is less than 0");

		__Vector3* pVDest = m_pMeshCollision->Vertices();
		__VertexSkinned* pVSrc = m_pSkinCollision->Vertices();

		nVC = m_pMeshCollision->VertexCount();
		int nSVC = m_pSkinCollision->VertexCount();
		if(nSVC != nVC)
		{
			__ASSERT(0, "Mesh Vertices count is different to Skin vertices count");
		}
		else
		{
			int nAffect = 0;
			float* pfWeights = NULL;
			for(j = 0; j < nVC; j++) // j < m_nBoneVertices �� ����..
			{
				nAffect = pVSrc[j].nAffect;
				if(1 == nAffect)
				{
					// ���� ����...
					nJIndex = pVSrc[j].pnJoints[0];
					pVDest[j] = (pVSrc[j].vOrigin * pMtxJIs[nJIndex]) * pMtxJs[nJIndex];
				}
				else if(nAffect > 1) 
				{
					pVDest[j].Zero();
					pfWeights = pVSrc[j].pfWeights;
					for(k = 0; k < nAffect; k++)
					{
						if(pfWeights[k] <= 0) continue;

						nJIndex = pVSrc[j].pnJoints[k];
						pVDest[j] += ((pVSrc[j].vOrigin * pMtxJIs[nJIndex]) * pMtxJs[nJIndex]) * pfWeights[k];
					}
				}
			}
		}
	}
*/

}

void CN3Chr::BuildMesh(int nLOD)
{
	if(nLOD < 0 || nLOD >= MAX_CHR_LOD) return;
	if(m_MtxJoints.empty() || m_MtxInverses.empty()) return;
	__ASSERT(m_pRootJointRef, "Joint pointer is NULL!");

	float fWeight = 0;
	int nJIndex = 0, nAffect = 0;

	int i = 0, j = 0, k = 0, nVC = 0;
	CN3IMesh* pIMesh = NULL;
	CN3Skin* pSkin = NULL;

	__Matrix44* pMtxJs = &(m_MtxJoints[0]);
	__Matrix44* pMtxJIs = &(m_MtxInverses[0]);

	int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++)
	{
		pSkin = m_Parts[i]->Skin(nLOD);
		if(NULL == pSkin) continue;

		__VertexXyzNormal* pVDest = pSkin->Vertices();
		__VertexSkinned* pVSrc = pSkin->SkinVertices();
		if(NULL == pVDest || NULL == pVSrc) continue;

		nVC = pSkin->VertexCount();
		__Vector3 vFinal;
		int nAffect = 0;
		float* pfWeights = NULL;
		for(j = 0; j < nVC; j++) // j < m_nBoneVertices �� ����..
		{

			nAffect = pVSrc[j].nAffect;
			if(1 == nAffect)
			{
				// ���� ����...
				nJIndex = pVSrc[j].pnJoints[0];
				pVDest[j] = (pVSrc[j].vOrigin * pMtxJIs[nJIndex]) * pMtxJs[nJIndex];
			}
			else if(nAffect > 1) 
			{
				vFinal.Zero();
				pfWeights = pVSrc[j].pfWeights;
				for(k = 0; k < nAffect; k++)
				{
					nJIndex = pVSrc[j].pnJoints[k];
					vFinal += ((pVSrc[j].vOrigin * pMtxJIs[nJIndex]) * pMtxJs[nJIndex]) * pfWeights[k];
				}
				pVDest[j] = vFinal;
			}
		}
	}
}

void CN3Chr::Init()
{
    CLogWriter::Write("CN3Chr::Init starting");
	if(NULL == m_pRootJointRef) return;

	// ������ � �ִ��� ����ϰ�?. ������ �Ҵ�..

	int iJC = 0;
	__Matrix44 mtxTmp; mtxTmp.Identity();
	m_pRootJointRef->NodeCount(iJC);
	m_JointRefs.assign(iJC, NULL);
	m_MtxJoints.assign(iJC, mtxTmp);
	m_MtxInverses.assign(iJC, mtxTmp);
	
	int nJI = 0;
	m_pRootJointRef->Tick(0); // �ʱ⿡ ���� ��ġ ���?.
	for(int i = 0; i < iJC; i++) // ���� ���� ��ŭ �� ������ ���� �����Ϳ� �������?�����´�..
	{
		m_JointRefs[i] = NULL;
		m_pRootJointRef->FindPointerByID(i, m_JointRefs[i]);
		::D3DXMatrixInverse(&(m_MtxInverses[i]), NULL, &(m_JointRefs[i]->m_Matrix)); // ���� �࿡ ���� �������?���Ѵ�..
		m_MtxJoints[i] = m_JointRefs[i]->m_Matrix;
	}

	this->RemakePlugTracePolygons();

	this->FindMinMax();
	
	// �浹 üũ�� ���� ������.. ũ�⿡ �°� ��ȯ..
	if(NULL == m_pMeshCollision) m_pMeshCollision = new CN3VMesh();
	m_pMeshCollision->CreateCube(m_vMin, m_vMax);
}

void CN3Chr::JointSet(const std::string& szFN)
{
	bool bNeedInit = false;
	if(NULL == m_pRootJointRef) bNeedInit = true;
	else if(m_pRootJointRef && m_pRootJointRef->FileName() != szFN) bNeedInit = true;// ���� �̸��� �޶��?�����?���� �Ѵ�..

	if(bNeedInit)
	{
		s_MngJoint.Delete(&m_pRootJointRef);
		m_pRootJointRef = s_MngJoint.Get(szFN);
		this->Init(); // �ʱ�ȭ...
	}
}

void CN3Chr::PartAlloc(int iCount)
{
	int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++) delete m_Parts[i];
	m_Parts.clear();
		
	if(iCount > 0) 
	{
		m_Parts.assign(iCount, NULL);
		for(int i = 0; i < iCount; i++) m_Parts[i] = new CN3CPart();
	}
}

void CN3Chr::PartDelete(int iIndex)
{
	if(iIndex < 0 || iIndex >= m_Parts.size()) return;

	it_CPart it = m_Parts.begin();
	for(int i = 0; i < iIndex; i++, it++);
	delete *it;
	m_Parts.erase(it);
}

CN3CPart* CN3Chr::PartSet(int iIndex, const std::string& szFN)
{
	if(iIndex < 0 || iIndex >= m_Parts.size()) return NULL;
	if(m_Parts[iIndex]->FileName() == szFN) return m_Parts[iIndex];

	if(szFN.size() <= 0) m_Parts[iIndex]->Release();
	else m_Parts[iIndex]->LoadFromFile(szFN);
	
	return m_Parts[iIndex];
}

void CN3Chr::PlugDelete(int iIndex)
{
	if(iIndex < 0 || iIndex >= m_Plugs.size()) return;

	it_CPlug it = m_Plugs.begin();
	for(int i = 0; i < iIndex; i++, it++);
	
	delete *it;
	m_Plugs.erase(it);
}

CN3CPlug* CN3Chr::PlugSet(int iIndex, const std::string& szFN)
{
	if(iIndex < 0 || iIndex >= m_Plugs.size()) return NULL;
		
	if(m_Plugs[iIndex]->FileName() == szFN) return m_Plugs[iIndex];

	if(szFN.size() <= 0) m_Plugs[iIndex]->Release();
	else m_Plugs[iIndex]->LoadFromFile(szFN);
	
	this->RemakePlugTracePolygons();

	return m_Plugs[iIndex];
}

void CN3Chr::PlugAlloc(int iCount)
{
	int iPC = m_Plugs.size();
	for(int i = 0; i < iPC; i++) delete m_Plugs[i];
	m_Plugs.clear();
		
	if(iCount > 0) 
	{
		m_Plugs.assign(iCount, NULL);
		for(int i = 0; i < iCount; i++) m_Plugs[i] = new CN3CPlug();
	}
}

/*
void CN3Chr::CollisionSkinSet(const std::string& szFN)
{
	s_MngSkin.Delete(m_pSkinCollision);
	m_pSkinCollision = s_MngSkin.Get(szFN);
}
*/

void CN3Chr::AniCtrlSet(const std::string& szFN)
{
	s_MngAniCtrl.Delete(&m_pAniCtrlRef);
	m_pAniCtrlRef = s_MngAniCtrl.Get(szFN);
	
	for(int i = 0; i < MAX_CHR_ANI_PART; i++)
	{
		m_FrmCtrl.iAni = -1;
		m_FrmCtrl.bOnceAndFreeze = false;	// �ѹ��� �ϰ� �����?�Ǵ°�??
		m_FrmCtrl.fFrmCur = 0;
		m_FrmCtrl.fFrmPrev = 0;				// �ֱ� ������
		m_FrmCtrl.iAniLoop = 0;
		m_FrmCtrl.pAniData = NULL;
	}
}

int	CN3Chr::AniCurSet(	int iAni,					// Animation ��ȣ,
						bool bOnceAndFreeze,		// �ѹ��� ���� ���߾��?�ϴ°�??
						float fBlendTime,			// �������ϴ� �ð�(�ʴ���), 
						float fFreezeTime,			// ����ð�?..
						bool bStopUpperAnimation)	// ��ü ���ϸ��̼��� ������.. ���ߵ���
{
	if(NULL == m_pAniCtrlRef) return -1;
	if(iAni == m_FrmCtrl.iAni) return -1; // ���� ���ϸ��̼��̸� ���ư���.
//	if(iAni == m_FrmCtrl.iAni) { m_FrmCtrl.iAniLoop = 0; return -1; } // ���� ���ϸ��̼��̸� ���� ���� ī��Ʈ�� �ʱ�ȭ�ϰ� ���ư���.
	if(iAni < 0 || iAni >= m_pAniCtrlRef->Count()) return -1;

	int iAniPrev = m_FrmCtrl.iAni; // ���� �� ����ϰ�?.
	float fFrmPrev = m_FrmCtrl.fFrmCur; // �ֱ��� ������..

	if(bStopUpperAnimation)
		m_FrmCtrlUpper.Init(); // ��ü ���ϸ��̼� ���� ���� !!
	m_FrmCtrl.Init();

	m_FrmCtrl.iAni = iAni;
	m_FrmCtrl.bOnceAndFreeze = bOnceAndFreeze; // �ѹ��� �ϰ� ���߾��?�ϴ°�....
	m_FrmCtrl.fFreezeTime = fFreezeTime;
	m_FrmCtrl.pAniData = m_pAniCtrlRef->DataGet(iAni);

	m_FrmCtrl.fFrmCur = m_FrmCtrl.pAniData->fFrmStart; // �������� ó������ ����..
	m_FrmCtrl.fFrmPrev = m_FrmCtrl.pAniData->fFrmEnd;
	if(FLT_MIN != fBlendTime) m_FrmCtrl.fBlendTime = fBlendTime; // ���?Blending �� ����..
	else m_FrmCtrl.fBlendTime = m_FrmCtrl.pAniData->fTimeBlend;
	if(m_FrmCtrl.fBlendTime > 0) m_FrmCtrl.fBlendFrm = fFrmPrev; // Blending �� ���ϸ��̼� Frame..
	else m_FrmCtrl.fBlendFrm = m_FrmCtrl.fFrmCur; // Blending �� ���ϸ��̼� �� ����!!

	return iAniPrev;
}

void CN3Chr::AniFixToLastFrame( int iAni ) // ������ ���������� ���� ���� ������.
{
	if(NULL == m_pAniCtrlRef) return;
	if(iAni == m_FrmCtrl.iAni) return; // ���� ���ϸ��̼��̸� ���ư���.
	if(iAni < 0 || iAni >= m_pAniCtrlRef->Count()) return;

	__AnimData* pAniData = m_pAniCtrlRef->DataGet(iAni);
	if(NULL == pAniData) return;

	m_FrmCtrl.Init();
	m_FrmCtrl.iAni = iAni;
	m_FrmCtrl.bOnceAndFreeze = true; // �ѹ��� �ϰ� ���߾��?�ϴ°�....
	m_FrmCtrl.pAniData = m_pAniCtrlRef->DataGet(iAni);
	m_FrmCtrl.fBlendTime = 0; // ���?Blending �� ����..
	m_FrmCtrl.fBlendFrm = pAniData->fFrmEnd; // Blending �� ���ϸ��̼� Frame..
	m_FrmCtrl.fFreezeTime = 1000.0f; // ���� �ð�..
	m_FrmCtrl.fFrmCur = pAniData->fFrmEnd; // �������� ó������ ����..
	m_FrmCtrl.fFrmPrev = pAniData->fFrmEnd;

	return;
}

void CN3Chr::AniUpperSet(int iAni, float fFreezeTime)
{
	if(NULL == m_pAniCtrlRef) return;
	if(iAni < 0 || iAni >= m_pAniCtrlRef->Count()) return;
	if(iAni == m_FrmCtrlUpper.iAni) return; // ���� �Ͱ� ������ �Ѿ��..

	m_FrmCtrlUpper.Init();

	m_FrmCtrlUpper.iAni = iAni;
	m_FrmCtrlUpper.bOnceAndFreeze = false;
	m_FrmCtrlUpper.pAniData = m_pAniCtrlRef->DataGet(iAni);

	// ���?Blending �� ����..
	m_FrmCtrlUpper.fBlendTime = m_FrmCtrlUpper.pAniData->fTimeBlend;
	m_FrmCtrlUpper.fBlendTimeCur = 0;
	m_FrmCtrlUpper.fBlendFrm = m_FrmCtrl.fFrmCur; // ��ü �����Ӱ� Blending �� ���ϸ��̼� Frame..
	m_FrmCtrlUpper.fFreezeTime = fFreezeTime; // ���� �ð�..

	m_FrmCtrlUpper.fFrmCur = m_FrmCtrlUpper.pAniData->fFrmStart; // �������� ó������ ����..
	m_FrmCtrlUpper.fFrmPrev = m_FrmCtrlUpper.pAniData->fFrmEnd;
}

#ifdef _N3TOOL
void CN3Chr::AniDefaultSet()
{
	s_MngAniCtrl.Delete(&m_pAniCtrlRef);
	s_MngAniCtrl.Release();

	// �⺻���� Animation Control �����?.
	CN3AnimControl* pAniCtrlDefault = new CN3AnimControl();
	pAniCtrlDefault->m_szName = "Default";
	pAniCtrlDefault->FileNameSet("Chr\\Default.N3Anim");
	s_MngAniCtrl.Add(pAniCtrlDefault);
	
	m_pAniCtrlRef = pAniCtrlDefault;
}
#endif // #ifdef _N3_TOOL


void CN3Chr::JointPartSet(int nAniPart, int nJS, int nJE)
{
	if(nAniPart < 0 || nAniPart >= MAX_CHR_ANI_PART) return;
	if(m_JointRefs.empty()) return;
	int iJC = m_JointRefs.size();
	if(nJE - nJS < 0 || nJS >= iJC || nJE >= iJC) return;

	m_nJointPartStarts[nAniPart] = nJS;
	m_nJointPartEnds[nAniPart] = nJE;
}

void CN3Chr::FindMinMax()
{
	m_vMin.Zero();
	m_vMax.Zero();
	m_fRadius = 0;

	if(m_JointRefs.empty()) return;
	
	this->TickAnimationKey(m_FrmCtrl.fFrmCur);
	this->TickJoints();

	int iJC = m_JointRefs.size();

	__Vector3 vTmp;
	m_vMin.Set(FLT_MAX, FLT_MAX, FLT_MAX);
	m_vMax.Set(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for(int i = 0; i < iJC; i++) // ���� ���� ��ŭ �� ������ ���� �����Ϳ� �������?�����´�..
	{
		// �� ����Ʈ�� ���� ��ġ ���� ���� �ִ� �ּҰ��� ã�´�.
		vTmp = m_JointRefs[i]->m_Matrix.Pos();
		if(vTmp.x < m_vMin.x) m_vMin.x = vTmp.x;
		if(vTmp.y < m_vMin.y) m_vMin.y = vTmp.y;
		if(vTmp.z < m_vMin.z) m_vMin.z = vTmp.z;
		if(vTmp.x > m_vMax.x) m_vMax.x = vTmp.x;
		if(vTmp.y > m_vMax.y) m_vMax.y = vTmp.y;
		if(vTmp.z > m_vMax.z) m_vMax.z = vTmp.z;
	}

	// �ʹ� ������ �÷��ش�..
	if(m_vMax.x - m_vMin.x < 0.5f) { m_vMax.x += 0.25f; m_vMin.x -= 0.25f; }
	if(m_vMax.y - m_vMin.y < 0.5f) { m_vMax.y += 0.25f; m_vMin.y -= 0.25f; }
	if(m_vMax.z - m_vMin.z < 0.5f) { m_vMax.z += 0.25f; m_vMin.z -= 0.25f; }

	// �ִ� �ּҰ��� ���� ������ ����Ѵ�?.
	m_fRadius  = (m_vMax - m_vMin).Magnitude() * 0.5f;
}

int CN3Chr::CheckCollisionPrecisely(int ixScreen, int iyScreen, __Vector3* pvPick)
{
	__Vector3 vPos, vDir;
	::_Convert2D_To_3DCoordinate(ixScreen, iyScreen, CN3Base::s_CameraData.mtxView, CN3Base::s_CameraData.mtxProjection, CN3Base::s_CameraData.vp, vPos, vDir);


	static __Matrix44 mtxWI, mtxWIRot;
	D3DXMatrixInverse(&mtxWI, NULL, &m_Matrix); // World Matrix Inverse
	mtxWIRot = mtxWI;
	mtxWIRot.PosSet(0,0,0);
	__Vector3 vPos2 = vPos * mtxWI;
	__Vector3 vDir2 = vDir * mtxWIRot;

	CN3VMesh* pvMesh = NULL;
	__Matrix44 Mtx = m_Matrix;
	pvMesh = CollisionMesh();

	if(NULL == pvMesh) 
		return -1;

	if(!pvMesh->Pick(Mtx, vPos, vDir, pvPick))
		return -1;

	// �ָ� �ִ� ĳ���ʹ� �ڽ��θ� üũ�ϰ� ������ �ִ°� �������ϰ� �浹üũ�� �Ѵ�...
	if(m_nLOD >= 3)
		return 1;

	return this->CheckCollisionPrecisely(vPos2, vDir2, pvPick);

}

int CN3Chr::CheckCollisionPrecisely(const __Vector3 &vPos, const __Vector3 &vDir, __Vector3* pvPick)
{
	this->TickJoints(); // ����Ʈ ��ĵ�?���?..
	BuildMesh(m_nLOD);

	__Vector3 v0, v1, v2;

	CN3Skin* pSkin = NULL;
	__Vector3 vPos2 = vPos, vDir2 = vDir;
	int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++)
	{
		pSkin = m_Parts[i]->Skin(m_nLOD);
		if(NULL == pSkin) continue;

		if(pSkin->CheckCollisionPrecisely(vPos2, vDir2, pvPick))
		{
			if(pvPick) (*pvPick) *= m_Matrix;
			return i;
		}
	}

	return -1;
}

//////////////////////////////////////////////////
//	Coded (By Dino On 2002-10-10 ���� 2:36:28 )
//	FXPlug

// FXPlugSet : FXPlug ������ �������ִ� �Լ�
CN3FXPlug* CN3Chr::FXPlugSet(const std::string& strFN)
{
    CLogWriter::Write("CN3Chr::FXPlugSet starting %s", strFN.c_str());
	if (m_pFXPlug) m_pFXPlug->Release();
	else m_pFXPlug = new CN3FXPlug();
	if (false == m_pFXPlug->LoadFromFile(strFN)) return NULL;
	return m_pFXPlug;
}

CN3FXPlug*	CN3Chr::FXPlugCreate()
{
	if (m_pFXPlug) m_pFXPlug->Release();
	else
	{
		m_pFXPlug = new CN3FXPlug();
		
		if (m_szName.size()>0)
		{
			char szFN[_MAX_PATH];
			wsprintf(szFN, "Chr\\%s.N3FXPlug", m_szName.c_str());	// ĳ������ �̸��� ���δ�.
			m_pFXPlug->FileNameSet(szFN);
		}
		else m_pFXPlug->FileNameSet("Chr\\Default.N3FXPlug");	// �׳� default�̸��� ���δ�.
	}
	return m_pFXPlug;
}

void	CN3Chr::FXPlugDelete()
{
	if (m_pFXPlug) delete m_pFXPlug;
	m_pFXPlug = NULL;
}
//	End Of Code (By Dino On 2002-10-10 ���� 2:36:28 )
//////////////////////////////////////////////////
