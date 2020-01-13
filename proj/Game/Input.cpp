//
//	Input.h
//	�L�[�{�[�h���́A�}�E�X�̎擾�̎���
//

#include"Input.h"
#include"Window.h"
#define MAX_KEY_NUM 256
enum Timing
{
	NOW,
	PREV,
	NUM_OF_TIMING
};

namespace Input
{
	//	�L�[�{�[�h

	BYTE keys[NUM_OF_TIMING][MAX_KEY_NUM] = {};

	void Keyboard::Reset()
	{
		memset(keys, 0, sizeof(keys));
	}

	void Keyboard::Update()
	{
		memcpy(keys[PREV], keys[NOW], sizeof(*keys));
		if (!GetKeyboardState(keys[NOW]))
			return;
	}

	//	�L�[��������Ă邩���ׂ�
	bool Test(BYTE k)
	{
		return ((k & 0x80) != 0);
	}

	/*	�L�[�����������Ă���
		����
			nVirtKey	: �L�[�̎��
	*/
	bool Keyboard::On(int nVirtKey)
	{
		return Test(keys[NOW][nVirtKey]);
	}

	/*	�L�[�������ꂽ�u��
	����
		nVirtKey	: �L�[�̎��
	*/
	bool Keyboard::Press(int nVirtKey)
	{
		return (On(nVirtKey) && !Test(keys[PREV][nVirtKey]));
	}

	/*	�L�[�������ꂽ�u��
	����
		nVirtKey	: �L�[�̎��
	*/
	bool Keyboard::Release(int nVirtKey)
	{
		return (!On(nVirtKey) && Test(keys[PREV][nVirtKey]));
	}

	Keyboard::Keyboard()
	{
		Reset();
	}


	//	�}�E�X
	//�}�E�X�̍��W�擾
	void Mouse::GetPos()
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(Win::Window::GetInstance(), &pt);
		RECT rect;
		GetClientRect(Win::Window::GetInstance(), &rect);
		posX = (float)pt.x - (float)rect.right / 2;
		posY = (float)pt.y - (float)rect.bottom / 2;
		
		if (posX > rect.right / 2)
		{
			pt.x -= rect.right;
		}
		else if (posX < -(rect.right / 2))
		{
			pt.x += rect.right;
		}

		if (posY > rect.bottom / 2)
		{
			pt.y -= rect.bottom;
		}
		else if (posY < -(rect.bottom / 2))
		{
			pt.y += rect.bottom;
		}

		ClientToScreen(Win::Window::GetInstance(), &pt);
		SetCursorPos((int)pt.x, (int)pt.y);

	}

	Mouse::Mouse()
		:posX(0),posY(0)
	{
		GetPos();
	}
}