// N3EffectInstance.cpp: implementation of the CN3EffectInstance class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfxBase.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "N3Shape.h"
#include "N3EffectInstance.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CN3EffectPartInstance::CN3EffectPartInstance()
{
	m_pVDeltas = NULL; // Instance ���� �ִ� ���� Move Vector
	m_pVPosCurs = NULL; // Instance ���� �ִ� Random
	m_pfGravityCurs = NULL; // ��ƼŬ�϶� ���� �߷¼ӵ�...
	m_pnTexIndices = NULL;
	m_pnTexCycles = NULL;
	m_pdwTexAlphaFactors = NULL;
	m_pdwTickPrevs = NULL;
	m_bDeads = NULL;

	this->Release();
}

CN3EffectPartInstance::~CN3EffectPartInstance()
{
	this->Release();
}

void CN3EffectPartInstance::Release()
{
	CN3Base::Release();

	delete [] m_pfGravityCurs; m_pfGravityCurs = NULL;
	delete [] m_pnTexCycles; m_pnTexCycles = NULL;
	delete [] m_pnTexIndices; m_pnTexIndices = NULL;
	delete [] m_pdwTickPrevs; m_pdwTickPrevs = NULL;
	delete [] m_bDeads; m_bDeads = NULL;
	
	delete [] m_pVPosCurs; m_pVPosCurs = NULL;
	delete [] m_pVDeltas; m_pVDeltas = NULL;

	m_pEffectPartRef = NULL; // Effect Part Class Reference Pointer

	m_nTexCycleCount = 0; // Texture Animation �� ���ư� Ƚ��.
	m_nTexIndexCur = 0; // ���� Texture Index;
	m_dwTickPrev = timeGetTime(); // Texture Frame �����

	m_fGravityCur = 0; // ���� �߷¼ӵ�...

	m_pShapeRef = NULL;
}

void CN3EffectPartInstance::EffectSet(CN3EffectPart* pEffect)
{
	if(pEffect == m_pEffectPartRef) return;
	this->Release();
	m_pEffectPartRef = pEffect;
	GenerateRandomPositions();
}

void CN3EffectPartInstance::GenerateRandomPositions()
{
	delete [] m_pfGravityCurs; m_pfGravityCurs = NULL;
	delete [] m_pnTexCycles;
	delete [] m_pnTexIndices;
	delete [] m_pdwTickPrevs;
	delete [] m_bDeads;

	delete [] m_pVPosCurs; m_pVPosCurs = NULL;
	delete [] m_pVDeltas; m_pVDeltas = NULL;

	if(NULL == m_pEffectPartRef || m_pEffectPartRef->m_nBoardCount <= 0) return;
	
	m_pfGravityCurs = new float[m_pEffectPartRef->m_nBoardCount];
	memset(m_pfGravityCurs, 0, 4 * m_pEffectPartRef->m_nBoardCount);

//	if(m_pEffectPartRef->m_eParticleType != PARTICLE_NOTHING) // ��ƼŬ �ϰ�쿡��
//	{
		int nBC = m_pEffectPartRef->m_nBoardCount;
		int nTC = m_pEffectPartRef->TexCount();

		m_pnTexCycles = new int[nBC];
		m_pnTexIndices = new int[nBC];
		m_pdwTickPrevs = new DWORD[nBC];
		m_bDeads = new bool[nBC];

		DWORD dwTick = timeGetTime();

		for(int i = 0; i < nBC; i++) // ������ �ұ�Ģ�ϰ� �����ش�..
		{
			m_pdwTickPrevs[i] = dwTick;
			m_pnTexCycles[i] = 0;
			if(nTC == 1) m_pnTexIndices[i] = rand()%256; // Texture �� �ϳ���.. �������� �����ϰ�..
			else m_pnTexIndices[i] = rand()%nTC;
			m_bDeads[i] = false;
		}
//	}

	
	m_pVPosCurs = new __Vector3[m_pEffectPartRef->m_nBoardCount];
	memset(m_pVPosCurs, 0, sizeof(__Vector3) * m_pEffectPartRef->m_nBoardCount);
	__Vector3 vRMin = m_pEffectPartRef->m_vRandomMin;
	__Vector3 vRMax = m_pEffectPartRef->m_vRandomMax;
	__Vector3 vRDelta = vRMax - vRMin;
	for(i = 0; i < m_pEffectPartRef->m_nBoardCount; i++)
	{
		if(vRDelta.x != 0) m_pVPosCurs[i].x = vRMin.x + vRDelta.x * (1.0f - ((rand()%1000)/1000.0f));
		if(vRDelta.y != 0) m_pVPosCurs[i].y = vRMin.y + vRDelta.y * (1.0f - ((rand()%1000)/1000.0f));
		if(vRDelta.z != 0) m_pVPosCurs[i].z = vRMin.z + vRDelta.z * (1.0f - ((rand()%1000)/1000.0f));
	}

	PARTICLE_TYPE ePType = m_pEffectPartRef->m_eParticleType;
	if(	PARTICLE_NOTHING != ePType)
	{
		m_pVDeltas = new __Vector3[m_pEffectPartRef->m_nBoardCount];
		memset(m_pVDeltas, 0, sizeof(__Vector3) * m_pEffectPartRef->m_nBoardCount);
		
		for(i = 0; i < m_pEffectPartRef->m_nBoardCount; i++)
		{
			m_pVDeltas[i] = m_pVPosCurs[i];
			m_pVDeltas[i].Normalize();
			if(PARTICLE_GATHER == ePType) m_pVDeltas[i] *= -1.0f; // ������ ���ϰ� �Ѵ�..
		}
	}
}

