//
//	Physics.h
//	当たり判定、自由落下の定義
//
#ifndef _INCLUDE_Physics_h_
#define	_INCLUDE_Physics_h_

#pragma once
#include<math.h>
#include<stdarg.h>

#include"Common.h"
#include"D3D9.h"

#define EPSIRON 0.00001f	//	誤差

namespace Physics
{

	//	球
	struct SPHERE
	{
		SPHERE() :center(0.f, 0.f, 0.f), pos(0.f, 0.f, 0.f), r(0), mesh(nullptr){}
		SPHERE(const LPD3DXMESH mesh);
		SPHERE(const SPHERE& sphere);
		~SPHERE();

		SPHERE& operator=(const SPHERE& sphere);

		//	BoundingSphereの作成
		void CreateSphere();

		//	球を作成する前に実行すること
		void SetMesh(const LPD3DXMESH& mesh);

		D3DXVECTOR3 center;	//	中心座標
		D3DXVECTOR3 pos;	//	球の座標
		float r;			//	半径

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

		//	AABBの作成
		void CreateAABB();

		//	AABBを作成する前に実行すること
		void SetMesh(const LPD3DXMESH& mesh);

		D3DXVECTOR3 pos;	//	座標
		D3DXVECTOR3 boxMin;	//	最小値
		D3DXVECTOR3 boxMax;	//	最大値
		D3DXVECTOR3 len;	//	長さ

	private:
		CComPtr<ID3DXMesh> mesh;
	};
	
	//	OBB
	struct OBB
	{
		OBB() :center(0.f, 0.f, 0.f),pos(0.f, 0.f, 0.f), axis(), halfLen(0.f, 0.f, 0.f), mesh(nullptr){}
		/*
			pos			:	OBBのワールド座標	
			angle[3]	:	OBBの傾き　pitch,yaw,rollの順番で格納された配列であること
		*/
		OBB(const LPD3DXMESH mesh, const D3DXVECTOR3 pos, const float *angle);
		OBB(const OBB& box);
		~OBB();

		OBB& operator=(const OBB& box);

		//	OBBの作成
		void CreateOBB();

		//	OBBを作成する前に実行すること
		void SetMesh(const LPD3DXMESH& mesh);

		D3DXVECTOR3 center;		//	中心座標
		D3DXVECTOR3 pos;		//	座標
		float angle[3] = {};	//	OBBの傾き
		D3DXVECTOR3 axis[3];	//	方向ベクトル
		D3DXVECTOR3 halfLen;	//	各座標軸に沿った長さの半分

	private:
		CComPtr<ID3DXMesh> mesh;
	};

	//	衝突
	class Collision
	{
	public:

		/*	面法線ベクトルの取得
			引数
				out	:	正規化された三角形の面法線ベクトルを返す（戻り値）
				p0	:	三角形の頂点0
				p1	:	三角形の頂点1
				p2	:	三角形の頂点2
				三角形を反時計回りで作っていると仮定した関数なので頂点の順番には気を付けること
		*/
		D3DXVECTOR3* GetPlaneNormal(D3DXVECTOR3* out, const D3DXVECTOR3& p0, const D3DXVECTOR3& p1, const D3DXVECTOR3& p2);

		/*	頂点法線ベクトルの取得
			引数
				num			:	渡す面法線ベクトルの数
				out			:	正規化された頂点法線ベクトルを返す（戻り値）
				...			:	面法線ベクトル　頂点が属している面の数だけの法線(D3DXVECTOR3)
		*/
		D3DXVECTOR3* GetVertexNormal(int num, D3DXVECTOR3* out, ...);

		/*	点と法線から平面を作成
			引数
				out			:	平面を返す（戻り値）
				normal		:	面法線ベクトル
				point		:	平面上の点
		*/
		D3DXPLANE* CreatePlane(D3DXPLANE* out, const D3DXVECTOR3& normal, const D3DXVECTOR3& point);

		/*	ある点から平面までの距離
			引数
				plane	:	平面
				point	:	点
			戻り値
				点から平面までの距離を返す
		*/
		float PlanePointDistance(const D3DXPLANE& plane, const D3DXVECTOR3& point);

		/*	壁ずりベクトル
			引数
				out		:	正規化された壁ずりベクトル（戻り値）
				front	:	進行ベクトル
				normal	:	衝突点の法線ベクトル
		*/
		D3DXVECTOR3* WallRubVector(D3DXVECTOR3* out, const D3DXVECTOR3& front, const D3DXVECTOR3& normal);

		/*	反射ベクトル
			引数
				out		:	正規化された反射ベクトル（戻り値）
				front	:	進行ベクトル
				normal	:	衝突点の法線ベクトル
		*/
		D3DXVECTOR3* ReflectVector(D3DXVECTOR3* out, const D3DXVECTOR3& front, const D3DXVECTOR3& normal);

