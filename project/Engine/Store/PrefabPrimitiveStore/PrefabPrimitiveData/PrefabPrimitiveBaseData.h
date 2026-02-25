#pragma once
#include "Resource/IndexBufferResource/IndexBufferResource.h"
#include "Handle/Handle.h"
#include "Data/PrefabData/PrefabData.h"
#include <string>
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

namespace Engine
{
	class BasePSOModel;
	class BasePSOShadowMap;
	class PrefabPrimitiveParameter;

	class PrefabPrimitiveBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param numInstance 
		/// @param hPrefab 
		PrefabPrimitiveBaseData(const std::string& name , uint32_t numInstance , PrefabPrimitiveHandle hPrefab, PrefabPrimitiveParameter* parameter)
			: name_(name), numInstance_(numInstance),hPrefab_(hPrefab), parameter_(parameter) {}

		/// @brief 仮想デストラクタ
		virtual ~PrefabPrimitiveBaseData() = default;

		/// @brief 更新処理
		virtual void Update() = 0;

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief 種類を取得する
		/// @return 
		Prefab::Type GetType()const { return type_; }

		/// @brief ハンドルを取得する
		/// @return 
		PrimitiveHandle GetHandle()const { return hPrefab_; }

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param pso 
		virtual void Register(ID3D12GraphicsCommandList* commandList, BasePSOModel* pso) = 0;

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
		void Reset() { numUseInstance_ = 0; }

		/// @brief デバッグ用パラメータ
		virtual void DebugParameter() = 0;


	protected:

		// 名前
		std::string name_{};

		/// @brief グループ名
		std::string group_{};

		// 種類
		Prefab::Type type_{};

		// ハンドル
		PrefabPrimitiveHandle hPrefab_ = 0;


		// インスタンス数
		uint32_t numInstance_ = 0;

		// 使用インスタンス数
		uint32_t numUseInstance_ = 0;


	protected:

		/// @brief パラメータ
		PrefabPrimitiveParameter* parameter_ = nullptr;
	};
}