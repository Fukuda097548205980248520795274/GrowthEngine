#pragma once
#include "../BaseLightData.h"

namespace Engine
{
	class SpotLightData : public BaseLightData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param handle 
		SpotLightData(const std::string& name, LightHandle handle);

		/// @brief 初期化
		/// @param parameter 
		void Initialize(LightParameter* parameter) override;

		/// @brief 更新処理
		void Update() override;

		/// @brief リセット
		void Reset()override;

		/// @brief 
		/// @return 
		void* GetParam() override { return param_.get(); }

		/// @brief 輝度を取得する
		/// @return 
		float GetIntensity() override { return param_->intensity; }

		/// @brief 種別名を取得する
		/// @return 
		Light::Type GetType() const override { return Light::Type::Spot; }


	public:

		/// @brief デバッグ用描画処理
		void DebugParameter() override;

		/// @brief デバッグ用の線を描画する
		void DebugDrawLine() override;

		/// @brief デバッグ用レイピッキング
		/// @param ray 
		/// @param pickList 
		void DebugRayPicking(const Collision3D::Ray& ray, std::vector<std::pair<float, DebugData::DebugGuizmoData*>>& pickList) override;

		/// @brief デバッグ用Guizmo操作
		/// @param viewMatrix 
		/// @param projMatrix 
		void DebugGuizmo(const Matrix4x4& viewMatrix , const Matrix4x4& projMatrix) override;


	private:

		// パラメータ
		std::unique_ptr<Light::SpotLightParam> param_ = nullptr;
	};
}