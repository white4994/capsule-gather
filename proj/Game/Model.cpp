//
//	Model.cpp
//	PMD、XFileの読み込みの実装
//
#include<fstream>
#include<iosfwd>
#include<unordered_map>

#include"Model.h"

namespace Model
{
	///	オブジェクト生成は禁止
	Mesh::~Mesh()
	{
	}


	//	meshDataに法線ベクトルを追加
	void Mesh::AddNormal(MESHDATA& meshData)
	{

		for (UINT i = 0; i < meshData.vertices.size(); ++i)meshData.vertices[i].normal = D3DXVECTOR3(0, 0, 0);
		for (UINT i = 0; i < meshData.faces.size(); ++i)
		{
			D3DXVECTOR3 p[3];
			for (UINT j = 0; j < 3; ++j)p[j] = meshData.vertices[meshData.faces[i].vtxIndex[j]].pos;
			D3DXPLANE plane;
			D3DXPlaneFromPoints(&plane, &p[0], &p[1], &p[2]);
			for (UINT j = 0; j < 3; ++j)meshData.vertices[meshData.faces[i].vtxIndex[j]].normal += D3DXVECTOR3(plane.a, plane.b, plane.c);
			float l = D3DXVec3Length(&D3DXVECTOR3(plane.a, plane.b, plane.c));
		}
		for (UINT i = 0; i < meshData.vertices.size(); ++i)D3DXVec3Normalize(&meshData.vertices[i].normal, &meshData.vertices[i].normal);
	}

	//	meshDataをmeshにセット
	void Mesh::SetMesh(const MESHDATA& meshData)
	{
		D3DXCreateMeshFVF((DWORD)meshData.faces.size(), (DWORD)meshData.vertices.size(), D3DXMESH_MANAGED, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, D3D9::Device::GetInstance(), &mesh);
		VERTEX* vtxBuf;
		mesh->LockVertexBuffer(0, (void**)&vtxBuf);
		for (UINT i = 0; i < meshData.vertices.size(); ++i)
		{
			vtxBuf[i].pos = meshData.vertices[i].pos;
			vtxBuf[i].normal = meshData.vertices[i].normal;
			vtxBuf[i].uv = meshData.vertices[i].uv;
		}
		mesh->UnlockVertexBuffer();

		WORD* indexBuf;
		mesh->LockIndexBuffer(0, (void**)&indexBuf);
		for (UINT i = 0; i < meshData.faces.size(); ++i)for (UINT j = 0; j < 3; ++j)indexBuf[3 * i + j] = meshData.faces[i].vtxIndex[j];
		mesh->UnlockIndexBuffer();

		DWORD* atrBuf;
		mesh->LockAttributeBuffer(0, &atrBuf);
		for (UINT i = 0; i < meshData.faces.size(); ++i)atrBuf[i] = meshData.faces[i].matNum;
		mesh->UnlockAttributeBuffer();

		numMat = (DWORD)meshData.materials.size();
		meshMats.resize(numMat);
		meshTextures.resize(numMat);
		for (DWORD i = 0; i < numMat; ++i)meshTextures[i] = 0;
		for (DWORD i = 0; i < numMat; ++i)
		{
			meshMats[i] = meshData.materials[i];
			char texFileName[256] = { 0 };
			TCHAR textureFileName[256] = { 0 };
			if (strcpy_s(texFileName, meshData.texFileName[i].c_str()))throw TEXT("テクスチャの読み込みに失敗しました");
#ifdef UNICODE
			if (strlen(texFileName) > 0)MultiByteToWideChar(CP_OEMCP, MB_PRECOMPOSED, texFileName, strlen(texFileName), textureFileName, sizeof(textureFileName) / 2);
#else
			if (strlen(texFileName) > 0)strcpy_s(textureFileName, texFileName);
#endif // UNICODE
			if (lstrlen(textureFileName) > 0)
			{
				Texture::Texture tex(textureFileName);
				meshTextures[i] = tex;
			}
		}
	}

	std::unordered_map<std::string, XFile>XList;
	void XFile::Load(LPCTSTR fileName)
	{
		if (fileName == nullptr || *fileName == '\0')
		{
			return;
		}

		auto itr = XList.find(fileName);
		if (itr == XList.end())
		{
			LPD3DXBUFFER buf = NULL;
			if (FAILED(D3DXLoadMeshFromX(fileName, D3DXMESH_SYSTEMMEM, D3D9::Device::GetInstance(), NULL, &buf, NULL, &numMat, &mesh)))throw TEXT("Xファイルの読み込みに失敗しました");
			D3DXMATERIAL* mats = (D3DXMATERIAL*)buf->GetBufferPointer();
			meshMats.resize(numMat);
			meshTextures.resize(numMat);
			for (DWORD i = 0; i < numMat; ++i)
			{
				meshMats[i] = mats[i].MatD3D;
				meshTextures[i] = 0;
				TCHAR texFileName[256] = { 0 };
#ifdef UNICODE
				if (mats[i].pTextureFilename)MultiByteToWideChar(CP_OEMCP, MB_PRECOMPOSED, mats[i].pTextureFilename, strlen(mats[i].pTextureFilename), texFileName, (sizeof texFileName) / 2);
#else
				if (mats[i].pTextureFilename)strcpy_s(texFileName, mats[i].pTextureFilename);
#endif // UNICODE
				if (texFileName != NULL && lstrlen(texFileName) > 0)
				{
					Texture::Texture tex(texFileName);
					meshTextures[i] = tex;
				}

			}
		}
		else
		{
			mesh = itr->second.mesh;
			numMat = itr->second.numMat;
			for (DWORD i = 0; i < numMat; i++)
			{
				meshMats[i] = itr->second.meshMats[i];
				meshTextures[i] = itr->second.meshTextures[i];
			}
		}
	}

