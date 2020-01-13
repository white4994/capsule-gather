#include"GameObj.h"
#include<assert.h>

#define rotSpeed 1.f
#define moveSpeed 0.215f

//----------------------------
//		Player
//----------------------------
Player::Player()
{
	playerModel.Load("player.pmd");
	playerCol.SetMesh(playerModel.GetMesh());


	D3DXMatrixIdentity(&world);

	cam.initParam.zf = 10000.f;
	cam.initParam.zn = 0.01f;
	cam.SetLook(pos);
	cam.SetPos(D3DXVECTOR3(pos.x, pos.y + 1.f, pos.z - 10.f));

}

void Player::Update()
{
	Input::Keyboard kb;
	
	cam.Reset();
	kb.Update();

	//	プレイヤー
	angle[0] = D3DXToRadian((float)(kb.On('S') - kb.On('W'))*rotSpeed);
	angle[2] = D3DXToRadian((float)(kb.On('A') - kb.On('D'))*rotSpeed);


	//	差分回転行列
	D3DXMATRIX mX, mY, mZ, sum;
	D3DXMatrixRotationZ(&mZ, angle[2]);
	D3DXMatrixRotationX(&mX, angle[0]);
	D3DXMatrixRotationY(&mY, angle[1]);
	sum = mZ * mX*mY;

	//	プレイヤーが向いている方向に進む
	D3DXVECTOR3 front(world._31, world._32, world._33);
	D3DXVECTOR3 move;
	if (kb.On(VK_SPACE)&& !(pos.x > 499.f || pos.x<-499.f || pos.y>499.f || pos.y<-499.f || pos.z>499.f || pos.z < -499.f))
	{
		move = front * moveSpeed * 8.f;
	}
	else
	{
		move = front * moveSpeed;
	}

	D3DXMATRIX trans;
	D3DXMatrixTranslation(&trans, move.x, move.y, move.z);


	//	変換
	world = sum * world * trans;
	cam.Update(world);

	pos.x = world._41;
	pos.y = world._42;
	pos.z = world._43;

	//	X座標限界
	if (world._41 >= 500.0f)
	{
		world._41 = 500.0f;
	}
	else if (world._41 <= -500.0f)
	{
		world._41 = -500.0f;
	}

	//	Y座標限界
	if (world._42 >= 500.0f)
	{
		world._42 = 500.0f;
	}
	else if (world._42 <= -500.0f)
	{
		world._42 = -500.0f;
	}

	//	Z座標限界
	if (world._43 >= 500.0f)
	{
		world._43 = 500.0f;
	}
	else if (world._43 <= -500.0f)
	{
		world._43 = -500.0f;
	}

	//	Collision
	for (int i = 0; i < 3; i++)
	{
		playerCol.angle[i] += angle[i];
	}
	playerCol.pos = pos;
	playerCol.CreateOBB();

}

void Player::Draw()
{
	D3D9::Device::GetInstance()->SetTransform(D3DTS_WORLD, &world);
	D3D9::Device::GetInstance()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	D3D9::Device::GetInstance()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	for (DWORD i = 0; i < playerModel.GetNumMat(); i++)
	{
		D3D9::Device::GetInstance()->SetMaterial(&playerModel.GetMats(i));
		D3D9::Device::GetInstance()->SetTexture(0, playerModel.GetTextures(i).GetTex());
		playerModel.GetMesh()->DrawSubset(i);
	}
	
}

const D3DXMATRIX& Player::GetViewMtx() const
{
	return cam.GetViewMtx();
}

const Physics::OBB& Player::GetCollision() const
{
	return playerCol;
}

