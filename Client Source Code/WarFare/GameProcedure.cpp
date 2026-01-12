// GameProcedure.cpp: implementation of the CGameProcedure class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"

#include "GameDef.h"
#include "GameEng.h"
#include "packetdef.h"
#include "LocalInput.h"
#include "APISocket.h"
#include "UIMessageBox.h"
#include "UIMessageBoxManager.h"
#include "UIManager.h"

#include "N3FXMgr.h"
#include "PlayerMyself.h"
#include "GameProcedure.h"
#include "GameProcLogIn.h"
//#include "GameProcStart.h"
#include "GameProcNationSelect.h"
#include "GameProcCharacterCreate.h"
#include "GameProcCharacterSelect.h"
#include "GameProcMain.h"
#include "GameProcOption.h"

#include "UILoading.h"
#include "UINotice.h"
#include "UIHelp.h"
#include "UIHotKeyDlg.h"
#include "UIChat.h"
#include "UIVarious.h"
#include "UIPartyOrForce.h"
#include "UIMessageWnd.h"
#include "UIEndingDisplay.h"

#include "../N3Base/N3UIEdit.h"
#include "../N3Base/N3SndObjStream.h"
#include "../N3Base/N3TableBase.h"
#include "..\N3Base\N3FXBundle.h"

#include "../N3Base/BitmapFile.h"
#include "../N3Base/Jpeg.h"
#include "../JPEG/JpegFile.h"

#include "MagicSkillMng.h"
#include "KnightChrMgr.h"
#include "GameCursor.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CKnightChrMgr*		CGameProcedure::s_pKnightChr = NULL;		// ����Ʈ ĳ����..
CN3SndObjStream*	CGameProcedure::s_pSnd_BGM = NULL;			// ���� ������� ������..
CLocalInput*		CGameProcedure::s_pLocalInput = NULL;		// ���콺�� Ű���� �Է� ��ü .. Direct Input �� ���.
CAPISocket*			CGameProcedure::s_pSocket = NULL;			// ���� ���� ��ü
CAPISocket*			CGameProcedure::s_pSocketSub = NULL;		// ���� ���� ��ü
CGameEng*			CGameProcedure::s_pEng = NULL;				// 3D Wrapper Engine
CN3FXMgr*			CGameProcedure::s_pFX = NULL;

CUIManager*			CGameProcedure::s_pUIMgr = NULL;			// UI Manager
CUILoading*			CGameProcedure::s_pUILoading = NULL;		// �ε���..
CUIMessageBoxManager*	CGameProcedure::s_pMsgBoxMgr = NULL;		// MessageBox Manager
//bool				CGameProcedure::s_bUseSpeedHack = false;

CGameProcedure*				CGameProcedure::s_pProcPrev = NULL;
CGameProcedure*				CGameProcedure::s_pProcActive = NULL;

CGameProcLogIn*				CGameProcedure::s_pProcLogIn = NULL;
CGameProcNationSelect*		CGameProcedure::s_pProcNationSelect = NULL;
CGameProcCharacterCreate*	CGameProcedure::s_pProcCharacterCreate = NULL;
CGameProcCharacterSelect*	CGameProcedure::s_pProcCharacterSelect = NULL;
CGameProcMain*				CGameProcedure::s_pProcMain = NULL;
CGameProcOption*			CGameProcedure::s_pProcOption = NULL;
CGameCursor*				CGameProcedure::s_pGameCursor = NULL;

HCURSOR	CGameProcedure::s_hCursorNormal = NULL;
HCURSOR	CGameProcedure::s_hCursorNormal1 = NULL;
HCURSOR	CGameProcedure::s_hCursorClick = NULL;
HCURSOR	CGameProcedure::s_hCursorClick1 = NULL;
HCURSOR	CGameProcedure::s_hCursorAttack = NULL;
HCURSOR	CGameProcedure::s_hCursorPreRepair = NULL;
HCURSOR	CGameProcedure::s_hCursorNowRepair = NULL;

e_Version CGameProcedure::s_eVersion =	W95;
e_LogInClassification CGameProcedure::s_eLogInClassification; // ������ ����.. MGame, Daum, KnightOnLine ....
std::string	CGameProcedure::s_szAccount = ""; // ���� ���ڿ�..
std::string	CGameProcedure::s_szPassWord = ""; // ���� ���..
std::string	CGameProcedure::s_szServer = ""; // ���� ���ڿ�..
bool CGameProcedure::m_bCursorLocked = false;
HCURSOR	CGameProcedure::m_hPrevGameCursor = NULL;
HWND CGameProcedure::s_hWndSubSocket = NULL; // ���� ���Ͽ� ������ �ڵ�..
int	CGameProcedure::s_iChrSelectIndex = 0;
bool CGameProcedure::s_bNeedReportConnectionClosed = false; // ���������� �������� �����ؾ� �ϴ���..
bool CGameProcedure::s_bWindowed = false; // â��� ����??
bool CGameProcedure::s_bKeyPress = false;	//Ű�� ���������� ui���� �ش��ϴ� ���۵����� �ִٸ�
bool CGameProcedure::s_bKeyPressed = false;	//Ű�� �ö����� ui���� �ش��ϴ� ���۵����� �ִٸ�

CGameProcedure::CGameProcedure()
{
	m_bCursorLocked = false;
}

CGameProcedure::~CGameProcedure()
{
	m_bCursorLocked = false;
}

void CGameProcedure::Release()
{
	s_pUIMgr->SetFocusedUI(NULL);
}

void CGameProcedure::Init()
{
	s_pUIMgr->SetFocusedUI(NULL);
}

