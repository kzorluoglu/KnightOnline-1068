// N3UIButton.cpp: implementation of the CN3UIButton class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfxBase.h"
#include "N3UIButton.h"
#include "N3UIImage.h"

#ifndef _REPENT
#ifdef _N3GAME
#include "..\Warfare\N3UIWndBase.h"
#endif
#endif

#include "N3SndMgr.h"
#include "N3SndObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CN3UIButton::CN3UIButton()
{
	m_eType = UI_TYPE_BUTTON;

	m_dwStyle = UISTYLE_BTN_NORMAL;
	m_eState = UI_STATE_BUTTON_NORMAL;
	ZeroMemory(m_ImageRef, sizeof(CN3UIImage*)*NUM_BTN_STATE);
	ZeroMemory(&m_rcClick, sizeof(m_rcClick));
	m_pSnd_On = NULL;
	m_pSnd_Click = NULL;
}

CN3UIButton::~CN3UIButton()
{
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_On);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Click);
}

void CN3UIButton::Release()
{
	CN3UIBase::Release();

	m_dwStyle = UISTYLE_BTN_NORMAL;
	m_eState = UI_STATE_BUTTON_NORMAL;
	ZeroMemory(m_ImageRef, sizeof(CN3UIImage*)*NUM_BTN_STATE);
	ZeroMemory(&m_rcClick, sizeof(m_rcClick));

	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_On);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Click);
}

void CN3UIButton::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	SetClickRect(Rect);
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		(*itor)->SetRegion(Rect);
	}
}

BOOL CN3UIButton::MoveOffset(int iOffsetX, int iOffsetY)
{
	if (FALSE == CN3UIBase::MoveOffset(iOffsetX, iOffsetY)) return FALSE;
	// click ����
	m_rcClick.left += iOffsetX;		m_rcClick.top += iOffsetY;
	m_rcClick.right += iOffsetX;	m_rcClick.bottom += iOffsetY;	
	return TRUE;
}

void CN3UIButton::Render()
{
 	if(!m_bVisible) return;

	switch(m_eState)
	{
	case UI_STATE_BUTTON_NORMAL:
		{
			if (m_ImageRef[BS_NORMAL]) m_ImageRef[BS_NORMAL]->Render();
		}
		break;
	case UI_STATE_BUTTON_DOWN:
	case UI_STATE_BUTTON_DOWN_2CHECKDOWN:
	case UI_STATE_BUTTON_DOWN_2CHECKUP:
		{
			if (m_ImageRef[BS_DOWN]) m_ImageRef[BS_DOWN]->Render();
		}
		break;
	case UI_STATE_BUTTON_ON:
		{
			if (m_ImageRef[BS_ON]) m_ImageRef[BS_ON]->Render();
		}
		break;
	case UI_STATE_BUTTON_DISABLE:
		{
			if (m_ImageRef[BS_DISABLE]) m_ImageRef[BS_DISABLE]->Render();
		}
	}

	int i = 0;
	for(UIListReverseItor itor = m_Children.rbegin(); m_Children.rend() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		for(i = 0; i < NUM_BTN_STATE; i++) // ��ư�� ���� ��Ұ� �ƴ��� ����..
			if(pChild == m_ImageRef[i]) break;
		if(i >= NUM_BTN_STATE) pChild->Render(); // ��ư ���ϵ尡 �ƴϸ� ������..
	}
}

DWORD CN3UIButton::MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	DWORD dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;

#ifndef _REPENT
#ifdef _N3GAME
	if ( CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer ) return dwRet;
