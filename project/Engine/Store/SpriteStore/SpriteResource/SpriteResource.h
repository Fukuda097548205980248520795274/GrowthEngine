#pragma once
#include <memory>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "Resource/ConstantBufferResource/ConstantBufferResource.h"

#include "Data/SpriteData/SpriteData.h"
#include "Data/ModelData/ModelData.h"
#include "DataForGPU/MaterialDataForGPU/MaterialDataForGPU.h"
#include "DataForGPU/TransformationDataForGPU/TransformationDataForGPU.h"

#include "Resource/VertexBufferResource/VertexBufferResource.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"

namespace Engine
{
	class Log;
	class BasePSOModel;
	class TextureStore;

	class SpriteResource
	{
	public:

		/// @brief コンストラクタ
		/// @param hSprite 
		SpriteResource(SpriteHandle hSprite, TextureHandle hTexture, std::string name);

		/// @brief 初期化
		/// @param vertexResource 
		/// @param indexResource 
		/// @param device 
		void Initialize(VertexBufferResource<SpriteVertexData>* vertexResource, IndexBufferResource* indexResource, TextureStore* textureStore,
			ID3D12Device* device, Log* log);

		/// @brief コマンドリストに登録
		/// @param commandList 
		void Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso);

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief ハンドルを取得する
		/// @return 
		SpriteHandle GetHandle()const { return hSprite_; }


	private:

		// 頂点リソース
		VertexBufferResource<SpriteVertexData>* vertexResource_ = nullptr;

		// インデックスリソース
		IndexBufferResource* indexResource_ = nullptr;

		// マテリアルリソース
		std::unique_ptr<ConstantBufferResource<Sprite::MaterialDataForGPU>> materialResource_ = nullptr;

		// 座標変換リソース
		std::unique_ptr<ConstantBufferResource<Sprite::TransformationDataForGPU>> transformationResource_ = nullptr;


	private:

		/// @brief パラメータ
		std::unique_ptr<Sprite::Param> param_ = nullptr;

		/// @brief ハンドル
		SpriteHandle hSprite_ = 0;


	private:

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;

		// 名前
		std::string name_{};
	};
}