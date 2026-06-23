#pragma once

#include <memory>

#include "Resource/ConstantBufferResource/ConstantBufferResource.h"
#include "Data/Render2DData/Render2DData.h"
#include "Data/ModelData/ModelData.h"

#include "DataForGPU/MaterialDataForGPU/MaterialDataForGPU.h"
#include "DataForGPU/TransformationDataForGPU/TransformationDataForGPU.h"

#include "Resource/VertexBufferResource/VertexBufferResource.h"
#include "DataForGPU/VertexDataForGPU/VertexDataForGPU.h"

#include "Data/DebugData/DebugData.h"

class GrowthEngine;
class WorldTransform2D;

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
		Render2DBaseData(Render2DHandle hRender2D, std::string name, Render2DParameter* parameter);

		/// @brief リセット
		virtual void Reset() = 0;

		/// @brief シーン前のリセット
		void PerSceneReset();

		/// @brief コマンドリストに登録
		/// @param commandList 
		virtual void Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso) = 0;

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief ハンドルを取得する
		/// @return 
		Render2DHandle GetHandle()const { return hRender2D_; }

		/// @brief 種類を取得する
		/// @return 
		Render2D::Type GetType() const { return type_; }

		/// @brief パラメータを取得する
		/// @return 
		virtual void* GetParam() = 0;

		/// @brief 親を設定する
		/// @param parent 
		void SetParent(WorldTransform2D* parent) { parent_ = parent; }


	public:

		/// @brief デバッグ用パラメータ
		virtual void DebugParameter() = 0;


	protected:

		/// @brief 親
		WorldTransform2D* parent_ = nullptr;

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

		/// @brief エンジン
		const GrowthEngine* engine_ = nullptr;

		// 名前
		std::string name_{};

		/// @brief グループ名
		std::string group_{};

		/// @brief 種類
		Render2D::Type type_;

		// 読み込んだかどうか
		bool isLoad_ = false;


	protected:

		/// @brief パラメータ
		Render2DParameter* parameter_ = nullptr;
	};
}