// DFont.h: interface for the CDFont class.
// ���� �� �������
// 1. SetText�Լ��� ��귮�� ����.(�۾��� �ٲܶ��� �θ���(�߿�))
// 2. DrawText �Լ��� 2d���� �׷��ش�.
// 3. Render3DText �Լ��� 3d�� �׸��� ����Ѵ�.(SetText�Լ����� 3D��� ��������� �Ѵ�.)
// ���� : �Ƕ��⸦ ���� 50���ۿ� �������� ���Ѵ�.(�� �̻� ������ �Ϸ��� MAX_NUM_VERTICES �� �����ָ� �ȴ�.)
// 4. InvalidateDeviceObjects();�� ȣ������ �ٽ� �۾��� ������ �ٽ� SetText�Լ��� ȣ������� �Ѵ�.
// 5. 3D������ �۾��� ������ ��� �����ؼ� ��� ������ SetText�Լ��� �ι�° �Ķ���Ϳ� (D3DFONT_3D|D3DFONT_CENTERED)�� �־��ش�.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DFONT_H__B1A14901_0027_40BC_8A6C_7FC78DE38686__INCLUDED_)
#define AFX_DFONT_H__B1A14901_0027_40BC_8A6C_7FC78DE38686__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Font creation flags
#define D3DFONT_BOLD        0x0001
#define D3DFONT_ITALIC      0x0002

// Font rendering flags
#define D3DFONT_CENTERED    0x0001		// 3D font������..
#define D3DFONT_TWOSIDED    0x0002		// 3D font������..
#define D3DFONT_FILTERED    0x0004		// texture�� D3DTSS_MAGFILTER �� ���
#define D3DFONT_3D		    0x0008		// 3D�� �������ϱ�

class CDFont  
{
public:
	CDFont(const char* strFontName, DWORD dwHeight, DWORD dwFlags=0L);
	virtual ~CDFont();

// Attributes
public:
	SIZE	GetSize() const {return m_Size;}
protected:
    char	m_strFontName[80];				// Font properties
    DWORD   m_dwFontHeight;					// Font Size
    DWORD   m_dwFontFlags;

    LPDIRECT3DDEVICE8       m_pd3dDevice;	// A D3DDevice used for rendering
    LPDIRECT3DTEXTURE8      m_pTexture;		// The d3d texture for this font
    LPDIRECT3DVERTEXBUFFER9 m_pVB;			// VertexBuffer for rendering text
    DWORD   m_dwTexWidth;					// Texture dimensions
    DWORD   m_dwTexHeight;					// Texture dimensions
    FLOAT   m_fTextScale;					// �� ��Ʈ�� �ʹ� Ŭ��� ���� ī�忡
											// ���� texture ũ�� ������ �Ѿ������ ������
											// �̷� ��� Scale�� �̿��Ͽ� ũ�� �÷� ��´�.

    // Stateblocks for setting and restoring render states
    DWORD   m_dwSavedStateBlock;			// ��Ʈ ��� ������ ���������ϴ� ����
    DWORD   m_dwDrawTextStateBlock;			// ��Ʈ ������ ���¸� �����ϴ� ����

	HDC		m_hDC;							// DC handle
	HFONT	m_hFont;						// Font handle
	UINT	m_iPrimitiveCount;				// �۾� ���� ���� ����
	D3DXVECTOR2	m_PrevLeftTop;				// DrawText�� ��� ��� ���� ��ġ�� ����Ǿ������� ���� ����
	DWORD		m_dwFontColor;				// �۾� ��
	BOOL		m_Is2D;						// 2D Text�ΰ�?
	SIZE		m_Size;						// �� �۾����� �����ϴ� ũ��(pixel����, ���� ����)

// Operations
public:
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice );	// d3d device�� �����ִ� �ʱ�ȭ �Լ� (Init�Ҷ� ȣ��)
	HRESULT RestoreDeviceObjects();								// resource�� �޸𸮿� �����ϴ� �ʱ�ȭ �Լ� (Init�Ҷ� ȣ��)
	HRESULT InvalidateDeviceObjects();							// resource���� ��ȿȭ��Ű�� �Լ� (release�Ҷ� ȣ��)
	HRESULT DeleteDeviceObjects();								// resource���� �޸𸮿��� ���� (release�Ҷ� ȣ��)

	HRESULT SetText(const char* pszText, DWORD dwFlags = 0L);	// ����� �۾��� �޶��������� ȣ���ϴ� ���� �߿�.
	HRESULT DrawText( FLOAT sx, FLOAT sy, DWORD dwColor, DWORD dwFlags );	// ���ۿ� ����� �۾��� �׸���.(2d)
	HRESULT Render3DText(DWORD dwColor, DWORD dwFlags );		// ���ۿ� ����� �۾��� �׸���.(3d)

	HRESULT	SetFont(const char* strFontName, DWORD dwHeight, DWORD dwFlags=0L);	// Font�� �ٲٰ� ������ ȣ���Ѵ�.
protected:
	void Make2DVertex(const int iFontHeight, const char* pszText);					// �Է� ���� ���ڸ� �����ϰ� ��ġ�� 2d ���������� �����.
	void Make3DVertex(const int iFontHeight, const char* pszText, DWORD dwFlags);	// �Է� ���� ���ڸ� �����ϰ� ��ġ�� 3d �������� �����.

};

#endif // !defined(AFX_DFONT_H__B1A14901_0027_40BC_8A6C_7FC78DE38686__INCLUDED_)
