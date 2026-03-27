#pragma once
#include <memory>
#include <string>
#include "Handle/Handle.h"
#include "Data/Prefab2DData/Prefab2DData.h"
#include "Data/ModelData/ModelData.h"
#include "DataForGPU/PrefabDataForGPU/PrefabDataForGPU.h"
#include "Resource/StructuredBufferResource/StructuredBufferResource.h"
#include "Resource/VertexBufferResource/VertexBufferResource.h"

namespace Engine
{
	class Log;
	class BasePSOModel;
	class TextureStore;
	class IndexBufferResource;
	class DX12Heap;
	class Camera2DStore;
	class Prefab2DParameter;

	class Prefab2DBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param hPrefab2D 
		/// @param numInstance 
		/// @param name 
		/// @param parameter 
		Prefab2DBaseData(Prefab2DHandle hPrefab2D, uint32_t numInstance, const std::string& name, Prefab2DParameter* parameter);

		/// @brief 仮想デストラクタ
		virtual ~Prefab2DBaseData() = default;

		/// @brief 更新処理
		virtual void Update() = 0;

		/// @brief リセット
		virtual void Reset() = 0;

		/// @brief シーン前のリセット
		void PerSceneReset();

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief 種類を取得する
		/// @return 
		Prefab2D::Type GetType()const { return type_; }

		/// @brief ハンドルを取得する
		/// @return 
		Prefab2DHandle GetHandle()const { return hPrefab2D_; }

		/// @brief リセット
		void InstanceReset() { useInstance_ = 0; }

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param pso 
		virtual void Register(ID3D12GraphicsCommandList* commandList, BasePSOModel* pso) = 0;

		/// @brief パラメータを取得する
		/// @return 
		virtual void* GetParam() = 0;

		/// @brief インスタンスを作成する
		/// @return 
		virtual void* CreateInstance() = 0;


	public:

		/// @brief デバッグ用パラメータ
		virtual void DebugParameter() = 0;


	protected:

		// 名前
		std::string name_{};

		/// @brief グループ名
		std::string group_{};

		// 種類
		Prefab2D::Type type_;

		// ハンドル
		Prefab2DHandle hPrefab2D_ = 0;

		// インスタンス数
		uint32_t numInstance_ = 0;

		// 使用インスタンス数
		uint32_t useInstance_ = 0;

		// 読み込まれたかどうか
		bool isLoad_ = false;


	protected:

		/// @brief パラメータ
		Prefab2DParameter* parameter_ = nullptr;


	protected:

		/// @brief 全てのインスタンスを削除する
		virtual void DestroyAllInstance() = 0;
	};
}