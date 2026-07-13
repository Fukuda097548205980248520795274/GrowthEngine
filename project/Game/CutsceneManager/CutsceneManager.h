#pragma once
#include <functional>
#include <string>

class CutsceneManager
{
public:

	using CameraWorkFunc = std::function<void(float progress, float dt)>;
	using OnEndCallback = std::function<void()>;

	// カットシーンのデータ構造体
	struct CutsceneData
	{
		float duration = 0.0f;
		CameraWorkFunc cameraWork = nullptr;
	};

public:

	/// @brief コンストラクタ
	CutsceneManager() = default;

	/// @brief デストラクタ
	~CutsceneManager() = default;


	/// @brief 演出パターンを登録する
	/// @param name 
	/// @param duration 
	/// @param func 
	void RegisterCutscene(const std::string& name, float duration, CameraWorkFunc func);

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

	// 演出データのデータベース
	std::unordered_map<std::string, CutsceneData> cutscenes_;

	// 演出中かどうか
	bool isPlaying_ = false;

	// 演出中のタイマー
	float timer_ = 0.0f;

	// 演出の時間
	float currentDuration_ = 0.0f;

	// 演出中のカメラワーク関数
	CameraWorkFunc currentCameraWork_ = nullptr;

	// 演出終了時のコールバック関数
	OnEndCallback onEndCallback_ = nullptr;
};

