#pragma once
#include "PSO/PSOSkybox/PSOSkybox/PSOSkybox.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"
#include <vector>
#include "SkyboxResource/SkyboxResource.h"

namespace Engine
{
	class TextureStore;
	class DX12Heap;

	class SkyboxStore
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param commandList 
		/// @param compiler 
		/// @param textureStore 
		/// @param heap 
		/// @param log 
		void Initialize(ID3D12Device* device,ID3D12GraphicsCommandList* commandList, ShaderCompiler* compiler, TextureStore* textureStore, DX12Heap* heap, Log* log);

		/// @brief 読み込む
		/// @param name 
		/// @param hTexture 
		/// @param textureStore 
		/// @return 
		SkyboxHandle Load(const std::string& name, TextureHandle hTexture, TextureStore* textureStore, ID3D12Device* device, Log* log);

		/// @brief 切り替え
		/// @param hSkybox 
		void Switch(SkyboxHandle hSkybox) { currentHandle_ = hSkybox; }

		/// @brief 描画処理
		/// @param commandList 
		/// @param viewProjection 
		void Draw(ID3D12GraphicsCommandList* commandList, const Matrix4x4& viewProjection)
		{
			dataTable_[currentHandle_]->Register(commandList, psoSkybox_.get(), viewProjection);
		}

		/// @brief パラメータを取得する
		/// @param hSkybox 
		/// @return 
		SkyboxData::Param* GetParam(SkyboxHandle hSkybox) { return dataTable_[hSkybox]->GetParam(); }


	private:

		/// @brief 初期用読み込み
		/// @param hTexture 
		/// @param textureStore 
		/// @param device 
		/// @param log 
		SkyboxHandle InitialLoad(TextureHandle hTexture, TextureStore* textureStore, ID3D12Device* device, Log* log);

		// スカイボックスPSO
		std::unique_ptr<PSOSkybox> psoSkybox_ = nullptr;

		// 現在のハンドル
		SkyboxHandle currentHandle_ = 0;

		// データテーブル
		std::vector<std::unique_ptr<SkyboxResource>> dataTable_;


	private:

		// 頂点リソース
		std::unique_ptr<VertexBufferResource<Vector4>> vertexResource_ = nullptr;

		// インデックスリソース
		std::unique_ptr<IndexBufferResource> indexResource_ = nullptr;
	};
}