void CGameProcedure::StaticMemberInit(HINSTANCE hInstance, HWND hWndMain, HWND hWndSub)
{
	CLogWriter::Write("StaticMemberInit: begin");
	s_pKnightChr = new CKnightChrMgr(hWndMain);
	CLogWriter::Write("StaticMemberInit: after KnightChrMgr");

	//////////////////////////////////////////////////////////////////////////////////////////
	// ���� �⺻ 3D ���� �����..
#if _DEBUG 
	s_bWindowed = true;
#endif // #if _DEBUG 

#ifndef _DEBUG
	// �븸 ������ ��� IME ������ ���� ��¥ Ǯ��带 ����..
	int iLangID = ::GetUserDefaultLangID();
	if(0x0404 == iLangID) 
	{
		s_bWindowed = true;
		
		DEVMODE dm;
		dm.dmSize = sizeof(DEVMODE);
		dm.dmPelsWidth = CN3Base::s_Options.iViewWidth;
		dm.dmPelsHeight = CN3Base::s_Options.iViewHeight;
		dm.dmBitsPerPel = CN3Base::s_Options.iViewColorDepth;
		dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		::ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
	}
#endif // #ifndef _DEBUG


	RECT rc;
	::GetClientRect(hWndMain, &rc);
	s_pEng = new CGameEng();
	if(false == s_pEng->Init(s_bWindowed, hWndMain, CN3Base::s_Options.iViewWidth, CN3Base::s_Options.iViewHeight, CN3Base::s_Options.iViewColorDepth, TRUE))
	{
		CLogWriter::Write("StaticMemberInit: s_pEng->Init failed");
		exit(-1);
	}
	CLogWriter::Write("StaticMemberInit: s_pEng->Init ok");
	// ���� �⺻ 3D ���� �����..
	::SetFocus(hWndMain); // Set focus this window..
	CLogWriter::Write("StaticMemberInit: after SetFocus");
	
	RECT rcTmp = rc; rcTmp.left = (rc.right - rc.left) / 2; rcTmp.bottom = rcTmp.top + 30;
	CN3UIEdit::CreateEditWindow(hWndMain, rcTmp);
	CLogWriter::Write("StaticMemberInit: after UIEdit create");
	//////////////////////////////////////////////////////////////////////////////////////////

	s_hWndSubSocket = hWndSub; // ���� ���Ͽ� ������ �ڵ�..

	CLogWriter::Write("StaticMemberInit: before CGameBase init");
	CGameBase::StaticMemberInit(); // Table �� ����, ������Ʈ, ĳ���� �ʱ�ȭ...
	CLogWriter::Write("StaticMemberInit: tables loaded");

	//////////////////////////////////////////////////////////////////////////////////////////
	// Game Procedure ���ϰ� ���� ��ǲ, 3D����, Resource Table �ε� �� �ʱ�ȭ...
	s_pSocket = new CAPISocket();
	s_pSocketSub = new CAPISocket();
	CLogWriter::Write("StaticMemberInit: sockets created");
	
    // Ŀ�� �����..
	s_hCursorNormal =		LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR_NORMAL));
	s_hCursorNormal1 =		LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR_NORMAL1));
	s_hCursorClick =		LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR_CLICK));
	s_hCursorClick1 =		LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR_CLICK1));
	s_hCursorAttack =		LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR_ATTACK));
	s_hCursorPreRepair =	LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR_PRE_REPAIR));
	s_hCursorNowRepair =	LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR_NOW_REPAIR));

	if(!CN3Base::s_Options.bWindowCursor)
	{
		s_pGameCursor = new CGameCursor();
		s_pGameCursor->LoadFromFile("ui\\cursor.uif");
	}
	SetGameCursor(s_hCursorNormal);
	CLogWriter::Write("StaticMemberInit: cursors loaded");

	s_pLocalInput = new CLocalInput();
	s_pLocalInput->Init(hInstance, hWndMain, FALSE); // Input �� �ʱ�ȭ.
	CLogWriter::Write("StaticMemberInit: input initialized");

	//////////////////////////////////////////////////////////////////////////////////////////
	// Sound �ʱ�ȭ..
	if(CN3Base::s_Options.bSndEnable)
	{
		CN3Base::s_SndMgr.Init(hWndMain);
		CN3Base::s_SndMgr.SetDuplicated(CN3Base::s_Options.bSndDuplicated);
	}
	CN3FXBundle::SetEffectSndDistance(float(CN3Base::s_Options.iEffectSndDist));
	CLogWriter::Write("StaticMemberInit: sound/fx init done");

	s_pFX = new CN3FXMgr();

	__TABLE_UI_RESRC* pTblUI = s_pTbl_UI->Find(NATION_ELMORAD); // �⺻�� ������ UI �� �Ѵ�..
	__ASSERT(pTblUI, "�⺻ UI �� �����ϴ�.");

	s_pUIMgr = new CUIManager(); // �⺻ UIManager
	s_pMsgBoxMgr = new CUIMessageBoxManager(); //MessageBox Manager
	CLogWriter::Write("StaticMemberInit: UI managers created");

	// ����..
	CN3UIBase::EnableTooltip(pTblUI->szToolTip);
	CLogWriter::Write("StaticMemberInit: tooltip enabled");

	//////////////////////////////////////////////////////////////////////////////////////////
	// �� ���ν����� ����
	s_pProcLogIn			= new CGameProcLogIn();				// �α��� ���ν���
	s_pProcNationSelect		= new CGameProcNationSelect();		// ���� ����
	s_pProcCharacterSelect	= new CGameProcCharacterSelect();	// ĳ���� ����
	s_pProcCharacterCreate	= new CGameProcCharacterCreate();	// ĳ���� �����
	s_pProcMain				= new CGameProcMain();				// ���� ���� ���ν���
	s_pProcOption			= new CGameProcOption();			// ���� �ɼ� ���ν���

	//////////////////////////////////////////////////////////////////////////////////////////
	// ���� ����.. ^^
	OSVERSIONINFO winfo;
	winfo.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&winfo);
	if(winfo.dwPlatformId==VER_PLATFORM_WIN32_NT)
	{
		if(winfo.dwMajorVersion>=5)
			s_eVersion=W2K;
		else
			s_eVersion=WNT4;
	}
	else
	if(winfo.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS)
	{
		if(winfo.dwMinorVersion<10)
			s_eVersion=W95;
		else
		if(winfo.dwMinorVersion<90)
		   s_eVersion=W98;
		else
		   s_eVersion=WME;				
	CLogWriter::Write("StaticMemberInit: end");
	}
}

