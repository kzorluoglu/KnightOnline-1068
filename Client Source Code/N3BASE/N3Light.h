// N3Light.h: interface for the CN3Light class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3ILight_h__INCLUDED_)
#define AFX_N3ILight_h__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3Transform.h"

class CN3Light : public CN3Transform
{
public:
	typedef struct __Light : public D3DLIGHT9
	{
	public:
		BOOL		bOn; // ����Ʈ�� ���� �ִ���..
		int			nNumber; // 0 ~ 8
		
		void		Zero() { memset(this, 0, sizeof(__Light)); }
		void		InitPoint(int nLgtNumber, D3DXVECTOR3& dvPos, D3DCOLORVALUE& ltColor, float fRange = 10000.0f, float fAttenuation = 0.5f)
		{
			this->Zero();
			nNumber = nLgtNumber; // ����Ʈ ��ȣ..
			Type = D3DLIGHT_POINT;
			Position = dvPos;
			//Specular = 
			Diffuse = ltColor;
			Ambient.r = ltColor.r * 0.7f;
			Ambient.g = ltColor.g * 0.7f;
			Ambient.b = ltColor.b * 0.7f;

			Falloff = 1.0f;		// ȿ���� �̹��ϰ� ���ϱ� �ɸ��� ������ ���� 1.0���� ����.
			if(fRange < 0.0001f) fRange = 0.0001f;
			Attenuation0 = 1.0f - fAttenuation;
			Attenuation1 = fAttenuation/fRange; // ���� �������. ������ ������ ��Ȯ�ϰ� ������ ���谡 �ǵ��� �Ѵ�..
			Attenuation2 = fAttenuation/(fRange*fRange);
			Range = fRange * 4.0f;
			bOn = TRUE;
		}
		void		InitDirection(int nLgtNumber, D3DXVECTOR3& dvDir, _D3DCOLORVALUE& ltColor)
		{
			this->Zero();
			
			nNumber = nLgtNumber; // ����Ʈ ��ȣ..
			bOn = TRUE;

			Type = D3DLIGHT_DIRECTIONAL;
			Direction = dvDir;

			//Specular = 
			Diffuse = ltColor;
			Ambient.r = ltColor.r * 0.7f;
			Ambient.g = ltColor.g * 0.7f;
			Ambient.b = ltColor.b * 0.7f;
		}
		void		InitSpot(int nLgtNumber, D3DXVECTOR3& dvPos, D3DXVECTOR3& dvDir, _D3DCOLORVALUE& ltColor, float fTheta, float fPhi, float fRange = 10000.0f)
		{
			this->Zero();
			nNumber = nLgtNumber; // ����Ʈ ��ȣ..
			Type = D3DLIGHT_SPOT;
			Position = dvPos;
			Direction = dvDir;

			Diffuse = ltColor;
			Ambient.r = ltColor.r * 0.7f;
			Ambient.g = ltColor.g * 0.7f;
			Ambient.b = ltColor.b * 0.7f;

			if(fRange < 0.0001f) fRange = 0.0001f;
			Attenuation0 = 1.0f;
			Attenuation1 = 1.0f/(fRange/2.0f); // ���� �������. ������ ������ ��Ȯ�ϰ� ������ ���谡 �ǵ��� �Ѵ�..
			Range = fRange;

			Falloff = 1.0f;		// ȿ���� �̹��ϰ� ���ϱ� �ɸ��� ������ ���� 1.0���� ����.
			Theta = fTheta;
			Phi = fPhi;
			bOn = TRUE;
		}
	} __Light;

	__Light m_Data;

public:
	void		DirSet(const __Vector3& vDir) { m_Data.Direction = vDir; }
	void		DirSet(float fx, float fy, float fz) { m_Data.Direction.x = fx; m_Data.Direction.y = fy; m_Data.Direction.z = fz; }
	
	void		PosSet(const __Vector3& vPos) { m_vPos = m_Data.Position = vPos; }
	void		PosSet(float fx, float fy, float fz) { m_Data.Position.x = fx; m_Data.Position.y = fy; m_Data.Position.z = fz; m_vPos = m_Data.Position; }

	void		Apply(); // ���õ� ����Ʈ���� ���� D3DDevice �� ����
	void		Tick(float fFrm = FRAME_SELFPLAY); // ����Ʈ���� �����Ѵ�..
	
	bool		Load(HANDLE hFile);
#ifdef _N3TOOL
	bool		Save(HANDLE hFile);
#endif // end of _N3TOOL

	void		Release();
	CN3Light();
	virtual ~CN3Light();
};

#endif // !defined(AFX_N3ILight_h__INCLUDED_)
