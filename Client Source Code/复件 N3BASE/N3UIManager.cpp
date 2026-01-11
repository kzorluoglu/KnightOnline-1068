// N3UIManager.cpp: implementation of the CN3UIManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafxBase.h"
#include "N3UIManager.h"
#include "N3UITooltip.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CN3UIManager::__RenderStateForUI CN3UIManager::s_sRSFU; // RenderStateForUI

CN3UIManager::CN3UIManager()
{

}

CN3UIManager::~CN3UIManager()
{
}

void CN3UIManager::Release()
{
	CN3UIBase::Release();
}

DWORD CN3UIManager::MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	m_dwMouseFlagsCur = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return m_dwMouseFlagsCur;

	if (s_pTooltipCtrl)	s_pTooltipCtrl->MouseProc(dwFlags, ptCur, ptOld);	// �������� ���콺 �޼��� ����.

	// child���� �޼��� ����
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; )
	{
		CN3UIBase* pChild = (*itor);
		DWORD dwChildRet = pChild->MouseProc(dwFlags, ptCur, ptOld);
		if (UI_MOUSEPROC_DONESOMETHING & dwChildRet)
		{	// �̰�쿡�� �հ� ��Ŀ���� ���� ����̴�.
			itor = m_Children.erase(itor);			// �켱 ����Ʈ���� �����
			m_Children.push_front(pChild);	// �Ǿտ� �ִ´�. �׸��� ������ �� ���߿� �׸����� �ϰ� �޼����� �� ���� �ް� �Ϸ���
			m_dwMouseFlagsCur |= (UI_MOUSEPROC_DONESOMETHING|UI_MOUSEPROC_CHILDDONESOMETHING);
			ReorderChildList();	// child list ������(�׻� ���� �ߴ� dialog ������ �ٽ� �����Ѵ�.)
			return m_dwMouseFlagsCur;
		}
		else if ( (	UI_MOUSE_LBCLICK & dwFlags) && (UI_MOUSEPROC_INREGION & dwChildRet) )
		{	// ���� ���� Ŭ�� ������ �հ� ���� �ߴٰ� �ϰ� �����ع�����.
			itor = m_Children.erase(itor);			// �켱 ����Ʈ���� �����
			m_Children.push_front(pChild);	// �Ǿտ� �ִ´�. �׸��� ������ �� ���߿� �׸����� �ϰ� �޼����� �� ���� �ް� �Ϸ���
			m_dwMouseFlagsCur |= (UI_MOUSEPROC_DIALOGFOCUS);
			ReorderChildList();	// child list ������(�׻� ���� �ߴ� dialog ������ �ٽ� �����Ѵ�.)
			return m_dwMouseFlagsCur;
		}
		else ++itor;
		//else if (UI_MOUSE_LBCLICKED|UI_MOUSE_MBCLICK|UI_MOUSE_MBCLICKED|UI_MOUSE_RBCLICK|UI_MOUSE_RBCLICKED)

		m_dwMouseFlagsCur |= dwChildRet;
	}

	return m_dwMouseFlagsCur;
}

void CN3UIManager::ReorderChildList()	// ���̾˷α� ���� ���ġ
{
	int iChildCount = m_Children.size();
	if (iChildCount<=0) return;
	CN3UIBase** ppBuffer = new CN3UIBase*[iChildCount];
	int iAlwaysTopChildCount = 0;

	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; )
	{
		CN3UIBase* pChild = (*itor);
		if (pChild->GetStyle() & UISTYLE_ALWAYSTOP)
		{
			itor = m_Children.erase(itor);			// �켱 ����Ʈ���� �����
			ppBuffer[iAlwaysTopChildCount++] = pChild;
		}
		else ++itor;
	}
	int i;
	for (i=iAlwaysTopChildCount-1; i>=0; --i)
	{
		m_Children.push_front(ppBuffer[i]);	// �Ǿտ� �ִ´�. �׸��� ������ �� ���߿� �׸����� �ϰ� �޼����� �� ���� �ް� �Ϸ���
	}
	delete [] ppBuffer;
}

