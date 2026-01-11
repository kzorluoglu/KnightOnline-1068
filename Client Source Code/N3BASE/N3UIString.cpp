// N3UIString.cpp: implementation of the CN3UIString class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfxBase.h"
#include "N3UIString.h"

#ifndef _REPENT
#ifdef _N3GAME
#include "..\warfare\n3uiwndbase.h"
#include "..\warfare\uiinventory.h"
#endif 
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CN3UIString::CN3UIString()
{
	m_eType = UI_TYPE_STRING;
	m_pDFont = NULL;

	m_Color = 0xffffffff;
	ZeroMemory(&m_ptDrawPos, sizeof(m_ptDrawPos));
	m_iLineCount = 0;
	m_iStartLine = 0;
}

CN3UIString::~CN3UIString()
{
	delete m_pDFont;	m_pDFont = NULL;
}

void CN3UIString::Release()
{
	CN3UIBase::Release();

	m_szString = "";
	delete m_pDFont;	m_pDFont = NULL;
	m_Color = 0xffffffff;

	ZeroMemory(&m_ptDrawPos, sizeof(m_ptDrawPos));
	m_iLineCount = 0;
	m_iStartLine = 0;
	m_NewLineIndices.clear();
}

void CN3UIString::Render()
{
	if(!m_bVisible) return;
	
	__ASSERT(m_pDFont,"m_pDFont is NULL");
	m_pDFont->DrawText( m_ptDrawPos.x, m_ptDrawPos.y, m_Color, 0 );

	CN3UIBase::Render();
}

void CN3UIString::Init(CN3UIBase* pParent)
{
	CN3UIBase::Init(pParent);

	if(m_pDFont) delete m_pDFont; m_pDFont = NULL;
	m_pDFont = new CDFont("����", 16);	// default �� ���� 16���� ����
	m_pDFont->InitDeviceObjects( CN3Base::s_lpD3DDev );
	m_pDFont->RestoreDeviceObjects();
}

void CN3UIString::SetString(const std::string& szString)
{
	m_szString = szString;

	if(m_szString.empty()) m_pDFont->SetText("");
	else WordWrap();
}

void CN3UIString::SetStringAsInt(int iVal)
{
	char szBuff[32] = "";
	sprintf(szBuff, "%d", iVal);
	this->SetString(szBuff);
}

void CN3UIString::SetString_NoWordWrap(const std::string& szString)
{
	// ������ �����ؼ� �׸���.
	m_ptDrawPos.x = m_rcRegion.left;
	m_ptDrawPos.y = m_rcRegion.top;

	m_pDFont->SetText(szString.c_str());
}

void CN3UIString::SetFont(const std::string& szFontName, DWORD dwHeight, BOOL bBold, BOOL bItalic)
{
	DWORD dwFlag = 0;
	if (bBold) dwFlag |= D3DFONT_BOLD;
	if (bItalic) dwFlag |= D3DFONT_ITALIC;
	if (m_pDFont)
	{
		m_pDFont->SetFont(szFontName, dwHeight, dwFlag);
		WordWrap();
	}
}

void CN3UIString::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	WordWrap();
}

void CN3UIString::SetStyle(DWORD dwStyle)
{
	CN3UIBase::SetStyle(dwStyle);
	WordWrap();
}

// �۾���� ��ġ�� �ٲ�� �ش�.
BOOL CN3UIString::MoveOffset(int iOffsetX, int iOffsetY)
{
	if (FALSE == CN3UIBase::MoveOffset(iOffsetX, iOffsetY)) return FALSE;
	m_ptDrawPos.x += iOffsetX;	m_ptDrawPos.y += iOffsetY;
	return TRUE;
}

