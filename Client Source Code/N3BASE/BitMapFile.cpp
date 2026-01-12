// BitMapFile.cpp: implementation of the CBitMapFile class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfxBase.h"
#include "BitMapFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBitMapFile::CBitMapFile()
{
	m_pPixels = NULL;
	Release();
}

CBitMapFile::~CBitMapFile()
{
	Release();
}

void CBitMapFile::Release()
{
	memset(&m_bmfHeader, 0, sizeof(m_bmfHeader));
	memset(&m_bmInfoHeader, 0, sizeof(m_bmInfoHeader));
	::GlobalFree(m_pPixels); // ���� �ȼ� ������
	m_pPixels = NULL;
}

bool CBitMapFile::Load(HANDLE hFile)
{
	this->Release(); // �ϴ� �� �����ϰ�..

	DWORD dwRWC = 0;

	// ���� ��� �б�
	ReadFile(hFile, &m_bmfHeader, sizeof(m_bmfHeader), &dwRWC, NULL);

	// bmp �������� ��Ÿ���� "BM"��Ŀ Ȯ��
	if (m_bmfHeader.bfType != 0x4D42)
	{
		MessageBox(::GetActiveWindow(), "���� ������ bitmap������ �ƴմϴ�.", "error", MB_OK);
		return FALSE;
	}

	// BITMAPINFOHEADER ���
	ReadFile(hFile, &m_bmInfoHeader, sizeof(m_bmInfoHeader), &dwRWC, NULL);

	// �ȼ��� ��Ʈ �� Ȯ��
	WORD wBitCount = m_bmInfoHeader.biBitCount;
	if (24 != wBitCount || m_bmInfoHeader.biWidth <= 0 || m_bmInfoHeader.biHeight <= 0)		// 24��Ʈ bmp�� �ƴϸ� return�� ������.
	{
		MessageBox(::GetActiveWindow(), "���� bitmap�� �ʺ�, ���̿� �̻��� �ְų� 24bit������ �ƴմϴ�.", "error", NULL);
		return FALSE;
	}

	// ���� �̹����� �޸𸮻� ���� ���� ���� (24bit)
	int iRealWidth = ((int)((m_bmInfoHeader.biWidth*3 + 3)/4))*4;	

	// ���� ���� �̹��� �޸� �Ҵ�
	int iDIBSize = iRealWidth * m_bmInfoHeader.biHeight;

	if ((m_pPixels = ::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, iDIBSize )) == NULL )
	{
		MessageBox(::GetActiveWindow(), "�޸𸮸� �Ҵ����� ���߽��ϴ�.", "error", MB_OK);
		return FALSE;
	}

	// �ȼ��� �д´�..
	for(int y = m_bmInfoHeader.biHeight - 1; y >= 0; y--) // ��Ʈ���� ���Ʒ��� �Ųٷ� �ִ�..
	{
		ReadFile(hFile, (BYTE*)m_pPixels + y * iRealWidth, iRealWidth, &dwRWC, NULL);
	}

	return TRUE;
}

void* CBitMapFile::Pixels(int x, int y)
{
	if(24 != m_bmInfoHeader.biBitCount) return NULL;

	int nPitch = this->Pitch();
	int nPixelSize = 3;
	return (char*)m_pPixels + y * nPitch + x * nPixelSize;
}

bool CBitMapFile::Save(HANDLE hFile)
{
	DWORD dwRWC = 0;

	// ���� ��� ����
	WriteFile(hFile, &m_bmfHeader, sizeof(m_bmfHeader), &dwRWC, NULL);

	// BITMAPINFOHEADER ����
	WriteFile(hFile, &m_bmInfoHeader, sizeof(m_bmInfoHeader), &dwRWC, NULL);

	// ���� �̹����� �޸𸮻� ���� ���� ���� (24bit)
	int iRealWidth = this->Pitch();

	// �ȼ��� �����Ѵ�...
	for(int y = m_bmInfoHeader.biHeight - 1; y >= 0; y--) // ��Ʈ���� ���Ʒ��� �Ųٷ� �ִ�..
	{
		WriteFile(hFile, (BYTE*)m_pPixels + y * iRealWidth, iRealWidth, &dwRWC, NULL);
	}

	return true;
}

