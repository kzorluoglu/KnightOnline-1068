// N3SkyMng.h: interface for the CN3SkyMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3SKYMNG_H__30DC78FB_6563_43BD_841E_B90928E850CD__INCLUDED_)
#define AFX_N3SKYMNG_H__30DC78FB_6563_43BD_841E_B90928E850CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3BaseFileAccess.h"
#include <vector>

const float TIME_REAL_PER_GAME = 10.0f;						// ���� �ð��� game�ð��� ����(���� ������ ���� �ð����� ���� ���� ���ư���..)
const float TIME_GAME_PER_REAL = 1.0f/TIME_REAL_PER_GAME;	// game�ð��� ���� �ð��� ����
#define CONVERT_SEC(h, m, s) ((h)*3600 + (m)*60 + (s))

// �ð� ����
enum		eSKY_DAYCHANGE {	SDC_SKYCOLOR=0,		// �ϴû�
								SDC_FOGCOLOR,		// �Ȱ���
								SDC_STARCOUNT,		// �� ��
								SDC_MOONPHASE,		// �� ���
								SDC_SUNCOLOR,		// �� ��, ũ�� - 1000 �� ����Ʈ���� ������ ũ��.
								SDC_GLOWCOLOR,		// ���� glow��, ũ�� - 1000 �� ����Ʈ���� ������ ũ��.
								SDC_FLARECOLOR,		// ���� flare��, ũ�� - 1000 �� ����Ʈ���� ������ ũ��.
								SDC_CLOUD1COLOR,	// ������ 1�� ��
								SDC_CLOUD2COLOR,	// ������ 2�� ��
								SDC_CLOUDTEX,		// ���� �ؽ��� �ٲٱ�
								
								SDC_LIGHT0COLOR,	// Direction Light 0
								SDC_LIGHT1COLOR,	// Direction Light 1
								SDC_LIGHT2COLOR,	// �÷��̾� ������ Light
								
								NUM_SKYDAYCHANGE,
								
								SDC_UNKNOWN = 0xffffffff
};

struct		__SKY_DAYCHANGE
{
	std::string		szName;			// �̸� ���̱�..
	eSKY_DAYCHANGE	eSkyDayChange;	// � ��ȭ�ΰ�?
	DWORD			dwWhen;			// ���� ��ȭ�����ΰ�?(0�ú��� �ʴ����� ��Ÿ�� �ð�)
	DWORD			dwParam1;		// parameter 1
	DWORD			dwParam2;		// parameter 2
	float			fHowLong;		// �󸶳� �ɸ����ΰ�?(�ǽð� ��)

	void Init()
	{
		szName = "";
		eSkyDayChange = SDC_UNKNOWN;
		dwWhen = dwParam1 = dwParam2 = 0;
		fHowLong = 0;
	}

	void Init(const std::string& szName_Arg, eSKY_DAYCHANGE eSDC_Arg, DWORD dwWhen_Arg, DWORD dwParam1_Arg, DWORD dwParam2_Arg, float fHowLong_Arg)
	{
		szName = szName_Arg;
		eSkyDayChange = eSDC_Arg;
		dwWhen = dwWhen_Arg;
		dwParam1 = dwParam1_Arg;
		dwParam2 = dwParam2_Arg;
		fHowLong = fHowLong_Arg;
	}

	bool Load(HANDLE hFile)
	{
		szName = "";

		DWORD dwRWC = 0;
		int nL = 0;
		ReadFile(hFile, &nL, 4, &dwRWC, NULL);
		if(nL > 0) 
		{
			std::vector<char> buffer(nL+1, NULL);
			ReadFile(hFile, buffer.begin(), nL, &dwRWC, NULL);
			szName = buffer.begin();
		}

		ReadFile(hFile, &eSkyDayChange, 4, &dwRWC, NULL);
		ReadFile(hFile, &dwWhen, 4, &dwRWC, NULL);
		ReadFile(hFile, &dwParam1, 4, &dwRWC, NULL);
		ReadFile(hFile, &dwParam2, 4, &dwRWC, NULL);
		ReadFile(hFile, &fHowLong, 4, &dwRWC, NULL);

		return true;
	}
	
	bool Save(HANDLE hFile)
	{
		DWORD dwRWC = 0;

		int nL = szName.size();
		WriteFile(hFile, &nL, 4, &dwRWC, NULL);
		if(nL > 0) WriteFile(hFile, szName.c_str(), nL, &dwRWC, NULL);

		WriteFile(hFile, &eSkyDayChange, 4, &dwRWC, NULL);
		WriteFile(hFile, &dwWhen, 4, &dwRWC, NULL);
		WriteFile(hFile, &dwParam1, 4, &dwRWC, NULL);
		WriteFile(hFile, &dwParam2, 4, &dwRWC, NULL);
		WriteFile(hFile, &fHowLong, 4, &dwRWC, NULL);

		return true;
	}

	__SKY_DAYCHANGE() { this->Init(); }
};

const int MAX_GAME_LIGHT = 3;

class CN3SkyMng : public CN3BaseFileAccess
{
public:
	CN3SkyMng();
	virtual ~CN3SkyMng();
// Attributes
public:
	enum		eSKY_WEATHER {  SW_CLEAR=1,	// ����
								SW_RAINY,	// ��
								SW_SNOW};	// ��
protected:
	class CN3Sky*			m_pSky;
	class CN3Moon*			m_pMoon;
	class CN3Sun*			m_pSun;
	class CN3Cloud*			m_pCloud;
	class CN3Star*			m_pStar;
	class CN3ColorChange*	m_pLightColorDiffuses[MAX_GAME_LIGHT];
	class CN3ColorChange*	m_pLightColorAmbients[MAX_GAME_LIGHT];

