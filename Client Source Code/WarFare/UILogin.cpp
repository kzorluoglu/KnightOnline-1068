#include "stdafx.h"
#include "resource.h"
#include "UILogIn.h"
#include "GameProcLogIn.h"

#include "../N3Base/N3UIEdit.h"
#include "../N3Base/N3UIButton.h"
#include "../N3Base/N3UIList.h"
#include "../N3Base/N3UIImage.h"
#include "../N3Base/N3UIString.h"
#include "UIMessageBoxManager.h"

#include <algorithm>
#include <functional>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUILogIn::CUILogIn()
{
	m_pEdit_id = NULL;
	m_pEdit_pw = NULL;
	
	m_pBtn_LogIn = NULL;
	m_pBtn_Connect = NULL;
	m_pBtn_Cancel = NULL;
	m_pBtn_Option = NULL;

	m_pGroup_ServerList = NULL;
	m_pGroup_LogIn = NULL;
	
	m_pText_Rights = NULL;
	m_pImg_MGameLogo = NULL;
	m_pImg_DaumLogo = NULL;
	m_pImg_GradeLogo = NULL;

	m_pList_Server = NULL;
	
	m_bOpenningNow = false; // ������ �Ʒ��� ������...������ �Ѵٸ�..
	m_fMoveDelta = 0;

	m_bLogIn = false;
}

CUILogIn::~CUILogIn()
{

}

bool CUILogIn::ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg)
{
	if(NULL == pSender) return false;

	//s_CameraData.vp;  //�ҷ� ���� ������ ���캻�� 
	//DWORD mm = s_CameraData.vp.Height;
	//DWORD ss = s_CameraData.vp.Width;	

	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (pSender == m_pBtn_LogIn && m_pEdit_id && m_pEdit_pw)
		{
			CGameProcedure::s_pProcLogIn->MsgSend_AccountLogIn(LIC_KNIGHTONLINE);
		}
		else if(pSender == m_pBtn_Connect)
		{
			CGameProcedure::s_pProcLogIn->ConnectToGameServer(); // ���� ���� ������ ����
		}
		else if (pSender == m_pBtn_Cancel)
		{
			PostQuitMessage(0);	// ����...
		}
		else if(pSender == m_pBtn_Option) // �ɼ�..
		{
			std::string szMsg;
			::_LoadStringFromResource(IDS_CONFIRM_EXECUTE_OPTION, szMsg);
			CGameProcedure::MessageBoxPost(szMsg, "", MB_YESNO, BEHAVIOR_EXECUTE_OPTION);
		}
	}
	else if(UIMSG_LIST_DBLCLK == dwMsg)
	{
		CGameProcedure::s_pProcLogIn->ConnectToGameServer(); // ���� ���� ������ ����
	}
	else if (dwMsg == UIMSG_EDIT_RETURN)
	{
		if(!m_bLogIn && m_pEdit_id && m_pEdit_pw)
		{
			CN3UIBase* pMsgBox = CGameProcedure::s_pMsgBoxMgr->GetFocusMsgBox();
			if( !(pMsgBox && pMsgBox->IsVisible()) )
				CGameProcedure::s_pProcLogIn->MsgSend_AccountLogIn(LIC_KNIGHTONLINE);
		}
	}

	return true;
}

void CUILogIn::LogAllUIElements(CN3UIBase* pUI, int depth)
{
	if(!pUI) return;
	
	std::string indent(depth * 2, ' ');
	if(!pUI->m_szID.empty())
	{
		CLogWriter::Write("CUILogIn::Load - %sID='%s' Type=%d Visible=%d", indent.c_str(), pUI->m_szID.c_str(), pUI->UIType(), pUI->IsVisible());
	}
	else
	{
		CLogWriter::Write("CUILogIn::Load - %s(no ID) Type=%d Visible=%d", indent.c_str(), pUI->UIType(), pUI->IsVisible());
	}
	
	UIListItor it = pUI->m_Children.begin();
	UIListItor itEnd = pUI->m_Children.end();
	for(; it != itEnd; ++it)
	{
		CN3UIBase* pChild = *it;
		LogAllUIElements(pChild, depth + 1);
	}
}