//----------------------------
//		Guardian
//----------------------------
GuardianObj::GuardianObj()
{
	D3DXCreateSphere(D3D9::Device::GetInstance(), 5.f, 36, 36, &sphere, NULL);

	D3DXMatrixIdentity(&world);

	mat.Diffuse.r = 1.f;
	mat.Diffuse.g = 1.f;
	mat.Diffuse.b = 1.f;
	mat.Diffuse.a = 1.f;
	mat.Ambient = mat.Diffuse;
	mat.Specular.r = 1.f;
	mat.Specular.g = 1.f;
	mat.Specular.b = 1.f;
	mat.Emissive.r = 0.1f;
	mat.Emissive.g = 0.1f;
	mat.Emissive.b = 0.1f;
	mat.Power = 120.f;

	D3DDISPLAYMODE ds;
	D3D9::Device::GetInstance()->GetDisplayMode(0, &ds);

	//	キューブマップ(環境マップ)テクスチャをddsファイルから読み込む	
	if (FAILED(D3DXCreateCubeTextureFromFileEx(D3D9::Device::GetInstance(), "Texture.dds", D3DX_DEFAULT, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, ds.Format, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0xFF000000, NULL, nullptr, &cubeTex)))
	{
		ERROR_MSG("キューブテクスチャ作成失敗");
	}

	guardCol.SetMesh(sphere);
}

GuardianObj::~GuardianObj()
{
}

GuardianObj::GuardianObj(const GuardianObj& guardian)
	:mat(guardian.mat), world(guardian.world), targetPos(guardian.targetPos), pitch(guardian.pitch), yaw(guardian.yaw), roll(guardian.roll),sphere(guardian.sphere)
{
}

GuardianObj& GuardianObj::operator=(GuardianObj& guardian)
{
	sphere = guardian.sphere;
	mat = guardian.mat;
	world = guardian.world;
	targetPos = guardian.targetPos;
	pitch = guardian.pitch;
	yaw = guardian.yaw;
	roll = guardian.roll;

	return *this;
}

void GuardianObj::Update()
{
	D3DXMATRIX pMtx, yMtx, rMtx;
	pitch += 2.f;
	yaw += 1.8f;
	roll += 1.25f;
	//	ピッチ、ヨー、ロールを行列に適用
	D3DXMatrixRotationX(&pMtx, D3DXToRadian(pitch));
	D3DXMatrixRotationY(&yMtx, D3DXToRadian(yaw));
	D3DXMatrixRotationZ(&rMtx, D3DXToRadian(roll));
	world = rMtx * pMtx*yMtx;
	//	正面を向くベクトルを行列で計算
	D3DXVECTOR3 dir, frontVec{ 0.f,0.f,1.f };
	D3DXVec3TransformNormal(&dir, &frontVec, &world);

	pos = targetPos - dir * 20.f;

	world._41 = pos.x;
	world._42 = pos.y;
	world._43 = pos.z;

	guardCol.pos = pos;

	guardCol.CreateSphere();
}

void GuardianObj::Draw()
{
	//	環境光を設定
	D3D9::Device::GetInstance()->SetRenderState(D3DRS_AMBIENT, 0x00111111);
	//	鏡面反射を設定
	D3D9::Device::GetInstance()->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

	//	キューブマップ用のテクスチャ座標を自動生成するように指示
	D3D9::Device::GetInstance()->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);

	//	スフィアのレンダリング
	D3D9::Device::GetInstance()->SetTransform(D3DTS_WORLD, &world);
	D3D9::Device::GetInstance()->SetMaterial(&mat);
	D3D9::Device::GetInstance()->SetTexture(0, cubeTex);
	sphere->DrawSubset(0);

	D3D9::Device::GetInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	D3D9::Device::GetInstance()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	D3D9::Device::GetInstance()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void GuardianObj::SetTarget(D3DXVECTOR3 pos)
{
	targetPos = pos;
}

const Physics::SPHERE& GuardianObj::GetCollision() const
{
	return guardCol;
}

//----------------------------
//		Target
//----------------------------
TargetObj::TargetObj()
{

	targetModel.Load("target.pmd");
	targetCol.SetMesh(targetModel.GetMesh());
	for (int i = 0; i < 3; i++)
	{
		targetCol.angle[i] = 0.f;
	}

	D3DXMatrixIdentity(&world);
}


void TargetObj::Update()
{
	D3DXMatrixTranslation(&world, pos.x, pos.y, pos.z);

	//	Collision
	targetCol.pos.x = world._41;
	targetCol.pos.y = world._42;
	targetCol.pos.z = world._43;

	targetCol.CreateOBB();
}