#endif
#endif

	if(false == IsIn(ptCur.x, ptCur.y))	// ���� ���̸�
	{
		if (false == IsIn(ptOld.x, ptOld.y))	return dwRet; // ���� pointer�� ���� ���̾����� �׳� ����
		dwRet |= UI_MOUSEPROC_PREVINREGION;	// ���� ���콺 ��ǥ�� ���� ���̾���.

		if (UI_STATE_BUTTON_DISABLE == m_eState) return dwRet;	// disable�̸� �׳� ����

		if(UISTYLE_BTN_NORMAL & m_dwStyle) // normal ��ư �̸�
		{
			SetState(UI_STATE_BUTTON_NORMAL);	// normal ���·�
		}
		else if (UISTYLE_BTN_CHECK & m_dwStyle) // check ��ư �̸�
		{
			if (UI_STATE_BUTTON_DOWN_2CHECKUP == m_eState)	// up��Ű���� �� ��� 
				SetState(UI_STATE_BUTTON_DOWN);	// down ���·�
			else if (UI_STATE_BUTTON_DOWN_2CHECKDOWN == m_eState ||	// down��Ű���� �� ��� �Ǵ�
				UI_STATE_BUTTON_ON == m_eState)	// on ������ ���
				SetState(UI_STATE_BUTTON_NORMAL);	// normal ���·�
		}
		return dwRet; // ���� ���̹Ƿ� ���̻� ó�� ���� �ʴ´�.
	}
	dwRet |= UI_MOUSEPROC_INREGION;	// �̹� ���콺 ��ǥ�� ���� ���̴�

	if (UI_STATE_BUTTON_DISABLE == m_eState) return dwRet;	// disable�̸� �׳� ����

	// Ŭ�� ���� ���̸�
	if (FALSE == PtInRect(&m_rcClick, ptCur))
	{
		if(UISTYLE_BTN_NORMAL & m_dwStyle) // normal ��ư �̸�
		{
			SetState(UI_STATE_BUTTON_NORMAL);	// normal ���·�
		}
		else if (UISTYLE_BTN_CHECK & m_dwStyle) // check ��ư �̸�
		{
			if (UI_STATE_BUTTON_DOWN_2CHECKUP == m_eState)	// up��Ű���� �� ��� 
				SetState(UI_STATE_BUTTON_DOWN);	// down ���·�
			else if (UI_STATE_BUTTON_DOWN_2CHECKDOWN == m_eState ||	// down��Ű���� �� ��� �Ǵ�
				UI_STATE_BUTTON_ON == m_eState)	// on ������ ���
				SetState(UI_STATE_BUTTON_NORMAL);	// normal ���·�
		}
		return dwRet;
	}

	// �Ʒ��� Ŭ�� ���� ���϶�..
	// normal ��ư �̸�
	if(UISTYLE_BTN_NORMAL & m_dwStyle)
	{
		if(dwFlags & UI_MOUSE_LBCLICK)  // ���ʹ�ư ������ ����
		{
			SetState(UI_STATE_BUTTON_DOWN); // ���� ���·� �����..
			if (m_pSnd_Click) m_pSnd_Click->Play();	// ���尡 ������ play �ϱ�
			dwRet |= UI_MOUSEPROC_DONESOMETHING;
			return dwRet;
		}
		else if(dwFlags & UI_MOUSE_LBCLICKED) // ���ʹ�ư�� ���� ����
		{
			if(m_pParent && UI_STATE_BUTTON_DOWN == m_eState) // ���� ���°� ��ư�� Down �����̸�
			{
				SetState(UI_STATE_BUTTON_ON); // ��ư�� On ���·� �����..
				m_pParent->ReceiveMessage(this, UIMSG_BUTTON_CLICK); // �θ𿡰� ��ư Ŭ�� ����..
			}
			dwRet |= UI_MOUSEPROC_DONESOMETHING;
			return dwRet;
		}
		else if (UI_STATE_BUTTON_NORMAL == m_eState)	// normal�����̸� on���·�..
		{
			SetState(UI_STATE_BUTTON_ON); // On ���·� �����..
			if (m_pSnd_On) m_pSnd_On->Play();	// ���尡 ������ play �ϱ�
			dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
			return dwRet;
			// UI_MOUSEPROC_DONESOMETHING�� ������ �ȵȴ�.(���콺 �����Ͱ� ��ư���� �ٸ� ��ư���� ������ �Űܰ��� 
			// ���� ��ư�� ���°� �̻������� ���� �����ϱ� ����)
		}
	}

	// üũ ��ư�̸�
	else if(UISTYLE_BTN_CHECK & m_dwStyle) 
	{
		if(dwFlags & UI_MOUSE_LBCLICK)  // ���ʹ�ư ������ ����
		{
			if (UI_STATE_BUTTON_NORMAL == m_eState || UI_STATE_BUTTON_ON == m_eState)
			{
				SetState(UI_STATE_BUTTON_DOWN_2CHECKDOWN); // �ӽ÷� ���� ����(DOWN_2CHECKDOWN)�� �����..
				if (m_pSnd_Click) m_pSnd_Click->Play();	// ���尡 ������ play �ϱ�
				dwRet |= UI_MOUSEPROC_DONESOMETHING;
				return dwRet;
			}
			else if(UI_STATE_BUTTON_DOWN == m_eState)
			{
				SetState(UI_STATE_BUTTON_DOWN_2CHECKUP); // �ӽ÷� ���� ����(DOWN_2CHECKUP)�� �����..
				if (m_pSnd_Click) m_pSnd_Click->Play();	// ���尡 ������ play �ϱ�
				dwRet |= UI_MOUSEPROC_DONESOMETHING;
				return dwRet;
			}
		}
		else if(dwFlags & UI_MOUSE_LBCLICKED)  // ���ʹ�ư ���� ����
		{
			if(UI_STATE_BUTTON_DOWN_2CHECKDOWN == m_eState) // ���� ���°� 2CHECKDOWN �����̸�
			{
				SetState(UI_STATE_BUTTON_DOWN); // down ���·� �����
				if (m_pParent) m_pParent->ReceiveMessage(this, UIMSG_BUTTON_CLICK); // �θ𿡰� ��ư Ŭ�� ����..
				dwRet |= UI_MOUSEPROC_DONESOMETHING;
				return dwRet;
			}
			else if (UI_STATE_BUTTON_DOWN_2CHECKUP == m_eState) // ���� ���°� 2CHECKUP �����̸�
			{
				SetState(UI_STATE_BUTTON_ON); // On ���·� �����
				if (m_pParent) m_pParent->ReceiveMessage(this, UIMSG_BUTTON_CLICK); // �θ𿡰� ��ư Ŭ�� ����..
				dwRet |= UI_MOUSEPROC_DONESOMETHING;
				return dwRet;
			}
		}
		else if (UI_STATE_BUTTON_NORMAL == m_eState)	// normal�����̸� on���·�..
		{
			SetState(UI_STATE_BUTTON_ON); // On ���·� �����..
			if (m_pSnd_On) m_pSnd_On->Play();	// ���尡 ������ play �ϱ�
			dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
			return dwRet;
			// UI_MOUSEPROC_DONESOMETHING�� ������ �ȵȴ�.(���콺 �����Ͱ� ��ư���� �ٸ� ��ư���� ������ �Űܰ��� 
			// ���� ��ư�� ���°� �̻������� ���� �����ϱ� ����)
		}

	}
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

