// N3FXPartMesh.h: interface for the CN3FXPartMesh class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __N3FXPARTMESH_H__
#define __N3FXPARTMESH_H__

#include "N3FXPartBase.h"

class CN3FXShape;

class CN3FXPartMesh : public CN3FXPartBase
{
public:
	CN3FXShape*	m_pShape;
	CN3FXShape*	m_pRefShape;

	DWORD		m_dwCurrColor;	//
	
	char		m_cTextureMoveDir;	//�ؽ��� �̵� ����..1:up 2:down, 3:left, 4:right
	float		m_fu;				//�ؽ��� �̵� �ӵ�
	float		m_fv;				//�ؽ��� �̵� �ӵ�

	__Vector3	m_vUnitScale;
	__Vector3	m_vScaleVel;
	__Vector3	m_vCurrScaleVel;
	__Vector3	m_vScaleAccel;
	__Vector3	m_vDir;

	bool		m_bTexLoop;
	float		m_fMeshFPS;

protected:
	bool	IsDead();

public:
	void	Init();				//	���� �������� ó�� �ε��� ���·� �ʱ�ȭ...
	void	Start();			//	��Ʈ ���� ����.
	void	Stop();				//	��Ʈ ���� ����..
	bool	Tick();				//	ticktick...
	void	Render();			//	ȭ�鿡 �Ѹ���..
	bool	Load(HANDLE hFile);	//	�������� �ҷ�����.
	bool	Save(HANDLE hFile);	//	�������� �������.
	void	Duplicate(CN3FXPartMesh* pSrc);
		
public:
	void	Rotate();
	void	Move();
	void	Scaling();
	void	MoveTexUV();

	int		NumPart();
	int		NumVertices(int Part);
	LPDIRECT3DVERTEXBUFFER9 GetVB(int Part);

	CN3FXPartMesh();
	virtual ~CN3FXPartMesh();

#ifdef _N3TOOL
	bool	ParseScript(char* szCommand, char* szBuff0, char* szBuff1, char* szBuff2, char* szBuff3);
#endif // end of _N3TOOL

};

#endif // #ifndef __N3FXPARTMESH_H__
