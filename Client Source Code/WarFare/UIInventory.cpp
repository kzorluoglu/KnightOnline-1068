// UIInventory.cpp: implementation of the CUIInventory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "PlayerMySelf.h"
#include "PacketDef.h"
#include "LocalInput.h"
#include "APISocket.h"
#include "GameProcMain.h"

#include "UIInventory.h"
#include "UITransactionDlg.h"
#include "UIImageTooltipDlg.h"
#include "UIManager.h"

#include "SubProcPerTrade.h"
#include "UITradeEditDlg.h"
#include "UIPerTradeDlg.h"

#include "CountableItemEditDlg.h"
#include "UIRepairTooltipDlg.h"

#include "UIHotKeyDlg.h"
#include "UISkillTreeDlg.h"

#include "../N3Base/N3UIString.h"
#include "../N3Base/N3UIEdit.h"
#include "../N3Base/N3SndObj.h"
#include "Resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static bool g_bItemClassGroup[26][26] = {	// [������][�÷��̾�]	
//	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }	// �ʱ�׷�
//	  0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
	{ 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 },	// 0�� Group
	{ 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1 },	// 1�� Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },	// 2�� Group
	{ 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1 },	// 3�� Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },	// 4�� Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },	// 5�� Group
	{ 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0 },	// 6�� Group
	{ 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },	// 7�� Group
	{ 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },	// 8�� Group
	{ 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1 },	// 9�� Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0 },	// 10�� Group
	{ 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1 },	// 11�� Group
	{ 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1 },	// 12�� Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0 },	// 13�� Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },	// 14�� Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },	// 15�� Group
	{ 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0 },	// 16�� Group
	{ 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0 },	// 17�� Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },	// 18�� Group
	{ 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0 },	// 19�� Group
	{ 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0 },	// 20�� Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1 },	// 21�� Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1 },	// 22�� Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1 },	// 23�� Group
	{ 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0 },	// 24�� Group
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } };	// 25�� Group (��� Class)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIInventory::CUIInventory()
{
	for( int i = 0; i < ITEM_SLOT_COUNT; i++ )	m_pMySlot[i] = NULL;
	for( int i = 0; i < MAX_ITEM_INVENTORY; i++ )	m_pMyInvWnd[i] = NULL;

	m_pUITooltipDlg = NULL;
	CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer = false;

	m_bOpenningNow = false; // ������ �ִ�..
	m_bClosingNow = false;	// ������ �ִ�..
	m_fMoveDelta = 0; // �ε巴�� ������ ������ ����� ���ؼ� ������ġ ��꿡 �ε��Ҽ����� ����..

	m_bDestoyDlgAlive	= false;
	m_pText_Weight = NULL;

	m_iRBtnDownOffs = -1;
	m_bRBtnProcessing = false;
}

CUIInventory::~CUIInventory()
{
	Release();
}

void CUIInventory::Release()
{
	CN3UIBase::Release();

	for( int i = 0; i < ITEM_SLOT_COUNT; i++ )
	{
		if ( m_pMySlot[i] != NULL )
		{
			delete m_pMySlot[i];
			m_pMySlot[i] = NULL;
		}
	}

	for( int i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if ( m_pMyInvWnd[i] != NULL )
		{
			delete m_pMyInvWnd[i];
			m_pMyInvWnd[i] = NULL;
		}
	}

	m_bOpenningNow = false; // ������ �ִ�..
	m_bClosingNow = false;	// ������ �ִ�..
	m_fMoveDelta = 0; // �ε巴�� ������ ������ ����� ���ؼ� ������ġ ��꿡 �ε��Ҽ����� ����..
	m_pText_Weight = NULL;
}

bool CUIInventory::HasAnyItemInSlot()
{
	for( int i = 0; i < ITEM_SLOT_COUNT; i++ )
	{
		if ( m_pMySlot[i] != NULL )
			return true;
	}
	return false;	
}

void CUIInventory::ReleaseItem()
{
	for( int i = 0; i < ITEM_SLOT_COUNT; i++ )
	{
		if ( m_pMySlot[i] != NULL )
		{
			if ( m_pMySlot[i]->pUIIcon )
			{
				RemoveChild(m_pMySlot[i]->pUIIcon);
				m_pMySlot[i]->pUIIcon->Release();
				delete m_pMySlot[i]->pUIIcon;
				m_pMySlot[i]->pUIIcon = NULL;
			}

			delete m_pMySlot[i];	
			m_pMySlot[i] = NULL;
		}
	}

	for( int i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if ( m_pMyInvWnd[i] != NULL )
		{
			if ( m_pMyInvWnd[i]->pUIIcon )
			{
				RemoveChild(m_pMyInvWnd[i]->pUIIcon);
				m_pMyInvWnd[i]->pUIIcon->Release();
				delete m_pMyInvWnd[i]->pUIIcon;
				m_pMyInvWnd[i]->pUIIcon = NULL;
			}

			delete m_pMyInvWnd[i];
			m_pMyInvWnd[i] = NULL;
		}
	}
}

void CUIInventory::Open(e_InvenState eIS)
{
	m_eInvenState = eIS;
	if ( eIS == INV_STATE_REPAIR )
	{
		CGameProcedure::SetGameCursor(CGameProcedure::s_hCursorPreRepair, true);
	}

	CN3UIString* pStatic = (CN3UIString* )GetChildByID("text_gold"); __ASSERT(pStatic, "NULL UI Component!!");
	if(pStatic)
	{
		pStatic->SetStringAsInt(CGameBase::s_pPlayer->m_InfoExt.iGold);
	}
	
	// ������ ������!!
	SetVisible(true);
	this->SetPos(CN3Base::s_CameraData.vp.Width, 10);
	m_fMoveDelta = 0;
	m_bOpenningNow = true;
	m_bClosingNow = false;

	m_iRBtnDownOffs = -1;
}

void CUIInventory::GoldUpdate()
{
	CN3UIString* pStatic = (CN3UIString* )GetChildByID("text_gold"); __ASSERT(pStatic, "NULL UI Component!!");
	if(pStatic)
	{
		pStatic->SetStringAsInt(CGameBase::s_pPlayer->m_InfoExt.iGold);
	}
}

void CUIInventory::Close(bool bByKey)
{
	if ( m_eInvenState == INV_STATE_REPAIR )
	{
		if (bByKey) return;
		CGameProcedure::RestoreGameCursor();

		if (CGameProcedure::s_pProcMain->m_pUIRepairTooltip->IsVisible())
		{
			CGameProcedure::s_pProcMain->m_pUIRepairTooltip->m_bBRender = false;
			CGameProcedure::s_pProcMain->m_pUIRepairTooltip->DisplayTooltipsDisable();
		}
	}

	m_eInvenState = INV_STATE_NORMAL;

	if (GetState() == UI_STATE_ICON_MOVING)
		IconRestore();
	SetState(UI_STATE_COMMON_NONE);
	CN3UIWndBase::AllHighLightIconFree();

	// ������ ������..!!
//	SetVisible(false); // �� ������ ���� ���ش�..
	RECT rc = this->GetRegion();
	this->SetPos(CN3Base::s_CameraData.vp.Width - (rc.right - rc.left), 10);
	m_fMoveDelta = 0;
	m_bOpenningNow = false;
	m_bClosingNow = true;

	if(m_pSnd_CloseUI) m_pSnd_CloseUI->Play(); // �ݴ� �Ҹ�..

	m_iRBtnDownOffs = -1;
}

void CUIInventory::Tick()
{
	if (!m_bVisible) return;	// ������ ������ �ڽĵ��� tick���� �ʴ´�.

	if(m_bOpenningNow) // �����ʿ��� �������� ������...������ �Ѵٸ�..
	{
		POINT ptCur = this->GetPos();
		RECT rc = this->GetRegion();
		float fWidth = (float)(rc.right - rc.left);

		float fDelta = 5000.0f * CN3Base::s_fSecPerFrm;
		fDelta *= (fWidth - m_fMoveDelta) / fWidth;
		if(fDelta < 2.0f) fDelta = 2.0f;
		m_fMoveDelta += fDelta;

		int iXLimit = CN3Base::s_CameraData.vp.Width - (int)fWidth;
		ptCur.x = CN3Base::s_CameraData.vp.Width - (int)m_fMoveDelta;
		if(ptCur.x <= iXLimit) // �ٿ��ȴ�!!
		{
			ptCur.x = iXLimit;
			m_bOpenningNow = false;
		}

		this->SetPos(ptCur.x, ptCur.y);

		CN3UIWndBase::AllHighLightIconFree();
	}
	else if(m_bClosingNow) // �����ʿ��� �������� ������...������ �Ѵٸ�..
	{
		POINT ptCur = this->GetPos();
		RECT rc = this->GetRegion();
		float fWidth = (float)(rc.right - rc.left);

		float fDelta = 5000.0f * CN3Base::s_fSecPerFrm;
		fDelta *= (fWidth - m_fMoveDelta) / fWidth;
		if(fDelta < 2.0f) fDelta = 2.0f;
		m_fMoveDelta += fDelta;

		int iXLimit = CN3Base::s_CameraData.vp.Width;
		ptCur.x = CN3Base::s_CameraData.vp.Width - (int)(fWidth - m_fMoveDelta);
		if(ptCur.x >= iXLimit) // �� ������..!!
		{
			ptCur.x = iXLimit;
			m_bClosingNow = false;

			this->SetVisibleWithNoSound(false, false, true); // �� �������� ������ �Ⱥ��̰� �Ѵ�.
		}

		this->SetPos(ptCur.x, ptCur.y);

		CN3UIWndBase::AllHighLightIconFree();
	}

	CGameBase::s_pPlayer->InventoryChrTick();
	CN3UIBase::Tick();
	CGameBase::s_pPlayer->m_ChrInv.m_nLOD = 1;

	m_cItemRepairMgr.Tick();
}