bool CUILogIn::Load(HANDLE hFile)
{
	if(CN3UIBase::Load(hFile)==false) return false;

	// Log all child IDs for debugging
	CLogWriter::Write("CUILogIn::Load - Enumerating all child IDs:");
	LogAllUIElements(this, 0);

	// Try to find GroupLogin by ID first
	m_pGroup_LogIn = GetChildByID("GroupLogin");
	if(m_pGroup_LogIn)
	{
		CLogWriter::Write("CUILogIn::Load - Found GroupLogin, setting visible");
		m_pGroup_LogIn->SetVisible(true);
		m_pBtn_LogIn = (CN3UIButton*)m_pGroup_LogIn->GetChildByID("Btn_LogIn");
		if(m_pBtn_LogIn) { CLogWriter::Write("CUILogIn::Load - Found Btn_LogIn, setting visible"); m_pBtn_LogIn->SetVisible(true); }
		else CLogWriter::Write("CUILogIn::Load - Btn_LogIn not found in GroupLogin");
		m_pBtn_Cancel = (CN3UIButton*)m_pGroup_LogIn->GetChildByID("Btn_Cancel");
		if(m_pBtn_Cancel) { CLogWriter::Write("CUILogIn::Load - Found Btn_Cancel, setting visible"); m_pBtn_Cancel->SetVisible(true); }
		else CLogWriter::Write("CUILogIn::Load - Btn_Cancel not found in GroupLogin");
		m_pBtn_Option = (CN3UIButton*)m_pGroup_LogIn->GetChildByID("Btn_Option");
		if(m_pBtn_Option) { CLogWriter::Write("CUILogIn::Load - Found Btn_Option, setting visible"); m_pBtn_Option->SetVisible(true); }
		else CLogWriter::Write("CUILogIn::Load - Btn_Option not found in GroupLogin");
		m_pEdit_id = (CN3UIEdit*)m_pGroup_LogIn->GetChildByID("Edit_ID");
		if(m_pEdit_id) { CLogWriter::Write("CUILogIn::Load - Found Edit_ID, setting visible"); m_pEdit_id->SetVisible(true); }
		else CLogWriter::Write("CUILogIn::Load - Edit_ID not found in GroupLogin");
		m_pEdit_pw = (CN3UIEdit*)m_pGroup_LogIn->GetChildByID("Edit_PW");
		if(m_pEdit_pw) { CLogWriter::Write("CUILogIn::Load - Found Edit_PW, setting visible"); m_pEdit_pw->SetVisible(true); }
		else CLogWriter::Write("CUILogIn::Load - Edit_PW not found in GroupLogin");
	}
	else
	{
		CLogWriter::Write("CUILogIn::Load - GroupLogin not found, trying direct IDs");
		m_pBtn_LogIn = (CN3UIButton*)GetChildByID("Btn_LogIn");
		if(!m_pBtn_LogIn) m_pBtn_LogIn = (CN3UIButton*)GetChildByID("btn_login");
		if(m_pBtn_LogIn) { CLogWriter::Write("CUILogIn::Load - Found Btn_LogIn/btn_login, setting visible"); m_pBtn_LogIn->SetVisible(true); }
		else CLogWriter::Write("CUILogIn::Load - Btn_LogIn/btn_login not found");
		m_pBtn_Cancel = (CN3UIButton*)GetChildByID("Btn_Cancel");
		if(!m_pBtn_Cancel) m_pBtn_Cancel = (CN3UIButton*)GetChildByID("btn_cancel");
		if(m_pBtn_Cancel) { CLogWriter::Write("CUILogIn::Load - Found Btn_Cancel/btn_cancel, setting visible"); m_pBtn_Cancel->SetVisible(true); }
		else CLogWriter::Write("CUILogIn::Load - Btn_Cancel/btn_cancel not found");
		m_pBtn_Option = (CN3UIButton*)GetChildByID("Btn_Option");
		if(!m_pBtn_Option) m_pBtn_Option = (CN3UIButton*)GetChildByID("btn_option");
		if(m_pBtn_Option) { CLogWriter::Write("CUILogIn::Load - Found Btn_Option/btn_option, setting visible"); m_pBtn_Option->SetVisible(true); }
		else CLogWriter::Write("CUILogIn::Load - Btn_Option/btn_option not found");
		m_pEdit_id = (CN3UIEdit*)GetChildByID("Edit_ID");
		if(!m_pEdit_id) m_pEdit_id = (CN3UIEdit*)GetChildByID("edit_id");
		if(m_pEdit_id) { CLogWriter::Write("CUILogIn::Load - Found Edit_ID/edit_id, setting visible"); m_pEdit_id->SetVisible(true); }
		else CLogWriter::Write("CUILogIn::Load - Edit_ID/edit_id not found");
		m_pEdit_pw = (CN3UIEdit*)GetChildByID("Edit_PW");
		if(!m_pEdit_pw) m_pEdit_pw = (CN3UIEdit*)GetChildByID("edit_pw");
		if(m_pEdit_pw) { CLogWriter::Write("CUILogIn::Load - Found Edit_PW/edit_pw, setting visible"); m_pEdit_pw->SetVisible(true); }
		else CLogWriter::Write("CUILogIn::Load - Edit_PW/edit_pw not found");
	}

	if (!m_pEdit_id) {
		m_pEdit_id = new CN3UIEdit();
		m_pEdit_id->Init(this);
		m_pEdit_id->SetID("edit_id");
		AddChild(m_pEdit_id);
		m_pEdit_id->SetVisible(true);
		RECT rc = {100, 100, 200, 120};
		m_pEdit_id->SetRegion(rc);
		CLogWriter::Write("CUILogIn::Load - Created edit_id");
	}
	if (!m_pEdit_pw) {
		m_pEdit_pw = new CN3UIEdit();
		m_pEdit_pw->Init(this);
		m_pEdit_pw->SetID("edit_pw");
		AddChild(m_pEdit_pw);
		m_pEdit_pw->SetVisible(true);
		RECT rc = {100, 130, 200, 150};
		m_pEdit_pw->SetRegion(rc);
		CLogWriter::Write("CUILogIn::Load - Created edit_pw");
	}
	if (!m_pBtn_LogIn) {
		m_pBtn_LogIn = new CN3UIButton();
		m_pBtn_LogIn->Init(this);
		m_pBtn_LogIn->SetID("btn_login");
		AddChild(m_pBtn_LogIn);
		m_pBtn_LogIn->SetVisible(true);
		RECT rc = {100, 160, 150, 180};
		m_pBtn_LogIn->SetRegion(rc);
		CLogWriter::Write("CUILogIn::Load - Created btn_login");
	}

	m_pText_Rights = GetChildByID("Text_Rights");
	m_pImg_MGameLogo = GetChildByID("Img_MGame");
	m_pImg_DaumLogo = GetChildByID("Img_Daum");

	if(m_pText_Rights) m_pText_Rights->SetVisible(false);
	if(m_pImg_MGameLogo) m_pImg_MGameLogo->SetVisible(false);
	if(m_pImg_DaumLogo) m_pImg_DaumLogo->SetVisible(false);

	m_pGroup_ServerList = GetChildByID("Group_ServerList");
	if(m_pGroup_ServerList)
	{
		m_pList_Server = (CN3UIList*)(m_pGroup_ServerList->GetChildByID("List_Server"));
		m_pBtn_Connect = (CN3UIButton*)m_pGroup_ServerList->GetChildByID("Btn_Connect");

		m_pGroup_ServerList->SetVisible(false);
	}

	return true;
}

