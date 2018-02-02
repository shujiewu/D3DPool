#ifndef GAMESOUND
#define GAMESOUND
#include "dsound.h"
#include "windows.h"
#include "mmsystem.h"
#include "mmreg.h"
#pragma comment(lib,"Dsound.lib")
#pragma comment(lib,"Dxguid.lib")
#pragma comment(lib,"Winmm.lib")
enum SoundState
{
	hitball,
	hitbar,
	pocket,
	tuck
};
class GameSound
{
private:
	HWND soundhwnd;

	HRESULT result;      //�������ܽ�����ķ���ֵ
	LPDIRECTSOUND pDS;   //����������DirectSound����
	LPDIRECTSOUNDBUFFER pMainBuf;  //������������ָ��
	DSBUFFERDESC desc;             //���������ṹ��������ʼ����������
	WAVEFORMATEX pwfmt;            //���������ṹ�������趨���Ÿ�ʽ

	WAVEFORMATEX swfmt;    //���������ṹ
	MMCKINFO     ckRiff;   //RIFF�������Ϣ
	MMCKINFO     ckInfo;   //���������Ϣ
	MMRESULT     mmresult; //���صĽ��
	DWORD        size;     //ʵ�����ϵĴ�С
	HMMIO        hbackground;    //�򿪵Ķ�ý���ļ�
public:
	GameSound();
	void GameSoundInit(HWND hwnd);             //GameSound����Ľ���
	void GameSoundbufferConstruct();      //�������Ĵ���
	void GameSoundfmtSet(int, int, int);  //ͨ����������ָ�������ò��Ÿ�ʽ
	void GameSoundReadWAVfile(char*, HMMIO&);//�������ļ����벢����ϸ����HMMIO�ṹ��
	void GameSoundReadinbuffer(LPDIRECTSOUNDBUFFER&, char*);//�������ļ�����λ�������
	void playsound(SoundState state);

	LPDIRECTSOUNDBUFFER pHitballmusic;  //�����ӻ�����ָ��
	LPDIRECTSOUNDBUFFER pHitbarmusic;  //�����ӻ�����ָ��
	LPDIRECTSOUNDBUFFER pPocketWarmusic;  //�����ӻ�����ָ��
	LPDIRECTSOUNDBUFFER pTuckmusic;  //�����ӻ�����ָ��


	void GameSoundAllstop();  //for�������֣��ñ������ָ���ʱ����ǰ���������ֶ�ֹͣ���Ӷ������µ�����
	void GameMusicplay(LPDIRECTSOUNDBUFFER&);     //��������ѭ������
	void GameSoundplay(LPDIRECTSOUNDBUFFER&);     //��������һ������Ч
};

#endif