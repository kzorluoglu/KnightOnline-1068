////////////////////////////////////////////////////////////////////////////////////////
//
//	LyTerrainDef.h
//	- �̰����� Terrain�� ���õ� �ڷ�������, �������...
//	- ����Ʈ�� �̿��� ����...^^
//
//	By Donghoon..
//
////////////////////////////////////////////////////////////////////////////////////////

#ifndef __LYTERRAINDEF_H__
#define __LYTERRAINDEF_H__


// common include header...
#include "..\N3BASE\N3Base.h"


// Constant Definitions..
const int N3ME_DATA_VERSION = 3;
//version info...
//���� �����͵��� ���� ������ ���� ���� �ʴ�.
//1 : �ʱ� �����Ϳ� lightmap������ ���� ��..
//2 : version 1�� Ǯ ����(Ǯ�� DTex�� �׷���� ���迡 ���� ����)�� ���� ��.
//3 : ������ lightmap..^^

const int N3ME_DTEX_DATA_VERSION = 2;
//
//1 : dTex�� ������ �� �ؽ����� �׷� ������ ���ؽ����̸�.dif���Ϸ� ���� �д´�..
//2 : ���ؽ����� �׷������� dtexinfo.dtx���Ϸκ��� �д´�.. 

const int TERRAIN_CELL_SIZE = 4;
//const int MAX_COLORMAPTEXTURE = 32;		//	�ִ� #colormaptexture at one side = 4096/128..
//���� m_iNumColorMap���� ��ü�ؼ� ���� �ִ�...
const int PATCH_TILE_SIZE = 8;		//	��ġ �ϳ��� Ÿ���� 8x8�� ����.

const int MAX_COLORMAPVB_SIZE = 1026;	//�÷������� ��� VB�� �ִ� ������.
const int MAX_TILEVB_SIZE = 16;	//Ÿ����� VB�� �ִ� ������.
const int MAX_LIGHTMAPVB_SIZE = 16;		//����Ʈ�� ���� VB�� �ִ� ������.

const int DTEX_SIZE = 1024; // Detail Texture Size
const int NUM_DTEXTILE = 8; // Detail Texture ���� ��..

const int LIGHTMAP_TEX_SIZE = 8;//32;

const int DISTANCE_TABLE_SIZE = 64;

const int NOTMOVE_HEIGHT = 10;	// �������� Ŀ�� �� �� ���� Ÿ���� �ּҰ�..
								// 10���ϸ� ������ �� �ְ�, �̻��̸� �� �� ����.
 

const int MAX_BRUSH_SIZE	= 21;
const int BRUSH_CENTER	 = (MAX_BRUSH_SIZE/2);
enum {BRUSHSHAPE_CIRCLE = 1, BRUSHSHAPE_SQUARE = 2};
enum {TEM_NOT=0, TEM_HEIGHTEN=1, TEM_SMOOTH=2, TEM_DTEX=3, TEM_LIGHTMAP=4, TEM_MAKENPCPATH=5, TEM_FLAT=6 ,TEM_BRUSH_SHOW = 7};

enum {DTEX_FULL=0, DTEX_1PER2, DTEX_1PER4, DTEX_3PER4, DTEX_1PER8, DTEX_7PER8, DTEX_5PER8, DTEX_3PER8, DTEX_MAX=8};


//macro..
#define DISTANCE3D(a,b) (sqrt( ((a).x-(b).x)*((a).x-(b).x) + ((a).y-(b).y)*((a).y-(b).y) + ((a).z-(b).z)*((a).z-(b).z)))
#define DISTANCE2D(a,b) (sqrt( ((a).x-(b).x)*((a).x-(b).x) + ((a).z-(b).z)*((a).z-(b).z)))


// Structure Definitions..

//�ʿ�����..
typedef struct __DTexAttr
{
	int Group;
	int Attr;

	__DTexAttr()
	{
		Group = 0;
		Attr = 0;
	}
} DTEXATTR, *LPDTEXATTR;

typedef struct __DTexTileAttr
{
	int TexID;
	int TileX;
	int TileY;
	
	__DTexTileAttr()
	{
		TexID = -1;
		TileX = 0;
		TileY = 0;
	}
} DTEXTILEATTR, *LPDTEXTILEATTR;

typedef struct __DTexInfo
{
	int				Dir;
	DTEXTILEATTR	TexIdx;
	DTEXATTR		Attr;

	__DTexInfo()
	{
		Dir = 0;		
	}
} DTEXINFO, *LPDTEXINFO;

typedef struct __MapData
{
	float	fHeight;	//������ ���̰�..
	//float	u1,v1;
	//__Vector3 Normal;
	DTEXINFO DTexInfo1;
	DTEXINFO DTexInfo2;

	__MapData()
	{
		fHeight = -FLT_MAX;
	}
} MAPDATA, *LPMAPDATA;

typedef struct __GameMapData
{
	float	fHeight;		//������ ���̰�..
	DWORD	bIsTileFull		:1;	//Ÿ���� Full �̸� true �ƴϸ� false..
	DWORD	Tex1Dir			:5;		//������ ����ϴ� �ε����� �ƴ�..
	DWORD	Tex2Dir			:5;	
	DWORD	Tex1Idx			:10;		//������ ����ϴ� �ε����� �ƴ�..
	DWORD	Tex2Idx			:10;
	
	__GameMapData()
	{
		bIsTileFull = true;
		fHeight = -FLT_MAX;
		Tex1Idx = 0;
		Tex1Dir = 0;
		Tex2Idx = 0;
		Tex2Dir = 0;
	}

} GAMEMAPDATA, *LPGAMEMAPDATA;

typedef struct __SeedGroup
{
	unsigned char Obj_Id        : 4;
	unsigned char Seed_Count    : 3;
	unsigned char sub_flage		: 1;

	__SeedGroup* SeedGroup_Sub;

	__SeedGroup()
	{
		Obj_Id = 0;
		Seed_Count = 0;
		SeedGroup_Sub = NULL;
	}
} SEEDGROUP, *LPSEEDGROUP;

#endif	//end of #ifndef __LYTERRAINDEF_H__
