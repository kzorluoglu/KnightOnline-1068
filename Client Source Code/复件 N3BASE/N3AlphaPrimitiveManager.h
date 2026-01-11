// N3AlphaPrimitiveManager.h: interface for the CN3AlphaPrimitiveManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3ALPHAPRIMITIVEMANAGER_H__616E318C_49E4_4468_9719_E62E384FC48D__INCLUDED_)
#define AFX_N3ALPHAPRIMITIVEMANAGER_H__616E318C_49E4_4468_9719_E62E384FC48D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "My_3DStruct.h"

//const DWORD RF_NOTHING = 0;
//const DWORD RF_ALPHABLENDING = 1;
//const DWORD RF_NOTUSEFOG = 2;
//const DWORD RF_DOUBLESIDED = 4;

struct __AlphaPrimitive
{
	float				fCameraDistance;	// ī�޶���� �Ÿ�..
	DWORD				dwBlendSrc;			
	DWORD				dwBlendDest;
	int 				nRenderFlags;		// ������ �÷���.. RF_... ����..
	LPDIRECT3DTEXTURE8	lpTex;				// Texture pointer
	DWORD				dwFVF;				// flexible vertex format
	D3DPRIMITIVETYPE	ePrimitiveType;	// Primitive Type 
	int					nPrimitiveCount;	// PrimitiveCount
	DWORD				dwPrimitiveSize;	// Primitive Size .. stream 0 stride
	BOOL				bUseVB;				// ���ؽ� ����, �ε��� ���۸� ����� ���ΰ� �ƴѰ�
	const void*			pwIndices;			// ���� Index ����̸�... NULL �� �ƴѰ��� ������ �ȴ�.
	int					nVertexCount;
	const void*			pVertices;			// �ﰢ��.. ���� ��������.. ���� ����ȯ�� ���� �پ��� �������� �������� ��� �Ѵ�..
	__Matrix44			MtxWorld;			// Matrix
};

const int MAX_ALPHAPRIMITIVE_BUFFER = 1024;

class CN3AlphaPrimitiveManager
{
protected:
	int					m_nToDrawCount; // �׷��� �� ���� ���� 
	__AlphaPrimitive	m_Buffers[MAX_ALPHAPRIMITIVE_BUFFER]; // ������Ƽ�� ����..

public:
	int					ToDrawCount() { return m_nToDrawCount; }
	__AlphaPrimitive*	Add();

	void		Render();

	static int SortByCameraDistance(const void *pArg1, const void *pArg2); // ���� �Լ�..
	
	CN3AlphaPrimitiveManager();
	virtual ~CN3AlphaPrimitiveManager();
};

#endif // !defined(AFX_N3ALPHAPRIMITIVEMANAGER_H__616E318C_49E4_4468_9719_E62E384FC48D__INCLUDED_)
