// N3Base.h: interface for the CN3Base class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3Base_h__INCLUDED_)
#define AFX_N3Base_h__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "My_3DStruct.h"
#include "N3Mng.h"
#include <stdio.h>
#include <mmsystem.h>

class CN3Texture;
class CN3Mesh;
class CN3VMesh;
class CN3PMesh;
class CN3Joint;
class CN3CPart;
class CN3AnimControl;

const DWORD TEX_CAPS_DXT1 =			0x00000001;
const DWORD TEX_CAPS_DXT2 =			0x00000002;
const DWORD TEX_CAPS_DXT3 =			0x00000004;
const DWORD TEX_CAPS_DXT4 =			0x00000008;
const DWORD TEX_CAPS_DXT5 =			0x00000010;
const DWORD TEX_CAPS_SQUAREONLY =	0x00000020;
const DWORD TEX_CAPS_MIPMAP =		0x00000040;
const DWORD TEX_CAPS_POW2 =			0x00000080;

const int MAX_CAMERA_RADIUS = 64;

class CN3Base  
{
public:
	static CN3Mng<CN3Texture>	s_MngTex; // Texture Manager
	static CN3Mng<CN3Mesh>		s_MngMesh; // Normal Mesh Manager
	static CN3Mng<CN3VMesh>		s_MngVMesh; // �ܼ��� �����︸ ���� �ִ� �޽� - �ַ� �浹 üũ�� ����..
	static CN3Mng<CN3PMesh>		s_MngPMesh; // Progressive Mesh Manager
	static CN3Mng<CN3Joint>		s_MngJoint; // Joint Manager
	static CN3Mng<CN3CPart>		s_MngCPart; // Character �� ���� Part Manager
	static CN3Mng<CN3AnimControl>	CN3Base::s_MngAniCtrl; // Animation Manager

	typedef struct __CameraData
	{
		__Vector3 vEye; // Camera Position Vector
		__Vector3 vAt; // Camera At Vector
		__Vector3 vUp; // Camera Up Vector
		float fEyeDotValues[MAX_CAMERA_RADIUS][5];	// Camera Eye Vector - Camera Direction Vector �� Dot Porduct �Ѱ�.. 
													// ù��° �迭[MAX_CAMERA_RADIUS]- 3 meter ������ MAX_CAMERA_RADIUS��, �� ī�޶� �ڷ� MAX_CAMERA_RADIUS * 10 meter ���� �� �Ÿ��� Ŀ���Ҽ� �ִٴ� �ǹ��̴�.
													// �ι�° �迭[5] - ī�޶� ��ġ. 1~4:Left, Right, Top, Bottom
		
		__Vector3 vFrustumNormals[5]; // ī�޶� ����ü ����� ���� ����.. 0:Camera ��ġ 1~4:Left, Right, Top, Bottom , �� ����� ��ġ�� vPos �̴�..
		__Vector3 vFarPoints[4]; // LeftTop, RightTop, BottomRight, BottomLeft

		float fFOV; // ī�޶� ���� �� : Field Of View
		float fAspect; // ��Ⱦ��
		float fNP; // NearPlane
		float fFP; // FarPlane
		D3DVIEWPORT9 vp; // ViewPort;
		__Matrix44 mtxView;
		__Matrix44 mtxViewInverse;
		__Matrix44 mtxProjection;

		BOOL IsOutOfFrustum(__Vector3& vPosition, float fRadius) // �̸� ���� ī�޶� ����� ��Ʈ ���δ�Ʈ ���� �������� ī�޶� ���ü �ۿ� ������  ���� �����ش�.
		{
			int nEyeDotIndex = (int)(fRadius / 3.0f); // 3 Meter ������ �̸� ����� ���´�..
			if(nEyeDotIndex < 0) nEyeDotIndex = 0;
			else if(nEyeDotIndex >= MAX_CAMERA_RADIUS) nEyeDotIndex = MAX_CAMERA_RADIUS - 1;
			if(	vPosition.Dot(vFrustumNormals[0]) < fEyeDotValues[nEyeDotIndex][0] || // Front...
				vPosition.Dot(vFrustumNormals[1]) < fEyeDotValues[nEyeDotIndex][1] || // Left
				vPosition.Dot(vFrustumNormals[2]) < fEyeDotValues[nEyeDotIndex][2] || // Right
				vPosition.Dot(vFrustumNormals[3]) < fEyeDotValues[nEyeDotIndex][3] || // Top
				vPosition.Dot(vFrustumNormals[4]) < fEyeDotValues[nEyeDotIndex][4] ) // Bottom
			{
				return TRUE;
			}
			
			return FALSE;
		}
	} __CameraData;