// ���ڸ� dwStyle�� �°� �������ش�.
void CN3UIString::WordWrap()
{
	m_iLineCount = 0;

	if (NULL == m_pDFont) return;
	int iStrLen = m_szString.size();

	if (0 == iStrLen)
	{
		m_pDFont->SetText("");

		// �۾��� ������ ������ edit�� ��� caret�� ��⶧���� �۾� �׸��� ��ǥ�� �������ش�.
		if (m_dwStyle & UISTYLE_STRING_SINGLELINE)
		{
			if (m_dwStyle & UISTYLE_STRING_ALIGNCENTER)	m_ptDrawPos.x = m_rcRegion.left + (m_rcRegion.right - m_rcRegion.left)/2;
			else m_ptDrawPos.x = m_rcRegion.left;

			if (m_dwStyle & UISTYLE_STRING_ALIGNVCENTER)
			{
				// ���ڿ��� pixel ���� ����
				SIZE sizeTmp = {0, 0};
				m_pDFont->GetTextExtent("��", 2, &sizeTmp);
				m_ptDrawPos.y = m_rcRegion.top + ((m_rcRegion.bottom - m_rcRegion.top-sizeTmp.cy)/2);
			}
			else m_ptDrawPos.y = m_rcRegion.top;
		}
		else
		{
			m_ptDrawPos.x = m_rcRegion.left;
			m_ptDrawPos.y = m_rcRegion.top;
		}

		return;
	}

	// ���ڿ��� pixel ���� ����
	SIZE size;
	const std::string szString = GetString();
	iStrLen = szString.size();
	if (FALSE == m_pDFont->GetTextExtent(szString.c_str(), iStrLen, &size))
	{	// ���̸� ������ �� �������(m_hDC�� �������� �ʾ������)
		m_pDFont->SetText(szString);
		return;
	}

	int iRegionWidth = m_rcRegion.right - m_rcRegion.left;
	int iRegionHeight = m_rcRegion.bottom - m_rcRegion.top;

	if (m_dwStyle & UISTYLE_STRING_SINGLELINE)
	{
		// �ӽ� ���� ���
		std::string szNewBuff;

		if (size.cy>iRegionHeight)	// ���� ���̰� �ش� �������� ū ���
		{	
			m_ptDrawPos.y = m_rcRegion.top;
			m_pDFont->SetText("���� ���̰� STRING control���� Ů�ϴ�.");
			return;
		}
		else if (size.cx <= iRegionWidth)	// ���� ���� ���̰� ���� ���̺��� �������
		{
			szNewBuff = m_szString;

			// ���� ���� 
			if (m_dwStyle & UISTYLE_STRING_ALIGNRIGHT) m_ptDrawPos.x = m_rcRegion.right - size.cx;
			else if (m_dwStyle & UISTYLE_STRING_ALIGNCENTER) m_ptDrawPos.x = m_rcRegion.left + ((iRegionWidth-size.cx)/2);
			else m_ptDrawPos.x = m_rcRegion.left;
			// ���� ����
			if (m_dwStyle & UISTYLE_STRING_ALIGNBOTTOM) m_ptDrawPos.y = m_rcRegion.bottom-size.cy;
			else if (m_dwStyle & UISTYLE_STRING_ALIGNVCENTER) m_ptDrawPos.y = m_rcRegion.top + ((iRegionHeight-size.cy)/2);
			else m_ptDrawPos.y = m_rcRegion.top;
			m_iLineCount = 1;
		}
		else	// ���� ���� ���̰� ���� ���̺��� Ŭ��� �����ȿ� ���� ���ڸ� ǥ������.
		{
			// ���� ������ ���� ���ķ�.
			m_ptDrawPos.x = m_rcRegion.left;
			// ���� ����
			if (m_dwStyle & UISTYLE_STRING_ALIGNBOTTOM) m_ptDrawPos.y = m_rcRegion.bottom-size.cy;
			else if (m_dwStyle & UISTYLE_STRING_ALIGNVCENTER) m_ptDrawPos.y = m_rcRegion.top + ((iRegionHeight-size.cy)/2);
			else m_ptDrawPos.y = m_rcRegion.top;

			// ���� �ڸ��� �ڵ�
			int iCount = 0;
			while(iCount<iStrLen)
			{
				if ('\n' == szString[iCount])		// \n
				{
					break;
				}
				else if (0x80 & szString[iCount])	// 2BYTE ����
				{
					BOOL bFlag = m_pDFont->GetTextExtent(szString.c_str(), iCount+2, &size);
					__ASSERT(bFlag, "cannot get size of dfont");
					if (size.cx>iRegionWidth) break;
					iCount += 2;
				}
				else								// 1BYTE ����
				{
					BOOL bFlag = m_pDFont->GetTextExtent(szString.c_str(), iCount+1, &size);
					__ASSERT(bFlag, "cannot get size of dfont");
					if (size.cx>iRegionWidth) break;
					++iCount;
				}
			}
			// strDisplayString �� iCount ��ŭ�� ī���ϱ�
			szNewBuff = m_szString.substr(0, iCount);	// +1�� �� �������� NULL �ֱ� ����
			m_iLineCount = 1;
		}
		m_pDFont->SetText(szNewBuff);

	}
	else
	{	// mutiline�϶��� �׻� ���� �����ؼ� �׸���.
		m_ptDrawPos.x = m_rcRegion.left;
		m_ptDrawPos.y = m_rcRegion.top;

		m_NewLineIndices.clear();	// ���ο� ���� �ε��� Ŭ����

		// ���� �ڸ��� �ڵ�, ���� ������ ����� ���ڴ� ���� �ʴ´�.
		int iCX=0;//, iCY=0;
		int iCount = 0;

		// �켱 �� ó�� ������ �� �� �ִ� ũ������ üũ�ϱ�
		BOOL bFlag = m_pDFont->GetTextExtent("��", 2, &size);
		__ASSERT(bFlag, "cannot get size of dfont");
//		iCY += size.cy;
//		if (iCY > iRegionHeight)
		if (size.cy > iRegionHeight)
		{
			m_pDFont->SetText("���� ���̰� STRING control���� Ů�ϴ�.");
			return;
		}

		m_iLineCount = 1;	// ������� ���� 1���� ������.
		m_NewLineIndices.push_back(0);	// �� ÷ ������ 0

		while(iCount<iStrLen)
		{
			if ('\n' == szString[iCount])		// \n
			{
//				szNewBuff += '\n';
//				iCY += size.cy;
//				if (iCY > iRegionHeight) break;	// ���� ������ ������ ���̻� ���ڸ� ���� �ʴ´�.
				++iCount;
				iCX = 0;
				if (iCount<iStrLen-1)
				{
					++m_iLineCount;	// ������ ���ڰ� �ƴҰ�� ���� ���ϱ� 
					m_NewLineIndices.push_back(iCount);
				}
			}
			else
			{
				int iCC=0;
				if (0x80 & szString[iCount])	iCC = 2;	// 2BYTE ����
				else iCC = 1;	// 1BYTE ����

				BOOL bFlag = m_pDFont->GetTextExtent(&(szString[iCount]), iCC, &size);
				__ASSERT(bFlag, "cannot get size of dfont");
				if ((iCX+size.cx) > iRegionWidth)	// ���� ���̰� �Ѿ�����
				{
//					szNewBuff += '\n';	// �����ٷ� ������.
					iCX = 0;
//					iCY += size.cy;
//					if (iCY > iRegionHeight) break;	// ���� ������ ������ ���̻� ���ڸ� ���� �ʴ´�.
					if (iCount<iStrLen-1)
					{
						++m_iLineCount;	// ������ ���ڰ� �ƴҰ�� ���� ���ϱ� 
						m_NewLineIndices.push_back(iCount);
					}
				}
				// ���� ī��
//				szNewBuff += szString.substr(iCount, iCC);
				
				iCount += iCC;
				iCX += size.cx;
			}
		}
		SetStartLine(0);
	}
	//m_pDFont->SetText(szNewBuff);
}

