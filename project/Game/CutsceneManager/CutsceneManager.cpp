#include "CutsceneManager.h"
#include <algorithm>

#include "CutsceneEditor/CutsceneSerializer/CutsceneSerializer.h"
#include "Entity/Character/Character.h"

/// @brief 初期化処理
/// @param cutsceneCamera 
void CutsceneManager::Initialize(MainCamera3D* cutsceneCamera)
{
    cutsceneCamera_ = cutsceneCamera;

	// 登録されている演出パターンを更新
	RefreshCutsceneList();
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

    // 現在のデータをセット
    currentData_ = &it->second;
    currentDuration_ = currentData_->duration;
    onEndCallback_ = onEndCallback;
}

/// @brief 演出を更新する
/// @param dt 
void CutsceneManager::Update(float dt)
{
	// 演出中でなければスキップ
	if (!isPlaying_) return;

	// データかカメラが存在しない場合はスキップ
	if (!currentData_ || !cutsceneCamera_) return;

	// タイマーを進める
	timer_ += dt;

	// サンプリングしてカメラのパラメータを取得
	CameraSample sample = SampleCutscene(*currentData_, timer_);

	// カットシーン用カメラのパラメータを更新
	if (!currentData_->positionKeys.empty()) cutsceneCamera_->param_->transform.translate = sample.position;
	if (!currentData_->rotationKeys.empty()) cutsceneCamera_->param_->transform.rotate = sample.rotation;
	if (!currentData_->fovKeys.empty()) cutsceneCamera_->param_->setting.fov = sample.fov;

	// キャラクターのキーフレームをサンプリングして座標を更新
	for (const auto& track : currentData_->characterTracks)
	{
		// 登録されている全キャラクターを走査して名前が一致するものを探す
		Character* targetCharacter = nullptr;
		for (auto* character : Character::GetCharacters()) // Characterクラスの静的リストを活用
		{
			if (character && character->GetEditorName() == track.characterName)
			{
				targetCharacter = character;
				break;
			}
		}

		// キャラクターが見つかった場合、キーフレームデータをサンプリングして座標を上書き
		if (targetCharacter)
		{
			CharacterSample charSample = SampleCharacterTrack(track, timer_);
			if (!track.positionKeys.empty())
			{
				targetCharacter->SetPosition(charSample.position);
			}
		}
	}

	// タイマー終了
	if (timer_ >= currentDuration_)
	{
		isPlaying_ = false;
		timer_ = 0.0f;
		currentData_ = nullptr;

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

/// @brief 演出パターンを登録する
void CutsceneManager::RefreshCutsceneList()
{
	// 既存の演出データをクリア
	cutscenes_.clear();

	// カットシーンディレクトリが存在しない場合は作成
	if(!std::filesystem::exists(kCutsceneDir))
		std::filesystem::create_directories(kCutsceneDir);

	// ディレクトリ内のJSONファイルを検索して読み込む
	for (auto& entry : std::filesystem::directory_iterator(kCutsceneDir))
	{
		if (entry.path().extension() == ".json")
			cutscenes_[entry.path().stem().string()] = LoadCutscene(entry.path().string());
	}
}