	typedef struct __RenderInfo
	{
		int nShape;
		int nShapePart;
		int nChr;
		int nChrPart;

		int nPolygon; // �ܼ� ������
		int nPolygonCharacter; // ĳ���� ������
		
		int nTexture_32X32; // 32 X 32 Texture
		int nTexture_64X64; // 64 X 64 Texture
		int nTexture_128X128; // 128 X 128 Texture
		int nTexture_256X256; // 256 X 256 Texture
		int nTexture_512X512; // 512 X 512 Texture
		int nTexture_Huge; // 512 X 512 �̻� size
		int nTexture_OtherSize; // Other size
	} __RenderInfo;

	typedef struct __ResrcInfo
	{
		int nTexture_Loaded_32X32; // 32 X 32 Texture
		int nTexture_Loaded_64X64; // 64 X 64 Texture
		int nTexture_Loaded_128X128; // 128 X 128 Texture
		int nTexture_Loaded_256X256; // 256 X 256 Texture
		int nTexture_Loaded_512X512; // 512 X 512 Texture
		int nTexture_Loaded_Huge; // 512 X 512 �̻� size
		int nTexture_Loaded_OtherSize; // Other size
	} __ResrcInfo;


	static LPDIRECT3DDEVICE8		s_lpD3DDev; // Device ���� ������.. �ڴ�� �����ϸ� �ȵȴ�..
	static D3DPRESENT_PARAMETERS	s_DevParam; // Device ���� Present Parameter
	static D3DCAPS8	s_DevCaps; // Device ȣȯ��...
	static DWORD	s_dwTextureCaps; // Texture ����.. DXT1 ~ DXT5, Square Only
	static HWND		s_hWndDevice; // Device Window Handle
	static HWND		s_hWndBase; // Init �Ҷ� �� Window Handle
	static HWND		s_hWndPresent; // �ֱٿ� Present �� Window Handle

	static __CameraData	s_CameraData; // ī�޶� ������ ���� ����..
	static __RenderInfo	s_RenderInfo; // Rendering Information..
	static __ResrcInfo	s_ResrcInfo; // Rendering Information..
	static float		s_fFrmPerSec; // Frame Per Second

protected:
	static char				s_szPath[512]; // ���α׷��� ����� ���.. 
	static LARGE_INTEGER	s_TimePrev; // �ð�..

protected:
	DWORD	m_dwType; // "MESH", "CAMERA", "SCENE", "???" .... ����...
	char*	m_szName;
	char*	m_szFileName; // Base Path �� ������ ���� ��� + ���� �̸�

public:
	float TimeGet();
	static const char* PathGet() { return s_szPath; }
	static void PathSet(const char* szPath);
	
	void ReleaseResrc();
	void SaveResrc();
	
	void TimerProcess();

	const char* Name() { return m_szName; }
	void NameSet(const char* szName);

	const char* FileNameShort(); // ���̽� ��θ� ������ ª�� �̸��� �����ش�..(���� ����̴�..)
	const char* FileName() { return m_szFileName; } // Full Path
	void FileNameSet(const char* szFileName);

	DWORD Type() { return m_dwType; } // ��ü ����..

	bool LoadFromFile(); // ���Ͽ��� �о����.
	bool LoadFromFile(const char* szFileName); // ���Ͽ��� �о����.
	bool SaveToFile(); // ���� ���� �̸���� ����.
	bool SaveToFile(const char* szFileName); // ���̸����� ����.

	virtual bool Load(HANDLE hFile); // �ڵ鿡�� �о����..
	virtual bool Save(HANDLE hFile); // �ڵ��� ���� ����..

	void RenderLines(const __Vector3 *pvLines, int nCount, D3DCOLOR color);

	virtual void Release();
	CN3Base();
	virtual ~CN3Base();
};

#endif // !defined(AFX_N3Base_h__INCLUDED_)