void CGameProcedure::StaticMemberRelease()
{
#ifndef _DEBUG
	// �븸 ������ ��� IME ������ ���� ��¥ Ǯ��带 ����..
	int iLangID = ::GetUserDefaultLangID();
	if(0x0404 == iLangID) 
	{
		DEVMODE dm;
		::EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, &dm);

		// ������Ʈ���� ���÷��� ���� ���� ������ ���÷��� ��尡 �ٸ��� ���� �����ش�.
		if(	dm.dmPelsWidth != CN3Base::s_Options.iViewWidth || 
			dm.dmPelsHeight != CN3Base::s_Options.iViewHeight || 
			dm.dmBitsPerPel != CN3Base::s_Options.iViewColorDepth)
		{
			dm.dmSize = sizeof(DEVMODE);
			dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
			::ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
		}
	}
#endif // #ifndef _DEBUG


	delete s_pSocket; s_pSocket = NULL; // ��� ����..
	delete s_pSocketSub; s_pSocketSub = NULL; // ���� ���� ���ֱ�..
	delete s_pFX; s_pFX = NULL;

	////////////////////////////////////////////////////////////
	// �⺻�� ����..
	if(s_pPlayer)
	{
		int iRun = s_pPlayer->IsRunning(); // �̵� ��尡 �ٴ� ���¿����� 
		CGameProcedure::RegPutSetting("UserRun", &iRun, 4); // �ȱ�, �ٱ� ���� ���..
	}
	
	if(s_pEng)
	{
		e_ViewPoint eVP = s_pEng->ViewPoint();
		CGameProcedure::RegPutSetting("CameraMode", &eVP, 4); // ī�޶� ���� ���
	}
	// �⺻�� ����..
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////
	// ����ȭ�� ���̱�..
	if(s_pPlayer)
	{
		e_Nation eNation = s_pPlayer->m_InfoBase.eNation;
		__TABLE_UI_RESRC* pTbl = s_pTbl_UI->Find(eNation);
		if(pTbl)
		{
			CUIEndingDisplay Credit; // ���� ǥ���ϱ�..
			Credit.LoadFromFile(pTbl->szEndingDisplay);
			Credit.Render();
		}
	}
	// ����ȭ�� ���̱�..
	////////////////////////////////////////////////////////////////////////

	delete s_pKnightChr;	s_pKnightChr = NULL;
//	if ( (s_pProcMain) && (s_pProcMain->m_pUIHotKeyDlg) )
//			s_pProcMain->m_pUIHotKeyDlg->CloseIconRegistry();

	// UI ��ġ�� ���̱� ���� ���� ����..
	if(s_pProcMain)
	{
		UIPostData_Write(UI_POST_WND_CHAT, s_pProcMain->m_pUIChatDlg);
		UIPostData_Write(UI_POST_WND_HOTKEY, s_pProcMain->m_pUIHotKeyDlg);
		UIPostData_Write(UI_POST_WND_HELP, s_pProcMain->m_pUIHelp);
//		UIPostData_Write(UI_POST_WND_PARTY, s_pProcMain->m_pUIPartyOrForce);
	}

	// �� ���ν�����
	delete s_pProcLogIn; s_pProcLogIn = NULL; 						// �α��� ���ν���
	delete s_pProcNationSelect; s_pProcNationSelect = NULL; 		// ���� ����
	delete s_pProcCharacterSelect; s_pProcCharacterSelect = NULL; 	// ĳ���� ����
	delete s_pProcCharacterCreate; s_pProcCharacterCreate = NULL; 	// ĳ���� �����
	delete s_pProcMain; s_pProcMain = NULL; 						// ���� ���� ���ν���
	delete s_pProcOption; s_pProcOption = NULL; 					// ���� �ɼ� ���ν���

	// UI �� ������..
	if(s_pUILoading) delete s_pUILoading; s_pUILoading = NULL;		// Loading Bar
	delete s_pMsgBoxMgr;
	delete s_pUIMgr; s_pUIMgr = NULL;				// UI Manager

//	delete s_pIME; s_pIME = NULL;
	delete s_pLocalInput; s_pLocalInput = NULL;
	delete s_pEng; s_pEng = NULL; // �� �������� ���� ������.!!!!!

	if(s_pGameCursor) delete s_pGameCursor; s_pGameCursor = NULL;

	CGameBase::StaticMemberRelease();
}