void CUILogIn::AccountIDGet(std::string& szID)
{
	if(m_pEdit_id) 
		szID = m_pEdit_id->GetString();
	else
		szID = "";
}

void CUILogIn::AccountPWGet(std::string& szPW)
{
	if(m_pEdit_pw)
		szPW = m_pEdit_pw->GetString();
	else
		szPW = "";
}

void CUILogIn::ConnectButtonSetEnable(bool bEnable)
{
	eUI_STATE eState1 = (bEnable ? UI_STATE_BUTTON_NORMAL : UI_STATE_BUTTON_DISABLE);
	eUI_STATE eState2 = (bEnable ? UI_STATE_LIST_ENABLE : UI_STATE_LIST_DISABLE);
	
	if(m_pBtn_Connect) m_pBtn_Connect->SetState(eState1);
	if(m_pList_Server) m_pList_Server->SetState(eState2);
}

void CUILogIn::FocusToID()
{
	if(m_pEdit_id) m_pEdit_id->SetFocus();
}

void CUILogIn::FocusCircular()
{
	if(NULL == m_pEdit_id || NULL == m_pEdit_pw) return;

	if(m_pEdit_id->HaveFocus())
		m_pEdit_pw->SetFocus();
	else
		m_pEdit_id->SetFocus();
}

