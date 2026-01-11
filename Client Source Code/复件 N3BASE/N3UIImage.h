// N3UIImage.h: interface for the CN3UIImage class.
// �����ؾ� �� �� : animation�Ǵ� �̹����� �׸����� �����ϰ� ����..
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3UIIMAGE_H__80AE123C_5A12_4A0E_9CF0_511ED98BB896__INCLUDED_)
#define AFX_N3UIIMAGE_H__80AE123C_5A12_4A0E_9CF0_511ED98BB896__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3UIBase.h"

class N3Texture;
class CN3UIImage : public CN3UIBase  
{
#ifdef _N3TOOL
friend class CPropertyView;	// ������ �� �������� �����ϱ� ���ؼ� 
#endif

public:
	CN3UIImage();
	virtual ~CN3UIImage();

// Attributes
public:
	__FLOAT_RECT*			GetUVRect() {return &m_frcUVRect;}
protected:
	LPDIRECT3DVERTEXBUFFER9 m_pVB;			// vertex buffer
	CN3Texture*				m_pTexRef;		// texture ���� ������ (s_TexMng���� �����ϹǷ� �����������̴�.)
	std::string				m_szTexFN;		// �ؽ�ó �̸�.. ���� ���� �ִ� ������ ������ �ؽ�ó �θ��Ⱑ ������ ��츦 ����ؼ��̴�.
	__FLOAT_RECT			m_frcUVRect;	// uv ��ǥ�� ����
	D3DCOLOR				m_Color;		// Į��(��� �̹����� ������� ���ȴ�.)

	float					m_fAnimFrame;	// 1�ʴ� �׷����� �̹����� ��
	int						m_iAnimCount;	// Animate�Ǵ� �̹��� ��(Load�� �� child�� ���ڷ� �ľ�)
	int						m_fCurAnimFrame;	// ���� �׷��� animate frame index (�Ҽ��� ������ �������)
	CN3UIImage**			m_pAnimImagesRef;	// Animate �� �ڽ� �̹����� ���� �����͵�

//	Operations
public:
	D3DCOLOR	GetColor()	{ return m_Color; }

	CN3Texture*		GetTex() const { return m_pTexRef; }	// Texture ������ ���
	void			SetTex(const std::string& szFN);		// Texture ����
	void			SetUVRect(float left, float top, float right, float bottom);	// image�� uv��ǥ ����
	void			SetColor(D3DCOLOR color);				// Į�� ����

	virtual void	SetRegion(const RECT& Rect);					// ���� ����
	virtual BOOL	MoveOffset(int iOffsetX, int iOffsetY);
	virtual void	Release();								// Release
	virtual void	Tick();									// Tick
	virtual void	Render();								// �׸���
	virtual void	RenderIconWrapper();
	virtual void	Init(CN3UIBase* pParent);				// �ʱ�ȭ
	virtual bool	Load(HANDLE hFile);

	virtual void	operator = (const CN3UIImage& other);

protected:
	bool			CreateVB();								// 4���� vertex�� ���� vertex buffer ����
	virtual void	SetVB();								// vertex buffer �ٽ� ����
// tool���� ����ϴ� �Լ���
#ifdef _N3TOOL
public:
	virtual bool	Save(HANDLE hFile);
	virtual void	ChangeImagePath(const std::string& szPathOld, const std::string& szPathNew);
	void			GatherImageFileName(std::set<std::string>& setImgFile);
	std::string		GetTexFN() { return m_szTexFN; }

	void			ReorderChildImage();
	void			SetAnimImage(int iAnimCount);
	CN3UIImage*		GetChildImage(int iIndex);
	virtual bool	ReplaceAllTextures(const std::string& strFind, const std::string& strReplace);
#endif
};

#endif // !defined(AFX_N3UIIMAGE_H__80AE123C_5A12_4A0E_9CF0_511ED98BB896__INCLUDED_)
