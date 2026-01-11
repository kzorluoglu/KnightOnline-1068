// N3Camera.h: interface for the CN3Camera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3Camera_h__INCLUDED_)
#define AFX_N3Camera_h__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3Transform.h"

class CN3Camera : public CN3Transform
{
public:
	__Vector3			m_vAt;
	__CameraData		m_Data;
	BOOL				m_bFogUse;
	D3DCOLOR			m_FogColor;
//	float				m_fFogStart;
//	float				m_fFogEnd;
//	float				m_fFogDensity;
	float				m_fRadianX;
	bool				m_bOrtho;

public:
	void		Rotate(float fRadianX, float fRadianY);
	void		LookAt(const __Vector3& vEye, __Vector3& vAt, __Vector3& vUp);
	__Vector3	EyePos() { return m_vPos; }
	__Vector3	AtPos() { return m_vAt; }
	__Vector3	UpVector() { return m_vScale; } // Rotation �� LookAt Position ó��, Scale �� UpVector ó�� ����..
	__Vector3	Dir() {__Vector3 vDir = m_vAt - m_vPos; vDir.Normalize(); return vDir;} // ������ �ǵ�����.

	void		EyePosSet(const __Vector3& v) { m_vPos = v; }
	void		EyePosSet(float x, float y, float z) { m_vPos.Set(x, y, z); }
	void		AtPosSet(const __Vector3& v) { m_vAt = v; } 
	void		AtPosSet(float x, float y, float z) { m_vAt.Set(x, y, z); }
	void		UpVectorSet(const __Vector3& v) { m_vScale = v; } // Scale �� UpVector ó�� ����..
	void		UpVectorSet(float x, float y, float z) { m_vScale.Set(x, y, z); } // Scale �� UpVector ó�� ����..

	bool		Load(HANDLE hFile);

#ifdef _N3TOOL
	BOOL		MoveByWindowMessage(MSG* pMsg);
	void		Move(const __Vector3& vDelta);
	void		MovePlane(float fX, float fY);
	void		MoveStraight(float fDistance, bool bSmall = false);
	void		LookAround(float fRadianX, float fRadianY);
	void		Zoom(float fDelta);
	bool		Save(HANDLE hFile);
#endif // end of #ifdef _N3TOOL
	void		Release();
	CN3Camera();
	virtual ~CN3Camera();

	void		Tick(float fFrm = FRAME_SELFPLAY); // ī�޶󰪸� �ܼ��� �����ϱ⸸ �Ѵ�..
	void		Render(float fUnitSize = 1.0f);
	void		Apply(); // ���õ� ī�޶��� ���� D3DDevice �� ����
};

#endif // !defined(AFX_N3Camera_h__INCLUDED_)
