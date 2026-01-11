//////////////////////////////////////////////////////////////////////////////////////
//
//	N3FXDef.h
//	Effect���� ���̴� ������̳� �ڷ����� ����...
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __N3FXDEF_H__
#define __N3FXDEF_H__

#include "N3Base.h"
#include "My_3DStruct.h"

const int MAX_FX_PART = 16;			//�� �ν��Ͻ��� ���ÿ� ǥ���Ҽ� �ִ� ����..
const int NUM_VERTEX_PARTICLE = 4;	//��ƼŬ �ϳ��� ���� ����..
const int NUM_VERTEX_BOTTOM = 10;	//
const int NUM_KEY_COLOR = 100;

enum e_FXPartType	//����Ʈ ��Ÿ��...�Ž��� �̿��� ����, ��ƼŬ�� �̿��� ����..���..
{
	FX_PART_TYPE_NONE = 0,
	FX_PART_TYPE_PARTICLE = 1,		//'particle'
	FX_PART_TYPE_BOARD = 2,			//'board'
	FX_PART_TYPE_MESH = 3,			//'mesh'
	FX_PART_TYPE_BOTTOMBOARD = 4	//'bottomboard'
};

enum e_FXPartState	//��Ʈ�� ����..
{
	FX_PART_STATE_DEAD = 0,
	FX_PART_STATE_DYING = 1,
	FX_PART_STATE_LIVE = 2,
	FX_PART_STATE_READY = 3
};

enum e_FXBundleState	//������ ����..
{
	FX_BUNDLE_STATE_DEAD = 0,
	FX_BUNDLE_STATE_DYING = 1,
	FX_BUNDLE_STATE_LIVE = 2	
};

enum e_FXBundleAct	//������ ���� �����ϴ���..
{
	FX_BUNDLE_MOVE_DIR_FIXEDTARGET = 0,
	FX_BUNDLE_MOVE_DIR_FLEXABLETARGET = 1,
	FX_BUNDLE_MOVE_DIR_FLEXABLETARGET_RATIO = 2,
	FX_BUNDLE_MOVE_CURVE_FIXEDTARGET = 3,	
	FX_BUNDLE_MOVE_DIR_SLOW = 4,
	FX_BUNDLE_REGION_POISON = 5,
	FX_BUNDLE_MOVE_NONE = 0xffffffff	
};

//
enum e_FXPartParticleEmitType	// ����Ʈ ��Ʈ�� � ������� �����Ǵ���...
{
	FX_PART_PARTICLE_EMIT_TYPE_NORMAL = 0,	//'normal'
	FX_PART_PARTICLE_EMIT_TYPE_SPREAD = 1,	//'spread'
	FX_PART_PARTICLE_EMIT_TYPE_GATHER = 2,	//'gather'	
};

//
/////////////////////////////////////////////////////////////////
//structures.....

/*
typedef struct __TABLE_FX	// FX ���ҽ� ���ڵ�...
{
	DWORD		dwID;		// ���� ID
	std::string	szFN;		// file name
	DWORD		dwSoundID;	// ȿ���� ���� ���� �Ƶ�.
} TABLE_FX;
*/

typedef struct Point3D
{
	float x;
	float y;
	float z;
} POINT3D;

typedef union __ParticleEmitCondition	//��ƼŬ �л�� �ʿ�����..
{
	POINT3D	vGatherPoint;	//EmitType�� gather�϶� ������� ��..
	float	fEmitAngle;		//EmitType�� spread�϶� �ѷ����� ��..
} PARTICLEEMITCONDITION;

typedef struct __FXPartWithStartTime	// ���鿡�� ��Ʈ�� �����Ҷ�..
{
	class CN3FXPartBase*	pPart;
	float					fStartTime;
	
	__FXPartWithStartTime()
	{
		pPart = NULL;
		fStartTime = 0.0f;
	}
} FXPARTWITHSTARTTIME, *LPFXPARTWITHSTARTTIME;

typedef struct __FXBInfo
{
	char	FXBName[MAX_PATH];
	int		joint;
	BOOL	IsLooping;

	__FXBInfo()
	{
		ZeroMemory(FXBName, MAX_PATH);
		joint = -1;
		IsLooping = FALSE;
	}	
} FXBINFO, *LPFXBINFO;

#endif // #ifndef __N3FXDEF_H__
