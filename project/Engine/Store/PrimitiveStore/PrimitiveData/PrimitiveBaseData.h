#pragma once
#include <string>
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"
#include "Handle/Handle.h"
#include "Data/PrimitiveData/PrimitiveData.h"

#include "Data/CollisionData/CollisionData.h"

namespace Engine
{
	class PrimitiveParameter;

	class PrimitiveBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		PrimitiveBaseData(const std::string& name , PrimitiveHandle hPrimitive, PrimitiveParameter* parameter)
			: name_(name) , hPrimitive_(hPrimitive), parameter_(parameter){}

		/// @brief 仮想デストラクタ
		virtual ~PrimitiveBaseData() = default;

		/// @brief 更新処理
		virtual void Update() = 0;

		/// @brief リセット
		virtual void Reset() = 0;

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief 種類を取得する
		/// @return 
		Primitive::Type GetType()const { return type_; }

		/// @brief ハンドルを取得する
		/// @return 
		PrimitiveHandle GetHandle()const { return hPrimitive_; }

		/// @brief パラメータを取得する
		/// @return 
		virtual void* GetParam() = 0;

		/// @brief デバッグ用パラメータ
		virtual void DebugParameter() = 0;

		/// @brief デバッグ用レイピッキング
		/// @param ray 
		/// @param pickList 
		virtual void DebugRayPicker(const Collision3D::Ray& ray, std::vector<std::pair<float, bool*>>& pickList) = 0;


	protected:

		// 種別名
		Primitive::Type type_;
		
		// 名前
		std::string name_{};

		/// @brief グループ名
		std::string group_{};

		/// @brief パラメータ
		PrimitiveParameter* parameter_ = nullptr;

	private:

		// ハンドル
		PrimitiveHandle hPrimitive_ = 0;


	protected:

		// ギズモの選択フラグ
		bool isGuizmoSelect_ = false;
	};
}