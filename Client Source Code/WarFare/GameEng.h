// GameEng.h: interface for the CGameEng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEENG_H__5422B19E_D43E_4252_B79B_69323824F3F9__INCLUDED_)
#define AFX_GAMEENG_H__5422B19E_D43E_4252_B79B_69323824F3F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../N3BASE/N3Eng.h"
#include <list>

typedef std::list<class CN3Light*>::iterator		it_Light;
typedef std::list<class CN3Camera*> ::iterator	it_Camera;

// ī�޶� ����
enum e_ViewPoint		{	VP_BACKWARD = 0,			// �÷��̾� �� �ڿ��� �ٶ� ��� 1.5��Ī
							VP_FIRST_PERSON = 1,		// ����Ī
							VP_FOWARD = 2,				// �÷��̾� �տ��� �ٶ�..
							VP_THIRD_PERSON = 3,		// ����Ī ��� .. ���콺�� ������..
							VP_UNKNOWN = 0xffffffff };  // ������ ���� ī�޶� ������....

const float CAMERA_MOVE_SPEED = 5.0f;

class CGameEng : public CN3Eng  
{
protected:
	CN3Light*				m_pRefLightSun;
	CN3Light*				m_pRefLightSupport;
	CN3Light*				m_pRefLightCam;
	//std::list<CN3Light*>	m_Lights;
	//std::list<CN3Light*>	m_LightsBackup;
	std::list<CN3Camera*>	m_Cameras;
	CN3Camera*				m_pActiveCam;

	__Vector3		m_vEyeToReach;
	__Vector3		m_vAtToReach;

	float			m_fFPDeltaCur; // ���� 
	float			m_fFPDeltaToReach; // �̰��� ��ǥ�� �ؼ� ���Ѵ�.

	float			m_fRotPitchBackward; // 1.5��Ī �����϶� ī�޶� ��ġ ����.
	float			m_fRotPitchFirstPerson; // ����Ī �����϶� ī�޶� ��ġ ����.
	float			m_fRotPitchFoward; // �տ��� �� �����϶�  ī�޶� ��ġ ����.
	float			m_fRotPitchThirdFirson; // ����Ī ���� ī�޶� ��ġ ����
	
	float			m_fOffsetVPGod; // ������ �ٶ� ������ ��쿡 �Ÿ�..
	float			m_fRotYawVPGod; // ������ �ٶ� ������ ��쿡 ī�޶� ȸ������..

	float			m_fZoomBackwardOrFoward; // 1.5��Ī, �տ��� �� �����϶� ī�޶� Zoom 1.0f �� �����̴�.
	float			m_fLightningTimeRemain; // ���� ĥ�� Ÿ�̸�..

	e_ViewPoint		m_eViewPoint; // ī�޶� ����.

public:
	void			SetDefaultLight(CN3Light* pSun, CN3Light* pSupport, CN3Light* pCam);
	void			RestoreLighting();
	void			BackupReleaseLighting();
	void			Lightning(); // ���� ġ�� ����Ʈ �����ϱ�.
	float			FarPlaneDeltaCur() { return m_fFPDeltaCur; }
	float			FarPlaneDeltaToReach() { return m_fFPDeltaToReach; }
	void			FarPlaneDeltaSet(float fFPDelta, bool bUpdateImmediately);

	// Light �Լ���
	//void			LightAdd(CN3Light* pLight) { m_Lights.push_back(pLight); }
	//int				LightCount() { return m_Lights.size(); }
	//CN3Light*		Light(int index);

	// Camera �Լ���
	void			CameraAdd(CN3Camera *pCamera) { m_Cameras.push_back(pCamera); }
	int				CameraCount() { return m_Cameras.size(); }
	CN3Camera*		Camera(int index);
	void			CameraSetActiveByIndex(int index) { CN3Camera* pCam = this->Camera(index); if(pCam) m_pActiveCam = pCam; }
	CN3Camera*		CameraGetActive() { return m_pActiveCam; }

	// �߰��Ѱ�..
	void			ViewPointChange(e_ViewPoint eVP = VP_UNKNOWN);
	e_ViewPoint		ViewPoint() { return m_eViewPoint; } // ����..
	void			CameraPitchAdd(float fRotXPerSec);
	void			CameraYawAdd(float fRotYPerSec);
	void			CameraZoom(float fDelta);
	float			CameraYaw() { return m_fRotYawVPGod; } // ������ �ٶ� ������ ��쿡 ī�޶� ȸ������..
	void			SetActiveCamera(CN3Camera* pCamera) { m_pActiveCam = pCamera; }
	void			Tick(const D3DCOLOR* crDiffuses,			// Diffuse ����Ʈ ����..
						const D3DCOLOR* crAmbients,			// Ambient ����Ʈ ����..
						const D3DCOLOR crFog,				// �Ȱ� ����..
						const __Vector3& vPosPlayer,		// �÷��̾� ��ġ
						const __Quaternion& qtPlayer,		// ȸ�� ���ʹϾ�
						float fHeightPlayer,				// Ű�� �μ��� ������ ī�޶�� ����Ʈ ó��..
						float fSunRadianZ);					// ���� Z ����..
	void			ApplyCameraAndLight(); // ī�޶�� ����Ʈ�� ���õ� �� ����. ����Ʈ�� ���� ������ ���� �ٸ��� �Ѵ�.

	CGameEng();
	virtual ~CGameEng();

};

/*
inline CN3Light* CGameEng::Light(int index)
{
	if(index < 0 || index >= m_Lights.size()) return NULL;
	it_Light itLgt = m_Lights.begin();
	for(int i = 0; i < index; i++, itLgt++);

	return *itLgt;
}
*/

inline CN3Camera* CGameEng::Camera(int index)
{
	if(index < 0 || index >= (int)m_Cameras.size()) return NULL;
	it_Camera itCam = m_Cameras.begin();
	for(int i = 0; i < index; i++, itCam++);

	return *itCam;
}


#endif // !defined(AFX_GAMEENG_H__5422B19E_D43E_4252_B79B_69323824F3F9__INCLUDED_)