void TargetObj::Draw()
{
	D3D9::Device::GetInstance()->SetTransform(D3DTS_WORLD, &world);
	D3D9::Device::GetInstance()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	D3D9::Device::GetInstance()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	D3D9::Device::GetInstance()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	D3D9::Device::GetInstance()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	for (DWORD i = 0; i < targetModel.GetNumMat(); i++)
	{
		D3D9::Device::GetInstance()->SetMaterial(&targetModel.GetMats(i));
		D3D9::Device::GetInstance()->SetTexture(0, targetModel.GetTextures(i).GetTex());
		targetModel.GetMesh()->DrawSubset(i);
	}
}

const Physics::OBB& TargetObj::GetCollision() const
{
	return targetCol;
}

//----------------------------
//		Stage
//----------------------------
Stage::~Stage()
{
	RELEASE(vtxShader);
	RELEASE(pixShader);
	RELEASE(skyBoxVtxShader);
	RELEASE(skyBoxPixShader);
	RELEASE(cubeTex);
	RELEASE(skyBox);
}

Stage::Stage()
	:vtxShader(0),pixShader(0),skyBoxVtxShader(0),skyBoxPixShader(0)
{
	D3DXMatrixIdentity(&view);
	D3DXMatrixIdentity(&inv);

	//	環境マップ用頂点シェーダ
	const char* vtxShaderStr =
		"float4x4 view : register(c0);      "
		"float4x4 proj : register(c4);      "
		"float4x4 world : register(c8);     "
		"float3 cameraPosW : register(c12); "	// カメラのワールド位置
		"                                   "
		"struct VS_IN {                     "
		"    float3 pos   : POSITION;       "
		"    float3 normal: NORMAL;         "
		"};                                 "
		"                                   "
		"struct VS_OUT {                    "
		"    float4 pos   : POSITION;       "
		"    float3 normalW: TEXCOORD0;     "	// ワールド空間の法線
		"	 float3 viewVecW: TEXCOORD1;    "	// ワールド空間での視線ベクトル
		"};                                 "
		"                                   "
		"VS_OUT main( VS_IN In ) {          "
		"    VS_OUT Out;                    "
		"    Out.pos = mul( float4(In.pos, 1.0f), world );"
		"    Out.viewVecW = Out.pos.xyz - cameraPosW;"
		"    Out.pos = mul( Out.pos, view );"
		"    Out.pos = mul( Out.pos, proj );"
		"                                   "
		"    Out.normalW = mul( float4(In.normal, 0.0f), world );"
		"                                   "
		"    return Out;                    "
		"}                                  "
		"";

	//	環境マップ用ピクセルシェーダ
	const char* pixShaderStr =
		"textureCUBE cubeTex;"
		"samplerCUBE cubeTexSampler =       "
		"sampler_state {                    "
		"    Texture = <cubeTex>;           "
		"    MinFilter = LINEAR;            "
		"    MagFilter = LINEAR;            "
		"    MipFilter = LINEAR;            "
		"};                                 "
		"                                   "
		"struct VS_OUT {                    "
		"    float3 normalW: TEXCOORD0;     "
		"	 float3 viewVecW: TEXCOORD1;    "
		"};                                 "
		"                                   "
		"float4 main( VS_OUT In ) : COLOR { "
		"	 float3 vReflect = reflect( In.viewVecW, In.normalW );"
		"    return texCUBE(cubeTexSampler, vReflect);"
		"}                                  ";

	//	スカイボックス用頂点シェーダ
	const char* skyBoxVtxShaderStr =
		"float4x4 invVP : register(c0);      "
		"                                    "
		"struct VS_OUT {                     "
		"    float4 pos : POSITION;          "
		"    float4 vec : TEXCOORD0;         "
		"};                                  "
		"                                    "
		"VS_OUT main( float3 pos : POSITION ) {"
		"    VS_OUT Out;                     "
		"    Out.vec = normalize(mul(float4(pos, 0.0f), invVP));"
		"    Out.pos = float4(pos, 1.0f);    "
		"    return Out;                     "
		"}                                   ";

	//	スカイボックス用ピクセルシェーダ
	const char* skyBoxPixShaderStr =
		"textureCUBE cubeTex;                "
		"samplerCUBE cubeTexSampler =        "
		"sampler_state {                     "
		"    Texture = <cubeTex>;            "
		"    MinFilter = LINEAR;             "
		"    MagFilter = LINEAR;             "
		"    MipFilter = LINEAR;             "
		"};                                  "
		"                                    "
		"struct VS_OUT {                     "
		"    float4 pos : POSITION;          "
		"    float4 vec : TEXCOORD0;         "
		"};                                  "
		"                                    "
		"float4 main( VS_OUT In ) : COLOR {  "
		"    return texCUBE(cubeTexSampler, In.vec);"
		"}                                   ";

	//	モデル作成
	//	Box(skyBox)
	D3DXCreateBox(D3D9::Device::GetInstance(), 2.f, 2.f, 2.f, &skyBox, 0);

	//	キューブマップ(環境マップ)テクスチャをddsファイルから読み込む
	if (FAILED(D3DXCreateCubeTextureFromFile(D3D9::Device::GetInstance(), _T("Texture.dds"), &cubeTex))) 
	{
		ERROR_MSG("キューブテクスチャ作成失敗");
	}


	//	シェーダを作成
	struct CreateShader
	{
		static IDirect3DVertexShader9* VS(const char* str)
		{
			ID3DXBuffer *shader, *error;
			IDirect3DVertexShader9* vertexShader = 0;
			if (FAILED(D3DXCompileShader(str, (UINT)std::strlen(str), 0, 0, "main", "vs_3_0", D3DXSHADER_PACKMATRIX_ROWMAJOR, &shader, &error, 0)))
			{
				OutputDebugStringA((const char*)error->GetBufferPointer());
				return 0;
			}
			D3D9::Device::GetInstance()->CreateVertexShader((const DWORD*)shader->GetBufferPointer(), &vertexShader);
			shader->Release();
			return vertexShader;
		}
		static IDirect3DPixelShader9* PS(const char* str)
		{
			ID3DXBuffer *shader, *error;
			IDirect3DPixelShader9* pixelShader = 0;
			if (FAILED(D3DXCompileShader(str, (UINT)std::strlen(str), 0, 0, "main", "ps_3_0", D3DXSHADER_PACKMATRIX_ROWMAJOR, &shader, &error, 0)))
			{
				OutputDebugStringA((const char*)error->GetBufferPointer());
				return 0;
			}
			D3D9::Device::GetInstance()->CreatePixelShader((const DWORD*)shader->GetBufferPointer(), &pixelShader);
			shader->Release();
			return pixelShader;
		}
	};

	//	頂点・ピクセルシェーダ作成
	if (!(vtxShader = CreateShader::VS(vtxShaderStr)))return;
	if (!(pixShader = CreateShader::PS(pixShaderStr)))return;
	if (!(skyBoxVtxShader = CreateShader::VS(skyBoxVtxShaderStr)))return;
	if (!(skyBoxPixShader = CreateShader::PS(skyBoxPixShaderStr)))return;

}