void CGameProcedure::Tick()
{
	s_pLocalInput->Tick(); // Ű����� ���콺�κ��� �Է��� �޴´�.
	if(s_pGameCursor) s_pGameCursor->Tick();

	ProcessUIKeyInput();

	DWORD dwMouseFlags = s_pLocalInput->MouseGetFlag();
	POINT ptPrev = s_pLocalInput->MouseGetPosOld();
	POINT ptCur = s_pLocalInput->MouseGetPos();

	e_Nation eNation = s_pPlayer->m_InfoBase.eNation;
	if(dwMouseFlags & MOUSE_LBCLICK) SetGameCursor(((NATION_ELMORAD == eNation) ? s_hCursorClick1 : s_hCursorClick));
	else if(dwMouseFlags & MOUSE_LBCLICKED) SetGameCursor(((NATION_ELMORAD == eNation) ? s_hCursorNormal1 : s_hCursorNormal));
	if(dwMouseFlags & MOUSE_RBCLICKED)
	{
		if(s_pPlayer->m_bAttackContinous && s_pProcActive == s_pProcMain) // ���� ���ν��� �̸�..
			SetGameCursor(s_hCursorAttack);
		else
			SetGameCursor(((NATION_ELMORAD == eNation) ? s_hCursorNormal1 : s_hCursorNormal));
	}

	DWORD dwRet = 0;
	dwRet = s_pMsgBoxMgr->MouseProcAndTick(dwMouseFlags, s_pLocalInput->MouseGetPos(), s_pLocalInput->MouseGetPosOld());

	if(0 == dwRet)
	{
		dwRet = s_pUIMgr->MouseProc(dwMouseFlags, ptCur, ptPrev);
		s_pUIMgr->Tick();
	}

	// �� �ϸ�... 
//	if((dwRet & UI_MOUSEPROC_CHILDDONESOMETHING) || (dwRet & UI_MOUSEPROC_DONESOMETHING))
//		s_pLocalInput->MouseRemoveFlag(0xffMOUSE_LBCLICK | MOUSE_LBCLICKED | MOUSE_LBDBLCLK);
	s_pUIMgr->m_bDoneSomething = false;		// UI ���� ������ �ߴ�...
	if(dwRet != UI_MOUSEPROC_NONE)
		s_pUIMgr->m_bDoneSomething = true;		// UI ���� ������ �ߴ�...

	CN3Base::s_SndMgr.Tick(); // Sound Engine...

	// ��ũ�� ĸ�� Ű..
	if(s_pLocalInput->IsKeyPress(DIK_NUMPADMINUS)) // Ű�е��� ���̳ʽ� Ű�� ������..
	{
		SYSTEMTIME st;
		::GetLocalTime(&st);
		char szFN[128] = "";
//		sprintf(szFN, "%d_%d_%d_%d.%d.%d.jpg", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		sprintf(szFN, "%d_%d_%d_%d.%d.%d.ksc", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		this->CaptureScreenAndSaveToFile(szFN);
	}

	//////////////////////////////////
	// Network Msg ó���ϱ�
	DataPack* pDataPack = NULL;
	while ( s_pSocket->m_qRecvPkt.size() > 0 )			// ��Ŷ ����Ʈ�� ��Ŷ�� �ֳ�????
	{
		int iOffset = 0;
		pDataPack = s_pSocket->m_qRecvPkt.front();				// ť�� ù��° ���� ����..
		if (false == ProcessPacket(pDataPack, iOffset))
		{
			// ��Ŷ�� ó���� ��Ȳ�� �ƴϴ�.
			int iTempOffst = 0;
			int iCmd = CAPISocket::Parse_GetByte(pDataPack->m_pData, iTempOffst);
			CLogWriter::Write("Invalid Packet... (%d)", iCmd);
		}
		delete pDataPack;
		s_pSocket->m_qRecvPkt.pop();					// ��Ŷ�� ť���� ����..
	}

	while ( s_pSocketSub->m_qRecvPkt.size() > 0 )		// ��Ŷ ����Ʈ�� ��Ŷ�� �ֳ�????
	{
		int iOffset = 0;
		pDataPack = s_pSocketSub->m_qRecvPkt.front();			// ť�� ù��° ���� ����..
		if (false == ProcessPacket(pDataPack, iOffset)) break;	// ��Ŷ�� ó���� ��Ȳ�� �ƴϴ�.
		delete pDataPack;
		s_pSocketSub->m_qRecvPkt.pop();					// ��Ŷ�� ť���� ����..
	}
	// Network Msg ó���ϱ�
	//////////////////////////////////
}

void CGameProcedure::Render()
{
	if(s_pUIMgr) s_pUIMgr->Render(); // UI �� ������..

	s_pMsgBoxMgr->Render();
	if(s_pGameCursor) s_pGameCursor->Render();
}

void CGameProcedure::TickActive()
{
	if(s_pProcActive != s_pProcPrev) // ���ν����� �ٲ��..
	{
		if(s_pProcPrev) s_pProcPrev->Release();
		if(s_pProcActive) s_pProcActive->Init();

		s_pProcPrev = s_pProcActive;
	}

	if(s_pProcActive)	s_pProcActive->Tick();		// ���� ���ν��� Tick ................................
}

void CGameProcedure::RenderActive()
{
	if(s_pProcActive != s_pProcPrev) // ���ν����� �ٲ��..
	{
		if(s_pProcPrev) s_pProcPrev->Release();
		if(s_pProcActive) s_pProcActive->Init();

		s_pProcPrev = s_pProcActive;
	}
	if(s_pProcActive) s_pProcActive->Render();
}

bool CGameProcedure::CaptureScreenAndSaveToFile(const std::string& szFN)
{
	if(szFN.empty()) return false;
	CJpegFile file;

	RECT wndRect;
	GetWindowRect(CN3Base::s_hWndBase, &wndRect);
	
	HANDLE hDIB = file.CopyScreenToDIB(&wndRect);
	if(hDIB)
	{
		int nQuality = 90;
		char szBuf[256] = "";

		//��ڴ� ������ ��ũ�� ĸ�ĸ� �Ҽ� �ְ�...
		if(s_pPlayer->m_InfoBase.iAuthority == AUTHORITY_MANAGER)
			nQuality = 100;

		if(file.EncryptJPEG(hDIB, nQuality, szFN, szBuf) == TRUE)
		{
			TRACE("Screen Captue %s\n", szFN.c_str());
		}
		GlobalFree(hDIB);
	}
	return true;
/*
	int iW = CN3Base::s_CameraData.vp.Width;
	int iH = CN3Base::s_CameraData.vp.Height;

	bool bResult = false;
	LPDIRECT3DSURFACE8 lpDDSTmp = NULL;
	LPDIRECT3DSURFACE8 lpDDSBack = NULL;
	CN3Base::s_lpD3DDev->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &lpDDSBack);
	if(lpDDSBack)
	{
		CN3Base::s_lpD3DDev->CreateImageSurface(iW, iH, D3DFMT_X8R8G8B8, &lpDDSTmp);
		if(lpDDSTmp)
		{
			HRESULT rval = D3DXLoadSurfaceFromSurface(lpDDSTmp, NULL, NULL, lpDDSBack, NULL, NULL, D3DX_FILTER_NONE, 0);
//			HRESULT rval = s_lpD3DDev->CopyRects(lpDDSBack, NULL, 0, lpDDSTmp, NULL);
//			char szErr[256];
//			::D3DXGetErrorString(rval, szErr, 256);

			if(D3D_OK == rval)
			{
				D3DLOCKED_RECT LR;
				if(D3D_OK == lpDDSTmp->LockRect(&LR, NULL, 0))
				{
//					std::vector<BYTE> buff(iW * iH * 3, 0);
					CBitMapFile bmf;
					bmf.Create(iW, iH);

					for(int y = 0; y < iH; y++)
					{
						BYTE* pPS = ((BYTE*)LR.pBits) + LR.Pitch * y;
//						BYTE* pPD = (BYTE*)(&(buff[y * (iW * 3)]));
						BYTE* pPD = (BYTE*)(bmf.Pixels(0, y));

						for(int x = 0; x < iW; x++, pPS += 4, pPD +=3 )
						{
							pPD[0] = pPS[0];
							pPD[1] = pPS[1];
							pPD[2] = pPS[2];
						}
					}
					lpDDSTmp->UnlockRect();

//					CJpeg jpg;
//					jpg.SaveJPG(szFN.c_str(), iW, iH, &(buff[0]));
					bmf.SaveToFile(szFN.c_str());
				}
			}
			
			lpDDSTmp->Release();
			lpDDSTmp = NULL;
		}


		lpDDSBack->Release();
		lpDDSBack = NULL;
	}

	return bResult;
*/
}

