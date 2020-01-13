//
//	Camera.h
//	�J�����̒�`
//
#ifndef _INCLUDE_Camera_h_
#define _INCLUDE_Camera_h_

#pragma once

#include"Common.h"
#include"D3D9.h"

//	�J����
namespace Camera
{
	//	��]��ړ������R�ȃJ����
	class FreeCamera
	{
		struct PARAMETER
		{
			//	�O�l�̎��_�ł̃J�����ƑΏۂƂ̋����̐����l
			float minDis = 0.f, maxDis = 10000.f;

			//	��p
			float viewAngle = D3DXToRadian(45.f);

			//	�O�l�̎��_�ł̃J�����ƑΏۂƂ̋���
			float distance = 1.f;

			//	�N���b�s���O�����izn���߁Azf�����j
			float zn = 0.1f, zf = 1000.f;

			//	true�̎���l�̎��_
			bool fpv = true;

		};


		struct AXIS
		{
			//	�s�b�`(x��)�A���[(y��)�A���[��(z��)�̒l
			float pitch = 0.f, yaw = 0.f, roll = 0.f;
		};

		//	�����Ώ�
		D3DXVECTOR3 look = { 0.f,0.f,0.f };

		//	�J�����̍��W�A����
		D3DXVECTOR3 pos, dir, up;
		const D3DXVECTOR3 frontVec = { 0.f,0.f,1.f }, upVec = { 0.f,1.f,0.f };

		D3DXMATRIX mtx, view, proj, pMtx, yMtx, rMtx;

	public:
		FreeCamera();
		FreeCamera(const FreeCamera& cam);
		FreeCamera& operator=(const FreeCamera& cam);

		//	������ true�̂Ƃ�UpdateInverse�ōX�V����
		void Reset(bool inv=true);

		//	�X�V
		void UpdateInverse(bool setTransform = true);	//	�t�s����g�p
		void UpdateLookAt(bool setTransform = true);	//	LookAtLH���g�p

		//	�Q�b�^�[

		//	�J�����̍s��̎擾
		const D3DXMATRIX& GetMtx() const
		{
			return mtx;
		}
		
		//	view�s��̎擾
		const D3DXMATRIX& GetViewMtx() const
		{
			return view;
		}

		//	proj�s��̎擾
		const D3DXMATRIX& GetProjMtx() const
		{
			return proj;
		}

		//	�J�����̍��W���擾
		const D3DXVECTOR3& GetPos() const
		{
			return pos;
		}

		//	�J�����̑O�����̎擾
		const D3DXVECTOR3& GetFrontDir() const
		{
			return dir;
		}

		//	�J�����̏�����̎擾
		const D3DXVECTOR3& GetUpDir() const
		{
			return up;
		}

		//	view,proj�s��̍����s��̎擾
		D3DXMATRIX GetViewProjMtx() const
		{
			return (view*proj);
		}

		//	�Z�b�^�[
		//	�����Ώۂ̐ݒ�
		void SetLook(D3DXVECTOR3 look)
		{
			this->look = look;
		}

		PARAMETER initParam;

		AXIS axis;

	};

	//	�I�u�W�F�N�g�𒍎����Ȃ���ꏏ�ɓ����J����
	class ChaseCamera
	{
		struct PARAMETER
		{
			//	��p
			float viewAngle = D3DXToRadian(45.f);

			//	�N���b�s���O�����izn���߁Azf�����j
			float zn = 0.1f, zf = 10000.f;
		};

	public:
		ChaseCamera();
		ChaseCamera(const ChaseCamera& cam);
		ChaseCamera& operator=(const ChaseCamera& cam);

		void Reset();

		//	�J�����̍X�V
		void Update(D3DXMATRIX& world);

		//	�Q�b�^�[
		//	view�s��̎擾
		const D3DXMATRIX& GetViewMtx() const
		{
			return view;
		}

		//	proj�s��̎擾
		const D3DXMATRIX& GetProjMtx() const
		{
			return proj;
		}

		//	view,proj�s��̍����s��̎擾
		D3DXMATRIX GetViewProjMtx() const
		{
			return (view*proj);
		}

		//	�Z�b�^�[
		//	�J�����̍��W�̐ݒ�
		void SetPos(D3DXVECTOR3 pos)
		{
			this->pos = pos;
		}

		//	�����Ώۂ̐ݒ�
		void SetLook(D3DXVECTOR3 look)
		{
			this->look = look;
		}

		PARAMETER initParam;

	private:
		//	�J�����̃��[�J�����W
		D3DXVECTOR3 pos, look;

		//	�s��
		D3DXMATRIX camLocal,view,proj;

		//	��x�N�g��
		D3DXVECTOR3 up = { 0.f,1.f,0.f };

	};
}

#endif // !_INCLUDE_Camera_h_
