//
//	Model.h
//	PMD、XFileの読み込みの定義
//
#pragma once

#include<string>
#include<vector>

#include"Common.h"
#include"Texture.h"
#include"Camera.h"

namespace Model
{

	///	オブジェクト生成は禁止
	class Mesh
	{
	protected:
		Mesh() :mesh(0), meshMats(0), meshTextures(0),numMat(0) {}

		CComPtr<ID3DXMesh> mesh;							//	メッシュ
		std::vector<D3DMATERIAL9> meshMats;					//	マテリアル配列
		std::vector<Texture::Texture> meshTextures;			//	テクスチャ配列
		DWORD numMat;										//	マテリアル、テクスチャ配列の大きさ


		struct VERTEX
		{
			D3DXVECTOR3 pos;		//	頂点位置
			D3DXVECTOR3 normal;		//	法線ベクトル
			D3DXVECTOR2 uv;			//	テクスチャ座標
		};

		struct FACE
		{
			WORD vtxIndex[3];	//	3頂点のインデックス
			DWORD matNum;		//	材料番号
		};

		struct MESHDATA
		{
			std::vector<VERTEX> vertices;
			std::vector<FACE> faces;
			std::vector<D3DMATERIAL9> materials;
			std::vector<std::string> texFileName;
		};

		//	meshDataに法線ベクトルを追加
		void AddNormal(MESHDATA& meshData);

		//	meshDataをmeshにセット
		void SetMesh(const MESHDATA& meshData);

	public:
		virtual ~Mesh();

		virtual const LPD3DXMESH& GetMesh() const
		{
			return mesh.p;
		}
		virtual const DWORD& GetNumMat() const
		{
			return numMat;
		}
		virtual const D3DMATERIAL9& GetMats(DWORD index) const
		{
			return meshMats[index];
		}
		virtual const Texture::Texture& GetTextures(DWORD index) const
		{
			return meshTextures[index];
		}

	};

	//	XFile
	class XFile sealed :public Mesh
	{
	public:
		XFile() {}
		//	Xファイルを読み込む　例　"model.x"
		XFile(LPCTSTR fileName);

		//	Xファイルの読み込み
		void Load(LPCTSTR fileName);
	};

	//	PMD
	class Pmd sealed:public Mesh
	{
#pragma pack(push,1)
		struct PMDHEADER
		{
			BYTE magic[3];			//	固定値
			float version;			//	固定値
			BYTE modelName[20];		//	モデル名称
			BYTE comment[256];		//	コメント
		};

		struct PMDVERTEX
		{
			float pos[3];		//	座標
			float normal[3];	//	法線ベクトル
			float uv[2];		//	テクスチャ座標
			WORD boneNum[2];	//	ボーン番号
			BYTE boneWeight;	//	ボーン1に与える影響度
			BYTE edgeFlag;		//	輪郭有効:0	輪郭無効:1
		};

		struct PMDMATERIAL
		{
			float diffuse[3];				//	減衰色
			float alpha;					//	減衰色の不透明度
			float specularity;				//	光沢色の強さ
			float specular[3];				//	光沢色
			float mirror[3];				//	環境色
			BYTE toonIndex;
			BYTE edgeFlag;					//	輪郭フラグ
			DWORD faceVtxCount;				//	この材料で使う面頂点リストのデータ数
			char textureFileName[20];		//	テクスチャファイル名
		};

		struct PMDBONE
		{
			char boneName[20];		//	ボーン名
			WORD parentBoneIndex;	//	親ボーン番号(ない場合は0xFFFF)
			WORD tailBoneIndex;		//	tail位置のボーン番号(ない場合は0xFFFF)
			BYTE boneType;			//	ボーンの種類	0:回転	1:回転と移動	2:IK	3:不明	4:IK影響下	5:回転影響下	6:IK接続先	7:非表示	8:捻り	9:回転運動	(8,9のみMMD4.0～)
			WORD IKParentBoneIndex;	//	IKボーン番号(ない場合は0)
			float boneHeadPos[3];	//	ボーンのヘッドの位置座標
		};

		struct PMDIK
		{
			WORD IKBoneIndex;		//	IKボーン番号
			WORD IKTargetBoneIndex;	//	IKターゲットボーン番号
			BYTE IKChainLen;		//	IKチェーンの長さ
			WORD itr;				//	再帰演算回数
			float ctrlWeight;		//	演算1回あたりの制限角度
		};

		struct PMDIKDATA :public PMDIK
		{
			std::vector<WORD> IKChildBoneIndex;	//	IK影響下のボーン番号
		};
#pragma pack(pop)

		PMDHEADER pmdHeader = {};
		std::vector<PMDVERTEX> pmdVertices;
		std::vector<WORD> pmdFaces;
		std::vector<PMDMATERIAL> pmdMaterials;
		std::vector<PMDBONE> pmdBones;
		std::vector<PMDIKDATA> pmdIKData;

		void CopyMaterial(D3DMATERIAL9* material, const PMDMATERIAL& pmdMat);

	public:
		Pmd() {}
		//	PMDファイルを読み込む　例　"model.pmd"
		Pmd(LPCTSTR fileName);

		//	PMDファイルの読み込み
		void Load(LPCTSTR fileName);

		const float scale = 0.1f;		//	スケール変換定数

	};

}