void Stage::SetViewMtx(D3DXMATRIX view)
{
	this->view = view;
}

void Stage::Update()
{
}

void Stage::Draw()
{
	D3D9::Device::GetInstance()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	D3D9::Device::GetInstance()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	D3D9::Device::GetInstance()->SetTexture(0, cubeTex);
	D3DXMatrixInverse(&inv, 0, &(view));
	D3D9::Device::GetInstance()->SetVertexShaderConstantF(0, (float*)&inv, 4);
	D3D9::Device::GetInstance()->SetVertexShader(skyBoxVtxShader);
	D3D9::Device::GetInstance()->SetPixelShader(skyBoxPixShader);
	skyBox->DrawSubset(0);
	D3D9::Device::GetInstance()->SetVertexShader(0);
	D3D9::Device::GetInstance()->SetPixelShader(0);
}


//----------------------------
//		Scene
//----------------------------

SceneManager* SceneManager::instance = 0;

void SceneManager::Create()
{
	assert(!instance);
	instance = new SceneManager();
}

void SceneManager::Destroy()
{
	assert(instance);
	DEL(instance);
}

SceneManager* SceneManager::GetInstance()
{
	return instance;
}

SceneManager::SceneManager() :child(0)
{
	//	最初はタイトルを作成
	child = new TitleScene;
}

SceneManager::~SceneManager()
{
	//	残っていれば削除
	DEL(child);
}