		/*	平面と点の位置関係を調べる
			引数
				plane	:	平面
				point	:	点
			戻り値
			+ は表  - は裏  0 は平面上に存在
		*/
		float PlanePoint(const D3DXPLANE& plane, const D3DXVECTOR3& point);

		/*	線と点の衝突判定
			引数
				linePoint	:	線上の点
				vec			:	線の方向を示すベクトル
				point		:	点
			戻り値
				trueのとき衝突
		*/
		bool LinePoint(const D3DXVECTOR3& linePoint, const D3DXVECTOR3& vec, const D3DXVECTOR3& point);

		/*	線分と点の衝突判定
			引数
				ps		:	線分のStart点
				pe		:	線分のEnd点
				point	:	点
			戻り値
				trueのとき衝突
		*/
		bool LineSegmentPoint(const D3DXVECTOR3& ps, const D3DXVECTOR3& pe, const D3DXVECTOR3& point);

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
		bool CalcLineCom(float* outDis, D3DXVECTOR3* outPos1, D3DXVECTOR3* outPos2, const D3DXVECTOR3& linePoint1, const D3DXVECTOR3& lineVec1, const D3DXVECTOR3& linePoint2, const D3DXVECTOR3& lineVec2);

		/*	線と平面の衝突判定 線が衝突してるかどうかのみ知れる
			引数
				planePoint		:	平面上の点
				normal			:	平面の法線ベクトル
				linePoint		:	線上の点
				vec				:	線の方向を示すベクトル
			戻り値
				trueのとき衝突
		*/
		bool PlaneLine(const D3DXVECTOR3& planePoint, const D3DXVECTOR3& normal, const D3DXVECTOR3& linePoint, const D3DXVECTOR3& vec);

		/*	線分と平面の衝突 線分と平面の衝突点がわかる
			引数
				out		:	衝突点を返す、衝突していなかった場合はNULL(戻り値)
				plane	:	対象の平面
				ps		:	線分のStart点
				pe		:	線分のEnd点
		*/
		D3DXVECTOR3* PlaneLineSegment(D3DXVECTOR3* out, const D3DXPLANE& plane, const D3DXVECTOR3& ps, const D3DXVECTOR3& pe);

		/*	球と球の衝突判定
			引数
				sphere1	:	球1
				sphere2	:	球2
			戻り値
				trueのとき衝突している
		*/
		bool SphereCom(const SPHERE& sphere1,const SPHERE& sphere2);

		/* 床ポリゴンの高さを取得
			引数
				normal		:	面法線ベクトル
				point		:	x,zのみわかっているポリゴンに含まれた点(yが含まれているかどうかは関係ない)
				p0			:	面法線ベクトルの面の頂点0
			戻り値
				床ポリゴンのpoint部分のy成分を返す 
		*/
		float GetFloorHeight(const D3DXVECTOR3& normal, const D3DXVECTOR3& point, const D3DXVECTOR3& p0);

		/*	球と平面の衝突判定と座標と時刻を取得
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
		bool CalcSpherePlane(float* outTime, D3DXVECTOR3* outPos, const SPHERE& sphere, const D3DXVECTOR3& spherePrePos, const D3DXVECTOR3& sphereNextPos, const D3DXVECTOR3& normal, const D3DXPLANE& plane);

		/*	AABBと点との最短距離
			引数
				boxMin	:	AABBの最小値
				boxMax	:	AABBの最大値
				point	:	点
			戻り値
				AABBと点の距離を返す	0のときAABB内に点がある
		*/
		float AABBPointDistance(const AABB& box, const D3DXVECTOR3& point);

		/*	AABB同士の衝突判定
			引数
				box1	:	AABB1
				box2	:	AABB2
			戻り値
				trueのとき衝突
		*/
		bool AABBCom(const AABB& box1, const AABB& box2);

		/*	OBBと点の最短距離
			引数
				box		:	OBB
				point	:	点
			戻り値
				OBBと点との最短距離をfloat型で返す
		*/
		float OBBPointDistance(const OBB& box, const D3DXVECTOR3& point);

		/*	OBBとOBBの衝突判定
			引数
				box1	:	OBB1
				box2	:	OBB2
			戻り値
				trueのとき衝突
		*/
		bool OBBCom(const OBB& box1, const OBB& box2);

		/*	OBBと平面の衝突判定
			引数
				box		:	OBB
				pos		:	平面上の点
				normal	:	平面の正規化された法線
				len		:	OBBと平面が衝突していた場合壁にめり込んだ分を戻すための距離を算出
			戻り値
				trueのとき衝突
		*/
		bool OBBPlane(const OBB& box, const D3DXVECTOR3& pos, const D3DXVECTOR3& normal, float* len = NULL);