void CN3UIManager::Tick()
{
	CN3UIBase::Tick();
	if (s_pTooltipCtrl)	s_pTooltipCtrl->Tick();
}

void CN3UIManager::Render()
{
	this->RenderStateSet();

	CN3UIBase::Render();	// �ڽĵ� render
	if (s_pTooltipCtrl) s_pTooltipCtrl->Render();	// tooltip render

	this->RenderStateRestore();
}

void CN3UIManager::RenderStateSet()
{
	if(NULL == s_lpD3DDev) return;

#ifdef _DEBUG
	__ASSERT(FALSE == s_sRSFU.bSet, "������ RenderStateSet()�Լ��� ȣ���ϰ� RenderStateRestore()�Լ��� ȣ����� ���� �����Դϴ�.");
	s_sRSFU.bSet = TRUE;
#endif

	// back up old state
	s_lpD3DDev->GetRenderState(D3DRS_ZENABLE, &(s_sRSFU.dwZEnable));
	s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &(s_sRSFU.dwAlphaBlend));
	s_lpD3DDev->GetRenderState(D3DRS_SRCBLEND, &(s_sRSFU.dwSrcBlend));
	s_lpD3DDev->GetRenderState(D3DRS_DESTBLEND, &(s_sRSFU.dwDestBlend));
	s_lpD3DDev->GetRenderState(D3DRS_FOGENABLE, &(s_sRSFU.dwFog));
	s_lpD3DDev->GetTextureStageState(0, D3DTSS_MAGFILTER,   &(s_sRSFU.dwMagFilter));
	s_lpD3DDev->GetTextureStageState(0, D3DTSS_MINFILTER,   &(s_sRSFU.dwMinFilter));
	s_lpD3DDev->GetTextureStageState(0, D3DTSS_MIPFILTER,   &(s_sRSFU.dwMipFilter));

	// set state
	if (D3DZB_FALSE != s_sRSFU.dwZEnable) s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	if (TRUE != s_sRSFU.dwAlphaBlend) s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	if (D3DBLEND_SRCALPHA != s_sRSFU.dwSrcBlend) s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	if (D3DBLEND_INVSRCALPHA != s_sRSFU.dwDestBlend) s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	if (FALSE != s_sRSFU.dwFog) s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE   , FALSE);	// 2d�� fog�� �Դ´� ��.��;
	if (D3DTEXF_POINT != s_sRSFU.dwMagFilter ) s_lpD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER,   D3DTEXF_POINT);
	if (D3DTEXF_POINT != s_sRSFU.dwMinFilter ) s_lpD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER,   D3DTEXF_POINT);
	if (D3DTEXF_NONE != s_sRSFU.dwMipFilter ) s_lpD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER,   D3DTEXF_NONE);
}

void CN3UIManager::RenderStateRestore()
{
#ifdef _DEBUG
	__ASSERT(TRUE == s_sRSFU.bSet, "������ RenderStateSet()�Լ��� ȣ������ �ʾҽ��ϴ�.");
	s_sRSFU.bSet = FALSE;
#endif

	// restore
	if (D3DZB_FALSE != s_sRSFU.dwZEnable) s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, s_sRSFU.dwZEnable);
	if (TRUE != s_sRSFU.dwAlphaBlend) s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, s_sRSFU.dwAlphaBlend);
	if (D3DBLEND_SRCALPHA != s_sRSFU.dwSrcBlend) s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, s_sRSFU.dwSrcBlend);
	if (D3DBLEND_INVSRCALPHA != s_sRSFU.dwDestBlend) s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, s_sRSFU.dwDestBlend);
	if (FALSE != s_sRSFU.dwFog) s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE   , s_sRSFU.dwFog);
	if (D3DTEXF_POINT != s_sRSFU.dwMagFilter ) s_lpD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER,   s_sRSFU.dwMagFilter);
	if (D3DTEXF_POINT != s_sRSFU.dwMinFilter ) s_lpD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER,   s_sRSFU.dwMinFilter);
	if (D3DTEXF_NONE != s_sRSFU.dwMipFilter ) s_lpD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER,   s_sRSFU.dwMipFilter);
}