void CN3EffectPartInstance::Tick(DWORD dwTick, __Vector3& vDir)
{
	if(NULL == m_pEffectPartRef || m_pEffectPartRef->m_nBoardCount <= 0) return;

	// Texture Index ó��
	if(dwTick >= m_dwTickPrev + (1000.0f / m_pEffectPartRef->m_fFPS))
	{
		m_nTexIndexCur++;
		m_dwTickPrev = dwTick;
	}
	if(m_nTexIndexCur >= m_pEffectPartRef->TexCount())
	{
		m_nTexIndexCur = 0;
		m_nTexCycleCount++;
	}

	PARTICLE_TYPE eType = m_pEffectPartRef->m_eParticleType;
	if(PARTICLE_NOTHING != eType) // ��ƼŬ �� ���..
	{
		static __Vector3 vDrop;
		float fDelta = m_pEffectPartRef->m_fParticleDelta;
		if(eType == PARTICLE_DROP) // Drop ��ƼŬ �� ���..
		{
			vDrop = vDir * (fDelta/CN3Base::s_fFrmPerSec);
		}

		bool bNeedReset = false;
		float fAccel = m_pEffectPartRef->m_fParticleAccel / CN3Base::s_fFrmPerSec; // ���ӵ�..
		__Vector3 vDTmp;
		for(int i = 0; i < m_pEffectPartRef->m_nBoardCount; i++)
		{
			if(m_bDeads[i]) continue; // ���� ��ƼŬ�̸� ��������..
			bNeedReset = false; // �����ؾ� �ϴ��� �÷���..

			if(fAccel != 0.0f)
			{
				vDTmp = m_pVDeltas[i];
				vDTmp.Normalize();
				m_pVDeltas[i] += vDTmp * fAccel;
			}

			m_pVPosCurs[i].y -= m_pfGravityCurs[i]; // �߷°� ����
			m_pfGravityCurs[i] += m_pEffectPartRef->m_fGravity / CN3Base::s_fFrmPerSec; // ���� �߷°� ���..

			if(eType == PARTICLE_DROP) // Drop ��ƼŬ �� ���..
			{
				m_pVPosCurs[i] += vDrop; // �ڷ� ó���� �ӵ� ����..
			}
			else if(eType != PARTICLE_NOTHING)
//					eType == PARTICLE_GATHER) // Explode ��ƼŬ �� ���..
			{
				m_pVPosCurs[i] += m_pVDeltas[i] * (fDelta / CN3Base::s_fFrmPerSec);
			}

			if(eType == PARTICLE_GATHER && m_pVPosCurs[i].Magnitude() < 0.1f) bNeedReset = true; // Gather ��ƼŬ �� ���..

			int nTC = m_pEffectPartRef->TexCount();
			if(nTC == 1) // Texture �� �����̸� ���� �����ϰ� �����...
			{
				m_pnTexIndices[i] -= (int)(10.0f * m_pEffectPartRef->m_fFPS / s_fFrmPerSec);
				if(m_pnTexIndices[i] <= 0) bNeedReset = true;
			}
			else if(nTC > 1)
			{
				// ��ƼŬ �ؽ�ó �ε��� ���..
				// Texture Index ó��
				if(dwTick >= m_pdwTickPrevs[i] + (1000.0f / m_pEffectPartRef->m_fFPS))
				{
					m_pnTexIndices[i]++;
					m_pdwTickPrevs[i] = dwTick;
				}
				if(m_pnTexIndices[i] >= nTC)
				{
					m_pnTexIndices[i] = 0;
					m_pnTexCycles[i]++;
				}
				if(m_pnTexCycles[i] >= m_pEffectPartRef->m_nTexCycle) bNeedReset = true; // �ؽ�ó�� �ѻ���Ŭ�� �� ����.. -> ���� �̰ɷ� ��ġ�ؾ� �Ѵ�..
			}
			
			if(bNeedReset && PARTICLE_EXPLODE == eType)
			{
				m_bDeads[i] = true; // ��ƼŬ ���δ�..
			}
			else if(bNeedReset)
			{
				m_pVPosCurs[i].Set(0,0,0); // ���� ��ġ�� ���..
				__Vector3 vRMin = m_pEffectPartRef->m_vRandomMin;
				__Vector3 vRMax = m_pEffectPartRef->m_vRandomMax;
				__Vector3 vRDelta = vRMax - vRMin;
				if(vRDelta.x != 0) m_pVPosCurs[i].x = vRMin.x + vRDelta.x * (1.0f - ((rand()%1000)/1000.0f));
				if(vRDelta.y != 0) m_pVPosCurs[i].y = vRMin.y + vRDelta.y * (1.0f - ((rand()%1000)/1000.0f));
				if(vRDelta.z != 0) m_pVPosCurs[i].z = vRMin.z + vRDelta.z * (1.0f - ((rand()%1000)/1000.0f));

				m_pVDeltas[i] = m_pVPosCurs[i];
				m_pVDeltas[i].Normalize();
				
				if(PARTICLE_GATHER == m_pEffectPartRef->m_eParticleType) // ���� ��ƼŬ�̸�...
				{
					m_pVDeltas[i] *= -1.0f;
				}
				
				if(nTC == 1) m_pnTexIndices[i] = 255; // Texture �� �ϳ���.. ���İ��� 255 ��..
				else m_pnTexCycles[i] = 0; // �ؽ�ó�� �������� �ؽ�ó ����Ŭ �ʱ�ȭ..

				m_pfGravityCurs[i] = 0; // �߷°� �ʱ�ȭ.. 
			}
		}
	}
}