	std::vector<__SKY_DAYCHANGE> m_DayChanges;		// �����Է��Ŀ� qsort����
	int					m_iDayChangeCurPos;

	std::vector<__SKY_DAYCHANGE> m_WeatherChanges;		// �����Է��Ŀ� qsort����
	int					m_iWeatherChangeCurPos;

	DWORD		m_dwCheckTick;	// �������� �ð��� �������� ������TickCount(�ǽð�) (���ӽð����� 24�ÿ� �ٽ� �����ϱ⵵ �Ѵ�.)
	DWORD		m_dwCheckGameTime;	// �������� �������� �ð�(���� �ð� �ʴ���) 0 ~ (24*60*60)
	eSKY_WEATHER	m_eWeather;

	int			m_iYear;
	int			m_iMonth;
	int			m_iDay;

	int			m_iHourFix;		// ��,�� ���� ������ �ð����� ��Ÿ���� ����.


	float			m_fCellSize;	// ���� �� ũ��
	POINT			m_CurCellPos;	// ���� ���μ� ��ǥ
	__Vector3		m_vPos[9];		// �ֺ� �� 9ĭ�� �߽���ǥ��

	// ȿ����..
	class CN3GERain*		m_pGERain;		// ��
	class CN3GESnow*		m_pGESnow;		// ��
#ifdef _N3GAME // ������ �ƴ� �������� �ʿ����...
	class CN3SndObj*		m_pSnd_Weather_Rain;
	class CN3SndObj*		m_pSnd_Weather_Snow;
#endif // #ifdef _N3GAME

// Operations
public:
#ifdef _N3TOOL
	void InitToDefaultHardCoding();
	bool LoadFromTextFile(const char* szIniFN);
	bool SaveToTextFile(const char* szIniFN);
	bool DayChangeParse(FILE* fp, __SKY_DAYCHANGE* pDayChange);
	bool DayChangeWrite(FILE* fp, __SKY_DAYCHANGE* pDayChange);

	int					DayChangeCount() { return m_DayChanges.size(); };
	__SKY_DAYCHANGE*	DayChangeGet(int iIndex);
	__SKY_DAYCHANGE*	DayChangeAdd();
	__SKY_DAYCHANGE*	DayChangeInsert(int iIndex);
	bool				DayChangeDelete(int iIndex);

	CN3Texture*			SunTextureSet(int iIndex, const char* szPath);
	CN3Texture*			MoonTextureSet(const char* szPath);
	CN3Texture*			CloudTextureSet(int iIndex, const char* szPath);
	
	CN3Texture*			SunTextureGet(int iIndex);
	CN3Texture*			MoonTextureGet();
	CN3Texture*			CloudTextureGet(int iIndex);
	const char*			CloudTextureFileName(int iIndex);
#endif

	bool Load(HANDLE hFile);

#ifdef _N3GAME // ������ �ƴ� �������� �ʿ����...
	void ReleaseSound();
#endif // #ifdef _N3GAME

#ifdef _N3TOOL
	bool Save(HANDLE hFile);
#endif

	D3DCOLOR	GetFogColor();
	D3DCOLOR	GetSkyColor();
	D3DCOLOR	GetSunColor();
	D3DCOLOR	GetGlowColor();
	D3DCOLOR	GetFlareColor();
	float		GetSunRatio();
	float		GetGlowRatio();
	float		GetFlareRatio();

	D3DCOLOR	GetCloud1Color();
	D3DCOLOR	GetCloud2Color();
	
	float		GetSunAngleByRadin();

	D3DCOLOR	GetLightDiffuseColor(int iIndex);
	D3DCOLOR	GetLightAmbientColor(int iIndex);

	void	Release();
	void	Render(); // �ϴø� ������..
	void	RenderWeather(); // �� ������ ���� ������..
	void	Tick();

	// �ð� ����
	void	GetGameTime(int* piYear, int* piMonth, int* piDay, int* piHour, int*piMin);
	void	SetGameTime(int iYear, int iMonth, int iDay, int iHour, int iMin);
	void	SetWeather(eSKY_WEATHER eWeather, int iPercentage);	//	���� ����
	
	void	SunAndMoonDirectionFixByHour(int iHour); // �ؿ� �� ���� ����

//	By : Ecli666 ( On 2002-04-04 ���� 10:53:58 )
//
	CN3Sun*	GetSunPointer()	{	return m_pSun;	}
//	~(By Ecli666 On 2002-04-04 ���� 10:53:58 )
	
protected:
	void		SetCheckGameTime(DWORD dwCheckGameTime);//	CheckGameTime�� �����ְ� ����ð��� �ٽ� �����Ѵ�.(Ư�� �ð����� ���������� ���鶧 ȣ���Ѵ�.)
	int			GetLatestChange(eSKY_DAYCHANGE eSDC, int iPos);// m_pDayChangeQueues���� ������ ��ġ(iPos) ������ ���� �ֱٿ� ��ȭ�ϴ� ��ġ ������
	void		ChangeSky(__SKY_DAYCHANGE* pSDC, float fTakeTime);
	int			GetDayChangePos_AfterNSec(DWORD dwCurGameTime, float fSec);		// �ǽð� N�� �Ŀ� DayChangeQueue�� ��ġ ���ϱ�
	static int		CompareTime(const void* pArg1, const void* pArg2);

};

#endif // !defined(AFX_N3SKYMNG_H__30DC78FB_6563_43BD_841E_B90928E850CD__INCLUDED_)
