//
//	Window.h
//	ウィンドウ管理の定義
//

#ifndef _INCLUDE_Window_h_
#define _INCLUDE_Window_h_


#pragma once
#include"Common.h"

namespace Win
{
	class Window
	{
	public:

		//	PostQuitMessageがよばれるまでtrueを返し続ける
		bool ProcessMessage();

		// シングルトンゲッター
		/*	インスタンスの取得
			引数
				width			: クライアント領域のデフォルトの幅
				height			: クライアント領域のデフォルトの高さ
				clsName			: クラス名
				winName			: ウィンドウ名
			初回呼び出し時は初期化を兼ねているので変更する場合はその際行うこと
		*/
		static Window& GetInstance(int width = 680, int height = 480, const char* clsName = "defalt", const char* winName = "defalt")
		{
			static Window win(width, height, clsName, winName);
			return win;
		}

		//	キャスト演算子オーバーロード
		operator HWND() const { return hWnd; };

		//	アロー演算子オーバーロード
		HWND operator->() const { return hWnd; };

	private:

		HWND hWnd = 0;

		/*	ウィンドウ作成
			引数
				width			: クライアント領域のデフォルトの幅
				height			: クライアント領域のデフォルトの高さ
				clsName			: クラス名
				winName			: ウィンドウ名
		*/
		Window(int width, int height, const char* clsName, const char* winName);

		Window(const Window&) = delete;
	};
}
#endif // !_INCLUDE_Window_h_