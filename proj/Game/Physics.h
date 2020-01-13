//
//	Physics.h
//	�����蔻��A���R�����̒�`
//
#ifndef _INCLUDE_Physics_h_
#define	_INCLUDE_Physics_h_

#pragma once
#include<math.h>
#include<stdarg.h>

#include"Common.h"
#include"D3D9.h"

#define EPSIRON 0.00001f	//	�덷

namespace Physics
{

	//	��
	struct SPHERE
	{
		SPHERE() :center(0.f, 0.f, 0.f), pos(0.f, 0.f, 0.f), r(0), mesh(nullptr){}
		SPHERE(const LPD3DXMESH mesh);
		SPHERE(const SPHERE& sphere);
		~SPHERE();

		SPHERE& operator=(const SPHERE& sphere);

		//	BoundingSphere�̍쐬
		void CreateSphere();

		//	�����쐬����O�Ɏ��s���邱��
		void SetMesh(const LPD3DXMESH& mesh);

		D3DXVECTOR3 center;	//	���S���W
		D3DXVECTOR3 pos;	//	���̍��W
		float r;			//	���a

	private:
		CComPtr<ID3DXMesh> mesh;
	};

	//	AABB
	struct AABB
	{
		AABB():boxMin(0.f,0.f,0.f),boxMax(0.f,0.f,0.f),len(0.f,0.f,0.f),mesh(nullptr){}
		AABB(const LPD3DXMESH mesh);
		AABB(const AABB& box);
		~AABB();

		AABB& operator=(const AABB& box);

		//	AABB�̍쐬
		void CreateAABB();

		//	AABB���쐬����O�Ɏ��s���邱��
		void SetMesh(const LPD3DXMESH& mesh);

		D3DXVECTOR3 pos;	//	���W
		D3DXVECTOR3 boxMin;	//	�ŏ��l
		D3DXVECTOR3 boxMax;	//	�ő�l
		D3DXVECTOR3 len;	//	����

	private:
		CComPtr<ID3DXMesh> mesh;
	};
	
	//	OBB
	struct OBB
	{
		OBB() :center(0.f, 0.f, 0.f),pos(0.f, 0.f, 0.f), axis(), halfLen(0.f, 0.f, 0.f), mesh(nullptr){}
		/*
			pos			:	OBB�̃��[���h���W	
			angle[3]	:	OBB�̌X���@pitch,yaw,roll�̏��ԂŊi�[���ꂽ�z��ł��邱��
		*/
		OBB(const LPD3DXMESH mesh, const D3DXVECTOR3 pos, const float *angle);
		OBB(const OBB& box);
		~OBB();

		OBB& operator=(const OBB& box);

		//	OBB�̍쐬
		void CreateOBB();

		//	OBB���쐬����O�Ɏ��s���邱��
		void SetMesh(const LPD3DXMESH& mesh);

		D3DXVECTOR3 center;		//	���S���W
		D3DXVECTOR3 pos;		//	���W
		float angle[3] = {};	//	OBB�̌X��
		D3DXVECTOR3 axis[3];	//	�����x�N�g��
		D3DXVECTOR3 halfLen;	//	�e���W���ɉ����������̔���

	private:
		CComPtr<ID3DXMesh> mesh;
	};

	//	�Փ�
	class Collision
	{
	public:

		/*	�ʖ@���x�N�g���̎擾
			����
				out	:	���K�����ꂽ�O�p�`�̖ʖ@���x�N�g����Ԃ��i�߂�l�j
				p0	:	�O�p�`�̒��_0
				p1	:	�O�p�`�̒��_1
				p2	:	�O�p�`�̒��_2
				�O�p�`�𔽎��v���ō���Ă���Ɖ��肵���֐��Ȃ̂Œ��_�̏��Ԃɂ͋C��t���邱��
		*/
		D3DXVECTOR3* GetPlaneNormal(D3DXVECTOR3* out, const D3DXVECTOR3& p0, const D3DXVECTOR3& p1, const D3DXVECTOR3& p2);

