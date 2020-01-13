//
//	D3D9.h
//	Direct3Dの作成、描画などの定義
//

#ifndef _INCLUDE_D3D9_h_
#define _INCLUDE_D3D9_h_


#pragma once
#include<atlbase.h>

#include"Common.h"
#include"Window.h"

namespace D3D9
{

	class Device
	{
	public:

		//	シングルトンゲッター
		//	インスタンスの取得	初期化を兼ねているので最初の生成時は引数HWNDは必ず入れること
		//	winModeはウィンドウモードの選択　trueでフルスクリーン
		static Device& GetInstance(HWND hWnd = NULL,bool winMode=false);

		/*	描画開始
			引数
				color	: 画面クリアの際に塗りつぶす色{ r, g, b, a }
		*/
		void Begin(D3DXCOLOR color = { 0.1f,0.3f,0.7f,1.f });

		//	描画終了
		void End(HWND hWnd = NULL);

		//	キャスト演算子オーバーロード
		operator LPDIRECT3DDEVICE9() const { return dev; };

		//	アロー演算子オーバーロード
		LPDIRECT3DDEVICE9 operator->() const { return dev; };

	private:

		CComPtr<IDirect3D9> d3d9;
		CComPtr<IDirect3DDevice9> dev;

		/*	初期化
			引数
				hWnd	: ウィンドウハンドル
		*/
		Device(HWND hWnd = NULL, bool winMode = false);

		Device(const Device&) = delete;
	};
}
#endif // !_INCLUDE_D3D)_h_