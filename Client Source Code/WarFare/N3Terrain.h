//	N3Terrain.h: interface for the CLyTerrain class.
//	2001. 10. 22.
//
//////////////////////////////////////////////////////////////////////

#ifndef __N3TERRAIN_H__
#define __N3TERRAIN_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////
//	�����տ� ti_�� ���� �� Ÿ�ϴ���..
//	�����տ� pat_�� ���� �� ��ġ����..
//	�����տ� re_�� ���� �� ���ʹ���..
//

#include "../N3Base/N3BaseFileAccess.h"
#include "../N3Base/N3Texture.h"
#include "N3TerrainDef.h"
#include "N3TerrainPatch.h"

#include <string>

//class CN3Terrain : public CGameBase
class CN3Terrain : public CN3BaseFileAccess
{
public:
	//common..
	__Material			m_Material;
	D3DSHADEMODE		m_ShadeMode;
	D3DFILLMODE			m_FillMode;
	int					m_iLodLevel;
			
	//Patch...
	CN3TerrainPatch**	m_ppPatch;
	POINT				m_pat_LBPos;
	POINT				m_pat_PrevLBPos;
	RECT				m_pat_BoundRect;	//LBPos�� ���� �����ǥ..
	int					m_iNumPatch;
	int					m_pat_Center2Side;	//�߽������� ���ʺ������� ����..
	int					m_iDistanceTable[DISTANCE_TABLE_SIZE][DISTANCE_TABLE_SIZE];
	
	//MapInfo..
	MAPDATA*			m_pMapData;
	int					m_ti_MapSize;				// ���� � ����.. 4096Meter -> 1024 + 1
	int					m_pat_MapSize;				// ��ġ ����.. ����� ���� Ʋ����..
	__Vector3*			m_pNormal;

	//LightMap Info..
	POINT				m_pat_CenterPos;
	stlMap_N3Tex		m_LightMapPatch[3][3];
	
	//bool**			m_ppIsLightMap;
	//CN3Texture***		m_pppLightMapTex;

	//Patch
	float**				m_ppPatchRadius;
	float**				m_ppPatchMiddleY;

	//Tile..
	POINT				m_ti_CenterPos;
	POINT				m_ti_PrevCenterPos;

	//Texture...
	int					m_NumTileTex;				// Tile Texture ����
	CN3Texture*			m_pTileTex;

	//ColorMap..
	CN3Texture**		m_ppColorMapTex;
	int					m_iNumColorMap;				// �÷����� ���� ����Ǿ� �ִ�.. ���� = 

	//�÷������� �� ����� ���̸�..
	CN3Texture			m_pBaseTex;

	//Ÿ�Ϲ���..
	float m_fTileDirU[8][4];
	float m_fTileDirV[8][4];

	//Grass Attr;
	char				m_pGrassTextureName[MAX_GRASS][MAX_PATH];
	char				m_pGrassFileName[MAX_PATH];
	unsigned char*		m_pGrassAttr;
	unsigned char*		m_pGrassNum;
	int					m_iNumGrass;

	class CN3River*		m_pRiver;
	class CN3Pond*		m_pPond;

	bool				m_bAvailableTile;

protected:
	void	SetNormals();
	void	SetLightMap(int dir);
	void	ReplaceLightMapPatch(int x, int z, stlMap_N3Tex& LightMapPatch);
	void	SetLightMapPatch(int x, int z, HANDLE hFile, int* pAddr);

	
	void	TestAvailableTile();
	void	MakeDistanceTable();

	inline	int	Log2(int x);	//2�� �¼� ����....
	int Real2Tile(float x){ return ((int)x / TILE_SIZE); } // ����ǥ -> Ÿ����ǥ...(������ǥ)
	int Tile2Patch(int x) { return (x / PATCH_TILE_SIZE); } // Ÿ����ǥ -> ��ġ��ǥ...(������ǥ��)
	int Real2Patch(float fX) { return ( ((int)fX / TILE_SIZE) / PATCH_TILE_SIZE ); } // ����ǥ -> ��ġ��ǥ..(������ǥ��)
		
	void	LoadTileInfo(HANDLE hFile);
	bool	CheckRenderablePatch();
	bool	CheckMovePatch();
	bool	CheckBound();
	void	DispositionPatch();
	void	SetBlunt();
	void	LoadGrassInfo();
	
public:
	CN3Texture* GetLightMap(int tx, int tz);
	float	GetWidthByMeter() { return (float)(m_ti_MapSize - 1) * TILE_SIZE; }
	int		GetLODLevel() { return m_iLodLevel; }
	bool	SetLODLevel(int level);
	float	GetHeight(float x, float z);
	void	Release();
	void	Init();
	bool	Load(HANDLE hFile);
	void	Tick();
	void	Render();
	void	SetFillMode(D3DFILLMODE FillMode) { m_FillMode = FillMode; }
	void	SetShadeMode(D3DSHADEMODE ShadeMode) {m_ShadeMode = ShadeMode; }
		
	CN3Terrain();
	virtual ~CN3Terrain();

public:	//additional........
	bool			GetTileTexInfo(float x, float z, TERRAINTILETEXINFO& TexInfo1, TERRAINTILETEXINFO& TexInfo2);
	CN3Texture*		GetTileTex(int x, int z);
	MAPDATA			GetMapData(int x, int z);
	unsigned short	GetGrassAttr(int x, int z);
	bool			LoadColorMap(const std::string& szFN);
	void			GetNormal(float x, float z, __Vector3& vNormal);
	bool			IsInTerrain(float x, float z);
	//..
	BOOL			Pick(int x, int y, __Vector3& vPick);
	BOOL			PickWide(int x, int y, __Vector3& vPick);
	void			CalcCollisionTerrainByOTPlayer(__Vector3, __Vector3, __Vector3& );

	bool			CheckIncline(const __Vector3& vPos, const __Vector3& vDir, float fIncline); // ���� ��ġ�� ���⿡���� ��簪�� �μ��� ���°ͺ��� ũ��(���ö� ���̸�) true, ������ false
	bool			CheckCollisionCamera(__Vector3& vEye, const __Vector3& vAt, float fNP); // vEye �� ���� ���� ���´�.. ī�޶� Near Plane�� ������ ���.
	BOOL			CheckCollisionByHeight(const __Vector3& vPos, float fUnitSize, float& fHeight)
	{
		fHeight = this->GetHeight(vPos.x, vPos.z);
		if(vPos.y < fHeight + fUnitSize) return TRUE; // ���� ���̰� �������� + ũ�� ���� �۴ٸ� 
		return FALSE;
	}
	bool			CheckCollision(__Vector3& vPos, __Vector3& vDir, float fVelocity, __Vector3* vpCol);
	
};

#endif // !defined(__N3TERRAIN_H__)
