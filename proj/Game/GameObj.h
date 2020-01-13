#pragma once
#include<memory>
#include<array>

#include"Common.h"
#include"Window.h"
#include"D3D9.h"
#include"Camera.h"
#include"DirectSound.h"
#include"Input.h"
#include"Model.h"
#include"Physics.h"
#include"Texture.h"

//	ゲームに置かれるオブジェクトの元
class GameObj
{
public:

	virtual ~GameObj() {}
	virtual void Update() = 0;
	virtual void Draw() = 0;

	D3DXVECTOR3 pos = { {0,0,0} };
};

//	プレイヤー
class Player :public GameObj
{
	float angle[3] = {},	//	pitch,yaw,rollの順番
		  camPitch = 0.f, camYaw = 0.f;

	//	カメラ
	Camera::ChaseCamera cam;

	//	モデル
	Model::Pmd playerModel;

	//	当たり判定Box
	Physics::OBB playerCol;

	D3DXMATRIX world;

	

public:

	Player();
	~Player(){}

	//	更新
	void Update();

	//	描画
	void Draw();

	const D3DXMATRIX& GetViewMtx() const;

	const Physics::OBB& GetCollision() const;

};

class GuardianObj:public GameObj
{

	CComPtr<ID3DXMesh> sphere;
	D3DMATERIAL9 mat;
	CComPtr<IDirect3DCubeTexture9> cubeTex = 0;	//	鉄球ぽさを出すためのキューブマップテクスチャ
	D3DXMATRIX world;
	D3DXVECTOR3 targetPos = { 0.f,0.f,0.f };
	float pitch = 90.f, yaw = 125.f, roll = 25.f;

	Physics::SPHERE guardCol;	//	当たり判定Sphere

public:
	GuardianObj();

	~GuardianObj();

	GuardianObj(const GuardianObj& guardian);

	GuardianObj& operator=(GuardianObj& guardian);

	//	更新
	void Update();

	//	描画
	void Draw();

	void SetTarget(D3DXVECTOR3 pos);

	const Physics::SPHERE& GetCollision() const;


};

//	収集対象オブジェクト
class TargetObj :public GameObj
{
	D3DXMATRIX world;
	Model::Pmd targetModel;
	Physics::OBB targetCol;	//	当たり判定Box

public:
	TargetObj();
	~TargetObj(){}

	//	更新
	void Update();

	//	描画
	void Draw();

	const Physics::OBB& GetCollision() const;

};

//	ゲームステージ
class Stage :public GameObj
{
	ID3DXMesh* skyBox = 0;
	IDirect3DCubeTexture9* cubeTex = 0;
	IDirect3DVertexShader9 *vtxShader, *skyBoxVtxShader;
	IDirect3DPixelShader9 *pixShader, *skyBoxPixShader;
	D3DXMATRIX inv, view;

public:
	Stage();

	~Stage();

	void SetViewMtx(D3DXMATRIX view);

	//	更新
	void Update();

	//	描画
	void Draw();

};

//	シーンベース
class SceneBase
{

public:

	virtual ~SceneBase() {}

	virtual SceneBase* Draw(SceneBase*) = 0;

	virtual SceneBase* Update(SceneBase*) = 0;

};


class MgrChild;

//	シーン管理
class SceneManager
{
	SceneManager();
	~SceneManager();
	MgrChild* child;
	static SceneManager* instance;

public:

	//	更新
	void Update();

	//	描画
	void Draw();

	//	インスタンスの作成
	static void Create();

	//	インスタンスの破棄
	static void Destroy();

	//	インスタンスの取得
	static SceneManager* GetInstance();

};

class MgrChild: public SceneBase
{
	static float sec;
public:
	virtual ~MgrChild(){}

	//	更新
	SceneBase* Update(SceneBase*);

	//	描画
	SceneBase* Draw(SceneBase*);

	//	待機状態
	template<class Func>
	static void wait(float time,Func func) 
	{
		sec += 1.f / 60.f;
		if (time <= sec) {
			sec = 0.f;
			func();
		}
	}

	virtual SceneBase* Update(SceneManager*) = 0;
	virtual SceneBase* Draw(SceneManager*) = 0;

};

class GameScene;
class ClearScene;
class GameOverScene;

