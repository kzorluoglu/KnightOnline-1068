// DFont.h: interface for the CDFont class.
// ���� �� �������
// 1. SetText�Լ��� ��귮�� ����.(�۾��� �ٲܶ��� �θ���(�߿�))
// 2. DrawText �Լ��� 2d���� �׷��ش�.
// 3. DrawText3D �Լ��� 3d�� �׸��� ����Ѵ�.(SetText�Լ����� 3D��� ��������� �Ѵ�.)
// ���� : �Ƕ��⸦ ���� 50���ۿ� �������� ���Ѵ�.(�� �̻� ������ �Ϸ��� MAX_NUM_VERTICES �� �����ָ� �ȴ�.)
// 4. InvalidateDeviceObjects();�� ȣ������ �ٽ� �۾��� ������ �ٽ� SetText�Լ��� ȣ������� �Ѵ�.
// 5. 3D������ �۾��� ������ ��� �����ؼ� ��� ������ SetText�Լ��� �ι�° �Ķ���Ϳ� (D3DFONT_3D|D3DFONT_CENTERED)�� �־��ش�.
//////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)

#if !defined(AFX_DFONT_H__B1A14901_0027_40BC_8A6C_7FC78DE38686__INCLUDED_)
#define AFX_DFONT_H__B1A14901_0027_40BC_8A6C_7FC78DE38686__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\N3Base\N3Base.h"

// Font creation flags
#define D3DFONT_BOLD        0x0001
#define D3DFONT_ITALIC      0x0002
#define D3DFONT_3D		    0x0008		// 3D text

// set text flag
#define D3DFONT_CENTERED    0x0004		// 3D font������..

// Font rendering flags (drawtext or DrawText3D �Լ����� ���� flag)
#define D3DFONT_TWOSIDED    0x0010		// 3D font������..
#define D3DFONT_FILTERED    0x0020		// texture�� D3DTSS_MAGFILTER �� ���


class CDFont  : public CN3Base
{
public:
	CDFont(const std::string& szFontName, DWORD dwHeight, DWORD dwFlags=0L);
	virtual ~CDFont();

// Attributes
public:
	const std::string& GetFontName() { return m_szFontName; }
	DWORD	GetFontHeight() const {return m_dwFontHeight;}
	int		GetFontHeightInLogicalUnit() const {return -MulDiv(m_dwFontHeight, GetDeviceCaps(s_hDC, LOGPIXELSY), 72);}
	DWORD	GetFontFlags() const {return m_dwFontFlags;}
	SIZE	GetSize() const {return m_Size;}
	DWORD	GetFontColor() const {return m_dwFontColor;}

protected:
	static HDC s_hDC;						// DC handle
	static int s_iInstanceCount;			// Class Instance Count
	static HFONT s_hFontOld;				// default font


	std::string	m_szFontName;				// Font properties
    DWORD		m_dwFontHeight;					// Font Size
    DWORD		m_dwFontFlags;

    LPDIRECT3DDEVICE9       m_pd3dDevice;	// A D3DDevice used for rendering
    LPDIRECT3DTEXTURE9      m_pTexture;		// The d3d texture for this font
    LPDIRECT3DVERTEXBUFFER9 m_pVB;			// VertexBuffer for rendering text
    DWORD	m_dwTexWidth;					// Texture dimensions
    DWORD	m_dwTexHeight;					// Texture dimensions
    FLOAT	m_fTextScale;					// �� ��Ʈ�� �ʹ� Ŭ��� ���� ī�忡
											// ���� texture ũ�� ������ �Ѿ������ ������
											// �̷� ��� Scale�� �̿��Ͽ� ũ�� �÷� ��´�.

//	HDC			m_hDC;							// DC handle
	HFONT		m_hFont;						// Font handle
	UINT		m_iPrimitiveCount;				// �۾� ���� ���� ����
	D3DXVECTOR2	m_PrevLeftTop;				// DrawText�� ��� ��� ���� ��ġ�� ����Ǿ������� ���� ����
	DWORD		m_dwFontColor;				// �۾� ��
	BOOL		m_Is2D;						// 2D Text�ΰ�?
	SIZE		m_Size;						// �� �۾����� �����ϴ� ũ��(pixel����, ���� ����)

// Operations
public:
	bool	IsSetText() { if(m_pTexture) return true; return false; }
	void	AddToAlphaManager(DWORD dwColor, float fDist, __Matrix44& mtxWorld, DWORD dwFlags);
	HRESULT	SetFontColor(DWORD dwColor);						// �۾� ���� �ٲ۴�.
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );	// d3d device�� �����ִ� �ʱ�ȭ �Լ� (Init�Ҷ� ȣ��)
	HRESULT RestoreDeviceObjects();								// resource�� �޸𸮿� �����ϴ� �ʱ�ȭ �Լ� (Init�Ҷ� ȣ��)
	HRESULT InvalidateDeviceObjects();							// resource���� ��ȿȭ��Ű�� �Լ� (release�Ҷ� ȣ��)
	HRESULT DeleteDeviceObjects();								// resource���� �޸𸮿��� ���� (release�Ҷ� ȣ��)

	HRESULT SetText(const std::string& szText, DWORD dwFlags = 0L);	// ����� �۾��� �޶��������� ȣ���ϴ� ���� �߿�.
	HRESULT DrawText( FLOAT sx, FLOAT sy, DWORD dwColor, DWORD dwFlags, FLOAT fZ = 1.0f);	// ���ۿ� ����� �۾��� �׸���.(2d)
	HRESULT DrawText3D(DWORD dwColor, DWORD dwFlags );		// ���ۿ� ����� �۾��� �׸���.(3d)

	HRESULT	SetFont(const std::string& szFontName, DWORD dwHeight, DWORD dwFlags=0L);	// Font�� �ٲٰ� ������ ȣ���Ѵ�. (dwHeight�� point size�� �ִ´�.)
	BOOL	GetTextExtent(const std::string& szString, int iStrLen, SIZE* pSize );
protected:
	void Make2DVertex(const int iFontHeight, const std::string& szText);					// �Է� ���� ���ڸ� �����ϰ� ��ġ�� 2d ���������� �����.
	void Make3DVertex(const int iFontHeight, const std::string& szText, DWORD dwFlags);	// �Է� ���� ���ڸ� �����ϰ� ��ġ�� 3d �������� �����.

};

#endif // !defined(AFX_DFONT_H__B1A14901_0027_40BC_8A6C_7FC78DE38686__INCLUDED_)