void CUILogIn::InitEditControls()
{
	if(m_pEdit_id)
	{
		m_pEdit_id->SetString("");
		m_pEdit_id->SetFocus();
	}
	
	if(m_pEdit_pw)
	{
		m_pEdit_pw->SetString("");
	}
}

bool CUILogIn::ServerInfoAdd(const __GameServerInfo& GSI)
{
	m_ListServerInfos.push_back(GSI);
#ifdef _N3GAME
	CLogWriter::Write("CUILogIn::ServerInfoAdd - added server '%s' ip='%s' users=%d", GSI.szName.c_str(), GSI.szIP.c_str(), GSI.iConcurrentUserCount);
#else
	CLogWriter::Write("CUILogIn::ServerInfoAdd - added server '%s' ip='%s' users=%d", GSI.szName.c_str(), GSI.szIP.c_str(), GSI.iConcurrentUserCount);
#endif
	return true;
}

bool CUILogIn::ServerInfoGet(int iIndex, __GameServerInfo& GSI)
{
	if(NULL == m_pList_Server) return false;
	if(iIndex < 0 || iIndex >= (int)m_ListServerInfos.size()) return false;

	GSI = m_ListServerInfos[iIndex];
	return true;
}

bool CUILogIn::ServerInfoGetCur(__GameServerInfo& GSI)
{
	GSI.Init();
	if(NULL == m_pList_Server) return false;

	int iIndex = m_pList_Server->GetCurSel();
	return this->ServerInfoGet(iIndex, GSI);
}

void CUILogIn::ServerInfoUpdate()
{
	if(NULL == m_pList_Server) return;

	m_pList_Server->ResetContent();
	if(!m_ListServerInfos.empty())
	{
		sort(m_ListServerInfos.begin(), m_ListServerInfos.end(), not2(__GameServerInfo()));
		int iSize = m_ListServerInfos.size();
		for(int i = 0; i < iSize; i++)
		{
			m_pList_Server->AddString(m_ListServerInfos[i].szName);
		}
	}
#ifdef _N3GAME
	CLogWriter::Write("CUILogIn::ServerInfoUpdate - list count=%d", (m_pList_Server?m_pList_Server->GetCount():0));
	if(m_pGroup_ServerList)
	{
		POINT pt = m_pGroup_ServerList->GetPos(); RECT rc = m_pGroup_ServerList->GetRegion();
		CLogWriter::Write("CUILogIn::ServerInfoUpdate - Group_ServerList pos=(%d,%d) region=(%d,%d,%d,%d) visible=%d",
			pt.x, pt.y, rc.left, rc.top, rc.right, rc.bottom, m_pGroup_ServerList->IsVisible());
	}
#else
	CLogWriter::Write("CUILogIn::ServerInfoUpdate - list count=%d", (m_pList_Server?m_pList_Server->GetCount():0));
	if(m_pGroup_ServerList)
	{
		POINT pt = m_pGroup_ServerList->GetPos(); RECT rc = m_pGroup_ServerList->GetRegion();
		CLogWriter::Write("CUILogIn::ServerInfoUpdate - Group_ServerList pos=(%d,%d) region=(%d,%d,%d,%d) visible=%d",
			pt.x, pt.y, rc.left, rc.top, rc.right, rc.bottom, m_pGroup_ServerList->IsVisible());
	}
#endif
}

