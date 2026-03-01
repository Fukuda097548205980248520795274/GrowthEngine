#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <string>
#include "Handle/Handle.h"
#include <memory>
#include "Resource/ConstantBufferResource/ConstantBufferResource.h"
#include "Data/SkyboxData/SkyboxData.h"
#include "Resource/VertexBufferResource/VertexBufferResource.h"

namespace Engine
{
	class TextureStore;
	class Log;
	class BasePSOSkybox;
	class IndexBufferResource;

	class SkyboxResource
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hTexture 
		/// @param hSkybox 
		SkyboxResource(const std::string& name, TextureHandle hTexture, SkyboxHandle hSkybox)
			: name_(name), hTexture_(hTexture), hSkybox_(hSkybox) {
		};

		/// @brief 初期化
		/// @param textureStore 
		/// @param device 
		/// @param indexResource 
		/// @param vertexResource 
		/// @param log 
		void Initialize(TextureStore* textureStore, ID3D12Device* device, IndexBufferResource* indexResource, VertexBufferResource<Vector4>* vertexResource, Log* log);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param pso 
		/// @param viewProjection 
		void Register(ID3D12GraphicsCommandList* commandList, BasePSOSkybox* pso, const Matrix4x4& viewProjection);

		/// @brief キューブマップをコマンドリストに登録する
		/// @param commandList 
		/// @param rootParameterIndex 
		void RegisterCubeMap(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex);

		/// @brief パラメータを取得する
		/// @return 
		SkyboxData::Param* GetParam() { return param_.get(); }

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief ハンドルを取得する
		/// @return 
		SkyboxHandle GetHandle()const { return hSkybox_; }


	private:

		// 座標変換リソース
		std::unique_ptr<ConstantBufferResource<Matrix4x4>> transformationResource_ = nullptr;

		// マテリアルリソース
		std::unique_ptr<ConstantBufferResource<Vector4>> materialResource_ = nullptr;


	private:

		// 名前
		std::string name_{};

		// テクスチャハンドル
		TextureHandle hTexture_ = 0;

		// スカイボックスハンドル
		SkyboxHandle hSkybox_ = 0;

		// パラメータ
		std::unique_ptr<SkyboxData::Param> param_ = nullptr;


	private:

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;

		// インデックスリソース
		IndexBufferResource* indexResource_ = nullptr;

		// 頂点リソース
		VertexBufferResource<Vector4>* vertexResource_ = nullptr;
	};
}