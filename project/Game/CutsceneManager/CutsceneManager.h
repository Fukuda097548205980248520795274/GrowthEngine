#pragma once
#include "GrowthEngine.h"
#include <functional>
#include <string>

#include "CutsceneEditor/CutsceneKeyframe/CutsceneKeyframe.h"

class CutsceneManager
{
public:

	using OnEndCallback = std::function<void()>;

public:

	/// @brief コンストラクタ
	CutsceneManager() = default;

	/// @brief デストラクタ
	~CutsceneManager() = default;

	/// @brief 初期化処理
	/// @param cutsceneCamera 
	void Initialize(MainCamera3D* cutsceneCamera);

	/// @brief 演出を更新する
	/// @param dt 
	void Update(float dt);

	/// @brief 演出を開始する
	/// @param name 
	/// @param onEndCallback 
	void Play(const std::string& name, OnEndCallback onEndCallback = nullptr);

	/// @brief 演出中かどうかを取得する
	/// @return 
	bool IsPlaying() const { return isPlaying_; }

	/// @brief 登録されている演出名の一覧を取得する
	/// @return 
	std::vector<std::string> GetCutsceneNames() const;


private:

	/// @brief 演出パターンを登録する
	void RefreshCutsceneList();


private:

	/// @brief カットシーン用カメラ
	MainCamera3D* cutsceneCamera_ = nullptr;

	// 演出データのデータベース
	std::unordered_map<std::string, KeyframeCutsceneData> cutscenes_;

	// 演出中かどうか
	bool isPlaying_ = false;

	// 演出中のタイマー
	float timer_ = 0.0f;

	// 演出の時間
	float currentDuration_ = 0.0f;

	// 現在再生中の演出データ
	const KeyframeCutsceneData* currentData_ = nullptr;

	// 演出終了時のコールバック関数
	OnEndCallback onEndCallback_ = nullptr;

	// カットシーンデータを保存するディレクトリパス
	const std::string kCutsceneDir = "./Assets/Parameter/Cutscene/";
};

