#pragma once
#include "Render2DData/Render2DBaseData.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"

#include "Parameter/Render2DParameter/Render2DParameter.h"

namespace Engine
{
	class FontStore;

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

		/// @brief シーン前のリセット
		void PerSceneReset();

		/// @brief 読み込み
		/// @param name 
		/// @param type 
		/// @param hTexture 
		/// @param hText 
		/// @param textureStore 
		/// @param fontStore 
		/// @param device 
		/// @param log 
		/// @return 
		Render2DHandle Load(const std::string& name, Render2D::Type type, TextureHandle hTexture, TextHandle hText,
			TextureStore* textureStore, FontStore* fontStore, ID3D12Device* device, Log* log);

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
		/// @tparam T 
		/// @param hRender2D 
		/// @return 
		template<typename T>
		T* GetParam(Render2DHandle hRender2D)
		{
			return static_cast<T*>(dataTable_[hRender2D]->GetParam());
		}

		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetParam(const std::string& name)
		{
			return static_cast<T*>(dataTable_[nameTable_[name]]->GetParam());
		}


	public:

		/// @brief デバッグ用パラメータ
		void DebugParameter();

		/// @brief デバッグ用ピッキング
		/// @param point 
		/// @param pickList 
		void DebugPicking(const Vector2& point, std::vector<std::pair<float, bool*>>& pickList);


	private:

		/// @brief データテーブル
		std::vector<std::unique_ptr<Render2DBaseData>> dataTable_;

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