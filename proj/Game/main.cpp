#include"GameObj.h"


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	//	初期化とウィンドウの作成
	Win::Window::GetInstance();
	//	描画するためにインスタンスの取得
	auto& d = D3D9::Device::GetInstance(Win::Window::GetInstance(),false);

	if (!SceneManager::GetInstance())
	{
		//	シーン管理の作成
		SceneManager::Create();
	}

	//	ゲームループ
	while (Win::Window::GetInstance().ProcessMessage())
	{
		//	シーンごとの更新
		SceneManager::GetInstance()->Update();

		d.Begin();
		{
			//	シーンごとの描画
			SceneManager::GetInstance()->Draw();

		}
		d.End();
	}
	//	シーン管理の破棄
	SceneManager::Destroy();
	return 0;
}