//
//	Window.cpp
//	�E�B���h�E�Ǘ��̎���
//

#include<Windows.h>

#include"Window.h"

namespace Win
{

	/*	���b�Z�[�W�����p�R�[���o�b�N�֐�
		����
			msg		: ���b�Z�[�W
			wParam	: ���b�Z�[�W�̍ŏ��̃p�����[�^
			lParam	: ���b�Z�[�W��2�Ԗڂ̃p�����[�^
		�߂�l
			���b�Z�[�W�̏�������
	*/
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CLOSE:			//	�E�B���h�E������ꂽ
			PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)	//	ESC�L�[�������ꂽ
			{
				PostQuitMessage(0);
			}
			break;
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	/*	�E�B���h�E�쐬
		����
			width			: �N���C�A���g�̈�̃f�t�H���g�̕�
			height			: �N���C�A���g�̈�̃f�t�H���g�̍���
			clsName			: �N���X��
			winName			: �E�B���h�E��
	*/
	Window::Window(int width, int height, const char* clsName, const char* winName)
	{
		if (hWnd != NULL)
		{
			return;
		}

		HINSTANCE hInstance = GetModuleHandle(NULL);

		//	�E�B���h�E�N���X�̏�����
		WNDCLASSEX wc = {};
		if (GetClassInfoEx(hInstance, clsName, &wc) == 0)
		{
			wc.cbSize = sizeof(WNDCLASSEX);				//	���̍\���̂̃T�C�Y
			wc.style = CS_HREDRAW | CS_VREDRAW;			//	�E�B���h�E�̃X�^�C��
			wc.lpfnWndProc = WndProc;					//	���b�Z�[�W�����֐��̓o�^
			wc.cbClsExtra = 0;							//	�ʏ�g��Ȃ��̂ŏ��0
			wc.cbWndExtra = 0;							//	�ʏ�g��Ȃ��̂ŏ��0
			wc.hInstance = hInstance;					//	�C���X�^���X�n���h��
			wc.hIcon = NULL;							//	�A�C�R���Ȃ�
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);	//	�J�[�\���̌`
			wc.hbrBackground = NULL;					//	�w�i�Ȃ�
			wc.lpszMenuName = NULL;						//	���j���[�Ȃ�
			wc.lpszClassName = clsName;					//	�N���X��
			wc.hIconSm = NULL;							//	���A�C�R��

			//	�E�B���h�E�N���X�̓o�^
			if (RegisterClassEx(&wc) == 0)
			{
				ERROR_MSG("�E�B���h�E�N���X�̓o�^�Ɏ��s���܂���");
				return;
			}
		}



		RECT rect = { 0,0,width,height };
		AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_LEFT);
		//	�E�B���h�E�̍쐬
		hWnd = CreateWindow(
			clsName,										//	�N���X��
			winName,										//	�E�B���h�E��
			WS_OVERLAPPEDWINDOW,							//	�E�B���h�E�X�^�C��
			CW_USEDEFAULT, CW_USEDEFAULT,					//	�E�B���h�E�̈ʒu
			rect.right - rect.left, rect.bottom - rect.top,	//	�E�B���h�E�̕��A����
			NULL,											//	�e�E�B���h�E�̃n���h��
			NULL,											//	���j���[��q�E�B���h�E�̃n���h��
			hInstance,										//	�A�v���P�[�V�����C���X�^���X�n���h��
			NULL);											//	�E�B���h�E�̍쐬�f�[�^

		if (hWnd == 0)
		{
			ERROR_MSG("�E�B���h�E�̍쐬�Ɏ��s���܂���");
			return;
		}

		//	�E�B���h�E�̕\��
		ShowWindow(hWnd, SW_SHOW);
		//	WM_PAINT���Ă΂�Ȃ��悤�ɂ���
		ValidateRect(hWnd, 0);
	}

	//	PostQuitMessage()����΂��܂�true��Ԃ�������
	bool Window::ProcessMessage()
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)	//	PostQuitMessage()���Ă΂ꂽ
			{
				return false;
			}
			//	���b�Z�[�W�|��ƃf�B�X�p�b�`
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}

		return true;
	}
}