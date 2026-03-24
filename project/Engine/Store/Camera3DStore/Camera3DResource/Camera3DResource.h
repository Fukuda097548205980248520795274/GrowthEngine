#pragma once
#include <memory>
#include <string>
#include "Camera/Camera3D/Camera3D.h"
#include "Handle/Handle.h"
#include "Data/DebugData/DebugData.h"
#include "Data/CollisionData/CollisionData.h"

#include <vector>

class GrowthEngine;

namespace Engine
{
	class Camera3DParameter;

	class Camera3DResource
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hCamera 
		Camera3DResource(const std::string& name, Camera3DHandle hCamera, Camera3DParameter* parameter);

		/// @brief 更新処理
		void Update();

		/// @brief リセット
		void Reset();

		/// @brief シーン前のリセット
		void PerSceneReset();

		/// @brief カメラデータを取得する
		/// @return 
		const Camera3D& GetCamera3D() { return *camera3d_; }
		
		/// @brief ハンドルを取得する
		/// @return 
		Camera3DHandle GetHandle() { return hCamera_; }

		/// @brief 名前を取得する
		/// @return 
		std::string GetName() { return name_; }


	public:

		/// @brief デバッグ用の線を描画する
		/// @param color 
		void DebugDrawLine(const Vector4& color);

		/// @brief デバッグ用パラメータ
		void DebugParameter();

		/// @brief デバッグ用レイピッキング
		/// @param ray 
		/// @param pickList 
		void DebugRayPicking(const Collision3D::Ray& ray, std::vector<std::pair<float, bool*>>& pickList);

		/// @brief Guizmo操作
		/// @param viewMatrix 
		/// @param projMatrix 
		void DebugGuizmo(const Matrix4x4& viewMatrix, const Matrix4x4& projMatrix);


	private:

		// エンジン
		const GrowthEngine* engine_ = nullptr;

		// カメラ3D
		std::unique_ptr<Camera3D> camera3d_ = nullptr;

		// パラメータ
		Camera3DData::Param* param_ = nullptr;

		// カメラハンドル
		Camera3DHandle hCamera_ = 0;

		// 名前
		std::string name_{};

		// 読み込まれたかどうか
		bool isLoad_ = false;


		// パラメータ
		Camera3DParameter* parameter_ = nullptr;


	private:

		/// @brief デバッグデータ : Guizmo
		DebugData::DebugGuizmoData guizmoData_{};
	};
}