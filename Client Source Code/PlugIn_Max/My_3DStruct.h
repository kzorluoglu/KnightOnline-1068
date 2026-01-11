#include "D3DX8.h"

struct __EXPORT_HEADER
{
	char szID[8];		// ID "N3Scene1"
	char szRemark[64];	// ����..
	
	int nNodeCount; // ��ü ��� ī��Ʈ
	int nStaticFrame; // ������� ��������..
	BOOL bExportCurrentFrame; // ��ü ���ϸ��̼��� 
	int nFrmStart;	// ���ϸ��̼� ���� ������ 
	int nFrmEnd;		// ���ϸ��̼� �� ������

	D3DCOLORVALUE dcvBackground;	// ��� ��
	D3DCOLORVALUE dcvAmbientLight;	// �⺻ ���� ��

	int nCameraCount;	// scene ���� ī�޶� ����
	int nMaterialCount;	// scene ���� ���� ����
	int nTextureCount;	// scene ���� �ؽ�ó ����
	int nLightCount;	// scene ���� ���� ����

	BOOL bObjGeometry;	// ������Ʈ�� �����͸� ���� �ִ�.
	BOOL bObjShape;		// 2D ������ �����͸� ���� �ִ�.
	BOOL bObjCamera;	// ī�޶� �����͸� ���� �ִ�.
	BOOL bObjLight;		// ����Ʈ �����͸� ���� �ִ�.
	BOOL bObjDummy;		// ����� ������Ʈ(??? - ���� �׿����� �ʿ���� ���߽ÿ��� �ʿ��� ������Ʈ�� ���Ѵ�)�����͸� ���� �ִ�. 

	BOOL bMesh;						// ������ �����͸� ���� �ִ�.
	BOOL bMaterial;					// ���� �����͸� ���� �ִ�.
	
	BOOL bAnimationKey;				// ���ϸ��̼� Ű�� ���� �ִ�.
	BOOL bMeshAnimation;			// ������ ���ϸ��̼� Ű�� ���� �ִ�.
	BOOL bCameraAndLightAnimation;	// ī�޶�, ���� ���ϸ��̼��� ���� �ִ�.
	BOOL bInverseKinematicsJoint;	// IK �������� ���� �ִ�..

	BOOL bGenerateTexture; // �ؽ�ó ������ �ڵ����� ����ȭ ���Ѽ� ���� Direct3D �� ���˿� �°� 2�� ������ ������ ���߾ "OBM" ��Ʈ�� ���Ϸ� ����.
	BOOL bGenerateIndexedVertex; // ���ε� ������ ���ڵ��� ����.. �޸𸮿� ���� ȿ�����̴�..
	BOOL bGenerateSmoothNormal; // �ε巴�� ���̵��� ���� ���͵��� �� ����Ѵ�.
};

struct __UVWH
{
	float	fV;
	float	fU;
	float	fW;
	float	fH;

	float	fUOffset;
	float	fVOffset;
	float	fUTile;
	float	fVTile;
};

