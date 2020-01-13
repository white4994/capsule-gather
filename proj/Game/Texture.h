//	
//	Texture.h	
//	テクスチャーのロードなどの定義
//
#ifndef _INCLUDE_Texture_h_
#define	_INCLUDE_Texture_h_


#pragma once
#include<string>

#include"Common.h"

namespace Texture
{
	class Texture
	{
		LPDIRECT3DTEXTURE9 tex;
		D3DXVECTOR2 size;
		std::string name;

	public:
		Texture() :tex(nullptr), size(0.f, 0.f) {}
		Texture(const char* fileName);
		Texture(const Texture& t);
		~Texture();
		Texture& operator=(const Texture& t);

		//	読み込み
		void Load(const char* fileName);

		//	ゲッター

		//	テクスチャの幅の取得
		const D3DXVECTOR2& GetSize() const;

		//	テクスチャ名の取得
		const std::string& GetName() const;

		const LPDIRECT3DTEXTURE9& GetTex() const;

	};
}
#endif // !_INCLUDE_Texture_h_