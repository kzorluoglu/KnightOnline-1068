// LyTerrain.h: interface for the CLyTerrain class.
//	- 2001. 9.24.~ Map Editor������ ��ȯ...
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3TERRAIN_H__7BCDE4B8_8FBC_432E_B4CB_4A56C5FC29F0__INCLUDED_)
#define AFX_N3TERRAIN_H__7BCDE4B8_8FBC_432E_B4CB_4A56C5FC29F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <list>

#include "LyTerrainDef.h"
#include "../N3BASE/N3BaseFileAccess.h"

class CN3Texture;
class CQTNode;

class CDTexMng;
class CDTexGroup;
class CDTexGroupMng;
class CDlgSetLightMap;

////////////////////////////////////////////////
//	multimap���	
//savegamedata�Ҷ� ������..
//colormap �������� ��..^^
//
typedef std::multimap<short, short> MMap;
typedef MMap::value_type MMValue;
typedef MMap::iterator MMIter;

typedef std::list<short> LList;
typedef LList::iterator LLIter;

typedef std::list<CQTNode*>::iterator it_QTNode;

class CLyTerrain : public CN3BaseFileAccess  
{
protected:
	__Material					m_Material;
	D3DFILLMODE					m_FillMode;

	//quadtree..
	CQTNode*					m_pRoot;
	CQTNode*					m_pRefCurrNode;

	//���� edit����...
	__VertexXyzColor			m_vBrushVertices[MAX_BRUSH_SIZE*MAX_BRUSH_SIZE*9];		// Brush�� ����� ��Ÿ���� vertex buffer
	WORD						m_wBrushIndices[MAX_BRUSH_SIZE*MAX_BRUSH_SIZE*6*4];		// Brush�� ����� ��Ÿ���� index buffer
	int							m_iBrushIndexCount;										// Brush�� ����� ��Ÿ���� index�� ����
	int							m_iEditMode;												// Edit�� ����ΰ�?

	CDTexMng*					m_pDTexMng;

	MMap						m_ColorMapTileTree;	//�÷��� ������� Ÿ�ϵ� ���� �ִ� Ʈ��..

	int							m_iDistanceTable[DISTANCE_TABLE_SIZE][DISTANCE_TABLE_SIZE];
	
public:
	float						m_fBrushIntensityMap[MAX_BRUSH_SIZE][MAX_BRUSH_SIZE];	// Brush�� ��ȭ���� ǥ���� ��
	float						m_fHeightScale;
	float						m_fFlatHeight;
	bool						m_bFlaten;


	int							m_iZoneID;

	//LightMap����...
	CDlgSetLightMap*			m_pDlgSetLightMap;
	LPDIRECT3DVERTEXBUFFER9		m_LightMapVB;	
	CN3Texture***				m_ppLightMapTexture;
	bool**						m_ppIsLightMap;
	int							m_iNumLightMap;
	bool						m_bDrawLineLightMap;	//����Ʈ �� �׸��� �ɼǿ���.. �������·� �׸�����..
	__VertexXyzColor			m_vLineLightMap[2];		//�������·� �׸��ٸ� �� ������ ȭ�鿡 ǥ���ؾߵǰ���?..^^ �����̾�..

	//LOD����..
	int							m_iHeightLimit;
	int							m_iDistLimit;

	std::list<CQTNode*>			m_RenderNodes;

	LPDIRECT3DVERTEXBUFFER9		m_ColorMapVB;	//	�÷��ʾ���..
	LPDIRECT3DVERTEXBUFFER9		m_TileVB;	//	�ϳ��� Ÿ�ϰ� �ϳ��� ����Ʈ��
	
	//common..
	MAPDATA**		m_ppMapData;
	BOOL**			m_ppRenderInfo;

	//texture..	
	CN3Texture**	m_pColorTexture;
		
	RECT			m_VisibleRect;	//Ÿ�ϴ����� ���ÿ���..
	POINT			m_EyePos;		//Ÿ�ϴ���..

	int				m_iHeightMapSize; // Cell Count �� ����� ���� -> 1 �� ���־�� Cell Count �̴�.
	int				m_iMaxLevel;

	int				m_iNumTileMap;	//���� ������ �ִ� Ÿ�ϸ��� ��..

	int				m_iColorMapTexSize;
	int				m_iColorMapPixelPerUnitDistance;
	int				m_iNumColorMap;

	//Ÿ����� �� ����� ����...
	float			m_fTileDirU[8][4];
	float			m_fTileDirV[8][4];
	float			m_fTileDirUforColorMap[8][4];
	float			m_fTileDirVforColorMap[8][4];

