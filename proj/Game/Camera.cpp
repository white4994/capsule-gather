//
//	Camera.cpp
//	カメラの実装
//

#include"Camera.h"

namespace Camera
{

	/*	初期化
	*/
	void FreeCamera::Reset(bool inv)
	{
		

		//	ビューポートの取得
		D3DVIEWPORT9 vp;
		if (FAILED(D3D9::Device::GetInstance()->GetViewport(&vp)))
		{
			ERROR_MSG("ビューポートの取得に失敗しました");
			return;
		}
		
		//	射影行列の初期化
		D3DXMatrixPerspectiveFovLH(&proj, initParam.viewAngle, (float)vp.Width / (float)vp.Height, initParam.zn, initParam.zf);
		
		//	射影行列の設定
		D3D9::Device::GetInstance()->SetTransform(D3DTS_PROJECTION, &proj);

		if (inv) { UpdateInverse(); }
		else { UpdateLookAt(); }
	}




	/*	ビュー行列の転送
		逆行列を使用
	*/
	void FreeCamera::UpdateInverse(bool setTransform)
	{

		if (initParam.minDis < initParam.maxDis)
		{
			if (initParam.distance < initParam.minDis)
			{
				initParam.distance = initParam.minDis;
			}
			else if(initParam.distance > initParam.maxDis)
			{
				initParam.distance = initParam.maxDis;
			}
		}
		

		//	ピッチ、ヨー、ロールを行列に適用
		D3DXMatrixRotationX(&pMtx, axis.pitch);
		D3DXMatrixRotationY(&yMtx, axis.yaw);
		D3DXMatrixRotationZ(&rMtx, axis.roll);
		mtx = rMtx * pMtx*yMtx;
		//	正面を向くベクトルを行列で計算
		D3DXVec3TransformNormal(&dir, &frontVec, &mtx);

		//	一人称視点
		if (initParam.fpv)
		{
			pos = look;
		}
		else   //	三人称視点
		{
			pos = look - dir * initParam.distance;
		}
		mtx._41 = pos.x;
		mtx._42 = pos.y;
		mtx._43 = pos.z;
		
		//	カメラの設定
		D3DXMatrixInverse(&view, nullptr, &mtx);

		if (setTransform)
		{
			D3D9::Device::GetInstance()->SetTransform(D3DTS_VIEW, &view);
		}
		
	}

	/*	ビュー行列の転送
		D3DXMatrixLookAtLHを使用
	*/
	void FreeCamera::UpdateLookAt(bool setTransform)
	{

		if (initParam.minDis < initParam.maxDis)
		{
			if (initParam.distance < initParam.minDis)
			{
				initParam.distance = initParam.minDis;
			}
			else if (initParam.distance > initParam.maxDis)
			{
				initParam.distance = initParam.maxDis;
			}
		}


		//	ピッチ、ヨー、ロールを行列に適用
		D3DXMatrixRotationX(&pMtx, axis.pitch);
		D3DXMatrixRotationY(&yMtx, axis.yaw);
		D3DXMatrixRotationZ(&rMtx, axis.roll);
		mtx = rMtx * pMtx*yMtx;
		//	正面を向くベクトルを行列で計算
		D3DXVec3TransformNormal(&dir, &frontVec, &mtx);

		//	一人称視点
		if (initParam.fpv)
		{
			pos = look;
		}
		else   //	三人称視点
		{
			pos = look - dir * initParam.distance;
		}
		mtx._41 = pos.x;
		mtx._42 = pos.y;
		mtx._43 = pos.z;

		//	カメラの設定
		D3DXVec3TransformNormal(&up, &upVec, &mtx);
		D3DXMatrixLookAtLH(&view, &pos, &look, &up);

		if (setTransform)
		{
			D3D9::Device::GetInstance()->SetTransform(D3DTS_VIEW, &view);
		}
		
	}

	FreeCamera::FreeCamera()
	{
		Reset();
	}

	FreeCamera::FreeCamera(const FreeCamera& cam)
		:initParam(cam.initParam),look(cam.look), axis(cam.axis)
	{
		Reset();
	}

	FreeCamera& FreeCamera::operator=(const FreeCamera& cam)
	{
		initParam = cam.initParam;
		look = cam.look;
		axis.pitch = cam.axis.pitch;
		axis.yaw = cam.axis.yaw;
		axis.roll = cam.axis.roll;

		return *this;
	}

	void ChaseCamera::Reset()
	{
		//	ビューポートの取得
		D3DVIEWPORT9 vp;
		if (FAILED(D3D9::Device::GetInstance()->GetViewport(&vp)))
		{
			ERROR_MSG("ビューポートの取得に失敗しました");
			return;
		}

		//	射影行列の初期化
		D3DXMatrixPerspectiveFovLH(&proj, initParam.viewAngle, (float)vp.Width / (float)vp.Height, initParam.zn, initParam.zf);

		//	射影行列の設定
		D3D9::Device::GetInstance()->SetTransform(D3DTS_PROJECTION, &proj);

		camLocal = {
			pos.x,pos.y,pos.z,1.f,
			up.x - pos.x,up.y - pos.y,up.z - pos.z,0.f,
			look.x,look.y,look.z,1.f,
			0.f,0.f,0.f,0.f
		};
	}

	void ChaseCamera::Update(D3DXMATRIX& world)
	{
		//	カメラ姿勢をワールド変換
		D3DXMATRIX cameraW = camLocal * world;
		D3DXMatrixLookAtLH(&view,
						   &D3DXVECTOR3(cameraW._11, cameraW._12, cameraW._13),
						   &D3DXVECTOR3(cameraW._31, cameraW._32, cameraW._33),
						   &D3DXVECTOR3(cameraW._21, cameraW._22, cameraW._23)
		);
		D3D9::Device::GetInstance()->SetTransform(D3DTS_VIEW, &view);
	}

	ChaseCamera::ChaseCamera()
	{
		Reset();
	}

	ChaseCamera::ChaseCamera(const ChaseCamera& cam)
		: initParam(cam.initParam),pos(cam.pos)
	{
		Reset();
	}

	ChaseCamera& ChaseCamera::operator=(const ChaseCamera& cam)
	{
		initParam = cam.initParam;
		pos = cam.pos;

		return *this;
	}

}