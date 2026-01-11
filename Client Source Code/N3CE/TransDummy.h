// TransDummy.h: interface for the CTransDummy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSDUMMY_H__EBD00CDE_FCE8_411E_A7A3_020D137904E2__INCLUDED_)
#define AFX_TRANSDUMMY_H__EBD00CDE_FCE8_411E_A7A3_020D137904E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../N3Base/N3Transform.h"
#include <afxtempl.h>

enum {DUMMY_CENTER=0, DUMMY_X=1, DUMMY_Y=2, DUMMY_Z=3, NUM_DUMMY=4,DUMMY_NO=5};
const int NUM_CUBEVERTEX = 36;

struct __DUMMYCUBE
{
	int iType;		// Center , X, Y, Z ���� ����.
	__VertexXyzNormalColor	Vertices[NUM_CUBEVERTEX];	// ť���� ���� ����
	__Vector3				vCenterPos;	// ť���� ��� ��
	float					fDistance;	// ī�޶���� �Ÿ�
};

class CTransDummy : public CN3Transform
{
public:
	CTransDummy();
	virtual ~CTransDummy();

// Attributes
public:
protected:
	__DUMMYCUBE			m_DummyCubes[NUM_DUMMY];	// 4���� ť��(center, x, y, z);
	__VertexXyzColor	m_LineVertices[6];			// 4���� ť�긦 �̾��ִ� ���� �����ϴ� ��
	CTypedPtrArray<CPtrArray, CN3Transform*> m_SelObjArray;	// MapMng���� ���õ� ��ü��
	__DUMMYCUBE*		m_pSortedCubes[NUM_DUMMY];	// Cube �Ÿ��� ���� ���ĵ� ������
	__DUMMYCUBE*		m_pSelectedCube;			// ���õ� ť��
	__Vector3			m_vPrevPos;					// �̹� ���콺 �巡�� ������ ��ġ ȸ�� Ȯ��/��Ұ�
	__Quaternion		m_qPrevRot;					// �̹� ���콺 �巡�� ������ ��ġ ȸ�� Ȯ��/��Ұ�
	__Vector3*			m_vPrevScaleArray;			// �̹� ���콺 �巡�� ������ Ȯ��/��Ұ��� (���õ� ��ü�� ����� ���� ������ �־�� �ϱ� ������ �迭)

// Operations
public:
	virtual void Tick();
	virtual void Render();
	virtual void Release();
	virtual void SetSelObj(CN3Transform* pObj);		// ���õ� ��ü �ٲٱ�
	virtual void AddSelObj(CN3Transform* pObj);	// ���õ� ��ü �߰�
	virtual BOOL MouseMsgFilter(LPMSG pMsg);	// ���콺 �޼��� ó��
	void GetPickRay(POINT point, __Vector3& vDir, __Vector3& vOrig);	// ���콺 �����Ͱ� ����Ű�� ������ ������� �� ��� �Լ�
protected:
	__DUMMYCUBE* Pick(int x, int y);	// ť�� picking
	void InitDummyCube(int iType, __DUMMYCUBE* pDummyCube, __Vector3& vOffset, D3DCOLOR color);	// ť�� �ʱ�ȭ
	static int SortCube(const void* pArg1, const void* pArg2);	// ť�� ī�޶� �Ÿ��� ���� �����Լ�
	virtual void TransDiff(__Vector3* pvDiffPos, __Quaternion* pvDiffRot, __Vector3* pvDiffScale);		// ���̸�ŭ ���õ� ������Ʈ���� ������Ų��.
};

#endif // !defined(AFX_TRANSDUMMY_H__EBD00CDE_FCE8_411E_A7A3_020D137904E2__INCLUDED_)