		/*	���_�@���x�N�g���̎擾
			����
				num			:	�n���ʖ@���x�N�g���̐�
				out			:	���K�����ꂽ���_�@���x�N�g����Ԃ��i�߂�l�j
				...			:	�ʖ@���x�N�g���@���_�������Ă���ʂ̐������̖@��(D3DXVECTOR3)
		*/
		D3DXVECTOR3* GetVertexNormal(int num, D3DXVECTOR3* out, ...);

		/*	�_�Ɩ@�����畽�ʂ��쐬
			����
				out			:	���ʂ�Ԃ��i�߂�l�j
				normal		:	�ʖ@���x�N�g��
				point		:	���ʏ�̓_
		*/
		D3DXPLANE* CreatePlane(D3DXPLANE* out, const D3DXVECTOR3& normal, const D3DXVECTOR3& point);

		/*	����_���畽�ʂ܂ł̋���
			����
				plane	:	����
				point	:	�_
			�߂�l
				�_���畽�ʂ܂ł̋�����Ԃ�
		*/
		float PlanePointDistance(const D3DXPLANE& plane, const D3DXVECTOR3& point);

		/*	�ǂ���x�N�g��
			����
				out		:	���K�����ꂽ�ǂ���x�N�g���i�߂�l�j
				front	:	�i�s�x�N�g��
				normal	:	�Փ˓_�̖@���x�N�g��
		*/
		D3DXVECTOR3* WallRubVector(D3DXVECTOR3* out, const D3DXVECTOR3& front, const D3DXVECTOR3& normal);

		/*	���˃x�N�g��
			����
				out		:	���K�����ꂽ���˃x�N�g���i�߂�l�j
				front	:	�i�s�x�N�g��
				normal	:	�Փ˓_�̖@���x�N�g��
		*/
		D3DXVECTOR3* ReflectVector(D3DXVECTOR3* out, const D3DXVECTOR3& front, const D3DXVECTOR3& normal);

		/*	���ʂƓ_�̈ʒu�֌W�𒲂ׂ�
			����
				plane	:	����
				point	:	�_
			�߂�l
			+ �͕\  - �͗�  0 �͕��ʏ�ɑ���
		*/
		float PlanePoint(const D3DXPLANE& plane, const D3DXVECTOR3& point);

		/*	���Ɠ_�̏Փ˔���
			����
				linePoint	:	����̓_
				vec			:	���̕����������x�N�g��
				point		:	�_
			�߂�l
				true�̂Ƃ��Փ�
		*/
		bool LinePoint(const D3DXVECTOR3& linePoint, const D3DXVECTOR3& vec, const D3DXVECTOR3& point);

		/*	�����Ɠ_�̏Փ˔���
			����
				ps		:	������Start�_
				pe		:	������End�_
				point	:	�_
			�߂�l
				true�̂Ƃ��Փ�
		*/
		bool LineSegmentPoint(const D3DXVECTOR3& ps, const D3DXVECTOR3& pe, const D3DXVECTOR3& point);

		/*	2�����̏Փ˔���ƍŒZ�����ƍŒZ�_���擾
			����
				outDis		:	2�����̋���
				outPos1		:	����1��̍ŒZ�_���W
				outPos2		:	����2��̍ŒZ�_���W
				linePoint1	:	����1���1�_
				lineVec1	:	����1�̕���
				linePoint2	:	����2���1�_
				lineVec2	:	����2�̕���
			�߂�l
				true�̂Ƃ�2�����͏Փ˂��Ă���
		*/
		bool CalcLineCom(float* outDis, D3DXVECTOR3* outPos1, D3DXVECTOR3* outPos2, const D3DXVECTOR3& linePoint1, const D3DXVECTOR3& lineVec1, const D3DXVECTOR3& linePoint2, const D3DXVECTOR3& lineVec2);

