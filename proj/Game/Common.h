//	Common.h
//	マクロや定数の定義

#pragma once
#ifndef _INCLUDE_Common_h_
#define _INCLUDE_Common_h_
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

#include<d3dx9.h>
#include<d3dx9math.h>
#include <Windows.h>

//	メモリの開放
#define DEL(p){ if(p) { delete(p); (p) = NULL; } }
//	配列型メモリの開放
#define DEL_ARRAY(p){ if(p) { delete[] (p);	(p) = NULL; }}
//	参照カウンタインクリメント
#define ADD(p){ if(p) { (p)->AddRef(); (p) = NULL; } }
//	参照型インクリメントの解放
#define RELEASE(p){ if(p) { (p)->Release(); (p) = NULL; } }
//	エラーの報告とアプリケーションの終了
#define ERROR_EXIT() { int line = __LINE__; const char *file = __FILE__;\
	char msg[_MAX_FNAME + _MAX_EXT + 256];\
	char drive[_MAX_DRIVE];\
	char dir[_MAX_DIR];\
	char fname[_MAX_FNAME];\
	char ext[_MAX_EXT];\
	_splitpath_s(file, drive, sizeof(drive), dir, sizeof(dir), fname, sizeof(fname), ext, sizeof(ext));\
	sprintf_s(msg, sizeof(msg),"ファイル : %s%s\r\n"\
		"行番号 : %d", fname, ext, line);\
	MessageBox(NULL, msg, "Error", MB_OK);\
	PostQuitMessage(1);\
}

//	msgに入れた文字列を出力するエラーメッセージ
#define ERROR_MSG(msg){ MessageBox(NULL, msg, "Error", MB_OK); }

#endif // !_INCLUDE_Common_h_