bool CN3UIButton::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;

	DWORD dwNum;
	ReadFile(hFile, &m_rcClick, sizeof(m_rcClick), &dwNum, NULL);	// click ����

	// m_ImageRef �����ϱ�
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_IMAGE != pChild->UIType()) continue;	// image�� ��󳻱�
		int iBtnState = (int)(pChild->GetReserved());
		if (iBtnState<NUM_BTN_STATE)
		{
			m_ImageRef[iBtnState] = (CN3UIImage*)pChild;
		}
	}

	// ���� uif������ ������ �Ϸ��� ���� �ε� �ϴ� �κ� ����
	int iSndFNLen = 0;
	ReadFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwNum, NULL);		//	���� ���� ���ڿ� ����
	if (iSndFNLen>0)
	{
		std::vector<char> buffer(iSndFNLen+1, NULL);
		ReadFile(hFile, &buffer[0], iSndFNLen, &dwNum, NULL);

		__ASSERT(NULL == m_pSnd_On, "memory leak");
		m_pSnd_On = s_SndMgr.CreateObj(&buffer[0], SNDTYPE_2D);
	}

	ReadFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwNum, NULL);		//	���� ���� ���ڿ� ����
	if (iSndFNLen>0)
	{
		std::vector<char> buffer(iSndFNLen+1, NULL);
		ReadFile(hFile, &buffer[0], iSndFNLen, &dwNum, NULL);

		__ASSERT(NULL == m_pSnd_Click, "memory leak");
		m_pSnd_Click = s_SndMgr.CreateObj(&buffer[0], SNDTYPE_2D);
	}

	return true;
}

