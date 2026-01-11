// N3FXBundle.h: interface for the CN3FXPartParticles class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __N3FXBUNDLE_H__
#define __N3FXBUNDLE_H__

#include "N3BaseFileAccess.h"
#include "N3FXDef.h"
#include "N3FXPartBase.h"

class CN3FXBundle : public CN3BaseFileAccess  
{
public:
	// �⺻ ������...
	
	int					m_iVersion;		//�������
	std::string			m_strName;		//�����̸�
	int					m_iMoveType;	//�����̴� ����..
	float				m_fVelocity;
	bool				m_bStatic;		//��ġ�� ������ų���ΰ�....

	float				m_fLife0;		//������ ����..

	FXPARTWITHSTARTTIME*	m_pPart[MAX_FX_PART];
	
	// ���ϴ� ������...
	float				m_fLife;		//���� ����..
	DWORD				m_dwState;		//���� ������ ����

	__Vector3			m_vPos;			//���� ������ ��ġ.
	__Vector3			m_vPrePos;		//�ε� �Ǵ� Duplicate �ϱ����� �ӽ÷� ��ġ ����
	__Vector3			m_vDestPos;		//���� ������ ��ǥ��ġ. 
	__Vector3			m_vDir;			//���� ������ ����.
	
	int					m_iSourceID;	//�� ȿ���� ���� ��ü�� ���̵�..(���ӿ���..)
	int					m_iTargetID;	//�� ȿ���� �޴� ����� ���̵�..(���ӿ���..)
	int					m_iTargetJoint;	//�� ȿ���� �޴� ����� ����..(���ӿ���..)
	int					m_iSourceJoint;

	bool				m_bDependScale;
	//__Vector3			m_vTargetScale;
	float				m_fTargetScale;

//	int					m_iNumStep;		//��ܰ質 �����ϴ���...^^
//	int					m_iCurrStep;	//���� �������� ������
	static float		m_fEffectSndDist;

	class CN3SndObj*	m_pSndObj;

protected:
	bool	CheckAllPartsDead();		//��� ��Ʈ���� �׾����� �˻�..

public:
//�⺻������ �ʿ��� �Լ���...
	virtual void	Trigger(int iSourceID=0, int iTargetID = 0, int iTargetJoint = 0, int iSndID = -1);
	virtual bool	Tick();
	virtual void	Duplicate(CN3FXBundle* pDestBundle);

	void	Init();
	void	Stop(bool immediately=false);
	
	void	Render();
	DWORD	GetState() { return m_dwState; }

	virtual bool	Load(HANDLE hFile); // �ڵ鿡�� �о����..
	bool	Save(HANDLE hFile);	// �����ϱ�..

//	int				GetPartCount() { return m_pPartList.size(); }
	CN3FXPartBase*	GetPart(int i);
	float			GetPartSTime(int i);
	void			SetPartSTime(int i, float time);
	static void		SetEffectSndDistance(float fDist) { m_fEffectSndDist = fDist; }
	static float	GetEffectSndDistance() { return m_fEffectSndDist; }

//������ �Ҹ���..
	CN3FXBundle();	
	virtual ~CN3FXBundle();

#ifdef _N3TOOL	//�������� ���� �Լ���...
public:
	CN3FXPartBase*	SetPart(const char* pFileName);
	bool	DecodeScriptFile(const char* lpPathName);
#endif // end of _N3TOOL
};

#endif // #ifndef __N3FXBUNDLE_H__
