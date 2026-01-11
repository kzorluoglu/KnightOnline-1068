// N3UITrackBar.cpp: implementation of the CN3UITrackBar class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfxBase.h"
#include "N3UITrackBar.h"
#include "N3UIImage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CN3UITrackBar::CN3UITrackBar()
{
	m_eType = UI_TYPE_TRACKBAR;

	m_pBkGndImageRef = NULL;
	m_pThumbImageRef = NULL;
	m_iMaxPos = 10;
	m_iMinPos = 0;
	m_iCurPos = 0;
	m_iPageSize = 2;
}

CN3UITrackBar::~CN3UITrackBar()
{
}

void CN3UITrackBar::Release()
{
	CN3UIBase::Release();
	m_pBkGndImageRef = NULL;
	m_pThumbImageRef = NULL;
	m_iMaxPos = 10;
	m_iMinPos = 0;
	m_iCurPos = 0;
	m_iPageSize = 2;
}

bool CN3UITrackBar::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;

	// ImageRef �����ϱ�
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_IMAGE != pChild->UIType()) continue;	// image�� ��󳻱�
		int iImageType = (int)(pChild->GetReserved());
		if (IMAGETYPE_BKGND == iImageType)
		{
			m_pBkGndImageRef = (CN3UIImage*)pChild;
		}
		else if (IMAGETYPE_THUMB == iImageType)
		{
			m_pThumbImageRef = (CN3UIImage*)pChild;
		}
	}
	return true;
}

void CN3UITrackBar::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	if(m_pBkGndImageRef) m_pBkGndImageRef->SetRegion(m_rcRegion);	// ����̹����� ���� ��������
	RECT rcThumb = m_pThumbImageRef->GetRegion();

	int iThumbWidth = rcThumb.right - rcThumb.left;		int iThumbHeight = rcThumb.bottom - rcThumb.top;
	int iBkWidth = Rect.right - Rect.left;			int iBkHeight = Rect.bottom - Rect.top;
	if (iBkWidth<=0 || iBkHeight<=0) return;
	
	if ( iThumbWidth<=0 && iThumbHeight<=0 )
	{	// thumb �̹����� �����Ǿ� ���� �ʴٸ� ������ �����ϰ� ����
		RECT rc;
		if (UISTYLE_TRACKBAR_VERTICAL == m_dwStyle)
		{	// ����
			rc.left = Rect.left;		rc.top = Rect.top + iBkHeight*0.3f;
			rc.right = Rect.right;	rc.bottom = rc.top + iBkHeight*0.3f;
		}
		else
		{	// ����
			rc.left = Rect.left + iBkWidth*0.3f;		rc.top = Rect.top;
			rc.right = rc.left + iBkWidth*0.3f;			rc.bottom = Rect.bottom;
		}
		m_pThumbImageRef->SetRegion(rc);
	}
	UpdateThumbPos();	
}

DWORD CN3UITrackBar::MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	DWORD dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;
	if (false == IsIn(ptCur.x, ptCur.y))	// ���� ���̸�
	{
		SetState(UI_STATE_COMMON_NONE);
		return dwRet;
	}
	dwRet |= UI_MOUSEPROC_INREGION;	// �̹���ǥ�� ���� ���̴�.

	if (UI_STATE_TRACKBAR_THUMBDRAG == m_eState)
	{
		if(dwFlags & UI_MOUSE_LBCLICKED)  // ���ʹ�ư ���� ����
		{
			SetState(UI_STATE_COMMON_NONE);		// drag ����
			dwRet |= UI_MOUSEPROC_DONESOMETHING;
			return dwRet;
		}
		else if (dwFlags & UI_MOUSE_LBDOWN)
		{
			// thumb�� �����δ�.
			if (UISTYLE_TRACKBAR_VERTICAL == m_dwStyle)	UpDownThumbPos(ptCur.y - ptOld.y);
			else UpDownThumbPos(ptCur.x - ptOld.x);
			// �θ𿡰� �޼����� ������
			if (m_pParent) m_pParent->ReceiveMessage(this, UIMSG_TRACKBAR_POS);
			dwRet |= UI_MOUSEPROC_DONESOMETHING;
			return dwRet;
		}
	}
	else
	{
		if(dwFlags & UI_MOUSE_LBCLICK)  // ���ʹ�ư ������ ����
		{
			if (m_pThumbImageRef->IsIn(ptCur.x, ptCur.y))	// thumb�� ������.
			{
				SetState(UI_STATE_TRACKBAR_THUMBDRAG);
				dwRet |= UI_MOUSEPROC_DONESOMETHING;
				return dwRet;
			}
			else	// thumb���κ� �Ǵ� �Ʒ� �κ�(�� �Ǵ� �� ����)�� ������.
			{
				RECT rcThumb = m_pThumbImageRef->GetRegion();
				if (UISTYLE_TRACKBAR_VERTICAL == m_dwStyle)
				{
					if (ptCur.y <= rcThumb.top) SetCurrentPos(m_iCurPos-m_iPageSize);// ���κ� Ŭ��
					else SetCurrentPos(m_iCurPos+m_iPageSize);// �Ʒ� �κ� Ŭ��
				}
				else
				{
					if (ptCur.x <= rcThumb.left) SetCurrentPos(m_iCurPos-m_iPageSize);// ���� �κ� Ŭ��
					else SetCurrentPos(m_iCurPos+m_iPageSize);// ������ �κ� Ŭ��
				}
				if (m_pParent) m_pParent->ReceiveMessage(this, UIMSG_TRACKBAR_POS);
				dwRet |= UI_MOUSEPROC_DONESOMETHING;
				return dwRet;
			}
		}
	}
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