		/*	���ƕ��ʂ̏Փ˔��� �����Փ˂��Ă邩�ǂ����̂ݒm���
			����
				planePoint		:	���ʏ�̓_
				normal			:	���ʂ̖@���x�N�g��
				linePoint		:	����̓_
				vec				:	���̕����������x�N�g��
			�߂�l
				true�̂Ƃ��Փ�
		*/
		bool PlaneLine(const D3DXVECTOR3& planePoint, const D3DXVECTOR3& normal, const D3DXVECTOR3& linePoint, const D3DXVECTOR3& vec);

		/*	�����ƕ��ʂ̏Փ� �����ƕ��ʂ̏Փ˓_���킩��
			����
				out		:	�Փ˓_��Ԃ��A�Փ˂��Ă��Ȃ������ꍇ��NULL(�߂�l)
				plane	:	�Ώۂ̕���
				ps		:	������Start�_
				pe		:	������End�_
		*/
		D3DXVECTOR3* PlaneLineSegment(D3DXVECTOR3* out, const D3DXPLANE& plane, const D3DXVECTOR3& ps, const D3DXVECTOR3& pe);

		/*	���Ƌ��̏Փ˔���
			����
				sphere1	:	��1
				sphere2	:	��2
			�߂�l
				true�̂Ƃ��Փ˂��Ă���
		*/
		bool SphereCom(const SPHERE& sphere1,const SPHERE& sphere2);

		/* ���|���S���̍������擾
			����
				normal		:	�ʖ@���x�N�g��
				point		:	x,z�݂̂킩���Ă���|���S���Ɋ܂܂ꂽ�_(y���܂܂�Ă��邩�ǂ����͊֌W�Ȃ�)
				p0			:	�ʖ@���x�N�g���̖ʂ̒��_0
			�߂�l
				���|���S����point������y������Ԃ� 
		*/
		float GetFloorHeight(const D3DXVECTOR3& normal, const D3DXVECTOR3& point, const D3DXVECTOR3& p0);

		/*	���ƕ��ʂ̏Փ˔���ƍ��W�Ǝ������擾
			����
				outTime			:	�Փ˂����Ƃ��̎���
				outPos			:	�Փ˂����Ƃ��̍��W
				sphere			:	��
				spherePrePos	:	���̑O�̈ʒu
				sphereNextPos	:	���̎��̓��B�ʒu
				normal			:	���ʂ̖@��
				planePoint		:	����
			�߂�l
				true�̂Ƃ����ƕ��ʂ͏Փ˂��Ă���
		*/
		bool CalcSpherePlane(float* outTime, D3DXVECTOR3* outPos, const SPHERE& sphere, const D3DXVECTOR3& spherePrePos, const D3DXVECTOR3& sphereNextPos, const D3DXVECTOR3& normal, const D3DXPLANE& plane);

		/*	AABB�Ɠ_�Ƃ̍ŒZ����
			����
				boxMin	:	AABB�̍ŏ��l
				boxMax	:	AABB�̍ő�l
				point	:	�_
			�߂�l
				AABB�Ɠ_�̋�����Ԃ�	0�̂Ƃ�AABB���ɓ_������
		*/
		float AABBPointDistance(const AABB& box, const D3DXVECTOR3& point);

		/*	AABB���m�̏Փ˔���
			����
				box1	:	AABB1
				box2	:	AABB2
			�߂�l
				true�̂Ƃ��Փ�
		*/
		bool AABBCom(const AABB& box1, const AABB& box2);

		/*	OBB�Ɠ_�̍ŒZ����
			����
				box		:	OBB
				point	:	�_
			�߂�l
				OBB�Ɠ_�Ƃ̍ŒZ������float�^�ŕԂ�
		*/
		float OBBPointDistance(const OBB& box, const D3DXVECTOR3& point);

		/*	OBB��OBB�̏Փ˔���
			����
				box1	:	OBB1
				box2	:	OBB2
			�߂�l
				true�̂Ƃ��Փ�
		*/
		bool OBBCom(const OBB& box1, const OBB& box2);

