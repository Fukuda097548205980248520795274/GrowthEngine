#pragma once
#include "GrowthEngine.h"
#include "PhaseManager/PhaseManager.h"
#include "StageSelectEditor/StageSelectEditor.h"

class TitleScene : public Scene
{
public:

	// @brief タイトルシーンのフェーズ
	enum class PhaseType
	{
		Intro,
		MainMenu,
		Play,
		StageSelect,
		Quit,
	};

	/// @brief タイトルシーンのメインメニューの選択肢
	enum class MainMenuOption
	{
		StartGame,
		QuitGame,

		// 選択肢の最大数を表すためのダミー値
		MaxOption,
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

	/// @brief イントロの経過時間
	float introTimer_ = 0.0f;

	/// @brief イントロの表示時間
	static constexpr float kIntroDuration = 1.0f;


private:

	/// @brief タイトルシーンのメインメニュー初期化処理
	void MainMenuInitialize();

	/// @brief タイトルシーンのメインメニュー更新処理
	void MainMenuUpdate();

	/// @brief タイトルシーンのメインメニュー描画処理
	void MainMenuDraw();

	/// @brief メインメニューの選択肢を処理する
	void SelectMainMenuOption();

	/// @brief メインメニューの選択肢を実行する
	void ExecuteMainMenuOption();

	/// @brief メインメニューの選択肢
	MainMenuOption mainManuOption_ = MainMenuOption::StartGame;


private:

	/// @brief タイトルシーンのステージセレクト初期化処理
	void StageSelectInitialize();

	/// @brief タイトルシーンのステージセレクト更新処理
	void StageSelectUpdate();

	/// @brief タイトルシーンのステージセレクト描画処理
	void StageSelectDraw();

	/// @brief ステージセレクトの選択肢を処理する
	void StageSelectMove();

	/// @brief ステージセレクトの選択肢を実行する
	void StageSelectExecute();

	/// @brief ステージセレクトの選択中のインデックス
	int stageSelectIndex_ = 0;


private:

	/// @brief タイトルシーンのプレイ初期化処理
	void PlayInitialize();
	
	/// @brief タイトルシーンのプレイ更新処理
	void PlayUpdate();

	/// @brief タイトルシーンのプレイ描画処理
	void PlayDraw();

	/// @brief プレイの経過時間
	float playTimer_ = 0.0f;

	/// @brief プレイの表示時間
	static constexpr float kPlayDuration = 1.0f;


private:

	/// @brief タイトルシーンの終了初期化処理
	void QuitInitialize();

	/// @brief タイトルシーンの終了更新処理
	void QuitUpdate();

	/// @brief タイトルシーンの終了描画処理
	void QuitDraw();

	/// @brief 終了の経過時間
	float quitTimer_ = 0.0f;

	/// @brief 終了の表示時間
	static constexpr float kQuitDuration = 1.0f;


private:

	/// @brief 次のステージ名
	std::string nextStageName_ = "";


private:

	/// @brief フェーズマネージャ
	std::unique_ptr<PhaseManager<PhaseType>> phaseManager_;

	/// @brief フェード用スプライト
	std::unique_ptr<Sprite> fadeSprite_;

	//  上下のキー入力
	std::unique_ptr<InputKey> wKey_;
	std::unique_ptr<InputKey> sKey_;

	// 決定キー入力
	std::unique_ptr<InputKey> spaceKey_;

	// 上下の左スティック入力
	std::unique_ptr<InputGamepadLeftStick> upLeftStick_;
	std::unique_ptr<InputGamepadLeftStick> downLeftStick_;

	// Aボタン
	std::unique_ptr<InputGamepadButton> aButton_;


private:

	/// @brief ステージセレクトエディタ
	std::unique_ptr<StageSelectEditor> stageSelectEditor_;
};

