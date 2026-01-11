// N3ShapeMod.h: interface for the CN3ShapeMod class.
// ������ : dino
// n3shape�� �� �Ϻκ��� ������ �� �ְ� �� Ŭ����
// ����(0�� ����, 1������, 2������)�� ������ �� �ִ�.
// ���� ����� (�ð��� �־��ָ� ���°��� pos rot scale ������ linear�ϰ� ��ȭ��Ų��.

// ������ : Shape ��ü�� �����̴� ���¸� �߰�����
//          ����� �߰��Ϸ��� �ʹ� �����Ͽ� ������. ����ũ �� �κ��� �׿� ���õ� ����. �߰� �����ؾ���.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3SHAPEMOD_H__6F3C87B4_D10B_412B_B385_0ECE2D4A19A8__INCLUDED_)
#define AFX_N3SHAPEMOD_H__6F3C87B4_D10B_412B_B385_0ECE2D4A19A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3Shape.h"

class CN3ShapeMod : public CN3Shape  
{
public:
	CN3ShapeMod();
	virtual ~CN3ShapeMod();

// Structures
protected:
	struct __ModPosRotScale
	{
		__Vector3 vPos;
		__Quaternion qRot;
		__Vector3 vScale;

		__ModPosRotScale() {vPos.Set(0,0,0); qRot.Set(0,0,0,0); vScale.Set(1,1,1);}
	};
	struct __ModPart
	{
		CN3Shape::CPart*	pPart;				// ������ Part
		bool				bPos;				// Pos ��ȭ�� �ִ°�?
		bool				bRot;				// Rot ��ȭ�� �ִ°�?
		bool				bScale;				// Scale ��ȭ�� �ִ°�?
		__ModPosRotScale*	pStateInfos;		// ������ ���� ������(�̵�,ȸ��,Ȯ����� ���� m_iStateCount��)
		__ModPosRotScale	CurStateInfo;		// ���� ������ ���� ����(tick�� ���� ��ȭ�Ѵ�)

		__ModPart() {pPart = NULL; pStateInfos = NULL; bPos = false; bRot = false; bScale = false; }
		~__ModPart() {pPart = NULL; if (pStateInfos) { delete [] pStateInfos; pStateInfos = NULL;}}
	};

	struct __ModShape
	{
		bool				bShapePos;
		bool				bShapeRot;
		bool				bShapeScale;		// Shape�� ��ġ, ȸ��, ������ ��ȭ�� �ִ°�?
		__ModPosRotScale*	pShapeStateInfos;	// Shape ��ü�� ��ȭ ����
		__ModPosRotScale	CurShapeStateInfo;	// ���� ������ ���� ����(tick�� ���� ��ȭ�Ѵ�)
		__ModShape() {pShapeStateInfos = NULL; bShapePos = false; bShapeRot = false; bShapeScale = false;}
		~__ModShape() {if (pShapeStateInfos) {delete [] pShapeStateInfos; pShapeStateInfos = NULL;}}
	};

// Attributes
public:
protected:
//	enum	{
//			N3SHAPEMOD_TYPE_NOT_MOD = 0,		// �ƹ� ������ ��ȭ���� �ʴ� ��
//			N3SHAPEMOD_TYPE_ONLY_SHAPE = 1,		// Shape������ ��ȭ�ϴ� ��
//			N3SHAPEMOD_TYPE_ONLY_PART = 2,		// Part ������ ��ȭ�ϴ� ��
//			N3SHAPEMOD_TYPE_NORMAL = 4			// �� �� ��ȭ�ϴ� ��
//	};
//	int		m_iModType;			// type..
	int		m_iStateCount;		// ���°� ��� �ִ��� ��Ÿ����.
	int		m_iCurState;		// ���� ����
	int		m_iPrevState;		// ���� ����(���ο� ���¸� �������ָ� Animation�Ǵ� ���� �������¸� �����صд�.
	float	m_fTimeChanged;		// ���°� ������ �ٲ�� �ð�
	float	m_fTimeChanging;	// ���°� �ٲ�� ���۽ð����� ���ݱ����� ��� �ð�

	__ModShape	m_ModShape;			// shape�� ���� ��ȭ ����
	int		m_iModPartCount;		// ��ȭ�ϴ� ��Ʈ�� ����
	__ModPart*	m_pModParts;		// ��ȭ�ϴ� ��Ʈ�� ����

	__ModPart**	m_pMatchPart2ModPart;		// CPart����Ʈ �迭������ __ModPart*�� ��ġ��Ų �迭 (load�� CPart ������ŭ �迭�� ��� ������ �ִ´�)

// Operations
public:
	BOOL	SetState(int iState, float fSec);	// fSec�ð����� ���¸� �ٲ۴�. (fSec�� 0�ϰ�� ��� �ٲ��.)
	BOOL	LoadStateInfo(FILE* stream);	// ���� ������ �о�´�.(text�κ���)
	virtual void	Release();
	virtual void	ReCalcMatrix();
	virtual void	Tick(float fFrm = FRAME_SELFPLAY);
	virtual bool	Load(HANDLE hFile);
protected:
	CPart* GetPartByPMeshName(LPCTSTR pszName);	// �̸����� PMesh������ ���ϱ�

};

#endif // !defined(AFX_N3SHAPEMOD_H__6F3C87B4_D10B_412B_B385_0ECE2D4A19A8__INCLUDED_)