		/*	OBB�ƕ��ʂ̏Փ˔���
			����
				box		:	OBB
				pos		:	���ʏ�̓_
				normal	:	���ʂ̐��K�����ꂽ�@��
				len		:	OBB�ƕ��ʂ��Փ˂��Ă����ꍇ�ǂɂ߂荞�񂾕���߂����߂̋������Z�o
			�߂�l
				true�̂Ƃ��Փ�
		*/
		bool OBBPlane(const OBB& box, const D3DXVECTOR3& pos, const D3DXVECTOR3& normal, float* len = NULL);

		/*	OBB�Ƌ��̏Փ˔���
			����
				box		:	OBB
				sphere	:	��
			�߂�l
				true�̎��Փ�
		*/
		bool OBBSphere(const OBB& box, const SPHERE& sphere);

		/*	���ƒ����̏Փ˔���ƍ��W�Ǝ������擾
			����
				outTime			:	�Փ˂����Ƃ��̎���
				outPos			:	�Փ˂����Ƃ��̍��W
				sphere			:	��
				spherePrePos	:	���̑O�̈ʒu
				sphereNextPos	:	���̎��̓��B�ʒu
				linePoint		:	������̓_
				linrDirect		:	�����̕���
			�߂�l
				true�̂Ƃ��Փ�
		*/
		bool CalcSphereLine(float* outTime, D3DXVECTOR3* outPos, const SPHERE& sphere, const D3DXVECTOR3& spherePrePos, const D3DXVECTOR3& sphereNextPos, const D3DXVECTOR3& linePoint, const D3DXVECTOR3& lineDirect);

		/*	Ray��AABB�Ƃ̏Փ˔���ƍ��W�Ǝ������擾
			����
				outTime		:	�Փ˂����Ƃ��̎���
				outPos		:	�Փ˂����Ƃ��̍��W
				rayPos		:	Ray�̊�_
				rayDir		:	Ray�̕���
				box			:	AABB
				boxWorld	:	AABB�̃��[���h�s��
			�߂�l
				true�Ȃ�Փ˂��Ă���
		*/
		bool CalcRayBox(float* outTime, D3DXVECTOR3* outPos, const D3DXVECTOR3& rayPos, const D3DXVECTOR3& rayDir, const AABB& box, const D3DXMATRIX& boxWorld);

		/*	Ray�Ƌ��̏Փ˔���ƏՓˍ��W���擾
			����
				outPosS		:	�ՓˊJ�n�_
				outPosE		:	�ՓˏI���_
				rayPos		:	Ray�̊�_
				rayVec		:	Ray�̕���
				sphere		:	��
			�߂�l
				true�Ȃ�Փ˂��Ă���
		*/
		bool CalcRaySphere(D3DXVECTOR3* outPosS, D3DXVECTOR3* outPosE, const D3DXVECTOR3& rayPos, const D3DXVECTOR3& rayVec, const SPHERE& sphere);

		/*	Ray�Ƌ��̏Փ˔���ƏՓˍ��W���擾
			����
				outPosS		:	�ՓˊJ�n�_
				outPosE		:	�ՓˏI���_
				rayPos		:	Ray�̊�_
				rayVec		:	Ray�̕���
				spherePos	:	���̒��S���W
				r			:	���̔��a
			�߂�l
				true�Ȃ�Փ˂��Ă���
		*/
		bool CalcRaySphere(D3DXVECTOR3* outPosS, D3DXVECTOR3* outPosE, const D3DXVECTOR3& rayPos, const D3DXVECTOR3& rayVec, const D3DXVECTOR3& spherePos, const float& r);

		/*	Ray�Ɩ����~���̏Փ˔���Ɗђʓ_���擾
			����
				outPosS	:	�ՓˊJ�n�_
				outPosE	:	�ՓˏI���_
				rayPos	:	Ray�̊�_
				rayVec	:	Ray�̕���
				cylPos1	:	�~������1�_
				cylPos2	:	�~�����̂���1�_
				r		:	�~���̔��a
			�߂�l
				true�Ȃ�Փ˂��Ă���
		*/
		bool CalcRayInfCylinder(D3DXVECTOR3* outPosS, D3DXVECTOR3* outPosE, const D3DXVECTOR3& rayPos, const D3DXVECTOR3& rayVec, const D3DXVECTOR3& cylPos1, const D3DXVECTOR3& cylPos2, const float& r);

