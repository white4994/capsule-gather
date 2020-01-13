//
//	Input.h
//	キーボード入力、マウスの取得の実装
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
	//	キーボード

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

	//	キーが押されてるか調べる
	bool Test(BYTE k)
	{
		return ((k & 0x80) != 0);
	}

	/*	キーを押し続けている
		引数
			nVirtKey	: キーの種類
	*/
	bool Keyboard::On(int nVirtKey)
	{
		return Test(keys[NOW][nVirtKey]);
	}

	/*	キーが押された瞬間
	引数
		nVirtKey	: キーの種類
	*/
	bool Keyboard::Press(int nVirtKey)
	{
		return (On(nVirtKey) && !Test(keys[PREV][nVirtKey]));
	}

	/*	キーが離された瞬間
	引数
		nVirtKey	: キーの種類
	*/
	bool Keyboard::Release(int nVirtKey)
	{
		return (!On(nVirtKey) && Test(keys[PREV][nVirtKey]));
	}

	Keyboard::Keyboard()
	{
		Reset();
	}


	//	マウス
	//マウスの座標取得
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