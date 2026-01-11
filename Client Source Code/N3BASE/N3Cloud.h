// N3Cloud.h: interface for the CN3Cloud class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3CLOUD_H__0C780CD3_38F2_48CD_B36E_E7C64C7893EB__INCLUDED_)
#define AFX_N3CLOUD_H__0C780CD3_38F2_48CD_B36E_E7C64C7893EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3Base.h"
#include "N3ColorChange.h"
#include <string>

const int NUM_CLOUD_VERTEX = 8;
//const int NUM_CLOUD_VERTEX = 12;

enum	e_CLOUDTEX {CLOUD_NONE=-1,CLOUD_WISPS=0, CLOUD_PUFFS, CLOUD_TATTERS, CLOUD_STREAKS,
	CLOUD_DENSE, CLOUD_OVERCAST, NUM_CLOUD};





class CN3Cloud : public CN3Base  
{
	friend class CN3SkyMng;
public:
	CN3Cloud();
	virtual ~CN3Cloud();

protected:
	__VertexXyzColorT2	m_pVertices[NUM_CLOUD_VERTEX]; // �������� ���ؽ�
	CN3Texture*		m_pTextures[NUM_CLOUD];				// �ؽ��ĵ�..
	std::string		m_szTextures[NUM_CLOUD];			// �ؽ�ó ���� �̸���...

	CN3ColorChange	m_Color1;							// ���� ��1
	CN3ColorChange	m_Color2;							// ���� ��2
	CN3ColorChange	m_Alpha;							// ���� �ٲ� alpha��
	e_CLOUDTEX		m_eCloud1;							// ���� �ؽ���1
	e_CLOUDTEX		m_eCloud2;							// ���� �ؽ���2
	e_CLOUDTEX		m_eCloud3;							// ���� �ؽ���3

	float			m_fCloudTexTime;					// ���� ���� ���� �ð�
	e_CLOUDTEX		m_eBackupCloud;						// 2��° ���� �����ؾ� �� texture���� ����
	float			m_fBackupTime;						// 2��° ���� �����ؾ� �� �ð� ����

// Operations
public:
	void			ChangeColor1(D3DCOLOR color, float fSec) {m_Color1.ChangeColor(color, fSec);}
	void			ChangeColor2(D3DCOLOR color, float fSec) {m_Color2.ChangeColor(color, fSec);}
	void			SetCloud(e_CLOUDTEX eCloud1, e_CLOUDTEX eCloud2, float fSec);
	void			Init(const std::string* pszFNs);
	virtual void	Release();
	virtual void	Render();
	virtual void	Tick();
protected:
	LPDIRECT3DTEXTURE9	GetTex(e_CLOUDTEX tex);

};

#endif // !defined(AFX_N3CLOUD_H__0C780CD3_38F2_48CD_B36E_E7C64C7893EB__INCLUDED_)
