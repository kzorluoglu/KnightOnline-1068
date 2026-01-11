//////////////////////////////////////////////////
//	Commented By : Lynus
//	Commented On 2001-04-12 ���� 5:09:28
//
//	CStreamSoundObj class (SoundObj.h)
//
//	End Of Comment (By Lynus On 2001-04-12 ���� 5:09:28 )
//////////////////////////////////////////////////

#ifndef __STREAMSOUNDOBJ_H_
#define __STREAMSOUNDOBJ_H_

#include "N3SndObj.h"

class CN3SndEng;

class CStreamSoundObj : public CN3SndObj
{
	private:
		int					m_PlayTime;		//wave������ ��ü �÷��� Ÿ��.
		int					m_FinalByte;
				
		DSBUFFERDESC		m_dsbd;
		DWORD				m_BufferSize;
		DWORD				m_BlockSize;	//buffer�� �ʴ����� ���� ��.
		
		int					m_CurrState;	//���� ��� ������ �ִ���..
		int					m_PrevState;	//���� �ٷ� ���� ��� ������ �־�����..
		DWORD				m_CurrPos;		//���� ���ۿ����� ��ġ 
		int					m_PastTime;		//��ü �÷��� Ÿ�ӿ��� ���� �ð�

		//related wave data
		WAVEFORMATEX		m_WaveFormat;
		DWORD				m_WaveSize;
		LPSTR				m_pWaveMem;
		HGLOBAL				m_hWaveMem;
		MMCKINFO			mmCkInfoRIFF;
		MMCKINFO			mmCkInfoChunk;
		HMMIO				hMMIO;
		
	public:
		BOOL Create(CN3SndEng* pEng);

//		void Play(float delay = 0.0f, float fFadeInTime = 0.0f);
//		void Stop(float fFadeOutTime = 0.0f);		
		
		void Tick();		
		void Release();
		
		CStreamSoundObj();
		virtual ~CStreamSoundObj();
				
	private:
		void RealPlay();
		
		//void Stop();

		BOOL LoadWave(LPCSTR pFileName);
		BOOL WriteBuffer();
		BOOL InitWriteBuffer();
		void Reset();
};

#endif // #ifndef __STREAMSOUNDOBJ_H_
