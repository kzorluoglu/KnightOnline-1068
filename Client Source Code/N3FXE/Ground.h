// Ground.h: interface for the CN3FXPartParticles class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __GROUND_H__
#define __GROUND_H__

#include "../N3Base/N3Base.h"

class CN3Texture;

class CGround : public CN3Base  
{
public:
	int			m_iSize;	//�Ѻ��� ����..
	int			m_iTileSize;	//Ÿ���� ����..
	__VertexT1* m_pVB;
	CN3Texture* m_pTile;
	
public:
	void SetTex(const char* szPath);
	void Render();

	CGround();	
	virtual ~CGround();
};

#endif // #ifndef __GROUND_H__
