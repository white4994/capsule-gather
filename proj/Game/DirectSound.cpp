//
//	DirectSound.cpp
//	サウンド関係の実装
//
#include<string>

#include"DirectSound.h"

namespace DirectSound
{
	Audio::Audio()
	{
		//	DirectSoundオブジェクトの作成
		DirectSoundCreate8(NULL, &ds8, NULL);

		//	協調レベルの設定
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

	/*	WAVファイルの読み込み
		引数
			fileName	: WAVファイルの名前
		※10MB以上のファイルは読み込まないでください
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

		//	WAVファイル内のヘッダー情報の確認と読み込み
		hMmio = mmioOpen((LPSTR)fileName.c_str(), &mmioInfo, MMIO_READ);
		if (!hMmio)
		{
			std::string msg = fileName + "を開くのに失敗しました";
			ERROR_MSG(msg.c_str());
			return NULL;
		}


		MMCKINFO riffckInfo;
		MMCKINFO ckInfo;

		MMRESULT mmRes;
		
		//	RIFFチャンクの読み込み
		riffckInfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		mmRes = mmioDescend(hMmio, &riffckInfo, NULL, MMIO_FINDRIFF);
		if (mmRes != MMSYSERR_NOERROR)
		{
			ERROR_MSG("最初（最上部）のチャンクに進入できません");
			mmioClose(hMmio, 0);
			return NULL;
		}

		//	WAVファイルかどうか確認する
		if ((riffckInfo.ckid != mmioFOURCC('R', 'I', 'F', 'F')) || (riffckInfo.fccType != mmioFOURCC('W', 'A', 'V', 'E')))
		{
			ERROR_MSG("これはWAVファイルではありません");
			mmioClose(hMmio, 0);
			return NULL;
		}

		//	ファイルポインタを fmt チャンクにセット
		ckInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
		mmRes = mmioDescend(hMmio, &ckInfo, &riffckInfo, MMIO_FINDCHUNK);
		if (mmRes != MMSYSERR_NOERROR)
		{
			ERROR_MSG("fmt チャンクが見つかりません");
			mmioClose(hMmio, 0);
			return NULL;
		}

		WAVEFORMATEX wFmt = {};
		if (mmioRead(hMmio, (HPSTR)&wFmt, ckInfo.cksize) != ckInfo.cksize)
		{
			ERROR_MSG("WAVフォーマットの読み込みに失敗しました");
			mmioClose(hMmio, 0);
			return NULL;
		}
		mmioAscend(hMmio, &ckInfo, 0);
		

		//	WAVファイル内の音データの読み込み
		ckInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
		//	データチャンクにセット
		mmRes = mmioDescend(hMmio, &ckInfo, &riffckInfo, MMIO_FINDCHUNK);
		if (mmRes != MMSYSERR_NOERROR)
		{
			ERROR_MSG("dataチャンクが見つかりません");
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

		//	セカンダリバッファ作成
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


		//	セカンダリバッファにWAVEデータ書き込み
		VOID* buffer = NULL;
		DWORD bufferSize = 0;
		dsBuf->Lock(0, 0, &buffer, &bufferSize, NULL, NULL, DSBLOCK_ENTIREBUFFER);
		memcpy(buffer, data, bufferSize);
		dsBuf->Unlock(buffer, bufferSize, NULL, 0);
		delete[] data;

		list.emplace(fileName, dsBuf);

		return dsBuf;
	}

	/*	再生、停止などの変更を行うためのインターフェイスを返す
	引数
		fileName	: WAVファイルの名前
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