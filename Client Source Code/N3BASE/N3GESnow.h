// N3GESnow.h: interface for the CN3GESnow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3GESNOW_H__F7097F94_15E3_42B8_95DB_9EEC3CFD2E9C__INCLUDED_)
#define AFX_N3GESNOW_H__F7097F94_15E3_42B8_95DB_9EEC3CFD2E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3GlobalEffect.h"

class CN3GESnow : public CN3GlobalEffect  
{
// Structures
protected:
	struct __SnowParticle
	{
		__Vector3	vPos;							// ȸ�� �ݰ��� �ܽ�
		__Vector3	vOffset1, vOffset2, vOffset3;	// �� 1 2 3�� ��ƼŬ �������� ������ ����
		float		fRadius;						// ȸ�� �ݰ�
		float		fRadian;						// ���� ȸ���� ����
		//float		fRVel							// ȸ�����ӵ� (�켱 �����ϴٰ� ����)
		// __Vector3 vVelocity						// �������� �ӵ�(�켱 ��� ���ٰ� ����)
	};

// Attributes
public:
	void SetVelocity(__Vector3& v) {m_vVelocity = v;}
protected:
	float		m_fWidth;		// ���� �������� ������ �ʺ�
	float		m_fHeight;		// ���� �������� ������ ����
	float		m_fSnowSize;	// �� �ϳ��� ũ��
	__Vector3	m_vVelocity;	// ��ü������ �������� ����� �ӵ�
	CN3Texture*	m_pTex;			// �� �ؽ���
	__SnowParticle*	m_pSnowParticle;	// ������ ����

// Operations
public:
	void Release();
	void Tick();
	void Render(__Vector3& vPos);
	void Create(float fDensity, float fWidth, float fHeight, float fSnowSize, const __Vector3& vVelocity, float fTimeToFade = 3.0f);

	CN3GESnow();
	virtual ~CN3GESnow();

};

#endif // !defined(AFX_N3GESNOW_H__F7097F94_15E3_42B8_95DB_9EEC3CFD2E9C__INCLUDED_)
