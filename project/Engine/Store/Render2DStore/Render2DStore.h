#pragma once
#include "SpriteResource/SpriteResource.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"

#include "Parameter/Render2DParameter/Render2DParameter.h"

namespace Engine
{
	class Render2DStore
	{
	public:

		/// @brief コンストラクタ
		Render2DStore();

		/// @brief 初期化
		/// @param device 
		/// @param log 
		void Initialize(ID3D12Device* device, Log* log);

		/// @brief 更新処理
		void Update();

		/// @brief 読み込み
		/// @param name 
		/// @param hTexture 
		/// @param textureStore 
		/// @return 
		Render2DHandle Load(const std::string& name, TextureHandle hTexture, TextureStore* textureStore, ID3D12Device* device, Log* log);

		/// @brief コマンドリストに登録する
		/// @param hSprite 
		/// @param viewProjection 
		/// @param commandList 
		/// @param pso 
		void Register(Render2DHandle hRender2D, const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso);

		/// @brief コマンドリストに登録する
		/// @param name 
		/// @param viewProjection 
		/// @param commandList 
		/// @param pso 
		void Register(const std::string& name, const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso);

		/// @brief パラメータを取得する
		/// @param hSprite 
		/// @return 
		Render2D::Sprite::Param* GetParam(Render2DHandle hSprite) { return dataTable_[hSprite]->GetParam(); }

		/// @brief パラメータを取得する
		/// @param name 
		/// @return 
		Render2D::Sprite::Param* GetParam(const std::string& name) { return dataTable_[nameTable_[name]]->GetParam(); }

		/// @brief デバッグ用パラメータ
		void DebugParameter();


	private:

		/// @brief データテーブル
		std::vector<std::unique_ptr<SpriteResource>> dataTable_;

		/// @brief 名前テーブル
		std::unordered_map<std::string, Render2DHandle> nameTable_;


	private:

		// 頂点リソース
		std::unique_ptr<VertexBufferResource<SpriteVertexData>> vertexResource_ = nullptr;

		/// @brief インデックスリソース
		std::unique_ptr<IndexBufferResource> indexResource_ = nullptr;


	private:

		// パラメータ
		std::unique_ptr<Render2DParameter> parameter_ = nullptr;
	};
}