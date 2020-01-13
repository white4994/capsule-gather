//
//	D3D9.cpp
//	Direct3D�̍쐬�A�`��Ȃǂ̎���
//

#include"D3D9.h"

namespace D3D9
{
	/*	������
		����
			hWnd	: �E�B���h�E�n���h��
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

		// Direct3D9�I�u�W�F�N�g�̍쐬
		if (d3d9 == nullptr)
		{
			ERROR_MSG("Direct3D�̍쐬�Ɏ��s���܂���");
			return;	// ���������s
		}

		// ���݂̃f�B�X�v���C���[�h���擾
		d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);

		// �f�o�C�X�̃v���[���e�[�V�����p�����[�^��������
		ZeroMemory(&pp, sizeof(D3DPRESENT_PARAMETERS));
		if (winMode)	//	�t���X�N���[���̏ꍇ
		{
			pp.BackBufferWidth = dm.Width;
			pp.BackBufferHeight = dm.Height;
			pp.Windowed = FALSE;
		}
		else    //	�E�B���h�E���[�h�̏ꍇ
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


		// �f�B�X�v���C�A�_�v�^��\�����߂̃f�o�C�X���쐬
		// �`��ƒ��_�������n�[�h�E�F�A�ōs��
		if (FAILED(d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &dev)))
		{
			//	�`����n�[�h�E�F�A�A���_������CPU�ōs��
			if (FAILED(d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &dev)))
			{
				ERROR_MSG("HAL���[�h��DIRECT3D�f�o�C�X���쐬�ł��܂���\n\
							\rREF���[�h�ōĎ��s���܂�");
				//	�`��A���_������CPU�ōs��
				if (FAILED(d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &dev)))
				{
					ERROR_MSG("DIRECT3D�f�o�C�X�̍쐬�Ɏ��s���܂���");
					return;
				}
			}
		}

		//	�J�����O���Ȃ�
		dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		//	�A���t�@
		dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		//	���_�J���[�̎Q�ƌ�
		dev->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
		dev->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
		dev->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
		dev->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

		//	�J���[�u�����f�B���O
		dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

		//	�A���t�@�u�����f�B���O
		dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		//	���C�g
		dev->SetLight(0, &light);
		dev->LightEnable(0, TRUE);
		dev->SetRenderState(D3DRS_LIGHTING, TRUE);
	}

	/*	�`��J�n
		����
			color	: ��ʃN���A�̍ۂɓh��Ԃ��F
	*/
	void Device::Begin(D3DXCOLOR color)
	{
		dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, color, 1.0f, 0);
		dev->BeginScene();
	}

	/*	�`��I��
	*/
	void Device::End(HWND hWnd)
	{
		dev->EndScene();
		dev->Present(NULL, NULL, hWnd, NULL);
	}

	//	�V���O���g���Q�b�^�[
	//	�C���X�^���X�̎擾	�����������˂Ă���̂ōŏ��̐������͈���HWND�͕K������邱��
	//	winMode�̓E�B���h�E���[�h�̑I���@true�Ńt���X�N���[��
	Device& Device::GetInstance(HWND hWnd,bool winMode)
	{
		static Device dev(hWnd,winMode);
		return dev;
	}
}