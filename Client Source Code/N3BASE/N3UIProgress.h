// N3UIProgress.h: interface for the CN3UIProgress class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3UIPROGRESS_H__09307DB0_2F95_4205_AF1D_0262F1CE0E24__INCLUDED_)
#define AFX_N3UIPROGRESS_H__09307DB0_2F95_4205_AF1D_0262F1CE0E24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3UIBase.h"

class CN3UIImage;
class CN3UIProgress : public CN3UIBase  
{
#ifdef _N3TOOL
friend class CPropertyView;	// ������ �� �������� �����ϱ� ���ؼ� 
#endif

public:
	CN3UIProgress();
	virtual ~CN3UIProgress();
// Attributes
public:
	enum eIMAGE_TYPE{ IMAGETYPE_BKGND=0, IMAGETYPE_FRGND, NUM_IMAGETYPE};
	int				GetMax() const {return m_iMaxValue;}
	int				GetMin() const {return m_iMinValue;}
	int				GetCurValue() const {return (int)m_fCurValue;}

	void			SetStepValue(int iValue) {m_iStepValue = iValue;}
	void			StepIt() {SetCurValue((int)m_fCurValue + m_iStepValue);}
protected:
	CN3UIImage*		m_pBkGndImgRef;
	CN3UIImage*		m_pFrGndImgRef;

	__FLOAT_RECT	m_frcFrGndImgUV;				// m_FrGndImgRef �� uv��ǥ

	int				m_iMaxValue;					// �ִ�
	int				m_iMinValue;					// �ּ�
	float			m_fCurValue;					// ���� �� - �ε巴�� ���� ���� �÷������� float �� �ߴ�..
	float			m_fChangeSpeedPerSec;			// ���簪�� ���ؾ� �Ǵ� �ӵ�.. Unit SpeedPerSec
	int				m_iValueToReach;				// �����ؾ� �ɰ� - �ε巴�� ���� �ö󰡴� ��쿡 �ʿ��ϴ�..
	float			m_fTimeToDelay;					// �����ð�..
	
	int				m_iStepValue;					// ��ȭ�� StepIt()�� ���� ��ȭ�Ǵ� ��
//	Operations
public:
	virtual void	Tick();
//	virtual void	Render();
	virtual void	Release();
	virtual void	SetRegion(const RECT& Rect);
	virtual void	SetStyle(DWORD dwStyle);
	virtual bool	Load(HANDLE hFile);

	void			SetCurValue(int iValue, float fTimeToDelay = 0, float fChangeSpeedPerSec = 0);
	void			SetCurValue_Smoothly(int iValue, float fPercentPerSec){SetCurValue(iValue, 0, fPercentPerSec*(m_iMaxValue-m_iMinValue));}	// �ʴ� �� �ۼ�Ʈ ��ġ�� �ö󰣴�.
	void			SetRange(int iMin, int iMax) {m_iMinValue = iMin; m_iMaxValue = iMax; UpdateFrGndImage();}
	void			SetFrGndUVFromFrGndImage();	// m_pFrGndImgRef�κ��� uv��ǥ�� ���ͼ� m_frcFrGndImgUV�� �����Ѵ�.
protected:
	void			UpdateFrGndImage();	//m_FrGndImgRef �� ������ uv��ǥ�� m_fCurValue�� ���� �˸°� �ٲ۴�.

#ifdef _N3TOOL
// tool���� ����ϴ� �Լ���
public:
	virtual void	operator = (const CN3UIProgress& other);
	virtual bool	Save(HANDLE hFile);
	CN3UIImage*		GetBkGndImgRef() const {return m_pBkGndImgRef;}
	CN3UIImage*		GetFrGndImgRef() const {return m_pFrGndImgRef;}
	void			CreateImages();		// �̹��� ����
	void			DeleteBkGndImage();	// Back ground�̹����� �ʿ� ���� ��찡 �ִ�.
#endif
};

#endif // !defined(AFX_N3UIPROGRESS_H__09307DB0_2F95_4205_AF1D_0262F1CE0E24__INCLUDED_)

