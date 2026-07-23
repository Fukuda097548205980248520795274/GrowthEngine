#pragma once
#include <string>
#include <memory>
#include <vector>
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"
#include "Handle/Handle.h"
#include "Data/Render3DData/Render3DData.h"
#include "Data/DebugData/DebugData.h"

#include "Data/CollisionData/CollisionData.h"

#include "PSO/PSOShadowMap/BasePSOShadowMap.h"
#include "PSO/PSOModel/BasePSOModel.h"
#include "PSO/ComputePSO/BaseComputePSO.h"
#include "PSO/PSOMotionVector/BasePSOMotionVector.h"
#include "PSO/PSOOutline/BasePSOOutline.h"

#include "Resource/ConstantBufferResource/ConstantBufferResource.h"
#include "Resource/RWStructuredVertexBufferResource/RWStructuredVertexBufferResource.h"
#include "Resource/StructuredBufferResource/StructuredBufferResource.h"

#include "DataForGPU/TransformationDataForGPU/TransformationDataForGPU.h"
#include "DataForGPU/MaterialDataForGPU/MaterialDataForGPU.h"
#include "DataForGPU/PrimitiveDataForGPU/PrimitiveDataForGPU.h"
#include "DataForGPU/VertexDataForGPU/VertexDataForGPU.h"
#include "DataForGPU/MotionVectorDataForGPU/MotionVectorDataForGPU.h"

class GrowthEngine;
class WorldTransform3D;

namespace Engine
{
	class Render3DParameter;
	class Camera3DStore;
	class SkyboxStore;

	class Render3DBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hRender3D 
		/// @param parameter 
		Render3DBaseData(const std::string& name, Render3DHandle hRender3D, Render3DParameter* parameter);

		/// @brief 仮想デストラクタ
		virtual ~Render3DBaseData() = default;

		/// @brief 更新処理
		virtual void Update() = 0;

		/// @brief リセット
		virtual void Reset() = 0;

		/// @brief シーン前のリセット
		void PerSceneReset();

		/// @brief 親を設定する
		/// @param parent 
		void SetParent(WorldTransform3D* parent) { parent_ = parent; }

		/// @brief コマンドリストに登録
		/// @param viewProjection 
		/// @param commandList 
		/// @param pso 
		/// @param lightStore 
		virtual void Register(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso) = 0;

		/// @brief コマンドリストに登録
		/// @param viewProjection 
		/// @param commandList 
		/// @param pso 
		virtual void Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso) = 0;

		/// @brief コマンドリストに登録
		/// @param commandList 
		/// @param pso 
		virtual void RegisterMotionVector(ID3D12GraphicsCommandList* commandList, BasePSOMotionVector* pso) = 0;

		/// @brief アウトライン用のコマンドリストに登録
		/// @param commandList 
		/// @param cameraStore 
		/// @param pso 
		virtual void RegisterOutline(ID3D12GraphicsCommandList* commandList, BasePSOOutline* pso) = 0;

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief 種類を取得する
		/// @return 
		Render3D::Type GetType()const { return type_; }

		/// @brief ハンドルを取得する
		/// @return 
		Render3DHandle GetHandle()const { return hRender3D_; }

		/// @brief 描画したかどうか
		/// @return 
		bool IsDrew()const { return isDrew_ || isPreDrew_; }

		/// @brief パラメータを取得する
		/// @return 
		virtual void* GetParam() = 0;

		/// @brief ボーンのワールド行列を取得する
		/// @param name 
		/// @return 
		virtual Matrix4x4 GetBoneWorldMatrix(const std::string& name) { return MakeIdentityMatrix4x4(); }


	public:

		/// @brief デバッグ用パラメータ
		virtual void DebugParameter() = 0;


	protected:

		/// @brief エンジン
		const GrowthEngine* engine_ = nullptr;

		/// @brief パラメータ
		Render3DParameter* parameter_ = nullptr;


	protected:

		// 種別名
		Render3D::Type type_;
		
		// 名前
		std::string name_{};

		/// @brief グループ名
		std::string group_{};

		/// @brief 読み込んだかどうか
		bool isLoad_ = false;


	protected:

		// 描画したかどうか
		bool isDrew_ = false;

		// 描画前に処理したかどうか
		bool isPreDrew_ = false;


		// 親ワールドトランスフォーム
		WorldTransform3D* parent_ = nullptr;

	private:

		// ハンドル
		Render3DHandle hRender3D_ = 0;
	};
}