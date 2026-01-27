#pragma once
#include "../PrimitiveBaseData.h"
#include "Handle/Handle.h"
#include <memory>
#include "Math/Vector/Vector3/Vector3.h"
#include <vector>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "Resource/PrimitiveModelMaterialResource/PrimitiveModelMaterialResource.h"
#include "Resource/PrimitiveModelTransformationResource/PrimitiveModelTransformationResource.h"

namespace Engine
{
	class ModelStore;
	class BasePSOModel;
	class Log;

	class PrimitiveStaticModelData : public PrimitiveBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hModel 
		PrimitiveStaticModelData(const std::string& name , ModelHandle hModel, PrimitiveStaticModelHandle hPrimitiveStaticModel) 
			: hModel_(hModel), hPrimitiveStaticModel_(hPrimitiveStaticModel), PrimitiveBaseData(name) { typeName_ = "StaticModel"; }

		/// @brief 初期化
		/// @param modelStore 
		/// @param device 
		void Initialize(ModelStore* modelStore, ID3D12Device* device, Log* log);

		/// @brief 更新処理
		/// @param viewProjection 
		void Update(const Matrix4x4& viewProjection) override;

		/// @brief ハンドルを取得する
		/// @return 
		PrimitiveStaticModelHandle GetHandle()const { return hPrimitiveStaticModel_; }

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param pso 
		void Register(ID3D12GraphicsCommandList* commandList, BasePSOModel* pso);


	private:

		// モデルハンドル
		ModelHandle hModel_ = 0;

		// 静的モデルハンドル
		PrimitiveStaticModelHandle hPrimitiveStaticModel_ = 0;


	private:

		// モデル用トランスフォーム
		struct Transform
		{
			std::unique_ptr<Vector3> scale = nullptr;
			std::unique_ptr<Vector3> rotation = nullptr;
			std::unique_ptr<Vector3> translate = nullptr;
		};

		// モデルトランスフォーム
		Transform modelTransform_{};

		// メッシュトランスフォーム
		std::vector<Transform> meshTransforms_;

		// 座標変換リソース
		std::vector<std::unique_ptr<PrimitiveModelTransformationResource>> meshTransformationResources_;


	private:

		// メッシュ用マテリアル
		struct MeshMaterial
		{
			std::unique_ptr<Vector4> color = nullptr;
		};

		// メッシュマテリアル
		std::vector<MeshMaterial> meshMaterials_;

		// マテリアルリソース
		std::vector<std::unique_ptr<PrimitiveModelMaterialResource>> meshMaterialResources_;


	private:

		/// @brief モデルストア
		ModelStore* modelStore_ = nullptr;
	};
}