void CUIInventory::Render()
{
	if (!m_bVisible) return;	// ������ ������ �ڽĵ��� render���� �ʴ´�.
	POINT ptCur = CGameProcedure::s_pLocalInput->MouseGetPos();
	m_pUITooltipDlg->DisplayTooltipsDisable();
	RECT rUser = m_pArea_User->GetRegion();

	bool bTooltipRender = false;
	__IconItemSkill* spItem = NULL;

	RECT rcRegion;
	SetRect(&rcRegion, rUser.left, rUser.top, rUser.right, rUser.bottom);			// ���� ����
	char strDummy[32];
	lstrcpy(strDummy, "elmo_ecli666");

	for(UIListReverseItor itor = m_Children.rbegin(); m_Children.rend() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if ( (m_szID != "Base_Iteminfo") && (pChild->GetID() != "area_samma") )
		{
			if ( (GetState() == UI_STATE_ICON_MOVING) && (pChild->UIType() == UI_TYPE_ICON) && (CN3UIWndBase::m_sSelectedIconInfo.pItemSelect) &&
				((CN3UIIcon *)pChild == CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pUIIcon) )	continue;
			pChild->Render();
		}
		if ( pChild->m_szID == strDummy )
			CGameBase::s_pPlayer->InventoryChrRender(rcRegion);
		if ( (pChild->UIType() == UI_TYPE_ICON) && (pChild->GetStyle() & UISTYLE_ICON_HIGHLIGHT)  && (!m_bOpenningNow) && (!m_bClosingNow))
		{
			bTooltipRender = true;
			spItem = GetHighlightIconItem( (CN3UIIcon* )pChild );
		}
	}

	if (m_bDestoyDlgAlive)
		m_pArea_Destroy->Render();

	if ( (GetState() == UI_STATE_ICON_MOVING) && (CN3UIWndBase::m_sSelectedIconInfo.pItemSelect))
		CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pUIIcon->Render();		

	// ���� ǥ�õǾ� �� ������ ���� ǥ��..
	for( int i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if ( m_pMyInvWnd[i] && ((m_pMyInvWnd[i]->pItemBasic->byContable == UIITEM_TYPE_COUNTABLE) || 
					(m_pMyInvWnd[i]->pItemBasic->byContable == UIITEM_TYPE_COUNTABLE_SMALL)) )
		{
			// string ���..
			CN3UIString* pStr = GetChildStringByiOrder(i);
			if(pStr) 
			{
				if ( (GetState() == UI_STATE_ICON_MOVING) && (m_pMyInvWnd[i] == CN3UIWndBase::m_sSelectedIconInfo.pItemSelect) )
				{
					pStr->SetVisible(false);
				}
				else
				{
					if ( m_pMyInvWnd[i]->pUIIcon->IsVisible() )
					{
						pStr->SetVisible(true);
						pStr->SetStringAsInt(m_pMyInvWnd[i]->iCount);
						pStr->Render();
					}
					else
					{
						pStr->SetVisible(false);
					}
				}
			}
		}
		else
		{
			// string ���..
			CN3UIString* pStr = GetChildStringByiOrder(i);
			if(pStr) 
				pStr->SetVisible(false);
		}
	}

	// ��������̸�.. ����;
	if (m_eInvenState == INV_STATE_REPAIR) { CGameProcedure::s_pProcMain->m_pUIRepairTooltip->Render(); return;	}
	
	if ( bTooltipRender && spItem )
		m_pUITooltipDlg->DisplayTooltipsEnable(ptCur.x, ptCur.y, spItem );
}

void CUIInventory::InitIconWnd(e_UIWND eWnd)
{
	m_pArea_User = (CN3UIArea *)GetChildByID("area_char"); __ASSERT(m_pArea_User, "NULL UI Component!!");
	if(NULL == m_pArea_User) return;

	m_pArea_Destroy = (CN3UIArea *)GetChildByID("area_samma"); __ASSERT(m_pArea_Destroy, "NULL UI Component!!");
	if(NULL == m_pArea_Destroy) return;

	m_pText_Weight		= (CN3UIString*)GetChildByID("text_weight");	__ASSERT(m_pText_Weight	, "NULL UI Component!!");
	__TABLE_UI_RESRC* pTblUI = CGameBase::s_pTbl_UI->Find(CGameBase::s_pPlayer->m_InfoBase.eNation);
	__ASSERT(pTblUI, "NULL Pointer UI Table");

	m_pUITooltipDlg = new CUIImageTooltipDlg();
	m_pUITooltipDlg->Init(this);
	m_pUITooltipDlg->LoadFromFile(pTblUI->szItemInfo);
	m_pUITooltipDlg->InitPos();
	m_pUITooltipDlg->SetVisible(FALSE);	

	CN3UIWndBase::InitIconWnd(eWnd);
}

void CUIInventory::UpdateWeight(std::string str)
{
	if(NULL == m_pText_Weight) return;

	m_pText_Weight->SetString(str);
}

void CUIInventory::InitIconUpdate()
{
	CN3UIArea* pArea;
	float fUVAspect = (float)45.0f/(float)64.0f;

	for( int i = 0; i < ITEM_SLOT_COUNT; i++ )
	{
		if ( m_pMySlot[i] != NULL )
		{
			m_pMySlot[i]->pUIIcon = new CN3UIIcon;
			m_pMySlot[i]->pUIIcon->Init(this);
			m_pMySlot[i]->pUIIcon->SetTex(m_pMySlot[i]->szIconFN);
			m_pMySlot[i]->pUIIcon->SetUVRect(0,0, fUVAspect, fUVAspect);
			m_pMySlot[i]->pUIIcon->SetUIType(UI_TYPE_ICON);
			m_pMySlot[i]->pUIIcon->SetStyle(UISTYLE_ICON_ITEM|UISTYLE_ICON_CERTIFICATION_NEED);
			pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, i);
			if ( pArea )
			{
				m_pMySlot[i]->pUIIcon->SetRegion(pArea->GetRegion());
				m_pMySlot[i]->pUIIcon->SetMoveRect(pArea->GetRegion());
			}
			if ( m_pMySlot[i]->iDurability == 0 )
				m_pMySlot[i]->pUIIcon->SetStyle(m_pMySlot[i]->pUIIcon->GetStyle() | UISTYLE_DURABILITY_EXHAUST);	
		}
	}

	for( int i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if ( m_pMyInvWnd[i] != NULL )
		{
			m_pMyInvWnd[i]->pUIIcon = new CN3UIIcon;
			m_pMyInvWnd[i]->pUIIcon->Init(this);
			m_pMyInvWnd[i]->pUIIcon->SetTex(m_pMyInvWnd[i]->szIconFN);
			m_pMyInvWnd[i]->pUIIcon->SetUVRect(0,0, fUVAspect, fUVAspect);
			m_pMyInvWnd[i]->pUIIcon->SetUIType(UI_TYPE_ICON);
			m_pMyInvWnd[i]->pUIIcon->SetStyle(UISTYLE_ICON_ITEM|UISTYLE_ICON_CERTIFICATION_NEED);
			pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, i);
			if ( pArea )
			{
				m_pMyInvWnd[i]->pUIIcon->SetRegion(pArea->GetRegion());
				m_pMyInvWnd[i]->pUIIcon->SetMoveRect(pArea->GetRegion());
			}
			if ( m_pMyInvWnd[i]->iDurability == 0 )
				m_pMyInvWnd[i]->pUIIcon->SetStyle(m_pMyInvWnd[i]->pUIIcon->GetStyle() | UISTYLE_DURABILITY_EXHAUST);	
		}
	}
}

__IconItemSkill* CUIInventory::GetHighlightIconItem(CN3UIIcon* pUIIcon)
{
	for( int i = 0; i < ITEM_SLOT_COUNT; i++ )
	{
		if ( (m_pMySlot[i] != NULL) && (m_pMySlot[i]->pUIIcon == pUIIcon) )
			return m_pMySlot[i];
	}

	for(int i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if ( (m_pMyInvWnd[i] != NULL) && (m_pMyInvWnd[i]->pUIIcon == pUIIcon) )
			return m_pMyInvWnd[i];
	}
	return NULL;
}

e_UIWND_DISTRICT CUIInventory::GetWndDistrict(__IconItemSkill* spItem)
{
	for( int i = 0; i < ITEM_SLOT_COUNT; i++ )
	{
		if ( (m_pMySlot[i] != NULL) && (m_pMySlot[i] == spItem) )
			return UIWND_DISTRICT_INVENTORY_SLOT;
	}

	for(int i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if ( (m_pMyInvWnd[i] != NULL) && (m_pMyInvWnd[i] == spItem) )
			return UIWND_DISTRICT_INVENTORY_INV;
	}
	return UIWND_DISTRICT_UNKNOWN;
}

int CUIInventory::GetItemiOrder(__IconItemSkill* spItem, e_UIWND_DISTRICT eWndDist)
{
	int iReturn = -1;
	int i;

	switch ( eWndDist )
	{
		case UIWND_DISTRICT_INVENTORY_SLOT:
			for( i = 0; i < ITEM_SLOT_COUNT; i++ )
			{
				if ( (m_pMySlot[i] != NULL) && (m_pMySlot[i] == spItem) )
					return i;
			}
			break;

		case UIWND_DISTRICT_INVENTORY_INV:
			for( i = 0; i < MAX_ITEM_INVENTORY; i++ )
			{
				if ( (m_pMyInvWnd[i] != NULL) && (m_pMyInvWnd[i] == spItem) )
					return i;
			}
			break;
	}

	return iReturn;
}

RECT CUIInventory::GetSampleRect()
{
	RECT rect;
	CN3UIArea* pArea;
	POINT ptCur = CGameProcedure::s_pLocalInput->MouseGetPos();
	pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, 0);
	rect = pArea->GetRegion();
	float fWidth = (float)(rect.right - rect.left);
	float fHeight = (float)(rect.bottom - rect.top);
	fWidth *= 0.5f; fHeight *= 0.5f;
	rect.left = ptCur.x - (int)fWidth;  rect.right  = ptCur.x + (int)fWidth;
	rect.top  = ptCur.y - (int)fHeight; rect.bottom = ptCur.y + (int)fHeight;
	return rect;
}

DWORD CUIInventory::MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	DWORD dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;
	if (CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer) { dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);  return dwRet; }

	// ��������̸�.. ����;
	if (m_eInvenState == INV_STATE_REPAIR) { dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);  return dwRet; }

	if (m_bDestoyDlgAlive)	
	{ 
		CN3UIImage* pImg = (CN3UIImage* )m_pArea_Destroy->GetChildByID("img_Destroy");
		__ASSERT(pImg, "NULL UI Component!!");

		if (!pImg) return dwRet;
		if (!pImg->IsIn(ptCur.x, ptCur.y))
		{
			//this_ui_add_start
			//�ı��ϴ� â�� ���������� ��â�� ����� �κ��丮â���� Ŭ���� ���� �ൿ�� �ϸ� ĳ���Ͱ� �̵��� �ؼ� �����ϴ� ��������...
			if(IsIn(ptCur.x, ptCur.y))
				dwRet |= UI_MOUSEPROC_INREGION;
			//this_ui_add_end

			return dwRet;
		}

		for ( int i = 0; i < m_pArea_Destroy->GetChildrenCount(); i++ )
		{
			CN3UIBase* pChild = m_pArea_Destroy->GetChildByIndex(i);
			if (pChild)	
				dwRet |= pChild->MouseProc(dwFlags, ptCur, ptOld);  
		}
		return dwRet;
	}

	// �巡�� �Ǵ� ������ ����..
	if ( (GetState() == UI_STATE_ICON_MOVING) && 
			(CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWnd == UIWND_INVENTORY) &&
			(CN3UIWndBase::m_sSelectedIconInfo.pItemSelect) )
	{
		CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pUIIcon->SetRegion(GetSampleRect());
		CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pUIIcon->SetMoveRect(GetSampleRect());
	}

	return CN3UIWndBase::MouseProc(dwFlags, ptCur, ptOld);
}

void CUIInventory::SendInvMsg(BYTE bDir, int iItemID, int SrcPos, int DestPos)
{
	BYTE byBuff[100];												// ����.. 
	int iOffset=0;												// �ɼ�..

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_ITEM_MOVE);				// Item Move
	CAPISocket::MP_AddByte(byBuff, iOffset, bDir);						
	CAPISocket::MP_AddDword(byBuff, iOffset, iItemID);					
	CAPISocket::MP_AddByte(byBuff, iOffset, (byte)SrcPos);				
	CAPISocket::MP_AddByte(byBuff, iOffset, (byte)DestPos);						

	TRACE("Send Inv Move %d, %d, %d, %d \n", bDir, iItemID, SrcPos, DestPos );

	CGameProcedure::s_pProcMain->s_pSocket->Send(byBuff, iOffset);									// ����..
}

