//
//	Physics.cpp
//	�����蔻��A���R�����̎���
//
#include<unordered_map>

#include"Physics.h"


namespace Physics
{

	//	���_�t�H�[�}�b�g
	struct VERTEX
	{
		D3DXVECTOR3	position;	//	���W
		D3DXVECTOR3 normal;		//	�@��
		D3DXVECTOR2 uv;			//	�e�N�X�`�����W
	};

	//	��
	SPHERE::SPHERE(const LPD3DXMESH mesh)
		:SPHERE()
	{
		SetMesh(mesh);

	}

	SPHERE::SPHERE(const SPHERE& sphere)
		:mesh(sphere.mesh), pos(sphere.pos), r(sphere.r), center(sphere.center)
	{
	}

	SPHERE::~SPHERE()
	{
	}

	SPHERE& SPHERE::operator=(const SPHERE& sphere)
	{
		mesh = sphere.mesh;
		pos = sphere.pos;
		r = sphere.r;
		center = sphere.center;

		return *this;
	}

	void SPHERE::SetMesh(const LPD3DXMESH& mesh)
	{
		this->mesh = mesh;
	}

	void SPHERE::CreateSphere()
	{
		//	mesh����̏ꍇ�͑���return
		if (mesh == nullptr)
		{
			return;
		}


		DWORD vtxCnt = mesh->GetNumVertices();	//	���_��
		DWORD vtxSize = mesh->GetNumBytesPerVertex();	//	1�̒��_�̃T�C�Y
		BYTE* p;

		//	���_�o�b�t�@�����b�N���A���_�̐擪�A�h���X�擾
		HRESULT hr = mesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&p);
		if (SUCCEEDED(hr))
		{
			D3DXComputeBoundingSphere((D3DXVECTOR3*)p, vtxCnt, vtxSize, &center, &r);

			mesh->UnlockVertexBuffer();

		}
		else if (FAILED(hr))
		{
			mesh->UnlockVertexBuffer();
			return;
		}
	}

	//	AABB
	AABB::AABB(const LPD3DXMESH mesh)
		:AABB()
	{
		SetMesh(mesh);
	}

	AABB::AABB(const AABB& box)
		: mesh(box.mesh), boxMin(box.boxMin), boxMax(box.boxMax), len(box.len)
	{
	}

	AABB::~AABB()
	{
	}

	AABB& AABB::operator=(const AABB& box)
	{
		mesh = box.mesh;
		boxMin = box.boxMin;
		boxMax = box.boxMax;
		len = box.len;

		return *this;
	}

	void AABB::SetMesh(const LPD3DXMESH& mesh)
	{
		this->mesh = mesh;
	}

	void AABB::CreateAABB()
	{

		if (mesh == nullptr)
		{
			return;
		}


		DWORD vtxCnt = mesh->GetNumVertices();
		DWORD vtxSize = mesh->GetNumBytesPerVertex();
		BYTE* p;

		HRESULT hr = mesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&p);

		if (SUCCEEDED(hr))
		{
			D3DXComputeBoundingBox((D3DXVECTOR3*)p, vtxCnt, vtxSize, &boxMin, &boxMax);
			len = boxMax - boxMin;

			mesh->UnlockVertexBuffer();

		}

	}

	//	OBB
	OBB::OBB(const LPD3DXMESH mesh, const D3DXVECTOR3 pos, const float *angle)
		:OBB()
	{
		SetMesh(mesh);
		this->pos = pos;
		for (int i = 0; i < 3; i++)
		{
			this->angle[i] = angle[i];
		}
	}

	OBB::OBB(const OBB& box)
		: mesh(box.mesh), pos(box.pos), halfLen(box.halfLen)
	{
		axis[0] = box.axis[0];
		axis[1] = box.axis[1];
		axis[2] = box.axis[2];
	}

	OBB::~OBB()
	{
	}

	OBB& OBB::operator=(const OBB& box)
	{
		mesh = box.mesh;
		this->pos = box.pos;
		axis[0] = box.axis[0];
		axis[1] = box.axis[1];
		axis[2] = box.axis[2];
		halfLen = box.halfLen;

		return *this;
	}

	void OBB::SetMesh(const LPD3DXMESH& mesh)
	{
		this->mesh = mesh;
	}

	void OBB::CreateOBB()
	{
		VERTEX* v = nullptr;
		D3DXMATRIX matRot;

		D3DXVECTOR3 max = D3DXVECTOR3(-10000.f, -10000.f, -10000.f);
		D3DXVECTOR3 min = D3DXVECTOR3(10000.f, 10000.f, 10000.f);

		if (mesh == nullptr)
		{
			return;
		}

		HRESULT hr = mesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&v);

		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < (int)mesh->GetNumVertices(); i++)
			{
				D3DXVECTOR3 pos = v[i].position;
				if (pos.x < min.x)min.x = pos.x;
				if (pos.x > max.x)max.x = pos.x;
				if (pos.y < min.y)min.y = pos.y;
				if (pos.y > max.y)max.y = pos.y;
				if (pos.z < min.z)min.z = pos.z;
				if (pos.z > max.z)max.z = pos.z;
			}

			mesh->UnlockVertexBuffer();

			center = (min + max)*0.5f + pos;

			D3DXMatrixRotationYawPitchRoll(&matRot, angle[1], angle[0], angle[2]);
			axis[0] = D3DXVECTOR3(matRot._11, matRot._12, matRot._13);
			axis[1] = D3DXVECTOR3(matRot._21, matRot._22, matRot._23);
			axis[2] = D3DXVECTOR3(matRot._31, matRot._32, matRot._33);

			halfLen.x = fabsf(max.x - min.x) * 0.5f;
			halfLen.y = fabsf(max.y - min.y) * 0.5f;
			halfLen.z = fabsf(max.z - min.z) * 0.5f;

		}

	}

	/*	�������ɓ��e���ꂽ���������瓊�e���������Z�o
		����
			normSep	:	���K�����ꂽ������
			e1		:	OBB�̎�1
			e2		:	OBB�̎�2
			e3		:	OBB�̎�3
	*/
	float Collision::LenSegOnSeparateAxis(const D3DXVECTOR3* normSep, const D3DXVECTOR3* e1, const D3DXVECTOR3* e2, const D3DXVECTOR3* e3)
	{
		//	3�̓��ς̐�Βl�̘a�œ��e���������v�Z
		float r1 = fabsf(D3DXVec3Dot(normSep, e1));
		float r2 = fabsf(D3DXVec3Dot(normSep, e2));
		float r3 = e3 ? (fabsf(D3DXVec3Dot(normSep, e3))) : 0;

		return r1 + r2 + r3;
	}

	/*	��P1P2P3�̓��ς��擾
		����
			p1	:	�_P1
			p2	:	�_P2
			p3	:	�_P3
		�߂�l
			��P1P2P3�̓��ς�Ԃ�
	*/
	float Collision::CalcDot(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3)
	{
		return (p1.x - p2.x)*(p3.x - p2.x) + (p1.y - p2.y)*(p3.y - p2.y) + (p1.z - p2.z)*(p3.z - p2.z);
	}

	//	�Փ�
	/*	�ʖ@���x�N�g���̎擾
		����
			out	:	���K�����ꂽ�O�p�`�̖ʖ@���x�N�g����Ԃ��i�߂�l�j
			p0	:	�O�p�`�̒��_0
			p1	:	�O�p�`�̒��_1
			p2	:	�O�p�`�̒��_2
			�O�p�`�𔽎��v���ō���Ă���Ɖ��肵���֐��Ȃ̂Œ��_�̏��Ԃɂ͋C��t���邱��
	*/
	D3DXVECTOR3* Collision::GetPlaneNormal(D3DXVECTOR3* out, const D3DXVECTOR3& p0, const D3DXVECTOR3& p1, const D3DXVECTOR3& p2)
	{
		D3DXVECTOR3 u = p1 - p0;	//	�x�N�g��01
		D3DXVECTOR3 v = p2 - p1;	//	�x�N�g��12

		D3DXVECTOR3 n;	//	�ʖ@���x�N�g�����i�[����ϐ�

		D3DXVec3Cross(&n, &u, &v);

		return D3DXVec3Normalize(out, &n);
	}

	/*	���_�@���x�N�g���̎擾
		����
			num			:	�n���ʖ@���x�N�g���̐�
			out			:	���K�����ꂽ���_�@���x�N�g����Ԃ��i�߂�l�j
			...			:	�ʖ@���x�N�g���@���_�������Ă���ʂ̐������̖@��(D3DXVECTOR3)
	*/
	D3DXVECTOR3* Collision::GetVertexNormal(int num, D3DXVECTOR3* out, ...)
	{
		va_list list;
		int count;

		if (num < 1)
		{
			return NULL;
		}

		va_start(list, out);

		for (count = 0; count < num; ++count)
		{
			*out += va_arg(list, D3DXVECTOR3);
		}

		va_end(list);

		return D3DXVec3Normalize(out, out);
	}

	/*	�_�Ɩ@�����畽�ʂ��쐬
		����
			out			:	���ʂ�Ԃ��i�߂�l�j
			normal		:	�ʖ@���x�N�g��
			point		:	���ʏ�̓_
	*/
	D3DXPLANE* Collision::CreatePlane(D3DXPLANE* out, const D3DXVECTOR3& normal, const D3DXVECTOR3& point)
	{
		D3DXVECTOR3 norm;
		D3DXVec3Normalize(&norm, &normal);
		return D3DXPlaneFromPointNormal(out, &point, &norm);
	}

	/*	����_���畽�ʂ܂ł̋���
		����
			plane	:	�Ώۂ̕���
			point	:	�Ώۂ̓_
		�߂�l
			�_���畽�ʂ܂ł̋�����Ԃ�
	*/
	float Collision::PlanePointDistance(const D3DXPLANE& plane, const D3DXVECTOR3& point)
	{
		D3DXPLANE out;
		D3DXPlaneNormalize(&out, &plane);
		return D3DXPlaneDotCoord(&plane, &point);
	}

	/*	�ǂ���x�N�g��
		����
			out		:	���K�����ꂽ�ǂ���x�N�g���i�߂�l�j
			front	:	�i�s�x�N�g��
			normal	:	�Փ˓_�̖@���x�N�g��
	*/
	D3DXVECTOR3* Collision::WallRubVector(D3DXVECTOR3* out, const D3DXVECTOR3& front, const D3DXVECTOR3& normal)
	{
		D3DXVECTOR3 norm;
		//	�Փ˓_�̖@���x�N�g���𐳋K��
		D3DXVec3Normalize(&norm, &normal);

		//	�ǂ���x�N�g�� = �i�s�x�N�g�� - Dot( �i�s�x�N�g��, norm ) * norm
		return D3DXVec3Normalize(out, &(front - D3DXVec3Dot(&front, &norm)*norm));
	}

	/*	���˃x�N�g��
		����
			out		:	���K�����ꂽ���˃x�N�g���i�߂�l�j
			front	:	�i�s�x�N�g��
			normal	:	�Փ˓_�̖@���x�N�g��
	*/
	D3DXVECTOR3* Collision::ReflectVector(D3DXVECTOR3* out, const D3DXVECTOR3& front, const D3DXVECTOR3& normal)
	{
		D3DXVECTOR3 norm;
		//	�Փ˓_�̖@���x�N�g���𐳋K��
		D3DXVec3Normalize(&norm, &normal);

		//	���˃x�N�g�� = �i�s�x�N�g�� - 2Dot( �i�s�x�N�g��, norm ) * norm
		return D3DXVec3Normalize(out, &(front - 2.f * D3DXVec3Dot(&front, &norm)*norm));
	}

	/*	���ʂƓ_�̈ʒu�֌W�𒲂ׂ�
		����
			plane	:	�Ώۂ̕���
			point	:	�Ώۂ̓_
		�߂�l
		+ �͕\  - �͗�  0 �͕��ʏ�ɑ���
	*/
	float Collision::PlanePoint(const D3DXPLANE& plane, const D3DXVECTOR3& point)
	{
		return D3DXPlaneDotCoord(&plane, &point);
	}

	/*	���Ɠ_�̏Փ˔���
		����
			linePoint	:	����̓_
			vec			:	���̕����������x�N�g��
			point		:	�Փ˂��Ă邩�ǂ������ׂ�Ώۂ̓_
		�߂�l
			true�̂Ƃ��Փ�
	*/
	bool Collision::LinePoint(const D3DXVECTOR3& linePoint, const D3DXVECTOR3& vec, const D3DXVECTOR3& point)
	{
		D3DXVECTOR3 v1 = point - linePoint;
		v1 = { fabsf(v1.x), fabsf(v1.y), fabsf(v1.z) };
		D3DXVECTOR3 v0(0.f, 0.f, 0.f);
		D3DXVECTOR3 vector = { fabsf(vec.x), fabsf(vec.y), fabsf(vec.z) };
		D3DXVec3Normalize(&vector, &vector);
		D3DXVECTOR3 out = {};
		D3DXVec3Cross(&out, &v1, &vector);
		if (out == v0)
		{
			return true;
		}

		return false;
	}

	/*	�����Ɠ_�̏Փ˔���
		����
			ps		:	������Start�_
			pe		:	������End�_
			point	:	�Փ˂��Ă邩�ǂ������ׂ�Ώۂ̓_
		�߂�l
			true�̂Ƃ��Փ�
	*/
	bool Collision::LineSegmentPoint(const D3DXVECTOR3& ps, const D3DXVECTOR3& pe, const D3DXVECTOR3& point)
	{
		D3DXVECTOR3 v = pe - ps;
		v = { fabsf(v.x), fabsf(v.y), fabsf(v.z) };
		D3DXVECTOR3 v1 = point - ps;
		v1 = { fabsf(v1.x), fabsf(v1.y), fabsf(v1.z) };
		if (LinePoint(ps, v, point))
		{
			if (v1.x <= v.x && v1.y <= v.y && v1.z <= v.z)
			{
				return true;
			}
			return false;
		}
		return false;
	}

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
	bool Collision::CalcLineCom(float* outDis, D3DXVECTOR3* outPos1, D3DXVECTOR3* outPos2, const D3DXVECTOR3& linePoint1, const D3DXVECTOR3& lineVec1, const D3DXVECTOR3& linePoint2, const D3DXVECTOR3& lineVec2)
	{
		D3DXVECTOR3 v1, v2;
		D3DXVECTOR3 p1 = linePoint1, p2 = linePoint2;

		D3DXVec3Normalize(&v1, &lineVec1);
		D3DXVec3Normalize(&v2, &lineVec2);

		float d1 = D3DXVec3Dot(&(p2 - p1), &v1);
		float d2 = D3DXVec3Dot(&(p2 - p1), &v2);
		D3DXVECTOR3 cross;
		float dv = D3DXVec3LengthSq(D3DXVec3Cross(&cross, &v1, &v2));

		if (dv < EPSIRON)
		{
			if (*outDis)
			{
				D3DXVECTOR3 v;
				*outDis = D3DXVec3Length(D3DXVec3Cross(&v, &(p2 - p1), &v1));
			}
			return false;
		}

		float dv2 = D3DXVec3Dot(&v1, &v2);
		float t1 = (d1 - d2 * dv2) / (1.f - dv2 * dv2);
		float t2 = (d2 - d1 * dv2) / (dv2*dv2 - 1.f);

		D3DXVECTOR3 q1 = p1 + t1 * v1, q2 = p2 + t2 * v2;

		if (*outDis)
		{
			*outDis = D3DXVec3Length(&(q2 - q1));
		}

		if (*outPos1)
		{
			*outPos1 = q1;
		}

		if (*outPos2)
		{
			*outPos2 = q2;
		}

		return true;
	}

	/*	���ƕ��ʂ̏Փ˔��� �����Փ˂��Ă邩�ǂ����̂ݒm���
		����
			planePoint		:	���ʏ�̓_
			normal			:	���ʂ̖@���x�N�g��
			linePoint		:	����̓_
			vec				:	���̕����������x�N�g��
		�߂�l
			true�̂Ƃ��Փ�
	*/
	bool Collision::PlaneLine(const D3DXVECTOR3& planePoint, const D3DXVECTOR3& normal, const D3DXVECTOR3& linePoint, const D3DXVECTOR3& vec)
	{
		D3DXVECTOR3 v1 = planePoint - linePoint;
		D3DXVECTOR3 norm;
		D3DXVECTOR3 v;
		D3DXVec3Normalize(&norm, &normal);
		D3DXVec3Normalize(&v, &vec);

		if (D3DXVec3Dot(&v1, &norm) == 0)
		{
			return true;
		}
		else if (D3DXVec3Dot(&v, &norm) != 0)
		{
			return true;
		}

		return false;
	}

	/*	�����ƕ��ʂ̏Փ� �����ƕ��ʂ̏Փ˓_���킩��
		����
			out		:	�Փ˓_��Ԃ��A�Փ˂��Ă��Ȃ������ꍇ��NULL(�߂�l)
			plane	:	�Ώۂ̕���
			ps		:	������Start�_
			pe		:	������End�_
	*/
	D3DXVECTOR3* Collision::PlaneLineSegment(D3DXVECTOR3* out, const D3DXPLANE& plane, const D3DXVECTOR3& ps, const D3DXVECTOR3& pe)
	{
		return D3DXPlaneIntersectLine(out, &plane, &ps, &pe);
	}

	/*	���Ƌ��̏Փ˔���
		����
			sphere1	:	��1
			sphere2	:	��2
		�߂�l
			true�̂Ƃ��Փ˂��Ă���
	*/
	bool Collision::SphereCom(const SPHERE& sphere1, const SPHERE& sphere2)
	{
		float p, r;
		p = powf((sphere2.pos.x + sphere2.center.x) - (sphere1.pos.x + sphere1.center.x), 2.f) + powf((sphere2.pos.y + sphere2.center.y) - (sphere1.pos.y + sphere1.center.y), 2.f) + powf((sphere2.pos.z + sphere2.center.z) - (sphere1.pos.z + sphere1.center.z), 2.f);
		r = powf(sphere1.r + sphere2.r, 2.f);
		
		if (p <= r)
		{
			return true;
		}

		return false;
	}

	/* ���|���S���̍������擾
		����
			normal		:	�ʖ@���x�N�g��
			point		:	x,z�݂̂킩���Ă���|���S���Ɋ܂܂ꂽ�_(y���܂܂�Ă��邩�ǂ����͊֌W�Ȃ�)
			p0			:	�ʖ@���x�N�g���̖ʂ̒��_0
		�߂�l
			���|���S����point������y������Ԃ�
	*/
	float Collision::GetFloorHeight(const D3DXVECTOR3& normal, const D3DXVECTOR3& point, const D3DXVECTOR3& p0)
	{
		D3DXVECTOR3 norm;
		D3DXVECTOR3 ao = point - p0;
		float height;

		D3DXVec3Normalize(&norm, &normal);
		if (norm.y < 0)
		{
			norm = -norm;
		}
		float vec = D3DXVec3Dot(&ao, &norm) - ao.y*norm.y;
		height = p0.y - ((1 / norm.y)* vec);

		return height;
	}

	/*	���ƕ��ʂ̏Փ˂�����W�Ǝ������擾
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
	bool Collision::CalcSpherePlane(float* outTime, D3DXVECTOR3* outPos, const SPHERE& sphere, const D3DXVECTOR3& spherePrePos, const D3DXVECTOR3& sphereNextPos, const D3DXVECTOR3& normal, const D3DXPLANE& plane)
	{
		D3DXVECTOR3 part = (sphereNextPos + sphere.center) - (spherePrePos + sphere.center);
		D3DXVECTOR3 norm;

		D3DXVec3Normalize(&norm, &normal);

		float dis = PlanePointDistance(plane, spherePrePos + sphere.center);
		float fabDis = fabsf(dis);

		float dot = D3DXVec3Dot(&part, &norm);

		if ((EPSIRON - fabsf(dot) > 0) && (fabDis < sphere.r))
		{
			*outTime = FLT_MAX;
			*outPos = spherePrePos + sphere.center;
			return true;
		}

		*outTime = (sphere.r - dis) / dot;

		*outPos = (spherePrePos + sphere.center) + (*outTime)*part;

		if (fabDis < sphere.r)
		{
			return true;
		}

		if (dot >= 0)
		{
			return false;
		}

		if ((0 <= *outTime) && (*outTime <= 1))
		{
			return true;
		}

		return false;
	}

	/*	AABB�Ɠ_�Ƃ̍ŒZ����
		����
			box		:	AABB
			point	:	�_
		�߂�l
			AABB�Ɠ_�̋�����Ԃ�	0�̂Ƃ�AABB���ɓ_������
	*/
	float Collision::AABBPointDistance(const AABB& box, const D3DXVECTOR3& point)
	{
		float sqDis = 0;

		if (point.x < box.boxMin.x + box.pos.x)
		{
			sqDis += (point.x - (box.boxMin.x + box.pos.x))*(point.x - (box.boxMin.x + box.pos.x));
		}
		if (point.x > box.boxMax.x + box.pos.x)
		{
			sqDis += (point.x - (box.boxMax.x + box.pos.x))*(point.x - (box.boxMax.x + box.pos.x));
		}

		if (point.y < box.boxMin.y + box.pos.y)
		{
			sqDis += (point.y - (box.boxMin.y + box.pos.y))*(point.y - (box.boxMin.y + box.pos.y));
		}
		if (point.y > box.boxMax.y + box.pos.y)
		{
			sqDis += (point.y - (box.boxMax.y + box.pos.y))*(point.y - (box.boxMax.y + box.pos.y));
		}

		if (point.z < box.boxMin.z + box.pos.z)
		{
			sqDis += (point.z - (box.boxMin.z + box.pos.z))*(point.z - (box.boxMin.z + box.pos.z));
		}
		if (point.z > box.boxMax.z + box.pos.z)
		{
			sqDis += (point.z - (box.boxMax.z + box.pos.z))*(point.z - (box.boxMax.z + box.pos.z));
		}

		return sqrtf(sqDis);
	}

	/*	AABB���m�̏Փ˔���
		����
			box1	:	AABB1
			box2	:	AABB2
		�߂�l
			true�̂Ƃ��Փ�
	*/
	bool Collision::AABBCom(const AABB& box1, const AABB& box2)
	{
		if (box1.boxMin.x + box1.pos.x > box2.boxMax.x + box2.pos.x)return false;
		if (box1.boxMax.x + box1.pos.x < box2.boxMin.x + box2.pos.x)return false;
		if (box1.boxMin.y + box1.pos.y > box2.boxMax.y + box2.pos.y)return false;
		if (box1.boxMax.y + box1.pos.y < box2.boxMin.y + box2.pos.y)return false;
		if (box1.boxMin.z + box1.pos.z > box2.boxMax.z + box2.pos.z)return false;
		if (box1.boxMax.z + box1.pos.z < box2.boxMin.z + box2.pos.z)return false;

		return true;
	}

	/*	OBB�Ɠ_�̍ŒZ����
		����
			box		:	OBB
			point	:	�_
		�߂�l
			OBB�Ɠ_�Ƃ̍ŒZ������float�^�ŕԂ�
	*/
	float Collision::OBBPointDistance(const OBB& box, const D3DXVECTOR3& point)
	{
		D3DXVECTOR3 vec(0, 0, 0);

		for (int i = 0; i < 3; i++)
		{
			float l = box.halfLen[i];
			if (l <= 0)continue;
			float s = D3DXVec3Dot(&(point - box.pos), &box.axis[i]) / l;

			s = fabsf(s);
			if (s > 1)vec += (1 - s)*l*box.axis[i];
		}

		return D3DXVec3Length(&vec);
	}

	/*	OBB��OBB�̏Փ˔���
		����
			box1	:	OBB1
			box2	:	OBB2
		�߂�l
			true�̂Ƃ��Փ�
	*/
	bool Collision::OBBCom(const OBB& box1, const OBB& box2)
	{
		//	�e�����x�N�g���̊m��
		D3DXVECTOR3 nAe1 = *D3DXVec3Normalize(&nAe1, &box1.axis[0]), ae1 = nAe1 * box1.halfLen[0];
		D3DXVECTOR3 nAe2 = *D3DXVec3Normalize(&nAe2, &box1.axis[1]), ae2 = nAe2 * box1.halfLen[1];
		D3DXVECTOR3 nAe3 = *D3DXVec3Normalize(&nAe3, &box1.axis[2]), ae3 = nAe3 * box1.halfLen[2];
		D3DXVECTOR3 nBe1 = *D3DXVec3Normalize(&nBe1, &box1.axis[0]), be1 = nBe1 * box1.halfLen[0];
		D3DXVECTOR3 nBe2 = *D3DXVec3Normalize(&nBe2, &box1.axis[1]), be2 = nBe2 * box1.halfLen[1];
		D3DXVECTOR3 nBe3 = *D3DXVec3Normalize(&nBe3, &box1.axis[2]), be3 = nBe3 * box1.halfLen[2];
		D3DXVECTOR3 interval = box1.pos - box2.pos;

		//	�������FAe1
		float rA = D3DXVec3Length(&ae1);
		float rB = LenSegOnSeparateAxis(&nAe1, &be1, &be2, &be3);
		float l = fabsf(D3DXVec3Dot(&interval, &nAe1));
		if (l > rA + rB)return false;

		//	�������FAe2
		rA = D3DXVec3Length(&ae2);
		rB = LenSegOnSeparateAxis(&nAe2, &be1, &be2, &be3);
		l = fabsf(D3DXVec3Dot(&interval, &nAe2));
		if (l > rA + rB)return false;

		//	�������FAe3
		rA = D3DXVec3Length(&ae3);
		rB = LenSegOnSeparateAxis(&nAe3, &be1, &be2, &be3);
		l = fabsf(D3DXVec3Dot(&interval, &nAe3));
		if (l > rA + rB)return false;

		//	�������FBe1
		rA = LenSegOnSeparateAxis(&nBe1, &ae1, &ae2, &ae3);
		rB = D3DXVec3Length(&be1);
		l = fabsf(D3DXVec3Dot(&interval, &nBe1));
		if (l > rA + rB)return false;

		//	�������FBe2
		rA = LenSegOnSeparateAxis(&nBe1, &ae1, &ae2, &ae3);
		rB = D3DXVec3Length(&be2);
		l = fabsf(D3DXVec3Dot(&interval, &nBe2));
		if (l > rA + rB)return false;

		//	�������FBe3
		rA = LenSegOnSeparateAxis(&nBe1, &ae1, &ae2, &ae3);
		rB = D3DXVec3Length(&be3);
		l = fabsf(D3DXVec3Dot(&interval, &nBe3));
		if (l > rA + rB)return false;

		//	�������FC11
		D3DXVECTOR3 cross;
		D3DXVec3Cross(&cross, &nAe1, &nBe1);
		rA = LenSegOnSeparateAxis(&cross, &ae2, &ae3);
		rB = LenSegOnSeparateAxis(&cross, &be2, &be3);
		l = fabsf(D3DXVec3Dot(&interval, &cross));
		if (l > rA + rB)return false;

		//	�������FC12
		D3DXVec3Cross(&cross, &nAe1, &nBe2);
		rA = LenSegOnSeparateAxis(&cross, &ae2, &ae3);
		rB = LenSegOnSeparateAxis(&cross, &be1, &be3);
		l = fabsf(D3DXVec3Dot(&interval, &cross));
		if (l > rA + rB)return false;

		//	�������FC13
		D3DXVec3Cross(&cross, &nAe1, &nBe3);
		rA = LenSegOnSeparateAxis(&cross, &ae2, &ae3);
		rB = LenSegOnSeparateAxis(&cross, &be1, &be2);
		l = fabsf(D3DXVec3Dot(&interval, &cross));
		if (l > rA + rB)return false;

		//	�������FC21
		D3DXVec3Cross(&cross, &nAe2, &nBe1);
		rA = LenSegOnSeparateAxis(&cross, &ae1, &ae3);
		rB = LenSegOnSeparateAxis(&cross, &be2, &be3);
		l = fabsf(D3DXVec3Dot(&interval, &cross));
		if (l > rA + rB)return false;

		//	�������FC22
		D3DXVec3Cross(&cross, &nAe2, &nBe2);
		rA = LenSegOnSeparateAxis(&cross, &ae1, &ae3);
		rB = LenSegOnSeparateAxis(&cross, &be1, &be3);
		l = fabsf(D3DXVec3Dot(&interval, &cross));
		if (l > rA + rB)return false;

		//	�������FC23
		D3DXVec3Cross(&cross, &nAe2, &nBe3);
		rA = LenSegOnSeparateAxis(&cross, &ae1, &ae3);
		rB = LenSegOnSeparateAxis(&cross, &be1, &be2);
		l = fabsf(D3DXVec3Dot(&interval, &cross));
		if (l > rA + rB)return false;

		//	�������FC31
		D3DXVec3Cross(&cross, &nAe3, &nBe1);
		rA = LenSegOnSeparateAxis(&cross, &ae1, &ae2);
		rB = LenSegOnSeparateAxis(&cross, &be2, &be3);
		l = fabsf(D3DXVec3Dot(&interval, &cross));
		if (l > rA + rB)return false;

		//	�������FC32
		D3DXVec3Cross(&cross, &nAe3, &nBe2);
		rA = LenSegOnSeparateAxis(&cross, &ae1, &ae2);
		rB = LenSegOnSeparateAxis(&cross, &be1, &be3);
		l = fabsf(D3DXVec3Dot(&interval, &cross));
		if (l > rA + rB)return false;

		//	�������FC33
		D3DXVec3Cross(&cross, &nAe3, &nBe3);
		rA = LenSegOnSeparateAxis(&cross, &ae1, &ae2);
		rB = LenSegOnSeparateAxis(&cross, &be1, &be2);
		l = fabsf(D3DXVec3Dot(&interval, &cross));
		if (l > rA + rB)return false;

		//	�������ʂ����݂��Ȃ��̂ŏՓ˂��Ă���
		return true;
	}

	/*	OBB�ƕ��ʂ̏Փ˔���
		����
			box		:	OBB
			pos		:	���ʏ�̓_
			normal	:	���ʂ̐��K�����ꂽ�@��
			len		:	OBB�ƕ��ʂ��Փ˂��Ă����ꍇ�ǂɂ߂荞�񂾕���߂����߂̋������Z�o
		�߂�l
			true�̂Ƃ��Փ�
	*/
	bool Collision::OBBPlane(const OBB& box, const D3DXVECTOR3& pos, const D3DXVECTOR3& normal, float* len)
	{
		float r = 0.f;
		for (int i = 0; i < 3; i++)
		{
			D3DXVECTOR3 dir = box.axis[i];
			r += fabsf(D3DXVec3Dot(&(dir*box.halfLen[i]), &normal));
		}

		//	���ʂ�OBB�̋������Z�o
		float s = D3DXVec3Dot(&(box.pos - pos), &normal);

		//	�߂��������Z�o
		if (len != NULL)
		{
			if (s > 0)
			{
				*len = r - fabsf(s);
			}
			else
			{
				*len = r + fabsf(s);
			}
		}

		//	�Փ˔���
		if (fabsf(s) - r < EPSIRON)
		{
			return true;
		}

		return false;
	}

	/*	OBB�Ƌ��̏Փ˔���
		����
			box		:	OBB
			sphere	:	��
		�߂�l
			true�̎��Փ�
	*/
	bool Collision::OBBSphere(const OBB& box, const SPHERE& sphere)
	{
		auto distance = OBBPointDistance(box, sphere.center + sphere.pos);
		return distance <= sphere.r;
	}

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
	bool Collision::CalcSphereLine(float* outTime, D3DXVECTOR3* outPos, const SPHERE& sphere, const D3DXVECTOR3& spherePrePos, const D3DXVECTOR3& sphereNextPos, const D3DXVECTOR3& linePoint, const D3DXVECTOR3& lineDirect)
	{
		D3DXVECTOR3 e = (sphereNextPos + sphere.center) - (spherePrePos + sphere.center);
		D3DXVECTOR3 v;
		D3DXVec3Normalize(&v, &lineDirect);
		D3DXVECTOR3 r = linePoint - (spherePrePos + sphere.center);
		D3DXVECTOR3 a = D3DXVec3Dot(&e, &v)*v - e;
		D3DXVECTOR3 c = r - D3DXVec3Dot(&r, &v)*v;
		float alpha = D3DXVec3LengthSq(&a);
		float beta = D3DXVec3Dot(&a, &c);
		float omega = D3DXVec3LengthSq(&c) - sphere.r * sphere.r;

		//	�Փ˔���
		float tmp = beta * beta - alpha * omega;

		//	�Փ˂��Ȃ����~�܂��Ă���ꍇ�͕s���l��Ԃ�
		if (fabsf(alpha) <= EPSIRON || tmp < 0)
		{
			*outTime = FLT_MAX;
			*outPos = (spherePrePos + sphere.center);
			if (omega < EPSIRON)
			{
				return true;
			}
			return false;
		}

		//	�Փˎ������Z�o
		*outTime = (-sqrtf(tmp) - beta) / alpha;

		//	�Փˎ�����0�ȏ�1�ȉ��Ȃ�ΏՓ�
		*outPos = (spherePrePos + sphere.center) + *outTime*e;
		if (*outTime >= EPSIRON&&*outTime <= 1.f)
		{
			return true;
		}

		//	���݈ʒu�ł߂荞��ł��邩�`�F�b�N
		if (omega < EPSIRON)
		{
			return true;
		}

		return false;
	}

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
	bool Collision::CalcRayBox(float* outTime, D3DXVECTOR3* outPos, const D3DXVECTOR3& rayPos, const D3DXVECTOR3& rayDir, const AABB& box, const D3DXMATRIX& boxWorld)
	{
		D3DXMATRIX invMtx;
		D3DXMatrixInverse(&invMtx, 0, &boxWorld);

		D3DXVECTOR3 pos, dir;
		D3DXVec3TransformCoord(&pos, &rayPos, &invMtx);
		invMtx._41 = 0.f;
		invMtx._42 = 0.f;
		invMtx._43 = 0.f;
		D3DXVec3TransformCoord(&dir, &rayDir, &invMtx);

		//	��������
		float p[3], d[3], min[3], max[3];
		memcpy(p, &pos, sizeof(D3DXVECTOR3));
		memcpy(d, &dir, sizeof(D3DXVECTOR3));
		memcpy(min, &(box.boxMin + box.pos), sizeof(D3DXVECTOR3));
		memcpy(max, &(box.boxMax + box.pos), sizeof(D3DXVECTOR3));

		*outTime = -FLT_MAX;
		float timeMax = FLT_MAX;

		for (int i = 0; i < 3; ++i)
		{
			if (abs(d[i]) < FLT_EPSILON)
			{
				if (p[i]<min[i] || p[i]>max[i])
				{
					return false;
				}
			}
			else
			{
				//	�X���u�Ƃ̋������Z�o
				//	t1���߃X���u�At2�����X���u�Ƃ̋���
				float odd = 1.f / d[i];
				float t1 = (min[i] - p[i])*odd;
				float t2 = (max[i] - p[i])*odd;
				if (t1 > t2)
				{
					float tmp = t1;
					t1 = t2;
					t2 = tmp;
				}

				if (t1 > *outTime)
				{
					*outTime = t1;
				}
				if (t2 < timeMax)
				{
					timeMax = t2;
				}

				//	�X���u�����`�F�b�N
				if (*outTime >= timeMax)
				{
					return false;
				}
			}

		}

		//	�������Ă���
		if (*outPos)
		{
			*outPos = rayPos + *outTime * (rayDir);
		}

		return true;
	}

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
	bool Collision::CalcRaySphere(D3DXVECTOR3* outPosS, D3DXVECTOR3* outPosE, const D3DXVECTOR3& rayPos, const D3DXVECTOR3& rayVec, const SPHERE& sphere)
	{
		D3DXVECTOR3 p = (sphere.center+sphere.pos) - rayPos;

		float a = D3DXVec3Dot(&rayVec, &rayVec);
		float b = D3DXVec3Dot(&rayVec, &p);
		float c = D3DXVec3Dot(&p, &p) - pow(sphere.r, 2);

		if (a == 0.f)
		{
			return false;	//	���C�̒�����0
		}

		float s = b * b - a * c;
		if (s < EPSIRON)
		{
			return false;
		}

		s = sqrtf(s);
		float a1 = (b - s) / a;
		float a2 = (b + s) / a;

		if (a1 < EPSIRON || a2 < EPSIRON)
		{
			return false;
		}

		*outPosS = rayPos + a1 * rayVec;
		*outPosE = rayPos + a2 * rayVec;

		return true;
	}

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
	bool Collision::CalcRaySphere(D3DXVECTOR3* outPosS, D3DXVECTOR3* outPosE, const D3DXVECTOR3& rayPos, const D3DXVECTOR3& rayVec, const D3DXVECTOR3& spherePos, const float& r)
	{
		D3DXVECTOR3 p = spherePos - rayPos;

		float a = D3DXVec3Dot(&rayVec, &rayVec);
		float b = D3DXVec3Dot(&rayVec, &p);
		float c = D3DXVec3Dot(&p, &p) - pow(r, 2);

		if (a == 0.f)
		{
			return false;	//	���C�̒�����0
		}

		float s = b * b - a * c;
		if (s < EPSIRON)
		{
			return false;
		}

		s = sqrtf(s);
		float a1 = (b - s) / a;
		float a2 = (b + s) / a;

		if (a1 < EPSIRON || a2 < EPSIRON)
		{
			return false;
		}

		*outPosS = rayPos + a1 * rayVec;
		*outPosE = rayPos + a2 * rayVec;

		return true;
	}

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
	bool Collision::CalcRayInfCylinder(D3DXVECTOR3* outPosS, D3DXVECTOR3* outPosE, const D3DXVECTOR3& rayPos, const D3DXVECTOR3& rayVec, const D3DXVECTOR3& cylPos1, const D3DXVECTOR3& cylPos2, const float& r)
	{
		D3DXVECTOR3 p = cylPos1 - rayPos;
		D3DXVECTOR3 p2 = cylPos2 - rayPos;
		D3DXVECTOR3 s = p2 - p;

		float dvv = D3DXVec3Dot(&rayVec, &rayVec);
		float dsv = D3DXVec3Dot(&s, &rayVec);
		float dpv = D3DXVec3Dot(&p, &rayVec);
		float dss = D3DXVec3Dot(&s, &s);
		float dps = D3DXVec3Dot(&p, &s);
		float dpp = D3DXVec3Dot(&p, &p);

		if (dss == 0.f)
		{
			return false;
		}

		float a = dvv - dsv * dsv / dss;
		float b = dpv - dps * dsv / dss;
		float c = dpp - dps * dps / dss - pow(r, 2);

		if (a == 0.f)
		{
			return false;
		}

		float ss = b * b - a * c;
		if (ss < EPSIRON)
		{
			return false;
		}
		ss = sqrtf(ss);

		float a1 = (b - ss) / a;
		float a2 = (b + ss) / a;

		*outPosS = rayPos + a1 * rayVec;
		*outPosE = rayPos + a2 * rayVec;

		return true;
	}

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
	bool Collision::CalcRayCapsule(D3DXVECTOR3* outPosS, D3DXVECTOR3* outPosE, const D3DXVECTOR3& rayPos, const D3DXVECTOR3& rayVec, const D3DXVECTOR3& capPoint1, const D3DXVECTOR3& capPoint2, const float& r)
	{
		bool q1Inp1 = false;
		bool q1Inp2 = false;
		bool q1InCld = false;

		//	Q1�̌���
		if (CalcRaySphere(outPosS, outPosE, rayPos, rayVec, capPoint1, r) == true && CalcDot(capPoint2, capPoint1, *outPosS) <= EPSIRON)
		{
			q1Inp1 = true;
		}
		else if (CalcRaySphere(outPosS, outPosE, rayPos, rayVec, capPoint2, r) == true && CalcDot(capPoint1, capPoint2, *outPosS) <= EPSIRON)
		{
			q1Inp2 = true;
		}
		else if (CalcRayInfCylinder(outPosS, outPosE, rayPos, rayVec, capPoint1, capPoint2, r) == true && CalcDot(capPoint1, capPoint2, *outPosS) > EPSIRON&&CalcDot(capPoint2, capPoint1, *outPosS) > EPSIRON)
		{
			q1InCld = true;
		}
		else
		{
			return false;
		}

		//	Q2�̌���
		D3DXVECTOR3 damy;
		if (q1Inp1&&CalcDot(capPoint2, capPoint1, *outPosE) <= EPSIRON)
		{
			return true;
		}
		else if (q1Inp2&&CalcDot(capPoint1, capPoint2, *outPosE) <= EPSIRON)
		{
			return true;
		}
		else if (q1InCld&&CalcDot(capPoint1, capPoint2, *outPosE) > EPSIRON&&CalcDot(capPoint2, capPoint1, *outPosE) > EPSIRON)
		{
			return true;
		}
		else if (CalcRaySphere(&damy, outPosE, rayPos, rayVec, capPoint1, r) == true && CalcDot(capPoint2, capPoint1, *outPosE) <= EPSIRON)
		{
			return true;
		}
		else if (CalcRaySphere(&damy, outPosE, rayPos, rayVec, capPoint2, r) == true && CalcDot(capPoint1, capPoint2, *outPosE) <= EPSIRON)
		{
			return true;
		}

		CalcRayInfCylinder(&damy, outPosE, rayPos, rayVec, capPoint1, capPoint2, r);

		return true;
	}

	//	���R����
	FreeFall::FreeFall(float unitTime)
		:t(unitTime)
	{
		memset(&pos, 0, sizeof(D3DXVECTOR3));
		memset(&vel, 0, sizeof(D3DXVECTOR3));
		memset(&acc, 0, sizeof(D3DXVECTOR3));
	}

	void FreeFall::Update(float unitTime, bool resetAccel)
	{
		//	�ʒu���Z�o
		GetPos(pos, unitTime);

		//	�����x���瑬�x���Z�o
		vel += acc * unitTime;

		//	�����x�����Z�b�g
		if (resetAccel)
		{
			acc.x = acc.y = acc.z = 0.0f;
		}
	}

	D3DXVECTOR3& FreeFall::GetPos(D3DXVECTOR3& out, float unitTime)
	{
		//	�������x�^���̎�
		D3DXVECTOR3 next = vel * t + 0.5f * acc * t * t;
		out = pos + unitTime / t * next;
		return out;
	}

	D3DXVECTOR3& FreeFall::GetVelocity(D3DXVECTOR3& out, float unitTime)
	{
		D3DXVECTOR3 next = acc * t;
		out = vel + unitTime / t * next;
		return out;
	}

}