void CN3UIString::SetStartLine(int iLine)
{
	if ((m_dwStyle & UISTYLE_STRING_SINGLELINE) || iLine >= m_iLineCount) return;
	m_iStartLine = iLine;

	SIZE size = {0,0};
	BOOL bFlag = m_pDFont->GetTextExtent("��", 2, &size);
	__ASSERT(bFlag, "cannot get size of dfont");
	if (0 == size.cy) return;

	int iEndLine = m_iStartLine + ((m_rcRegion.bottom - m_rcRegion.top)/size.cy);
	bool bMoreLine = true;
	if (iEndLine >= m_iLineCount)
	{
		iEndLine = m_iLineCount;
		bMoreLine = false;
	}
	int i, iCC, iSize;
	std::string strNew;
	for (i=m_iStartLine; i<iEndLine-1; ++i)
	{
		iCC = m_NewLineIndices[i+1] - m_NewLineIndices[i];
		if (iCC>0)
		{
			strNew += m_szString.substr(m_NewLineIndices[i], iCC);
			iSize = strNew.size();
			if ((iSize>0) && ('\n' != strNew[iSize-1])) strNew += "\n";
		}
	}
	// �������� ó��
	if (bMoreLine)
	{
		iCC = m_NewLineIndices[iEndLine] - m_NewLineIndices[iEndLine-1];
		if (iCC>0) strNew += m_szString.substr(m_NewLineIndices[i], iCC);
	}
	else
	{
		iCC = m_szString.size() - m_NewLineIndices[iEndLine-1];
		if (iCC>0) strNew += m_szString.substr(m_NewLineIndices[i], iCC);
	}
	m_pDFont->SetText(strNew);
}

