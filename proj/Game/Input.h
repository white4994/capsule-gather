//
//	Input.h
//	�L�[�{�[�h���́A�}�E�X�̎擾�̒�`
//

#ifndef _INCLUDE_Input_h_
#define	_INCLUDE_Input_h_


#pragma once
#include"Common.h"

namespace Input
{
	//	�L�[�{�[�h
	class Keyboard
	{
	public:
		Keyboard();

		//	���͂̍X�V
		void Update();

		/*	�L�[�����������Ă���
			����
				nVirtKey	: �L�[�̎��
		*/
		bool On(int nVirtKey);

		/*	�L�[�������ꂽ�u��
			����
				nVirtKey	: �L�[�̎��
		*/
		bool Press(int nVirtKey);

		/*	�L�[�������ꂽ�u��
			����
				nVirtKey	: �L�[�̎��
		*/
		bool Release(int nVirtKey);

	private:
		//	������
		void Reset();

	};

	/*	�}�E�X
		�I�u�W�F�N�g�쐬�̓��[�v���ōs������
		�{�^���̎擾�̓L�[�{�[�h�N���X���g������
	*/
	class Mouse
	{
	public:

		Mouse();

		//	�Q�b�^�[

		//	�}�E�X�̓񎟌� X ���W
		float GetX() const
		{
			return posX;
		}

		//	�}�E�X�̓񎟌� Y ���W
		float GetY() const
		{
			return posY;
		}

	private:
		//	�}�E�X�̓񎟌����W
		float posX, posY;

		//�}�E�X�̍��W�擾
		void GetPos();

	};
}
#endif // !_INCLUDE_Input_h_