int CUIInventory::GetInvDestinationIndex(__IconItemSkill* spItem)
{
	if( CN3UIWndBase::m_sSelectedIconInfo.pItemSelect == NULL )
		return false;

	for( int i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if (!m_pMyInvWnd[i])
			return i;
	}

	return -1;
}

int	CUIInventory::GetArmDestinationIndex(__IconItemSkill* spItem)
{
	if( CN3UIWndBase::m_sSelectedIconInfo.pItemSelect == NULL )
		return false;

	__TABLE_ITEM_BASIC*		pItem;
	pItem = CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemBasic;

	e_PartPosition ePart;
	e_PlugPosition ePlug;
	e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, NULL, NULL, ePart, ePlug); // �����ۿ� ���� ���� �̸��� ����
	if(ITEM_TYPE_UNKNOWN == eType) return false;

	if ( IsValidRaceAndClass(pItem, CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemExt) )
	{
		switch ( pItem->byAttachPoint )
		{
			case ITEM_ATTACH_POS_DUAL:
				if (m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT] && m_pMySlot[ITEM_SLOT_POS_HAND_LEFT])	// ���ʿ� �ִ� ���..
					return ITEM_SLOT_POS_HAND_RIGHT;				// �Ѵ� ������ ������..
				if (!m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT])	// �����ʿ� ���� ���..
					return ITEM_SLOT_POS_HAND_RIGHT;
				else
				{
					if (m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT]->pItemBasic->byAttachPoint == ITEM_ATTACH_POS_TWOHAND_RIGHT)
						return ITEM_SLOT_POS_HAND_RIGHT;
					else
						return ITEM_SLOT_POS_HAND_LEFT;
				}
				return -1;

			case ITEM_ATTACH_POS_HAND_RIGHT:
				return ITEM_SLOT_POS_HAND_RIGHT;

			case ITEM_ATTACH_POS_HAND_LEFT:
				return ITEM_SLOT_POS_HAND_LEFT;

			case ITEM_ATTACH_POS_TWOHAND_RIGHT:				// ��հ��� �����տ� ����..
				if (m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT] && m_pMySlot[ITEM_SLOT_POS_HAND_LEFT])	// ���ʿ� �ִ� ���..
					return -1;
				else 
					return ITEM_SLOT_POS_HAND_RIGHT;

			case ITEM_ATTACH_POS_TWOHAND_LEFT:				// ��հ��� �����տ� ����..
				if (m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT] && m_pMySlot[ITEM_SLOT_POS_HAND_LEFT])	// ���ʿ� �ִ� ���..
					return -1;
				else 
					return ITEM_SLOT_POS_HAND_LEFT;

			case ITEM_ATTACH_POS_EAR:
				if (!m_pMySlot[ITEM_SLOT_POS_EAR_RIGHT])	// �����ʿ� ���� ���..
					return ITEM_SLOT_POS_EAR_RIGHT;
				if (!m_pMySlot[ITEM_SLOT_POS_EAR_LEFT])		// ���ʿ� ���� ���..
					return ITEM_SLOT_POS_EAR_LEFT;
				return ITEM_SLOT_POS_EAR_RIGHT;				// �Ѵ� ������ ������..

			case ITEM_ATTACH_POS_HEAD:		
				return ITEM_SLOT_POS_HEAD;

			case ITEM_ATTACH_POS_NECK:		
				return ITEM_SLOT_POS_NECK;

			case ITEM_ATTACH_POS_UPPER:
				return ITEM_SLOT_POS_UPPER;

			case ITEM_ATTACH_POS_CLOAK:
				return ITEM_SLOT_POS_SHOULDER;

			case ITEM_ATTACH_POS_BELT:
				return ITEM_SLOT_POS_BELT;

			case ITEM_ATTACH_POS_FINGER:
				if (!m_pMySlot[ITEM_SLOT_POS_RING_RIGHT])	// �����ʿ� ���� ���..
					return ITEM_SLOT_POS_RING_RIGHT;
				if (!m_pMySlot[ITEM_SLOT_POS_RING_LEFT])		// ���ʿ� ���� ���..
					return ITEM_SLOT_POS_RING_LEFT;
				return ITEM_SLOT_POS_RING_RIGHT;				// �Ѵ� ������ ������..

			case ITEM_ATTACH_POS_LOWER:
				return ITEM_SLOT_POS_LOWER;

			case ITEM_ATTACH_POS_ARM:
				return ITEM_SLOT_POS_GLOVES;

			case ITEM_ATTACH_POS_FOOT:
				return ITEM_SLOT_POS_SHOES;
			default:
				return -1;
		}
	}			

	return -1;
}

bool CUIInventory::CheckIconDropIfSuccessSendToServer(__IconItemSkill* spItem)
{
	// ���� �������� �� �� �ִ��� �˻��ϰ�..
	bool  bFound = false;
	bool  bArm = true;
	CN3UIArea* pArea;
	int   i, iDestiOrder;

	if (!m_bRBtnProcessing)
	{
		POINT ptCur = CGameProcedure::s_pLocalInput->MouseGetPos();
		for( i = 0; i < ITEM_SLOT_COUNT; i++ )
		{
			pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, i);
			if ( pArea && pArea->IsIn(ptCur.x, ptCur.y) )
			{
				bFound = true;
				bArm = true;
				iDestiOrder = i;
				break;
			}
		}
		if ( !bFound )
		{
			for( i = 0; i < MAX_ITEM_INVENTORY; i++ )
			{
				pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, i);
				if ( pArea && pArea->IsIn(ptCur.x, ptCur.y) )
				{
					bFound = true;
					bArm = false;
					iDestiOrder = i;
					break;
				}
			}
		}

		if ( !bFound )		// �� ã������ �κ��丮 ĳ���� ���� �˻�..
		{
			if ( m_pArea_User->IsIn(ptCur.x, ptCur.y) )
			{	
				// �κ� ������ �������� �ƴϸ�.. false return..
				if (CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict == UIWND_DISTRICT_INVENTORY_INV)
				{
					iDestiOrder = GetArmDestinationIndex(CN3UIWndBase::m_sSelectedIconInfo.pItemSelect);
					if (iDestiOrder != -1)
					{
						bFound = true;
						bArm = true;
					}
				}
			}
		}
	}
	else
	{
		if (CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict == UIWND_DISTRICT_INVENTORY_INV)
		{
			iDestiOrder = GetArmDestinationIndex(CN3UIWndBase::m_sSelectedIconInfo.pItemSelect);
			if (iDestiOrder != -1)
			{
				bFound = true;
				bArm = true;
			}
		}
		else
		{
			iDestiOrder = GetInvDestinationIndex(CN3UIWndBase::m_sSelectedIconInfo.pItemSelect);
			if (iDestiOrder != -1)
			{
				bFound = true;
				bArm = false;
			}				
		}
	}

	if ( !bFound )	return false;	// �� ã�����Ƿ�.. ����..

	// ���������� Recovery Info�� Ȱ���ϱ� �����Ѵ�.. 
	// ���� WaitFromServer�� On���� �ϰ�.. Select Info�� Recovery Info�� ����..
	CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer				= true;
	CN3UIWndBase::m_sRecoveryJobInfo.pItemSource					= CN3UIWndBase::m_sSelectedIconInfo.pItemSelect;
	CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.UIWnd			= CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWnd;
	CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.UIWndDistrict = CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict;
	CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder		= CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder;
	CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWnd			= UIWND_INVENTORY;
	CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= NULL;
	// �˻��ϴ� ���߿� Recovery Info�߿� pItemTarget�� �ʿ��ϴٸ� �ۼ��ϰ� false�� �����Ҷ��� �������..

	// Arm -> Arm
	if ( (CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict == UIWND_DISTRICT_INVENTORY_SLOT) && bArm )
	{
		// ���� �������� �ִ��� ���캸�� ���� �������� ������..
		if ( !m_pMySlot[iDestiOrder] )
		{
			if ( IsValidPosFromArmToArm(iDestiOrder) )
			{
				// �������� �� �� ������.. ������ ���и� �� ��츦 ����ؼ� ��� ������ �ۼ�.. 
				// �׸��� ������ ������ �� ��� �ؾ��� �۾� ������ �ۼ�..
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_SLOT;				
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder			= iDestiOrder;

				// Send To Server.. 
				SendInvMsg(0x04, CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic->dwID+
					CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemExt->dwID, 
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder, iDestiOrder);
				return true;
			}
			else
			{
				CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer  = false;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemSource		= NULL;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget		= NULL;
				return false;
			}
		}
		// ���� �������� ������..
		else
		{
			// �������� �� �� ������..
			if ( IsValidPosFromArmToArm(iDestiOrder) )
			{
				// ���� ������ ������ pItemTarget�� UIWndTargetStart�� �����ϰ�..
				CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMySlot[iDestiOrder];
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= iDestiOrder;
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;

				// �� �ݴ뵵 �����ϸ�..
				if ( IsValidPosFromArmToArmInverse(CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder) )
				{
					// �������� �� �� ������.. ������ ���и� �� ��츦 ����ؼ� ��� ������ �ۼ�.. 
					// �׸��� ������ ������ �� ��� �ؾ��� �۾� ������ �ۼ�..
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_SLOT;				
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder			= iDestiOrder;

					CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_SLOT;
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;

					// Send To Server.. 
					SendInvMsg(0x04, CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic->dwID+
						CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemExt->dwID, 
						CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder, iDestiOrder);
					return true;
				}
			}
			else
			{
				CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer  = false;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemSource		= NULL;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget		= NULL;
				return false;
			}
		}
	}
	// Arm -> Inv
	else if ( (CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict == UIWND_DISTRICT_INVENTORY_SLOT) && !bArm )
	{
		// ���� �������� �ִ��� ���캸�� ���� �������� ������..
		if ( !m_pMyInvWnd[iDestiOrder] )
		{
			// �������� �翬�� �� �� �ִ�.. ^^
			// �������� �� �� ������.. ������ ���и� �� ��츦 ����ؼ� ��� ������ �ۼ�.. 
			// �׸��� ������ ������ �� ��� �ؾ��� �۾� ������ �ۼ�..
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;				
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder			= iDestiOrder;

			// Send To Server.. 
			SendInvMsg(0x02, CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic->dwID+
				CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemExt->dwID, 
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder, iDestiOrder);
			return true;
		}
		// ���� �������� ������..
		else
		{
			// �κ��丮 �󽽷��� ã�� ����..
			bFound = false;
			for( i = 0; i < MAX_ITEM_INVENTORY; i++ )
			{
				if ( !m_pMyInvWnd[i] )
				{
					bFound = true;
					break;
				}
			}

			if ( bFound )	// �� ������ ã������..
			{
				// �������� �� �� ������.. ������ ���и� �� ��츦 ����ؼ� ��� ������ �ۼ�.. 
				// �׸��� ������ ������ �� ��� �ؾ��� �۾� ������ �ۼ�..
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;				
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder			= i;

				// Send To Server.. 
				SendInvMsg(0x02, CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic->dwID+
					CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemExt->dwID, 
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder, i);
				return true;
			}
			else
			{
				CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer  = false;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemSource		= NULL;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget		= NULL;
				return false;
			}
		}
	}
	// Inv -> Arm
	else if ( (CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict == UIWND_DISTRICT_INVENTORY_INV) && bArm )
	{
		// �˿� �����ϴ� ���� ���� �������� �ִ��� ���캼 �ʿ䰡 ����.. �ֳĸ�, �˻��ϴ� �Լ��� �ϴϱ�..
		// ���� �������� �ִ��� ���캸�� ���� �������� ������..
		if ( !m_pMySlot[iDestiOrder] )
		{
			if ( IsValidPosFromInvToArm(iDestiOrder) )
			{
				// �������� �� �� ������.. ������ ���и� �� ��츦 ����ؼ� ��� ������ �ۼ�.. 
				// �׸��� ������ ������ �� ��� �ؾ��� �۾� ������ �ۼ�..
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_SLOT;				
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder			= iDestiOrder;

				// Send To Server.. 
				SendInvMsg(0x01, CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic->dwID+
					CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemExt->dwID, 
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder, iDestiOrder);
				return true;
			}
			else
			{
				CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer  = false;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemSource		= NULL;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget		= NULL;
				return false;
			}
		}
		// ���� �������� ������..
		else
		{
			if ( IsValidPosFromInvToArm(iDestiOrder) )
			{
				// �������� �� �� ������.. ������ ���и� �� ��츦 ����ؼ� ��� ������ �ۼ�.. 
				// �׸��� ������ ������ �� ��� �ؾ��� �۾� ������ �ۼ�..
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_SLOT;				
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder			= iDestiOrder;

				// �˿� �����ϴ� ��찡 �ƴϸ�..
				if ( (iDestiOrder != ITEM_SLOT_POS_HAND_RIGHT) && (iDestiOrder != ITEM_SLOT_POS_HAND_LEFT) )
				{
					CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMySlot[iDestiOrder];
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= iDestiOrder;
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
				}
				// Send To Server.. 
				SendInvMsg(0x01, CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic->dwID+
					CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemExt->dwID, 
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder, iDestiOrder);
				return true;
			}
			else
			{
				CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer  = false;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemSource		= NULL;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget		= NULL;
				return false;
			}
		}
	}
	// Inv -> Inv
	else if ( (CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict == UIWND_DISTRICT_INVENTORY_INV) && !bArm )
	{
		// ���� �������� �ִ��� ���캸�� ���� �������� ������..
		if ( !m_pMyInvWnd[iDestiOrder] )
		{
			// �������� �� �� ������.. ������ ���и� �� ��츦 ����ؼ� ��� ������ �ۼ�.. 
			// �׸��� ������ ������ �� ��� �ؾ��� �۾� ������ �ۼ�..
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;				
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder			= iDestiOrder;

			// Send To Server.. 
			SendInvMsg(0x03, CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic->dwID+
				CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemExt->dwID, 
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder, iDestiOrder);
			return true;
		}
		// ���� �������� ������.. 
		else
		{
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;				
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder			= iDestiOrder;

			CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMyInvWnd[iDestiOrder];
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_INV;
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= iDestiOrder;
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
			// Send To Server.. 
			SendInvMsg(0x03, CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic->dwID+
				CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemExt->dwID, 
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder, iDestiOrder);
			return true;
		}
	}
	
	CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer  = false;
	CN3UIWndBase::m_sRecoveryJobInfo.pItemSource		= NULL;
	CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget		= NULL;
	return false;
}