bool CN3UIString::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;
	DWORD dwNum;
	// font ����
	int iStrLen = 0;
	ReadFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, NULL);			// font �̸� ���� 
	if (iStrLen>0)
	{
		std::string szFontName(iStrLen, '?');
		ReadFile(hFile, &(szFontName[0]), iStrLen, &dwNum, NULL);				// string

		DWORD dwFontFlags = 0, dwFontHeight = 0;
		ReadFile(hFile, &dwFontHeight, sizeof(dwFontHeight), &dwNum, NULL);	// font height
		ReadFile(hFile, &dwFontFlags, sizeof(dwFontFlags), &dwNum, NULL);	// font flag (bold, italic)

		SetFont(szFontName, dwFontHeight, dwFontFlags & D3DFONT_BOLD, dwFontFlags & D3DFONT_ITALIC);	// �۲� ����
	}
#ifdef _N3TOOL
	else
	{
		SetFont("����", 10, FALSE, FALSE);	// �ӽ÷� �۲� ����
		MessageBox(GetActiveWindow(), "��Ʈ�� �������� ���� UIString�� �־ ����(10)���� �����Ͽ����ϴ�.", "No font", MB_OK);
	}
#else
	__ASSERT(iStrLen>0, "No font name");
#endif

	// string
	ReadFile(hFile, &m_Color, sizeof(m_Color), &dwNum, NULL);			// ���� ��
	ReadFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, NULL);			// string ���� 
	if (iStrLen>0)
	{
		std::string szString(iStrLen, '?');
		ReadFile(hFile, &(szString[0]), iStrLen, &dwNum, NULL);				// string
		SetString(szString);
	}
	return true;
}

void CN3UIString::operator = (const CN3UIString& other)
{
	CN3UIBase::operator = (other);

	m_ptDrawPos = other.m_ptDrawPos;		// ���� ȭ�鿡 ǥ�õ� ������ ���� ���� ��� ��ǥ
	m_Color = other.m_Color;			// ���� ��

	// ��Ʈ ����
	DWORD dwFontFlags = other.GetFontFlags();
	SetFont(other.GetFontName(), other.GetFontHeight(), dwFontFlags & D3DFONT_BOLD, dwFontFlags & D3DFONT_ITALIC);

	// �۾� ����
	this->SetString(other.m_szString); // m_szString = other.m_szString;			// string buffer
}

#ifdef _N3TOOL
bool CN3UIString::Save(HANDLE hFile)
{
	if (false == CN3UIBase::Save(hFile)) return false;
	DWORD dwNum;
	// font ����
	char* pszFontName = NULL;
	__ASSERT(m_pDFont, "no font");
	const std::string strFontName(m_pDFont->GetFontName());
	int iStrLen = strFontName.size();
	__ASSERT(iStrLen>0, "No font name");
	WriteFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, NULL);			// font �̸� ���� 
	if (iStrLen>0)
	{
		WriteFile(hFile, strFontName.c_str(), iStrLen, &dwNum, NULL);				// string
		DWORD dwFontFlags = 0, dwFontHeight = 0;
		if (m_pDFont)
		{
			dwFontHeight = m_pDFont->GetFontHeight();
			dwFontFlags = m_pDFont->GetFontFlags();
		}
		WriteFile(hFile, &dwFontHeight, sizeof(dwFontHeight), &dwNum, NULL);	// font height
		WriteFile(hFile, &dwFontFlags, sizeof(dwFontFlags), &dwNum, NULL);	// font flag (bold, italic)
	}

	// string
	WriteFile(hFile, &m_Color, sizeof(m_Color), &dwNum, NULL);			// ���� ��
	iStrLen = 0;
	iStrLen = m_szString.size();
	WriteFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, NULL);			// string ���� 
	if (iStrLen>0)
	{
		WriteFile(hFile, m_szString.c_str(), iStrLen, &dwNum, NULL);				// string
	}
	return true;
}

