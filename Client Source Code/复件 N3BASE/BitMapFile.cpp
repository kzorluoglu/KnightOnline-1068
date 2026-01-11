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
		MessageBox(s_hWndBase, "���� ������ bitmap������ �ƴմϴ�.", "error", MB_OK);
		return FALSE;
	}

	// BITMAPINFOHEADER ���
	ReadFile(hFile, &m_bmInfoHeader, sizeof(m_bmInfoHeader), &dwRWC, NULL);

	// �ȼ��� ��Ʈ �� Ȯ��
	WORD wBitCount = m_bmInfoHeader.biBitCount;
	if (24 != wBitCount || m_bmInfoHeader.biWidth <= 0 || m_bmInfoHeader.biHeight <= 0)		// 24��Ʈ bmp�� �ƴϸ� return�� ������.
	{
		MessageBox(s_hWndBase, "���� bitmap�� �ʺ�, ���̿� �̻��� �ְų� 24bit������ �ƴմϴ�.", "error", NULL);
		return FALSE;
	}

	// ���� �̹����� �޸𸮻� ���� ���� ���� (24bit)
	int iRealWidthSrc = ((int)((m_bmInfoHeader.biWidth*3 + 3)/4))*4;	

	// ���� ���� �̹��� �޸� �Ҵ�
	int iDIBSize = iRealWidthSrc * m_bmInfoHeader.biHeight;

	if ((m_pPixels = ::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, iDIBSize )) == NULL )
	{
		MessageBox(s_hWndBase, "�޸𸮸� �Ҵ����� ���߽��ϴ�.", "error", MB_OK);
		return FALSE;
	}

	// �ȼ��� �д´�..
	for(int y = m_bmInfoHeader.biHeight; y >= 0; y--) // ��Ʈ���� ���Ʒ��� �Ųٷ� �ִ�..
	{
		ReadFile(hFile, (BYTE*)m_pPixels + y * iRealWidthSrc, iRealWidthSrc, &dwRWC, NULL);
	}

	return TRUE;
}

void* CBitMapFile::Pixels()
{
	return m_pPixels;
}

bool CBitMapFile::Save(HANDLE hFile)
{
	DWORD dwRWC = 0;

	// ���� ��� ����
	WriteFile(hFile, &m_bmfHeader, sizeof(m_bmfHeader), &dwRWC, NULL);

	// BITMAPINFOHEADER ����
	WriteFile(hFile, &m_bmInfoHeader, sizeof(m_bmInfoHeader), &dwRWC, NULL);

	// ���� �̹����� �޸𸮻� ���� ���� ���� (24bit)
	int iRealWidthSrc = ((int)((m_bmInfoHeader.biWidth*3 + 3)/4))*4;	

	// ���� ���� �̹��� �޸� �Ҵ�
	int iDIBSize = iRealWidthSrc * m_bmInfoHeader.biHeight;

	// �ȼ��� �����Ѵ�...
	WriteFile(hFile, m_pPixels, iDIBSize, &dwRWC, NULL);

	CloseHandle(hFile);
	return TRUE;
}

bool CBitMapFile::SaveRectToFile(const char* szFileName, RECT rc)
{
	if(rc.right <= rc.left) return false;
	if(rc.bottom <= rc.top) return false;
	if(rc.left < 0) rc.left = 0;
	if(rc.top < 0) rc.top = 0;
	if(rc.right > m_bmInfoHeader.biWidth) rc.right = m_bmInfoHeader.biWidth;
	if(rc.bottom > m_bmInfoHeader.biHeight) rc.bottom = m_bmInfoHeader.biHeight;

	int nWidth = rc.right - rc.left;
	int nHeight = rc.bottom - rc.top;

	if (lstrlen(szFileName) == 0 || nWidth<=0 || nHeight<=0)
	{
		MessageBox(s_hWndBase, "���� ���ΰ� 0������ bitmap���� �����Ҽ� �����ϴ�.", "error", MB_OK);
		return FALSE;
	}

	DWORD dwRWC = 0;
	HANDLE hFile = ::CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	// ���� ���� ���� ����
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(s_hWndBase, "���� bitmap�� �� �� �����ϴ�.", "error", MB_OK);
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
	RECT rcPixel = { rc.left, m_bmInfoHeader.biHeight - rc.bottom, rc.right, m_bmInfoHeader.biHeight - rc.top };
	int iRealWidthSrc = ((int)((m_bmInfoHeader.biWidth*3 + 3)/4))*4;	
	for(int y = rcPixel.bottom - 1; y >= rcPixel.top; y--)
	{
		void* pPixelDest = ((byte*)m_pPixels) + iRealWidthSrc * y + (rcPixel.left * 3);
		WriteFile(hFile, pPixelDest, iRealWidthDest, &dwRWC, NULL); // ���� ����..
	}

	CloseHandle(hFile);
	return true;
}
