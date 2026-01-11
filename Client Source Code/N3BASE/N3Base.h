// N3Base.h: interface for the CN3Base class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3Base_h__INCLUDED_)
#define AFX_N3Base_h__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786)

#include "My_3DStruct.h"
#include "N3Mng.h"
#include "N3AlphaPrimitiveManager.h"
#ifdef _N3GAME
#include "LogWriter.h"
#endif

#include <d3d9.h>
#include <d3dx9.h>

#include <string>

#if _N3GAME // ������ �ƴ� �������� �ʿ����...
#include "N3SndMgr.h"
#endif
#ifdef _N3UIE	// ui �������϶��� �ʿ��ϴ�.
#include "N3SndMgr.h"
#endif

const DWORD TEX_CAPS_DXT1 =			0x00000001;
const DWORD TEX_CAPS_DXT2 =			0x00000002;
const DWORD TEX_CAPS_DXT3 =			0x00000004;
const DWORD TEX_CAPS_DXT4 =			0x00000008;
const DWORD TEX_CAPS_DXT5 =			0x00000010;
const DWORD TEX_CAPS_SQUAREONLY =	0x00000020;
const DWORD TEX_CAPS_MIPMAP =		0x00000040;
const DWORD TEX_CAPS_POW2 =			0x00000080;

const float CAMERA_RADIUS_UNIT = 2.0f;
const int MAX_CAMERA_RADIUS = 512; // 2���� ������ 128 ���� ��Ʈ ���δ�Ʈ �̸� ����� ���´�..

enum TIMER_COMMAND { TIMER_RESET, TIMER_START, TIMER_STOP, TIMER_ADVANCE,
                     TIMER_GETABSOLUTETIME, TIMER_GETAPPTIME, TIMER_GETELAPSEDTIME };

struct __CameraData
{
	__CameraData()
	{
		Release();
	}
	void	Release()
	{
		memset(this, 0, sizeof(__CameraData));
		mtxView.Identity();
		mtxViewInverse.Identity();
		mtxProjection.Identity();
	}
	__Vector3		vEye; // Camera Position Vector
	__Vector3		vAt; // Camera At Vector
	__Vector3		vUp; // Camera Up Vector

	float			fFOV; // ī�޶� ���� �� : Field Of View
//	float			fInverse_SineHalfOfFOV;
	float			fAspect; // ��Ⱦ��
	float			fNP; // NearPlane
	float			fFP; // FarPlane
	D3DVIEWPORT9	vp; // ViewPort;
	__Matrix44		mtxView;
	__Matrix44		mtxViewInverse;
	__Matrix44		mtxProjection;

	float			fFrustum [6][4];

	// fRadius - ��ü�� ���������� �ణ �� ���� �ְ� ��� ������ �׸�ŭ Ŭ������ �����ְ� �Ѵ�..
	BOOL IsOutOfFrustum(const __Vector3& vPosition, float fRadius) 
		// �̸� ���� ī�޶� ����� ��Ʈ ���δ�Ʈ ���� �������� ī�޶� ���ü �ۿ� ������  ���� �����ش�.
	{
		if((vEye - vPosition).Magnitude() > fFP + fRadius) 
			return TRUE; // Far Plane �Ÿ�üũ

		int p;
		for( p = 0; p < 6; p++ )
		{
			if( fFrustum[p][0] * vPosition.x + fFrustum[p][1] * vPosition.y +
				fFrustum[p][2] * vPosition.z + fFrustum[p][3] <= -fRadius )
				return TRUE;
		}
		return FALSE;
	}
};

struct __RenderInfo
{
	int nShape;
	int nShape_Part;
	int nShape_Polygon; // �ܼ� ������
	
	int nChr;
	int nChr_Part;
	int nChr_Polygon; // ĳ���� ������
	int nChr_Plug; // ĳ���Ϳ� ���� �����..
	int	nChr_Plug_Polygon; // ĳ���Ϳ� ���� ������� ������..

	int nTerrain_Polygon; // Ÿ�� ����� ���� ������..
	int nTerrain_Tile_Polygon; // Ÿ�� ����� ���� ������..

	int nAlpha_Polygon;
	
	int nTexture_32X32; // 32 X 32 Texture
	int nTexture_64X64; // 64 X 64 Texture
	int nTexture_128X128; // 128 X 128 Texture
	int nTexture_256X256; // 256 X 256 Texture
	int nTexture_512X512; // 512 X 512 Texture
	int nTexture_Huge; // 512 X 512 �̻� size
	int nTexture_OtherSize; // Other size
};

struct __ResrcInfo
{
	int nTexture_Loaded_32X32; // 32 X 32 Texture
	int nTexture_Loaded_64X64; // 64 X 64 Texture
	int nTexture_Loaded_128X128; // 128 X 128 Texture
	int nTexture_Loaded_256X256; // 256 X 256 Texture
	int nTexture_Loaded_512X512; // 512 X 512 Texture
	int nTexture_Loaded_Huge; // 512 X 512 �̻� size
	int nTexture_Loaded_OtherSize; // Other size
};