inline	bool CUIInventory::InvOpsSomething(__IconItemSkill* spItem)
{
	if (!spItem) return false;
	CN3UIArea* pArea = NULL;

	// �˻��Ѵ�..�����̸� �������� ����..
	if ( CheckIconDropIfSuccessSendToServer(spItem) )												
	{																								
		// ������ �̵�.. Source.. ���� ������ ������ �����̴� �Ÿ�.. ���� �����ϰ� �߰���  �ʿ���� �̵��� �ϸ� �ȴ�..
		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemSource )											
		{																							
			// ����..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.UIWndDistrict )				
			{																						
				case UIWND_DISTRICT_INVENTORY_SLOT:														
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] = NULL;		
					break;																			

				case UIWND_DISTRICT_INVENTORY_INV:													
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] = NULL;	
					break;																			
			}																						
		}																							
		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget )											
		{																							
			// ����..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict )				
			{																							
				case UIWND_DISTRICT_INVENTORY_SLOT:													
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder] = NULL;		
					break;																			

				case UIWND_DISTRICT_INVENTORY_INV:													
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder] = NULL;	
					break;																				
			}																						
		}																							

		// ������ �̵�.. Source.. ���� ������ ������ �����̴� �Ÿ�.. ���� �����ϰ� �߰���  �ʿ���� �̵��� �ϸ� �ȴ�..
		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemSource )											
		{																								
			__IconItemSkill*	spItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemSource;				

			// �߰�..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict )				
			{																						
				case UIWND_DISTRICT_INVENTORY_SLOT:													
					pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder);	
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder] = spItem;												
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());				
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());			
					break;																													

				case UIWND_DISTRICT_INVENTORY_INV:																							
					pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder);	
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder] = spItem;											
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());			
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());			
					break;																													
			}																																
		}																																	

		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget )																					
		{																																	
			__IconItemSkill*	spItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget;														

			// �߰�..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict )														
			{																																
				case UIWND_DISTRICT_INVENTORY_SLOT:																								
					pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder);	
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder] = spItem;												
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());				
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());			
					break;																													

				case UIWND_DISTRICT_INVENTORY_INV:																							
					pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder);		
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder] = spItem;													
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());			
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());			
					break;																													
			}																																
		}																																	

		CN3UIWndBase::AllHighLightIconFree();																								
		SetState(UI_STATE_COMMON_NONE);																										
		return true;																														
	}																																		
	else																																	
	{																																		
		CN3UIWndBase::AllHighLightIconFree();																								
		SetState(UI_STATE_COMMON_NONE);																										
		return false;																														
	}			
}

bool CUIInventory::ReceiveIconDrop(__IconItemSkill* spItem, POINT ptCur)
{
	CN3UIArea* pArea = NULL;

	if (!m_bVisible) return false;
	if (!spItem) return false;

	// ���� ������ �������� �ƴϸ�..
	if ( CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWnd != m_eUIWnd )
		return false;

	// ���� ������ �������̰� �κ��丮 ������ ��, Arm->Arm, Arm->Inv, Inv->Arm, Inv->Inv�̴�..
	// ���õ� �����ܰ� ������.. 
	switch ( CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict )
	{
		case UIWND_DISTRICT_INVENTORY_SLOT:
			pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder);
			break;	

		case UIWND_DISTRICT_INVENTORY_INV:
			pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder);
			break;	
	}

	if ( (pArea) && (pArea->IsIn(ptCur.x, ptCur.y)) )
	{
		if (GetState()  == UI_STATE_ICON_MOVING)
		{
			if (spItem) PlayItemSound(spItem->pItemBasic);
			CN3UIWndBase::AllHighLightIconFree();																								
			SetState(UI_STATE_COMMON_NONE);																										
			return false;
		}
	}
	else if ( m_pArea_Destroy->IsIn(ptCur.x, ptCur.y) )
	{
		m_bDestoyDlgAlive = true;

		// ������ �� ����..
		RECT rect = { 0, 0, 0, 0 };

		switch ( CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict )
		{
			case UIWND_DISTRICT_INVENTORY_SLOT:
				if ( m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder] != NULL )
				{
					m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetRegion(m_pArea_Destroy->GetRegion());
					m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetMoveRect(rect);
				}
				break;

			case UIWND_DISTRICT_INVENTORY_INV:
				if ( m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder] != NULL )
				{
					m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetRegion(m_pArea_Destroy->GetRegion());
					m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetMoveRect(rect);
				}
				break;
		}

		CN3UIWndBase::m_sRecoveryJobInfo.pItemSource					= CN3UIWndBase::m_sSelectedIconInfo.pItemSelect;
		CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.UIWnd			= CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWnd;
		CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.UIWndDistrict = CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict;
		CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder		= CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder;

		CN3UIWndBase::AllHighLightIconFree();																								
		SetState(UI_STATE_COMMON_NONE);	
		return true;
	}
	else
		return InvOpsSomething(spItem);

	return false;
}

void CUIInventory::ReceiveResultFromServer(BYTE bResult)
{
	CN3UIArea* pArea = NULL;

	if (bResult == 0x01)		// ����..
	{
		// �������� �ٲ������ ���� �����͸� �̵�..
		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemSource )
		{
			__IconItemSkill*	spItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemSource;
			e_ItemSlot eSlot = ITEM_SLOT_UNKNOWN;

			// ����..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.UIWndDistrict )
			{
				case UIWND_DISTRICT_INVENTORY_SLOT:
					eSlot = (e_ItemSlot)CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
					if (!spItem) return;
					this->ItemDelete(spItem->pItemBasic, spItem->pItemExt, eSlot);
					break;
			}
		}

		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget )
		{
			__IconItemSkill*	spItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget;
			if (!spItem) return;
			e_ItemSlot eSlot = ITEM_SLOT_UNKNOWN;

			// ����..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict )
			{
				case UIWND_DISTRICT_INVENTORY_SLOT:
					eSlot = (e_ItemSlot)CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder;
					if (!spItem) return;
					this->ItemDelete(spItem->pItemBasic,  spItem->pItemExt, eSlot);
					break;
			}
		}

		// �������� �ٲ������ ���� �����͸� �̵�..
		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemSource )
		{
			__IconItemSkill*	spItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemSource;
			if (!spItem) return;

			// �߰�..
			e_ItemSlot eSlot = ITEM_SLOT_UNKNOWN;
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict )
			{
				case UIWND_DISTRICT_INVENTORY_SLOT:
					eSlot = (e_ItemSlot)CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder;
					if (!spItem) return;
					ItemAdd(spItem->pItemBasic, spItem->pItemExt, eSlot);
					break;
			}
		}

		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget )
		{
			__IconItemSkill*	spItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget;
			if (!spItem) return;
			e_ItemSlot eSlot = ITEM_SLOT_UNKNOWN;

			// �߰�..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict )
			{
				case UIWND_DISTRICT_INVENTORY_SLOT:
					eSlot = (e_ItemSlot)CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder;
					if (!spItem) return;
					ItemAdd(spItem->pItemBasic, spItem->pItemExt, eSlot);
					break;
			}
		}
	}
	else						// ����..
	{
		// �������� �����·�..
		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemSource )
		{
			// ����..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict )
			{
				case UIWND_DISTRICT_INVENTORY_SLOT:
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder] = NULL;
					break;

				case UIWND_DISTRICT_INVENTORY_INV:
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder] = NULL;
					break;
			}
		}

		// ������ �̵�.. Target..
		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget )
		{
			// ����..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict )
			{
				case UIWND_DISTRICT_INVENTORY_SLOT:
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder] = NULL;
					break;

				case UIWND_DISTRICT_INVENTORY_INV:
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder] = NULL;
					break;
			}
		}

		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemSource )
		{
			__IconItemSkill*	spItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemSource;

			// �߰�..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.UIWndDistrict )
			{
				case UIWND_DISTRICT_INVENTORY_SLOT:
					pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder);
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] = spItem;
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
					break;

				case UIWND_DISTRICT_INVENTORY_INV:
					pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder);
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] = spItem;
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
					break;
			}
		}

		// ������ �̵�.. Target..
		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget )
		{
			__IconItemSkill*	spItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget;

			// �߰�..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict )
			{
				case UIWND_DISTRICT_INVENTORY_SLOT:
					pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder);
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder] = spItem;
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
					break;

				case UIWND_DISTRICT_INVENTORY_INV:
					pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder);
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder] = spItem;
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
					break;
			}
		}
	}

	CN3UIWndBase::AllHighLightIconFree();
	SetState(UI_STATE_COMMON_NONE);

	CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer  = false;
	CN3UIWndBase::m_sRecoveryJobInfo.pItemSource		= NULL;
	CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget		= NULL;

	if (CGameProcedure::s_pProcMain->m_pUISkillTreeDlg) CGameProcedure::s_pProcMain->m_pUISkillTreeDlg->UpdateDisableCheck();
	if (CGameProcedure::s_pProcMain->m_pUIHotKeyDlg) CGameProcedure::s_pProcMain->m_pUIHotKeyDlg->UpdateDisableCheck();
}