void CGameProcedure::ProcActiveSet(CGameProcedure *pProc)
{
	if(NULL == pProc || s_pProcActive == pProc) return;

	if(s_pUIMgr) s_pUIMgr->EnableOperationSet(true); // UI�� �����Ҽ� �ְ� �Ѵ�..
	CGameProcedure::MessageBoxClose(-1); // MessageBox �� �� ������ �����.

	s_pProcPrev = s_pProcActive; // ���� �� ������ ���..
	s_pProcActive = pProc;
}

void CGameProcedure::ReConnect()
{
	s_bNeedReportConnectionClosed = false; // ���������� �������� �����ؾ� �ϴ���..
	CGameProcedure::s_pSocket->ReConnect();
	s_bNeedReportConnectionClosed = true; // ���������� �������� �����ؾ� �ϴ���..
}

std::string CGameProcedure::MessageBoxPost(const std::string& szMsg, const std::string& szTitle, int iStyle, e_Behavior eBehavior)
{
	return s_pMsgBoxMgr->MessageBoxPost(szMsg, szTitle, iStyle, eBehavior);
}

void CGameProcedure::MessageBoxClose(const std::string& szMsg)
{
	s_pMsgBoxMgr->MessageBoxClose(szMsg);
}

void CGameProcedure::MessageBoxClose(int iMsgBoxIndex)
{
	if( iMsgBoxIndex == -1 )
		s_pMsgBoxMgr->MessageBoxCloseAll();
}


bool CGameProcedure::RegPutSetting( const char *ValueName, void *pValueData, long length )
{
	HKEY	hKey;

	if ( RegOpenKey( HKEY_CURRENT_USER, GetStrRegKeySetting().c_str(), &hKey ) != ERROR_SUCCESS ) 
	{
		if ( RegCreateKey( HKEY_CURRENT_USER, GetStrRegKeySetting().c_str(), &hKey ) != ERROR_SUCCESS )
		{
			__ASSERT(0, "Registry Create Failed!!!");
			return false;
		}
		if(RegOpenKey( HKEY_CURRENT_USER, GetStrRegKeySetting().c_str(), &hKey ) != ERROR_SUCCESS ) 
		{
			__ASSERT(0, "Registry Open Failed!!!");
			return false;
		}
	}

	// set the value
	if ( RegSetValueEx( hKey, ValueName, 0, REG_BINARY, (const BYTE *)pValueData, length ) != ERROR_SUCCESS )
	{
		__ASSERT(0, "Registry Write Failed!!!");
		RegCloseKey( hKey );
		return false;
	}

	if ( RegCloseKey( hKey ) != ERROR_SUCCESS ) 
	{
		__ASSERT(0, "Registry Close Failed!!!");
		return false;
	}

	return true;
}

bool CGameProcedure::RegGetSetting( const char *ValueName, void *pValueData, long length)
{
	HKEY	hKey;
	DWORD	Type;
	DWORD	len;

	len = length;

	if ( RegOpenKey( HKEY_CURRENT_USER, GetStrRegKeySetting().c_str(), &hKey ) != ERROR_SUCCESS ) 
	{
//		__ASSERT(0, "Registry Open Failed!!!");
		return false;
	}

	// get the value
	if ( RegQueryValueEx( hKey, ValueName, NULL, &Type, (BYTE *)pValueData, &len ) != ERROR_SUCCESS )
	{
//		__ASSERT(0, "Registry Query Failed!!!");
		RegCloseKey( hKey );
		return false;
	}

	if ( RegCloseKey( hKey ) != ERROR_SUCCESS ) 
	{
//		__ASSERT(0, "Registry Close Failed!!!");
		return false;
	}

	return true;
}

void CGameProcedure::UIPostData_Write(const std::string& szKey, CN3UIBase* pUI)
{
	if(szKey.empty() || NULL == pUI) return;

	__WndInfo WI;
	lstrcpyn(WI.szName, szKey.c_str(), 16);
	WI.bVisible = pUI->IsVisible();
	WI.ptPosition = pUI->GetPos();

	RegPutSetting(WI.szName, &WI, sizeof(__WndInfo) );
}

