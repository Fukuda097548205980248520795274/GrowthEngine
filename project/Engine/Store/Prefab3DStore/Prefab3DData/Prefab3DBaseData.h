#pragma once

#include "Handle/Handle.h"
#include "Data/Prefab3DData/Prefab3DData.h"

#include "DataForGPU/PrefabDataForGPU/PrefabDataForGPU.h"
#include <memory>
#include <list>
#include <string>

#include "Resource/IndexBufferResource/IndexBufferResource.h"
#include "Resource/StructuredBufferResource/StructuredBufferResource.h"

namespace Engine
{
	class BasePSOModel;
	class BasePSOShadowMap;
	class PrefabPrimitiveParameter;
	class SkyboxStore;

	class Prefab3DBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param numInstance 
		/// @param hPrefab 
		Prefab3DBaseData(const std::string& name , uint32_t numInstance , Prefab3DHandle hPrefab3D, PrefabPrimitiveParameter* parameter)
			: name_(name), numInstance_(numInstance),hPrefab3D_(hPrefab3D), parameter_(parameter) {}

		/// @brief 仮想デストラクタ
		virtual ~Prefab3DBaseData() = default;

		/// @brief 更新処理
		virtual void Update() = 0;
		
		/// @brief リセット
		virtual void Reset() = 0;

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief 種類を取得する
		/// @return 
		Prefab3D::Type GetType()const { return type_; }

		/// @brief ハンドルを取得する
		/// @return 
		Prefab3DHandle GetHandle()const { return hPrefab3D_; }

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param pso 
		virtual void Register(SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso) = 0;

		/// @brief シャドウマップを描画する
		/// @param viewProjection 
		/// @param commandList 
		/// @param pso 
		virtual void DrawShadowMap(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso) = 0;

		/// @brief パラメータを取得する
		/// @return 
		virtual void* GetParam() = 0;

		/// @brief インスタンスを作成する
		/// @return 
		virtual void* CreateInstance() = 0;

		/// @brief 全てのインスタンスを削除する
		virtual void DestroyAllInstance() = 0;

		/// @brief リセット
		void InstanceReset() { numUseInstance_ = 0; }

		/// @brief デバッグ用パラメータ
		virtual void DebugParameter() = 0;


	protected:

		// 名前
		std::string name_{};

		/// @brief グループ名
		std::string group_{};

		// 種類
		Prefab3D::Type type_{};

		// ハンドル
		Prefab3DHandle hPrefab3D_ = 0;


		// インスタンス数
		uint32_t numInstance_ = 0;

		// 使用インスタンス数
		uint32_t numUseInstance_ = 0;


	protected:

		/// @brief パラメータ
		PrefabPrimitiveParameter* parameter_ = nullptr;
	};
}