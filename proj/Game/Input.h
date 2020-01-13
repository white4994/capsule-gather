//
//	Input.h
//	キーボード入力、マウスの取得の定義
//

#ifndef _INCLUDE_Input_h_
#define	_INCLUDE_Input_h_


#pragma once
#include"Common.h"

namespace Input
{
	//	キーボード
	class Keyboard
	{
	public:
		Keyboard();

		//	入力の更新
		void Update();

		/*	キーを押し続けている
			引数
				nVirtKey	: キーの種類
		*/
		bool On(int nVirtKey);

		/*	キーが押された瞬間
			引数
				nVirtKey	: キーの種類
		*/
		bool Press(int nVirtKey);

		/*	キーが離された瞬間
			引数
				nVirtKey	: キーの種類
		*/
		bool Release(int nVirtKey);

	private:
		//	初期化
		void Reset();

	};

	/*	マウス
		オブジェクト作成はループ内で行うこと
		ボタンの取得はキーボードクラスを使うこと
	*/
	class Mouse
	{
	public:

		Mouse();

		//	ゲッター

		//	マウスの二次元 X 座標
		float GetX() const
		{
			return posX;
		}

		//	マウスの二次元 Y 座標
		float GetY() const
		{
			return posY;
		}

	private:
		//	マウスの二次元座標
		float posX, posY;

		//マウスの座標取得
		void GetPos();

	};
}
#endif // !_INCLUDE_Input_h_