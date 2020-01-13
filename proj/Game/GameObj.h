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

//	�Q�[���ɒu�����I�u�W�F�N�g�̌�
class GameObj
{
public:

	virtual ~GameObj() {}
	virtual void Update() = 0;
	virtual void Draw() = 0;

	D3DXVECTOR3 pos = { {0,0,0} };
};

//	�v���C���[
class Player :public GameObj
{
	float angle[3] = {},	//	pitch,yaw,roll�̏���
		  camPitch = 0.f, camYaw = 0.f;

	//	�J����
	Camera::ChaseCamera cam;

	//	���f��
	Model::Pmd playerModel;

	//	�����蔻��Box
	Physics::OBB playerCol;

	D3DXMATRIX world;

	

public:

	Player();
	~Player(){}

	//	�X�V
	void Update();

	//	�`��
	void Draw();

	const D3DXMATRIX& GetViewMtx() const;

	const Physics::OBB& GetCollision() const;

};

class GuardianObj:public GameObj
{

	CComPtr<ID3DXMesh> sphere;
	D3DMATERIAL9 mat;
	CComPtr<IDirect3DCubeTexture9> cubeTex = 0;	//	�S���ۂ����o�����߂̃L���[�u�}�b�v�e�N�X�`��
	D3DXMATRIX world;
	D3DXVECTOR3 targetPos = { 0.f,0.f,0.f };
	float pitch = 90.f, yaw = 125.f, roll = 25.f;

	Physics::SPHERE guardCol;	//	�����蔻��Sphere

public:
	GuardianObj();

	~GuardianObj();

	GuardianObj(const GuardianObj& guardian);

	GuardianObj& operator=(GuardianObj& guardian);

	//	�X�V
	void Update();

	//	�`��
	void Draw();

	void SetTarget(D3DXVECTOR3 pos);

	const Physics::SPHERE& GetCollision() const;


};

//	���W�ΏۃI�u�W�F�N�g
class TargetObj :public GameObj
{
	D3DXMATRIX world;
	Model::Pmd targetModel;
	Physics::OBB targetCol;	//	�����蔻��Box

public:
	TargetObj();
	~TargetObj(){}

	//	�X�V
	void Update();

	//	�`��
	void Draw();

	const Physics::OBB& GetCollision() const;

};

//	�Q�[���X�e�[�W
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

	//	�X�V
	void Update();

	//	�`��
	void Draw();

};

//	�V�[���x�[�X
class SceneBase
{

public:

	virtual ~SceneBase() {}

	virtual SceneBase* Draw(SceneBase*) = 0;

	virtual SceneBase* Update(SceneBase*) = 0;

};


class MgrChild;

//	�V�[���Ǘ�
class SceneManager
{
	SceneManager();
	~SceneManager();
	MgrChild* child;
	static SceneManager* instance;

public:

	//	�X�V
	void Update();

	//	�`��
	void Draw();

	//	�C���X�^���X�̍쐬
	static void Create();

	//	�C���X�^���X�̔j��
	static void Destroy();

	//	�C���X�^���X�̎擾
	static SceneManager* GetInstance();

};

class MgrChild: public SceneBase
{
	static float sec;
public:
	virtual ~MgrChild(){}

	//	�X�V
	SceneBase* Update(SceneBase*);

	//	�`��
	SceneBase* Draw(SceneBase*);

	//	�ҋ@���
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

//	�^�C�g���V�[��
class TitleScene :public MgrChild
{
	Texture::Texture title;	//	�^�C�g���摜
	D3DMATERIAL9 mat = { {1.f,1.f,1.f,1.f},{0.f,0.f,0.f,0.f},{0.f,0.f,0.f,0.f},{1.f,1.f,1.f,1.f},0 };

	//	�J�X�^�����_
	struct VERTEX
	{
		D3DXVECTOR4 pos = {};
		DWORD color = D3DCOLOR_XRGB(255, 255, 255);
		D3DXVECTOR2 uv = {};
	};

	VERTEX titleVtx[4];

	//	sound
	DirectSound::Audio audioStart;

	//	�V�[���J�ڎ��ҋ@��Ԃɓ��邩�ǂ����̃t���O
	bool startFlag = false;

public:

	TitleScene();

	//	�X�V
	SceneBase* Update(SceneManager*);

	//	�`��
	SceneBase* Draw(SceneManager*);

};

//	�Q�[���V�[��
class GameScene : public MgrChild
{
	std::unique_ptr<Player> player = std::make_unique<Player>();	//	�v���C���[
	std::array<std::unique_ptr<TargetObj>, 5> target;				//	���W�Ώ�
	std::array<std::unique_ptr<GuardianObj>,5> guardian;			//	���W�Ώۂ̎������Ŏז��������
	std::unique_ptr<Stage> stage = std::make_unique<Stage>();		//	�X�e�[�W

	Physics::Collision col;	//	�����蔻��

	int count = 0;	//	���W�Ώۂ̎��W��
	float outTime = 5.f;

	//	UI
	D3DXMATRIX projWorld;
	D3DXMATRIX orthoProj;	//	���ˉe�s��
	Texture::Texture UITex;

	//	�J�X�^�����_
	struct VERTEX
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR2 uv;
	};

	//	�r���{�[�h���_
	VERTEX billboard[4] = {
		{D3DXVECTOR3(-0.5f,-0.5f,0.1f),D3DXVECTOR2(0.f,1.f)},
		{D3DXVECTOR3(-0.5f,0.5f,0.1f) ,D3DXVECTOR2(0.f,0.f)},
		{D3DXVECTOR3(0.5f,-0.5f,0.1f) ,D3DXVECTOR2(1.f,1.f)},
		{D3DXVECTOR3(0.5f,0.5f,0.1f)  ,D3DXVECTOR2(1.f,0.f)},
	};

	CComPtr<ID3DXFont> countFont = NULL, outFont = NULL;

	//	sound
	DirectSound::Audio audioGame;

	//	�V�[���J�ڎ��ҋ@��Ԃɓ��邩�ǂ����̃t���O
	bool hitFlag = false;

public:

	GameScene();

	//	�X�V
	SceneBase* Update(SceneManager*);

	//	�`��
	SceneBase* Draw(SceneManager*);

};

//	CLEAR���
class ClearScene :public MgrChild
{
	Texture::Texture clear;	//	�N���A�摜
	D3DMATERIAL9 mat = { {1.f,1.f,1.f,1.f},{0.f,0.f,0.f,0.f},{0.f,0.f,0.f,0.f},{1.f,1.f,1.f,1.f},0 };

	//	�J�X�^�����_
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

	//	�X�V
	SceneBase* Update(SceneManager*);

	//	�`��
	SceneBase* Draw(SceneManager*);

};

//	GameOver���
class GameOverScene:public MgrChild
{
	Texture::Texture gameOver;	//	�Q�[���I�[�o�[�摜
	D3DMATERIAL9 mat = { {1.f,1.f,1.f,1.f},{0.f,0.f,0.f,0.f},{0.f,0.f,0.f,0.f},{1.f,1.f,1.f,1.f},0 };

	//	�J�X�^�����_
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

	//	�X�V
	SceneBase* Update(SceneManager*);
	//	�`��
	SceneBase* Draw(SceneManager*);
	  
};