void SceneManager::Update()
{
	SceneBase* nextChild = child->Update(this);
	if (nextChild != child)
	{
		//	遷移判定
		MgrChild* casted = dynamic_cast<MgrChild*>(nextChild);
		assert(casted);
		DEL(child);
		child = casted;
	}

	nextChild = 0;
}

void SceneManager::Draw()
{
	SceneBase* nextChild = child->Draw(this);
	if (nextChild != child)
	{
		//	遷移判定
		MgrChild* casted = dynamic_cast<MgrChild*>(nextChild);
		assert(casted);
		DEL(child);
		child = casted;
	}

	nextChild = 0;
}

//	静的なメンバ変数の実態を生成
float MgrChild::sec;

SceneBase* MgrChild::Update(SceneBase* base)
{
	SceneManager* mgr = dynamic_cast<SceneManager*>(base);
	assert(mgr);
	return Update(mgr);
}

SceneBase* MgrChild::Draw(SceneBase* base)
{
	SceneManager* mgr = dynamic_cast<SceneManager*>(base);
	assert(mgr);
	return Draw(mgr);
}

//------------------------------------
//			TITLE
//------------------------------------
TitleScene::TitleScene()
{
	//	tex
	title.Load("title.png");

	//	sound
	audioStart.Load("Start.wav");
}

SceneBase* TitleScene::Update(SceneManager*)
{
	SceneBase* next = this;

	//	タイトル画像
	RECT client;
	GetClientRect(Win::Window::GetInstance(), &client);
	titleVtx[0] = { D3DXVECTOR4(0.f,(float)client.bottom,0.f,1.f),					D3DCOLOR_XRGB(255,255,255), D3DXVECTOR2(0.f,1.f) };
	titleVtx[1] = { D3DXVECTOR4(0.f,0.f,0.f,1.f),									D3DCOLOR_XRGB(255,255,255), D3DXVECTOR2(0.f,0.f) };
	titleVtx[2] = { D3DXVECTOR4((float)client.right,(float)client.bottom,0.f,1.f),	D3DCOLOR_XRGB(255,255,255), D3DXVECTOR2(1.f,1.f) };
	titleVtx[3] = { D3DXVECTOR4((float)client.right,0.f,0.f,1.f),					D3DCOLOR_XRGB(255,255,255), D3DXVECTOR2(1.f,0.f) };

	//	sound
	auto start = audioStart.GetBuf("Start.wav");

	//	入力取得
	Input::Keyboard kb;
	kb.Update();
	//	Sキーを押すとゲーム開始
	if (kb.Press('S'))
	{
		start->Play(0, 0, 0);
		startFlag = true;
	}

	if (startFlag)
	{
		wait(0.095f, [&]() {next = new GameScene; });
	}

	return next;
}

SceneBase* TitleScene::Draw(SceneManager*)
{
	SceneBase* next = this;
	D3D9::Device::GetInstance()->SetMaterial(&mat);
	D3D9::Device::GetInstance()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	D3D9::Device::GetInstance()->SetTexture(0, title.GetTex());
	D3D9::Device::GetInstance()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, titleVtx, sizeof(VERTEX));
	
	return next;
}


//------------------------------------
//			GAME
//------------------------------------
GameScene::GameScene()
{
	for (int i = 0; i < 5; i++)
	{
		target[i] = std::make_unique<TargetObj>();
		guardian[i] = std::make_unique<GuardianObj>();
	}

	target[0]->pos = D3DXVECTOR3(-410.f, 276.f, 50.f);
	target[1]->pos = D3DXVECTOR3(-86.f, 70.f, 33.f);
	target[2]->pos = D3DXVECTOR3(337.f, -204.f, 294.f);
	target[3]->pos = D3DXVECTOR3(-183.f, -228.f, 238.f);
	target[4]->pos = D3DXVECTOR3(72.f, -377.f, -363.f);

	for (int i = 0; i < 5; i++)
	{
		guardian[i]->SetTarget(target[i]->pos);
	}


	//	UI
	UITex.Load("ui.png");
	D3DXMatrixIdentity(&orthoProj);
	D3DXMatrixIdentity(&projWorld);
	projWorld._11 = 200.f;
	projWorld._22 = 200.f;
	projWorld._41 = 0.f;
	projWorld._42 = 200.f;

	if (FAILED(D3DXCreateFont(D3D9::Device::GetInstance(), 30, 20, FW_REGULAR, NULL, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "tahoma", &countFont)))
	{
		ERROR_EXIT();
	}
	
	if (FAILED(D3DXCreateFont(D3D9::Device::GetInstance(), 30, 20, FW_REGULAR, NULL, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "tahoma", &outFont)))
	{
		ERROR_EXIT();
	}

	//	sound
	audioGame.Load("BGM.wav");
	audioGame.Load("Collect.wav");
	audioGame.Load("Hit.wav");
}