void CUIInventory::CancelIconDrop(__IconItemSkill* spItem)
{
	CN3UIWndBase::AllHighLightIconFree();
	SetState(UI_STATE_COMMON_NONE);
}

void CUIInventory::AcceptIconDrop(__IconItemSkill* spItem)
{
	CN3UIWndBase::AllHighLightIconFree();
	SetState(UI_STATE_COMMON_NONE);
}

void CUIInventory::IconRestore()
{
	CN3UIArea* pArea;

	switch ( CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict )
	{
		case UIWND_DISTRICT_INVENTORY_SLOT:
			if ( m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder] != NULL )
			{
				pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder);
				if ( pArea )
				{
					m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
					m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
				}
			}
			break;

		case UIWND_DISTRICT_INVENTORY_INV:
			if ( m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder] != NULL )
			{
				pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder);
				if ( pArea )
				{
					m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
					m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
				}
			}
			break;
	}
}

int CUIInventory::GetIndexInArea(POINT pt)
{
	CN3UIArea* pArea;
	RECT	rect;

	for (int i = 0; i < ITEM_SLOT_COUNT; i++)
	{
		pArea = NULL;
		pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, i);
		if (pArea)
		{
			rect = pArea->GetRegion();

			if ( (pt.x >= rect.left) && (pt.x <= rect.right) && (pt.y >= rect.top) && (pt.y <= rect.bottom) )
				return i;
		}
	}

	for (int i = 0; i < MAX_ITEM_INVENTORY; i++)
	{
		pArea = NULL;
		pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, i);
		if (pArea)
		{
			rect = pArea->GetRegion();

			if ( (pt.x >= rect.left) && (pt.x <= rect.right) && (pt.y >= rect.top) && (pt.y <= rect.bottom) )
				return i + ITEM_SLOT_COUNT;
		}
	}

	return -1;
}

bool CUIInventory::ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg)
{
// Temp Define
#define FAIL_CODE {		\
				SetState(UI_STATE_COMMON_NONE);	\
				return false;	\
			}

// Code Begin
	if(NULL == pSender) return false;
	__IconItemSkill* spItem = NULL;
	e_UIWND_DISTRICT eUIWnd;
	int iOrder;
	DWORD dwBitMask = 0x0f1f0000;

	if (dwMsg == UIMSG_BUTTON_CLICK)					
	{
		if(pSender->m_szID == "btn_close")
		{
			// �κ��丮�� �� ������..
			Close();
		}
	}

	if (m_eInvenState == INV_STATE_REPAIR) FAIL_CODE
	POINT ptCur = CGameProcedure::s_pLocalInput->MouseGetPos();

	switch (dwMsg & dwBitMask)
	{
		case UIMSG_ICON_RDOWN_FIRST:
			m_iRBtnDownOffs = GetIndexInArea(ptCur);
			break;

		case UIMSG_ICON_RUP:
			{
				int iRBtn = GetIndexInArea(ptCur);
				if (iRBtn != -1 && m_iRBtnDownOffs != -1 && m_iRBtnDownOffs == iRBtn)
				{
					bool bSlot;
					if ((iRBtn - ITEM_SLOT_COUNT) >= 0)
					{
						bSlot = false;
						iRBtn -= ITEM_SLOT_COUNT;
					}
					else
						bSlot = true;
					
					// Get Item..
					spItem = GetHighlightIconItem((CN3UIIcon* )pSender);

					CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWnd = UIWND_INVENTORY;
					if (bSlot)
						CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
					else
						CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict = UIWND_DISTRICT_INVENTORY_INV;
					
					CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder = iRBtn;
					CN3UIWndBase::m_sSelectedIconInfo.pItemSelect = spItem;
					
					if (spItem) PlayItemSound(spItem->pItemBasic);

					//..
					m_bRBtnProcessing = true;
					InvOpsSomething(spItem);
					m_bRBtnProcessing = false;
				}
			}
			break;

		case UIMSG_AREA_DOWN_FIRST:
			// ���ΰ� �ŷ����̰�.. �� ���̵� "area_gold"�̸�..  
			// SubProcPerTrade�� �Լ��� ȣ��..	( �� �Լ��� edit�ϴ� ���� �ƴϸ�.. ȣ��)
			if ( (CGameProcedure::s_pProcMain->m_pSubProcPerTrade->m_ePerTradeState == PER_TRADE_STATE_NORMAL) &&
				(pSender->m_szID.compare("area_gold") == 0)	)
					CGameProcedure::s_pProcMain->m_pSubProcPerTrade->RequestItemCountEdit();
				break;
			break;

		case UIMSG_ICON_DOWN_FIRST:
			CN3UIWndBase::AllHighLightIconFree();

			// Get Item..
			spItem = GetHighlightIconItem((CN3UIIcon* )pSender);

			// Save Select Info..
			CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWnd = UIWND_INVENTORY;
			eUIWnd = GetWndDistrict(spItem);
			if ( eUIWnd == UIWND_DISTRICT_UNKNOWN )	FAIL_CODE
			CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict = eUIWnd;
			iOrder = GetItemiOrder(spItem, eUIWnd);
			if ( iOrder == -1 )	FAIL_CODE
			CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder = iOrder;
			CN3UIWndBase::m_sSelectedIconInfo.pItemSelect = spItem;
			// Do Ops..
			((CN3UIIcon* )pSender)->SetRegion(GetSampleRect());
			((CN3UIIcon* )pSender)->SetMoveRect(GetSampleRect());
			// Sound..
			if (spItem) PlayItemSound(spItem->pItemBasic);
			break;

		case UIMSG_ICON_UP:
			// ������ �Ŵ��� ��������� ���� �ٴϸ鼭 �˻�..
			if ( !CGameProcedure::s_pUIMgr->BroadcastIconDropMsg(CN3UIWndBase::m_sSelectedIconInfo.pItemSelect) )
				// ������ ��ġ �������..
				IconRestore();
			else
			{
				if (CN3UIWndBase::m_sSelectedIconInfo.pItemSelect) PlayItemSound(CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemBasic);
			}
			SetState(UI_STATE_COMMON_NONE);
			break;

		case UIMSG_ICON_DOWN:
			if ( GetState()  == UI_STATE_ICON_MOVING )
			{
				CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pUIIcon->SetRegion(GetSampleRect());
				CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pUIIcon->SetMoveRect(GetSampleRect());
			}
			break;

		case UIMSG_ICON_DBLCLK:
			SetState(UI_STATE_COMMON_NONE);

			// ������ ��ġ �������..
			IconRestore();				
			break;			
	}

	return true;
}

bool CUIInventory::IsValidRaceAndClass(__TABLE_ITEM_BASIC* pItem, __TABLE_ITEM_EXT* pItemExt)
{
	int iNeedValue;

	char szBuf[256] = "";
	bool	bValid = false;

	if (!pItem) return false;

	// ����..
	switch ( pItem->byNeedRace )
	{
		case 0:
			bValid = true;
			break;

		default:
			if ( pItem->byNeedRace == CGameBase::s_pPlayer->m_InfoBase.eRace )
				bValid = true;
			break;
	}

	__InfoPlayerMySelf*	pInfoExt = NULL;
	pInfoExt = &(CGameBase::s_pPlayer->m_InfoExt);
	if (!pInfoExt)	return false;

	std::string szMsg;
	if ( bValid )
	{
		// ����..		
		if (pItem->byNeedClass != 0)
		{
			switch (pItem->byNeedClass)
			{
				case CLASS_KINDOF_WARRIOR:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_WARRIOR:
						case CLASS_KA_BERSERKER:
						case CLASS_KA_GUARDIAN:
						case CLASS_EL_WARRIOR:
						case CLASS_EL_BLADE:
						case CLASS_EL_PROTECTOR:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_ROGUE:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_ROGUE:
						case CLASS_KA_HUNTER:
						case CLASS_KA_PENETRATOR:
						case CLASS_EL_ROGUE:
						case CLASS_EL_RANGER:
						case CLASS_EL_ASSASIN:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_WIZARD:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_WIZARD:
						case CLASS_KA_SORCERER:
						case CLASS_KA_NECROMANCER:
						case CLASS_EL_WIZARD:
						case CLASS_EL_MAGE:
						case CLASS_EL_ENCHANTER:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_PRIEST:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_PRIEST:
						case CLASS_KA_SHAMAN:
						case CLASS_KA_DARKPRIEST:
						case CLASS_EL_PRIEST:
						case CLASS_EL_CLERIC:
						case CLASS_EL_DRUID:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_ATTACK_WARRIOR:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_BERSERKER:
						case CLASS_EL_BLADE:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_DEFEND_WARRIOR:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_GUARDIAN:
						case CLASS_EL_PROTECTOR:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_ARCHER:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_HUNTER:
						case CLASS_EL_RANGER:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_ASSASSIN:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_PENETRATOR:
						case CLASS_EL_ASSASIN:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_ATTACK_WIZARD:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_SORCERER:
						case CLASS_EL_MAGE:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_PET_WIZARD:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_NECROMANCER:
						case CLASS_EL_ENCHANTER:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_HEAL_PRIEST:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_SHAMAN:
						case CLASS_EL_CLERIC:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_CURSE_PRIEST:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_DARKPRIEST:
						case CLASS_EL_DRUID:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				default:
					if (CGameBase::s_pPlayer->m_InfoBase.eClass != pItem->byClass)
					{
						::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
						CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
						return false;
					}
					break;
			}					
		}

		if ( (pItem->byAttachPoint == ITEM_LIMITED_EXHAUST) && (CGameBase::s_pPlayer->m_InfoBase.iLevel < pItem->cNeedLevel+pItemExt->siNeedLevel) )
		{
			::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_LOW_LEVEL, szMsg);
			CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
			return false;
		}

		if ( pInfoExt->iRank < pItem->byNeedRank+pItemExt->siNeedRank )
		{
			::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_LOW_RANK, szMsg);
			CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
			return false;
		}
		
		if ( pInfoExt->iTitle < pItem->byNeedTitle+pItemExt->siNeedTitle )
		{
			::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_LOW_TITLE, szMsg);
			CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
			return false;
		}

		iNeedValue = pItem->byNeedStrength;
		if (iNeedValue != 0)
			iNeedValue += pItemExt->siNeedStrength;
		if ( pInfoExt->iStrength < iNeedValue )
		{
			::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_LOW_POWER, szMsg);
			CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
			return false;
		}

		iNeedValue = pItem->byNeedStamina;
		if (iNeedValue != 0)
			iNeedValue += pItemExt->siNeedStamina;
		if ( pInfoExt->iStamina < iNeedValue )
		{
			::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_LOW_STR, szMsg);
			CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
			return false;
		}

		iNeedValue = pItem->byNeedDexterity;
		if (iNeedValue != 0)
			iNeedValue += pItemExt->siNeedDexterity;
		if ( pInfoExt->iDexterity < iNeedValue )
		{
			::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_LOW_DEX, szMsg);
			CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff1010);
			return false;
		}

		iNeedValue = pItem->byNeedInteli;
		if (iNeedValue != 0)
			iNeedValue += pItemExt->siNeedInteli;
		if ( pInfoExt->iIntelligence < iNeedValue )
		{
			::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_LOW_INT, szMsg);
			CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
			return false;
		}

		iNeedValue = pItem->byNeedMagicAttack;
		if (iNeedValue != 0)
			iNeedValue += pItemExt->siNeedMagicAttack;
		if ( pInfoExt->iMagicAttak < iNeedValue )
		{
			::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_LOW_CHA, szMsg);
			CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
			return false;
		}

		return true;
	}
	else
	{
		::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_RACE, szMsg);
		CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
	}
	return false;
}

