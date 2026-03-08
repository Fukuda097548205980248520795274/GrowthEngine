#pragma once
#include "SpriteResource/SpriteResource.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"

#include "Parameter/SpriteParameter/SpriteParameter.h"

namespace Engine
{
	class SpriteStore
	{
	public:

		/// @brief コンストラクタ
		SpriteStore();

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
		SpriteHandle Load(const std::string& name, TextureHandle hTexture, TextureStore* textureStore, ID3D12Device* device, Log* log);

		/// @brief コマンドリストに登録する
		/// @param hSprite 
		/// @param viewProjection 
		/// @param commandList 
		/// @param pso 
		void Register(SpriteHandle hSprite, const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso);

		/// @brief コマンドリストに登録する
		/// @param name 
		/// @param viewProjection 
		/// @param commandList 
		/// @param pso 
		void Register(const std::string& name, const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso);

		/// @brief パラメータを取得する
		/// @param hSprite 
		/// @return 
		Sprite::Param* GetParam(SpriteHandle hSprite) { return dataTable_[hSprite]->GetParam(); }

		/// @brief パラメータを取得する
		/// @param name 
		/// @return 
		Sprite::Param* GetParam(const std::string& name) { return dataTable_[nameTable_[name]]->GetParam(); }

		/// @brief デバッグ用パラメータ
		void DebugParameter();


	private:

		/// @brief データテーブル
		std::vector<std::unique_ptr<SpriteResource>> dataTable_;

		/// @brief 名前テーブル
		std::unordered_map<std::string, SpriteHandle> nameTable_;


	private:

		// 頂点リソース
		std::unique_ptr<VertexBufferResource<SpriteVertexData>> vertexResource_ = nullptr;

		/// @brief インデックスリソース
		std::unique_ptr<IndexBufferResource> indexResource_ = nullptr;


	private:

		// パラメータ
		std::unique_ptr<SpriteParameter> parameter_ = nullptr;
	};
}