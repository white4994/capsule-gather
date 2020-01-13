//
//	Model.h
//	PMD�AXFile�̓ǂݍ��݂̒�`
//
#pragma once

#include<string>
#include<vector>

#include"Common.h"
#include"Texture.h"
#include"Camera.h"

namespace Model
{

	///	�I�u�W�F�N�g�����͋֎~
	class Mesh
	{
	protected:
		Mesh() :mesh(0), meshMats(0), meshTextures(0),numMat(0) {}

		CComPtr<ID3DXMesh> mesh;							//	���b�V��
		std::vector<D3DMATERIAL9> meshMats;					//	�}�e���A���z��
		std::vector<Texture::Texture> meshTextures;			//	�e�N�X�`���z��
		DWORD numMat;										//	�}�e���A���A�e�N�X�`���z��̑傫��


		struct VERTEX
		{
			D3DXVECTOR3 pos;		//	���_�ʒu
			D3DXVECTOR3 normal;		//	�@���x�N�g��
			D3DXVECTOR2 uv;			//	�e�N�X�`�����W
		};

		struct FACE
		{
			WORD vtxIndex[3];	//	3���_�̃C���f�b�N�X
			DWORD matNum;		//	�ޗ��ԍ�
		};

		struct MESHDATA
		{
			std::vector<VERTEX> vertices;
			std::vector<FACE> faces;
			std::vector<D3DMATERIAL9> materials;
			std::vector<std::string> texFileName;
		};

		//	meshData�ɖ@���x�N�g����ǉ�
		void AddNormal(MESHDATA& meshData);

		//	meshData��mesh�ɃZ�b�g
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
		//	X�t�@�C����ǂݍ��ށ@��@"model.x"
		XFile(LPCTSTR fileName);

		//	X�t�@�C���̓ǂݍ���
		void Load(LPCTSTR fileName);
	};

	//	PMD
	class Pmd sealed:public Mesh
	{
#pragma pack(push,1)
		struct PMDHEADER
		{
			BYTE magic[3];			//	�Œ�l
			float version;			//	�Œ�l
			BYTE modelName[20];		//	���f������
			BYTE comment[256];		//	�R�����g
		};

		struct PMDVERTEX
		{
			float pos[3];		//	���W
			float normal[3];	//	�@���x�N�g��
			float uv[2];		//	�e�N�X�`�����W
			WORD boneNum[2];	//	�{�[���ԍ�
			BYTE boneWeight;	//	�{�[��1�ɗ^����e���x
			BYTE edgeFlag;		//	�֊s�L��:0	�֊s����:1
		};

		struct PMDMATERIAL
		{
			float diffuse[3];				//	�����F
			float alpha;					//	�����F�̕s�����x
			float specularity;				//	����F�̋���
			float specular[3];				//	����F
			float mirror[3];				//	���F
			BYTE toonIndex;
			BYTE edgeFlag;					//	�֊s�t���O
			DWORD faceVtxCount;				//	���̍ޗ��Ŏg���ʒ��_���X�g�̃f�[�^��
			char textureFileName[20];		//	�e�N�X�`���t�@�C����
		};

		struct PMDBONE
		{
			char boneName[20];		//	�{�[����
			WORD parentBoneIndex;	//	�e�{�[���ԍ�(�Ȃ��ꍇ��0xFFFF)
			WORD tailBoneIndex;		//	tail�ʒu�̃{�[���ԍ�(�Ȃ��ꍇ��0xFFFF)
			BYTE boneType;			//	�{�[���̎��	0:��]	1:��]�ƈړ�	2:IK	3:�s��	4:IK�e����	5:��]�e����	6:IK�ڑ���	7:��\��	8:�P��	9:��]�^��	(8,9�̂�MMD4.0�`)
			WORD IKParentBoneIndex;	//	IK�{�[���ԍ�(�Ȃ��ꍇ��0)
			float boneHeadPos[3];	//	�{�[���̃w�b�h�̈ʒu���W
		};

		struct PMDIK
		{
			WORD IKBoneIndex;		//	IK�{�[���ԍ�
			WORD IKTargetBoneIndex;	//	IK�^�[�Q�b�g�{�[���ԍ�
			BYTE IKChainLen;		//	IK�`�F�[���̒���
			WORD itr;				//	�ċA���Z��
			float ctrlWeight;		//	���Z1�񂠂���̐����p�x
		};

		struct PMDIKDATA :public PMDIK
		{
			std::vector<WORD> IKChildBoneIndex;	//	IK�e�����̃{�[���ԍ�
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
		//	PMD�t�@�C����ǂݍ��ށ@��@"model.pmd"
		Pmd(LPCTSTR fileName);

		//	PMD�t�@�C���̓ǂݍ���
		void Load(LPCTSTR fileName);

		const float scale = 0.1f;		//	�X�P�[���ϊ��萔

	};

}