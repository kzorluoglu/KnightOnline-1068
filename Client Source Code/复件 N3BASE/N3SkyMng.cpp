// N3SkyMng.cpp: implementation of the CN3SkyMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafxBase.h"
#include "N3SkyMng.h"
#include "N3Sky.h"
#include "N3Moon.h"
#include "N3Sun.h"
#include "N3Cloud.h"
#include "N3Star.h"
#include "mmsystem.h"
#include "N3Texture.h"

#include "N3GERain.h"
#include "N3GESnow.h"

#ifdef _N3GAME // ������ �ƴ� �������� �ʿ����...
#include "N3SndObj.h"
#include "N3SndMgr.h"
#endif // #ifdef _N3GAME

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

typedef std::vector<__SKY_DAYCHANGE>::iterator it_SDC;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CN3SkyMng::CN3SkyMng()
{
	m_pMoon = NULL;
	m_pSky = NULL;
	m_pSun = NULL;
	m_pCloud = NULL;
	m_pStar = NULL;

	memset(m_pLightColorDiffuses, 0, sizeof(m_pLightColorDiffuses));
	memset(m_pLightColorAmbients, 0, sizeof(m_pLightColorAmbients));

	m_pGERain = NULL;
	m_pGESnow = NULL;

	m_iDayChangeCurPos = 0;
	m_iWeatherChangeCurPos = 0;
	m_eWeather = SW_CLEAR;
	m_dwCheckTick = timeGetTime();

	m_iYear = 0;
	m_iMonth = 0;
	m_iDay = 0;

	m_iHourFix = 0;

#ifdef _N3GAME // ������ �ƴ� �������� �ʿ����...
	m_pSnd_Weather_Snow = NULL;
	m_pSnd_Weather_Rain = NULL;
#endif // #ifdef _N3GAME
}

CN3SkyMng::~CN3SkyMng()
{
	if (m_pSky) {delete m_pSky;	m_pSky = NULL;}
	if (m_pMoon) {delete m_pMoon; m_pMoon = NULL;}
	if (m_pSun)	{delete m_pSun;	m_pSun = NULL;}
	if (m_pCloud) {delete m_pCloud; m_pCloud = NULL;}
	if (m_pStar) {delete m_pStar; m_pStar = NULL;}
	for(int i = 0; i < MAX_GAME_LIGHT; i++)
	{
		delete m_pLightColorDiffuses[i]; m_pLightColorDiffuses[i] = NULL;
		delete m_pLightColorAmbients[i]; m_pLightColorAmbients[i] = NULL;
	}

	m_fCellSize = 0.0f;
	m_CurCellPos.x = m_CurCellPos.y = -1;
	if (m_pGERain) {delete m_pGERain; m_pGERain = NULL;}
	if (m_pGESnow) {delete m_pGESnow; m_pGESnow = NULL;}

#ifdef _N3GAME // ������ �ƴ� �������� �ʿ����...
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Weather_Snow);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Weather_Rain);
#endif // #ifdef _N3GAME
}

#ifdef _N3GAME // ������ �ƴ� �������� �ʿ����...
void CN3SkyMng::ReleaseSound()
{	
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Weather_Snow);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Weather_Rain);
}
#endif // #ifdef _N3GAME

void CN3SkyMng::Release()
{
	CN3Base::Release();
	
	if (m_pSky) {delete m_pSky;	m_pSky = NULL;}
	if (m_pMoon) {delete m_pMoon; m_pMoon = NULL;}
	if (m_pSun)	{delete m_pSun;	m_pSun = NULL;}
	if (m_pCloud) {delete m_pCloud; m_pCloud = NULL;}
	if (m_pStar) {delete m_pStar; m_pStar = NULL;}
	for(int i = 0; i < MAX_GAME_LIGHT; i++)
	{
		delete m_pLightColorDiffuses[i]; m_pLightColorDiffuses[i] = NULL;
		delete m_pLightColorAmbients[i]; m_pLightColorAmbients[i] = NULL;
	}

	m_DayChanges.clear();
	m_iDayChangeCurPos = 0;

	m_WeatherChanges.clear();
	m_iWeatherChangeCurPos = 0;
	m_eWeather = SW_CLEAR;

	if (m_pGERain) {delete m_pGERain; m_pGERain = NULL;}
	if (m_pGESnow) {delete m_pGESnow; m_pGESnow = NULL;}

#ifdef _N3GAME // ������ �ƴ� �������� �ʿ����...
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Weather_Snow);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Weather_Rain);
#endif // #ifdef _N3GAME
}

void CN3SkyMng::Render()
{
    // Disable zbuffer, center view matrix, and set FOV to 72 degrees
    __Matrix44 matView = s_CameraData.mtxView;
    matView._41 = matView._42 = matView._43 = 0.0f;
    
    s_lpD3DDev->SetTransform( D3DTS_VIEW,       &matView );

	// backup render state
	DWORD dwAlphaBlend, dwSrcBlend, dwDestBlend, dwZEnable, dwFog, dwLighting;
    s_lpD3DDev->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlend );
    s_lpD3DDev->GetRenderState( D3DRS_SRCBLEND,   &dwSrcBlend );
    s_lpD3DDev->GetRenderState( D3DRS_DESTBLEND,  &dwDestBlend );
	s_lpD3DDev->GetRenderState( D3DRS_ZENABLE, &dwZEnable);
	s_lpD3DDev->GetRenderState( D3DRS_FOGENABLE, &dwFog );
	s_lpD3DDev->GetRenderState( D3DRS_LIGHTING,	&dwLighting );

	// set render state
	if (TRUE != dwAlphaBlend) s_lpD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	if (D3DBLEND_SRCALPHA != dwSrcBlend) s_lpD3DDev->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
	if (D3DBLEND_INVSRCALPHA != dwDestBlend) s_lpD3DDev->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
	if (D3DZB_FALSE != dwZEnable) s_lpD3DDev->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
    if (FALSE != dwFog) s_lpD3DDev->SetRenderState( D3DRS_FOGENABLE, FALSE );
	if (FALSE != dwLighting) s_lpD3DDev->SetRenderState( D3DRS_LIGHTING, FALSE );
	s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );	// default ��ġ �̴�.
	s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );		// default ��ġ �̴�.

	if (m_pSky) m_pSky->Render();							// �ϴ�
	if (m_pStar) m_pStar->Render();							// ��
	if (m_pMoon) m_pMoon->Render(matView, s_CameraData.mtxProjection);							// ��
	if (m_pSun)	 m_pSun->Render(matView, s_CameraData.mtxProjection);							// ��
	if (m_pCloud) m_pCloud->Render();						// ����

    // Restore the modified renderstates
	if (TRUE != dwAlphaBlend) s_lpD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlend );
	if (D3DBLEND_SRCALPHA != dwSrcBlend) s_lpD3DDev->SetRenderState( D3DRS_SRCBLEND,   dwSrcBlend );
	if (D3DBLEND_INVSRCALPHA != dwDestBlend) s_lpD3DDev->SetRenderState( D3DRS_DESTBLEND,  dwDestBlend );
	if (D3DZB_FALSE != dwZEnable) s_lpD3DDev->SetRenderState( D3DRS_ZENABLE, dwZEnable );
    if (FALSE != dwFog) s_lpD3DDev->SetRenderState( D3DRS_FOGENABLE, dwFog );
	if (FALSE != dwLighting) s_lpD3DDev->SetRenderState( D3DRS_LIGHTING, dwLighting );
    s_lpD3DDev->SetTransform( D3DTS_VIEW,       &s_CameraData.mtxView );
}

void CN3SkyMng::RenderWeather()
{
	int i, j;
	for (i=0; i<3; ++i)
	{
		for(j=0; j<3; ++j)
		{
			if (m_pGERain)
			{
				m_pGERain->Render(m_vPos[j*3+i]);
			}
			if (m_pGESnow)
			{
				m_pGESnow->Render(m_vPos[j*3+i]);
			}
		}
	}
}

