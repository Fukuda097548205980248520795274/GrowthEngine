#include "Scene/GameScene/GameScene.h"
#include <algorithm>

namespace
{
	// ピボット中心の追従補間速度
	constexpr float kPivotFollowSpeed = 10.0f;

	// ピボット回転速度[rad/s]
	constexpr float kPivotRotateSpeed = 1.5f;

	// ピボットX軸回転の最大角度
	constexpr float kPivotMaxPitch = 70.0f * (std::numbers::pi_v<float> / 180.0f);

	// ピボット中心の高さオフセット
	const Vector3 kPivotCenterOffset = Vector3(0.0f, 1.5f, 0.0f);
}

/// @brief カメラ制御の初期化
void GameScene::InitializeCameraControl()
{
	// プレイヤーがいない場合はカメラ制御を初期化しない
	if (!player_)return;

	// すでにピボットポイントが存在する場合は削除する
	if (pivotPoint_)
	{
		pivotPoint_.reset();
		pivotPoint_ = nullptr;
	}

	// カメラ用のピボットポイントを生成する
	pivotPoint_ = std::make_unique<PivotPoint>();

	// ピボットポイントの初期化
	Vector3 targetPivotPos = player_->GetWorldPosition();
	targetPivotPos += kPivotCenterOffset;

	pivotPoint_->GetData()->center = targetPivotPos;
	pivotPoint_->GetData()->radius = 8.0f;
	pivotPoint_->GetData()->phi = -std::numbers::pi_v<float> / 2.0f;
	pivotPoint_->GetData()->theta = 0.0f;

	// カメラ回転入力の生成
	inputCameraRotate_ = std::make_unique<InputGamepadRightStick>("Camera_Rotate", InputState::Press, 0, Vector2(0.0f, 0.0f), 0.5f);

	// 補間係数を初期化する
	cameraCurrentT_ = 1.0f;

	pivotPoint_->Update();

	PivotPoint::Data* pivotData = pivotPoint_->GetData();

	// カメラ座標の即時適用
	mainCamera_->param_->transform.translate = pivotData->sphericalCoordinates;

	// カメラ回転（オイラー角）の即時適用
	const Vector3 kLookDirection = pivotData->toCenter;
	const float kYaw = std::atan2(kLookDirection.x, kLookDirection.z);
	const float kHorizontal = std::sqrt(kLookDirection.x * kLookDirection.x + kLookDirection.z * kLookDirection.z);
	const float kPitch = std::atan2(-kLookDirection.y, kHorizontal);
	mainCamera_->param_->transform.rotate = Vector3(kPitch, kYaw, 0.0f);

	// 次フレームの当たり判定（カメラセグメント）用に初期値をセット
	cameraSegmentInstance_->param_->start = pivotData->center;
	cameraSegmentInstance_->param_->diff = pivotData->sphericalCoordinates - pivotData->center;

	// バトル制御用（カメラの前方向）の即時適用
	Vector3 cameraForward = kLookDirection;
	cameraForward.y = 0.0f;
	if (cameraForward.LengthSq() > 0.0f)
		cameraForward = cameraForward.Normalize();

	// バトル制御用（カメラの前方向）の即時適用
	BattleDirector::GetInstance().SetCameraForward(cameraForward);
}

/// @brief カメラ制御の更新
/// @param deltaTime
void GameScene::UpdateCameraControl(float deltaTime)
{
	// プレイヤーがいない場合はカメラ制御を更新しない
	if (!player_)return;

	// ピボットポイントがない場合は更新しない
	if (!pivotPoint_)
		return;

	if (deltaTime > 0.1f) {
		deltaTime = 0.1f;
	}

	// ピボット中心の追従更新
	UpdatePivotFollow(deltaTime);

	// ピボット回転入力の更新
	UpdatePivotRotateInput(deltaTime);

	// ピボットの球面座標と注視方向を更新する
	pivotPoint_->Update();

	// ピボットからカメラ姿勢を更新する
	ApplyCameraFromPivot(deltaTime);
}

