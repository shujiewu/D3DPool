#include "GameSound.h"

GameSound::GameSound()
{
}

void GameSound::GameSoundInit(HWND hwnd)
{
	this->pDS;
	this->result = DirectSoundCreate(NULL, &pDS, NULL);
	if (this->result != DS_OK)
		::MessageBox(0, L"���� DirectSound ����ʧ�ܣ�", NULL, MB_OK);
		//::MessageBox(hwnd,L"���� DirectSound ����ʧ�ܣ�", NULL, MB_OK);

	this->result = this->pDS->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (this->result != DS_OK)
		::MessageBox(0, L"�趨����Э���㼶ʧ�ܣ�", NULL, MB_OK);
		//MessageBox(hwnd, L"�趨����Э���㼶ʧ�ܣ�", NULL, MB_OK);

	this->GameSoundbufferConstruct();
}

void GameSound::GameSoundbufferConstruct()
{

	memset(&this->desc, 0, sizeof(desc));   //��սṹ����
	desc.dwSize = sizeof(desc);             //���������ṹ��С
	desc.dwFlags = DSBCAPS_PRIMARYBUFFER;   //???
	desc.dwBufferBytes = 0;
	desc.lpwfxFormat = NULL;
	result = pDS->CreateSoundBuffer(&desc, &this->pMainBuf, NULL);
	if (this->result != DS_OK)
		//MessageBox(this->soundhwnd, L"��������������ʧ�ܣ�", NULL, MB_OK);
		::MessageBox(0, L"��������������ʧ�ܣ�", NULL, MB_OK);

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
		//MessageBox(this->soundhwnd, L"�趨���Ÿ�ʽʧ�ܣ�", NULL, MB_OK);
		::MessageBox(0, L"�趨���Ÿ�ʽʧ�ܣ�", NULL, MB_OK);
}
LPWSTR ConvertCharToLPWSTR(char * szString)
{
	int dwLen = strlen(szString) + 1;
	int nwLen = MultiByteToWideChar(CP_ACP, 0, szString, dwLen, NULL, 0);//������ʵĳ���
	LPWSTR lpszPath = new WCHAR[dwLen];
	MultiByteToWideChar(CP_ACP, 0, szString, dwLen, lpszPath, nwLen);
	return lpszPath;
}
void GameSound::GameSoundReadWAVfile(char* filename, HMMIO &hmmbackground)
{
	LPWSTR A = ConvertCharToLPWSTR(filename);
	hmmbackground = mmioOpen(A, NULL, MMIO_ALLOCBUF | MMIO_READ);  //���ļ�
	if (hmmbackground == NULL)
		//::MessageBox(this->soundhwnd, L"�ļ������ڣ�", NULL, MB_OK);
		::MessageBox(0, L"�ļ������ڣ�", NULL, MB_OK);
	//��������
	ckRiff.fccType = mmioFOURCC('W', 'A', 'V', 'E');//�趨�ļ�����
	mmresult = mmioDescend(hmmbackground, &ckRiff, NULL, MMIO_FINDRIFF);
	if (mmresult != MMSYSERR_NOERROR)
		//MessageBox(this->soundhwnd, L"�ļ���ʽ����", NULL, MB_OK);
		::MessageBox(0, L"�ļ���ʽ����", NULL, MB_OK);
	//��������
	ckInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');//�趨��������
	mmresult = mmioDescend(hmmbackground, &ckInfo, &ckRiff, MMIO_FINDCHUNK);
	if (mmresult != MMSYSERR_NOERROR)
		::MessageBox(0, L"�ļ���ʽ����", NULL, MB_OK);
		//MessageBox(this->soundhwnd, L"�ļ���ʽ����", NULL, MB_OK);
	if (mmioRead(hmmbackground, (HPSTR)&swfmt, sizeof(swfmt)) == -1)
		::MessageBox(0, L"��ȡ��ʽʧ�ܣ�", NULL, MB_OK);
		//MessageBox(this->soundhwnd, L"��ȡ��ʽʧ�ܣ�", NULL, MB_OK);

	mmresult = mmioAscend(hmmbackground, &ckInfo, 0);   //����������

	//��������
	ckInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmresult = mmioDescend(hmmbackground, &ckInfo, &ckRiff, MMIO_FINDCHUNK);
	if (mmresult != MMSYSERR_NOERROR)
		//MessageBox(this->soundhwnd, L"�ļ���ʽ����!", NULL, MB_OK);
		::MessageBox(0, L"�ļ���ʽ����!", NULL, MB_OK);
	size = ckInfo.cksize;

}

void GameSound::GameSoundReadinbuffer(LPDIRECTSOUNDBUFFER& buffer, char* filename)
{
	LPVOID pAudio;
	DWORD bytesAudio;


	this->GameSoundReadWAVfile(filename, this->hbackground);

	memset(&this->desc, 0, sizeof(desc));   //��սṹ����
	desc.dwSize = sizeof(desc);             //���������ṹ��С
	desc.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRLPAN |
		DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS;   //???
	desc.dwBufferBytes = this->size;
	desc.lpwfxFormat = &this->swfmt;
	result = pDS->CreateSoundBuffer(&desc, &buffer, NULL);
	if (this->result != DS_OK)
		::MessageBox(0, L"�����λ�������ʧ�ܣ�", NULL, MB_OK);



	result = buffer->Lock(0, this->size, &pAudio, &bytesAudio, NULL, NULL, NULL);
	if (this->result != DS_OK)
		::MessageBox(0, L"����������ʧ��!", NULL, MB_OK);

	this->mmresult = mmioRead(this->hbackground, (HPSTR)pAudio, bytesAudio);

	if (mmresult == -1)
		::MessageBox(0, L"��ȡ�����ļ�����ʧ��", NULL, MB_OK);

	this->result = buffer->Unlock(pAudio, bytesAudio, NULL, NULL);

	if (this->result != DS_OK)
		::MessageBox(0, L"�������������ʧ��!", NULL, MB_OK);

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