struct __Options
{
	int iUseShadow;
	int iTexLOD_Chr;			// 0 - ���� ũ��.. 1 - �Ѵܰ� �۰�. 2 - �δܰ� �۰�..
	int iTexLOD_Shape;			// 0 - ���� ũ��.. 1 - �Ѵܰ� �۰�. 2 - �δܰ� �۰�..
	int iTexLOD_Terrain;		// 0 - ���� ũ��.. 1 - �Ѵܰ� �۰�. 2 - �δܰ� �۰�..
	int iViewWidth;
	int iViewHeight;
	int iViewColorDepth;
	int iViewDist;
	int iEffectSndDist;			// ����Ʈ ���� �Ÿ�

	bool bSndEnable;		// 0 - High, 1 - Low
	bool bSndDuplicated;	// �ߺ��� ���� ���
	bool bWindowCursor;		// 0 - ���ӿ��� �׷��ִ� Ŀ�� 1 - ������ Ŀ�� ���
	bool bWindowed;			// 0 - Fullscreen, 1 - Windowed

	void InitDefault()
	{
		iUseShadow = true;
		iTexLOD_Chr = 0;
		iTexLOD_Shape = 0;
		iTexLOD_Terrain = 0;
		iViewColorDepth = 16;
		iViewWidth = 1024;
		iViewHeight = 768;
		iViewDist = 512;
		iEffectSndDist = 48;
		bSndEnable = 0;
		bSndDuplicated = false;
		bWindowCursor = true;
		bWindowed = false;
	}
	__Options() { InitDefault(); }
};

class CN3Base
{
public:
	static LPDIRECT3DDEVICE9		s_lpD3DDev; // Device ���� ������.. �ڴ�� �����ϸ� �ȵȴ�..
	static D3DPRESENT_PARAMETERS	s_DevParam; // Device ���� Present Parameter
	static D3DCAPS9					s_DevCaps; // Device ȣȯ��...
	static DWORD					s_dwTextureCaps; // Texture ����.. DXT1 ~ DXT5, Square Only
	static HWND						s_hWndBase; // Init �Ҷ� �� Window Handle
	static HWND						s_hWndPresent; // �ֱٿ� Present �� Window Handle

	static __CameraData				s_CameraData; // ī�޶� ������ ���� ����..
	static __ResrcInfo				s_ResrcInfo; // Rendering Information..
	static __Options				s_Options;	// ���� �ɼǵ�...
#ifdef _DEBUG
	static __RenderInfo				s_RenderInfo; // Rendering Information..
#endif
	static float					s_fFrmPerSec; // Frame Per Second
	static float					s_fSecPerFrm; // Second Per Frame = 1.0f/s_fFrmPerSec (Dino�� �߰�)
	
#ifdef _N3GAME // ������ �ƴ� �������� �ʿ����...
	static CN3SndMgr				s_SndMgr;
#endif
#ifdef _N3UIE	// ui �������϶��� �ʿ��ϴ�.
	static CN3SndMgr				s_SndMgr;
#endif

	static CN3AlphaPrimitiveManager	s_AlphaMgr; // Alpha blend �� ��������� ����.. �߰��ߴٰ�.. ī�޶� �Ÿ��� ���߾� �����ϰ� �Ѳ����� �׸���..

	static CN3Mng<class CN3Texture>		s_MngTex; // Texture Manager
	static CN3Mng<class CN3Mesh>		s_MngMesh; // Normal Mesh Manager
	static CN3Mng<class CN3VMesh>		s_MngVMesh; // �ܼ��� �����︸ ���� �ִ� �޽� - �ַ� �浹 üũ�� ����..
	static CN3Mng<class CN3PMesh>		s_MngPMesh; // Progressive Mesh Manager
	static CN3Mng<class CN3Joint>		s_MngJoint; // Joint Manager
	static CN3Mng<class CN3CPartSkins>	s_MngSkins; // Character Part Skin Manager
	static CN3Mng<class CN3AnimControl>	s_MngAniCtrl; // Animation Manager
	static CN3Mng<class CN3FXPMesh>		s_MngFXPMesh; // FX���� ���� PMesh - ������ �Ϲ� PMesh�� ������ ���� �ٸ���.
	static CN3Mng<class CN3FXShape>		s_MngFXShape; // FX���� ���� Shape - ������ �Ϲ� shape�� ������ ���� �ٸ���.
	

protected:
	static std::string 				s_szPath; // ���α׷��� ����� ���.. 

protected:
	DWORD							m_dwType; // "MESH", "CAMERA", "SCENE", "???" .... ����...

public:
	std::string 					m_szName;

public:
	static float		TimeGet();
	static const std::string& 	PathGet() { return s_szPath; }
	static void			PathSet(const std::string& szPath);
	
	static void			RenderLines(const __Vector3 *pvLines, int nCount, D3DCOLOR color);
	static void			RenderLines(const RECT& rc, D3DCOLOR color); 
	
	static float		TimerProcess( TIMER_COMMAND command );

	DWORD				Type() { return m_dwType; } // ��ü ����..

	void				ReleaseResrc();
//#ifdef _N3TOOL
	void				SaveResrc();
//#endif // end of _N3TOOL
	virtual void Release();
	CN3Base();
	virtual ~CN3Base();
};

#endif // !defined(AFX_N3Base_h__INCLUDED_)