SceneBase* GameScene::Update(SceneManager*)
{
	SceneBase* next = this;
	
	player->Update();
	stage->SetViewMtx(player->GetViewMtx());
	for (int i = 0; i < 5; i++)
	{
		if (target[i])
		{
			target[i]->Update();
			guardian[i]->Update();
		}
	}
	stage->Update();

	//	sound
	auto BGM = audioGame.GetBuf("BGM.wav");
	auto collect = audioGame.GetBuf("Collect.wav");
	auto hit = audioGame.GetBuf("Hit.wav");

	BGM->Play(0, 0, DSBPLAY_LOOPING);

	//	当たり判定
	for (int i = 0; i < 5; ++i)
	{
		if (target[i])
		{
			if (col.OBBSphere(player->GetCollision(), guardian[i]->GetCollision()))
			{
				hit->Play(0, 0, 0);
				hitFlag = true;
			}

			if (col.OBBCom(player->GetCollision(), target[i]->GetCollision()))
			{
				count++;
				collect->Play(0, 0, 0);
				target[i].release();
				guardian[i].release();
			}
		}
	}

	if (hitFlag)
	{
		wait(0.397f, [&]() {next = new GameOverScene; });
	}

	if (count==5)
	{
		//	収集対象を5個集めたらクリアシーン
		next = new ClearScene;
	}

	if (outTime<=EPSIRON)
	{
		//	ステージ範囲外にoutTime以上出ていたらゲームオーバー
		next = new GameOverScene;
	}

	//	UI
	RECT rect;
	GetClientRect(Win::Window::GetInstance(), &rect);
	orthoProj._11 = 2.f / rect.right;
	orthoProj._22 = 2.f / rect.bottom;

	return next;
}

SceneBase* GameScene::Draw(SceneManager*)
{
	SceneBase* next = this;

	player->Draw();
	for (int i = 0; i < 5; i++)
	{
		if (target[i])
		{
			target[i]->Draw();
			guardian[i]->Draw();
		}
	
	}
	stage->Draw();
	
	//	UI
	TCHAR timeStr[100];
	if (player->pos.x > 499.f || player->pos.x<-499.f || player->pos.y>499.f || player->pos.y<-499.f || player->pos.z>499.f || player->pos.z < -499.f)
	{
		auto& dev = D3D9::Device::GetInstance();
		D3DXMATRIX w;
		w = projWorld;
		dev->SetTransform(D3DTS_WORLD, &w);
		dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		dev->SetRenderState(D3DRS_LIGHTING, FALSE);
		dev->SetTransform(D3DTS_PROJECTION, &orthoProj);
		D3DXMATRIX view;
		D3DXMatrixIdentity(&view);
		dev->SetTransform(D3DTS_VIEW, &view);
		dev->SetTexture(0, UITex.GetTex());
		dev->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
		dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, billboard, sizeof(VERTEX));

		dev->SetRenderState(D3DRS_LIGHTING, TRUE);

		//	outTime
		outTime -= 1.f / 60.f;
		sprintf_s(timeStr, "エリアから離れろ!!\n%12.2f",outTime);
		RECT r;
		GetClientRect(Win::Window::GetInstance(), &r);
		RECT outRect = { r.right-r.right/6,0,0,0 };
		outFont->DrawTextA(NULL, timeStr, -1, &outRect, DT_CALCRECT, NULL);
		outFont->DrawTextA(NULL, timeStr, -1, &outRect, DT_BOTTOM, 0xffe2041b);
	}
	else
	{
		outTime = 5.f;
	}

	//	Collect
	TCHAR str[100];
	sprintf_s(str, "CollectionCount : %d/5", count);
	RECT rect = { 20,20,0,0 };
	countFont->DrawTextA(NULL, str, -1, &rect, DT_CALCRECT, NULL);
	countFont->DrawTextA(NULL, str, -1, &rect, DT_LEFT | DT_BOTTOM, 0xfffcc800);

	return next;
}