void CN3EffectPartInstance::Render(__Vector3& vPos)
{
	if(m_pEffectPartRef == NULL || m_pEffectPartRef->m_nBoardCount <= 0) return;

	// Alpha Blending Factor
	s_lpD3DDev->SetRenderState( D3DRS_SRCBLEND, m_pEffectPartRef->m_dwSrcBlend );
	s_lpD3DDev->SetRenderState( D3DRS_DESTBLEND, m_pEffectPartRef->m_dwDestBlend );

	s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,    D3DTOP_SELECTARG1 );
	s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1,  D3DTA_TEXTURE );
	
	int nTC = m_pEffectPartRef->TexCount();
	
		//color = tex mod diffuse
	s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
//	s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
//	s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	if(nTC == 1)
	{
		//alpha = select texture alpha
		s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
//		s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

		//stage 1
		s_lpD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		s_lpD3DDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

//		s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
//		s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	}
	else
	{
		s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	}

	static __Matrix44 mtx;
	mtx = CN3Base::s_CameraData.mtxViewInverse;
	mtx.PosSet(0,0,0);

	CN3Texture* pTex;

	__Vector3 vD;
	PARTICLE_TYPE pType = m_pEffectPartRef->m_eParticleType;
	DWORD dwAlpha;
	for(int i = 0; i < m_pEffectPartRef->m_nBoardCount; i++)
	{
		if(m_bDeads[i]) continue; // ���� ��ƼŬ�� �� ���� �ʴ´�..

		if(nTC == 1) // �ؽ�ó�� �ϳ��̸�..
		{
			dwAlpha = (m_pnTexIndices[i] << 24) | (m_pnTexIndices[i] << 16) | (m_pnTexIndices[i] << 8); // �������� ���..
//			dwAlpha = 0x10000000;
//			s_lpD3DDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwAlpha);	// alpha factor
			m_pEffectPartRef->m_VPlanes[0].color = dwAlpha;
			m_pEffectPartRef->m_VPlanes[1].color = dwAlpha;
			m_pEffectPartRef->m_VPlanes[2].color = dwAlpha;
			m_pEffectPartRef->m_VPlanes[3].color = dwAlpha;
			m_pEffectPartRef->m_VPlanes[4].color = dwAlpha;
			m_pEffectPartRef->m_VPlanes[5].color = dwAlpha;
			
			pTex = m_pEffectPartRef->Tex(m_nTexIndexCur);
		}
		else
		{
			pTex = m_pEffectPartRef->Tex(m_pnTexIndices[i]);
		}
		if(pTex) s_lpD3DDev->SetTexture(0, pTex->Get());

		mtx.PosSet(vPos + m_pVPosCurs[i]);

		s_lpD3DDev->SetTransform(D3DTS_WORLD, &mtx);
		// Render the vertex buffer contents
		s_lpD3DDev->SetFVF( FVF_XYZCOLORT1 );
		s_lpD3DDev->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 2, m_pEffectPartRef->m_VPlanes, sizeof(__VertexXyzColorT1));
	}
}

