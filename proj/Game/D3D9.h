//
//	D3D9.h
//	Direct3D�̍쐬�A�`��Ȃǂ̒�`
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

		//	�V���O���g���Q�b�^�[
		//	�C���X�^���X�̎擾	�����������˂Ă���̂ōŏ��̐������͈���HWND�͕K������邱��
		//	winMode�̓E�B���h�E���[�h�̑I���@true�Ńt���X�N���[��
		static Device& GetInstance(HWND hWnd = NULL,bool winMode=false);

		/*	�`��J�n
			����
				color	: ��ʃN���A�̍ۂɓh��Ԃ��F{ r, g, b, a }
		*/
		void Begin(D3DXCOLOR color = { 0.1f,0.3f,0.7f,1.f });

		//	�`��I��
		void End(HWND hWnd = NULL);

		//	�L���X�g���Z�q�I�[�o�[���[�h
		operator LPDIRECT3DDEVICE9() const { return dev; };

		//	�A���[���Z�q�I�[�o�[���[�h
		LPDIRECT3DDEVICE9 operator->() const { return dev; };

	private:

		CComPtr<IDirect3D9> d3d9;
		CComPtr<IDirect3DDevice9> dev;

		/*	������
			����
				hWnd	: �E�B���h�E�n���h��
		*/
		Device(HWND hWnd = NULL, bool winMode = false);

		Device(const Device&) = delete;
	};
}
#endif // !_INCLUDE_D3D)_h_