//void CN3UITrackBar::Render()
//{
//	if(!m_bVisible) return;
//	if (m_pBkGndImageRef) m_pBkGndImageRef->Render();
//	if (m_pThumbImageRef) m_pThumbImageRef->Render();
//}

void CN3UITrackBar::SetRange(int iMin, int iMax)
{
	if (m_iMaxPos == iMax && m_iMinPos == iMin) return;
	m_iMaxPos = iMax;		m_iMinPos = iMin;
	if (m_iCurPos>m_iMaxPos)	m_iCurPos = m_iMaxPos;
	if (m_iCurPos<m_iMinPos)	m_iCurPos = m_iMinPos;
	UpdateThumbPos();
}

void CN3UITrackBar::SetCurrentPos(int iPos)
{
	if (iPos == m_iCurPos) return;
	m_iCurPos = iPos;
	if (m_iCurPos>m_iMaxPos)	m_iCurPos = m_iMaxPos;
	if (m_iCurPos<m_iMinPos)	m_iCurPos = m_iMinPos;
	UpdateThumbPos();
}

// Pos��ġ�� Thumb�� ��ġ�� ����
void CN3UITrackBar::UpdateThumbPos()
{
	if (NULL == m_pThumbImageRef) return;
	float fDiff = m_iMaxPos - m_iMinPos;
	if (0.0f == fDiff) return;
	float fPercentage = (float)m_iCurPos/fDiff;
	RECT rcThumb = m_pThumbImageRef->GetRegion();

	if (UISTYLE_TRACKBAR_VERTICAL == m_dwStyle)
	{
		int iDY = fPercentage*((m_rcRegion.bottom - m_rcRegion.top) - (rcThumb.bottom - rcThumb.top));
		m_pThumbImageRef->SetPos(	m_rcRegion.left, m_rcRegion.top + iDY);
	}
	else
	{
		int iDX = fPercentage*((m_rcRegion.right - m_rcRegion.left) - (rcThumb.right - rcThumb.left));
		m_pThumbImageRef->SetPos( m_rcRegion.left + iDX, m_rcRegion.top );
	}
}

