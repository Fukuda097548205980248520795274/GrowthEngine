#include "../GameScene.h"

/// @brief ポーズフェーズの初期化処理
void GameScene::PosePhaseInitialize()
{

}

/// @brief ポーズフェーズの更新処理
void GameScene::PosePhaseUpdate()
{
	// ポーズ画面の切り替え
	if (engine_->GetKeyTrigger(DIK_ESCAPE) ||
		engine_->GetGamepadButtonTrigger(0, XINPUT_GAMEPAD_START))
	{
		phaseManager_->ChangePhase(PhaseType::Battle);
	}
}