void CUILogIn::Tick()
{
	CN3UIBase::Tick();

	if(m_pGroup_ServerList)
	{
		if(m_bOpenningNow) // ������ �Ʒ��� ������...������ �Ѵٸ�..
		{
			RECT rc = m_pGroup_ServerList->GetRegion();
			float fHeight = (float)(rc.bottom - rc.top);

			float fDelta = 5000.0f * CN3Base::s_fSecPerFrm;
			fDelta *= (fHeight - m_fMoveDelta) / fHeight;
			if(fDelta < 2.0f) fDelta = 2.0f;
			m_fMoveDelta += fDelta;

			int iYLimit = 0;
			POINT ptCur = m_pGroup_ServerList->GetPos();
			ptCur.y = (int)(m_fMoveDelta - fHeight);
			if(ptCur.y >= iYLimit) // �ٿ��ȴ�!!
			{
				ptCur.y = iYLimit;
				m_bOpenningNow = false;
			}

			m_pGroup_ServerList->SetPos(ptCur.x, ptCur.y);
		}
	}
}

void CUILogIn::OpenServerList()
{
	if(m_bOpenningNow || NULL == m_pGroup_ServerList) return;

	// ������ ������!!
	m_pGroup_ServerList->SetVisible(true);
	RECT rc = m_pGroup_ServerList->GetRegion();
	m_pGroup_ServerList->SetPos(0, -(rc.bottom - rc.top));
	CLogWriter::Write("CUILogIn::OpenServerList - SetVisible(true), initial pos=(0,%d), region=(%d,%d,%d,%d)",
		-(rc.bottom - rc.top), rc.left, rc.top, rc.right, rc.bottom);
	
	m_fMoveDelta = 0;
	m_bOpenningNow = true;
}

void CUILogIn::SetVisibleLogInUIs(bool bEnable)
{
	if(m_pGroup_LogIn) m_pGroup_LogIn->SetVisible(bEnable); // �α����� �����..
	
	// �α����� ������ ���п� ���� UI ������...
	if(m_pText_Rights) m_pText_Rights->SetVisible(false);
	if(m_pImg_MGameLogo) m_pImg_MGameLogo->SetVisible(false);
	if(m_pImg_DaumLogo) m_pImg_DaumLogo->SetVisible(false);
	
	if(false == bEnable)
	{
		if(LIC_MGAME == CGameProcedure::s_eLogInClassification)
		{
			if(m_pText_Rights && m_pImg_MGameLogo)
			{
				// �Ʒ��� �ߴ����� �����..
				RECT rcView = { 0, 0, (LONG)s_CameraData.vp.Width, (LONG)s_CameraData.vp.Height };
				int iX = (rcView.right - (m_pText_Rights->GetWidth() + m_pImg_MGameLogo->GetWidth()))/2;
				int iY = rcView.bottom - m_pText_Rights->GetHeight() - 20;
				m_pText_Rights->SetPos(iX, iY);
				RECT rc = m_pText_Rights->GetRegion();
				m_pImg_MGameLogo->SetPos(rc.right, rcView.bottom - m_pImg_MGameLogo->GetHeight() - 20);

				m_pText_Rights->SetVisible(true);
				m_pImg_MGameLogo->SetVisible(true);
			}
		}
		else if(LIC_DAUM == CGameProcedure::s_eLogInClassification)
		{
			if(m_pText_Rights && m_pImg_DaumLogo)
			{
				// �Ʒ��� �ߴ����� �����..
				RECT rcView = { 0, 0, (LONG)s_CameraData.vp.Width, (LONG)s_CameraData.vp.Height };
				int iX = (rcView.right - (m_pText_Rights->GetWidth() + m_pImg_DaumLogo->GetWidth()))/2;
				int iY = rcView.bottom - m_pText_Rights->GetHeight() - 20;
				m_pText_Rights->SetPos(iX, iY);
				RECT rc = m_pText_Rights->GetRegion();
				m_pImg_DaumLogo->SetPos(rc.right, rcView.bottom - m_pImg_DaumLogo->GetHeight() - 20);

				m_pText_Rights->SetVisible(true);
				m_pImg_DaumLogo->SetVisible(true);
			}
		}
		else
		{
			if(m_pText_Rights)
			{
				RECT rcView = { 0, 0, (LONG)s_CameraData.vp.Width, (LONG)s_CameraData.vp.Height };
				int iX = (rcView.right - m_pText_Rights->GetWidth())/2;
				int iY = rcView.bottom - m_pText_Rights->GetHeight() - 20;
				m_pText_Rights->SetPos(iX, iY);

				m_pText_Rights->SetVisible(true);
			}
		}
	}
}

