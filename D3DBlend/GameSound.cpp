#include "GameSound.h"

GameSound::GameSound()
{
}

void GameSound::GameSoundInit(HWND hwnd)
{
	this->pDS;
	this->result = DirectSoundCreate(NULL, &pDS, NULL);
	if (this->result != DS_OK)
		::MessageBox(0, L"建立 DirectSound 对象失败！", NULL, MB_OK);
		//::MessageBox(hwnd,L"建立 DirectSound 对象失败！", NULL, MB_OK);

	this->result = this->pDS->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (this->result != DS_OK)
		::MessageBox(0, L"设定程序协调层级失败！", NULL, MB_OK);
		//MessageBox(hwnd, L"设定程序协调层级失败！", NULL, MB_OK);

	this->GameSoundbufferConstruct();
}

void GameSound::GameSoundbufferConstruct()
{

	memset(&this->desc, 0, sizeof(desc));   //清空结构内容
	desc.dwSize = sizeof(desc);             //配制描述结构大小
	desc.dwFlags = DSBCAPS_PRIMARYBUFFER;   //???
	desc.dwBufferBytes = 0;
	desc.lpwfxFormat = NULL;
	result = pDS->CreateSoundBuffer(&desc, &this->pMainBuf, NULL);
	if (this->result != DS_OK)
		//MessageBox(this->soundhwnd, L"建立主缓冲区域失败！", NULL, MB_OK);
		::MessageBox(0, L"建立主缓冲区域失败！", NULL, MB_OK);

	this->GameSoundReadinbuffer(this->pHitballmusic, "sound//hitball.wav");
	this->GameSoundReadinbuffer(this->pHitbarmusic, "sound//hitbar.wav");
	this->GameSoundReadinbuffer(this->pPocketWarmusic, "sound//pocket.wav");
	this->GameSoundReadinbuffer(this->pTuckmusic, "sound//tuck.wav");


}

void GameSound::GameSoundfmtSet(int channels, int SamplesPerSec, int wBitPerSample)
{
	memset(&this->pwfmt, 0, sizeof(pwfmt));
	this->pwfmt.wFormatTag = WAVE_FORMAT_PCM;
	this->pwfmt.nChannels = channels;
	this->pwfmt.nSamplesPerSec = SamplesPerSec;
	this->pwfmt.wBitsPerSample = wBitPerSample;
	this->pwfmt.nBlockAlign = this->pwfmt.wBitsPerSample / 8 * this->pwfmt.nChannels;
	this->pwfmt.nAvgBytesPerSec = this->pwfmt.nSamplesPerSec * this->pwfmt.nBlockAlign;
	this->result = this->pMainBuf->SetFormat(&this->pwfmt);
	if (this->result != DS_OK)
		//MessageBox(this->soundhwnd, L"设定播放格式失败！", NULL, MB_OK);
		::MessageBox(0, L"设定播放格式失败！", NULL, MB_OK);
}
LPWSTR ConvertCharToLPWSTR(char * szString)
{
	int dwLen = strlen(szString) + 1;
	int nwLen = MultiByteToWideChar(CP_ACP, 0, szString, dwLen, NULL, 0);//算出合适的长度
	LPWSTR lpszPath = new WCHAR[dwLen];
	MultiByteToWideChar(CP_ACP, 0, szString, dwLen, lpszPath, nwLen);
	return lpszPath;
}
void GameSound::GameSoundReadWAVfile(char* filename, HMMIO &hmmbackground)
{
	LPWSTR A = ConvertCharToLPWSTR(filename);
	hmmbackground = mmioOpen(A, NULL, MMIO_ALLOCBUF | MMIO_READ);  //打开文件
	if (hmmbackground == NULL)
		//::MessageBox(this->soundhwnd, L"文件不存在！", NULL, MB_OK);
		::MessageBox(0, L"文件不存在！", NULL, MB_OK);
	//搜索类型
	ckRiff.fccType = mmioFOURCC('W', 'A', 'V', 'E');//设定文件类型
	mmresult = mmioDescend(hmmbackground, &ckRiff, NULL, MMIO_FINDRIFF);
	if (mmresult != MMSYSERR_NOERROR)
		//MessageBox(this->soundhwnd, L"文件格式错误！", NULL, MB_OK);
		::MessageBox(0, L"文件格式错误！", NULL, MB_OK);
	//搜索区块
	ckInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');//设定区块类型
	mmresult = mmioDescend(hmmbackground, &ckInfo, &ckRiff, MMIO_FINDCHUNK);
	if (mmresult != MMSYSERR_NOERROR)
		::MessageBox(0, L"文件格式错误！", NULL, MB_OK);
		//MessageBox(this->soundhwnd, L"文件格式错误！", NULL, MB_OK);
	if (mmioRead(hmmbackground, (HPSTR)&swfmt, sizeof(swfmt)) == -1)
		::MessageBox(0, L"读取格式失败！", NULL, MB_OK);
		//MessageBox(this->soundhwnd, L"读取格式失败！", NULL, MB_OK);

	mmresult = mmioAscend(hmmbackground, &ckInfo, 0);   //跳出子区块

	//搜索区块
	ckInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmresult = mmioDescend(hmmbackground, &ckInfo, &ckRiff, MMIO_FINDCHUNK);
	if (mmresult != MMSYSERR_NOERROR)
		//MessageBox(this->soundhwnd, L"文件格式错误!", NULL, MB_OK);
		::MessageBox(0, L"文件格式错误!", NULL, MB_OK);
	size = ckInfo.cksize;

}

