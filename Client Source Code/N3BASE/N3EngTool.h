// N3EngTool.h: interface for the CN3EngTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_D3DENGINETEST_H__4DE7DD27_A9BC_43C5_9D67_E99031ED38B5__INCLUDED_)
#define AFX_D3DENGINETEST_H__4DE7DD27_A9BC_43C5_9D67_E99031ED38B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../N3Base/N3Eng.h"

struct __EXPORT_OPTION
{
	char szID[8];		// ID "N3Scene1"
	char szRemark[64];	// ����..
	
	int nNodeCount; // ��ü ��� ī��Ʈ
	
	BOOL	bAnimationKey; 
	int		nFrmStart;	// ���ϸ��̼� ���� ������ 
	int		nFrmEnd;	// ���ϸ��̼� �� ������
	float	fSamplingRate; // Key Sampling - 30.0f �� ǥ��..

	D3DCOLORVALUE dcvBackground;	// ��� ��
	D3DCOLORVALUE dcvAmbientLight;	// �⺻ ���� ��

//	int nCameraCount;	// scene ���� ī�޶� ����
//	int nMaterialCount;	// scene ���� ���� ����
//	int nTextureCount;	// scene ���� �ؽ�ó ����
//	int nLightCount;	// scene ���� ���� ����

	BOOL bExportCamera;	// ī�޶� �����͸� ���� �ִ�.
	BOOL bExportLight;		// ����Ʈ �����͸� ���� �ִ�.
	BOOL bExportGeometry;	// ������Ʈ�� �����͸� ���� �ִ�.
	BOOL bExportDummy;	// ����� ������Ʈ(??? - ���� �׿����� �ʿ���� ���߽ÿ��� �ʿ��� ������Ʈ�� ���Ѵ�)�����͸� ���� �ִ�. 
	BOOL bExportCharacter;	// ����� ������Ʈ(??? - ���� �׿����� �ʿ���� ���߽ÿ��� �ʿ��� ������Ʈ�� ���Ѵ�)�����͸� ���� �ִ�. 
	
	BOOL bExportSelectedOnly; // ���õ� �͸� ???

	BOOL bGenerateFileName; // ���� �̸��� 0_0000_00_0 �������� �ٲ۴�..??
	BOOL bGenerateSmoothNormal; // �ε巴�� ���̵��� ���� ���͵��� �� ����Ѵ�.
//	BOOL bGenerateProgressiveMesh; // Progressive Mesh ����
	BOOL bGenerateHalfSizeTexture; // �ؽ�ó ������ �ڵ����� ����ȭ ���Ѽ� ���� Direct3D �� ���˿� �°� 2�� ������ ������ ���߾ "OBM" ��Ʈ�� ���Ϸ� ����.
	BOOL bGenerateCompressedTexture; // Texture ���� ���

	char szSubDir[_MAX_DIR];		// export �Ҷ� �����ϴ� sub������ �������ش�.
};

class CN3EngTool : public CN3Eng  
{
public:
	int					m_nGridLineCount; // �׸��� ���� ī��Ʈ..
	__VertexColor*		m_pVGrids; // �׸��� ������ ��

	__VertexColor		m_VAxis[60]; // �� ������ ��
	__VertexColor		m_VDir[6]; // ���� ǥ�� ��
	__VertexTransformed m_VPreview[6];	// �ؽ�ó ������ ��



//	LPDIRECT3DDEVICE8	m_lpD3DDevExtra;

public:
//	bool CreateExtraDevice(int nWidth, int nHeight);
	void GridCreate(int nWidth, int nHeight);
	void RenderTexturePreview(CN3Texture* pTex, HWND hDlgWndDiffuse, RECT* pRCSrc = NULL);
	void RenderGrid(const __Matrix44& mtxWorld);
	void RenderAxis(bool bShowDir = false);

	CN3EngTool();
	virtual ~CN3EngTool();

};

#endif // !defined(AFX_D3DENGINETEST_H__4DE7DD27_A9BC_43C5_9D67_E99031ED38B5__INCLUDED_)
