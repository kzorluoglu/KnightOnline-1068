// N3FXParticle.h: interface for the CN3FXParticle class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __N3FXPARTPARTICLES_H__
#define __N3FXPARTPARTICLES_H__

#pragma warning(disable : 4786)

#include <list>
#include "N3FXPartBase.h"

class CN3FXParticle;
class CN3FXShape;

class CN3FXPartParticles : public CN3FXPartBase  
{
public:
	//related whole particle system...
	__VertexXyzColorT1			m_vUnit[NUM_VERTEX_PARTICLE];
	//WORD						m_wUnitIB[6];

	__VertexXyzColorT1*			m_pVB;
	//WORD*						m_pIB;

	__Matrix44					m_mtxVI;				//	inverse view mtx..

	int							m_iNumParticle;			//	��ƼŬ�� �� ��..(maximum)
	int							m_iNumLodParticle;		//	�Ÿ��� ���� ��ƼŬ ����
	//float						m_fParticleSize;		//	��ƼŬ�� ũ��
	std::pair<float, float>		m_pair_fParticleSize;	//	��ƼŬ ũ���� ����
	std::pair<float, float>		m_pair_fParticleLife;	//	��ƼŬ ������ ����
	
	std::list<CN3FXParticle*>	m_pVBList_Alive;		//	����ִ� ��ƼŬ�� ��� �ִ� ����..
	std::list<CN3FXParticle*>	m_pVBList_Dead;			//	���� ��ƼŬ�� ��� �ִ� ����..

	float						m_fCreateDelay;			//	��ƼŬ ���� �ð� ����.
	float						m_CurrCreateDelay;		//	���� ���� create delay time..
	int							m_iNumCreate;			//	�ѹ� ������ ��������� ��ƼŬ ��.

	__Vector3					m_MinCreateRange;		//	��ƼŬ ���� ������ ����..min..
	__Vector3					m_MaxCreateRange;		//	��ƼŬ ���� ������ ����..max..

	//emitter...
	DWORD						m_dwEmitType;			//	�߻�����..(spread, gather)..
	PARTICLEEMITCONDITION		m_uEmitCon;				//	�߻����¿� ���� �ʿ� ����Ÿ..
	__Vector3					m_vEmitterDir;
	__Vector3					m_vPrevShapePos;
	__Vector3					m_vShapePos;

	//particle..
	__Vector3	m_vPtEmitDir;		//emitter���� ��ƼŬ �л����..
	float		m_fPtVelocity;		//��ƼŬ �л�ӵ�..
	float		m_fPtAccel;			//�л� ���ӵ�..
	float		m_fPtRotVelocity;	//��ƼŬ ȸ�� �ӵ�..
	float		m_fPtGravity;		//�߷°��ӵ�..

	//related particle color....
	DWORD		m_dwChangeColor[NUM_KEY_COLOR];
	bool		m_bChangeColor;

	//related animation key
	CN3FXShape*	m_pShape;
	CN3FXShape*	m_pRefShape;
	float		m_fMeshFPS;
	bool		m_bAnimKey;

	//particle local rotate......2002.10.21.
	float		m_fTexRotateVelocity;		//	��ƼŬ �ؽ��ĸ� ���� Z������ ȸ�����Ѽ� ���ۺ��� ���� ��ó�� ���̰�...(�ӵ�)
	float		m_fScaleVelX;
	float		m_fScaleVelY;

protected:
	void	Rotate();
	void	Scaling();
	void	Move();

	void	InitVB();
	void	CreateParticles();
	void	CreateParticles_Spread();
	void	CreateParticles_Gather();

	std::list<CN3FXParticle*>::iterator	DestroyParticle(std::list<CN3FXParticle*>::iterator it);

	bool	IsDead();

	bool RotateQuaternion(__Vector3 vSrcDir, __Vector3 vDestDir, __Quaternion* pQt);
	float CameraDist(__Vector3 v1, __Vector3 v2, __Vector3 v3);

	//////////////////////////////////////////////
	//m_pVBList_Alive�� ��Ʈ�ϱ������̾�..
	void	PSort();
	void	PMerge(std::list<CN3FXParticle*>& l1, std::list<CN3FXParticle*>& l2);
	bool	PComp(CN3FXParticle* pP1, CN3FXParticle* pP2);
	//
	//////////////////////////////////////////////
	
public:
	void	Init();				//	���� �������� ó�� �ε��� ���·� �ʱ�ȭ...
	void	Start();			//	��Ʈ ���� ����.
	void	Stop();				//	��Ʈ ���� ����..
	bool	Tick();				//	ticktick...
	void	Render();			//	ȭ�鿡 �Ѹ���..
	bool	Load(HANDLE hFile);	//	�������� �ҷ�����.
	bool	Save(HANDLE hFile);	//	�������� �������.
	void	Duplicate(CN3FXPartParticles* pSrc);

	bool	GetColor(int key, DWORD& color);

	CN3FXPartParticles();	
	virtual ~CN3FXPartParticles();	

#ifdef _N3TOOL
	bool	ParseScript(char* szCommand, char* szBuff0, char* szBuff1, char* szBuff2, char* szBuff3);

	bool	m_bChangeColorKey[NUM_KEY_COLOR];
	bool	m_bChangeAlphaKey[NUM_KEY_COLOR];
#endif // end of _N3TOOL
};

#endif // #ifndef __N3FXPARTPARTICLES_H__


