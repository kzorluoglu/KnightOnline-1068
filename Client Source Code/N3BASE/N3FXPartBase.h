// N3FXPartBase.h: interface for the CN3FXPartBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3EFFECTPARTBASE_H__E11B5E1E_C4FF_4186_9645_E05DB129747B__INCLUDED_)
#define AFX_N3EFFECTPARTBASE_H__E11B5E1E_C4FF_4186_9645_E05DB129747B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3BaseFileAccess.h"
#include "N3FXDef.h"
#include "N3Texture.h"

class CN3FXBundle;

class CN3FXPartBase : public CN3BaseFileAccess  
{
public:
//��� ������..
	int				m_iVersion;			//	�ڷ��� ����..
	int				m_iBaseVersion;

	CN3FXBundle*	m_pRefBundle;		//	���� ���� ������ ������.
	CN3FXPartBase*	m_pRefPrevPart;		//	�� ���� ��Ʈ..

	int				m_iType;			//	����Ÿ��(particle, mesh, board...)
	float			m_fLife;			//	�÷��̽ð�..

	std::string		m_strName;			//	��Ʈ�̸�..

	__Vector3		m_vVelocity;		//	�ʴ� �����̴� �ӵ�..(�ʼӵ�)
	__Vector3		m_vAcceleration;	//	���ӵ�...(�߷°��ӵ��� �ְ� ������ y = -9.8..^^
	__Vector3		m_vRotVelocity;		//	�ʴ� ȸ���� �� �ִ� ���ӵ�..

	float			m_fCurrLife;			
	__Vector3		m_vCurrVelocity;
	__Vector3		m_vCurrPos;

	DWORD			m_dwState;			//	���� ��Ʈ�� ����..
	__Vector3		m_vPos;				//	���鿡�� ��Ʈ�� ��ġ.

	bool			m_bOnGround;		//�ٴڿ� �پ �� ���ΰ�...

	//texture..
	char			m_pTexName[MAX_PATH];
	CN3Texture**	m_ppRefTex;
	int				m_iNumTex;
	float			m_fTexFPS;

	float			m_fFadeOut;
	float			m_fFadeIn;

	DWORD			m_dwRenderFlag;

	DWORD			m_dwSrcBlend;
	DWORD			m_dwDestBlend;
	BOOL			m_bAlpha;

	DWORD			m_dwZEnable;
	DWORD			m_dwZWrite;
	DWORD			m_dwLight;
	DWORD			m_dwDoubleSide;
		
protected:
	virtual bool	IsDead();

public:
	virtual float	GetGroundHeight(float x, float z) { return 0.01f; }

	virtual void	Init();				//	���� �������� ó�� �ε��� ���·� �ʱ�ȭ...
	virtual void	Start();			//	��Ʈ ���� ����.
	virtual void	Stop();				//	��Ʈ ���� ����..
	virtual bool	Tick();				//	ticktick...
	virtual void	Render();			//	ȭ�鿡 �Ѹ���..
	virtual bool	Load(HANDLE hFile);	//	�������� �ҷ�����.
	virtual bool	Save(HANDLE hFile);	//	�������� �����ϱ�.
	void	Duplicate(CN3FXPartBase* pSrc);

#ifdef _N3TOOL
protected:
	virtual bool	ParseScript(char* szCommand, char* szBuff0, char* szBuff1, char* szBuff2, char* szBuff3);	//�������� ��ũ��Ʈ �ؼ� �Լ�..
	
public:
	bool	DecodeScriptFile(const char* lpPathName);
#endif // end of _N3TOOL

public:
//������ �Ҹ���..
	CN3FXPartBase();
	virtual ~CN3FXPartBase();
};

#endif // !defined(AFX_N3EFFECTPARTBASE_H__E11B5E1E_C4FF_4186_9645_E05DB129747B__INCLUDED_)