void CUILogIn::RecalcGradePos()
{
	if(m_pImg_GradeLogo) // �̿��� ǥ��
	{
		RECT rc = m_pImg_GradeLogo->GetRegion();
		int iX = s_CameraData.vp.Width - (rc.right - rc.left + 10);
		int iY = 10;
		m_pImg_GradeLogo->SetPos(iX, iY);
	}
}

bool CUILogIn::OnKeyPress(int iKey)
{
	if(!m_bLogIn)
	{
		switch(iKey)
		{
		case DIK_TAB:
			FocusCircular();
			return true;
//		case DIK_NUMPADENTER:
//		case DIK_RETURN:
//			CGameProcedure::s_pProcLogIn->MsgSend_AccountLogIn(LIC_KNIGHTONLINE);
//			return true;
		}
	}
	else if(!m_bOpenningNow && m_pGroup_ServerList && m_pGroup_ServerList->IsVisible())
	{
		switch(iKey)
		{
		case DIK_UP:
			{
				if(NULL == m_pList_Server) return false;

				int iIndex = m_pList_Server->GetCurSel();

				if(iIndex > 0) m_pList_Server->SetCurSel(iIndex - 1);
				int iCnt = m_pList_Server->GetCount();
			}
			return true;
		case DIK_DOWN:
			{
				if(NULL == m_pList_Server) return false;

				int iIndex = m_pList_Server->GetCurSel();
				int iCnt = m_pList_Server->GetCount();

				if(iCnt - iIndex > 1) m_pList_Server->SetCurSel(iIndex + 1);
			}
			return true;
		case DIK_NUMPADENTER:
		case DIK_RETURN:
			ReceiveMessage(m_pBtn_Connect, UIMSG_BUTTON_CLICK);
			return true;
		}
	}

	return CN3UIBase::OnKeyPress(iKey);
}

void CUILogIn::FindUIElementsRecursive(CN3UIBase* pBase, std::vector<CN3UIEdit*>& edits, std::vector<CN3UIButton*>& buttons)
{
	if(!pBase) return;

	CLogWriter::Write("CUILogIn::FindUIElementsRecursive - Checking base type=%d with %d children", pBase->UIType(), pBase->GetChildrenCount());

	if(pBase->UIType() == UI_TYPE_EDIT)
	{
		edits.push_back((CN3UIEdit*)pBase);
		CLogWriter::Write("CUILogIn::FindUIElementsRecursive - Found edit");
	}
	else if(pBase->UIType() == UI_TYPE_BUTTON)
	{
		buttons.push_back((CN3UIButton*)pBase);
		CLogWriter::Write("CUILogIn::FindUIElementsRecursive - Found button");
	}

	// Recurse into children
	int childCount = pBase->GetChildrenCount();
	for(int i = 0; i < childCount; ++i)
	{
		CN3UIBase* pChild = pBase->GetChildByIndex(i);
		if(pChild)
		{
			FindUIElementsRecursive(pChild, edits, buttons);
		}
	}
}