	XFile::XFile(LPCTSTR fileName)
		:XFile()
	{
		Load(fileName);
	}

	std::unordered_map<std::string, Pmd>PmdList;
	void Pmd::CopyMaterial(D3DMATERIAL9* material, const PMDMATERIAL& pmdMat)
	{
		material->Ambient.a = pmdMat.alpha;
		material->Ambient.r = pmdMat.mirror[0];
		material->Ambient.g = pmdMat.mirror[1];
		material->Ambient.b = pmdMat.mirror[2];
		material->Diffuse.a = pmdMat.alpha;
		material->Diffuse.r = pmdMat.diffuse[0];
		material->Diffuse.g = pmdMat.diffuse[1];
		material->Diffuse.b = pmdMat.diffuse[2];
		material->Power = pmdMat.specularity;
		material->Specular.a = pmdMat.alpha;
		material->Specular.r = pmdMat.specular[0];
		material->Specular.g = pmdMat.specular[1];
		material->Specular.b = pmdMat.specular[2];
	}

	void Pmd::Load(LPCTSTR fileName)
	{
		if (fileName == nullptr || *fileName == '\0')
		{
			return;
		}

		
		auto itr = PmdList.find(fileName);
		if (itr == PmdList.end())
		{
			//	PMDファイルからPMDデータを抽出
			std::ifstream ifs(fileName, std::ios::binary);
			if (ifs.fail())throw TEXT("ファイルがありません");
			ifs.read((char*)&pmdHeader, sizeof(pmdHeader));
			DWORD numPmdVertex;
			ifs.read((char*)&numPmdVertex, sizeof(numPmdVertex));
			pmdVertices.resize(numPmdVertex);
			ifs.read((char*)& pmdVertices[0], (std::streamsize)sizeof(PMDVERTEX) * pmdVertices.size());
			DWORD numPmdFace;
			ifs.read((char*)&numPmdFace, sizeof(numPmdFace));
			pmdFaces.resize(numPmdFace);
			ifs.read((char*)&pmdFaces[0], (std::streamsize)sizeof(WORD) * pmdFaces.size());
			DWORD numPmdMaterial;
			ifs.read((char*)&numPmdMaterial, sizeof(numPmdMaterial));
			pmdMaterials.resize(numPmdMaterial);
			ifs.read((char*)&pmdMaterials[0], (std::streamsize)sizeof(PMDMATERIAL) * pmdMaterials.size());

			//	PMDデータからメッシュデータにコピー
			MESHDATA meshData;
			for (UINT i = 0; i < numPmdVertex; ++i)
			{
				VERTEX v;
				v.pos = scale * D3DXVECTOR3(pmdVertices[i].pos[0], pmdVertices[i].pos[1], pmdVertices[i].pos[2]);
				v.normal = D3DXVECTOR3(pmdVertices[i].normal[0], pmdVertices[i].normal[1], pmdVertices[i].normal[2]);
				v.uv = D3DXVECTOR2(pmdVertices[i].uv[0], pmdVertices[i].uv[1]);
				meshData.vertices.push_back(v);
			}

			FACE f;
			for (UINT i = 0; i < numPmdFace; ++i)
			{
				f.vtxIndex[i % 3] = pmdFaces[i];
				if (i % 3 == 2)meshData.faces.push_back(f);
			}

			D3DMATERIAL9 mat = { 0 };
			UINT j = 0, materialEnd = 0;
			for (UINT i = 0; i < numPmdMaterial; ++i)
			{
				CopyMaterial(&mat, pmdMaterials[i]);
				meshData.materials.push_back(mat);
				char tex[21] = { 0 };
				memcpy(tex, pmdMaterials[i].textureFileName, 20);
				std::string s(tex);
				s = s.substr(0, s.find("*"));
				meshData.texFileName.push_back(s);
				materialEnd += pmdMaterials[i].faceVtxCount;
				for (; j < materialEnd; ++j)meshData.faces[j / 3].matNum = i;
			}

			SetMesh(meshData);
		}
		else
		{
			mesh = itr->second.mesh;
			numMat = itr->second.numMat;
			for (DWORD i = 0; i < numMat; i++)
			{
				meshMats[i] = itr->second.meshMats[i];
				meshTextures[i] = itr->second.meshTextures[i];
			}
		}
	}

	Pmd::Pmd(LPCTSTR fileName)
		:Pmd()
	{
		Load(fileName);
	}
}