CN3EffectInstance::CN3EffectInstance()
{
	Release();
}

CN3EffectInstance::~CN3EffectInstance()
{
	Release();
}

void CN3EffectInstance::Release()
{
	m_pEffectRef = NULL; // Effect Reference Pointer
	m_dwTickPrev0 = m_dwTickPrev1 = timeGetTime();

	m_PartInst0.Release();
	m_PartInst1.Release();

	m_fTime = 0.0f;
	m_nTickStep = 0;
}

void CN3EffectInstance::Create(CN3Effect* pEffect)
{
	m_pEffectRef = pEffect;
	m_fTime = 0.0f;
	m_nTickStep = 0;
}

void CN3EffectInstance::Tick(DWORD dwTick)
{
	if(NULL == m_pEffectRef || m_nTickStep >= m_pEffectRef->TickStepCount()) return;
	
	// ó�� �ܰ踦 �����Ѵ�..
	CN3Effect::__EFFECT_TICK* pEC = m_pEffectRef->TickStepStruct(m_nTickStep);

	CN3EffectPart* pEP0 = m_pEffectRef->Part(pEC->nPart0);
	CN3EffectPart* pEP1 = m_pEffectRef->Part(pEC->nPart1);

	if(NULL == pEP0 && NULL == pEP1)
	{
		m_nTickStep++; // ���� �ܰ�� �Ѿ��.
		m_dwTickPrev0 = dwTick; // �ֱ� �ð��� ����� ���´�..

		return;
	}

	m_PartInst0.EffectSet(pEP0);
	m_PartInst1.EffectSet(pEP1);
	
	bool bCollision = false;
	__Vector3 vPosCollision = m_vPosCur;

	if(pEP0)
	{
		int nTC = pEP0->TexCount();
		if(nTC > 1) // Texture �� �������̸� ���� �ܰ�� �Ѿ�� ������ TexCycleCount �̴�..
		{
			if(m_PartInst0.TexCycleCount() >= pEC->nCondition)
			{
				m_nTickStep++; // ���� �ܰ�� �Ѿ��.
				m_dwTickPrev0 = dwTick; // �ֱ� �ð��� ����� ���´�..
			}
		}
		else
		{
			if(dwTick > m_dwTickPrev0 + pEC->nCondition)
			{
				m_nTickStep++; // ���� �ܰ�� �Ѿ��.
				m_dwTickPrev0 = dwTick; // �ֱ� �ð��� ����� ���´�..
			}
		}

		// ���ư���.. ��ġ�� ����Ѵ�..
		if(0.0f != pEP0->m_fSpeedY || 0.0f != pEP0->m_fSpeedZ)
		{
			float fSpeed = (pEP0->m_fSpeedZ / CN3Base::s_fFrmPerSec);
			m_vPosCur	+= m_vDir * fSpeed; // ���� ��ġ ���..
			m_vPosCur.y += pEP0->m_fSpeedY / CN3Base::s_fFrmPerSec;
			m_vPosCur.y -= m_PartInst0.m_fGravityCur;
			m_PartInst0.m_fGravityCur += pEP0->m_fGravity / CN3Base::s_fFrmPerSec; // �߷� ����
		}

		// ���� �浹 üũ�� �ʿ��� �Ŷ��..
		if(pEP0->m_bNeedCollisionCheck)
		{
			// ����� �浹 �˻�..
			float fHeight = 0;
			float fUnitSize = pEP0->m_fBoardSize / 4.0f; // Board ������ ���� �۰� üũ�Ѵ�.
		}
	}

	if(NULL != pEP0 && true == bCollision)
	{
		m_vPosCur = vPosCollision; // �浹 ��ġ�� ��ġ ����...
//		m_vPosCur.y += pEP0->m_fBoardSize / 2.0f; // ���� ũ���� �ݸ�ŭ �����ش�..
	}

	// ���� Shape Pointer �� �ִٸ�..
	if(m_PartInst0.m_pShapeRef) m_PartInst0.m_pShapeRef->PosSet(m_vPosCur);
	if(m_PartInst1.m_pShapeRef) m_PartInst1.m_pShapeRef->PosSet(m_vPosCur);

	// ī�޶� �þ� ����..
	m_bDontRender = s_CameraData.IsOutOfFrustum(m_vPosCur, 20.0f);
	if(FALSE == m_bDontRender)
	{
		__Vector3 vDir;
		vDir = m_vPosPrev - m_vPosCur;
		vDir.Normalize();
		m_PartInst0.Tick(dwTick, vDir);
		m_PartInst1.Tick(dwTick, vDir);
	}

	m_vPosPrev = m_vPosCur; // ������ġ�� ����صд�..
}

