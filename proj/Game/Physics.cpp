//
//	Physics.cpp
//	当たり判定、自由落下の実装
//
#include<unordered_map>

#include"Physics.h"


namespace Physics
{

	//	頂点フォーマット
	struct VERTEX
	{
		D3DXVECTOR3	position;	//	座標
		D3DXVECTOR3 normal;		//	法線
		D3DXVECTOR2 uv;			//	テクスチャ座標
	};

	//	球
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
		//	meshが空の場合は即時return
		if (mesh == nullptr)
		{
			return;
		}


		DWORD vtxCnt = mesh->GetNumVertices();	//	頂点数
		DWORD vtxSize = mesh->GetNumBytesPerVertex();	//	1つの頂点のサイズ
		BYTE* p;

		//	頂点バッファをロックし、頂点の先頭アドレス取得
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

	/*	分離軸に投影された軸成分から投影線分長を算出
		引数
			normSep	:	正規化された分離軸
			e1		:	OBBの軸1
			e2		:	OBBの軸2
			e3		:	OBBの軸3
	*/
	float Collision::LenSegOnSeparateAxis(const D3DXVECTOR3* normSep, const D3DXVECTOR3* e1, const D3DXVECTOR3* e2, const D3DXVECTOR3* e3)
	{
		//	3つの内積の絶対値の和で投影線分長を計算
		float r1 = fabsf(D3DXVec3Dot(normSep, e1));
		float r2 = fabsf(D3DXVec3Dot(normSep, e2));
		float r3 = e3 ? (fabsf(D3DXVec3Dot(normSep, e3))) : 0;

		return r1 + r2 + r3;
	}

	/*	∠P1P2P3の内積を取得
		引数
			p1	:	点P1
			p2	:	点P2
			p3	:	点P3
		戻り値
			∠P1P2P3の内積を返す
	*/
	float Collision::CalcDot(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3)
	{
		return (p1.x - p2.x)*(p3.x - p2.x) + (p1.y - p2.y)*(p3.y - p2.y) + (p1.z - p2.z)*(p3.z - p2.z);
	}

	//	衝突
	/*	面法線ベクトルの取得
		引数
			out	:	正規化された三角形の面法線ベクトルを返す（戻り値）
			p0	:	三角形の頂点0
			p1	:	三角形の頂点1
			p2	:	三角形の頂点2
			三角形を反時計回りで作っていると仮定した関数なので頂点の順番には気を付けること
	*/
	D3DXVECTOR3* Collision::GetPlaneNormal(D3DXVECTOR3* out, const D3DXVECTOR3& p0, const D3DXVECTOR3& p1, const D3DXVECTOR3& p2)
	{
		D3DXVECTOR3 u = p1 - p0;	//	ベクトル01
		D3DXVECTOR3 v = p2 - p1;	//	ベクトル12

		D3DXVECTOR3 n;	//	面法線ベクトルを格納する変数

		D3DXVec3Cross(&n, &u, &v);

		return D3DXVec3Normalize(out, &n);
	}

	/*	頂点法線ベクトルの取得
		引数
			num			:	渡す面法線ベクトルの数
			out			:	正規化された頂点法線ベクトルを返す（戻り値）
			...			:	面法線ベクトル　頂点が属している面の数だけの法線(D3DXVECTOR3)
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

	/*	点と法線から平面を作成
		引数
			out			:	平面を返す（戻り値）
			normal		:	面法線ベクトル
			point		:	平面上の点
	*/
	D3DXPLANE* Collision::CreatePlane(D3DXPLANE* out, const D3DXVECTOR3& normal, const D3DXVECTOR3& point)
	{
		D3DXVECTOR3 norm;
		D3DXVec3Normalize(&norm, &normal);
		return D3DXPlaneFromPointNormal(out, &point, &norm);
	}

	/*	ある点から平面までの距離
		引数
			plane	:	対象の平面
			point	:	対象の点
		戻り値
			点から平面までの距離を返す
	*/
	float Collision::PlanePointDistance(const D3DXPLANE& plane, const D3DXVECTOR3& point)
	{
		D3DXPLANE out;
		D3DXPlaneNormalize(&out, &plane);
		return D3DXPlaneDotCoord(&plane, &point);
	}

