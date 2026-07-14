#include "CutsceneManager.h"
#include <algorithm>

/// @brief 演出パターンを登録する
/// @param name 
/// @param duration 
/// @param func 
void CutsceneManager::RegisterCutscene(const std::string& name, float duration, CameraWorkFunc func)
{
    cutscenes_[name] = CutsceneData{ duration, func };
}

/// @brief 演出を開始する
/// @param name 
/// @param onEndCallback 
void CutsceneManager::Play(const std::string& name, OnEndCallback onEndCallback)
{
    // 登録されていない演出名ならスキップ
    auto it = cutscenes_.find(name);
    if (it == cutscenes_.end()) return;

    isPlaying_ = true;
    timer_ = 0.0f;
    currentDuration_ = it->second.duration;
    currentCameraWork_ = it->second.cameraWork;
    onEndCallback_ = onEndCallback;
}

/// @brief 演出を更新する
/// @param dt 
void CutsceneManager::Update(float dt)
{
	// 演出中でなければスキップ
    if (!isPlaying_) return;

	// タイマーを進める
    timer_ += dt;

	// 演出の進行度を計算する（0.0f から 1.0f の範囲にクランプ）
    float progress = std::clamp(timer_ / currentDuration_, 0.0f, 1.0f);

	// カメラワーク関数を呼び出す
    if (currentCameraWork_)
        currentCameraWork_(progress, dt);

    // タイマー終了
    if (timer_ >= currentDuration_)
    {
        isPlaying_ = false;
        timer_ = 0.0f;
        currentCameraWork_ = nullptr;

		// 演出終了時のコールバックを呼び出す
        if (onEndCallback_)
            onEndCallback_();
    }
}

/// @brief 登録されている演出名の一覧を取得する
/// @return 
std::vector<std::string> CutsceneManager::GetCutsceneNames() const
{
    std::vector<std::string> names;
    for (const auto& [name, data] : cutscenes_)
    {
        names.push_back(name);
    }
    return names;
}