//------------------------------------
//			CLEAR
//------------------------------------
ClearScene::ClearScene()
{
	clear.Load("clear.png");
	//	sound
	audioClear.Load("Clear.wav");
	auto clear = audioClear.GetBuf("Clear.wav");
	clear->Play(0, 0, 0);
}

SceneBase* ClearScene::Update(SceneManager*)
{
	SceneBase* next = this;

	//	クリア画像
	RECT client;
	GetClientRect(Win::Window::GetInstance(), &client);
	clearVtx[0] = { D3DXVECTOR4(0.f,(float)client.bottom,0.f,1.f),					D3DCOLOR_XRGB(255,255,255), D3DXVECTOR2(0.f,1.f) };
	clearVtx[1] = { D3DXVECTOR4(0.f,0.f,0.f,1.f),									D3DCOLOR_XRGB(255,255,255), D3DXVECTOR2(0.f,0.f) };
	clearVtx[2] = { D3DXVECTOR4((float)client.right,(float)client.bottom,0.f,1.f),	D3DCOLOR_XRGB(255,255,255), D3DXVECTOR2(1.f,1.f) };
	clearVtx[3] = { D3DXVECTOR4((float)client.right,0.f,0.f,1.f),					D3DCOLOR_XRGB(255,255,255), D3DXVECTOR2(1.f,0.f) };
	
	//	入力取得
	Input::Keyboard kb;
	kb.Update();
	//	Enterキーを押すとタイトルの戻る
	if (kb.Press(VK_RETURN))
	{
		next = new TitleScene;
	}

	return next;
}

SceneBase* ClearScene::Draw(SceneManager*)
{
	SceneBase* next = this;

	D3D9::Device::GetInstance()->SetMaterial(&mat);
	D3D9::Device::GetInstance()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	D3D9::Device::GetInstance()->SetTexture(0, clear.GetTex());
	D3D9::Device::GetInstance()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, clearVtx, sizeof(VERTEX));

	return next;
}

//------------------------------------
//			GAMEOVER
//------------------------------------
GameOverScene::GameOverScene()
{
	gameOver.Load("gameover.png");
	//	sound
	audioGameOver.Load("GameOver.wav");
	auto gameOver = audioGameOver.GetBuf("GameOver.wav");
	gameOver->Play(0, 0, 0);
}

SceneBase* GameOverScene::Update(SceneManager*)
{
	SceneBase* next = this;

	RECT client;
	GetClientRect(Win::Window::GetInstance(), &client);
	gameOverVtx[0] = { D3DXVECTOR4(0.f,(float)client.bottom,0.f,1.f),					D3DCOLOR_XRGB(255,255,255), D3DXVECTOR2(0.f,1.f) };
	gameOverVtx[1] = { D3DXVECTOR4(0.f,0.f,0.f,1.f),									D3DCOLOR_XRGB(255,255,255), D3DXVECTOR2(0.f,0.f) };
	gameOverVtx[2] = { D3DXVECTOR4((float)client.right,(float)client.bottom,0.f,1.f),	D3DCOLOR_XRGB(255,255,255), D3DXVECTOR2(1.f,1.f) };
	gameOverVtx[3] = { D3DXVECTOR4((float)client.right,0.f,0.f,1.f),					D3DCOLOR_XRGB(255,255,255), D3DXVECTOR2(1.f,0.f) };

	//	入力取得
	Input::Keyboard kb;
	kb.Update();
	//	Enterキーを押すとタイトルに戻る
	if (kb.Press(VK_RETURN))
	{
		next = new TitleScene;
	}

	return next;
}

SceneBase* GameOverScene::Draw(SceneManager*)
{
	SceneBase* next = this;

	D3D9::Device::GetInstance()->SetMaterial(&mat);
	D3D9::Device::GetInstance()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	D3D9::Device::GetInstance()->SetTexture(0, gameOver.GetTex());
	D3D9::Device::GetInstance()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, gameOverVtx, sizeof(VERTEX));

	return next;
}