	/*	壁ずりベクトル
		引数
			out		:	正規化された壁ずりベクトル（戻り値）
			front	:	進行ベクトル
			normal	:	衝突点の法線ベクトル
	*/
	D3DXVECTOR3* Collision::WallRubVector(D3DXVECTOR3* out, const D3DXVECTOR3& front, const D3DXVECTOR3& normal)
	{
		D3DXVECTOR3 norm;
		//	衝突点の法線ベクトルを正規化
		D3DXVec3Normalize(&norm, &normal);

		//	壁ずりベクトル = 進行ベクトル - Dot( 進行ベクトル, norm ) * norm
		return D3DXVec3Normalize(out, &(front - D3DXVec3Dot(&front, &norm)*norm));
	}

	/*	反射ベクトル
		引数
			out		:	正規化された反射ベクトル（戻り値）
			front	:	進行ベクトル
			normal	:	衝突点の法線ベクトル
	*/
	D3DXVECTOR3* Collision::ReflectVector(D3DXVECTOR3* out, const D3DXVECTOR3& front, const D3DXVECTOR3& normal)
	{
		D3DXVECTOR3 norm;
		//	衝突点の法線ベクトルを正規化
		D3DXVec3Normalize(&norm, &normal);

		//	反射ベクトル = 進行ベクトル - 2Dot( 進行ベクトル, norm ) * norm
		return D3DXVec3Normalize(out, &(front - 2.f * D3DXVec3Dot(&front, &norm)*norm));
	}

	/*	平面と点の位置関係を調べる
		引数
			plane	:	対象の平面
			point	:	対象の点
		戻り値
		+ は表  - は裏  0 は平面上に存在
	*/
	float Collision::PlanePoint(const D3DXPLANE& plane, const D3DXVECTOR3& point)
	{
		return D3DXPlaneDotCoord(&plane, &point);
	}

	/*	線と点の衝突判定
		引数
			linePoint	:	線上の点
			vec			:	線の方向を示すベクトル
			point		:	衝突してるかどうか調べる対象の点
		戻り値
			trueのとき衝突
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

	/*	線分と点の衝突判定
		引数
			ps		:	線分のStart点
			pe		:	線分のEnd点
			point	:	衝突してるかどうか調べる対象の点
		戻り値
			trueのとき衝突
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

	/*	2直線の衝突判定と最短距離と最短点を取得
		引数
			outDis		:	2直線の距離
			outPos1		:	直線1上の最短点座標
			outPos2		:	直線2上の最短点座標
			linePoint1	:	直線1上の1点
			lineVec1	:	直線1の方向
			linePoint2	:	直線2上の1点
			lineVec2	:	直線2の方向
		戻り値
			trueのとき2直線は衝突している
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

	/*	線と平面の衝突判定 線が衝突してるかどうかのみ知れる
		引数
			planePoint		:	平面上の点
			normal			:	平面の法線ベクトル
			linePoint		:	線上の点
			vec				:	線の方向を示すベクトル
		戻り値
			trueのとき衝突
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

	/*	線分と平面の衝突 線分と平面の衝突点がわかる
		引数
			out		:	衝突点を返す、衝突していなかった場合はNULL(戻り値)
			plane	:	対象の平面
			ps		:	線分のStart点
			pe		:	線分のEnd点
	*/
	D3DXVECTOR3* Collision::PlaneLineSegment(D3DXVECTOR3* out, const D3DXPLANE& plane, const D3DXVECTOR3& ps, const D3DXVECTOR3& pe)
	{
		return D3DXPlaneIntersectLine(out, &plane, &ps, &pe);
	}