bool CBitMapFile::SaveRectToFile(const std::string& szFN, RECT rc)
{
	if(szFN.empty()) return false;

	if(rc.right <= rc.left) return false;
	if(rc.bottom <= rc.top) return false;
	if(rc.left < 0) rc.left = 0;
	if(rc.top < 0) rc.top = 0;
	if(rc.right > m_bmInfoHeader.biWidth) rc.right = m_bmInfoHeader.biWidth;
	if(rc.bottom > m_bmInfoHeader.biHeight) rc.bottom = m_bmInfoHeader.biHeight;

	int nWidth = rc.right - rc.left;
	int nHeight = rc.bottom - rc.top;

	if (nWidth <=0 || nHeight <=0)
	{
		MessageBox(::GetActiveWindow(), "���� ���ΰ� 0������ bitmap���� �����Ҽ� �����ϴ�.", "error", MB_OK);
		return FALSE;
	}

	DWORD dwRWC = 0;
	HANDLE hFile = ::CreateFile(szFN.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	// ���� ���� ���� ����
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(::GetActiveWindow(), "���� bitmap�� �� �� �����ϴ�.", "error", MB_OK);
		return false;
	}

	// ���� �̹����� �޸𸮻� ���� ���� ���� (24bit)
	int iRealWidthDest = ((int)((nWidth*3 + 3)/4))*4;	
	int iDestDIBSize = sizeof(BITMAPINFOHEADER) + iRealWidthDest * nHeight;

	// ���� ���� �̹��� file header ���� ä���
	BITMAPFILEHEADER bmfHeaderDest = m_bmfHeader;
	bmfHeaderDest.bfType = 0x4D42; // "BM"
	bmfHeaderDest.bfSize = sizeof(bmfHeaderDest) + iDestDIBSize;
	bmfHeaderDest.bfOffBits = sizeof(bmfHeaderDest) + sizeof(BITMAPINFOHEADER);

	// ���� ���� �̹��� bitmap info header ���� ä���
	BITMAPINFOHEADER bmInfoHeaderDest = m_bmInfoHeader;
	bmInfoHeaderDest.biSize = sizeof(bmInfoHeaderDest);
	bmInfoHeaderDest.biWidth = nWidth;
	bmInfoHeaderDest.biHeight = nHeight;
	bmInfoHeaderDest.biPlanes = 1;
	bmInfoHeaderDest.biSizeImage = iRealWidthDest * nHeight;

	// ���� ��� ����
	WriteFile(hFile, &bmfHeaderDest, sizeof(bmfHeaderDest), &dwRWC, NULL);

	// BITMAPINFOHEADER ����
	WriteFile(hFile, &bmInfoHeaderDest, sizeof(bmInfoHeaderDest), &dwRWC, NULL);

	// �ȼ��� �����Ѵ�...
	int iRealWidth = ((int)((m_bmInfoHeader.biWidth*3 + 3)/4))*4;	
	for(int y = rc.bottom - 1; y >= rc.top; y--)
	{
		void* pPixelDest = ((byte*)m_pPixels) + iRealWidth * y + (rc.left * 3);
		WriteFile(hFile, pPixelDest, iRealWidthDest, &dwRWC, NULL); // ���� ����..
	}

	CloseHandle(hFile);
	return true;
}

bool CBitMapFile::LoadFromFile(const char* pszFN)
{
	if(NULL == pszFN || lstrlen(pszFN) <= 0)
		return false;

	DWORD dwRWC = 0;
	HANDLE hFile = ::CreateFile(pszFN, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(INVALID_HANDLE_VALUE == hFile)
		return false;

	bool bSuccess =	this->Load(hFile);

	CloseHandle(hFile);

	return bSuccess;
}

bool CBitMapFile::SaveToFile(const char* pszFN)
{
	if(NULL == pszFN || lstrlen(pszFN) <= 0)
		return false;

	DWORD dwRWC = 0;
	HANDLE hFile = ::CreateFile(pszFN, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile == INVALID_HANDLE_VALUE)
		return false;

	this->Save(hFile);

	CloseHandle(hFile);
	return true;
}

bool CBitMapFile::Create(int nWidth, int nHeight, int nBPP)
{
	if(nWidth <= 0 || nHeight <= 0) return false;
	this->Release(); // �ϴ� �� �����ϰ�..

	if(24 != nBPP) return FALSE;

	int iRealWidth = ((nWidth*3 + 3)/4)*4; // ���� �̹����� �޸𸮻� ���� ���� ���� (24bit)
	int iDIBSize = iRealWidth * nHeight; // ���� ���� �̹��� �޸� �Ҵ�

	if ((m_pPixels = ::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, iDIBSize )) == NULL )
	{
		MessageBox(::GetActiveWindow(), "�޸𸮸� �Ҵ����� ���߽��ϴ�.", "error", MB_OK);
		return FALSE;
	}

	memset(m_pPixels, 0, iDIBSize);

	// ���� ���� �̹��� file header ���� ä���
	m_bmfHeader.bfType = 0x4D42; // "BM"
	m_bmfHeader.bfSize = sizeof(m_bmfHeader) + iDIBSize;
	m_bmfHeader.bfOffBits = sizeof(m_bmfHeader) + sizeof(BITMAPINFOHEADER);

	// ���� ���� �̹��� bitmap info header ���� ä���
	m_bmInfoHeader.biSize = sizeof(m_bmInfoHeader);
	m_bmInfoHeader.biWidth = nWidth;
	m_bmInfoHeader.biHeight = nHeight;
	m_bmInfoHeader.biBitCount = nBPP;
	m_bmInfoHeader.biPlanes = 1;
	m_bmInfoHeader.biSizeImage = iRealWidth * nHeight;

	return TRUE;
}
