//
//	DirectSound.h
//	�T�E���h�֌W�̒�`
//
#ifndef INCLUDE_DirectSound_h_
#define INCLUDE_DirectSound_h_

#pragma once
#include<dsound.h>
#include<mmsystem.h>
#include<unordered_map>

#include"Common.h"
#include"Window.h"

#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"dxguid.lib")

namespace DirectSound
{
	/*	�g�p��
		DirectSound::Audio audio;				//	audio�I�u�W�F�N�g�̍쐬
		audio.Load("boon.wav");					//	�t�@�C���̓ǂݍ���

		auto sound = audio.getBuf("boon.wav");	//	�T�E���h�o�b�t�@�̓ǂݍ���
		sound->Play(0, 0, 0);					//	�P���Đ�
		sound->Play(0, 0, DSBPLAY_LOOPING);		//	���[�v�Đ�
		sound->Stop();							//	��~
		sound->SetCurrentPosition(0);			//	���o��
		sound->SetVolume(-1000);				//	���ʒ��߁i1/1000dB�P�ʁE���̒l�̂݁j
	*/
	class Audio
	{
	public:
		Audio();
		Audio(const Audio&) = delete;
		~Audio();

		/*	WAV�t�@�C���̓ǂݍ���
			����
				fileName	: WAV�t�@�C���̖��O
			��10MB�ȏ�̃t�@�C���͓ǂݍ��܂Ȃ��ł�������
		*/
		IDirectSoundBuffer8* Load(std::string fileName);

		/*	�Đ��A��~�Ȃǂ̕ύX���s�����߂̃C���^�[�t�F�C�X��Ԃ�
			����
				fileName	: WAV�t�@�C���̖��O
		*/
		IDirectSoundBuffer8* GetBuf(std::string fileName) const;

	private:
		std::unordered_map<std::string, IDirectSoundBuffer8*> list;

		IDirectSound8* ds8;
		DWORD size = 0;
		IDirectSoundBuffer* primaryBuf;

	};
}
#endif // !INCLUDE_DirectSound_h_