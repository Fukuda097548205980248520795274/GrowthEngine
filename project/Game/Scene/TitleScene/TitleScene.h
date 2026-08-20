#pragma once
#include "GrowthEngine.h"
#include "PhaseManager/PhaseManager.h"
#include "StageSelectEditor/StageSelectEditor.h"
#include "ObjectEditor/UIEditor/UIEditor.h"

class TitleScene : public Scene
{
public:

	// @brief タイトルシーンのフェーズ
	enum class PhaseType
	{
		Intro,
		Title,
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

	/// @brief イントロの経過時間
	float introTimer_ = 0.0f;

	/// @brief イントロの表示時間
	static constexpr float kIntroDuration = 1.0f;


private:

	/// @brief タイトルシーンのタイトル初期化処理
	void TitleInitialize();

	/// @brief タイトルシーンのタイトル更新処理
	void TitleUpdate();

	/// @brief タイトルの選択肢を処理する
	void ExecuteTitleOption();

	/// @brief タイトルのタイマー
	float titleTimer_ = 0.0f;

	/// @brief タイトルの時間
	static constexpr float kTitleDuration = 0.3f;

	/// @brief タイトルの選択肢が実行されたかどうか
	bool isTitleOptionExecuted_ = false;


	/// @brief タイトルのスプライトのアルファ値
	float titleSpriteParamAlpha_ = 0.0f;


private:

	/// @brief タイトルシーンのメインメニュー初期化処理
	void MainMenuInitialize();

	/// @brief タイトルシーンのメインメニュー更新処理
	void MainMenuUpdate();

	/// @brief メインメニューの選択肢を処理する
	void SelectMainMenuOption();

	/// @brief メインメニューの選択肢を実行する
	void ExecuteMainMenuOption();

	/// @brief タイトルに戻る
	void BackToTitle();

	/// @brief メインメニューの選択肢のスプライトを更新する
	void UpdateMainMenuOptionSprite();

	/// @brief メインメニューの選択肢が実行されたかどうか
	bool isMainMenuOptionExecuted_ = false;

	/// @brief ゲーム終了が実行されたかどうか
	bool isQuitExecuted_ = false;

	/// @brief タイトルに戻るかどうか
	bool isBackToTitle_ = false;

	/// @brief メインメニューのタイマー
	float mainMenuTimer_ = 0.0f;

	/// @brief メインメニューの表示時間
	static constexpr float kMainMenuDuration = 0.2f;

	/// @brief メインメニューのスプライトのアルファ値
	float mainMenuSpriteParamAlpha_ = 0.0f;

	/// @brief メインメニューの選択肢
	MainMenuOption mainManuOption_ = MainMenuOption::StartGame;

	/// @brief メインメニューのスプライト
	Sprite* mainMenuSprite_[static_cast<int>(MainMenuOption::MaxOption)] = { nullptr };

	/// @brief メインメニューの背景スプライト
	Sprite* mainMenuSpriteBG_[static_cast<int>(MainMenuOption::MaxOption)] = { nullptr };


private:

	/// @brief タイトルシーンのステージセレクト初期化処理
	void StageSelectInitialize();

	/// @brief タイトルシーンのステージセレクト更新処理
	void StageSelectUpdate();

	/// @brief ステージセレクトの選択肢を処理する
	void StageSelectMove();

	/// @brief ステージセレクトの選択肢を実行する
	void StageSelectExecute();

	/// @brief タイトルに戻る
	void BackToMainMenu();

	/// @brief ステージセレクトの背景スプライトを更新する
	void StageSelectBgSpriteUpdate();


	/// @brief ステージセレクトのタイマー
	float stageSelectTimer_ = 0.0f;

	/// @brief ステージセレクトの表示時間
	static constexpr float kStageSelectDuration = 0.2f;

	/// @brief ステージセレクトのスプライトのアルファ値
	float stageSelectSpriteParamAlpha_ = 0.0f;

	/// @brief ステージセレクトの選択中のインデックス
	int stageSelectIndex_ = 0;

	/// @brief ステージセレクトの選択肢が実行されたかどうか
	bool isStageSelectExecuted_ = false;

	/// @brief ステージセレクトの選択肢が実行されたかどうか
	bool isBackToMainMenu_ = false;


	/// @brief ステージセレクトの背景スプライト
	std::unique_ptr<PrefabBaseSprite> stageSelectBGSprite_ = nullptr;

	/// @brief ステージセレクトの選択肢のスプライト
	std::vector<PrefabInstanceSprite*> stageSelectOptionSprites_;

	/// @brief ステージセレクトの選択肢のテキストスプライト
	std::vector<Sprite*> stageSelectOptionTextSprites_;

	/// @brief ステージセレクトの選択肢のスプライトの位置
	static constexpr Vector2 kStageSelectOptionSpritePosition = Vector2(969.0f, 280.0f);

	/// @brief ステージセレクトの選択肢のスプライトの間隔
	static constexpr float kStageSelectOptionSpriteSpacing = 100.0f;


private:

	/// @brief タイトルシーンのプレイ初期化処理
	void PlayInitialize();
	
	/// @brief タイトルシーンのプレイ更新処理
	void PlayUpdate();

	/// @brief プレイの経過時間
	float playTimer_ = 0.0f;

	/// @brief プレイの表示時間
	static constexpr float kPlayDuration = 1.0f;


private:

	/// @brief タイトルシーンの終了初期化処理
	void QuitInitialize();

	/// @brief タイトルシーンの終了更新処理
	void QuitUpdate();

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

	/// @brief キャンセルキー入力
	std::unique_ptr<InputKey> escapeKey_;

	// 上下の左スティック入力
	std::unique_ptr<InputGamepadLeftStick> upLeftStick_;
	std::unique_ptr<InputGamepadLeftStick> downLeftStick_;

	// Aボタン
	std::unique_ptr<InputGamepadButton> aButton_;

	/// @brief Bボタン
	std::unique_ptr<InputGamepadButton> bButton_;


private:

	/// @brief ポストエフェクトのブルーム
	std::unique_ptr<PostEffectBloom> bloom_;

	/// @brief ポストエフェクトのビネット
	std::unique_ptr<PostEffectVignetting> vignetting_;


private:

	/// @brief タイトルBGM
	std::unique_ptr<Bgm> titleBgm_ = nullptr;

	/// @brief タイトルBGMの最大音量
	static constexpr float kBgmMaxVolume = 0.2f;

	/// @brief 選択SE
	std::unique_ptr<Se> selectSe_ = nullptr;

	/// @brief 決定SE
	std::unique_ptr<Se> executeSe_ = nullptr;

	/// @brief 戻るSE
	std::unique_ptr<Se> backSe_ = nullptr;


private:

	/// @brief ステージセレクトエディタ
	std::unique_ptr<StageSelectEditor> stageSelectEditor_;

	/// @brief UIエディタ
	std::unique_ptr<UIEditor> uiEditor_;
};

