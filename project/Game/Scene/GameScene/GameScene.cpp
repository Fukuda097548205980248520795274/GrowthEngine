#include "GameScene.h"

/// @brief 初期化
void GameScene::Initialize()
{
	light_ = std::make_unique<LightDirectional>("TEST");


	gameCamera2D_ = std::make_unique<GameCamera2D>("TEST");

	gameCamera_ = std::make_unique<GameCamera3D>("Test");

	engine_->LoadTexture("./Assets/Textures/rostock_laage_airport_4k.dds");

	model_ = std::make_unique<PrimitiveSkinningModel>(
		GrowthEngine::GetInstance()->LoadModel("./Assets/Models/walk", "walk.gltf"),
		GrowthEngine::GetInstance()->LoadAnimation("./Assets/Models/walk", "walk.gltf"),
		GrowthEngine::GetInstance()->LoadSkeleton("./Assets/Models/walk", "walk.gltf"),
		"suzanne");

	animationModel_ = std::make_unique<PrimitiveStaticModel>(
		GrowthEngine::GetInstance()->LoadModel("./Assets/Models/AnimatedCube", "AnimatedCube.gltf"),"AnimatedCube");
	animationModel_->param_->modelTransform.translate.y = -1.0f;

	blur_ = std::make_unique<PostEffectRadialBlur>("PostEffect");


	modelPrefab_ = std::make_unique<PrefabBaseStaticModel>(GrowthEngine::GetInstance()->LoadModel("./Assets/Models/plane", "plame.obj"), 100, "TEST");
	instance1_ = modelPrefab_->CreateInstance();
	instance2_ = modelPrefab_->CreateInstance();
	instance1_->param_.modelTransform.translate.y = -1.0f;
	instance2_->param_.modelTransform.translate.y = -2.0f;


	object_ = std::make_unique<AudioObject>("./Assets/Sounds/bgm/forget_me_not.mp3", 0.4f, true);
	object_->PlayAudio();

	sprite_ = std::make_unique<Sprite>(engine_->LoadTexture("./Assets/Textures/uvChecker.png"), "TestSprite");
}

/// @brief 更新処理
void GameScene::Update()
{
	ImGui::Begin("Stage Editor");

	// キャンバスのサイズ
	ImVec2 canvasSize = ImGui::GetContentRegionAvail();
	ImVec2 canvasPos = ImGui::GetCursorScreenPos();

	// DrawList を取得
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	// 背景
	drawList->AddRectFilled(canvasPos,
		ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
		IM_COL32(50, 50, 50, 255));

	// グリッド描画
	const float gridSize = 32.0f;
	for (float x = 0; x < canvasSize.x; x += gridSize) {
		drawList->AddLine(
			ImVec2(canvasPos.x + x, canvasPos.y),
			ImVec2(canvasPos.x + x, canvasPos.y + canvasSize.y),
			IM_COL32(80, 80, 80, 255)
		);
	}
	for (float y = 0; y < canvasSize.y; y += gridSize) {
		drawList->AddLine(
			ImVec2(canvasPos.x, canvasPos.y + y),
			ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + y),
			IM_COL32(80, 80, 80, 255)
		);
	}

	// タイル配置例
	ImVec2 tilePos = ImVec2(canvasPos.x + 32, canvasPos.y + 32);
	drawList->AddRectFilled(tilePos,
		ImVec2(tilePos.x + gridSize, tilePos.y + gridSize),
		IM_COL32(200, 100, 100, 255));

	ImGui::End();

	ImVec2 mousePos = ImGui::GetMousePos();
	bool isInside =
		mousePos.x >= canvasPos.x && mousePos.x < canvasPos.x + canvasSize.x &&
		mousePos.y >= canvasPos.y && mousePos.y < canvasPos.y + canvasSize.y;

	if (isInside && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		int gridX = static_cast<int32_t>((mousePos.x - canvasPos.x) / gridSize);
		int gridY = static_cast<int32_t>((mousePos.y - canvasPos.y) / gridSize);

		stage_[gridY][gridX] = id_;
	}

	ImGui::Begin("Palette");

	for (int i = 0; i < 3; i++) {
		if (ImGui::Selectable(tileName[i].c_str(), id_ == i))
		{
			id_ = i;
		}
	}

	ImGui::End();





	light_->param_->position = Vector3(0.0f, 10.0f, 0.0f);

	model_->param_->animation.timer += engine_->GetDeltaTime();
	model_->param_->animation.timer = std::fmod(model_->param_->animation.timer, 1.0f);

	if (engine_->GetKeyTrigger(DIK_SPACE))
	{
		Transition("Title");
	}
}

/// @brief 描画処理
void GameScene::Draw()
{

	model_->Draw();

	animationModel_->Draw();

	instance1_->Draw();
	instance2_->Draw();
	modelPrefab_->Draw();

	sprite_->Draw();

	blur_->Draw();
}