void CN3SkyMng::Tick()
{
	DWORD dwCurTickCount = timeGetTime();
	__ASSERT(dwCurTickCount >= m_dwCheckTick,"�����̴�.");
	DWORD dwCurGameTime = m_dwCheckGameTime + (DWORD)((dwCurTickCount - m_dwCheckTick)*TIME_REAL_PER_GAME*0.001f);
	if (!m_DayChanges.empty())
	{
		// dwCurGameTime �� 24*60*60�� �Ѿ��� ���
		if ( dwCurGameTime>86400)
		{
			m_iDayChangeCurPos = 0;
			__ASSERT(m_dwCheckGameTime < 86400, "�ð��� �̻��ؿ�");
			m_dwCheckTick += (int)((86400 - m_dwCheckGameTime)*TIME_GAME_PER_REAL);	// CheckTick�� ���� 0�ÿ� �°� �ٽ� ����
			dwCurGameTime -= 86400;
			m_dwCheckGameTime = 0;	// ���� ���ӽð��� 0�÷�..

			m_iDay++; // ��¥ ����..
			m_pMoon->SetMoonPhase(m_iMonth*30 + m_iDay); // �޸���� ��ȭ��Ű��..
			if(m_iDay > 30)
			{
				m_iDay = 0;
				m_iMonth++;
				if(m_iMonth > 12)
				{
					m_iMonth = 0;
					m_iYear++;
				}
			}
		}

		// ���� ��ġ ���
		float fAngleTime = 0;
		if(m_iHourFix > 0) fAngleTime = (m_iHourFix * 3600.0f / 86400.0f) * 360.0f;// �ؿ� ���� ��ġ�� �����ž� �Ѵٸ�..
		else fAngleTime = (dwCurGameTime / 86400.0f) * 360.0f;

		if(m_pSun) m_pSun->SetCurAngle(fAngleTime + 270.0f);
		if(m_pMoon) m_pMoon->SetCurAngle(fAngleTime + 90.0f);

		while(m_iDayChangeCurPos < m_DayChanges.size() &&
			m_DayChanges[m_iDayChangeCurPos].dwWhen < dwCurGameTime)
		{
			__SKY_DAYCHANGE* pSDC = &(m_DayChanges[m_iDayChangeCurPos]);
			// ������ ������ �ð��� ���� �ð� ����
			DWORD dwDiffTime = dwCurGameTime - pSDC->dwWhen;

			// ��ȭ�� �ɸ��� �ð� ���� 
			float fTakeTime = pSDC->fHowLong - dwDiffTime*TIME_GAME_PER_REAL;	// �ɸ��� �ð�
			if (fTakeTime<0.0f)
			{
				TRACE("!!!!! �ϴ��� ��� ��ȭ�� !!!!!\n");
				fTakeTime = 0.0f;	// 0���� ������ ��� ��ȭ�ϰ� ����
			}

			// �ϴ� ��ȭ���� �����ϱ�
			if (SW_CLEAR == m_eWeather ||	// ���� �����̰ų�
				(SDC_SKYCOLOR != pSDC->eSkyDayChange && SDC_FOGCOLOR != pSDC->eSkyDayChange &&
				SDC_SUNCOLOR != pSDC->eSkyDayChange && SDC_FLARECOLOR != pSDC->eSkyDayChange && SDC_GLOWCOLOR != pSDC->eSkyDayChange &&
				SDC_CLOUD1COLOR != pSDC->eSkyDayChange && SDC_CLOUD2COLOR != pSDC->eSkyDayChange &&
				SDC_CLOUDTEX != pSDC->eSkyDayChange))	// ���� ��ȭ��Ұ� �ƴҰ��
			{	
				ChangeSky(pSDC, fTakeTime);
			}
			m_iDayChangeCurPos++;
		}

		// ���� ��ȭ�� ���� �ϴ� ��ȭ���� �����ϱ�
		if (!m_WeatherChanges.empty())
		{
			while(m_iWeatherChangeCurPos < m_WeatherChanges.size() &&
				m_WeatherChanges[m_iWeatherChangeCurPos].dwWhen < dwCurGameTime )
			{
				__SKY_DAYCHANGE* pSDC = &(m_WeatherChanges[m_iWeatherChangeCurPos]);
				// ������ ������ �ð��� ���� �ð� ����
				DWORD dwDiffTime = dwCurGameTime - pSDC->dwWhen;

				// ��ȭ�� �ɸ��� �ð� ���� 
				float fTakeTime = pSDC->fHowLong - dwDiffTime*TIME_GAME_PER_REAL;	// �ɸ��� �ð�
				if (fTakeTime<0.0f)	fTakeTime = 0.0f;	// 0���� ������ ��� ��ȭ�ϰ� ����
				ChangeSky(pSDC, fTakeTime);	// ��ȭ��Ű��
				m_iWeatherChangeCurPos++;
			}
			// ���� ��ȭ queue �����ϱ�
			if (m_iWeatherChangeCurPos>=m_WeatherChanges.size())
			{
				m_WeatherChanges.clear();
				m_iWeatherChangeCurPos = 0;
			}
		}
	}
	if (m_pSky) m_pSky->Tick();
//	if (m_pMoon) m_pMoon->Tick();
	if (m_pSun) m_pSun->Tick();
	if (m_pCloud) m_pCloud->Tick();
	if (m_pStar) m_pStar->Tick();
	
	for(int i = 0; i < MAX_GAME_LIGHT; i++)
	{
		m_pLightColorDiffuses[i]->Tick();
		m_pLightColorAmbients[i]->Tick();
	}

	if (m_fCellSize > 0.0f)
	{
		POINT NewCellPos;	
		NewCellPos.x = int(s_CameraData.vEye.x/m_fCellSize);
		NewCellPos.y = int(s_CameraData.vEye.z/m_fCellSize);
		if (NewCellPos.x != m_CurCellPos.x || NewCellPos.y != m_CurCellPos.y)
		{
			m_CurCellPos = NewCellPos;
			int i, j;
			for(i=0; i<3; ++i)
				for(j=0; j<3; ++j)
					m_vPos[j*3+i].Set( (m_CurCellPos.x+i-0.5f)*m_fCellSize, 0, (m_CurCellPos.y+j-0.5f)*m_fCellSize);
		}

		if (m_pGERain)
		{
			m_pGERain->Tick();
			if(m_pGERain->NeedDelete()) { delete m_pGERain; m_pGERain = NULL; }
		}
		if (m_pGESnow)
		{
			m_pGESnow->Tick();
			if(m_pGESnow->NeedDelete()) { delete m_pGESnow; m_pGESnow = NULL; }
		}
	}
}

#ifdef _N3TOOL
bool CN3SkyMng::DayChangeParse(FILE* fp, __SKY_DAYCHANGE* pDayChange)
{
	if(NULL == fp || NULL == pDayChange) return false;
	char szLine0[512] = "", szLine1[512] = "";
	char* pResult0 = fgets(szLine0, 256, fp);
	char* pResult1 = fgets(szLine1, 256, fp);
	if(NULL == pResult0 || NULL == pResult1) return false;

	pDayChange->Init(); // �ʱ�ȭ ���ְ�..
	
	pDayChange->szName = szLine0;
	if(pDayChange->szName.size() >= 2)
		pDayChange->szName = pDayChange->szName.substr(0, pDayChange->szName.size() - 2);
	sscanf(szLine1, "Type : [%d], Time : [%d], Parameter1 : [%d], Parameter2 : [%d], Elapse : [%f]",
		&(pDayChange->eSkyDayChange), &pDayChange->dwWhen, &pDayChange->dwParam1, &pDayChange->dwParam2, &pDayChange->fHowLong);

	return true;
}
#endif // #ifdef _N3TOOL

#ifdef _N3TOOL
bool CN3SkyMng::DayChangeWrite(FILE* fp, __SKY_DAYCHANGE* pDayChange)
{
	if(NULL == fp || NULL == pDayChange) return false;
	
	char szLine[512] = "";
	fprintf(fp, "%s\r\n", pDayChange->szName.c_str());
	fprintf(fp, "Type : [%d], Time : [%d], Parameter1 : [%d], Parameter2 : [%d], Elapse : [%f]\r\n",
		pDayChange->eSkyDayChange, pDayChange->dwWhen, pDayChange->dwParam1, pDayChange->dwParam2, pDayChange->fHowLong);

	return true;
}
#endif // #ifdef _N3TOOL

#ifdef _N3TOOL
bool CN3SkyMng::LoadFromTextFile(const char* szIniFN)
{
	if(NULL == szIniFN || strlen(szIniFN) <= 0) return false;

	FILE* fp = fopen(szIniFN, "r");
	if(!fp) return false;

	std::string szMoon;
	std::string szSuns[NUM_SUNPART];
	std::string szClouds[NUM_CLOUD];

	char szLine[512] = "", szBuff[256] = "";
	char* pResult = fgets(szLine, 512, fp);
	if(pResult)
	{
		sscanf(szLine, "Moon : %s", szBuff);
		szMoon = szBuff;
	}

	for(int i = 0 ; i < NUM_SUNPART; i++)
	{
		pResult = fgets(szLine, 512, fp);
		if(pResult)
		{
			sscanf(szLine, "Sun : %s", szBuff);
			szSuns[i] = szBuff;
		}
	}

	for(i = 0 ; i < NUM_CLOUD; i++)
	{
		pResult = fgets(szLine, 512, fp);
		if(pResult)
		{
			sscanf(szLine, "Cloud : %s", szBuff);
			szClouds[i] = szBuff;
		}
	}

	int iSDCC = 0;
	pResult = fgets(szLine, 512, fp);
	if(pResult)
	{
		sscanf(szLine, "DayChange Count : %d", &iSDCC);

		if(iSDCC > 0)
		{
			m_DayChanges.assign(iSDCC);
			for(int i = 0; i < iSDCC; i++)
			{
				if(false == this->DayChangeParse(fp, &(m_DayChanges[i])))
				{
					char szErrLine[128];
					sprintf(szErrLine, "From \"DayChage Count : \" -> Line : %d", i);
					MessageBox(CN3Base::s_hWndBase, szErrLine, "�ϴ� ȯ�漳�� ������ Parsing ����", MB_OK);
					this->Release();
					return false;
				}
			}
		}
	}

	fclose(fp); // ���� �ݱ�..

	if(NULL == m_pSky) m_pSky = new CN3Sky();
	m_pSky->Init();

	if(NULL == m_pStar) m_pStar = new CN3Star();
	m_pStar->Init();

	if(NULL == m_pSun) m_pSun = new CN3Sun();
	m_pSun->Init(szSuns);

	if(NULL == m_pCloud) m_pCloud = new CN3Cloud();
	m_pCloud->Init(szClouds);

	if(NULL == m_pMoon) m_pMoon = new CN3Moon();
	m_pMoon->Init(szMoon);

	for(i = 0; i < MAX_GAME_LIGHT; i++)
	{
		if(NULL == m_pLightColorDiffuses[i]) m_pLightColorDiffuses[i] = new CN3ColorChange();
		if(NULL == m_pLightColorAmbients[i]) m_pLightColorAmbients[i] = new CN3ColorChange();
	}

	std::vector<__SKY_DAYCHANGE>(m_DayChanges).swap(m_DayChanges); // �뷮�� �� ���߱�..

	qsort(&(m_DayChanges[0]), m_DayChanges.size(), sizeof(__SKY_DAYCHANGE), CompareTime);
	SetCheckGameTime(CONVERT_SEC(10,0,0));

	return true;
}
#endif // #ifdef _N3TOOL

