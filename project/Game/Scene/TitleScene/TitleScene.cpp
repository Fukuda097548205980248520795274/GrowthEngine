#include "TitleScene.h"

/// @brief 初期化
void TitleScene::Initialize()
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 上下のキー入力を作成する
	wKey_ = std::make_unique<InputKey>("TitleWKey", InputState::Trigger, DIK_W);
	sKey_ = std::make_unique<InputKey>("TitleSKey", InputState::Trigger, DIK_S);

	// 決定キー入力を作成する
	spaceKey_ = std::make_unique<InputKey>("TitleSpaceKey", InputState::Trigger, DIK_SPACE);

	// フェーズマネージャを作成する
	phaseManager_ = std::make_unique<PhaseManager<PhaseType>>();
	phaseManager_->SetOnEnter(PhaseType::Intro, [&]() { IntroInitialize(); });
	phaseManager_->SetOnUpdate(PhaseType::Intro, [&]() { IntroUpdate(); });
	phaseManager_->SetOnEnter(PhaseType::MainMenu, [&]() { MainMenuInitialize(); });
	phaseManager_->SetOnUpdate(PhaseType::MainMenu, [&]() { MainMenuUpdate(); });
	phaseManager_->SetOnEnter(PhaseType::Play, [&]() { PlayInitialize(); });
	phaseManager_->SetOnUpdate(PhaseType::Play, [&]() { PlayUpdate(); });
	phaseManager_->SetOnEnter(PhaseType::Quit, [&]() { QuitInitialize(); });
	phaseManager_->SetOnUpdate(PhaseType::Quit, [&]() { QuitUpdate(); });
	phaseManager_->Initialize(PhaseType::Intro);

	// オブジェクトの描画レンダーパスの読み込み
	engine_->LoadRenderPass("Object", [&]()
		{
			engine_->DrawToRenderPass("Object", "PrevDraw");
		}
	);

	// ポストエフェクトの描画レンダーパスの読み込み
	engine_->LoadRenderPass("PostEffect", [&]()
		{
			engine_->DrawToRenderPass("PostEffect", "Object");
		}
	);

	// HUDの描画レンダーパスの読み込み
	engine_->LoadRenderPass("HUD", [&]()
		{
			engine_->DrawToRenderPass("HUD", "PostEffect");

		}
	);

	engine_->LoadRenderPass("MainPass", [&]()
		{
			engine_->DrawToRenderPass("MainPass", "HUD");
		}
	);
}

/// @brief 更新処理
void TitleScene::Update()
{
	// フェーズマネージャの更新処理を呼び出す
	phaseManager_->Update();
}

/// @brief 描画処理
void TitleScene::Draw()
{
	// オブジェクトの描画レンダーパスを呼び出す
	engine_->ExecuteRenderPass("Object");

	// ポストエフェクトの描画レンダーパスを呼び出す
	engine_->ExecuteRenderPass("PostEffect");

	// HUDの描画レンダーパスを呼び出す
	engine_->ExecuteRenderPass("HUD");

	// メインパスの描画レンダーパスを呼び出す
	engine_->ExecuteRenderPass("MainPass");
}