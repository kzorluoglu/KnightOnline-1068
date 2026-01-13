// N3UIStatic.cpp: implementation of the CN3UIStatic class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfxBase.h"
#include "N3UIStatic.h"
#include "N3UIString.h"
#include "N3UIImage.h"

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

CN3UIStatic::CN3UIStatic()
{
	m_eType = UI_TYPE_STATIC;

	m_pBuffOutRef = NULL;
	m_pImageBkGnd = NULL;
	m_pSnd_Click = NULL;
}

CN3UIStatic::~CN3UIStatic()
{
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Click);
}

void CN3UIStatic::Release()
{
	CN3UIBase::Release();
	m_pBuffOutRef = NULL;
	m_pImageBkGnd = NULL;
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Click);
}

//void CN3UIStatic::Render()
//{
//	if (!m_bVisible) return;
//	
//	CN3UIBase::Render();
//
////	if (m_pImageBkGnd) m_pImageBkGnd->Render();
////	if (m_pBuffOutRef) m_pBuffOutRef->Render();
//}

void CN3UIStatic::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		(*itor)->SetRegion(Rect);
	}
}

bool CN3UIStatic::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;

	// m_pImageBkGnd,  m_pBuffOutRef �����ϱ�
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_IMAGE == pChild->UIType())
		{
			m_pImageBkGnd = (CN3UIImage*)pChild;
		}
		else if (UI_TYPE_STRING == pChild->UIType())
		{
			m_pBuffOutRef = (CN3UIString*)pChild;
		}
	}
	
	// ���� uif������ ������ �Ϸ��� ���� �ε� �ϴ� �κ� ����
	int iSndFNLen = 0;
	DWORD dwNum;
	ReadFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwNum, NULL);		//	���� ���� ���ڿ� ����
	if (iSndFNLen>0)
	{
		std::vector<char> buffer(iSndFNLen+1, NULL);
		ReadFile(hFile, &buffer[0], iSndFNLen, &dwNum, NULL);

		std::string strSnd = &buffer[0];
		size_t pos = strSnd.find("UI_en");
		if (pos != std::string::npos) {
			strSnd.replace(pos, 5, "UI_CH");
			strcpy(&buffer[0], strSnd.c_str());
		}
		__ASSERT(NULL == m_pSnd_Click, "memory leak");
		m_pSnd_Click = s_SndMgr.CreateObj(&buffer[0], SNDTYPE_2D);
	}

	return true;
}

const std::string& CN3UIStatic::GetString()
{
	if (m_pBuffOutRef) return m_pBuffOutRef->GetString();
	else return s_szStringTmp;
}

void CN3UIStatic::SetString(const std::string& szString)
{
	if (m_pBuffOutRef) m_pBuffOutRef->SetString(szString);
}


DWORD CN3UIStatic::MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	DWORD dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;

	if((dwFlags & UI_MOUSE_LBCLICK) && IsIn(ptCur.x, ptCur.y))  // ���ʹ�ư ������ ���� ���� ���̸�
	{
		if (m_pSnd_Click) m_pSnd_Click->Play();	// ���尡 ������ play�ϱ�

		if(m_pParent) m_pParent->ReceiveMessage(this, UIMSG_BUTTON_CLICK); // �θ𿡰� ��ư Ŭ�� ����..
		dwRet |= (UI_MOUSEPROC_DONESOMETHING|UI_MOUSEPROC_INREGION);
		return dwRet;
	}

	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

#ifdef _N3TOOL
void CN3UIStatic::operator = (const CN3UIStatic& other)
{
	CN3UIBase::operator = (other);

	SetSndClick(other.GetSndFName_Click());

	// m_pImageBkGnd,  m_pBuffOutRef �����ϱ�
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_IMAGE == pChild->UIType())
		{
			m_pImageBkGnd = (CN3UIImage*)pChild;
		}
		else if (UI_TYPE_STRING == pChild->UIType())
		{
			m_pBuffOutRef = (CN3UIString*)pChild;
		}
	}
}

bool CN3UIStatic::Save(HANDLE hFile)
{
	if (false == CN3UIBase::Save(hFile)) return false;

	DWORD dwNum;
	int iSndFNLen = 0;
	if (m_pSnd_Click) iSndFNLen = m_pSnd_Click->m_szFileName.size();
	WriteFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwNum, NULL);		//	���� ���� ���ڿ� ����
	if (iSndFNLen>0) WriteFile(hFile, m_pSnd_Click->m_szFileName.c_str(), iSndFNLen, &dwNum, NULL);
	return true;
}

void CN3UIStatic::CreateImageAndString()
{
	m_pImageBkGnd = new CN3UIImage();
	m_pImageBkGnd->Init(this);
	m_pImageBkGnd->SetRegion(m_rcRegion);

	m_pBuffOutRef = new CN3UIString();		// ȭ�鿡 ǥ���� ui string �����ϰ� 
	m_pBuffOutRef->Init(this);				// �ʱ�ȭ(�ڵ����� children list ����.)
	m_pImageBkGnd->SetRegion(m_rcRegion);
}

void CN3UIStatic::DeleteImage()
{
	if (m_pImageBkGnd) {delete m_pImageBkGnd; m_pImageBkGnd = NULL;}
}

void CN3UIStatic::SetSndClick(const std::string& strFileName)
{
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Click);
	if (0 == strFileName.size()) return;

	CN3BaseFileAccess tmpBase;
	tmpBase.FileNameSet(strFileName);	// Base��ο� ���ؼ� ����� ��θ� �Ѱ��ش�.

	SetCurrentDirectory(tmpBase.PathGet().c_str());
	m_pSnd_Click = s_SndMgr.CreateObj(tmpBase.FileName(), SNDTYPE_2D);
}

std::string CN3UIStatic::GetSndFName_Click() const
{
	if (m_pSnd_Click) return m_pSnd_Click->m_szFileName;
	else return std::string("");
}
#endif