bool CUIInventory::IsValidPosFromInvToArm(int iOrder)
{
	if( CN3UIWndBase::m_sSelectedIconInfo.pItemSelect == NULL )
		return false;

	__TABLE_ITEM_BASIC*		pItem;
	pItem = CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemBasic;

	e_PartPosition ePart;
	e_PlugPosition ePlug;
	e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, NULL, NULL, ePart, ePlug); // �����ۿ� ���� ���� �̸��� ����
	if(ITEM_TYPE_UNKNOWN == eType) return false;

	if ( IsValidRaceAndClass(pItem, CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemExt) )
	{
		switch ( iOrder )
		{
			case ITEM_SLOT_POS_EAR_RIGHT:
			case ITEM_SLOT_POS_EAR_LEFT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_EAR:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_HEAD:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_HEAD:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_NECK:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_NECK:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_UPPER:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_UPPER:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_SHOULDER:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_CLOAK:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_HAND_RIGHT:				// ������..
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_DUAL:
					case ITEM_ATTACH_POS_HAND_RIGHT:
						// �ϼտ� ��� ���Ⱑ �ִ��� �˾ƺ���..
						if ( (m_pMySlot[ITEM_SLOT_POS_HAND_LEFT]) 
							&& (m_pMySlot[ITEM_SLOT_POS_HAND_LEFT]->pItemBasic->byAttachPoint == ITEM_ATTACH_POS_TWOHAND_LEFT) )
						{
							// �����տ� ���Ⱑ �ִ� ��� false ����.. ^^
							if (m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT]) 
								return false;
							// �����տ� ���Ⱑ ���� ���.. �޼� ���Ⱑ �κ��丮��.. ^^ 
							else
							{
								CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMySlot[ITEM_SLOT_POS_HAND_LEFT];
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= ITEM_SLOT_POS_HAND_LEFT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
								return true;
							}
						}
						else	// �޼տ� ��չ��Ⱑ ���� ���.. 
						{
							// �����տ� ���Ⱑ �ִ� ���.. Item Exchange.. ^^
							if (m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT]) 
							{
								CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT];
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= ITEM_SLOT_POS_HAND_RIGHT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
								return true;
							}
							// �����տ� ���Ⱑ ���� ���..  true ����..
							else
								return true;
						}
						break;

					case ITEM_ATTACH_POS_TWOHAND_RIGHT:				// ��հ��� �����տ� ����..
						if ( m_pMySlot[ITEM_SLOT_POS_HAND_LEFT] )	// �޼տ� ���Ⱑ �ְ�..
						{
							// �����տ� ���Ⱑ �ִ� ��� false ����.. ^^
							if (m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT]) 
								return false;
							// �����տ� ���Ⱑ ���� ��� .. �޼��� ���Ⱑ �κ��丮��.. ^^
							else
							{
								CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMySlot[ITEM_SLOT_POS_HAND_LEFT];
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= ITEM_SLOT_POS_HAND_LEFT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
								return true;
							}
						}
						else										// �޼տ� ���Ⱑ ����..	
						{
							// �����տ� ���Ⱑ �ִ� ���.. Item Exchange.. ^^
							if (m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT]) 
							{
								CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT];
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= ITEM_SLOT_POS_HAND_RIGHT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
								return true;
							}
							// �����տ� ���Ⱑ ���� ���..  true ����..
							else
								return true;
						}
						break;
				}
				break;

			case ITEM_SLOT_POS_HAND_LEFT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_DUAL:
					case ITEM_ATTACH_POS_HAND_LEFT:
						// �����տ� ��� ���Ⱑ �ִ��� �˾ƺ���..
						if ( (m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT]) 
							&& (m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT]->pItemBasic->byAttachPoint == ITEM_ATTACH_POS_TWOHAND_RIGHT) )
						{
							// �޼տ� ���Ⱑ �ִ� ��� false ����.. ^^
							if (m_pMySlot[ITEM_SLOT_POS_HAND_LEFT]) 
								return false;
							// �޼տ� ���Ⱑ ���� ���.. ������ ���Ⱑ �κ��丮��.. ^^ 
							else
							{
								CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT];
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= ITEM_SLOT_POS_HAND_RIGHT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
								return true;
							}
						}
						else	// �����տ� ��չ��Ⱑ ���� ���.. 
						{
							// �޼տ� ���Ⱑ �ִ� ���.. Item Exchange.. ^^
							if (m_pMySlot[ITEM_SLOT_POS_HAND_LEFT]) 
							{
								CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMySlot[ITEM_SLOT_POS_HAND_LEFT];
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= ITEM_SLOT_POS_HAND_LEFT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
								return true;
							}
							// �޼տ� ���Ⱑ ���� ���..  true ����..
							else
								return true;
						}
						break;

					case ITEM_ATTACH_POS_TWOHAND_LEFT:				// ��հ��� �޼տ� ����..
						if ( m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT] )	// �����տ� ���Ⱑ �ְ�..
						{
							// �޼տ� ���Ⱑ �ִ� ��� false ����.. ^^
							if (m_pMySlot[ITEM_SLOT_POS_HAND_LEFT]) 
								return false;
							// �޼տ� ���Ⱑ ���� ���.. ������ ���Ⱑ �κ��丮��.. ^^ 
							else
							{
								CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT];
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= ITEM_SLOT_POS_HAND_RIGHT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
								return true;
							}
						}
						else	// �����տ� ��չ��Ⱑ ���� ���.. 
						{
							// �޼տ� ���Ⱑ �ִ� ���.. Item Exchange.. ^^
							if (m_pMySlot[ITEM_SLOT_POS_HAND_LEFT]) 
							{
								CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMySlot[ITEM_SLOT_POS_HAND_LEFT];
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= ITEM_SLOT_POS_HAND_LEFT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
								return true;
							}
							// �޼տ� ���Ⱑ ���� ���..  true ����..
							else
								return true;
						}
						break;
				}
				break;

			case ITEM_SLOT_POS_BELT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_BELT:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_RING_RIGHT:
			case ITEM_SLOT_POS_RING_LEFT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_FINGER:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_LOWER:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_LOWER:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_GLOVES:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_ARM:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_SHOES:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_FOOT:
						return true;
						break;
				}
				break;
		}
	}
	return false;
}

bool CUIInventory::IsValidPosFromArmToArm(int iOrder)
{
	if( CN3UIWndBase::m_sRecoveryJobInfo.pItemSource == NULL )
		return false;

	__TABLE_ITEM_BASIC*		pItem;
	pItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic;

	e_PartPosition ePart;
	e_PlugPosition ePlug;
	e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, NULL, NULL, ePart, ePlug); // �����ۿ� ���� ���� �̸��� ����
	if(ITEM_TYPE_UNKNOWN == eType) return false;

	if ( IsValidRaceAndClass(pItem, CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemExt) )
	{
		switch ( iOrder )
		{
			case ITEM_SLOT_POS_EAR_RIGHT:
			case ITEM_SLOT_POS_EAR_LEFT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_EAR:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_HAND_RIGHT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_DUAL:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_HAND_LEFT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_DUAL:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_RING_RIGHT:
			case ITEM_SLOT_POS_RING_LEFT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_FINGER:
						return true;
						break;
				}
				break;
		}
	}
	return false;
}

bool CUIInventory::IsValidPosFromArmToArmInverse(int iOrder)
{
	if( CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget == NULL )
		return false;

	__TABLE_ITEM_BASIC*		pItem;
	pItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget->pItemBasic;

	e_PartPosition ePart;
	e_PlugPosition ePlug;
	e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, NULL, NULL, ePart, ePlug); // �����ۿ� ���� ���� �̸��� ����
	if(ITEM_TYPE_UNKNOWN == eType) return false;

	if ( IsValidRaceAndClass(pItem, CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemExt) )
	{
		switch (iOrder)
		{
			case ITEM_SLOT_POS_EAR_RIGHT:
			case ITEM_SLOT_POS_EAR_LEFT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_EAR:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_HAND_RIGHT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_DUAL:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_HAND_LEFT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_DUAL:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_RING_RIGHT:
			case ITEM_SLOT_POS_RING_LEFT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_FINGER:
						return true;
						break;
				}
				break;
		}
	}

	return false;
}

void CUIInventory::ItemAdd(__TABLE_ITEM_BASIC* pItem, __TABLE_ITEM_EXT* pItemExt, e_ItemSlot eSlot)
{
	std::string szFN;
	e_PartPosition ePart;
	e_PlugPosition ePlug;
	e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, &szFN, NULL, ePart, ePlug); // �����ۿ� ���� ���� �̸��� ����

	if(ITEM_TYPE_PLUG == eType)
	{
		if(ITEM_SLOT_HAND_LEFT == eSlot) ePlug = PLUG_POS_LEFTHAND;
		else if(ITEM_SLOT_HAND_RIGHT == eSlot) ePlug = PLUG_POS_RIGHTHAND;
		else { __ASSERT(0, "Invalid Item Plug Position"); }

		CGameBase::s_pPlayer->PlugSet(ePlug, szFN, pItem, pItemExt); // �÷��� ����..
		CGameBase::s_pPlayer->DurabilitySet(eSlot, m_pMySlot[eSlot]->iDurability);
	}
	else if(ITEM_TYPE_PART == eType)
	{
		CGameBase::s_pPlayer->PartSet(ePart, szFN, pItem, pItemExt);
		CGameBase::s_pPlayer->DurabilitySet(eSlot, m_pMySlot[eSlot]->iDurability);
	}
}

