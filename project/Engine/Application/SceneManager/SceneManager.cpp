#include "SceneManager.h"
#include "Scene/GameScene/GameScene.h"
#include "Scene/TitleScene/TitleScene.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param sceneName 
SceneManager::SceneManager(const std::string& sceneName) :sceneName_(sceneName)
{
	// シーン名を取得する
	currentSceneName_ = sceneName_;

	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 最初は遷移する
	isTransition_ = true; 
}

/// @brief 更新処理
void SceneManager::Update()
{
#ifdef _DEVELOPMENT

	ImGui::Begin("Reset Scene");

	ImVec2 pos = ImGui::GetCursorScreenPos();
	float size = 40.0f;
	ImVec2 center = ImVec2(pos.x + size * 0.5f, pos.y + size * 0.5f);

	ImDrawList* draw = ImGui::GetWindowDrawList();

	// ボタンの当たり判定
	ImGui::InvisibleButton("play_button", ImVec2(size, size));
	bool hovered = ImGui::IsItemHovered();
	bool clicked = ImGui::IsItemClicked();

	// 背景の円
	draw->AddCircleFilled(center, size * 0.5f,
		hovered ? IM_COL32(200, 200, 200, 255) : IM_COL32(150, 150, 150, 255));

	// 三角形（再生マーク）
	ImVec2 p1 = ImVec2(center.x - size * 0.15f, center.y - size * 0.20f);
	ImVec2 p2 = ImVec2(center.x - size * 0.15f, center.y + size * 0.20f);
	ImVec2 p3 = ImVec2(center.x + size * 0.20f, center.y);

	draw->AddTriangleFilled(p1, p2, p3, IM_COL32(255, 255, 255, 255));

	if (clicked)
	{
		// 再生処理
		Transition(currentSceneName_);
	}

	ImGui::End();

#endif

	// 遷移するとき
	if (isTransition_)
	{
		// シーン前処理
		engine_->PerScene();

		// ゲームシーン
		if (sceneName_ == "Game")
		{
			scene_ = std::make_unique<GameScene>(this);
			scene_->Initialize();
			isTransition_ = false;
		}

		// タイトルシーン
		if (sceneName_ == "Title")
		{
			scene_ = std::make_unique<TitleScene>(this);
			scene_->Initialize();
			isTransition_ = false;
		}

		// シーン名を取得する
		currentSceneName_ = sceneName_;

		// まだ遷移中は処理しない
		assert(!isTransition_);
	}

	// シーンの更新
	scene_->Update();
}

/// @brief 描画処理
void SceneManager::Draw()
{
	// シーンの描画
	scene_->Draw();
}