	/*	球と球の衝突判定
		引数
			sphere1	:	球1
			sphere2	:	球2
		戻り値
			trueのとき衝突している
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

	/* 床ポリゴンの高さを取得
		引数
			normal		:	面法線ベクトル
			point		:	x,zのみわかっているポリゴンに含まれた点(yが含まれているかどうかは関係ない)
			p0			:	面法線ベクトルの面の頂点0
		戻り値
			床ポリゴンのpoint部分のy成分を返す
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

	/*	球と平面の衝突する座標と時刻を取得
		引数
			outTime			:	衝突したときの時刻
			outPos			:	衝突したときの座標
			sphere			:	球
			spherePrePos	:	球の前の位置
			sphereNextPos	:	球の次の到達位置
			normal			:	平面の法線
			planePoint		:	平面
		戻り値
			trueのとき球と平面は衝突している
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

	/*	AABBと点との最短距離
		引数
			box		:	AABB
			point	:	点
		戻り値
			AABBと点の距離を返す	0のときAABB内に点がある
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

	/*	AABB同士の衝突判定
		引数
			box1	:	AABB1
			box2	:	AABB2
		戻り値
			trueのとき衝突
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

	/*	OBBと点の最短距離
		引数
			box		:	OBB
			point	:	点
		戻り値
			OBBと点との最短距離をfloat型で返す
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

	/*	OBBとOBBの衝突判定
		引数
			box1	:	OBB1
			box2	:	OBB2
		戻り値
			trueのとき衝突
	*/
	bool Collision::OBBCom(const OBB& box1, const OBB& box2)
	{
		//	各方向ベクトルの確保
		D3DXVECTOR3 nAe1 = *D3DXVec3Normalize(&nAe1, &box1.axis[0]), ae1 = nAe1 * box1.halfLen[0];
		D3DXVECTOR3 nAe2 = *D3DXVec3Normalize(&nAe2, &box1.axis[1]), ae2 = nAe2 * box1.halfLen[1];
		D3DXVECTOR3 nAe3 = *D3DXVec3Normalize(&nAe3, &box1.axis[2]), ae3 = nAe3 * box1.halfLen[2];
		D3DXVECTOR3 nBe1 = *D3DXVec3Normalize(&nBe1, &box1.axis[0]), be1 = nBe1 * box1.halfLen[0];
		D3DXVECTOR3 nBe2 = *D3DXVec3Normalize(&nBe2, &box1.axis[1]), be2 = nBe2 * box1.halfLen[1];
		D3DXVECTOR3 nBe3 = *D3DXVec3Normalize(&nBe3, &box1.axis[2]), be3 = nBe3 * box1.halfLen[2];
		D3DXVECTOR3 interval = box1.pos - box2.pos;

		//	分離軸：Ae1
		float rA = D3DXVec3Length(&ae1);
		float rB = LenSegOnSeparateAxis(&nAe1, &be1, &be2, &be3);
		float l = fabsf(D3DXVec3Dot(&interval, &nAe1));
		if (l > rA + rB)return false;

		//	分離軸：Ae2
		rA = D3DXVec3Length(&ae2);
		rB = LenSegOnSeparateAxis(&nAe2, &be1, &be2, &be3);
		l = fabsf(D3DXVec3Dot(&interval, &nAe2));
		if (l > rA + rB)return false;

		//	分離軸：Ae3
		rA = D3DXVec3Length(&ae3);
		rB = LenSegOnSeparateAxis(&nAe3, &be1, &be2, &be3);
		l = fabsf(D3DXVec3Dot(&interval, &nAe3));
		if (l > rA + rB)return false;

		//	分離軸：Be1
		rA = LenSegOnSeparateAxis(&nBe1, &ae1, &ae2, &ae3);
		rB = D3DXVec3Length(&be1);
		l = fabsf(D3DXVec3Dot(&interval, &nBe1));
		if (l > rA + rB)return false;

		//	分離軸：Be2
		rA = LenSegOnSeparateAxis(&nBe1, &ae1, &ae2, &ae3);
		rB = D3DXVec3Length(&be2);
		l = fabsf(D3DXVec3Dot(&interval, &nBe2));
		if (l > rA + rB)return false;

		//	分離軸：Be3
		rA = LenSegOnSeparateAxis(&nBe1, &ae1, &ae2, &ae3);
		rB = D3DXVec3Length(&be3);
		l = fabsf(D3DXVec3Dot(&interval, &nBe3));
		if (l > rA + rB)return false;

		//	分離軸：C11
		D3DXVECTOR3 cross;
		D3DXVec3Cross(&cross, &nAe1, &nBe1);
		rA = LenSegOnSeparateAxis(&cross, &ae2, &ae3);
		rB = LenSegOnSeparateAxis(&cross, &be2, &be3);
		l = fabsf(D3DXVec3Dot(&interval, &cross));
		if (l > rA + rB)return false;

		//	分離軸：C12
		D3DXVec3Cross(&cross, &nAe1, &nBe2);
		rA = LenSegOnSeparateAxis(&cross, &ae2, &ae3);
		rB = LenSegOnSeparateAxis(&cross, &be1, &be3);
		l = fabsf(D3DXVec3Dot(&interval, &cross));
		if (l > rA + rB)return false;

		//	分離軸：C13
		D3DXVec3Cross(&cross, &nAe1, &nBe3);
		rA = LenSegOnSeparateAxis(&cross, &ae2, &ae3);
		rB = LenSegOnSeparateAxis(&cross, &be1, &be2);
		l = fabsf(D3DXVec3Dot(&interval, &cross));
		if (l > rA + rB)return false;

		//	分離軸：C21
		D3DXVec3Cross(&cross, &nAe2, &nBe1);
		rA = LenSegOnSeparateAxis(&cross, &ae1, &ae3);
		rB = LenSegOnSeparateAxis(&cross, &be2, &be3);
		l = fabsf(D3DXVec3Dot(&interval, &cross));
		if (l > rA + rB)return false;

		//	分離軸：C22
		D3DXVec3Cross(&cross, &nAe2, &nBe2);
		rA = LenSegOnSeparateAxis(&cross, &ae1, &ae3);
		rB = LenSegOnSeparateAxis(&cross, &be1, &be3);
		l = fabsf(D3DXVec3Dot(&interval, &cross));
		if (l > rA + rB)return false;

		//	分離軸：C23
		D3DXVec3Cross(&cross, &nAe2, &nBe3);
		rA = LenSegOnSeparateAxis(&cross, &ae1, &ae3);
		rB = LenSegOnSeparateAxis(&cross, &be1, &be2);
		l = fabsf(D3DXVec3Dot(&interval, &cross));
		if (l > rA + rB)return false;

		//	分離軸：C31
		D3DXVec3Cross(&cross, &nAe3, &nBe1);
		rA = LenSegOnSeparateAxis(&cross, &ae1, &ae2);
		rB = LenSegOnSeparateAxis(&cross, &be2, &be3);
		l = fabsf(D3DXVec3Dot(&interval, &cross));
		if (l > rA + rB)return false;

		//	分離軸：C32
		D3DXVec3Cross(&cross, &nAe3, &nBe2);
		rA = LenSegOnSeparateAxis(&cross, &ae1, &ae2);
		rB = LenSegOnSeparateAxis(&cross, &be1, &be3);
		l = fabsf(D3DXVec3Dot(&interval, &cross));
		if (l > rA + rB)return false;

		//	分離軸：C33
		D3DXVec3Cross(&cross, &nAe3, &nBe3);
		rA = LenSegOnSeparateAxis(&cross, &ae1, &ae2);
		rB = LenSegOnSeparateAxis(&cross, &be1, &be2);
		l = fabsf(D3DXVec3Dot(&interval, &cross));
		if (l > rA + rB)return false;

		//	分離平面が存在しないので衝突している
		return true;
	}

