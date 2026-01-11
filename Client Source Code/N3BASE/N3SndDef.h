////////////////////////////////////////////////////////////////////////////////////////
//
//	N3SndDef.h
//	- �̰����� Sound�� ���õ� �ڷ�������, �������...
//
//	By Donghoon..
//
////////////////////////////////////////////////////////////////////////////////////////

#ifndef __N3SNDDEF_H__
#define __N3SNDDEF_H__

#include "WaveFile.h"
#include <DSound.h>
#include <string>

class CN3SndObj;

typedef struct __SoundSource
{
	int						iID;
	std::string				szFN;
	LPDIRECTSOUNDBUFFER		pDSBuff;
	int						Size;
	int						Type; // 0:2d 1:3d 2: streammin..
	int						Count;
	int						iMaxCount;
	CN3SndObj**				ppObjs;

	__SoundSource()
	{
		iID = 0;
		ppObjs = NULL;
		szFN = "";
		pDSBuff = NULL;
		Size = 0;
		Type = 0;
		Count = 0;
		iMaxCount = 0;
	}	
} SOUNDSOURCE, *LPSOUNDSOURCE;

typedef struct __TABLE_SOUND // Sound ���ҽ� ���ڵ�...
{
	DWORD		dwID;		// ���� ID
	std::string	szFN;		// wave file name
	int			iType;		// ���� Ÿ��...
	int			iNumInst;	// �ִ� ����� �� �ִ� �ν��Ͻ��� ����..
} TABLE_SOUND;


// ���� ������Ʈ Ÿ�� ����..
enum e_SndType { SNDTYPE_2D=0, SNDTYPE_3D, SNDTYPE_STREAM };
enum e_SndState { SNDSTATE_STOP=0, SNDSTATE_DELAY, SNDSTATE_FADEIN, SNDSTATE_PLAY, SNDSTATE_FADEOUT };

#endif	//end of #ifndef __N3SNDDEF_H__
