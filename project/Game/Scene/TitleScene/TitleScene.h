#pragma once
#include "GrowthEngine.h"
#include "PhaseManager/PhaseManager.h"

class TitleScene : public Scene
{
public:

	// @brief タイトルシーンのフェーズ
	enum class PhaseType
	{
		Intro,
		MainMenu,
		Play,
		Quit,
	};


public:

	/// @brief コンストラクタ
	/// @param sceneManager 
	TitleScene(SceneManager* sceneManager) : Scene(sceneManager) {}

	/// @brief 初期化
	void Initialize() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;


private:

	/// @brief タイトルシーンのイントロ初期化処理
	void IntroInitialize();

	/// @brief タイトルシーンのイントロ更新処理
	void IntroUpdate();

	/// @brief タイトルシーンのイントロ描画処理
	void IntroDraw();


private:

	/// @brief タイトルシーンのメインメニュー初期化処理
	void MainMenuInitialize();

	/// @brief タイトルシーンのメインメニュー更新処理
	void MainMenuUpdate();

	/// @brief タイトルシーンのメインメニュー描画処理
	void MainMenuDraw();


private:

	/// @brief タイトルシーンのプレイ初期化処理
	void PlayInitialize();
	
	/// @brief タイトルシーンのプレイ更新処理
	void PlayUpdate();

	/// @brief タイトルシーンのプレイ描画処理
	void PlayDraw();


private:

	/// @brief タイトルシーンの終了初期化処理
	void QuitInitialize();

	/// @brief タイトルシーンの終了更新処理
	void QuitUpdate();

	/// @brief タイトルシーンの終了描画処理
	void QuitDraw();


private:

	/// @brief フェーズマネージャ
	std::unique_ptr<PhaseManager<PhaseType>> phaseManager_;

};