/// @brief ピボット中心をプレイヤーへ追従させる
/// @param deltaTime
void GameScene::UpdatePivotFollow(float deltaTime)
{
	// プレイヤーがいない場合は更新しない
	if (!player_)return;

	// ピボットのデータを取得する
	PivotPoint::Data* pivotData = pivotPoint_->GetData();

	// ターゲットの位置をプレイヤーの位置に設定する
	Vector3 targetPivotPos = player_->GetPosition();

	// プレイヤーの位置からピボット中心までの高さオフセットを加算する
	targetPivotPos.y += 1.5f;

	// ロックオン中はターゲットの位置にピボットをオフセットする
	if (player_->IsStance() && player_->GetLockOnTarget() != nullptr)
	{
		// ロックオン中はターゲットの位置にピボットをオフセットする
		Vector3 targetPos = player_->GetLockOnTarget()->GetPosition();
		Vector3 playerPos = player_->GetPosition();

		// ターゲットの方向ベクトルを計算する
		Vector3 dir = targetPos - playerPos;
		dir.y = 0.0f;
		dir = dir.Normalize();

		// ターゲットの左右方向ベクトルを計算する
		Vector3 rightDir = Vector3(dir.z, 0.0f, -dir.x);

		// ターゲットの左右どちらかにピボットをオフセットする量
		constexpr float kRightOffset = 0.0f;

		// ターゲットの左右どちらかにピボットをオフセットする
		targetPivotPos += rightDir * kRightOffset;
	}

	// ピボット中心をターゲット位置に向かって補間移動させる
	Vector3 diff = targetPivotPos - pivotData->center;
	pivotData->center += diff * kPivotFollowSpeed * deltaTime;
}

/// @brief ピボット回転入力を反映する
/// @param deltaTime
void GameScene::UpdatePivotRotateInput(float deltaTime)
{
	// プレイヤーがいない場合は更新しない
	if (!player_)return;

	// ピボットのデータを取得する
	PivotPoint::Data* pivotData = pivotPoint_->GetData();

	// キー入力でカメラ回転しているかを判定する
	const bool kIsKeyCameraRotate =
		engine_->GetKeyPress(DIK_LEFT) || engine_->GetKeyPress(DIK_RIGHT) ||
		engine_->GetKeyPress(DIK_DOWN) || engine_->GetKeyPress(DIK_UP);

	// ゲームパッドの右スティック入力を取得する
	Vector2 rightStick(0.0f, 0.0f);
	if (inputCameraRotate_ && inputCameraRotate_->param_)
	{
		rightStick = engine_->GetGamepadRightStick(inputCameraRotate_->param_->controller);
	}

	// キー入力または右スティック入力がある場合は手動でカメラ回転しているとみなす
	bool isManualCameraControl = kIsKeyCameraRotate || (rightStick.Length() > 0.01f);

	// 手動でカメラ回転入力がある場合はピボットを回転させる
	if (isManualCameraControl)
	{
		// プレイヤーに手動でカメラ回転していることを通知する
		player_->SetIsOperationCamera(true);

		// 手動でカメラ回転入力がある場合はピボットを回転させる
		if (!kIsKeyCameraRotate)
		{
			pivotData->phi += -rightStick.x * kPivotRotateSpeed * deltaTime;
			pivotData->theta += -rightStick.y * kPivotRotateSpeed * deltaTime;
		}
	}
	else if (player_->GetLockOnTarget() != nullptr)
	{
		// ロックオン中はターゲットの方向にピボットを回転させる
		Character* target = player_->GetLockOnTarget();
		Vector3 targetPos = target->GetPosition();
		targetPos.y += 1.0f;

		// ターゲットの方向ベクトルを計算する
		Vector3 dir = targetPos - pivotData->center;
		if (target->IsBlownAway() || target->IsBlownFalling())dir.y = 0.0f;
		dir.Normalize();

		// ターゲットの方向からピボットの回転角度を計算する
		float targetPhi = std::atan2(-dir.x, dir.z) - (std::numbers::pi_v<float> / 2.0f);
		float targetTheta = std::atan2(-dir.y, std::sqrt(dir.x * dir.x + dir.z * dir.z));

		// ターゲットの高さに合わせてピボットのX軸回転を制限する
		constexpr float kLockOnMaxPitch = 20.0f * (std::numbers::pi_v<float> / 180.0f);
		targetTheta = std::clamp(targetTheta, -kLockOnMaxPitch, kLockOnMaxPitch);

		// ピボットのY軸回転とターゲットの方向の差を計算する
		float diffPhi = targetPhi - pivotData->phi;

		// 角度の差を-π～πの範囲に収める
		while (diffPhi > std::numbers::pi_v<float>)  diffPhi -= 2.0f * std::numbers::pi_v<float>;
		while (diffPhi < -std::numbers::pi_v<float>) diffPhi += 2.0f * std::numbers::pi_v<float>;

		// ピボットのY軸回転はターゲットの方向に合わせて補間する
		constexpr float kLockOnCameraFollowSpeed = 3.0f;
		pivotData->phi += diffPhi * kLockOnCameraFollowSpeed * deltaTime;

		// ピボットのX軸回転はターゲットの高さに合わせて補間する
		float diffTheta = targetTheta - pivotData->theta;
		pivotData->theta += diffTheta * kLockOnCameraFollowSpeed * deltaTime;
	}

	// ピボットのX軸回転は最大角度で制限する
	pivotData->theta = std::clamp(pivotData->theta, -kPivotMaxPitch, kPivotMaxPitch);
}

