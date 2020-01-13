//
//	Camera.h
//	カメラの定義
//
#ifndef _INCLUDE_Camera_h_
#define _INCLUDE_Camera_h_

#pragma once

#include"Common.h"
#include"D3D9.h"

//	カメラ
namespace Camera
{
	//	回転や移動が自由なカメラ
	class FreeCamera
	{
		struct PARAMETER
		{
			//	三人称視点でのカメラと対象との距離の制限値
			float minDis = 0.f, maxDis = 10000.f;

			//	画角
			float viewAngle = D3DXToRadian(45.f);

			//	三人称視点でのカメラと対象との距離
			float distance = 1.f;

			//	クリッピング距離（znが近、zfが遠）
			float zn = 0.1f, zf = 1000.f;

			//	trueの時一人称視点
			bool fpv = true;

		};


		struct AXIS
		{
			//	ピッチ(x軸)、ヨー(y軸)、ロール(z軸)の値
			float pitch = 0.f, yaw = 0.f, roll = 0.f;
		};

		//	注視対象
		D3DXVECTOR3 look = { 0.f,0.f,0.f };

		//	カメラの座標、向き
		D3DXVECTOR3 pos, dir, up;
		const D3DXVECTOR3 frontVec = { 0.f,0.f,1.f }, upVec = { 0.f,1.f,0.f };

		D3DXMATRIX mtx, view, proj, pMtx, yMtx, rMtx;

	public:
		FreeCamera();
		FreeCamera(const FreeCamera& cam);
		FreeCamera& operator=(const FreeCamera& cam);

		//	初期化 trueのときUpdateInverseで更新する
		void Reset(bool inv=true);

		//	更新
		void UpdateInverse(bool setTransform = true);	//	逆行列を使用
		void UpdateLookAt(bool setTransform = true);	//	LookAtLHを使用

		//	ゲッター

		//	カメラの行列の取得
		const D3DXMATRIX& GetMtx() const
		{
			return mtx;
		}
		
		//	view行列の取得
		const D3DXMATRIX& GetViewMtx() const
		{
			return view;
		}

		//	proj行列の取得
		const D3DXMATRIX& GetProjMtx() const
		{
			return proj;
		}

		//	カメラの座標を取得
		const D3DXVECTOR3& GetPos() const
		{
			return pos;
		}

		//	カメラの前方向の取得
		const D3DXVECTOR3& GetFrontDir() const
		{
			return dir;
		}

		//	カメラの上方向の取得
		const D3DXVECTOR3& GetUpDir() const
		{
			return up;
		}

		//	view,proj行列の合成行列の取得
		D3DXMATRIX GetViewProjMtx() const
		{
			return (view*proj);
		}

		//	セッター
		//	注視対象の設定
		void SetLook(D3DXVECTOR3 look)
		{
			this->look = look;
		}

		PARAMETER initParam;

		AXIS axis;

	};

	//	オブジェクトを注視しながら一緒に動くカメラ
	class ChaseCamera
	{
		struct PARAMETER
		{
			//	画角
			float viewAngle = D3DXToRadian(45.f);

			//	クリッピング距離（znが近、zfが遠）
			float zn = 0.1f, zf = 10000.f;
		};

	public:
		ChaseCamera();
		ChaseCamera(const ChaseCamera& cam);
		ChaseCamera& operator=(const ChaseCamera& cam);

		void Reset();

		//	カメラの更新
		void Update(D3DXMATRIX& world);

		//	ゲッター
		//	view行列の取得
		const D3DXMATRIX& GetViewMtx() const
		{
			return view;
		}

		//	proj行列の取得
		const D3DXMATRIX& GetProjMtx() const
		{
			return proj;
		}

		//	view,proj行列の合成行列の取得
		D3DXMATRIX GetViewProjMtx() const
		{
			return (view*proj);
		}

		//	セッター
		//	カメラの座標の設定
		void SetPos(D3DXVECTOR3 pos)
		{
			this->pos = pos;
		}

		//	注視対象の設定
		void SetLook(D3DXVECTOR3 look)
		{
			this->look = look;
		}

		PARAMETER initParam;

	private:
		//	カメラのローカル座標
		D3DXVECTOR3 pos, look;

		//	行列
		D3DXMATRIX camLocal,view,proj;

		//	上ベクトル
		D3DXVECTOR3 up = { 0.f,1.f,0.f };

	};
}

#endif // !_INCLUDE_Camera_h_