		/*	OBBと球の衝突判定
			引数
				box		:	OBB
				sphere	:	球
			戻り値
				trueの時衝突
		*/
		bool OBBSphere(const OBB& box, const SPHERE& sphere);

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
		bool CalcSphereLine(float* outTime, D3DXVECTOR3* outPos, const SPHERE& sphere, const D3DXVECTOR3& spherePrePos, const D3DXVECTOR3& sphereNextPos, const D3DXVECTOR3& linePoint, const D3DXVECTOR3& lineDirect);

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
		bool CalcRayBox(float* outTime, D3DXVECTOR3* outPos, const D3DXVECTOR3& rayPos, const D3DXVECTOR3& rayDir, const AABB& box, const D3DXMATRIX& boxWorld);

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
		bool CalcRaySphere(D3DXVECTOR3* outPosS, D3DXVECTOR3* outPosE, const D3DXVECTOR3& rayPos, const D3DXVECTOR3& rayVec, const SPHERE& sphere);

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
		bool CalcRaySphere(D3DXVECTOR3* outPosS, D3DXVECTOR3* outPosE, const D3DXVECTOR3& rayPos, const D3DXVECTOR3& rayVec, const D3DXVECTOR3& spherePos, const float& r);

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
		bool CalcRayInfCylinder(D3DXVECTOR3* outPosS, D3DXVECTOR3* outPosE, const D3DXVECTOR3& rayPos, const D3DXVECTOR3& rayVec, const D3DXVECTOR3& cylPos1, const D3DXVECTOR3& cylPos2, const float& r);

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
		bool CalcRayCapsule(D3DXVECTOR3* outPosS, D3DXVECTOR3* outPosE, const D3DXVECTOR3& rayPos, const D3DXVECTOR3& rayVec, const D3DXVECTOR3& capPoint1, const D3DXVECTOR3& capPoint2, const float& r);

	private:

		/*	分離軸に投影された軸成分から投影線分長を算出
			引数
				normSep	:	正規化された分離軸
				e1		:	OBBの軸1
				e2		:	OBBの軸2
				e3		:	OBBの軸3
		*/
		float LenSegOnSeparateAxis(const D3DXVECTOR3* normSep, const D3DXVECTOR3* e1, const D3DXVECTOR3* e2, const D3DXVECTOR3* e3 = 0);

		/*	∠P1P2P3の内積を取得
			引数
				p1	:	点P1
				p2	:	点P2
				p3	:	点P3
			戻り値
				∠P1P2P3の内積を返す
		*/
		float CalcDot(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3);
	};

	//	自由落下
	class FreeFall
	{
	public:
		FreeFall(float unitTime = 1 / 60.f);
		FreeFall(const FreeFall& ff) = delete;

		//	セッター

		//	位置を設定
		void SetPos(D3DXVECTOR3& pos)
		{
			this->pos = pos;
		}
		//	速度を設定
		void SetVelocity(D3DXVECTOR3& vel)
		{
			this->vel = vel;
		}
		//	加速度を設定
		void SetAccel(D3DXVECTOR3& acc)
		{
			this->acc = acc;
		}

		//	ゲッター

		//	指定時刻での位置を取得
		D3DXVECTOR3& GetPos(D3DXVECTOR3& out, float unitTime);
		//	指定時刻での速度を取得
		D3DXVECTOR3& GetVelocity(D3DXVECTOR3& out, float unitTime);
		//	現在位置を取得
		D3DXVECTOR3& GetCurPos()
		{
			return pos;
		}
		//	現在の速度を取得
		D3DXVECTOR3& GetCurVelocity()
		{
			return vel;
		}
		//	現在設定されている加速度を取得
		D3DXVECTOR3& GetCurAccel()
		{
			return acc;
		}


		//	位置を加算
		D3DXVECTOR3& AddPos(D3DXVECTOR3& pos)
		{
			this->pos += pos;
			return this->pos;
		}
		//	速度を加算
		D3DXVECTOR3& AddVelocity(D3DXVECTOR3& vel)
		{
			this->vel += vel;
			return this->vel;
		}
		//	加速度を加算
		D3DXVECTOR3& AddAccel(D3DXVECTOR3& acc)
		{
			this->acc += acc;
			return this->acc;
		}

		//	位置や速度の更新
		void Update(float unitTime, bool resetAccel = true);

	private:

		float t;			//	差分時間
		D3DXVECTOR3	pos;	//	位置
		D3DXVECTOR3 vel;	//	速度
		D3DXVECTOR3	acc;	//	加速度

	};
}
#endif // !_INCLUDE_Physics_h_