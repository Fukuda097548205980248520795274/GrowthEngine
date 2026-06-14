#pragma once
#include "Render2DData/Render2DBaseData.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"
#include "PSO/PSOModel/PSORender2D/PSORender2D.h"

#include "Parameter/Render2DParameter/Render2DParameter.h"

class WorldTransform2D;

namespace Engine
{
	class FontStore;
	class Camera2DStore;
	class ShaderCompiler;

	class Render2DStore
	{
	public:

		/// @brief コンストラクタ
		Render2DStore();

		/// @brief 初期化
		/// @param device 
		/// @param log 
		void Initialize(ID3D12Device* device,ShaderCompiler* compiler, Log* log);

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
		void Register(Render2DHandle hRender2D, Camera2DStore* cameraStore, ID3D12GraphicsCommandList* commandList);

		/// @brief コマンドリストに登録する
		/// @param name 
		/// @param viewProjection 
		/// @param commandList 
		void Register(const std::string& name, Camera2DStore* cameraStore, ID3D12GraphicsCommandList* commandList);

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

		/// @brief 親を設定する
		/// @param handle 
		/// @param parent 
		void SetParent(Render2DHandle handle, WorldTransform2D* parent) { dataTable_[handle]->SetParent(parent); }

		/// @brief 親を設定する
		/// @param name 
		/// @param parent 
		void SetParent(const std::string& name, WorldTransform2D* parent) { dataTable_[nameTable_[name]]->SetParent(parent); }


	public:

		/// @brief デバッグ用パラメータ
		void DebugParameter();

		/// @brief デバッグ用ピッキング
		/// @param point 
		/// @param pickList 
		void DebugPicking(const Vector2& point, std::vector<std::pair<float, DebugData::DebugGuizmoData*>>& pickList);

		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

	private:

		// 2D描画頂点シェーダ
		ComPtr<IDxcBlob> render2DVS_ = nullptr;

		// 2D描画ピクセルシェーダ
		ComPtr<IDxcBlob> render2DPS_ = nullptr;

		/// @brief テキストピクセルシェーダ
		ComPtr<IDxcBlob> textPS_ = nullptr;


	private:

		// 2D描画PSO
		std::unique_ptr<PSORender2D> psoRender2D_ = nullptr;

		// テキストPSO
		std::unique_ptr<PSORender2D> psoText_ = nullptr;


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