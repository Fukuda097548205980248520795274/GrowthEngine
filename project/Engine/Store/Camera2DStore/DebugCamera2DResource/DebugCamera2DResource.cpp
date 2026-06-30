#include "DebugCamera2DResource.h"
#include "GrowthEngine.h"
#include <numbers>

/// @brief コンストラクタ
Engine::DebugCamera2DResource::DebugCamera2DResource()
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 2Dカメラの生成
	camera2d_ = std::make_unique<Camera2D>();

	// パラメータを取得する
	Camera2DData::Param* param = camera2d_->GetParam();

	// パラメータの生成と初期化
	parameter_ = std::make_unique<DebugCameraParameter>("Debug");
	parameter_->SetValue<Vector2>("Camera2D", "Translate", &param->transform.translate);
	parameter_->SetValue<Vector2>("Camera2D", "Scale", &param->transform.scale);

	// 記録ファイルがあったら反映させる
	parameter_->RegisterGroupDataReflection("Camera2D");
}

/// @brief コンストラクタ
Engine::DebugCamera2DResource::~DebugCamera2DResource()
{
	// 記録を保存する
	parameter_->SaveFile("Camera2D");
}

/// @brief 更新処理
void Engine::DebugCamera2DResource::Update(bool isHoverViewWindow)
{
	// F2キーで有効
	if (engine_->GetKeyTrigger(DIK_F2))
		enable_ = !enable_;

	// 有効でないと処理しない
	if (!enable_)return;

	// マウスがビューウィンドウ上にない場合は処理しない
	if (!isHoverViewWindow)return;

	// パラメータを取得する
	Camera2DData::Param* param = camera2d_->GetParam();

	// マウスホイールを上回転させると、向いている方向にズームイン
	if (engine_->GetMouseWheelUp())
	{
		param->transform.scale /= 1.2f;

		// 1以下にならないようにする
		if (param->transform.scale.x <= 0.05f || param->transform.scale.y <= 0.05f)
		{
			param->transform.scale = Vector2(0.05f, 0.05f);
		}
	}

	// マウスホイールを下回転させると、向いている方向にズームアウト
	if (engine_->GetMouseWheelDown())
	{
		param->transform.scale *= 1.2f;
	}

	// LShift + マウスホイールを押して動かすと、カメラを動かす
	if (engine_->GetKeyPress(DIK_LSHIFT) && engine_->GetMouseButtonPress(MouseButton::Middle))
	{
		// マウスのベクトルと距離を取得し、正規化する
		Vector2 mouseVector = engine_->GetMouseVelocity();
		float mouseVectorLength = mouseVector.Length();
		mouseVector = mouseVector.Normalize();
		mouseVector.x *= -1.0f;

		param->transform.translate += mouseVector * 5.0f;
	}

	// カメラの更新
	camera2d_->Update();
}