	// DTex����...
	LPDIRECT3DVERTEXBUFFER9		m_pDTexVB;
	__VertexT1*					m_pDTexVertices;
	int							m_DTexFaceCnt;
	int							m_DTexBuff[5000];
	int							m_DTexAttrTable[9][14];
	int							m_DTexDirTable[9][14];

	// ColorMap ����...
	LPDIRECT3DVERTEXBUFFER9		m_pColorMapTmpVB;
	__VertexTransformedT2*		m_pColorMapTmpVertices;

	//Brush ����..
	BOOL						m_bBrushFlat;	//���� ���̰� ������ �귯���� �����ϰ� �Ҳ���?

protected:
	int DetectRealLightMap(int sx, int sz, int range);
	void MakeDistanceTable();
	void FillLightMap(int x, int z, int lcx, int lcz, DWORD Color, float alpha);
	void SetLightMap(int x, int z, int lcx, int lcz);

	void PutColorMapTile(int x, int z);		//�÷��� ���� Ÿ�� �ֱ�...(���� �ȸ������...�׷� Ÿ�ϵ� ��⿭�� �ִ°�..)
	void VerifyTexInfo(LPDTEXINFO pTex);
	void SetColorMap(int x, int y);
	void SetPertinentTile(CDTexGroupMng* pDTexGroupMng, LPDTEXINFO pDest1, LPDTEXINFO pDest2); 

	inline	int	Log2(int x);
	inline	void ClearRenderInfo();
	void	SetVisibleRect();	//Ÿ�ϴ���..

	//map editor ����..
	//
	void SetDTexManual(int x, int y);	//�������
	void SetDTex(int x, int y);			//�ڵ����.
	BOOL IntersectTriangle( const D3DXVECTOR3 orig, const D3DXVECTOR3 dir, 
						D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2, float* t, float* u, float* v);
	void Heighten(POINT ptCenter, float fHeight);	// ������ �����ϰų� ���� �ϴ� �Լ�
	void Smooth(POINT ptCenter);		// ������ �ֺ� ���� ���̰��� ���ؼ� �ε巴�� �Ѵ�.
	void Flaten(POINT ptCenter);
	void SetApexHeight(int x, int z, float fHeight);		// ���̸��� ���̰� ����
	float GetApexHeight(int x, int z) const;				// ���̸��� ���̰� ���
	float GetFallOffValue(float fFallOff, float x);		// BrushIntensity ����Ҷ� ���̴� ��.

	//������ ����Ʈ�ʰ���..(2002. 11. 13..)
	void ConvertLightMapToolDataV2toV3();
	//
			
public:
	void UpdateBrushArea(POINT ptCenter);				// Brush ���� ǥ�� vertex����
	void RenderBrushArea();								// Brush ���� ǥ�� �׸���

	void	ExportHeightBMP(const char* szPathName);
	void	ImportHeightBMP(const char* szPathName);

	void	MakeMoveTable(short** ppEvent);
	void	Init(int HeightMapSize = 5);
	bool	LoadFromFile(const char* lpszPath);
	bool	SaveToFile(const char* lpszPath);
	bool	SaveToFilePartition(const char* lpszPath, float psx, float psz, float width);
	void	Tick();
	void	Render();
	void	Release();

	void	MakeGameLightMap(char* szFullPathName);
	void	MakeGameColorMap(char* szFullPathName);
	void	GeneraterColorMap(bool bIsAll = false);
	void	TilingAll(); //���� ��ü�� ���õ� Ÿ�Ϸ� ���..

	void	UpDateDistLimit();

	CN3Texture* GetTileTex(int id);
	void	SetFillMode(D3DFILLMODE fillmode);
	float	GetHeight(float x, float z);

	//map editor ����..
	//
	SIZE GetPatchNum(float fSize);
	BOOL MouseMsgFilter(LPMSG pMsg);	// ���� ��ĥ�� ���콺 �޼��� ó��
	bool Pick(int x, int y, __Vector3* vec, POINT* pHeightMapPos = NULL);
	void Import(LPCTSTR pFileName, float fSize);
	void ImportHeight(LPCTSTR pFileName);
	void SaveServerData(HANDLE hFile);
	void SetEditMode(int iEditMode);		// ���� Edit���� ����
	void UpdateBrushIntensityMap(int iShape, int iSize, float fFallOff);	// �귯���� ���� ����� ���� IntensityMap�� �ٽ� �����Ѵ�.
	void SaveGameData(HANDLE hFile);
	void ColorMapImport(LPCTSTR lpszPathName);
	void ColorMapExport(LPCTSTR lpszPathName);
	void GenerateMiniMap(LPCTSTR lpszPathName, int size);
	//
		
	CLyTerrain();
	virtual ~CLyTerrain();
};

#endif // !defined(AFX_N3TERRAIN_H__7BCDE4B8_8FBC_432E_B4CB_4A56C5FC29F0__INCLUDED_)