	/*	OBBと平面の衝突判定
		引数
			box		:	OBB
			pos		:	平面上の点
			normal	:	平面の正規化された法線
			len		:	OBBと平面が衝突していた場合壁にめり込んだ分を戻すための距離を算出
		戻り値
			trueのとき衝突
	*/
	bool Collision::OBBPlane(const OBB& box, const D3DXVECTOR3& pos, const D3DXVECTOR3& normal, float* len)
	{
		float r = 0.f;
		for (int i = 0; i < 3; i++)
		{
			D3DXVECTOR3 dir = box.axis[i];
			r += fabsf(D3DXVec3Dot(&(dir*box.halfLen[i]), &normal));
		}

		//	平面とOBBの距離を算出
		float s = D3DXVec3Dot(&(box.pos - pos), &normal);

		//	戻し距離を算出
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

		//	衝突判定
		if (fabsf(s) - r < EPSIRON)
		{
			return true;
		}

		return false;
	}

	/*	OBBと球の衝突判定
		引数
			box		:	OBB
			sphere	:	球
		戻り値
			trueの時衝突
	*/
	bool Collision::OBBSphere(const OBB& box, const SPHERE& sphere)
	{
		auto distance = OBBPointDistance(box, sphere.center + sphere.pos);
		return distance <= sphere.r;
	}