#ifdef _N3TOOL
void CN3UIButton::operator = (const CN3UIButton& other)
{
	CN3UIBase::operator = (other);

	m_rcClick = other.m_rcClick;			// Ŭ�� ����
	SetSndOn(other.GetSndFName_On());		// ����
	SetSndClick(other.GetSndFName_Click());	// ����

	// m_ImageRef �����ϱ�
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_IMAGE != pChild->UIType()) continue;	// image�� ��󳻱�
		int iBtnState = (int)(pChild->GetReserved());
		if (iBtnState<NUM_BTN_STATE)
		{
			m_ImageRef[iBtnState] = (CN3UIImage*)pChild;
		}
	}
}

bool CN3UIButton::Save(HANDLE hFile)
{
	if (false == CN3UIBase::Save(hFile)) return false;
	DWORD dwNum;
	WriteFile(hFile, &m_rcClick, sizeof(m_rcClick), &dwNum, NULL);	// click ����

	int iSndFNLen = 0;
	if (m_pSnd_On) iSndFNLen = m_pSnd_On->m_szFileName.size();
	WriteFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwNum, NULL);		//	���� ���� ���ڿ� ����
	if (iSndFNLen>0) WriteFile(hFile, m_pSnd_On->m_szFileName.c_str(), iSndFNLen, &dwNum, NULL);

	iSndFNLen = 0;
	if (m_pSnd_Click) iSndFNLen = m_pSnd_Click->m_szFileName.size();
	WriteFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwNum, NULL);		//	���� ���� ���ڿ� ����
	if (iSndFNLen>0) WriteFile(hFile, m_pSnd_Click->m_szFileName.c_str(), iSndFNLen, &dwNum, NULL);

	return true;
}

// ������ ����ϱ� ���� �Լ� : n3uiImage�� �����Ѵ�.
void CN3UIButton::CreateImages()
{
	int i;
	for (i=0; i<NUM_BTN_STATE; ++i)
	{
		__ASSERT(NULL == m_ImageRef[i],"�̹����� �̹� �Ҵ�Ǿ� �־");
		m_ImageRef[i] = new CN3UIImage();
		m_ImageRef[i]->Init(this);
		m_ImageRef[i]->SetRegion(m_rcRegion);

		m_ImageRef[i]->SetReserved(i);		// ���� ��ȣ(eBTN_STATE) �Ҵ�.
	}
}

void CN3UIButton::SetSndOn(const std::string& strFileName)
{
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_On);
	if (0 == strFileName.size()) return;

	CN3BaseFileAccess tmpBase;
	tmpBase.FileNameSet(strFileName);	// Base��ο� ���ؼ� ����� ��θ� �Ѱ��ش�.

	SetCurrentDirectory(tmpBase.PathGet().c_str());
	m_pSnd_On = s_SndMgr.CreateObj(tmpBase.FileName(), SNDTYPE_2D);
}

void CN3UIButton::SetSndClick(const std::string& strFileName)
{
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Click);
	if (0 == strFileName.size()) return;

	CN3BaseFileAccess tmpBase;
	tmpBase.FileNameSet(strFileName);	// Base��ο� ���ؼ� ����� ��θ� �Ѱ��ش�.

	SetCurrentDirectory(tmpBase.PathGet().c_str());
	m_pSnd_Click = s_SndMgr.CreateObj(tmpBase.FileName(), SNDTYPE_2D);
}

std::string CN3UIButton::GetSndFName_On() const
{
	if (m_pSnd_On) return m_pSnd_On->m_szFileName;
	else return std::string("");
}

std::string CN3UIButton::GetSndFName_Click() const
{
	if (m_pSnd_Click) return m_pSnd_Click->m_szFileName;
	else return std::string("");
}

#endif
