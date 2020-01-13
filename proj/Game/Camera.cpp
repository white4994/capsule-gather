//
//	Camera.cpp
//	�J�����̎���
//

#include"Camera.h"

namespace Camera
{

	/*	������
	*/
	void FreeCamera::Reset(bool inv)
	{
		

		//	�r���[�|�[�g�̎擾
		D3DVIEWPORT9 vp;
		if (FAILED(D3D9::Device::GetInstance()->GetViewport(&vp)))
		{
			ERROR_MSG("�r���[�|�[�g�̎擾�Ɏ��s���܂���");
			return;
		}
		
		//	�ˉe�s��̏�����
		D3DXMatrixPerspectiveFovLH(&proj, initParam.viewAngle, (float)vp.Width / (float)vp.Height, initParam.zn, initParam.zf);
		
		//	�ˉe�s��̐ݒ�
		D3D9::Device::GetInstance()->SetTransform(D3DTS_PROJECTION, &proj);

		if (inv) { UpdateInverse(); }
		else { UpdateLookAt(); }
	}




	/*	�r���[�s��̓]��
		�t�s����g�p
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
		

		//	�s�b�`�A���[�A���[�����s��ɓK�p
		D3DXMatrixRotationX(&pMtx, axis.pitch);
		D3DXMatrixRotationY(&yMtx, axis.yaw);
		D3DXMatrixRotationZ(&rMtx, axis.roll);
		mtx = rMtx * pMtx*yMtx;
		//	���ʂ������x�N�g�����s��Ōv�Z
		D3DXVec3TransformNormal(&dir, &frontVec, &mtx);

		//	��l�̎��_
		if (initParam.fpv)
		{
			pos = look;
		}
		else   //	�O�l�̎��_
		{
			pos = look - dir * initParam.distance;
		}
		mtx._41 = pos.x;
		mtx._42 = pos.y;
		mtx._43 = pos.z;
		
		//	�J�����̐ݒ�
		D3DXMatrixInverse(&view, nullptr, &mtx);

		if (setTransform)
		{
			D3D9::Device::GetInstance()->SetTransform(D3DTS_VIEW, &view);
		}
		
	}

	/*	�r���[�s��̓]��
		D3DXMatrixLookAtLH���g�p
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


		//	�s�b�`�A���[�A���[�����s��ɓK�p
		D3DXMatrixRotationX(&pMtx, axis.pitch);
		D3DXMatrixRotationY(&yMtx, axis.yaw);
		D3DXMatrixRotationZ(&rMtx, axis.roll);
		mtx = rMtx * pMtx*yMtx;
		//	���ʂ������x�N�g�����s��Ōv�Z
		D3DXVec3TransformNormal(&dir, &frontVec, &mtx);

		//	��l�̎��_
		if (initParam.fpv)
		{
			pos = look;
		}
		else   //	�O�l�̎��_
		{
			pos = look - dir * initParam.distance;
		}
		mtx._41 = pos.x;
		mtx._42 = pos.y;
		mtx._43 = pos.z;

		//	�J�����̐ݒ�
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
		//	�r���[�|�[�g�̎擾
		D3DVIEWPORT9 vp;
		if (FAILED(D3D9::Device::GetInstance()->GetViewport(&vp)))
		{
			ERROR_MSG("�r���[�|�[�g�̎擾�Ɏ��s���܂���");
			return;
		}

		//	�ˉe�s��̏�����
		D3DXMatrixPerspectiveFovLH(&proj, initParam.viewAngle, (float)vp.Width / (float)vp.Height, initParam.zn, initParam.zf);

		//	�ˉe�s��̐ݒ�
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
		//	�J�����p�������[���h�ϊ�
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