//	タイトルシーン
class TitleScene :public MgrChild
{
	Texture::Texture title;	//	タイトル画像
	D3DMATERIAL9 mat = { {1.f,1.f,1.f,1.f},{0.f,0.f,0.f,0.f},{0.f,0.f,0.f,0.f},{1.f,1.f,1.f,1.f},0 };

	//	カスタム頂点
	struct VERTEX
	{
		D3DXVECTOR4 pos = {};
		DWORD color = D3DCOLOR_XRGB(255, 255, 255);
		D3DXVECTOR2 uv = {};
	};

	VERTEX titleVtx[4];

	//	sound
	DirectSound::Audio audioStart;

	//	シーン遷移時待機状態に入るかどうかのフラグ
	bool startFlag = false;

public:

	TitleScene();

	//	更新
	SceneBase* Update(SceneManager*);

	//	描画
	SceneBase* Draw(SceneManager*);

};

//	ゲームシーン
class GameScene : public MgrChild
{
	std::unique_ptr<Player> player = std::make_unique<Player>();	//	プレイヤー
	std::array<std::unique_ptr<TargetObj>, 5> target;				//	収集対象
	std::array<std::unique_ptr<GuardianObj>,5> guardian;			//	収集対象の周りを飛んで邪魔するもの
	std::unique_ptr<Stage> stage = std::make_unique<Stage>();		//	ステージ

	Physics::Collision col;	//	当たり判定

	int count = 0;	//	収集対象の収集数
	float outTime = 5.f;

	//	UI
	D3DXMATRIX projWorld;
	D3DXMATRIX orthoProj;	//	正射影行列
	Texture::Texture UITex;

	//	カスタム頂点
	struct VERTEX
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR2 uv;
	};

	//	ビルボード頂点
	VERTEX billboard[4] = {
		{D3DXVECTOR3(-0.5f,-0.5f,0.1f),D3DXVECTOR2(0.f,1.f)},
		{D3DXVECTOR3(-0.5f,0.5f,0.1f) ,D3DXVECTOR2(0.f,0.f)},
		{D3DXVECTOR3(0.5f,-0.5f,0.1f) ,D3DXVECTOR2(1.f,1.f)},
		{D3DXVECTOR3(0.5f,0.5f,0.1f)  ,D3DXVECTOR2(1.f,0.f)},
	};

	CComPtr<ID3DXFont> countFont = NULL, outFont = NULL;

	//	sound
	DirectSound::Audio audioGame;

	//	シーン遷移時待機状態に入るかどうかのフラグ
	bool hitFlag = false;

public:

	GameScene();

	//	更新
	SceneBase* Update(SceneManager*);

	//	描画
	SceneBase* Draw(SceneManager*);

};

//	CLEAR画面
class ClearScene :public MgrChild
{
	Texture::Texture clear;	//	クリア画像
	D3DMATERIAL9 mat = { {1.f,1.f,1.f,1.f},{0.f,0.f,0.f,0.f},{0.f,0.f,0.f,0.f},{1.f,1.f,1.f,1.f},0 };

	//	カスタム頂点
	struct VERTEX
	{
		D3DXVECTOR4 pos = {};
		DWORD color = D3DCOLOR_XRGB(255, 255, 255);
		D3DXVECTOR2 uv = {};
	}; 
	
	VERTEX clearVtx[4];

	//	sound
	DirectSound::Audio audioClear;

public:
	ClearScene();

	//	更新
	SceneBase* Update(SceneManager*);

	//	描画
	SceneBase* Draw(SceneManager*);

};

//	GameOver画面
class GameOverScene:public MgrChild
{
	Texture::Texture gameOver;	//	ゲームオーバー画像
	D3DMATERIAL9 mat = { {1.f,1.f,1.f,1.f},{0.f,0.f,0.f,0.f},{0.f,0.f,0.f,0.f},{1.f,1.f,1.f,1.f},0 };

	//	カスタム頂点
	struct VERTEX
	{
		D3DXVECTOR4 pos = {};
		DWORD color = D3DCOLOR_XRGB(255, 255, 255);
		D3DXVECTOR2 uv = {};
	}; 
	
	VERTEX gameOverVtx[4];

	//	sound
	DirectSound::Audio audioGameOver;

public:
	GameOverScene();

	//	更新
	SceneBase* Update(SceneManager*);
	//	描画
	SceneBase* Draw(SceneManager*);
	  
};