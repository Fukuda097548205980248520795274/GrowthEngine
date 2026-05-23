#pragma once
#include "Prefab2DData/Prefab2DBaseData.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"
#include "PSO/PSOModel/PSOPrefab2D/PSOPrefab2D.h"
#include <vector>

#include "Parameter/Prefab2DParameter/Prefab2DParameter.h"

namespace Engine
{
	class ShaderCompiler;

	class Prefab2DStore
	{
	public:

		/// @brief コンストラクタ
		Prefab2DStore();

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, Log* log);

		/// @brief 更新処理
		void Update();

		/// @brief シーン前リセット
		void PerSceneReset();

		/// @brief 読み込み
		/// @param name 
		/// @param hTexture 
		/// @param numInstance 
		/// @param textureStore 
		/// @param device 
		/// @param log 
		/// @return 
		Prefab2DHandle Load(const std::string& name, TextureHandle hTexture, uint32_t numInstance,
			TextureStore* textureStore,Camera2DStore* cameraStore, DX12Heap* heap, ID3D12Device* device, Log* log);

		/// @brief すべてのプレハブの描画処理
		/// @param commandList 
		/// @param pso 
		void AllDrawPrefab(ID3D12GraphicsCommandList* commandList);

		/// @brief プレハブの描画処理
		/// @param hPrefab2D 
		/// @param commandList 
		/// @param pso 
		void DrawPrefab(Prefab2DHandle hPrefab2D, ID3D12GraphicsCommandList* commandList);

		/// @brief プレハブの描画処理
		/// @param name 
		/// @param commandList 
		/// @param pso 
		void DrawPrefab(const std::string& name, ID3D12GraphicsCommandList* commandList);

		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param hPrefab2D 
		/// @return 
		template<typename T>
		T* GetParam(Prefab2DHandle hPrefab2D) { return static_cast<T*>(dataTable_[hPrefab2D]->GetParam()); }

		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetParam(const std::string& name) { return static_cast<T*>(dataTable_[nameTable_[name]]->GetParam()); }

		/// @brief インスタンスを作成する
		/// @tparam T 
		/// @param hPrefab2D 
		/// @return 
		template<typename T>
		T* CreateInstance(Prefab2DHandle hPrefab2D) { return static_cast<T*>(dataTable_[hPrefab2D]->CreateInstance()); }

		/// @brief インスタンスを作成する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* CreateInstance(const std::string& name) { return static_cast<T*>(dataTable_[nameTable_[name]]->CreateInstance()); }

		/// @brief リセット
		void Reset();

		/// @brief デバッグ用パラメータ
		void DebugParameter();


		// Microsoft::WRL 省略
		template<typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;


	private:

		/// @brief データテーブル
		std::vector<std::unique_ptr<Prefab2DBaseData>> dataTable_;

		/// @brief 名前テーブル
		std::unordered_map<std::string, Prefab2DHandle> nameTable_;

		// パラメータ
		std::unique_ptr<Prefab2DParameter> parameter_ = nullptr;


	private:

		// 2Dプレハブ頂点シェーダ
		ComPtr<IDxcBlob> prefab2DVS_ = nullptr;

		// 2Dプレハブピクセルシェーダ
		ComPtr<IDxcBlob> prefab2DPS_ = nullptr;

	private:

		/// @brief 2Dプレハブ用PSO
		std::unique_ptr<PSOPrefab2D> psoPrefab2D_ = nullptr;


	private:

		// 頂点リソース
		std::unique_ptr<VertexBufferResource<SpriteVertexData>> vertexResource_ = nullptr;

		/// @brief インデックスリソース
		std::unique_ptr<IndexBufferResource> indexResource_ = nullptr;
	};
}