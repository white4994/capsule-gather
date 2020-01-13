//
//	Window.cpp
//	ウィンドウ管理の実装
//

#include<Windows.h>

#include"Window.h"

namespace Win
{

	/*	メッセージ処理用コールバック関数
		引数
			msg		: メッセージ
			wParam	: メッセージの最初のパラメータ
			lParam	: メッセージの2番目のパラメータ
		戻り値
			メッセージの処理結果
	*/
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CLOSE:			//	ウィンドウが閉じられた
			PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)	//	ESCキーが押された
			{
				PostQuitMessage(0);
			}
			break;
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	/*	ウィンドウ作成
		引数
			width			: クライアント領域のデフォルトの幅
			height			: クライアント領域のデフォルトの高さ
			clsName			: クラス名
			winName			: ウィンドウ名
	*/
	Window::Window(int width, int height, const char* clsName, const char* winName)
	{
		if (hWnd != NULL)
		{
			return;
		}

		HINSTANCE hInstance = GetModuleHandle(NULL);

		//	ウィンドウクラスの初期化
		WNDCLASSEX wc = {};
		if (GetClassInfoEx(hInstance, clsName, &wc) == 0)
		{
			wc.cbSize = sizeof(WNDCLASSEX);				//	この構造体のサイズ
			wc.style = CS_HREDRAW | CS_VREDRAW;			//	ウィンドウのスタイル
			wc.lpfnWndProc = WndProc;					//	メッセージ処理関数の登録
			wc.cbClsExtra = 0;							//	通常使わないので常に0
			wc.cbWndExtra = 0;							//	通常使わないので常に0
			wc.hInstance = hInstance;					//	インスタンスハンドル
			wc.hIcon = NULL;							//	アイコンなし
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);	//	カーソルの形
			wc.hbrBackground = NULL;					//	背景なし
			wc.lpszMenuName = NULL;						//	メニューなし
			wc.lpszClassName = clsName;					//	クラス名
			wc.hIconSm = NULL;							//	小アイコン

			//	ウィンドウクラスの登録
			if (RegisterClassEx(&wc) == 0)
			{
				ERROR_MSG("ウィンドウクラスの登録に失敗しました");
				return;
			}
		}



		RECT rect = { 0,0,width,height };
		AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_LEFT);
		//	ウィンドウの作成
		hWnd = CreateWindow(
			clsName,										//	クラス名
			winName,										//	ウィンドウ名
			WS_OVERLAPPEDWINDOW,							//	ウィンドウスタイル
			CW_USEDEFAULT, CW_USEDEFAULT,					//	ウィンドウの位置
			rect.right - rect.left, rect.bottom - rect.top,	//	ウィンドウの幅、高さ
			NULL,											//	親ウィンドウのハンドル
			NULL,											//	メニューや子ウィンドウのハンドル
			hInstance,										//	アプリケーションインスタンスハンドル
			NULL);											//	ウィンドウの作成データ

		if (hWnd == 0)
		{
			ERROR_MSG("ウィンドウの作成に失敗しました");
			return;
		}

		//	ウィンドウの表示
		ShowWindow(hWnd, SW_SHOW);
		//	WM_PAINTが呼ばれないようにする
		ValidateRect(hWnd, 0);
	}

	//	PostQuitMessage()がよばれるまでtrueを返し続ける
	bool Window::ProcessMessage()
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)	//	PostQuitMessage()が呼ばれた
			{
				return false;
			}
			//	メッセージ翻訳とディスパッチ
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}

		return true;
	}
}