void CGameProcedure::UIPostData_Read(const std::string& szKey, CN3UIBase* pUI, int iDefaultX, int	 iDefaultY)
{
	if(szKey.empty() || NULL == pUI) return;

	// 1. ����Ʈ �����͸� �����..
	// 2. �����͸� �о�´�..
	// 3. ������ ��ȿ������ �Ǵ��Ѵ�..

	__WndInfo WI;
	WI.ptPosition.x = iDefaultX;
	WI.ptPosition.y = iDefaultY;
	if(false == RegGetSetting(szKey.c_str(), &WI, sizeof(__WndInfo))) WI.bVisible = true; // �⺻ �����Ͱ� ������ ������ ���̰� �Ѵ�..

	RECT rc = pUI->GetRegion();

	if (WI.ptPosition.x < 0) WI.ptPosition.x = 0;
	if (WI.ptPosition.x + (rc.right - rc.left) > CN3Base::s_CameraData.vp.Width)
		WI.ptPosition.x = CN3Base::s_CameraData.vp.Width - (rc.right - rc.left);
	if (WI.ptPosition.y < 0) WI.ptPosition.y = 0;
	if (WI.ptPosition.y + (rc.bottom - rc.top) > CN3Base::s_CameraData.vp.Height)
		WI.ptPosition.y = CN3Base::s_CameraData.vp.Height - (rc.bottom - rc.top);

	pUI->SetVisible(WI.bVisible);
	if(0 == WI.ptPosition.x && 0 == WI.ptPosition.y)
		pUI->SetPos(iDefaultX, iDefaultY);
	else pUI->SetPos(WI.ptPosition.x, WI.ptPosition.y);
}

void CGameProcedure::SetGameCursor(HCURSOR hCursor, bool bLocked)
{
	if(s_pGameCursor)
	{
		e_Cursor eCursor = CURSOR_KA_NORMAL;

		if(hCursor == s_hCursorNormal)
			eCursor = CURSOR_KA_NORMAL;
		else if(hCursor == s_hCursorNormal1)
			eCursor = CURSOR_EL_NORMAL;
		else if(hCursor == s_hCursorClick)
			eCursor = CURSOR_KA_CLICK;
		else if(hCursor == s_hCursorClick1)
			eCursor = CURSOR_EL_CLICK;
		else if(hCursor == s_hCursorAttack)
			eCursor = CURSOR_ATTACK;
		else if(hCursor == s_hCursorPreRepair)
			eCursor = CURSOR_PRE_REPAIR;
		else if(hCursor == s_hCursorNowRepair)
			eCursor = CURSOR_NOW_REPAIR;
		else if(hCursor == NULL)
			eCursor = CURSOR_UNKNOWN;

		SetGameCursor(eCursor, bLocked);

		if ((!m_bCursorLocked) && bLocked)
		{
			m_bCursorLocked = true;
		}

	}
	else
	{
		if ((m_bCursorLocked) && (!bLocked) ) return;
		else if ( ((m_bCursorLocked) && bLocked) || ((!m_bCursorLocked) && !bLocked) )
		{
			::SetCursor(hCursor);
			return;
		}
		else if ((!m_bCursorLocked) && bLocked)
		{
			m_hPrevGameCursor = ::GetCursor();
			m_bCursorLocked = true;
			::SetCursor(hCursor);
		}
	}
}

void CGameProcedure::SetGameCursor(e_Cursor eCursor, bool bLocked)
{
	if(s_pGameCursor == NULL) return;
	s_pGameCursor->SetGameCursor(eCursor, bLocked);
}

void CGameProcedure::RestoreGameCursor()
{
	if(s_pGameCursor)
	{
		if (m_bCursorLocked) 
			m_bCursorLocked = false;

		if(s_pGameCursor) s_pGameCursor->RestoreGameCursor();
	}
	else
	{
		if (m_bCursorLocked) 
			m_bCursorLocked = false;

		::SetCursor(m_hPrevGameCursor);	
	}
}

std::string CGameProcedure::GetStrRegKeySetting()
{
	char szBuff[256];
	sprintf(szBuff, "Software\\KnightOnline\\%s_%s_%d", s_szAccount.c_str(), s_szServer.c_str(), s_iChrSelectIndex);
	return szBuff;
}

bool CGameProcedure::ProcessPacket(DataPack* pDataPack, int& iOffset)
{
	int iCmd = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// Ŀ��� �Ľ�..
	switch ( iCmd )										// Ŀ��忡 �ٶ� �б�..
	{
		case N3_VERSION_CHECK: // ��ȣȭ�� ���� �޴´�..
			this->MsgRecv_VersionCheck(pDataPack, iOffset); // virtual
			return true;

		case N3_GAME_SERVER_LOGIN:
			this->MsgRecv_GameServerLogIn(pDataPack, iOffset);
			return true;

		case N3_SERVER_CHANGE:				// ���� �ٲٱ� �޽���..
		{
			// �ٸ� �� ������ �ٽ� �����Ѵ�.
			int iLen = 0;
			std::string szName, szIP;
//			iLen = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // ���� �̸�
//			CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szName, iLen);
			iLen = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // ���� IP
			CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szIP, iLen);
			DWORD dwPort = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			s_pPlayer->m_InfoExt.iZoneInit = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
			s_pPlayer->m_InfoExt.iZoneCur = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
			int iVictoryNation = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
			CGameProcedure::LoadingUIChange(iVictoryNation);

			s_bNeedReportConnectionClosed = false; // ���������� �������� �����ؾ� �ϴ���..
			s_pSocket->Disconnect(); // ����...
			Sleep(2000); // 2�� ������.. ������ ó���� �ð��� �ش�.
			int iErr = s_pSocket->Connect(s_hWndBase, szIP.c_str(), dwPort);
			s_bNeedReportConnectionClosed = true; // ���������� �������� �����ؾ� �ϴ���..

			if(iErr) this->ReportServerConnectionFailed("Current Zone", iErr, true); // ���� ���� ����.. Exit.
			else
			{
				// ����üũ�� ������.. �������� ������ ��ȣȭ Ű�� �´�.
				// ���� ���ν����� ��� Character_Select �� ������ �α����ϰ�� GameServer_LogIn �� ������.
				this->MsgSend_VersionCheck(); 
			}
		}
		return true;

		case N3_CHARACTER_SELECT:
		{
			this->MsgRecv_CharacterSelect(pDataPack, iOffset); // virtual
		}
		return true;

		case N3_ALIVE_CHECK:
		{
			this->MsgSend_AliveCheck();
		}
		return true;
	}

	return false;
}

