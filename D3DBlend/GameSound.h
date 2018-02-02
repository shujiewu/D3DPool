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

	HRESULT result;      //用来接受建立后的返回值
	LPDIRECTSOUND pDS;   //代表声卡的DirectSound对象
	LPDIRECTSOUNDBUFFER pMainBuf;  //声明主缓冲区指针
	DSBUFFERDESC desc;             //声明描述结构，用来初始化缓冲区域
	WAVEFORMATEX pwfmt;            //声明声音结构，用来设定播放格式

	WAVEFORMATEX swfmt;    //声明声音结构
	MMCKINFO     ckRiff;   //RIFF区块的信息
	MMCKINFO     ckInfo;   //子区块的信息
	MMRESULT     mmresult; //返回的结果
	DWORD        size;     //实际资料的大小
	HMMIO        hbackground;    //打开的多媒体文件
public:
	GameSound();
	void GameSoundInit(HWND hwnd);             //GameSound对象的建立
	void GameSoundbufferConstruct();      //缓冲区的创建
	void GameSoundfmtSet(int, int, int);  //通过主缓冲区指针来设置播放格式
	void GameSoundReadWAVfile(char*, HMMIO&);//将声音文件读入并将明细存在HMMIO结构中
	void GameSoundReadinbuffer(LPDIRECTSOUNDBUFFER&, char*);//将声音文件读入次缓冲区中
	void playsound(SoundState state);

	LPDIRECTSOUNDBUFFER pHitballmusic;  //声明子缓冲区指针
	LPDIRECTSOUNDBUFFER pHitbarmusic;  //声明子缓冲区指针
	LPDIRECTSOUNDBUFFER pPocketWarmusic;  //声明子缓冲区指针
	LPDIRECTSOUNDBUFFER pTuckmusic;  //声明子缓冲区指针


	void GameSoundAllstop();  //for背景音乐，让背景音乐更换时，先前的所有音乐都停止，从而播放新的音乐
	void GameMusicplay(LPDIRECTSOUNDBUFFER&);     //用来播放循环音乐
	void GameSoundplay(LPDIRECTSOUNDBUFFER&);     //用来播放一次性音效
};

#endif