		/*	Ray�ƃJ�v�Z���̏Փ˔���Ɗђʓ_���擾
			����
				outPosS		:	�ՓˊJ�n�_
				outPosE		:	�ՓˏI���_
				rayPos		:	Ray�̊�_
				rayVec		:	Ray�̕���
				capPoint1	:	�J�v�Z������1�_
				capPoint2	:	�J�v�Z�����̂���1�_
				r			:	�J�v�Z���̔��a
			�߂�l
				true�Ȃ�Փ˂��Ă���
		*/
		bool CalcRayCapsule(D3DXVECTOR3* outPosS, D3DXVECTOR3* outPosE, const D3DXVECTOR3& rayPos, const D3DXVECTOR3& rayVec, const D3DXVECTOR3& capPoint1, const D3DXVECTOR3& capPoint2, const float& r);

	private:

		/*	�������ɓ��e���ꂽ���������瓊�e���������Z�o
			����
				normSep	:	���K�����ꂽ������
				e1		:	OBB�̎�1
				e2		:	OBB�̎�2
				e3		:	OBB�̎�3
		*/
		float LenSegOnSeparateAxis(const D3DXVECTOR3* normSep, const D3DXVECTOR3* e1, const D3DXVECTOR3* e2, const D3DXVECTOR3* e3 = 0);

		/*	��P1P2P3�̓��ς��擾
			����
				p1	:	�_P1
				p2	:	�_P2
				p3	:	�_P3
			�߂�l
				��P1P2P3�̓��ς�Ԃ�
		*/
		float CalcDot(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3);
	};

	//	���R����
	class FreeFall
	{
	public:
		FreeFall(float unitTime = 1 / 60.f);
		FreeFall(const FreeFall& ff) = delete;

		//	�Z�b�^�[

		//	�ʒu��ݒ�
		void SetPos(D3DXVECTOR3& pos)
		{
			this->pos = pos;
		}
		//	���x��ݒ�
		void SetVelocity(D3DXVECTOR3& vel)
		{
			this->vel = vel;
		}
		//	�����x��ݒ�
		void SetAccel(D3DXVECTOR3& acc)
		{
			this->acc = acc;
		}

		//	�Q�b�^�[

		//	�w�莞���ł̈ʒu���擾
		D3DXVECTOR3& GetPos(D3DXVECTOR3& out, float unitTime);
		//	�w�莞���ł̑��x���擾
		D3DXVECTOR3& GetVelocity(D3DXVECTOR3& out, float unitTime);
		//	���݈ʒu���擾
		D3DXVECTOR3& GetCurPos()
		{
			return pos;
		}
		//	���݂̑��x���擾
		D3DXVECTOR3& GetCurVelocity()
		{
			return vel;
		}
		//	���ݐݒ肳��Ă�������x���擾
		D3DXVECTOR3& GetCurAccel()
		{
			return acc;
		}


		//	�ʒu�����Z
		D3DXVECTOR3& AddPos(D3DXVECTOR3& pos)
		{
			this->pos += pos;
			return this->pos;
		}
		//	���x�����Z
		D3DXVECTOR3& AddVelocity(D3DXVECTOR3& vel)
		{
			this->vel += vel;
			return this->vel;
		}
		//	�����x�����Z
		D3DXVECTOR3& AddAccel(D3DXVECTOR3& acc)
		{
			this->acc += acc;
			return this->acc;
		}

		//	�ʒu�⑬�x�̍X�V
		void Update(float unitTime, bool resetAccel = true);

	private:

		float t;			//	��������
		D3DXVECTOR3	pos;	//	�ʒu
		D3DXVECTOR3 vel;	//	���x
		D3DXVECTOR3	acc;	//	�����x

	};
}
#endif // !_INCLUDE_Physics_h_