void CUIInventory::ItemDelete(__TABLE_ITEM_BASIC* pItem, __TABLE_ITEM_EXT* pItemExt, e_ItemSlot eSlot)
{
	__TABLE_PLAYER_LOOKS* pLooks = CGameBase::s_pTbl_UPC_Looks->Find(CGameBase::s_pPlayer->m_InfoBase.eRace);	// User Player Character Skin ����ü ������..
	__ASSERT(pLooks, "NULL Basic Looks!");
	if(NULL == pLooks) return;

	e_PartPosition ePart;
	e_PlugPosition ePlug;
	e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, NULL, NULL, ePart, ePlug); // �����ۿ� ���� ���� �̸��� ����
	
	if(pLooks)
	{
		if(ITEM_TYPE_PLUG == eType)
		{
			if(ITEM_SLOT_HAND_LEFT == eSlot) ePlug = PLUG_POS_LEFTHAND;
			else if(ITEM_SLOT_HAND_RIGHT == eSlot) ePlug = PLUG_POS_RIGHTHAND;

			CGameBase::s_pPlayer->PlugSet(ePlug, "", NULL, NULL); // �÷��� ����..
		}
		else if(ITEM_TYPE_PART == eType)
		{
			if(PART_POS_HAIR_HELMET == ePart) CGameBase::s_pPlayer->InitHair();
			else CGameBase::s_pPlayer->PartSet(ePart, pLooks->szPartFNs[ePart], NULL, NULL);
		}
	}
}

void CUIInventory::DurabilityChange(e_ItemSlot eSlot, int iDurability)
{
	char szBuf[512];
	std::string szDur; 

	if ( eSlot < ITEM_SLOT_EAR_RIGHT || eSlot >= ITEM_SLOT_COUNT )
	{
		__ASSERT(0, "Durability Change Item Index Weird.");
		CLogWriter::Write("Durability Change Item Index Weird. Slot(%d) Durability(%d)", eSlot, iDurability);
		return;
	}

	if ( m_pMySlot[eSlot] )
	{
		m_pMySlot[eSlot]->iDurability = iDurability;
		if ( iDurability == 0 )
		{
			if(m_pMySlot[eSlot]->pUIIcon && m_pMySlot[eSlot]->pItemBasic) 
			{
				m_pMySlot[eSlot]->pUIIcon->SetStyle(m_pMySlot[eSlot]->pUIIcon->GetStyle() | UISTYLE_DURABILITY_EXHAUST);
				// �޽��� �ڽ� ���..
				::_LoadStringFromResource(IDS_DURABILITY_EXOAST, szDur);
				wsprintf(szBuf, szDur.c_str(), m_pMySlot[eSlot]->pItemBasic->szName.c_str() );
				CGameProcedure::s_pProcMain->MsgOutput(szBuf, 0xffff3b3b);
			}
			else
			{
				__ASSERT(0, "Durability Change Item NULL icon or NULL item.");
				CLogWriter::Write("Durability Change Item NULL icon or NULL item. Slot(%d) Durability(%d)", eSlot, iDurability);
			}
		}
	}
	else
	{
		__ASSERT(0, "Durability Change Item NULL Slot.");
		CLogWriter::Write("Durability Change Item ... NULL Slot. Slot(%d) Durability(%d)", eSlot, iDurability);
	}
}

void CUIInventory::ReceiveResultFromServer(int iResult, int iUserGold)
{
	m_cItemRepairMgr.ReceiveResultFromServer(iResult, iUserGold);
}

int CUIInventory::GetCountInInvByID(int iID)
{
	int i;
	for( i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if ( (m_pMyInvWnd[i] != NULL) && (m_pMyInvWnd[i]->pItemBasic->dwID == (iID/1000*1000)) &&
				(m_pMyInvWnd[i]->pItemExt->dwID == (iID%1000)) )
		return m_pMyInvWnd[i]->iCount;
	}

	return 0;
}

void CUIInventory::ItemCountChange(int iDistrict, int iIndex, int iCount, int iID)
{
	__IconItemSkill* spItem;
	switch (iDistrict)
	{
		case 0x00:
			if (m_pMySlot[iIndex] && ( (m_pMySlot[iIndex]->pItemBasic->dwID+m_pMySlot[iIndex]->pItemExt->dwID) != iID ) )	// ������ �������� �ִ°��..
			{
				// ������ ����.. ���� �κ��丮 �����츸.. 
				spItem = m_pMySlot[iIndex];

				// �κ��丮������ �����..
				m_pMySlot[iIndex] = NULL;

				// iOrder�� �� �Ŵ����� �������� ����Ʈ���� �����Ѵ�..
				RemoveChild(spItem->pUIIcon);

				// ������ ���ҽ� ����...
				spItem->pUIIcon->Release();
				delete spItem->pUIIcon;
				spItem->pUIIcon = NULL;
				delete spItem;
				spItem = NULL;

				// �������� ����� �ִ´�..
				__TABLE_ITEM_BASIC* pItem = NULL;								// ������ ���̺� ����ü ������..	
				__TABLE_ITEM_EXT* pItemExt = NULL;								// ������ ���̺� ����ü ������..	

				pItem = CGameProcedure::s_pTbl_Items_Basic->Find(iID/1000*1000);	// �� ������ ���..
				if(pItem && pItem->byExtIndex >= 0 && pItem->byExtIndex < MAX_ITEM_EXTENSION)
					pItemExt = CGameProcedure::s_pTbl_Items_Exts[pItem->byExtIndex]->Find(iID%1000);	// �� ������ ���..
				if ( NULL == pItem || NULL == pItemExt )
				{
					__ASSERT(0, "NULL Item");
					CLogWriter::Write("MyInfo - Inv - Unknown Item %d, IDNumber", iID);
					return;
				}

				e_PartPosition ePart;
				e_PlugPosition ePlug;
				std::string szIconFN;
				e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, NULL, &szIconFN, ePart, ePlug); // �����ۿ� ���� ���� �̸��� ����
				if(ITEM_TYPE_UNKNOWN == eType) CLogWriter::Write("MyInfo - slot - Unknown Item");
				__ASSERT(ITEM_TYPE_UNKNOWN != eType, "Unknown Item");
				
				spItem = new __IconItemSkill;
				spItem->pItemBasic	= pItem;
				spItem->pItemExt	= pItemExt;
				spItem->szIconFN = szIconFN; // ������ ���� �̸� ����..
				spItem->iCount	= iCount;
				spItem->iDurability = pItem->siMaxDurability + pItemExt->siMaxDurability;
				m_pMySlot[iIndex] = spItem;
				return;
			}
			else if (m_pMySlot[iIndex])
			{
				m_pMySlot[iIndex]->iCount = iCount;
				if (iCount == 0)
				{
					// ������ ����.. ���� �κ��丮 �����츸.. 
					__IconItemSkill* spItem;
					spItem = m_pMySlot[iIndex];

					// �κ��丮������ �����..
					m_pMySlot[iIndex] = NULL;

					// iOrder�� �� �Ŵ����� �������� ����Ʈ���� �����Ѵ�..
					RemoveChild(spItem->pUIIcon);

					// ������ ���ҽ� ����...
					spItem->pUIIcon->Release();
					delete spItem->pUIIcon;
					spItem->pUIIcon = NULL;
					delete spItem;
					spItem = NULL;
				}
			}
			else	// �������� ���� ���..
			{
				// �������� ����� �ִ´�..
				__TABLE_ITEM_BASIC* pItem = NULL;								// ������ ���̺� ����ü ������..	
				__TABLE_ITEM_EXT* pItemExt = NULL;								// ������ ���̺� ����ü ������..	

				pItem = CGameProcedure::s_pTbl_Items_Basic->Find(iID/1000*1000);	// �� ������ ���..
				if(pItem && pItem->byExtIndex >= 0 && pItem->byExtIndex < MAX_ITEM_EXTENSION)
					pItemExt = CGameProcedure::s_pTbl_Items_Exts[pItem->byExtIndex]->Find(iID%1000);	// �� ������ ���..
				if ( NULL == pItem || NULL == pItemExt )
				{
					__ASSERT(0, "NULL Item");
					CLogWriter::Write("MyInfo - Inv - Unknown Item %d, IDNumber", iID);
					return;
				}

				e_PartPosition ePart;
				e_PlugPosition ePlug;
				std::string szIconFN;
				e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, NULL, &szIconFN, ePart, ePlug); // �����ۿ� ���� ���� �̸��� ����
				if(ITEM_TYPE_UNKNOWN == eType) CLogWriter::Write("MyInfo - slot - Unknown Item");
				__ASSERT(ITEM_TYPE_UNKNOWN != eType, "Unknown Item");
				
				spItem = new __IconItemSkill;
				spItem->pItemBasic	= pItem;
				spItem->pItemExt	= pItemExt;
				spItem->szIconFN = szIconFN; // ������ ���� �̸� ����..
				spItem->iCount	= iCount;
				spItem->iDurability = pItem->siMaxDurability + pItemExt->siMaxDurability;
				m_pMySlot[iIndex] = spItem;
				return;
			}
			break;

		case 0x01:
			if (m_pMyInvWnd[iIndex] && ( (m_pMyInvWnd[iIndex]->pItemBasic->dwID+m_pMyInvWnd[iIndex]->pItemExt->dwID) != iID ) )	// ������ �������� �ִ°��..
			{
				// ������ ����.. ���� �κ��丮 �����츸.. 
				spItem = m_pMyInvWnd[iIndex];

				// �κ��丮������ �����..
				m_pMyInvWnd[iIndex] = NULL;

				// iOrder�� �� �Ŵ����� �������� ����Ʈ���� �����Ѵ�..
				RemoveChild(spItem->pUIIcon);

				// ������ ���ҽ� ����...
				spItem->pUIIcon->Release();
				delete spItem->pUIIcon;
				spItem->pUIIcon = NULL;
				delete spItem;
				spItem = NULL;

				// �������� ����� �ִ´�..
				__TABLE_ITEM_BASIC* pItem = NULL;								// ������ ���̺� ����ü ������..	
				__TABLE_ITEM_EXT* pItemExt = NULL;								// ������ ���̺� ����ü ������..	

				pItem = CGameProcedure::s_pTbl_Items_Basic->Find(iID/1000*1000);	// �� ������ ���..
				if(pItem && pItem->byExtIndex >= 0 && pItem->byExtIndex < MAX_ITEM_EXTENSION)
					pItemExt = CGameProcedure::s_pTbl_Items_Exts[pItem->byExtIndex]->Find(iID%1000);	// �� ������ ���..
				if ( NULL == pItem || NULL == pItemExt )
				{
					__ASSERT(0, "NULL Item");
					CLogWriter::Write("MyInfo - Inv - Unknown Item %d, IDNumber", iID);
					return;
				}

				e_PartPosition ePart;
				e_PlugPosition ePlug;
				std::string szIconFN;
				e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, NULL, &szIconFN, ePart, ePlug); // �����ۿ� ���� ���� �̸��� ����
				if(ITEM_TYPE_UNKNOWN == eType) CLogWriter::Write("MyInfo - slot - Unknown Item");
				__ASSERT(ITEM_TYPE_UNKNOWN != eType, "Unknown Item");
				
				spItem = new __IconItemSkill;
				spItem->pItemBasic	= pItem;
				spItem->pItemExt	= pItemExt;
				spItem->szIconFN = szIconFN; // ������ ���� �̸� ����..
				spItem->iCount	= iCount;
				spItem->iDurability = pItem->siMaxDurability + pItemExt->siMaxDurability;
				m_pMyInvWnd[iIndex] = spItem;
				return;
			}
			else if (m_pMyInvWnd[iIndex])
			{
				m_pMyInvWnd[iIndex]->iCount = iCount;
				if (iCount == 0)
				{
					// ������ ����.. ���� �κ��丮 �����츸.. 
					__IconItemSkill* spItem;
					spItem = m_pMyInvWnd[iIndex];

					// �κ��丮������ �����..
					m_pMyInvWnd[iIndex] = NULL;

					// iOrder�� �� �Ŵ����� �������� ����Ʈ���� �����Ѵ�..
					RemoveChild(spItem->pUIIcon);

					// ������ ���ҽ� ����...
					spItem->pUIIcon->Release();
					delete spItem->pUIIcon;
					spItem->pUIIcon = NULL;
					delete spItem;
					spItem = NULL;
				}
			}
			else	// �������� ���� ���..
			{
				// �������� ����� �ִ´�..
				__TABLE_ITEM_BASIC* pItem = NULL;								// ������ ���̺� ����ü ������..	
				__TABLE_ITEM_EXT* pItemExt = NULL;								// ������ ���̺� ����ü ������..	

				pItem = CGameProcedure::s_pTbl_Items_Basic->Find(iID/1000*1000);	// �� ������ ���..
				if(pItem && pItem->byExtIndex >= 0 && pItem->byExtIndex < MAX_ITEM_EXTENSION)
					pItemExt = CGameProcedure::s_pTbl_Items_Exts[pItem->byExtIndex]->Find(iID%1000);	// �� ������ ���..
				if ( NULL == pItem || NULL == pItemExt )
				{
					__ASSERT(0, "NULL Item");
					CLogWriter::Write("MyInfo - Inv - Unknown Item %d, IDNumber", iID);
					return;
				}

				e_PartPosition ePart;
				e_PlugPosition ePlug;
				std::string szIconFN;
				e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, NULL, &szIconFN, ePart, ePlug); // �����ۿ� ���� ���� �̸��� ����
				if(ITEM_TYPE_UNKNOWN == eType) CLogWriter::Write("MyInfo - slot - Unknown Item");
				__ASSERT(ITEM_TYPE_UNKNOWN != eType, "Unknown Item");
				
				spItem = new __IconItemSkill;
				spItem->pItemBasic	= pItem;
				spItem->pItemExt	= pItemExt;
				spItem->szIconFN = szIconFN; // ������ ���� �̸� ����..
				spItem->iCount	= iCount;
				spItem->iDurability = pItem->siMaxDurability + pItemExt->siMaxDurability;
				m_pMyInvWnd[iIndex] = spItem;

				m_pMyInvWnd[iIndex]->pUIIcon = new CN3UIIcon;
				m_pMyInvWnd[iIndex]->pUIIcon->Init(this);
				m_pMyInvWnd[iIndex]->pUIIcon->SetTex(m_pMyInvWnd[iIndex]->szIconFN);
				float fUVAspect = (float)45.0f/(float)64.0f;
				m_pMyInvWnd[iIndex]->pUIIcon->SetUVRect(0,0, fUVAspect, fUVAspect);
				m_pMyInvWnd[iIndex]->pUIIcon->SetUIType(UI_TYPE_ICON);
				m_pMyInvWnd[iIndex]->pUIIcon->SetStyle(UISTYLE_ICON_ITEM|UISTYLE_ICON_CERTIFICATION_NEED);
				CN3UIArea* pArea = NULL;
				pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, iIndex);
				if ( pArea )
				{
					m_pMyInvWnd[iIndex]->pUIIcon->SetRegion(pArea->GetRegion());
					m_pMyInvWnd[iIndex]->pUIIcon->SetMoveRect(pArea->GetRegion());
				}
				return;
			}
			break;
	}
}

