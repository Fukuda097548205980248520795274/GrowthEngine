#pragma once
#include "Data/ModelData/ModelData.h"
#include "Handle/Handle.h"
#include <memory>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "Resource/VertexBufferResource/VertexBufferResource.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"

namespace Engine
{
	class Log;
	class TextureStore;
	class DX12Heap;

	class ModelResource
	{
	public:

		/// @brief 初期化
		/// @param device 
		void Initialize(const std::string& directory, const std::string& fileName, ModelHandle handle,
			TextureStore* textureStore, DX12Heap* heap, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, Log* lo);


		/// @brief モデルデータを取得する
		/// @return 
		ModelData GetModelData() { return modelData_; }

		/// @brief ファイルパスを取得する
		/// @return 
		std::string GetFilePath()const { return filePath_; }

		/// @brief ハンドルを取得する
		/// @return 
		ModelHandle GetHandle()const { return handle_; }

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param meshIndex 
		void Register(ID3D12GraphicsCommandList* commandList, UINT meshIndex);


	private:

		// モデルデータ
		ModelData modelData_{};

		// モデルハンドル
		ModelHandle handle_ = 0;


		// ファイルパス
		std::string filePath_{};


	private:

		/// @brief 頂点テーブル
		std::vector<std::unique_ptr<VertexBufferResource<ModelVertexData>>> vertexTable_;

		/// @brief インデックステーブル
		std::vector<std::unique_ptr<IndexBufferResource>> indexTable_;
	};
}