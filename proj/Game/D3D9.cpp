//
//	D3D9.cpp
//	Direct3Dの作成、描画などの実装
//

#include"D3D9.h"

namespace D3D9
{
	/*	初期化
		引数
			hWnd	: ウィンドウハンドル
	*/
	Device::Device(HWND hWnd,bool winMode)
		:d3d9(Direct3DCreate9(D3D_SDK_VERSION))
	{
		D3DLIGHT9 light = {
			D3DLIGHT_DIRECTIONAL,
			{1.f,1.f,1.f,1.f},
			{1.f,1.f,1.f,1.f},
			{0.5f,0.5f,0.5f,1.f},
			{0.f,0.f,0.f},
			{0.f,-1.f,0.f},
		};

		D3DDISPLAYMODE dm;
		D3DPRESENT_PARAMETERS pp;

		// Direct3D9オブジェクトの作成
		if (d3d9 == nullptr)
		{
			ERROR_MSG("Direct3Dの作成に失敗しました");
			return;	// 初期化失敗
		}

		// 現在のディスプレイモードを取得
		d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);

		// デバイスのプレゼンテーションパラメータを初期化
		ZeroMemory(&pp, sizeof(D3DPRESENT_PARAMETERS));
		if (winMode)	//	フルスクリーンの場合
		{
			pp.BackBufferWidth = dm.Width;
			pp.BackBufferHeight = dm.Height;
			pp.Windowed = FALSE;
		}
		else    //	ウィンドウモードの場合
		{
			pp.BackBufferWidth = 0;
			pp.BackBufferHeight = 0;
			pp.Windowed = TRUE;
		}
		pp.BackBufferFormat = D3DFMT_A8R8G8B8;
		pp.BackBufferCount = 1;
		pp.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;
		pp.MultiSampleQuality = 0;
		pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		pp.hDeviceWindow = hWnd;
		pp.EnableAutoDepthStencil = TRUE;
		pp.AutoDepthStencilFormat = D3DFMT_D24S8;
		pp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
		pp.FullScreen_RefreshRateInHz = 0;
		pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;


		// ディスプレイアダプタを表すためのデバイスを作成
		// 描画と頂点処理をハードウェアで行う
		if (FAILED(d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &dev)))
		{
			//	描画をハードウェア、頂点処理をCPUで行う
			if (FAILED(d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &dev)))
			{
				ERROR_MSG("HALモードでDIRECT3Dデバイスを作成できません\n\
							\rREFモードで再試行します");
				//	描画、頂点処理をCPUで行う
				if (FAILED(d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &dev)))
				{
					ERROR_MSG("DIRECT3Dデバイスの作成に失敗しました");
					return;
				}
			}
		}

		//	カリングしない
		dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		//	アルファ
		dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		//	頂点カラーの参照元
		dev->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
		dev->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
		dev->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
		dev->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

		//	カラーブレンディング
		dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

		//	アルファブレンディング
		dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		//	ライト
		dev->SetLight(0, &light);
		dev->LightEnable(0, TRUE);
		dev->SetRenderState(D3DRS_LIGHTING, TRUE);
	}

	/*	描画開始
		引数
			color	: 画面クリアの際に塗りつぶす色
	*/
	void Device::Begin(D3DXCOLOR color)
	{
		dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, color, 1.0f, 0);
		dev->BeginScene();
	}

	/*	描画終了
	*/
	void Device::End(HWND hWnd)
	{
		dev->EndScene();
		dev->Present(NULL, NULL, hWnd, NULL);
	}

	//	シングルトンゲッター
	//	インスタンスの取得	初期化を兼ねているので最初の生成時は引数HWNDは必ず入れること
	//	winModeはウィンドウモードの選択　trueでフルスクリーン
	Device& Device::GetInstance(HWND hWnd,bool winMode)
	{
		static Device dev(hWnd,winMode);
		return dev;
	}
}