	/*	球と直線の衝突判定と座標と時刻を取得
		引数
			outTime			:	衝突したときの時刻
			outPos			:	衝突したときの座標
			sphere			:	球
			spherePrePos	:	球の前の位置
			sphereNextPos	:	球の次の到達位置
			linePoint		:	直線上の点
			linrDirect		:	直線の方向
		戻り値
			trueのとき衝突
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

		//	衝突判定
		float tmp = beta * beta - alpha * omega;

		//	衝突しないか止まっている場合は不正値を返す
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

		//	衝突時刻を算出
		*outTime = (-sqrtf(tmp) - beta) / alpha;

		//	衝突時刻が0以上1以下ならば衝突
		*outPos = (spherePrePos + sphere.center) + *outTime*e;
		if (*outTime >= EPSIRON&&*outTime <= 1.f)
		{
			return true;
		}

		//	現在位置でめり込んでいるかチェック
		if (omega < EPSIRON)
		{
			return true;
		}

		return false;
	}

	/*	RayとAABBとの衝突判定と座標と時刻を取得
		引数
			outTime		:	衝突したときの時刻
			outPos		:	衝突したときの座標
			rayPos		:	Rayの基点
			rayDir		:	Rayの方向
			box			:	AABB
			boxWorld	:	AABBのワールド行列
		戻り値
			trueなら衝突している
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

		//	交差判定
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
				//	スラブとの距離を算出
				//	t1が近スラブ、t2が遠スラブとの距離
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

				//	スラブ交差チェック
				if (*outTime >= timeMax)
				{
					return false;
				}
			}

		}

		//	交差している
		if (*outPos)
		{
			*outPos = rayPos + *outTime * (rayDir);
		}

		return true;
	}

	/*	Rayと球の衝突判定と衝突座標を取得
		引数
			outPosS		:	衝突開始点
			outPosE		:	衝突終了点
			rayPos		:	Rayの基点
			rayVec		:	Rayの方向
			sphere		:	球
		戻り値
			trueなら衝突している
	*/
	bool Collision::CalcRaySphere(D3DXVECTOR3* outPosS, D3DXVECTOR3* outPosE, const D3DXVECTOR3& rayPos, const D3DXVECTOR3& rayVec, const SPHERE& sphere)
	{
		D3DXVECTOR3 p = (sphere.center+sphere.pos) - rayPos;

		float a = D3DXVec3Dot(&rayVec, &rayVec);
		float b = D3DXVec3Dot(&rayVec, &p);
		float c = D3DXVec3Dot(&p, &p) - pow(sphere.r, 2);

		if (a == 0.f)
		{
			return false;	//	レイの長さが0
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

	/*	Rayと球の衝突判定と衝突座標を取得
		引数
			outPosS		:	衝突開始点
			outPosE		:	衝突終了点
			rayPos		:	Rayの基点
			rayVec		:	Rayの方向
			spherePos	:	球の中心座標
			r			:	球の半径
		戻り値
			trueなら衝突している
	*/
	bool Collision::CalcRaySphere(D3DXVECTOR3* outPosS, D3DXVECTOR3* outPosE, const D3DXVECTOR3& rayPos, const D3DXVECTOR3& rayVec, const D3DXVECTOR3& spherePos, const float& r)
	{
		D3DXVECTOR3 p = spherePos - rayPos;

		float a = D3DXVec3Dot(&rayVec, &rayVec);
		float b = D3DXVec3Dot(&rayVec, &p);
		float c = D3DXVec3Dot(&p, &p) - pow(r, 2);

		if (a == 0.f)
		{
			return false;	//	レイの長さが0
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

	/*	Rayと無限円柱の衝突判定と貫通点を取得
		引数
			outPosS	:	衝突開始点
			outPosE	:	衝突終了点
			rayPos	:	Rayの基点
			rayVec	:	Rayの方向
			cylPos1	:	円柱軸の1点
			cylPos2	:	円柱軸のもう1点
			r		:	円柱の半径
		戻り値
			trueなら衝突している
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

	/*	Rayとカプセルの衝突判定と貫通点を取得
		引数
			outPosS		:	衝突開始点
			outPosE		:	衝突終了点
			rayPos		:	Rayの基点
			rayVec		:	Rayの方向
			capPoint1	:	カプセル軸の1点
			capPoint2	:	カプセル軸のもう1点
			r			:	カプセルの半径
		戻り値
			trueなら衝突している
	*/
	bool Collision::CalcRayCapsule(D3DXVECTOR3* outPosS, D3DXVECTOR3* outPosE, const D3DXVECTOR3& rayPos, const D3DXVECTOR3& rayVec, const D3DXVECTOR3& capPoint1, const D3DXVECTOR3& capPoint2, const float& r)
	{
		bool q1Inp1 = false;
		bool q1Inp2 = false;
		bool q1InCld = false;

		//	Q1の検査
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

		//	Q2の検査
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

	//	自由落下
	FreeFall::FreeFall(float unitTime)
		:t(unitTime)
	{
		memset(&pos, 0, sizeof(D3DXVECTOR3));
		memset(&vel, 0, sizeof(D3DXVECTOR3));
		memset(&acc, 0, sizeof(D3DXVECTOR3));
	}

	void FreeFall::Update(float unitTime, bool resetAccel)
	{
		//	位置を算出
		GetPos(pos, unitTime);

		//	加速度から速度を算出
		vel += acc * unitTime;

		//	加速度をリセット
		if (resetAccel)
		{
			acc.x = acc.y = acc.z = 0.0f;
		}
	}

	D3DXVECTOR3& FreeFall::GetPos(D3DXVECTOR3& out, float unitTime)
	{
		//	等加速度運動の式
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