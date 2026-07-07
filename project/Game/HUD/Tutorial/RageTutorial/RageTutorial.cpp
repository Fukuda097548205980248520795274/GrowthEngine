#include "RageTutorial.h"
#include "Entity/Character/Player/Player.h"
#include "HUD/Button/TriggerButton/TriggerButton.h"

/// @brief デストラクタ
RageTutorial::~RageTutorial()
{

}

/// @brief 初期化処理
/// @param initData 
void RageTutorial::Initialize(const InitData& initData)
{
	// 練習時間
	practiceTime_ = initData.practiceTime;
	practiceTimer_ = practiceTime_;

	// プレイヤー
	if (initData.player)player_ = initData.player;

	// ボタンHUD
	if (initData.buttonHud)
	{
		buttonHud_ = initData.buttonHud;
		buttonHud_->FadeIn();
	}

	// ワールドトランスフォームの更新
	worldTransform_->Update();
}

/// @brief 更新処理
void RageTutorial::Update()
{
	// 更新が無効な場合は処理しない
	if (!updateEnabled_)return;

	// プレイヤーが存在しない場合は終了する
	if (!player_)
	{
		Delete();
		if (buttonHud_)buttonHud_->FadeOut();
		return;
	}

	// ボタンHUDの位置をプレイヤーの頭の上に設定
	if (buttonHud_)
	{
		buttonHud_->SetPosition(player_->GetBonePosition(JointType::Head) + Vector3(0.0f, 0.5f, 0.0f));

		// ボタンHUDの入力を更新
		if (player_->IsInputRageMode() && state_ != State::Clear)
			buttonHud_->Input();
	}

	if (state_ == State::Practice)
	{
		// チュートリアル中はプレイヤーのレイジゲージを最大にする
		player_->SetRageGage(player_->GetMaxRageGage());

		// クリア条件を満たしているかどうかをチェック
		if (player_->IsInputRageMode())
		{
			state_ = State::Clear;
			if (buttonHud_)buttonHud_->FadeOut();
		}
	}

	// 基底クラスの更新
	Tutorial::Update();
}

/// @brief 描画処理
void RageTutorial::Draw()
{
	if (buttonHud_)buttonHud_->Draw();
}