void GameSound::GameSoundReadinbuffer(LPDIRECTSOUNDBUFFER& buffer, char* filename)
{
	LPVOID pAudio;
	DWORD bytesAudio;


	this->GameSoundReadWAVfile(filename, this->hbackground);

	memset(&this->desc, 0, sizeof(desc));   //清空结构内容
	desc.dwSize = sizeof(desc);             //配制描述结构大小
	desc.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRLPAN |
		DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS;   //???
	desc.dwBufferBytes = this->size;
	desc.lpwfxFormat = &this->swfmt;
	result = pDS->CreateSoundBuffer(&desc, &buffer, NULL);
	if (this->result != DS_OK)
		::MessageBox(0, L"建立次缓冲区域失败！", NULL, MB_OK);



	result = buffer->Lock(0, this->size, &pAudio, &bytesAudio, NULL, NULL, NULL);
	if (this->result != DS_OK)
		::MessageBox(0, L"锁定缓冲区失败!", NULL, MB_OK);

	this->mmresult = mmioRead(this->hbackground, (HPSTR)pAudio, bytesAudio);

	if (mmresult == -1)
		::MessageBox(0, L"读取声音文件资料失败", NULL, MB_OK);

	this->result = buffer->Unlock(pAudio, bytesAudio, NULL, NULL);

	if (this->result != DS_OK)
		::MessageBox(0, L"解除锁定缓冲区失败!", NULL, MB_OK);

	mmioClose(this->hbackground, 0);
}

void GameSound::GameSoundAllstop()
{
}

void GameSound::GameMusicplay(LPDIRECTSOUNDBUFFER& buffer)
{
	this->GameSoundAllstop();

	buffer->Play(0, 0, 1);
}
void GameSound::playsound(SoundState state)
{
	switch (state)
	{
	case hitball:
		GameSoundplay(pHitballmusic);
		break;
	case hitbar:
		GameSoundplay(pHitbarmusic);
		break;
	case pocket:
		GameSoundplay(pPocketWarmusic);
		break;
	case tuck:
		GameSoundplay(pTuckmusic);
		break;
	}
	
}
void GameSound::GameSoundplay(LPDIRECTSOUNDBUFFER& buffer)
{
	buffer->Play(0, 0, 0);
}