/// @brief ピボットからカメラ姿勢へ反映する
void GameScene::ApplyCameraFromPivot(float deltaTime)
{
	// プレイヤーがいない場合は更新しない
	if (!player_)return;

	// ピボットのデータを取得する
	PivotPoint::Data* pivotData = pivotPoint_->GetData();

	// カメラの位置をピボットの球面座標から計算する
	Vector3 finalCameraPos = pivotData->sphericalCoordinates;

	// カメラセグメントのパラメータを更新する
	cameraSegmentInstance_->param_->start = pivotData->center;
	cameraSegmentInstance_->param_->diff = finalCameraPos - pivotData->center;

	// 係数Tの初期値を1.0fに設定する 
	float targetT = 1.0f;

	// カメラセグメントの衝突判定が有効な場合は、カメラの位置を調整する
	if (cameraSegmentInstance_->isCollision_)
	{
		// 複数の壁（OBB）と衝突している可能性があるため、最も近い交点を探す
		for (auto* opponent : cameraSegmentInstance_->hitOpponents_)
		{
			auto* obb = dynamic_cast<Collision3DInstanceOBB*>(opponent);
			float t = Engine::IntersectSegmentOBB(*cameraSegmentInstance_->param_, *obb->param_);

			// 最もピボット中心に近い交点を採用する
			if (t >= 0.0f && t < targetT)
			{
				targetT = t;
			}
		}
	}

	// カメラの位置を補間する速度を設定する
	constexpr float kCameraShrinkSpeed = 25.0f; // 壁に近づくときの速度
	constexpr float kCameraExpandSpeed = 5.0f;  // 元の位置に戻るときの速度

	// 現在よりターゲットが近い（縮む）か、遠い（戻る）かで速度を分岐
	float lerpSpeed = (targetT < cameraCurrentT_) ? kCameraShrinkSpeed : kCameraExpandSpeed;

	// 線形補間（Lerp）で現在のTを目標のTへ近づける
	cameraCurrentT_ += (targetT - cameraCurrentT_) * lerpSpeed * deltaTime;
	cameraCurrentT_ = std::clamp(cameraCurrentT_, 0.0f, 1.0f);

	// 補間された割合を使って、最終的なカメラ位置を計算
	Vector3 hitPosition = pivotData->center + (cameraSegmentInstance_->param_->diff * cameraCurrentT_);

	// 壁に近づいている（Tが1.0未満）場合はニアクリップ対策のオフセットを適用
	if (cameraCurrentT_ < 0.99f)
	{
		constexpr float kCameraOffset = 0.2f;
		Vector3 dir = cameraSegmentInstance_->param_->diff;
		dir = dir.Normalize();
		finalCameraPos = hitPosition - (dir * kCameraOffset);
	}
	else
	{
		finalCameraPos = hitPosition;
	}

	// カメラシェイクのオフセットを加算する
	if (cameraShake_) finalCameraPos += cameraShake_->GetShakeOffset();

	// カメラの位置を更新する
	mainCamera_->param_->transform.translate = finalCameraPos;

	// center方向を向くようにオイラー角を計算する
	const Vector3 kLookDirection = pivotData->toCenter;
	const float kYaw = std::atan2(kLookDirection.x, kLookDirection.z);
	const float kHorizontal = std::sqrt(kLookDirection.x * kLookDirection.x + kLookDirection.z * kLookDirection.z);
	const float kPitch = std::atan2(-kLookDirection.y, kHorizontal);
	mainCamera_->param_->transform.rotate = Vector3(kPitch, kYaw, 0.0f);

	// カメラの前方向ベクトルを計算する
	Vector3 cameraForward = kLookDirection;
	cameraForward.y = 0.0f; // 高さの影響をなくすためにYを0にする
	if (cameraForward.LengthSq() > 0.0f)
		cameraForward = cameraForward.Normalize();

	// バトル制御にカメラの前方向を通知する
	BattleDirector::GetInstance().SetCameraForward(cameraForward);
}