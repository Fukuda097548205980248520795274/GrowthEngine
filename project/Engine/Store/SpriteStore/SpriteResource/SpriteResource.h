#pragma once
#include <memory>

#include "Resource/ConstantBufferResource/ConstantBufferResource.h"

#include "Data/SpriteData/SpriteData.h"
#include "Data/ModelData/ModelData.h"
#include "DataForGPU/MaterialDataForGPU/MaterialDataForGPU.h"
#include "DataForGPU/TransformationDataForGPU/TransformationDataForGPU.h"

#include "Resource/VertexBufferResource/VertexBufferResource.h"

namespace Engine
{
	class Log;
	class BasePSOModel;
	class TextureStore;
	class IndexBufferResource;
	class SpriteParameter;

	class SpriteResource
	{
	public:

		/// @brief コンストラクタ
		/// @param hSprite 
		SpriteResource(SpriteHandle hSprite, std::string name, SpriteParameter* parameter) : hSprite_(hSprite_), name_(name), parameter_(parameter) {}

		/// @brief 初期化
		/// @param vertexResource 
		/// @param indexResource 
		/// @param device 
		void Initialize(VertexBufferResource<SpriteVertexData>* vertexResource, IndexBufferResource* indexResource, TextureStore* textureStore, TextureHandle hTexture,
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

		/// @brief デバッグ用パラメータ
		void DebugParameter();


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

		/// @brief テクスチャファイルパス
		std::string textureFilePath_{};

		/// @brief ハンドル
		SpriteHandle hSprite_ = 0;


	private:

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;

		// 名前
		std::string name_{};

		/// @brief グループ名
		std::string group_{};


	private:

		/// @brief パラメータ
		SpriteParameter* parameter_ = nullptr;
	};
}