void CN3EffectInstance::Render()
{
	if(TRUE == m_bDontRender) return;

	m_PartInst0.Render(m_vPosCur);
	m_PartInst1.Render(m_vPosCur);
}

void CN3EffectInstance::MakeBoard(__VertexXyzColorT1 *pVs, float fSize, D3DCOLOR Color, __Vector3& vOffset, __Vector3& vRandom)
{
	__Vector3 vR(0,0,0);
	if(vRandom.x > 0) vR.x = (vRandom.x/2.0f) * (1.0f - ((rand()%2000)/1000.0f));
	if(vRandom.y > 0) vR.y = (vRandom.y/2.0f) * (1.0f - ((rand()%2000)/1000.0f));
	if(vRandom.z > 0) vR.z = (vRandom.z/2.0f) * (1.0f - ((rand()%2000)/1000.0f));
	
	pVs[0].Set(-fSize/2.0f + vOffset.x + vR.x,  fSize/2.0f + vOffset.y + vR.y, vOffset.z + vR.z, Color, 0.0f, 0.0f);
	pVs[1].Set( fSize/2.0f + vOffset.x + vR.x,  fSize/2.0f + vOffset.y + vR.y, vOffset.z + vR.z, Color, 1.0f, 0.0f);
	pVs[2].Set( fSize/2.0f + vOffset.x + vR.x, -fSize/2.0f + vOffset.y + vR.y, vOffset.z + vR.z, Color, 1.0f, 1.0f);
	pVs[3] = pVs[0];
	pVs[4] = pVs[2];
	pVs[5].Set(-fSize/2.0f + vOffset.x + vR.x, -fSize/2.0f + vOffset.y + vR.y, vOffset.z + vR.y, Color, 0.0f, 1.0f);
}
