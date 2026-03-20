#pragma once

#include <memory>

#include "Resource/ConstantBufferResource/ConstantBufferResource.h"
#include "Data/Render2DData/Render2DData.h"
#include "Data/ModelData/ModelData.h"

#include "DataForGPU/MaterialDataForGPU/MaterialDataForGPU.h"
#include "DataForGPU/TransformationDataForGPU/TransformationDataForGPU.h"

#include "Resource/VertexBufferResource/VertexBufferResource.h"
#include "DataForGPU/VertexDataForGPU/VertexDataForGPU.h"

namespace Engine
{
	class Log;
	class BasePSOModel;
	class TextureStore;
	class IndexBufferResource;
	class Render2DParameter;

	class Render2DBaseData
	{
	public:

		/// @brief 仮想デストラクタ
		virtual ~Render2DBaseData() = default;

		/// @brief コンストラクタ
		/// @param hSprite 
		Render2DBaseData(Render2DHandle hRender2D, std::string name, Render2DParameter* parameter) : hRender2D_(hRender2D), name_(name), parameter_(parameter) {}

		/// @brief リセット
		virtual void Reset() = 0;

		/// @brief コマンドリストに登録
		/// @param commandList 
		virtual void Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso) = 0;

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief ハンドルを取得する
		/// @return 
		Render2DHandle GetHandle()const { return hRender2D_; }

		/// @brief パラメータを取得する
		/// @return 
		virtual void* GetParam() = 0;

		/// @brief デバッグ用パラメータ
		virtual void DebugParameter() = 0;


	protected:

		// 頂点リソース
		VertexBufferResource<SpriteVertexData>* vertexResource_ = nullptr;

		// インデックスリソース
		IndexBufferResource* indexResource_ = nullptr;


	protected:

		/// @brief テクスチャファイルパス
		std::string textureFilePath_{};

		/// @brief ハンドル
		Render2DHandle hRender2D_ = 0;


	protected:

		// 名前
		std::string name_{};

		/// @brief グループ名
		std::string group_{};


	protected:

		/// @brief パラメータ
		Render2DParameter* parameter_ = nullptr;
	};
}