void CGameProcedure::ReportServerConnectionFailed(const std::string& szServerName, int iErrCode, bool bNeedQuitGame)
{
	char szErr[256];
	std::string szFmt; ::_LoadStringFromResource(IDS_FMT_CONNECT_ERROR, szFmt);
	sprintf(szErr, szFmt.c_str(), szServerName.c_str(), iErrCode);
	
	e_Behavior eBehavior = ((bNeedQuitGame) ? BEHAVIOR_EXIT : BEHAVIOR_NOTHING);
	CGameProcedure::MessageBoxPost(szErr, "", MB_OK, eBehavior);
	return;
}

void CGameProcedure::ReportServerConnectionClosed(bool bNeedQuitGame)
{
	if(!s_bNeedReportConnectionClosed) return;

	std::string szMsg; ::_LoadStringFromResource(IDS_CONNECTION_CLOSED, szMsg);
	e_Behavior eBehavior = ((bNeedQuitGame) ? BEHAVIOR_EXIT : BEHAVIOR_NOTHING);
	CGameProcedure::MessageBoxPost(szMsg, "", MB_OK, eBehavior);

	if(s_pPlayer)
	{
		__Vector3 vPos = s_pPlayer->Position();
		CLogWriter::Write("Socket Closed... Zone(%d) Pos(%.1f, %.1f, %.1f) Exp(%d)",
			s_pPlayer->m_InfoExt.iZoneCur, vPos.x, vPos.y, vPos.z, s_pPlayer->m_InfoExt.iExp);
	}
	else
	{
		CLogWriter::Write("Socket Closed...");
	}

	if(s_pSocket) s_pSocket->Release();
}

void CGameProcedure::ReportDebugStringAndSendToServer(const std::string& szDebug)
{
	if(szDebug.empty()) return;

	CLogWriter::Write(szDebug.c_str());

	if(s_pSocket && s_pSocket->IsConnected())
	{
		int iLen = szDebug.size();
		std::vector<BYTE> buffer;	// ����.. 
		buffer.resize(iLen + 4);
		int iOffset=0;												// �ɼ�..
		s_pSocket->MP_AddByte(&(buffer[0]), iOffset, N3_REPORT_DEBUG_STRING);
		s_pSocket->MP_AddShort(&(buffer[0]), iOffset, iLen);
		s_pSocket->MP_AddString(&(buffer[0]), iOffset, szDebug);
		s_pSocket->Send(&(buffer[0]), iOffset);				// ����..
	}
}

void CGameProcedure::MsgSend_GameServerLogIn()
{
	BYTE byBuff[128];										// ��Ŷ ����..
	int iOffset = 0;										// ������ ������..

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_GAME_SERVER_LOGIN);	// Ŀ���.
	CAPISocket::MP_AddShort(byBuff, iOffset, s_szAccount.size());	// ���̵� ����..
	CAPISocket::MP_AddString(byBuff, iOffset, s_szAccount);			// ���� ���̵�..
	CAPISocket::MP_AddShort(byBuff, iOffset, s_szPassWord.size());	// �н����� ����
	CAPISocket::MP_AddString(byBuff, iOffset, s_szPassWord);		// ���� �н�����
		
	s_pSocket->Send(byBuff, iOffset);								// ������
}

void CGameProcedure::MsgSend_VersionCheck() // virtual
{
	// Version Check
	int iOffset = 0;
	BYTE byBuffs[4];
	CAPISocket::MP_AddByte(byBuffs, iOffset, N3_VERSION_CHECK);				// Ŀ���.
	s_pSocket->Send(byBuffs, iOffset);	// ������

#ifdef _CRYPTION
	s_pSocket->m_bEnableSend = FALSE; // ������ ����..?
#endif // #ifdef _CRYPTION
}

void CGameProcedure::MsgSend_CharacterSelect() // virtual
{
	BYTE byBuff[64];
	int iOffset = 0;
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_CHARACTER_SELECT);				// Ŀ���.
	CAPISocket::MP_AddShort(byBuff, iOffset, s_szAccount.size());				// ���� ����..
	CAPISocket::MP_AddString(byBuff, iOffset, s_szAccount);						// ���� ���ڿ�..
	CAPISocket::MP_AddShort(byBuff, iOffset, s_pPlayer->IDString().size());		// ĳ�� ���̵� ����..
	CAPISocket::MP_AddString(byBuff, iOffset, s_pPlayer->IDString());			// ĳ�� ���̵� ���ڿ�..
	CAPISocket::MP_AddByte(byBuff, iOffset, s_pPlayer->m_InfoExt.iZoneInit);	// ó�� �������� �ƴ��� 0x01:ó�� ����
	CAPISocket::MP_AddByte(byBuff, iOffset, s_pPlayer->m_InfoExt.iZoneCur);		// ĳ���� ����â������ ĳ���� �� ��ȣ
	s_pSocket->Send(byBuff, iOffset);	// ������

	CLogWriter::Write("MsgSend_CharacterSelect - name(%s) zone(%d)",
		s_pPlayer->IDString().c_str(), s_pPlayer->m_InfoExt.iZoneCur); // ����� �α�..
}

void CGameProcedure::MsgSend_AliveCheck()
{
	BYTE byBuff[4];
	int iOffset = 0;
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_ALIVE_CHECK);				// Ŀ���.
	s_pSocket->Send(byBuff, iOffset);	// ������
}

int CGameProcedure::MsgRecv_VersionCheck(DataPack* pDataPack, int& iOffset) // virtual
{
	int iVersion = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);	// ����
#ifdef _CRYPTION
	__int64 iPublicKey = CAPISocket::Parse_GetInt64(pDataPack->m_pData, iOffset); // ��ȣȭ ����Ű
	DataPack::InitCrypt(iPublicKey);
	s_pSocket->m_bEnableSend = TRUE; // ������ ����..?