void CN3UIString::ChangeFont(const std::string& szFont)
{
	if(m_pDFont)
	{
		DWORD dwFlag = m_pDFont->GetFontFlags();
		bool bBold = (dwFlag & D3DFONT_BOLD) ? true : false;
		bool bItalic = (dwFlag & D3DFONT_ITALIC) ? true : false;
		this->SetFont(szFont, m_pDFont->GetFontHeight(), bBold, bItalic);
	}

	CN3UIBase::ChangeFont(szFont);
}
#endif

int CN3UIString::GetStringRealWidth(int iNum)
{
	SIZE size;
	BOOL bFlag = m_pDFont->GetTextExtent("��", lstrlen("��"), &size);
	__ASSERT(bFlag, "cannot get size of dfont");
	int iLength = iNum/2;
	if (iLength == 0) return 0;
	return (size.cx*iLength); 
}

void CN3UIString::SetStyle(DWORD dwType, DWORD dwStyleEx)
{
	if (dwType == UI_STR_TYPE_LINE)
	{
		DWORD dwStyle = m_dwStyle;
		if (UISTYLE_STRING_SINGLELINE == dwStyleEx) dwStyle |= UISTYLE_STRING_SINGLELINE;
		else if (UISTYLE_STRING_MULTILINE == dwStyleEx) dwStyle &= (~UISTYLE_STRING_SINGLELINE);
		SetStyle(dwStyle);
	}
	else if (dwType == UI_STR_TYPE_HALIGN)
	{
		DWORD dwStyle = m_dwStyle;
		dwStyle &= (~UISTYLE_STRING_ALIGNLEFT);
		dwStyle &= (~UISTYLE_STRING_ALIGNCENTER);
		dwStyle &= (~UISTYLE_STRING_ALIGNRIGHT);
		if (UISTYLE_STRING_ALIGNLEFT == dwStyleEx) dwStyle |= UISTYLE_STRING_ALIGNLEFT;
		else if (UISTYLE_STRING_ALIGNCENTER == dwStyleEx) dwStyle |= UISTYLE_STRING_ALIGNCENTER;
		else if (UISTYLE_STRING_ALIGNRIGHT == dwStyleEx) dwStyle |= UISTYLE_STRING_ALIGNRIGHT;
		SetStyle(dwStyle);
	}
	else if (dwType == UI_STR_TYPE_VALIGN)
	{
		DWORD dwStyle = m_dwStyle;
		dwStyle &= (~UISTYLE_STRING_ALIGNTOP);
		dwStyle &= (~UISTYLE_STRING_ALIGNVCENTER);
		dwStyle &= (~UISTYLE_STRING_ALIGNBOTTOM);
		if (UISTYLE_STRING_ALIGNTOP == dwStyleEx) dwStyle |= UISTYLE_STRING_ALIGNTOP;
		else if (UISTYLE_STRING_ALIGNVCENTER == dwStyleEx) dwStyle |= UISTYLE_STRING_ALIGNVCENTER;
		else if (UISTYLE_STRING_ALIGNBOTTOM == dwStyleEx) dwStyle |= UISTYLE_STRING_ALIGNBOTTOM;
		SetStyle(dwStyle);
	}
}

DWORD CN3UIString::MouseProc(DWORD dwFlags, const POINT &ptCur, const POINT &ptOld)
{
	DWORD dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;

#ifndef _REPENT
#ifdef _N3GAME
	if ( CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer ) return dwRet;
#endif
#endif

	// Ư�� �̺�Ʈ�� ���� �޽��� ����..
	if(IsIn(ptCur.x, ptCur.y) && (dwFlags & UI_MOUSE_LBCLICKED) )	
	{
		m_pParent->ReceiveMessage(this, UIMSG_STRING_LCLICK); // �θ𿡰� ��ư Ŭ�� ����..
//		dwRet |= UI_MOUSEPROC_DONESOMETHING;
	}

	if(IsIn(ptCur.x, ptCur.y) && (dwFlags & UI_MOUSE_LBDBLCLK) )	
	{
		m_pParent->ReceiveMessage(this, UIMSG_STRING_LDCLICK); // �θ𿡰� ��ư Ŭ�� ����..
//		dwRet |= UI_MOUSEPROC_DONESOMETHING;
	}

//	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}