#ifdef _N3TOOL
bool CN3SkyMng::SaveToTextFile(const char* szIniFN)
{
	if(NULL == szIniFN || strlen(szIniFN) <= 0) return false;

	FILE* fp = fopen(szIniFN, "w");
	if(!fp) return false;

	char szBuff[256] = "";

	if(this->MoonTextureGet()) fprintf(fp, "Moon : %s\r\n", this->MoonTextureGet()->FileName().c_str());
	else fprintf(fp, "Moon : \r\n");
	

	int i = 0;
	for(i = 0; i < NUM_SUNPART; i++) 
	{
		if(this->SunTextureGet(i)) fprintf(fp, "Sun : %s\r\n", this->SunTextureGet(i)->FileName().c_str());
		else fprintf(fp, "Sun : \r\n");
	}

	for(i = 0; i < NUM_CLOUD; i++) 
	{
		if(this->CloudTextureFileName(i)) fprintf(fp, "Cloud : %s\r\n", this->CloudTextureFileName(i));
		else fprintf(fp, "Cloud : \r\n");
	}
	

	int iDC = m_DayChanges.size();
	fprintf(fp, "DayChange Count : %d\r\n", iDC);
	for(i = 0; i < iDC; i++)
	{
		this->DayChangeWrite(fp, &(m_DayChanges[i]));
	}

	fclose(fp); // ���� �ݱ�..

	return true;
}
#endif // #ifdef _N3TOOL

