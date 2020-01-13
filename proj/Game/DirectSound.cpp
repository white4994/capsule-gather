//
//	DirectSound.cpp
//	�T�E���h�֌W�̎���
//
#include<string>

#include"DirectSound.h"

namespace DirectSound
{
	Audio::Audio()
	{
		//	DirectSound�I�u�W�F�N�g�̍쐬
		DirectSoundCreate8(NULL, &ds8, NULL);

		//	�������x���̐ݒ�
		ds8->SetCooperativeLevel(Win::Window::GetInstance(), DSSCL_NORMAL);

		DSBUFFERDESC desc = {
			sizeof(DSBUFFERDESC),
			DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS,
			size,
			0,
			NULL,
			GUID_NULL
		};

		ds8->CreateSoundBuffer(&desc, &primaryBuf, NULL);
	}

	Audio::~Audio()
	{
		for (auto& it : list)
		{
			while (it.second->Release() != 0);
		}
		RELEASE(primaryBuf);
		RELEASE(ds8);
	}

	/*	WAV�t�@�C���̓ǂݍ���
		����
			fileName	: WAV�t�@�C���̖��O
		��10MB�ȏ�̃t�@�C���͓ǂݍ��܂Ȃ��ł�������
	*/
	IDirectSoundBuffer8* Audio::Load(std::string fileName)
	{
		auto itr = list.find(fileName);
		if (itr!=list.end())
		{
			return itr->second;
		}

		HMMIO hMmio = NULL;
		MMIOINFO mmioInfo = {};

		//	WAV�t�@�C�����̃w�b�_�[���̊m�F�Ɠǂݍ���
		hMmio = mmioOpen((LPSTR)fileName.c_str(), &mmioInfo, MMIO_READ);
		if (!hMmio)
		{
			std::string msg = fileName + "���J���̂Ɏ��s���܂���";
			ERROR_MSG(msg.c_str());
			return NULL;
		}


		MMCKINFO riffckInfo;
		MMCKINFO ckInfo;

		MMRESULT mmRes;
		
		//	RIFF�`�����N�̓ǂݍ���
		riffckInfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		mmRes = mmioDescend(hMmio, &riffckInfo, NULL, MMIO_FINDRIFF);
		if (mmRes != MMSYSERR_NOERROR)
		{
			ERROR_MSG("�ŏ��i�ŏ㕔�j�̃`�����N�ɐi���ł��܂���");
			mmioClose(hMmio, 0);
			return NULL;
		}

		//	WAV�t�@�C�����ǂ����m�F����
		if ((riffckInfo.ckid != mmioFOURCC('R', 'I', 'F', 'F')) || (riffckInfo.fccType != mmioFOURCC('W', 'A', 'V', 'E')))
		{
			ERROR_MSG("�����WAV�t�@�C���ł͂���܂���");
			mmioClose(hMmio, 0);
			return NULL;
		}

		//	�t�@�C���|�C���^�� fmt �`�����N�ɃZ�b�g
		ckInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
		mmRes = mmioDescend(hMmio, &ckInfo, &riffckInfo, MMIO_FINDCHUNK);
		if (mmRes != MMSYSERR_NOERROR)
		{
			ERROR_MSG("fmt �`�����N��������܂���");
			mmioClose(hMmio, 0);
			return NULL;
		}

		WAVEFORMATEX wFmt = {};
		if (mmioRead(hMmio, (HPSTR)&wFmt, ckInfo.cksize) != ckInfo.cksize)
		{
			ERROR_MSG("WAV�t�H�[�}�b�g�̓ǂݍ��݂Ɏ��s���܂���");
			mmioClose(hMmio, 0);
			return NULL;
		}
		mmioAscend(hMmio, &ckInfo, 0);
		

		//	WAV�t�@�C�����̉��f�[�^�̓ǂݍ���
		ckInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
		//	�f�[�^�`�����N�ɃZ�b�g
		mmRes = mmioDescend(hMmio, &ckInfo, &riffckInfo, MMIO_FINDCHUNK);
		if (mmRes != MMSYSERR_NOERROR)
		{
			ERROR_MSG("data�`�����N��������܂���");
			mmioClose(hMmio, 0);
			return NULL;
		}

		BYTE* data = new BYTE[ckInfo.cksize];

		if (mmioRead(hMmio, (HPSTR)data, ckInfo.cksize) != ckInfo.cksize)
		{
			DEL_ARRAY(data);
			return NULL;
		}

		mmioClose(hMmio, 0);

		//	�Z�J���_���o�b�t�@�쐬
		size = ckInfo.cksize;
		DSBUFFERDESC desc = {
			sizeof(DSBUFFERDESC),
			DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS,
			size,
			0,
			&wFmt,
			GUID_NULL
		};

		IDirectSoundBuffer* tmpBuf;
		IDirectSoundBuffer8* dsBuf;
		ds8->CreateSoundBuffer(&desc, &tmpBuf, NULL);
		tmpBuf->QueryInterface(IID_IDirectSoundBuffer8, (void**)&dsBuf);
		RELEASE(tmpBuf);


		//	�Z�J���_���o�b�t�@��WAVE�f�[�^��������
		VOID* buffer = NULL;
		DWORD bufferSize = 0;
		dsBuf->Lock(0, 0, &buffer, &bufferSize, NULL, NULL, DSBLOCK_ENTIREBUFFER);
		memcpy(buffer, data, bufferSize);
		dsBuf->Unlock(buffer, bufferSize, NULL, 0);
		delete[] data;

		list.emplace(fileName, dsBuf);

		return dsBuf;
	}

	/*	�Đ��A��~�Ȃǂ̕ύX���s�����߂̃C���^�[�t�F�C�X��Ԃ�
	����
		fileName	: WAV�t�@�C���̖��O
	*/
	IDirectSoundBuffer8* Audio::GetBuf(std::string fileName)const
	{
		auto itr = list.find(fileName);
		if (itr != list.end())
		{
			return itr->second;
		}
		
		return NULL;
	}
}