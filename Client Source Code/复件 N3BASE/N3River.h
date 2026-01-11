// N3River.h: interface for the CN3River class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3RIVER_H__D0171C53_F631_4EC3_9D42_B4B754093FAC__INCLUDED_)
#define AFX_N3RIVER_H__D0171C53_F631_4EC3_9D42_B4B754093FAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3Base.h"
#include "N3LinkedList.h"

struct __RiverInfo
{
	// ������ ����
	short		iRiverID;
	DWORD		dwAlphaFactor;
	float		fSpeed1;
	float		fSpeed2;
	char*		pszTexName;

	// animation texture ����
	float		fAnimTexFPS;	// �ʴ� �����Ӽ�
	int			iAnimTexCount;	// Animation texture ����
	char**		pszAnimTexNames;// animation texture �̸���

	// �ǽð����� ���ϴ� ����
	float		fCurDiffV1;		// ������ ���� V��ǥ�� ����
	float		fCurDiffV2;		// ������ ���� V2��ǥ�� ����
	CN3Texture*	pTexture;		// Texture ���������� (NULL�� �ƴϸ� Load�Ǿ� �ִ� ���̴�.
	int			iCurAnimTexFrm;	// ���� animation texture�� ������ ��ȣ..
	CN3Texture** pAnimTextures;	// animation texture�����͵�..(NULL�� �ƴϸ� Load�Ǿ� �ִ� ���̴�.
	DWORD		dwPrevTick;		// ���� iCurAnimTexFrm�� ������Ų tickcount

	__RiverInfo () {iRiverID = -1; dwAlphaFactor = 0x00000000; fSpeed1 = fSpeed2 = 0; pszTexName = NULL;
					fAnimTexFPS = 30.0f; iAnimTexCount = 0; pszAnimTexNames = NULL;
					fCurDiffV1 = fCurDiffV2 = 0; pTexture = NULL; iCurAnimTexFrm = 0; pAnimTextures = NULL; dwPrevTick = 0;}
	~__RiverInfo () { if (pszTexName) delete [] pszTexName;
						__ASSERT(pTexture==NULL, "Texture ���� �����Ͱ� ���� ���� �ʾҽ��ϴ�.");
						for(int i=0; i<iAnimTexCount; ++i)
						{	__ASSERT(pszAnimTexNames, "");
							if (pszAnimTexNames[i]) {delete [] pszAnimTexNames[i]; pszAnimTexNames[i] = NULL;}
							__ASSERT(pAnimTextures, ""); __ASSERT(pAnimTextures[i]==NULL, "Texture ���� �����Ͱ� ���� ���� �ʾҽ��ϴ�.");
						}
						if (pszAnimTexNames) {delete [] pszAnimTexNames; pszAnimTexNames = NULL;}
						if (pAnimTextures) {delete [] pAnimTextures; pAnimTextures = NULL;}
					}
	void		SetTexName(const char* pszName)
	{
		int iLen = lstrlen(pszName); if (iLen==0) return;
		if(pszTexName) delete [] pszTexName;
		pszTexName = new char[iLen+1];
		lstrcpy(pszTexName, pszName);
	}
	void		SetAnimTexName(int iIndex, const char* pszName)
	{
		__ASSERT(pAnimTextures && pszAnimTexNames, "SetAnimTexCount�� �ѹ� ȣ���ؾ� �Ѵ�.");
		int iLen = lstrlen(pszName); if (iLen==0) return;
		if(pszAnimTexNames[iIndex]) delete [] pszAnimTexNames[iIndex];
		pszAnimTexNames[iIndex] = new char[iLen+1];
		lstrcpy(pszAnimTexNames[iIndex], pszName);
	}
	void		SetAnimTexCount(int iCount)	// ���Լ��� ó���� �ѹ��� �ҷ���� �Ѵ�.
	{
		__ASSERT(pAnimTextures == NULL && iAnimTexCount == 0 && pszAnimTexNames == NULL && iCount>0,"���Լ��� ó���� �ѹ��� �ҷ���� �Ѵ�.");
		iAnimTexCount = iCount;
		pszAnimTexNames = new char*[iAnimTexCount];
		pAnimTextures = new CN3Texture*[iAnimTexCount];
		ZeroMemory(pszAnimTexNames, sizeof(char*)*iAnimTexCount);
		ZeroMemory(pAnimTextures, sizeof(CN3Texture*)*iAnimTexCount);
	}
	void		DeleteTextures()
	{
		if (pTexture) { CN3Base::s_MngTex.Delete(pTexture); pTexture = NULL;}
		for (int i=0; i < iAnimTexCount; ++i)	{	__ASSERT(pAnimTextures, "pAnimTextures is NULL");
			CN3Base::s_MngTex.Delete(pAnimTextures[i]);		pAnimTextures[i] = NULL;}
	}
	void		GetTextures()
	{
		char szFN[_MAX_PATH] = "";
		if (pTexture == NULL)
		{
			wsprintf(szFN, "Terrain\\River\\%s", pszTexName);
			pTexture = CN3Base::s_MngTex.Get(szFN);
		}
		__ASSERT(pTexture, "pTexture is NULL");
		for (int i=0; i < iAnimTexCount; ++i)	{	__ASSERT(pAnimTextures, "pAnimTextures is NULL");
			__ASSERT(pszAnimTexNames, "pAnimTextures is NULL");
			if (pAnimTextures[i] != NULL) continue;
			wsprintf(szFN, "Terrain\\River\\%s", pszAnimTexNames[i]);
			pAnimTextures[i] = CN3Base::s_MngTex.Get(szFN);
			__ASSERT(pAnimTextures[i], "pAnimTextures[i] is NULL");}
	}
};