// thumb�� pixel������ ��ġ �����ϰ� thumb�� ��ġ�� �������� pos ��ġ�� ����Ͽ� ����
void CN3UITrackBar::UpDownThumbPos(int iDiff)
{
	if (NULL == m_pThumbImageRef) return;
	RECT rcThumb = m_pThumbImageRef->GetRegion();

	if (UISTYLE_TRACKBAR_VERTICAL == m_dwStyle)		// �Ʒ� ������ ��
	{
		int iRegionHeight = m_rcRegion.bottom - m_rcRegion.top;
		int iThumbHeight = rcThumb.bottom - rcThumb.top;
		if (0==iRegionHeight || 0==iThumbHeight) return;

		// �ű��� thumb�� ��ġ percentage ���ϱ�
		float fPercentage = ((rcThumb.top-m_rcRegion.top)+iDiff) / (((float)(iRegionHeight)) - iThumbHeight);

		if (fPercentage>1.0f)	// �ʹ� �Ʒ��� ���ȴ�.
		{
			m_pThumbImageRef->SetPos(rcThumb.left, rcThumb.bottom-iThumbHeight);
			m_iCurPos = m_iMaxPos;		// SetCurrentPos�Լ��� ȣ���ϸ� thumb��ġ�� �ٽ� ����ϱ� ������ ���� �ٲپ���.
		}
		else if (fPercentage<0.0f)	// �ʹ� ���� �÷ȴ�.
		{
			m_pThumbImageRef->SetPos(rcThumb.left, rcThumb.top);
			m_iCurPos = m_iMinPos;// SetCurrentPos�Լ��� ȣ���ϸ� thumb��ġ�� �ٽ� ����ϱ� ������ ���� �ٲپ���.
		}
		else
		{
			m_pThumbImageRef->SetPos(rcThumb.left, rcThumb.top+iDiff);
			m_iCurPos = m_iMinPos + (m_iMaxPos-m_iMinPos)*fPercentage;// SetCurrentPos�Լ��� ȣ���ϸ� thumb��ġ�� �ٽ� ����ϱ� ������ ���� �ٲپ���.
		}
	}
	else											// �¿�� ������ ��
	{
		int iRegionWidth = m_rcRegion.right - m_rcRegion.left;
		int iThumbWidth = rcThumb.right - rcThumb.left;
		if (0==iRegionWidth || 0==iThumbWidth) return;

		// �ű��� thumb�� ��ġ percentage ���ϱ�
		float fPercentage = ((rcThumb.left-m_rcRegion.left)+iDiff) / (((float)(iRegionWidth)) - iThumbWidth);

		if (fPercentage>1.0f)	// �ʹ� ���������� �о���.
		{
			m_pThumbImageRef->SetPos(rcThumb.right-iThumbWidth, rcThumb.top);
			m_iCurPos = m_iMaxPos;// SetCurrentPos�Լ��� ȣ���ϸ� thumb��ġ�� �ٽ� ����ϱ� ������ ���� �ٲپ���.
		}
		else if (fPercentage<0.0f)	// �ʹ� �������� �о���
		{
			m_pThumbImageRef->SetPos(rcThumb.left, rcThumb.top);
			m_iCurPos = m_iMinPos;// SetCurrentPos�Լ��� ȣ���ϸ� thumb��ġ�� �ٽ� ����ϱ� ������ ���� �ٲپ���.
		}
		else
		{
			m_pThumbImageRef->SetPos(rcThumb.left+iDiff, rcThumb.top);
			m_iCurPos = m_iMinPos + (m_iMaxPos-m_iMinPos)*fPercentage;// SetCurrentPos�Լ��� ȣ���ϸ� thumb��ġ�� �ٽ� ����ϱ� ������ ���� �ٲپ���.
		}
	}
}

#ifdef _N3TOOL
void CN3UITrackBar::operator = (const CN3UITrackBar& other)
{
	CN3UIBase::operator = (other);

	m_iMaxPos = other.m_iMaxPos;									// �ִ�
	m_iMinPos = other.m_iMinPos;									// �ּ�
	m_iCurPos = other.m_iCurPos;									// ���� ��
	m_iPageSize = other.m_iPageSize;								// page���� �̵��Ҷ� �̵���

	// ImageRef �����ϱ�
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_IMAGE != pChild->UIType()) continue;	// image�� ��󳻱�
		int iImageType = (int)(pChild->GetReserved());
		if (IMAGETYPE_BKGND == iImageType)
		{
			m_pBkGndImageRef = (CN3UIImage*)pChild;
		}
		else if (IMAGETYPE_THUMB == iImageType)
		{
			m_pThumbImageRef = (CN3UIImage*)pChild;
		}
	}
}

void CN3UITrackBar::CreateImages()
{
	__ASSERT(NULL == m_pBkGndImageRef && NULL == m_pThumbImageRef, "�̹����� �̹� �Ҵ�Ǿ� �־");
	m_pBkGndImageRef = new CN3UIImage();
	m_pBkGndImageRef->Init(this);
	m_pBkGndImageRef->SetReserved(IMAGETYPE_BKGND);

	m_pThumbImageRef = new CN3UIImage();
	m_pThumbImageRef->Init(this);
	m_pThumbImageRef->SetReserved(IMAGETYPE_THUMB);

	SetRegion(m_rcRegion);	// ���� �ٽ� ����
}

void CN3UITrackBar::DeleteBkImage()
{
	delete m_pBkGndImageRef;
	m_pBkGndImageRef = NULL;
}

#endif
