//
//	Window.h
//	�E�B���h�E�Ǘ��̒�`
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

		//	PostQuitMessage����΂��܂�true��Ԃ�������
		bool ProcessMessage();

		// �V���O���g���Q�b�^�[
		/*	�C���X�^���X�̎擾
			����
				width			: �N���C�A���g�̈�̃f�t�H���g�̕�
				height			: �N���C�A���g�̈�̃f�t�H���g�̍���
				clsName			: �N���X��
				winName			: �E�B���h�E��
			����Ăяo�����͏����������˂Ă���̂ŕύX����ꍇ�͂��̍ۍs������
		*/
		static Window& GetInstance(int width = 680, int height = 480, const char* clsName = "defalt", const char* winName = "defalt")
		{
			static Window win(width, height, clsName, winName);
			return win;
		}

		//	�L���X�g���Z�q�I�[�o�[���[�h
		operator HWND() const { return hWnd; };

		//	�A���[���Z�q�I�[�o�[���[�h
		HWND operator->() const { return hWnd; };

	private:

		HWND hWnd = 0;

		/*	�E�B���h�E�쐬
			����
				width			: �N���C�A���g�̈�̃f�t�H���g�̕�
				height			: �N���C�A���g�̈�̃f�t�H���g�̍���
				clsName			: �N���X��
				winName			: �E�B���h�E��
		*/
		Window(int width, int height, const char* clsName, const char* winName);

		Window(const Window&) = delete;
	};
}
#endif // !_INCLUDE_Window_h_