void CUIInventory::ItemDestroyOK()
{
	m_bDestoyDlgAlive = false;

	BYTE byBuff[32];															// ��Ŷ ����..
	int iOffset=0;																// ��Ŷ ������..

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_ITEM_DESTROY);					// ���� ��ŸƮ ��Ŷ Ŀ���..

	switch ( CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict )
	{
		case UIWND_DISTRICT_INVENTORY_SLOT:
			CAPISocket::MP_AddByte(byBuff, iOffset, 0x01);						// ���̵� ���� ��Ŷ�� �ֱ�..
			break;
		case UIWND_DISTRICT_INVENTORY_INV:
			CAPISocket::MP_AddByte(byBuff, iOffset, 0x02);						// ���̵� ���� ��Ŷ�� �ֱ�..
			break;
	}
	CAPISocket::MP_AddByte(byBuff, iOffset, CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder);	// ���̵� ���� ��Ŷ�� �ֱ�..
	CAPISocket::MP_AddDword(byBuff, iOffset, CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemBasic->dwID+
		CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemExt->dwID);	// ���̵� ���ڿ� ��Ŷ�� �ֱ�..

	CGameProcedure::s_pSocket->Send(byBuff, iOffset);	

	CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer	= true;
}

void CUIInventory::ItemDestroyCancel()
{
	m_bDestoyDlgAlive = false;
	CN3UIArea* pArea = NULL;

	switch ( CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict )
	{
		case UIWND_DISTRICT_INVENTORY_SLOT:
			if ( m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder] != NULL )
			{
				pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder);
				if ( pArea )
				{
					m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
					m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
				}
			}
			break;

		case UIWND_DISTRICT_INVENTORY_INV:
			if ( m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder] != NULL )
			{
				pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder);
				if ( pArea )
				{
					m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
					m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
				}
			}
			break;
	}
}

void CUIInventory::ReceiveResultItemRemoveFromServer(int iResult)
{
	CN3UIArea* pArea = NULL;
	__IconItemSkill* spItem = NULL;
	CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer	= false;

	switch (iResult)
	{
		case 0x01:			// ����..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.UIWndDistrict )
			{
				case UIWND_DISTRICT_INVENTORY_SLOT:
					spItem = m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder];
					// �� ���������� �����..
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] = NULL;
					this->ItemDelete(spItem->pItemBasic, spItem->pItemExt, (e_ItemSlot)CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder);
					break;

				case UIWND_DISTRICT_INVENTORY_INV:
					spItem = m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder];
					// �� ���������� �����..
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] = NULL;
					break;
			}

			// iOrder�� �� �Ŵ����� �������� ����Ʈ���� �����Ѵ�..
			if (!spItem) return;
			RemoveChild(spItem->pUIIcon);

			// ������ ���ҽ� ����...
			spItem->pUIIcon->Release();
			delete spItem->pUIIcon;
			spItem->pUIIcon = NULL;
			delete spItem;
			spItem = NULL;
			break;

		case 0x00:			// ����..
			switch ( CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict )
			{
				case UIWND_DISTRICT_INVENTORY_SLOT:
					if ( m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder] != NULL )
					{
						pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder);
						if ( pArea )
						{
							m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
							m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
						}
					}
					break;

				case UIWND_DISTRICT_INVENTORY_INV:
					if ( m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder] != NULL )
					{
						pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder);
						if ( pArea )
						{
							m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
							m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
						}
					}
					break;
			}
			break;
	}

	if (CGameProcedure::s_pProcMain->m_pUISkillTreeDlg) CGameProcedure::s_pProcMain->m_pUISkillTreeDlg->UpdateDisableCheck();
	if (CGameProcedure::s_pProcMain->m_pUIHotKeyDlg) CGameProcedure::s_pProcMain->m_pUIHotKeyDlg->UpdateDisableCheck();
}

bool CUIInventory::CheckWeightValidate(__IconItemSkill* spItem)
{
	std::string szMsg;

	if (!spItem)	return false;

	__InfoPlayerMySelf*	pInfoExt = &(CGameBase::s_pPlayer->m_InfoExt);
	if ( (pInfoExt->iWeight + spItem->pItemBasic->siWeight) > pInfoExt->iWeightMax)
	{	 
		::_LoadStringFromResource(IDS_ITEM_WEIGHT_OVERFLOW, szMsg);	
		CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);	
		return false;	
	}

	return true;
}

//this_ui_add_start
bool CUIInventory::OnKeyPress(int iKey)
{
	if(m_bDestoyDlgAlive && m_pArea_Destroy)
	{
		switch(iKey)
		{
		case DIK_RETURN:
			{
				CN3UIButton* pBtnDestroyOk = (CN3UIButton* )m_pArea_Destroy->GetChildByID("btn_Destroy_ok");
				if(pBtnDestroyOk) m_pArea_Destroy->ReceiveMessage(pBtnDestroyOk, UIMSG_BUTTON_CLICK);
				else return false;
			}
			return true;
		case DIK_ESCAPE:
			{
				CN3UIButton* pBtnDestroyCancel = (CN3UIButton* )m_pArea_Destroy->GetChildByID("btn_Destroy_cancel");
				if(pBtnDestroyCancel) m_pArea_Destroy->ReceiveMessage(pBtnDestroyCancel, UIMSG_BUTTON_CLICK);
				else return false;
			}
			return true;
		}
	}
	else
	{
		switch(iKey)
		{
		case DIK_ESCAPE:
			if(!m_bClosingNow) this->Close();
			if(m_pUITooltipDlg) m_pUITooltipDlg->DisplayTooltipsDisable();
			return true;
		}
	}

	return CN3UIBase::OnKeyPress(iKey);
}

void CUIInventory::SetVisible(bool bVisible)
{
	CN3UIBase::SetVisible(bVisible);
	if(bVisible)
		CGameProcedure::s_pUIMgr->SetVisibleFocusedUI(this);
	else
		CGameProcedure::s_pUIMgr->ReFocusUI();

	m_iRBtnDownOffs = -1;
}

void CUIInventory::SetVisibleWithNoSound(bool bVisible, bool bWork, bool bReFocus)
{
	CN3UIBase::SetVisibleWithNoSound(bVisible, bWork, bReFocus);

	if(bWork)
	{
		if ( m_eInvenState == INV_STATE_REPAIR )
		{
			CGameProcedure::RestoreGameCursor();

			if (CGameProcedure::s_pProcMain->m_pUIRepairTooltip->IsVisible())
			{
				CGameProcedure::s_pProcMain->m_pUIRepairTooltip->m_bBRender = false;
				CGameProcedure::s_pProcMain->m_pUIRepairTooltip->DisplayTooltipsDisable();
			}
		}

		m_eInvenState = INV_STATE_NORMAL;

		if (GetState() == UI_STATE_ICON_MOVING)
			IconRestore();
		SetState(UI_STATE_COMMON_NONE);
		CN3UIWndBase::AllHighLightIconFree();

		if(m_pUITooltipDlg) m_pUITooltipDlg->DisplayTooltipsDisable();
	}

	if(bReFocus)
	{
		if(bVisible)
			CGameProcedure::s_pUIMgr->SetVisibleFocusedUI(this);
		else
			CGameProcedure::s_pUIMgr->ReFocusUI();
	}

	m_iRBtnDownOffs = -1;
}

int CUIInventory::GetIndexItemCount(DWORD dwIndex)
{
	int iCnt = 0;

	for( int i = 0; i < ITEM_SLOT_COUNT; i++ )
	{
		if(m_pMySlot[i] && m_pMySlot[i]->pItemBasic)
		{
			if(m_pMySlot[i]->pItemBasic->dwID == dwIndex)
				iCnt += m_pMySlot[i]->iCount;
		}
	}

	for( int i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if(m_pMyInvWnd[i] && m_pMyInvWnd[i]->pItemBasic)
		{
			if(m_pMyInvWnd[i]->pItemBasic->dwID == dwIndex)
				iCnt += m_pMyInvWnd[i]->iCount;
		}
	}

	return iCnt;
}
//this_ui_add_end
