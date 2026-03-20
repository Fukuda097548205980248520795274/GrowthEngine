#pragma once
#include <memory>

#include "Resource/ConstantBufferResource/ConstantBufferResource.h"

#include "Data/Render2DData/Render2DData.h"
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
	class Render2DParameter;

	class SpriteResource
	{
	public:

		/// @brief コンストラクタ
		/// @param hSprite 
		SpriteResource(Render2DHandle hRender2D, std::string name, Render2DParameter* parameter) : hRender2D_(hRender2D), name_(name), parameter_(parameter) {}

		/// @brief 初期化
		/// @param vertexResource 
		/// @param indexResource 
		/// @param device 
		void Initialize(VertexBufferResource<SpriteVertexData>* vertexResource, IndexBufferResource* indexResource, TextureStore* textureStore, TextureHandle hTexture,
			ID3D12Device* device, Log* log);

		/// @brief リセット
		void Reset();

		/// @brief コマンドリストに登録
		/// @param commandList 
		void Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso);

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief ハンドルを取得する
		/// @return 
		Render2DHandle GetHandle()const { return hRender2D_; }

		/// @brief パラメータを取得する
		/// @return 
		Render2D::Sprite::Param* GetParam()const { return param_.get(); }

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
		std::unique_ptr<Render2D::Sprite::Param> param_ = nullptr;

		/// @brief テクスチャファイルパス
		std::string textureFilePath_{};

		/// @brief ハンドル
		Render2DHandle hRender2D_ = 0;


	private:

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;

		// 名前
		std::string name_{};

		/// @brief グループ名
		std::string group_{};


	private:

		/// @brief パラメータ
		Render2DParameter* parameter_ = nullptr;
	};
}