#define _LOAD_ALL_RIVERPATCH		// ��� patch�� �Ѳ����� Load�Ͽ� ���� ���� ��������
// �������� ������ �ֺ� 9ĭ�� patch���� ���ϸ� load�Ͽ� ����Ѵ�.

class CN3RiverPatch;
class CN3River : public CN3Base  // CN3RiverPatch�� �����ϴ� Ŭ����
{
public:
	CN3River();
	virtual ~CN3River();

// structures
protected:

// Attributes
public:
	void			SetMaxPatchSize(int iX, int iZ) {m_MaxPatchSize.cx = iX; m_MaxPatchSize.cy = iZ;};
protected:
	int				m_iRiverCount;		// ��� ���� ��
	__RiverInfo*	m_RiverInfos;		// ��� ���� ����
	CN3RiverPatch*	m_pRiverPatches[9];	// �ֺ� 9ĭ�� ��ġ����
	POINT			m_CurPatchPos;		//���� �ִ� ��ġ�� Patch��ǥ
	SIZE			m_MaxPatchSize;		// Patch�� ���� ���� ��� �ִ���..
	CN3LinkedList<struct __RiverEx*> m_RiverList;	// ������ �׷��� �� �ִ� ������ ����Ʈ

#ifndef _LOAD_ALL_RIVERPATCH
	char			m_PatchBaseName[_MAX_PATH];	//Patch������ �⺻���� �̸� (�ǵ��� XXXX�� ��ġ��ǥ�� �� �̸�)
#else
	CN3RiverPatch**	m_pAllRiverPatches;	// �� ���� ��� patch����

public:
	void LoadAllPatches(HANDLE hFile);	// ��� �� ��ġ������ �Ѳ����� Load
#endif

// Operations
public:
	virtual void	Release();
	virtual bool	Load(HANDLE hFile);
	virtual bool	Save(HANDLE hFile);
	void			Render();
	void			Tick();

	__RiverInfo*	CreateRiverInfo(int iRiverCount);	// m_RiverInfos ���� ����
	void			SetPatchPos(int iX, int iZ);		// ���� �ִ� ��ġ patch��ǥ ����
protected:
	__RiverInfo*	GetRiverInfoByID(int iRiverID);	
	void			ReleaseRiverList();					
	void			ReleaseAllRiverPatches();			// RiverPatch�� ��� Release
	CN3RiverPatch*	LoadPatch(int iX, int iZ);			// �ش� ��ġ�� patch�� new�Ͽ� load�Ѵ�.
	void			SortRiver();						// ���� ������ �ٽ� �����Ѵ�.
	static int		CompareVertexRiver( const void *arg1, const void *arg2 );	// __VertexRiver���� �����Ҷ� ���ϴ� �Լ�
};

#endif // !defined(AFX_N3RIVER_H__D0171C53_F631_4EC3_9D42_B4B754093FAC__INCLUDED_)
