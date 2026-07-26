#include "TitleScene.h"

/// @brief 初期化
void TitleScene::Initialize()
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// エディタを作成する
	lightEditor_ = std::make_unique<LightEditor>();
	modelEditor_ = std::make_unique<ModelEditor>();
	
	// 上下のキー入力を作成する
	wKey_ = std::make_unique<InputKey>("TitleWKey", InputState::Trigger, DIK_W);
	sKey_ = std::make_unique<InputKey>("TitleSKey", InputState::Trigger, DIK_S);

	// 決定キー入力を作成する
	spaceKey_ = std::make_unique<InputKey>("TitleSpaceKey", InputState::Trigger, DIK_SPACE);

	// 上下の左スティック入力を作成する
	upLeftStick_ = std::make_unique<InputGamepadLeftStick>("TitleUpLeftStick", InputState::Trigger, 0, Vector2(0.0f, -1.0f), 0.5f);
	downLeftStick_ = std::make_unique<InputGamepadLeftStick>("TitleDownLeftStick", InputState::Trigger, 0, Vector2(0.0f, 1.0f), 0.5f);

	// Aボタン入力を作成する
	aButton_ = std::make_unique<InputGamepadButton>("TitleAButton", InputState::Trigger, 0, XINPUT_GAMEPAD_A);

	// フェード用スプライトを作成する
	fadeSprite_ = std::make_unique<Sprite>(engine_->LoadTexture("./Assets/Textures/white2x2.png"), "Fade");
	fadeSprite_->param_->texture.anchor = Vector2(0.0f, 1.0f);
	fadeSprite_->param_->screenAnchor = Engine::Render2D::ScreenAnchor::LeftBottom;
	fadeSprite_->param_->transform.scale = Vector2(static_cast<float>(engine_->GetScreenWidth()), static_cast<float>(engine_->GetScreenHeight()));
	fadeSprite_->param_->material.color = Vector4(0.0f, 0.0f, 0.0f, 1.0f);

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

			// UIエディタのUI描画処理を呼び出す
			modelEditor_->DrawUI();
			lightEditor_->DrawUI();

			// UIの描画処理を呼び出す
			modelEditor_->Draw();
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

			// フェード用スプライトの描画
			fadeSprite_->Draw();
		}
	);
}

/// @brief 更新処理
void TitleScene::Update()
{
	float dt = engine_->GetDeltaTime();

	// UIエディタの更新処理を呼び出す
	modelEditor_->Update(dt);
	lightEditor_->Update(dt);

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