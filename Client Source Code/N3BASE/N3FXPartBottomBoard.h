// N3FXPartBottomBoard.h: interface for the CN3FXPartBottomBoard class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __N3FXPARTBOTTOMBOARD_H__
#define __N3FXPARTBOTTOMBOARD_H__

#include "N3BaseFileAccess.h"
#include "N3FXDef.h"
#include "N3FXPartBase.h"

//
//	�ٴڿ� �׸��� �Ƕ���..
//	triangle fan���� �׸��� �ﰢ�� �������� �׸���..
//

class CN3FXPartBottomBoard : public CN3FXPartBase  
{
public:
	float				m_fSizeX;			//	������ ũ��
	float				m_fSizeZ;

	bool				m_bTexLoop;
	int					m_iTexIdx;

	DWORD				m_dwCurrColor;

	__VertexXyzColorT1*	m_pVB;

	float				m_fScaleVelX;
	float				m_fScaleVelZ;
	float				m_fScaleAccelX;
	float				m_fScaleAccelZ;

	float				m_fCurrScaleVelX;
	float				m_fCurrScaleVelZ;

	float				m_fCurrSizeX;			//	������ ũ��
	float				m_fCurrSizeZ;

	float				m_fGap;
	
protected:
	__Vector3*			m_vUnit;

protected:
	void	CreateVB();
	bool	IsDead();

public:
	void	Init();									//	���� �������� ó�� �ε��� ���·� �ʱ�ȭ...
	void	Start();								//	��Ʈ ���� ����.
	void	Stop();									//	��Ʈ ���� ����..
	bool	Tick();									//	ticktick...
	void	Render();								//	ȭ�鿡 �Ѹ���..
	bool	Load(HANDLE hFile);						//	�������� �ҷ�����.
	bool	Save(HANDLE hFile);						//	�������� �������.
	void	Duplicate(CN3FXPartBottomBoard* pSrc);

public:
	CN3FXPartBottomBoard();
	virtual ~CN3FXPartBottomBoard();

#ifdef _N3TOOL
	bool	ParseScript(char* szCommand, char* szBuff0, char* szBuff1, char* szBuff2, char* szBuff3);
#endif // end of _N3TOOL
};

#endif // #ifndef __N3FXPARTBOTTOMBOARD_H__