#endif // #ifdef _CRYPTION

	if(iVersion != CURRENT_VERSION)
	{
		char szErr[256];

		int iLangID = ::GetUserDefaultLangID();
		if(0x0404 == iLangID)// Taiwan Language
		{
			std::string szFmt; ::_LoadStringFromResource(IDS_VERSION_CONFIRM_TW, szFmt);
			CGameProcedure::MessageBoxPost(szFmt, "", MB_OK, BEHAVIOR_EXIT);
		}
		else
		{
			std::string szFmt; ::_LoadStringFromResource(IDS_VERSION_CONFIRM, szFmt);
			sprintf(szErr, szFmt.c_str(), CURRENT_VERSION / 1000.0f, iVersion / 1000.0f);
			CGameProcedure::MessageBoxPost(szErr, "", MB_OK, BEHAVIOR_EXIT);
		}

	}

	return iVersion;
}

int CGameProcedure::MsgRecv_GameServerLogIn(DataPack* pDataPack, int& iOffset) // virtual
{
	int iNation = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // ���� - 0 ���� 0xff - ����..
	return iNation;
}

bool CGameProcedure::MsgRecv_CharacterSelect(DataPack* pDataPack, int& iOffset) // virtual
{
	int iResult = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // 0x00 ����
	if(1 == iResult) // ����..
	{
		int iZoneCur = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
		float fX = (CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;
		float fZ = (CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;
		float fY = (CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset))/10.0f;

		int iVictoryNation = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
		CGameProcedure::LoadingUIChange(iVictoryNation);

		int iZonePrev = 
		s_pPlayer->m_InfoExt.iZoneCur = iZoneCur;
		s_pPlayer->PositionSet(__Vector3(fX, fY, fZ), true);

		CLogWriter::Write("MsgRecv_CharacterSelect - name(%s) zone(%d -> %d)", s_pPlayer->m_InfoBase.szID.c_str(), iZonePrev, iZoneCur);
		return true;
	}
	else // ����
	{
		CLogWriter::Write("MsgRecv_CharacterSelect - failed(%d)", iResult);
		return false;
	}

	if(iResult) return true;
	else return false;
}

void CGameProcedure::ProcessUIKeyInput(bool bEnable)
{
	s_bKeyPressed	= false;	//Ű�� �ö����� ui���� �ش��ϴ� ���۵����� �ִٸ�

	if(!bEnable)
	{
		if(s_bKeyPress)
		{
			for(int i = 0; i < NUMDIKEYS; i++)
			{
				if(s_pLocalInput->IsKeyPressed(i))
				{
					if(!s_bKeyPressed) s_bKeyPress = false;
					break;
				}
			}
		}
		return;
	}


	CN3UIBase* pMsgBox = s_pMsgBoxMgr->GetFocusMsgBox();
	CN3UIBase* pUIFocus = s_pUIMgr->GetFocusedUI();

	if(pMsgBox && pMsgBox->IsVisible())//this_ui
	{
		for(int i = 0; i < NUMDIKEYS; i++)
		{
			if(s_pLocalInput->IsKeyPress(i))
				s_bKeyPress |= pMsgBox->OnKeyPress(i);
			if(s_pLocalInput->IsKeyPressed(i))
				s_bKeyPressed |= pMsgBox->OnKeyPressed(i);
		}
	}
	else if(pUIFocus && pUIFocus->IsVisible()) // ��Ŀ�� �� UI �� ������...
	{
		for(int i = 0; i < NUMDIKEYS; i++)
		{
			if(s_pLocalInput->IsKeyPress(i))
			{
				if(pUIFocus->m_pChildUI && pUIFocus->m_pChildUI->IsVisible())
					s_bKeyPress |= pUIFocus->m_pChildUI->OnKeyPress(i);
				else
					s_bKeyPress |= pUIFocus->OnKeyPress(i);
			}
			if(s_pLocalInput->IsKeyPressed(i))
			{
				if(pUIFocus->m_pChildUI && pUIFocus->m_pChildUI->IsVisible())
					s_bKeyPressed |= pUIFocus->m_pChildUI->OnKeyPressed(i);
				else
					s_bKeyPressed |= pUIFocus->OnKeyPressed(i);
			}
		}
	}

	if(s_bKeyPress)
	{
		for(int i = 0; i < NUMDIKEYS; i++)
		{
			if(s_pLocalInput->IsKeyPressed(i))
			{
				if(!s_bKeyPressed) s_bKeyPress = false;
				break;
			}
		}
	}
}

bool CGameProcedure::IsUIKeyOperated()
{
	if (!s_bKeyPress && !s_bKeyPressed)
		return false;

	return true;
}

void CGameProcedure::LoadingUIChange(int iVictoryNation)
{
	if(s_pPlayer->m_InfoExt.iVictoryNation == iVictoryNation)
		return;

	s_pPlayer->m_InfoExt.iVictoryNation = iVictoryNation;

	std::string szLoading;
	if(s_pUILoading) delete s_pUILoading; s_pUILoading = NULL;		// Loading Bar

	s_pUILoading = new CUILoading();
	__ASSERT(s_pUILoading, "�ε�ȭ�� ���� ����");
	if(s_pUILoading == NULL) return;

	__TABLE_UI_RESRC* pTblUI = s_pTbl_UI->Find(NATION_ELMORAD); // �⺻�� ������ UI �� �Ѵ�..
	__ASSERT(pTblUI, "�⺻ UI �� �����ϴ�.");
	if(pTblUI == NULL) return;

	switch(iVictoryNation)
	{
	case VICTORY_ABSENCE:
		szLoading = pTblUI->szLoading;
		break;
	case VICTORY_ELMORAD:
		szLoading = pTblUI->szElLoading;
		break;
	case VICTORY_KARUS:
		szLoading = pTblUI->szKaLoading;
		break;
	default:
		szLoading = pTblUI->szLoading;
		break;
	}

	TRACE("Loading UIF : %s\n", szLoading.c_str());
	s_pUILoading->LoadFromFile(szLoading); // �⺻���� �ε� �� �����..
}
