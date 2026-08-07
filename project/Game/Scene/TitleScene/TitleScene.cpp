#include "TitleScene.h"

/// @brief 初期化
void TitleScene::Initialize()
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// ステージセレクトエディタを作成する
	stageSelectEditor_ = std::make_unique<StageSelectEditor>();
	stageSelectEditor_->Initialize();

	// UIエディタを作成する
	uiEditor_ = std::make_unique<UIEditor>();
	uiEditor_->Load("Title_Scene");

	// タイトルBGMを作成する
	titleBgm_ = std::make_unique<Bgm>("TitleBGM", engine_->LoadAudio("./Assets/Sounds/bgm/title_bgm.mp3"));
	titleBgm_->param_->volume = 0.0f;
	titleBgm_->param_->enableLoop = true;
	titleBgm_->Play();

	// 選択SEと決定SEを作成する
	selectSe_ = std::make_unique<Se>("SelectSE", engine_->LoadAudio("./Assets/Sounds/se/title_select.mp3"));
	executeSe_ = std::make_unique<Se>("ExecuteSE", engine_->LoadAudio("./Assets/Sounds/se/title_execute.mp3"));
	backSe_ = std::make_unique<Se>("BackSE", engine_->LoadAudio("./Assets/Sounds/se/title_back.mp3"));

	// ポストエフェクトのビネットを作成する
	vignetting_ = std::make_unique<PostEffectVignetting>("TitleVignetting");

	// ポストエフェクトのブルームを作成する
	bloom_ = std::make_unique<PostEffectBloom>("TitleBloom");

	// 上下のキー入力を作成する
	wKey_ = std::make_unique<InputKey>("TitleWKey", InputState::Trigger, DIK_W);
	sKey_ = std::make_unique<InputKey>("TitleSKey", InputState::Trigger, DIK_S);

	// 決定キー入力を作成する
	spaceKey_ = std::make_unique<InputKey>("TitleSpaceKey", InputState::Trigger, DIK_SPACE);

	// キャンセルキー入力を作成する
	escapeKey_ = std::make_unique<InputKey>("TitleEscapeKey", InputState::Trigger, DIK_ESCAPE);

	// 上下の左スティック入力を作成する
	upLeftStick_ = std::make_unique<InputGamepadLeftStick>("TitleUpLeftStick", InputState::Trigger, 0, Vector2(0.0f, 1.0f), 0.5f);
	downLeftStick_ = std::make_unique<InputGamepadLeftStick>("TitleDownLeftStick", InputState::Trigger, 0, Vector2(0.0f, -1.0f), 0.5f);

	// Aボタン入力を作成する
	aButton_ = std::make_unique<InputGamepadButton>("TitleAButton", InputState::Trigger, 0, XINPUT_GAMEPAD_A);

	// Bボタン入力を作成する
	bButton_ = std::make_unique<InputGamepadButton>("TitleBButton", InputState::Trigger, 0, XINPUT_GAMEPAD_B);

	// フェード用スプライトを作成する
	fadeSprite_ = std::make_unique<Sprite>(engine_->LoadTexture("./Assets/Textures/white2x2.png"), "Fade");
	fadeSprite_->param_->texture.anchor = Vector2(0.0f, 1.0f);
	fadeSprite_->param_->screenAnchor = Engine::Render2D::ScreenAnchor::LeftBottom;
	fadeSprite_->param_->transform.scale = Vector2(static_cast<float>(engine_->GetScreenWidth()), static_cast<float>(engine_->GetScreenHeight()));
	fadeSprite_->param_->material.color = Vector4(0.0f, 0.0f, 0.0f, 1.0f);

	// フェーズビューのスプライトを取得する
	auto phaseView = uiEditor_->GetSprite("Phase_View");
	phaseView->param_->material.color = Vector4(1.0f, 1.0f, 1.0f, 0.0f);

	// メインメニューのスプライトを取得する
	mainMenuSprite_[static_cast<int>(MainMenuOption::StartGame)] = uiEditor_->GetSprite("mainMenu_GameStart");
	mainMenuSprite_[static_cast<int>(MainMenuOption::QuitGame)] = uiEditor_->GetSprite("mainMenu_Quit");

	// メインメニューの背景スプライトを取得する
	mainMenuSpriteBG_[static_cast<int>(MainMenuOption::StartGame)] = uiEditor_->GetSprite("mainMenuBG_GameStart");
	mainMenuSpriteBG_[static_cast<int>(MainMenuOption::QuitGame)] = uiEditor_->GetSprite("mainMenuBG_Quit");

	// メインメニューのスプライトのアルファ値を初期化する
	mainMenuSprite_[static_cast<int>(MainMenuOption::StartGame)]->param_->material.color.w = 0.0f;
	mainMenuSprite_[static_cast<int>(MainMenuOption::QuitGame)]->param_->material.color.w = 0.0f;
	mainMenuSpriteBG_[static_cast<int>(MainMenuOption::StartGame)]->param_->material.color.w = 0.0f;
	mainMenuSpriteBG_[static_cast<int>(MainMenuOption::QuitGame)]->param_->material.color.w = 0.0f;


	// フェーズマネージャを作成する
	phaseManager_ = std::make_unique<PhaseManager<PhaseType>>();
	phaseManager_->SetOnEnter(PhaseType::Intro, [&]() { IntroInitialize(); });
	phaseManager_->SetOnUpdate(PhaseType::Intro, [&]() { IntroUpdate(); });
	phaseManager_->SetOnEnter(PhaseType::Title, [&]() { TitleInitialize(); });
	phaseManager_->SetOnUpdate(PhaseType::Title, [&]() { TitleUpdate(); });
	phaseManager_->SetOnEnter(PhaseType::MainMenu, [&]() { MainMenuInitialize(); });
	phaseManager_->SetOnUpdate(PhaseType::MainMenu, [&]() { MainMenuUpdate(); });
	phaseManager_->SetOnEnter(PhaseType::StageSelect, [&]() { StageSelectInitialize(); });
	phaseManager_->SetOnUpdate(PhaseType::StageSelect, [&]() { StageSelectUpdate(); });
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

			uiEditor_->DrawUI();
			uiEditor_->Draw();
		}
	);

	engine_->LoadRenderPass("MainPass", [&]()
		{
			engine_->DrawToRenderPass("MainPass", "HUD");

			// ポストエフェクトのブルーム描画
			bloom_->Draw();

			// ポストエフェクトのビネット描画
			vignetting_->Draw();

			// フェード用スプライトの描画
			fadeSprite_->Draw();

			// ステージセレクトエディタのUI描画
			stageSelectEditor_->DrawUI();
		}
	);
}

/// @brief 更新処理
void TitleScene::Update()
{
	float dt = engine_->GetDeltaTime();

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