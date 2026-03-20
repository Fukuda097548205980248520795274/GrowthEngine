#pragma once
#include <string>
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"
#include "Handle/Handle.h"
#include "Data/Render3DData/Render3DData.h"

#include "Data/CollisionData/CollisionData.h"

namespace Engine
{
	class Render3DParameter;

	class Render3DBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		Render3DBaseData(const std::string& name , Render3DHandle hRender3D, Render3DParameter* parameter)
			: name_(name) , hRender3D_(hRender3D), parameter_(parameter){}

		/// @brief 仮想デストラクタ
		virtual ~Render3DBaseData() = default;

		/// @brief 更新処理
		virtual void Update() = 0;

		/// @brief リセット
		virtual void Reset() = 0;

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief 種類を取得する
		/// @return 
		Render3D::Type GetType()const { return type_; }

		/// @brief ハンドルを取得する
		/// @return 
		Render3DHandle GetHandle()const { return hRender3D_; }

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
		Render3D::Type type_;
		
		// 名前
		std::string name_{};

		/// @brief グループ名
		std::string group_{};

		/// @brief パラメータ
		Render3DParameter* parameter_ = nullptr;

	private:

		// ハンドル
		Render3DHandle hRender3D_ = 0;


	protected:

		// ギズモの選択フラグ
		bool isGuizmoSelect_ = false;
	};
}