#ifdef _N3TOOL
void CN3SkyMng::InitToDefaultHardCoding()
{
	this->Release();

	m_pSky = new CN3Sky();
	m_pSky->Init();

	m_pMoon = new CN3Moon();
	m_pMoon->Init("misc\\sky\\phases.tga");

	std::string szSuns[NUM_SUNPART] = { "misc\\sky\\sundisk.bmp", "misc\\sky\\sunglow.bmp",	"misc\\sky\\sunflare.bmp" };
	m_pSun = new CN3Sun();
	m_pSun->Init(szSuns);

	std::string szClouds[NUM_CLOUD] = { "misc\\sky\\wisps.tga", "misc\\sky\\puffs.tga", "misc\\sky\\tatters.tga", "misc\\sky\\streaks.tga", "misc\\sky\\dense.tga", "misc\\sky\\overcast.tga" };
	m_pCloud = new CN3Cloud();
	m_pCloud->Init(szClouds);

	m_pStar = new CN3Star();
	m_pStar->Init();

	for(int i = 0; i < MAX_GAME_LIGHT; i++)
	{
		m_pLightColorDiffuses[i] = new CN3ColorChange();
		m_pLightColorAmbients[i] = new CN3ColorChange();
	}

	// �ӽ� hard coding
	__SKY_DAYCHANGE tmpDayChange;
	m_DayChanges.reserve(64);
	DWORD dwTime = 0;

	// �ض߱�..
	dwTime = CONVERT_SEC(5,0,0); // 5�ÿ� �ذ� ���..
	
	tmpDayChange.Init("SunRise - SkyColor", SDC_SKYCOLOR, dwTime, D3DCOLOR_ARGB(255, 165, 115, 85), 0, 180.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("SunRise - FogColor", SDC_FOGCOLOR, dwTime, D3DCOLOR_ARGB(255, 80, 110, 160), 0, 180.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("SunRise - SunColor", SDC_SUNCOLOR, dwTime, D3DCOLOR_ARGB(255, 165, 115, 85), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("SunRise - GlowColor", SDC_GLOWCOLOR, dwTime, D3DCOLOR_ARGB(255, 165, 122, 110), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("SunRise - FlareColor", SDC_FLARECOLOR, dwTime, D3DCOLOR_ARGB(255, 122, 80, 50), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("SunRise - Cloud1Color", SDC_CLOUD1COLOR, dwTime, D3DCOLOR_ARGB(255, 165, 115, 85), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("SunRise - Cloud2Color", SDC_CLOUD2COLOR, dwTime, D3DCOLOR_ARGB(255, 165, 115, 85), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("SunRise - Light0", SDC_LIGHT0COLOR, dwTime, D3DCOLOR_ARGB(255, 165, 115, 85), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("SunRise - Light1", SDC_LIGHT1COLOR, dwTime, D3DCOLOR_ARGB(255, 165, 115, 85), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("SunRise - Light2", SDC_LIGHT2COLOR, dwTime, D3DCOLOR_ARGB(255, 165, 115, 85), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);


	// �ѳ�..
	dwTime = CONVERT_SEC(6,0,0); // 6�ú��� ���̴�..

	tmpDayChange.Init("Noon - StarCount", SDC_STARCOUNT, dwTime, 0, 0, 300.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("Noon - SkyColor", SDC_SKYCOLOR, dwTime, D3DCOLOR_ARGB(255, 46, 97, 189), 0, 180.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("Noon - FogColor", SDC_FOGCOLOR, dwTime, D3DCOLOR_ARGB(255, 169, 203, 215), 0, 180.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("Noon - SunColor", SDC_SUNCOLOR, dwTime, D3DCOLOR_ARGB(255, 255, 255, 255), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("Noon - GlowColor", SDC_GLOWCOLOR, dwTime, D3DCOLOR_ARGB(255, 224, 224, 192), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("Noon - FlareColor", SDC_FLARECOLOR, dwTime, D3DCOLOR_ARGB(255, 96, 96, 96), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("Noon - Cloud1Color", SDC_CLOUD1COLOR, dwTime, D3DCOLOR_ARGB(255, 255, 255, 255), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("Noon - Cloud2Color", SDC_CLOUD2COLOR, dwTime, D3DCOLOR_ARGB(255, 255, 255, 255), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("Noon - Light0", SDC_LIGHT0COLOR, dwTime, D3DCOLOR_ARGB(255, 255, 255, 255), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("Noon - Light1", SDC_LIGHT1COLOR, dwTime, D3DCOLOR_ARGB(255, 255, 255, 255), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("Noon - Light2", SDC_LIGHT2COLOR, dwTime, D3DCOLOR_ARGB(255, 255, 255, 255), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);


	tmpDayChange.Init("Noon - MoonPhase", SDC_MOONPHASE, CONVERT_SEC(12,0,0), 0, 0, 0); // �� 12�ÿ� �� ��� ��ȭ
	m_DayChanges.push_back(tmpDayChange);


	// ������..
	dwTime = CONVERT_SEC(20,0,0); // ���� 8�ÿ� �ذ� ���� �����Ѵ�..

	tmpDayChange.Init("SunSet - SkyColor", SDC_SKYCOLOR, dwTime, D3DCOLOR_ARGB(255, 98, 115, 125), 0, 180.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("SunSet - FogColor", SDC_FOGCOLOR, dwTime, D3DCOLOR_ARGB(255, 135, 162, 159), 0, 180.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("SunSet - SunColor", SDC_SUNCOLOR, dwTime, D3DCOLOR_ARGB(255, 203, 132, 85), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("SunSet - GlowColor", SDC_GLOWCOLOR, dwTime, D3DCOLOR_ARGB(255, 192, 128, 128), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("SunSet - FlareColor", SDC_FLARECOLOR, dwTime, D3DCOLOR_ARGB(255, 204, 128, 40), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("SunSet - Cloud1Color", SDC_CLOUD1COLOR, dwTime, D3DCOLOR_ARGB(255, 182, 152, 116), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("SunSet - Cloud2Color", SDC_CLOUD2COLOR, dwTime, D3DCOLOR_ARGB(255, 162, 145, 107), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("SunSet - Light0", SDC_LIGHT0COLOR, dwTime, D3DCOLOR_ARGB(255, 135, 162, 159), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("SunSet - Light1", SDC_LIGHT1COLOR, dwTime, D3DCOLOR_ARGB(255, 135, 162, 159), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("SunSet - Light2", SDC_LIGHT2COLOR, dwTime, D3DCOLOR_ARGB(255, 135, 162, 159), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);



	// ������ �ذ� ��..
	dwTime = CONVERT_SEC(21,0,0); // ���� 9�ÿ� ������ �ذ� ����.

	tmpDayChange.Init("MidNight - SkyColor", SDC_SKYCOLOR, dwTime, D3DCOLOR_ARGB(255, 15, 18, 49), 0, 180.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("MidNight - FogColor", SDC_FOGCOLOR, dwTime, D3DCOLOR_ARGB(255, 39, 52, 95), 0, 180.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("MidNight - StarCount", SDC_STARCOUNT, dwTime, MAX_STAR, 0, 300.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("MidNight - SunColor", SDC_SUNCOLOR, dwTime, D3DCOLOR_ARGB(255, 30, 36, 85), 0, 150.0f); // Ǫ������
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("MidNight - GlowColor", SDC_GLOWCOLOR, dwTime, D3DCOLOR_ARGB(255, 15, 18, 49), 0, 150.0f); // �ϴð� ��
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("MidNight - FlareColor", SDC_FLARECOLOR, dwTime, D3DCOLOR_ARGB(255, 15, 18, 49), 0, 150.0f); // �ϴð� ��
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("MidNight - Cloud1Color", SDC_CLOUD1COLOR, dwTime, D3DCOLOR_ARGB(255, 151, 144, 160), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("MidNight - Cloud2Color", SDC_CLOUD2COLOR, dwTime, D3DCOLOR_ARGB(255, 79, 78, 92), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("MidNight - Light0", SDC_LIGHT0COLOR, dwTime, D3DCOLOR_ARGB(255, 15, 18, 49), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("MidNight - Light1", SDC_LIGHT1COLOR, dwTime, D3DCOLOR_ARGB(255, 15, 18, 49), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	tmpDayChange.Init("MidNight - Light2", SDC_LIGHT2COLOR, dwTime, D3DCOLOR_ARGB(255, 15, 18, 49), 0, 150.0f);
	m_DayChanges.push_back(tmpDayChange);

	
	std::vector<__SKY_DAYCHANGE>(m_DayChanges).swap(m_DayChanges); // �뷮�� �� ���߱�..

	qsort(&(m_DayChanges[0]), m_DayChanges.size(), sizeof(__SKY_DAYCHANGE), CompareTime);
	SetCheckGameTime(CONVERT_SEC(10,0,0));
}
#endif // #ifdef _N3TOOL


//	CheckGameTime�� �����ְ� ����ð��� �ٽ� �����Ѵ�.(Ư�� �ð����� ���������� ���鶧 ȣ���Ѵ�.)
void CN3SkyMng::SetCheckGameTime(DWORD dwCheckGameTime)
{
	dwCheckGameTime %= 86400;
	DWORD dwCheckTick = timeGetTime();
	m_dwCheckGameTime = dwCheckGameTime;
	m_dwCheckTick = dwCheckTick;

	// �ؿ� ���� ���� ���.
	m_pSun->SetCurAngle(dwCheckGameTime/86400.0f*360.0f  + 270.0f);
	m_pMoon->SetCurAngle(dwCheckGameTime/86400.0f*360.0f  + 90.0f);

	if (m_DayChanges.empty()) return;

	// ť���� ���� ���ӽð��� �´� ������ ã�´�.
	m_iDayChangeCurPos = 0;
	int iDCC = m_DayChanges.size();
	while(m_iDayChangeCurPos<iDCC &&
		m_DayChanges[m_iDayChangeCurPos].dwWhen < dwCheckGameTime) ++m_iDayChangeCurPos;
	if (m_iDayChangeCurPos >= iDCC) m_iDayChangeCurPos = iDCC - 1;

	// ���� ���ӽð����� �� sky���º��� ���� �ֱٿ� ����� ���� ã�Ƽ� ���� �������ش�.
	int i;
	for(i=0; i<NUM_SKYDAYCHANGE; ++i)
	{
		if (i == SDC_MOONPHASE) continue;

		int iPos = GetLatestChange((eSKY_DAYCHANGE)i, m_iDayChangeCurPos);
		if (iPos<0) continue;	// �ѹ����� �� ���Ҵµ��� ��ȭ���� ã�� �� ����.
		__SKY_DAYCHANGE* pSDC = &(m_DayChanges[iPos]);
		DWORD dwEnd = pSDC->dwWhen + (DWORD)(TIME_REAL_PER_GAME * pSDC->fHowLong);	// ��ȭ�� ������ �ð�
		if (dwEnd>86400) dwEnd -= 86400;	// 24�ð��� �Ѿ������ 24�ð��� ���ش�.
		if ( dwEnd < dwCheckGameTime)
		{	// ���� �׽ð����� ��ȭ�� �̹� ������ ���
			ChangeSky(pSDC, 0.0f);
		}
		else
		{	// ���� �׽ð����� ��ȭ�� �������� ���
			int iPrevPos = GetLatestChange((eSKY_DAYCHANGE)i, iPos);
			__ASSERT(iPrevPos>=0, "���⿡ �ø��� ����");
			__SKY_DAYCHANGE* pPrevSDC = &(m_DayChanges[iPrevPos]);
			ChangeSky(pPrevSDC, 0.0f);	// ���� ���·� �����

			// ��ȭ���·� �����
			// ������ ������ �ð��� ���� �ð� ����
			DWORD dwDiffTime = dwCheckGameTime - pSDC->dwWhen;
			// ��ȭ�� �ɸ��� �ð� ���� 
			float fTakeTime = pSDC->fHowLong - dwDiffTime*TIME_GAME_PER_REAL;	// �ɸ��� �ð�
			if (fTakeTime<0.0f) fTakeTime = 0.0f;	// 0���� ������ ��� ��ȭ�ϰ� ����
			ChangeSky(pSDC, fTakeTime);
		}
	}
}

// m_DayChanges���� ������ ��ġ(iPos) ������ ���� �ֱٿ� ��ȭ�ϴ� ��ġ ������
int CN3SkyMng::GetLatestChange(eSKY_DAYCHANGE eSDC, int iPos)
{
	int iFind = iPos-1;
	if (iFind<0 || iFind >= m_DayChanges.size()) iFind = m_DayChanges.size()-1;
	while(iFind>=0)
	{
		if (m_DayChanges[iFind].eSkyDayChange == eSDC) break;	// ���� �ֱ��� ��ȭ�� ã�Ҵ�.
		--iFind;
	}

	if (iFind<0)
	{
		// �� �ڿ������� �ٽ� �˻�
		iFind = m_DayChanges.size()-1;
		while(iFind>=0)
		{
			if (m_DayChanges[iFind].eSkyDayChange == eSDC) break;	// ���� �ֱ��� ��ȭ�� ã�Ҵ�.
			if (iPos > iFind) {iFind = -1; break;}	// �ѹ����� �� ���Ҵµ��� ��ȭ���� ã�� �� ����.
			--iFind;
		}
	}
	return iFind;
}

// �ϴ��� ���¸� �ٲٱ� (pSDC->fHowLong ���� fTakeTime���� ���Ͽ� ��Ȯ�� ��ȭ���� ����Ͽ� �ش�.)
void CN3SkyMng::ChangeSky(__SKY_DAYCHANGE* pSDC, float fTakeTime)
{
	if (NULL == pSDC) return;
	float fPercentage = 1.0f - fTakeTime/pSDC->fHowLong;

	// �ϴ� ��ȭ���� �����ϱ�
	switch(pSDC->eSkyDayChange)
	{
	case SDC_SKYCOLOR:
		m_pSky->m_SkyColor.ChangeColor(pSDC->dwParam1, pSDC->fHowLong);
		m_pSky->m_SkyColor.SetPercentage(fPercentage);
		break;
	case SDC_FOGCOLOR:
		m_pSky->m_FogColor.ChangeColor(pSDC->dwParam1, pSDC->fHowLong);
		m_pSky->m_FogColor.SetPercentage(fPercentage);
		break;
	case SDC_STARCOUNT:
		m_pStar->SetStar(pSDC->dwParam1, fTakeTime);
		break;
	case SDC_MOONPHASE:
		m_pMoon->SetMoonPhase(m_pMoon->GetMoonPhaseIndex()+1);	// phase index�� �ϳ� ������Ų��.
		break;
	case SDC_SUNCOLOR:
		m_pSun->m_Parts[SUNPART_SUN].Color.ChangeColor(pSDC->dwParam1, pSDC->fHowLong); // ��
		m_pSun->m_Parts[SUNPART_SUN].Color.SetPercentage(fPercentage);
		m_pSun->m_Parts[SUNPART_SUN].Delta.ChangeDelta(pSDC->dwParam2/1000.0f, pSDC->fHowLong); // ũ��..
		m_pSun->m_Parts[SUNPART_SUN].Delta.SetPercentage(fPercentage);
		break;
	case SDC_GLOWCOLOR:
		m_pSun->m_Parts[SUNPART_GLOW].Color.ChangeColor(pSDC->dwParam1, pSDC->fHowLong); // ��
		m_pSun->m_Parts[SUNPART_GLOW].Color.SetPercentage(fPercentage);
		m_pSun->m_Parts[SUNPART_GLOW].Delta.ChangeDelta(pSDC->dwParam2/1000.0f, pSDC->fHowLong); // ũ��..
		m_pSun->m_Parts[SUNPART_GLOW].Delta.SetPercentage(fPercentage);
		break;
	case SDC_FLARECOLOR:
		m_pSun->m_Parts[SUNPART_FLARE].Color.ChangeColor(pSDC->dwParam1, pSDC->fHowLong); // ��
		m_pSun->m_Parts[SUNPART_FLARE].Color.SetPercentage(fPercentage);
		m_pSun->m_Parts[SUNPART_FLARE].Delta.ChangeDelta(pSDC->dwParam2/1000.0f, pSDC->fHowLong); // ũ��..
		m_pSun->m_Parts[SUNPART_FLARE].Delta.SetPercentage(fPercentage);
		break;
	case SDC_CLOUD1COLOR:
		m_pCloud->m_Color1.ChangeColor(pSDC->dwParam1, pSDC->fHowLong);
		m_pCloud->m_Color1.SetPercentage(fPercentage);
		break;
	case SDC_CLOUD2COLOR:
		m_pCloud->m_Color2.ChangeColor(pSDC->dwParam1, pSDC->fHowLong);
		m_pCloud->m_Color2.SetPercentage(fPercentage);
		break;
	case SDC_CLOUDTEX:
		m_pCloud->SetCloud((e_CLOUDTEX)pSDC->dwParam1, (e_CLOUDTEX)pSDC->dwParam2, fTakeTime);
		break;
	case SDC_LIGHT0COLOR:
		m_pLightColorDiffuses[0]->ChangeColor(pSDC->dwParam1, pSDC->fHowLong);
		m_pLightColorDiffuses[0]->SetPercentage(fPercentage);
		m_pLightColorAmbients[0]->ChangeColor(pSDC->dwParam2, pSDC->fHowLong);
		m_pLightColorAmbients[0]->SetPercentage(fPercentage);
		break;
	case SDC_LIGHT1COLOR:
		m_pLightColorDiffuses[1]->ChangeColor(pSDC->dwParam1, pSDC->fHowLong);
		m_pLightColorDiffuses[1]->SetPercentage(fPercentage);
		m_pLightColorAmbients[1]->ChangeColor(pSDC->dwParam2, pSDC->fHowLong);
		m_pLightColorAmbients[1]->SetPercentage(fPercentage);
		break;
	case SDC_LIGHT2COLOR:
		m_pLightColorDiffuses[2]->ChangeColor(pSDC->dwParam1, pSDC->fHowLong);
		m_pLightColorDiffuses[2]->SetPercentage(fPercentage);
		m_pLightColorAmbients[2]->ChangeColor(pSDC->dwParam2, pSDC->fHowLong);
		m_pLightColorAmbients[2]->SetPercentage(fPercentage);
		break;
	}
}

float CN3SkyMng::GetSunAngleByRadin()
{
	if(m_pSun) return m_pSun->m_fCurRadian;
	else return 0;
}

int CN3SkyMng::CompareTime(const void* pArg1, const void* pArg2)
{
	const __SKY_DAYCHANGE* pSDC1 = (__SKY_DAYCHANGE*)pArg1;
	const __SKY_DAYCHANGE* pSDC2 = (__SKY_DAYCHANGE*)pArg2;
	return (int)pSDC1->dwWhen - (int)pSDC2->dwWhen;
}

void CN3SkyMng::SetWeather(eSKY_WEATHER eWeather, int iPercentage)
{
	// ������ ���ϴ� �͵� : SDC_SKYCOLOR, SDC_FOGCOLOR, SDC_CLOUD1COLOR, SDC_CLOUD2COLOR, SDC_CLOUDTEX
	eSKY_WEATHER ePrevWeather = m_eWeather;	// ���� ����
	m_eWeather = eWeather;

	// ���� ���� �ð� ���ϱ�
	DWORD dwCurTickCount = timeGetTime();
	__ASSERT(dwCurTickCount >= m_dwCheckTick,"�����̴�.");
	DWORD dwCurGameTime = m_dwCheckGameTime + (DWORD)((dwCurTickCount - m_dwCheckTick)*TIME_REAL_PER_GAME*0.001f);
	BOOL	IsNight = (dwCurGameTime < CONVERT_SEC(6,0,0) || dwCurGameTime > CONVERT_SEC(19,0,0));

	if(SW_CLEAR == m_eWeather) // ���� ����. �ۼ�Ʈ�� �Ȱ�...
	{
		if (SW_CLEAR == ePrevWeather) return;	// ���� ������ �������� ��ȭ��Ű�� �ʴ´�.
		int iAfterNSecPos = GetDayChangePos_AfterNSec(dwCurGameTime, 10);	// 60�� �Ŀ� DayChangePos���ϱ�
		float fHowLong = 10.0f;
		DWORD dwWhen = dwCurGameTime + CONVERT_SEC(0, 0,0);
		DWORD dwParam1 = 0, dwParam2 = 0;

		// ���� ��ȭ ť �����
		m_WeatherChanges.clear();
		m_iWeatherChangeCurPos = 0;
		m_WeatherChanges.reserve(16);
		__SKY_DAYCHANGE tmpWeatherChange;
		int iPos = 0;

		// ��
		iPos = GetLatestChange(SDC_SUNCOLOR, iAfterNSecPos); // N�� �Ŀ� ���� �ֱٿ� ���� �ϴ� ��ȭ ��ġ ���ϱ�
		if (iPos>=0) { dwParam1 = m_DayChanges[iPos].dwParam1; dwParam2 = m_DayChanges[iPos].dwParam2; }
		else { dwParam1 = D3DCOLOR_ARGB(255, 255, 255, 255); dwParam2 = 100; }
		tmpWeatherChange.Init("Sun", SDC_SUNCOLOR, dwWhen, dwParam1, dwParam2, fHowLong);
		m_WeatherChanges.push_back(tmpWeatherChange);

		// ���� flare
		iPos = GetLatestChange(SDC_FLARECOLOR, iAfterNSecPos); // N�� �Ŀ� ���� �ֱٿ� ���� �ϴ� ��ȭ ��ġ ���ϱ�
		if (iPos>=0) { dwParam1 = m_DayChanges[iPos].dwParam1; dwParam2 = m_DayChanges[iPos].dwParam2; }
		else { dwParam1 = D3DCOLOR_ARGB(255, 255, 255, 255); dwParam2 = 100; }
		tmpWeatherChange.Init("Flare", SDC_FLARECOLOR, dwWhen, dwParam1, dwParam2, fHowLong);
		m_WeatherChanges.push_back(tmpWeatherChange);

		// ���� glow
		iPos = GetLatestChange(SDC_GLOWCOLOR, iAfterNSecPos); // N�� �Ŀ� ���� �ֱٿ� ���� �ϴ� ��ȭ ��ġ ���ϱ�
		if (iPos>=0) { dwParam1 = m_DayChanges[iPos].dwParam1; dwParam2 = m_DayChanges[iPos].dwParam2; }
		else { dwParam1 = D3DCOLOR_ARGB(255, 255, 255, 255); dwParam2 = 100; }
		tmpWeatherChange.Init("Glow", SDC_GLOWCOLOR, dwWhen, dwParam1, dwParam2, fHowLong);
		m_WeatherChanges.push_back(tmpWeatherChange);

		// �ϴû�
		iPos = GetLatestChange(SDC_SKYCOLOR, iAfterNSecPos); // N�� �Ŀ� ���� �ֱٿ� ���� �ϴ� ��ȭ ��ġ ���ϱ�
		if (iPos>=0) { dwParam1 = m_DayChanges[iPos].dwParam1; dwParam2 = m_DayChanges[iPos].dwParam2; }
		else { dwParam1 = D3DCOLOR_ARGB(255, 255, 255, 255); dwParam2 = 100; }
		tmpWeatherChange.Init("Sky", SDC_SKYCOLOR, dwWhen, dwParam1, dwParam2, fHowLong);
		m_WeatherChanges.push_back(tmpWeatherChange);

		// �Ȱ���
		iPos = GetLatestChange(SDC_FOGCOLOR, iAfterNSecPos); // N�� �Ŀ� ���� �ֱٿ� ���� �ϴ� ��ȭ ��ġ ���ϱ�
		if (iPos>=0) { dwParam1 = m_DayChanges[iPos].dwParam1; dwParam2 = m_DayChanges[iPos].dwParam2; }
		else { dwParam1 = D3DCOLOR_ARGB(255, 255, 255, 255); dwParam2 = 100; }
		tmpWeatherChange.Init("Fog", SDC_FOGCOLOR, dwWhen, dwParam1, dwParam2, fHowLong);
		m_WeatherChanges.push_back(tmpWeatherChange);

		// ���� 1 ��
		iPos = GetLatestChange(SDC_CLOUD1COLOR, iAfterNSecPos); // N�� �Ŀ� ���� �ֱٿ� ���� �ϴ� ��ȭ ��ġ ���ϱ�
		if (iPos>=0) { dwParam1 = m_DayChanges[iPos].dwParam1; dwParam2 = m_DayChanges[iPos].dwParam2; }
		else { dwParam1 = D3DCOLOR_ARGB(255, 255, 255, 255); dwParam2 = 100; }
		tmpWeatherChange.Init("Cloud1", SDC_CLOUD1COLOR, dwWhen, dwParam1, dwParam2, fHowLong);
		m_WeatherChanges.push_back(tmpWeatherChange);

		// ���� 2�� 
		iPos = GetLatestChange(SDC_CLOUD2COLOR, iAfterNSecPos); // N�� �Ŀ� ���� �ֱٿ� ���� �ϴ� ��ȭ ��ġ ���ϱ�
		if (iPos>=0) { dwParam1 = m_DayChanges[iPos].dwParam1; dwParam2 = m_DayChanges[iPos].dwParam2; }
		else { dwParam1 = D3DCOLOR_ARGB(255, 255, 255, 255); dwParam2 = 100; }
		tmpWeatherChange.Init("Cloud2", SDC_CLOUD2COLOR, dwWhen, dwParam1, dwParam2, fHowLong);
		m_WeatherChanges.push_back(tmpWeatherChange);
		
		// ���� �ؽ��� - �̸� �ٲ�� �Ѵ�..
		iPos = GetLatestChange(SDC_CLOUDTEX, iAfterNSecPos); // N�� �Ŀ� ���� �ֱٿ� ���� �ϴ� ��ȭ ��ġ ���ϱ�
		if (iPos>=0) { dwParam1 = m_DayChanges[iPos].dwParam1; dwParam2 = m_DayChanges[iPos].dwParam2; }
		else { dwParam1 = CLOUD_WISPS; dwParam2 = CLOUD_PUFFS; }
		tmpWeatherChange.Init("CloudTex", SDC_CLOUDTEX, dwCurGameTime, dwParam1, dwParam2, 10.0f);
		m_WeatherChanges.push_back(tmpWeatherChange);

		// ����Ʈ 0
		iPos = GetLatestChange(SDC_LIGHT0COLOR, iAfterNSecPos); // N�� �Ŀ� ���� �ֱٿ� ���� �ϴ� ��ȭ ��ġ ���ϱ�
		if (iPos>=0) { dwParam1 = m_DayChanges[iPos].dwParam1; dwParam2 = m_DayChanges[iPos].dwParam2; }
		else { dwParam1 = D3DCOLOR_ARGB(255, 255, 255, 255); dwParam2 = 100; }
		tmpWeatherChange.Init("Light0", SDC_LIGHT0COLOR, dwWhen, dwParam1, dwParam2, fHowLong);
		m_WeatherChanges.push_back(tmpWeatherChange);
		
		// ����Ʈ 1
		iPos = GetLatestChange(SDC_LIGHT1COLOR, iAfterNSecPos); // N�� �Ŀ� ���� �ֱٿ� ���� �ϴ� ��ȭ ��ġ ���ϱ�
		if (iPos>=0) { dwParam1 = m_DayChanges[iPos].dwParam1; dwParam2 = m_DayChanges[iPos].dwParam2; }
		else { dwParam1 = D3DCOLOR_ARGB(255, 255, 255, 255); dwParam2 = 100; }
		tmpWeatherChange.Init("Light1", SDC_LIGHT1COLOR, dwWhen, dwParam1, dwParam2, fHowLong);
		m_WeatherChanges.push_back(tmpWeatherChange);

		// ����Ʈ 2
		iPos = GetLatestChange(SDC_LIGHT2COLOR, iAfterNSecPos); // N�� �Ŀ� ���� �ֱٿ� ���� �ϴ� ��ȭ ��ġ ���ϱ�
		if (iPos>=0) { dwParam1 = m_DayChanges[iPos].dwParam1; dwParam2 = m_DayChanges[iPos].dwParam2; }
		else { dwParam1 = D3DCOLOR_ARGB(255, 255, 255, 255); dwParam2 = 100; }
		tmpWeatherChange.Init("Light2", SDC_LIGHT2COLOR, dwWhen, dwParam1, dwParam2, fHowLong);
		m_WeatherChanges.push_back(tmpWeatherChange);


		// ������� ���� ������..
		iAfterNSecPos = GetDayChangePos_AfterNSec(dwCurGameTime, 10);	// 10�� �Ŀ� DayChangePos���ϱ�

		std::vector<__SKY_DAYCHANGE>(m_WeatherChanges).swap(m_WeatherChanges); // �뷮�� �� ���߱�..
		qsort(&(m_WeatherChanges[0]), m_WeatherChanges.size(), sizeof(__SKY_DAYCHANGE), CompareTime);


		// ���� ����~
		if (m_pGESnow) m_pGESnow->FadeSet(3.0f, false);
		if (m_pGERain) m_pGERain->FadeSet(3.0f, false);
#ifdef _N3GAME // ������ �ƴ� �������� �ʿ����...
		if (m_pSnd_Weather_Rain) m_pSnd_Weather_Rain->Stop(5.0f);
		if (m_pSnd_Weather_Snow) m_pSnd_Weather_Snow->Stop(5.0f);
#endif // #ifdef _N3GAME
	}
	else if(SW_RAINY == m_eWeather || SW_SNOW == m_eWeather) // ��, �� ��
	{
		if (SW_CLEAR != ePrevWeather) return;	// ���� ������ �������� �ƴϸ� ��ȭ��Ű�� �ʴ´�.
		float fHowLong = 10.0f;

		// ���� ��ȭ ť �����
		m_WeatherChanges.clear();
		m_iWeatherChangeCurPos = 0;
		m_WeatherChanges.reserve(16);
		__SKY_DAYCHANGE tmpWeatherChange;
		__ColorValue crTmp1, crTmp2;
		float fDelta = (0.5f + 0.3f * (100 - iPercentage) / 100.0f);
		DWORD dwWhen = dwCurGameTime + CONVERT_SEC(0, 0, 10);

		// ����Ʈ ���� ���� ���ϱ�..
		__ColorValue crLgt(1,1,1,1);
		int iAfterNSecPos = GetDayChangePos_AfterNSec(dwCurGameTime, 10);	// 60�� �Ŀ� DayChangePos���ϱ�
		int iPos = GetLatestChange(SDC_LIGHT1COLOR, iAfterNSecPos); // ������ �����ش� ��
		if (iPos>=0) { crLgt = m_DayChanges[iPos].dwParam1; }
		float fDelta2 = (crLgt.r + crLgt.g + crLgt.b) / 3.0f;
		fDelta2 = 1.0f - fDelta2 * 0.8f * (iPercentage/100.0f);


		// ��
		tmpWeatherChange.Init("Sun", SDC_SUNCOLOR, dwWhen, 0xff000000, this->GetSunRatio() * 1000, fHowLong);
		m_WeatherChanges.push_back(tmpWeatherChange);

		// ���� flare
		tmpWeatherChange.Init("Flare", SDC_FLARECOLOR, dwWhen, 0xff000000, this->GetFlareRatio() * 1000, fHowLong);
		m_WeatherChanges.push_back(tmpWeatherChange);

		// ���� glow
		tmpWeatherChange.Init("Glow", SDC_GLOWCOLOR, dwWhen, 0xff000000, this->GetGlowRatio() * 1000, fHowLong);
		m_WeatherChanges.push_back(tmpWeatherChange);

		//  �ϴû�
		crTmp1 = this->GetSkyColor(); crTmp1 *= fDelta; crTmp1.a = 1.0f;
		tmpWeatherChange.Init("Sky", SDC_SKYCOLOR, dwWhen, crTmp1.ToD3DCOLOR(), 0, fHowLong);
		m_WeatherChanges.push_back(tmpWeatherChange);

		//  �Ȱ���
		crTmp1 = this->GetFogColor(); crTmp1 *= fDelta2; crTmp1.a = 1.0f;
		tmpWeatherChange.Init("Fog", SDC_FOGCOLOR, dwWhen, crTmp1.ToD3DCOLOR(), 0, fHowLong);
		m_WeatherChanges.push_back(tmpWeatherChange);

		// ������1
		crTmp1 = this->GetCloud1Color(); crTmp1 *= fDelta * 0.75f; crTmp1.a = 1.0f;
		tmpWeatherChange.Init("Cloud1Color", SDC_CLOUD1COLOR, dwWhen, crTmp1.ToD3DCOLOR(), 0, fHowLong);
		m_WeatherChanges.push_back(tmpWeatherChange);

		// ������2 - ���� ��Ӱ� �Ѵ�..
		crTmp1 = this->GetCloud2Color(); crTmp1 *= fDelta * 0.5f; crTmp1.a = 1.0f;
		tmpWeatherChange.Init("Cloud2Color", SDC_CLOUD2COLOR, dwWhen, crTmp1.ToD3DCOLOR(), 0, fHowLong);
		m_WeatherChanges.push_back(tmpWeatherChange);
		
		// ���� �ؽ���
		DWORD dwTex1, dwTex2;
		if (iPercentage>80)			{ dwTex1 = CLOUD_OVERCAST;	dwTex2 = CLOUD_DENSE; }
		else if (iPercentage>60)	{ dwTex1 = CLOUD_DENSE;		dwTex2 = CLOUD_STREAKS; }
		else if (iPercentage>40)	{ dwTex1 = CLOUD_STREAKS;	dwTex2 = CLOUD_TATTERS; }
		else { dwTex1 = CLOUD_TATTERS; dwTex2 = CLOUD_PUFFS; }
		tmpWeatherChange.Init("CloudTex", SDC_CLOUDTEX, dwCurGameTime, dwTex1, dwTex2, 10.0f);
		m_WeatherChanges.push_back(tmpWeatherChange);

		// ����Ʈ 0
		crTmp1 = this->GetLightDiffuseColor(0); crTmp1 *= fDelta2; crTmp1.a = 1.0f;
		crTmp2 = this->GetLightAmbientColor(0); crTmp2 *= fDelta2; crTmp2.a = 1.0f;
		tmpWeatherChange.Init("Light0", SDC_LIGHT0COLOR, dwWhen, crTmp1.ToD3DCOLOR(), crTmp2.ToD3DCOLOR(), fHowLong);
		m_WeatherChanges.push_back(tmpWeatherChange);
		
		// ����Ʈ 1
		crTmp1 = this->GetLightDiffuseColor(1); crTmp1 *= fDelta2; crTmp1.a = 1.0f;
		crTmp2 = this->GetLightAmbientColor(1); crTmp2 *= fDelta2; crTmp2.a = 1.0f;
		tmpWeatherChange.Init("Light1", SDC_LIGHT1COLOR, dwWhen, crTmp1.ToD3DCOLOR(), crTmp2.ToD3DCOLOR(), fHowLong);
		m_WeatherChanges.push_back(tmpWeatherChange);
		
		// ����Ʈ 2
		crTmp1 = this->GetLightDiffuseColor(2); crTmp1 *= fDelta2; crTmp1.a = 1.0f;
		crTmp2 = this->GetLightAmbientColor(2); crTmp2 *= fDelta2; crTmp2.a = 1.0f;
		tmpWeatherChange.Init("Light2", SDC_LIGHT2COLOR, dwWhen, crTmp1.ToD3DCOLOR(), crTmp2.ToD3DCOLOR(), fHowLong);
		m_WeatherChanges.push_back(tmpWeatherChange);


		
		std::vector<__SKY_DAYCHANGE>(m_WeatherChanges).swap(m_WeatherChanges); // �뷮�� �� ���߱�..
		qsort(&(m_WeatherChanges[0]), m_WeatherChanges.size(), sizeof(m_WeatherChanges[0]), CompareTime);


		float fHeight = 20.0f;
		float fPercent = iPercentage / 100.0f;
//		float fDensity = fPercent * 0.1f;
		float fDensity = fPercent * 0.03f;
		if(SW_RAINY == m_eWeather)
		{
			if (m_pGESnow) m_pGESnow->FadeSet(3.0f, false);
			if (m_pGERain == NULL) m_pGERain = new CN3GERain;

			__Vector3 vVelocity(3.0f * ((50-rand()%100) / 50.0f), -(10.0f + 8.0f * fPercent), 0);
			float fRainLength = 0.4f + 0.6f * fPercent;

			m_fCellSize = 20.0f;
			m_pGERain->Create(fDensity, m_fCellSize, fHeight, fRainLength, vVelocity, 10.0f);	// ��
			m_pGERain->SetActive(TRUE);

#ifdef _N3GAME // ������ �ƴ� �������� �ʿ����...
			if(m_pSnd_Weather_Snow) m_pSnd_Weather_Snow->Stop(5.0f);
			CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Weather_Rain);
			m_pSnd_Weather_Rain = CN3Base::s_SndMgr.CreateObj(201);	// ����� �Ҹ�..
			if(m_pSnd_Weather_Rain)
			{
				int iMaxVol = (int)(70 * fPercent);
				m_pSnd_Weather_Rain->SetMaxVolume(iMaxVol);
				m_pSnd_Weather_Rain->Looping(true);
				m_pSnd_Weather_Rain->Play(NULL, 0.0f, 2.0f);
			}
#endif // #ifdef _N3GAME
		}
		else if(SW_SNOW == m_eWeather)
		{
			if (m_pGERain) m_pGERain->FadeSet(3.0f, false);
			if (m_pGESnow == NULL) m_pGESnow = new CN3GESnow;

			float fHorz = (3.0f * fPercent) + (3.0f * ((50-rand()%100) / 50.0f));
			__Vector3 vVelocity(fHorz, -(2.0f + 2.0f * fPercent), 0);
			float fSnowSize = 0.1f + 0.1f * fPercent;

			m_fCellSize = 20.0f;
			m_pGESnow->Create(fDensity, m_fCellSize, fHeight, fSnowSize, vVelocity, 10.0f);	// ��
			m_pGESnow->SetActive(TRUE);

#ifdef _N3GAME // ������ �ƴ� �������� �ʿ����...
			if(m_pSnd_Weather_Rain) m_pSnd_Weather_Rain->Stop(5.0f);
			CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Weather_Snow);
			m_pSnd_Weather_Snow = CN3Base::s_SndMgr.CreateObj(200);	// �ٶ��Ҹ�..
			if(m_pSnd_Weather_Snow)
			{
				int iMaxVol = (int)(70 * fPercent);
				m_pSnd_Weather_Snow->SetMaxVolume(iMaxVol);
				m_pSnd_Weather_Snow->Looping(true);
				m_pSnd_Weather_Snow->Play(NULL, 0.0f, 2.0f);
			}
#endif // #ifdef _N3GAME
		}
	}
}

void CN3SkyMng::SunAndMoonDirectionFixByHour(int iHour) // �ؿ� �� ���� ����
{
	m_iHourFix = iHour;
}

void CN3SkyMng::GetGameTime(int* piYear, int* piMonth, int* piDay, int* piHour, int*piMin)
{
	DWORD dwCurTickCount = timeGetTime();
	__ASSERT(dwCurTickCount >= m_dwCheckTick,"�����̴�.");
	DWORD dwCurGameTime = m_dwCheckGameTime + (DWORD)((dwCurTickCount - m_dwCheckTick)*TIME_REAL_PER_GAME*0.001f);

	// dwCurGameTime - ��
	int iSecond = dwCurGameTime%3600;

	if(piYear)	*piYear = m_iYear;
	if(piMonth) *piMonth = m_iMonth;
	if(piDay)	*piDay = m_iDay;
	if(piHour)	*piHour = dwCurGameTime/3600;
	if(piMin)	*piMin = iSecond/60;
}

void CN3SkyMng::SetGameTime(int iYear, int iMonth, int iDay, int iHour, int iMin)
{
	m_iYear = iYear;
	m_iMonth = iMonth;
	m_iDay = iDay;

	__ASSERT(m_pMoon,"null pointer");
	m_pMoon->SetMoonPhase(iMonth*30 + iDay);
	SetCheckGameTime(CONVERT_SEC(iHour, iMin,0));
}

int	CN3SkyMng::GetDayChangePos_AfterNSec(DWORD dwCurGameTime, float fSec)
{
	// n�� ���� üũ�� ���� �ð��� ���
	DWORD dwCheckGameTime = dwCurGameTime + (DWORD)(fSec*TIME_REAL_PER_GAME);	// 150�� �� ���ӽð�
	int iCheckDayChangeCurPos = m_iDayChangeCurPos;
	if (dwCheckGameTime>86400)	// üũ �ð��� ���ӽð��� 24�ø� ������
	{
		iCheckDayChangeCurPos = 0;
		dwCheckGameTime %= 86400;
	}

	// n������ queue�� ��ġ ã��
	while(iCheckDayChangeCurPos<m_DayChanges.size() &&
		m_DayChanges[m_iDayChangeCurPos].dwWhen < dwCheckGameTime) ++iCheckDayChangeCurPos;
	if (iCheckDayChangeCurPos >= m_DayChanges.size()) iCheckDayChangeCurPos = m_DayChanges.size() - 1;
	return iCheckDayChangeCurPos;
}


D3DCOLOR CN3SkyMng::GetFogColor()
{
	if(m_pSky) return m_pSky->m_FogColor.GetCurColor(); 
	else return 0;
}

D3DCOLOR CN3SkyMng::GetSkyColor()
{
	if(m_pSky) return m_pSky->m_SkyColor.GetCurColor();
	else return 0;
}

D3DCOLOR CN3SkyMng::GetSunColor()
{
	if(m_pSun) return m_pSun->m_Parts[SUNPART_SUN].Color.GetCurColor();
	else return 0;
}

D3DCOLOR CN3SkyMng::GetGlowColor()
{
	if(m_pSun) return m_pSun->m_Parts[SUNPART_GLOW].Color.GetCurColor();
	else return 0;
}

D3DCOLOR CN3SkyMng::GetFlareColor()
{
	if(m_pSun) return m_pSun->m_Parts[SUNPART_FLARE].Color.GetCurColor();
	else return 0;
}

float CN3SkyMng::GetSunRatio()
{
	if(m_pSun) return m_pSun->m_Parts[SUNPART_SUN].Delta.GetCurDelta();
	else return 0;
}

float CN3SkyMng::GetGlowRatio()
{
	if(m_pSun) return m_pSun->m_Parts[SUNPART_GLOW].Delta.GetCurDelta();
	else return 0;
}

float CN3SkyMng::GetFlareRatio()
{
	if(m_pSun) return m_pSun->m_Parts[SUNPART_FLARE].Delta.GetCurDelta();
	else return 0;
}


D3DCOLOR CN3SkyMng::GetCloud1Color()
{
	if(m_pCloud) return m_pCloud->m_Color1.GetCurColor();
	else return 0;
}

D3DCOLOR CN3SkyMng::GetCloud2Color()
{
	if(m_pCloud) return m_pCloud->m_Color2.GetCurColor();
	else return 0;
}

D3DCOLOR CN3SkyMng::GetLightDiffuseColor(int iIndex)
{
	if(iIndex < 0 || iIndex >= MAX_GAME_LIGHT) return 0;
	if(NULL == m_pLightColorDiffuses[iIndex]) return 0;
	
	return m_pLightColorDiffuses[iIndex]->GetCurColor();
}

D3DCOLOR CN3SkyMng::GetLightAmbientColor(int iIndex)
{
	if(iIndex < 0 || iIndex >= MAX_GAME_LIGHT) return 0;
	if(NULL == m_pLightColorAmbients[iIndex]) return 0;
	
	return m_pLightColorAmbients[iIndex]->GetCurColor();
}

bool CN3SkyMng::Load(HANDLE hFile)
{
	DWORD dwRWC = 0;
	std::string szSuns[NUM_SUNPART];
	std::string szClouds[NUM_CLOUD];
	std::string szMoon;

	int i = 0;
	for(i = 0; i < NUM_SUNPART; i++) 
	{
		int iL = 0;
		ReadFile(hFile, &iL, 4, &dwRWC, NULL);
		if(iL > 0)
		{
			szSuns[i].assign(iL, ' ');
			ReadFile(hFile, &(szSuns[i][0]), iL, &dwRWC, NULL);
		}
	}

	for(i = 0; i < NUM_CLOUD; i++) 
	{
		int iL = 0;
		ReadFile(hFile, &iL, 4, &dwRWC, NULL);
		if(iL > 0)
		{
			szClouds[i].assign(iL, ' ');
			ReadFile(hFile, &(szClouds[i][0]), iL, &dwRWC, NULL);
		}
	}
	
	int iL = 0;
	ReadFile(hFile, &iL, 4, &dwRWC, NULL);
	if(iL > 0)
	{
		szMoon.assign(iL, ' ');
		ReadFile(hFile, &(szMoon[0]), iL, &dwRWC, NULL);
	}

	if(NULL == m_pSky) m_pSky = new CN3Sky();
	m_pSky->Init();

	if(NULL == m_pStar) m_pStar = new CN3Star();
	m_pStar->Init();

	if(NULL == m_pSun) m_pSun = new CN3Sun();
	m_pSun->Init(szSuns);

	if(NULL == m_pCloud) m_pCloud = new CN3Cloud();
	m_pCloud->Init(szClouds);

	if(NULL == m_pMoon) m_pMoon = new CN3Moon();
	m_pMoon->Init(szMoon);

	for(i = 0; i < MAX_GAME_LIGHT; i++)
	{
		if(NULL == m_pLightColorDiffuses[i]) m_pLightColorDiffuses[i] = new CN3ColorChange();
		if(NULL == m_pLightColorAmbients[i]) m_pLightColorAmbients[i] = new CN3ColorChange();
	}

	// Day Change .....
	m_DayChanges.clear();
	int iSDCC = 0;
	ReadFile(hFile, &iSDCC, 4, &dwRWC, NULL);
	if(iSDCC > 0)
	{
		m_DayChanges.assign(iSDCC);
		for(i = 0; i < iSDCC; i++)
		{
			m_DayChanges[i].Load(hFile);
		}

		qsort(&(m_DayChanges[0]), m_DayChanges.size(), sizeof(__SKY_DAYCHANGE), CompareTime); // �ð������� ����
	}

	SetCheckGameTime(CONVERT_SEC(10,0,0));
	return true;
}

#ifdef _N3TOOL
bool CN3SkyMng::Save(HANDLE hFile)
{
	DWORD dwRWC = 0;

	std::string szSuns[NUM_SUNPART];
	std::string szClouds[NUM_CLOUD];
	std::string szMoon;

	int i = 0;
	for(i = 0; i < NUM_SUNPART; i++) if(m_pSun && m_pSun->m_Parts[i].pTex) szSuns[i] = m_pSun->m_Parts[i].pTex->FileName();
	for(i = 0; i < NUM_CLOUD; i++) if(m_pCloud) szClouds[i] = m_pCloud->m_szTextures[i];
	if(m_pMoon && m_pMoon->m_pTexture) szMoon = m_pMoon->m_pTexture->FileName();

	for(i = 0; i < NUM_SUNPART; i++) 
	{
		int iL = szSuns[i].size();
		WriteFile(hFile, &iL, 4, &dwRWC, NULL);
		if(iL > 0) WriteFile(hFile, szSuns[i].c_str(), iL, &dwRWC, NULL);
	}

	for(i = 0; i < NUM_CLOUD; i++)
	{
		int iL = szClouds[i].size();
		WriteFile(hFile, &iL, 4, &dwRWC, NULL);
		if(iL > 0) WriteFile(hFile, szClouds[i].c_str(), iL, &dwRWC, NULL);
	}
	
	int iL = szMoon.size();
	WriteFile(hFile, &iL, 4, &dwRWC, NULL);
	if(iL > 0) WriteFile(hFile, szMoon.c_str(), iL, &dwRWC, NULL);

	// Day Change .....
	int iSDCC = m_DayChanges.size();
	WriteFile(hFile, &iSDCC, 4, &dwRWC, NULL);
	for(i = 0; i < iSDCC; i++)
	{
		m_DayChanges[i].Save(hFile);
	}

	return true;
}
#endif // #ifdef _N3TOOL


#ifdef _N3TOOL
__SKY_DAYCHANGE* CN3SkyMng::DayChangeGet(int iIndex)
{
	if(iIndex < 0 || iIndex >= m_DayChanges.size()) return NULL;
	return &(m_DayChanges[iIndex]);
}

__SKY_DAYCHANGE* CN3SkyMng::DayChangeAdd()
{
	__SKY_DAYCHANGE SDC;
	m_DayChanges.push_back(SDC);
	return &(m_DayChanges[m_DayChanges.size()-1]);
}

__SKY_DAYCHANGE* CN3SkyMng::DayChangeInsert(int iIndex)
{
	if(iIndex < 0 || iIndex >= m_DayChanges.size()) return NULL;
	
	it_SDC it = m_DayChanges.begin();
	for(int i = 0; i < iIndex; i++, it++);
	
	it = m_DayChanges.insert(it);
	
	return &(*it);
}

bool CN3SkyMng::DayChangeDelete(int iIndex)
{
	if(iIndex < 0 || iIndex >= m_DayChanges.size()) return false;
	
	it_SDC it = m_DayChanges.begin();
	for(int i = 0; i < iIndex; i++, it++);
	
	it = m_DayChanges.erase(it);
	return true;
}

CN3Texture* CN3SkyMng::SunTextureSet(int iIndex, const char* szPath)
{
	if(NULL == szPath || NULL == m_pSun || iIndex < 0 || iIndex >= NUM_SUNPART) return NULL;

	s_MngTex.Delete(&(m_pSun->m_Parts[iIndex].pTex));
	m_pSun->m_Parts[iIndex].pTex = s_MngTex.Get(szPath);

	return m_pSun->m_Parts[iIndex].pTex;
}

CN3Texture* CN3SkyMng::MoonTextureSet(const char* szPath)
{
	if(NULL == szPath || NULL == m_pMoon) return NULL;

	s_MngTex.Delete(&(m_pMoon->m_pTexture));
	m_pMoon->m_pTexture = s_MngTex.Get(szPath);

	return m_pMoon->m_pTexture;
}

CN3Texture* CN3SkyMng::CloudTextureSet(int iIndex, const char* szPath)
{
	if(NULL == szPath || NULL == m_pCloud || iIndex < 0 || iIndex >= NUM_CLOUD) return NULL;

	s_MngTex.Delete(&(m_pCloud->m_pTextures[iIndex]));
	m_pCloud->m_pTextures[iIndex] = s_MngTex.Get(szPath);
	m_pCloud->m_szTextures[iIndex] = "";
	if(m_pCloud->m_pTextures[iIndex])
		m_pCloud->m_szTextures[iIndex] = m_pCloud->m_pTextures[iIndex]->FileName();

	return m_pCloud->m_pTextures[iIndex];
}

CN3Texture* CN3SkyMng::SunTextureGet(int iIndex)
{
	if(NULL == m_pSun || iIndex < 0 || iIndex >= NUM_CLOUD) return NULL;
	return m_pSun->m_Parts[iIndex].pTex;
}

CN3Texture* CN3SkyMng::MoonTextureGet()
{
	if(NULL == m_pMoon) return NULL;
	return m_pMoon->m_pTexture;
}

CN3Texture* CN3SkyMng::CloudTextureGet(int iIndex)
{
	if(NULL == m_pCloud || iIndex < 0 || iIndex >= NUM_CLOUD) return NULL;
	return m_pCloud->m_pTextures[iIndex];
}

const char* CN3SkyMng::CloudTextureFileName(int iIndex)
{
	if(NULL == m_pCloud || iIndex < 0 || iIndex >= NUM_CLOUD) return NULL;
	return m_pCloud->m_szTextures[iIndex].c_str();
}
#endif

/*
void CN3SkyMng::ColorDeltaSet(int iPercentage, float fHowLong) // ���� �ϴ�, �Ȱ� ���� �ۼ�Ʈ ������ ��ȭ��Ų��.. ��, �� �ö� ����..
{
	if(iPercentage < 0) iPercentage = 0;
	if(iPercentage > 100) iPercentage = 100;

	D3DCOLOR crFinalFog = m_pSky->m_FogColor.GetCurColor();
	D3DCOLOR crFinalSky = m_pSky->m_SkyColor.GetCurColor();

	crFinalFog = 	((((crFinalFog & 0x00ff0000) >> 16) * iPercentage / 100) << 16) | 
					((((crFinalFog & 0x0000ff00) >> 8) * iPercentage / 100) << 8) | 
					((crFinalFog & 0x000000ff) * iPercentage / 100);
	
	crFinalSky = 	((((crFinalSky & 0x00ff0000) >> 16) * iPercentage / 100) << 16) | 
					((((crFinalSky & 0x0000ff00) >> 8) * iPercentage / 100) << 8) | 
					((crFinalSky & 0x000000ff) * iPercentage / 100);
	
	m_pSky->m_FogColor.ChangeColor(crFinalFog, fHowLong);
	m_pSky->m_SkyColor.ChangeColor(crFinalSky, fHowLong);

#ifdef _N3GAME // ������ �ƴ� �������� �ʿ����...
	if(m_pSnd_Weather_Snow) m_pSnd_Weather_Snow->Stop(5.0f);
	if(m_pSnd_Weather_Rain) m_pSnd_Weather_Rain->Stop(5.0f);
#endif // #ifdef _N3GAME
}
*/
