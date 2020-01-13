//
//	DirectSound.h
//	サウンド関係の定義
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
	/*	使用例
		DirectSound::Audio audio;				//	audioオブジェクトの作成
		audio.Load("boon.wav");					//	ファイルの読み込み

		auto sound = audio.getBuf("boon.wav");	//	サウンドバッファの読み込み
		sound->Play(0, 0, 0);					//	単純再生
		sound->Play(0, 0, DSBPLAY_LOOPING);		//	ループ再生
		sound->Stop();							//	停止
		sound->SetCurrentPosition(0);			//	頭出し
		sound->SetVolume(-1000);				//	音量調節（1/1000dB単位・負の値のみ）
	*/
	class Audio
	{
	public:
		Audio();
		Audio(const Audio&) = delete;
		~Audio();

		/*	WAVファイルの読み込み
			引数
				fileName	: WAVファイルの名前
			※10MB以上のファイルは読み込まないでください
		*/
		IDirectSoundBuffer8* Load(std::string fileName);

		/*	再生、停止などの変更を行うためのインターフェイスを返す
			引数
				fileName	: WAVファイルの名前
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