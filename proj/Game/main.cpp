#include"GameObj.h"


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	//	�������ƃE�B���h�E�̍쐬
	Win::Window::GetInstance();
	//	�`�悷�邽�߂ɃC���X�^���X�̎擾
	auto& d = D3D9::Device::GetInstance(Win::Window::GetInstance(),false);

	if (!SceneManager::GetInstance())
	{
		//	�V�[���Ǘ��̍쐬
		SceneManager::Create();
	}

	//	�Q�[�����[�v
	while (Win::Window::GetInstance().ProcessMessage())
	{
		//	�V�[�����Ƃ̍X�V
		SceneManager::GetInstance()->Update();

		d.Begin();
		{
			//	�V�[�����Ƃ̕`��
			SceneManager::GetInstance()->Draw();

		}
		d.End();
	}
	//	�V�[���Ǘ��̔j��
	SceneManager::Destroy();
	return 0;
}