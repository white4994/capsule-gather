//	Common.h
//	�}�N����萔�̒�`

#pragma once
#ifndef _INCLUDE_Common_h_
#define _INCLUDE_Common_h_
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

#include<d3dx9.h>
#include<d3dx9math.h>
#include <Windows.h>

//	�������̊J��
#define DEL(p){ if(p) { delete(p); (p) = NULL; } }
//	�z��^�������̊J��
#define DEL_ARRAY(p){ if(p) { delete[] (p);	(p) = NULL; }}
//	�Q�ƃJ�E���^�C���N�������g
#define ADD(p){ if(p) { (p)->AddRef(); (p) = NULL; } }
//	�Q�ƌ^�C���N�������g�̉��
#define RELEASE(p){ if(p) { (p)->Release(); (p) = NULL; } }
//	�G���[�̕񍐂ƃA�v���P�[�V�����̏I��
#define ERROR_EXIT() { int line = __LINE__; const char *file = __FILE__;\
	char msg[_MAX_FNAME + _MAX_EXT + 256];\
	char drive[_MAX_DRIVE];\
	char dir[_MAX_DIR];\
	char fname[_MAX_FNAME];\
	char ext[_MAX_EXT];\
	_splitpath_s(file, drive, sizeof(drive), dir, sizeof(dir), fname, sizeof(fname), ext, sizeof(ext));\
	sprintf_s(msg, sizeof(msg),"�t�@�C�� : %s%s\r\n"\
		"�s�ԍ� : %d", fname, ext, line);\
	MessageBox(NULL, msg, "Error", MB_OK);\
	PostQuitMessage(1);\
}

//	msg�ɓ��ꂽ��������o�͂���G���[���b�Z�[�W
#define ERROR_MSG(msg){ MessageBox(NULL, msg, "Error", MB_OK); }

#endif // !_INCLUDE_Common_h_