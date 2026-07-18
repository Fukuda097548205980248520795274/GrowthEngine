#pragma once
#include <memory>
#include "CutsceneKeyframe/CutsceneKeyframe.h"

class MainCamera3D;

class CutsceneEditor
{
public:

	/// @brief ドラッグ中のトラックの種類
	enum class DraggingTrack 
	{
		None, 
		Position, 
		Rotation,
		FOV,
		CharacterPosition
	};


public:

	/// @brief コンストラクタ
	CutsceneEditor() = default;

	/// @brief デストラクタ
	~CutsceneEditor() = default;

	/// @brief 初期化処理
	/// @param cutsceneCamera 
	/// @param mainCamera 
	void Initialize(MainCamera3D* cutsceneCamera, MainCamera3D* mainCamera);

	/// @brief 更新処理
	/// @param dt 
	void Update(float dt);

	/// @brief UIを描画する
	void DrawUI();

	/// @brief エディタがアクティブかどうかを取得する
	/// @return 
	bool IsActive() const { return isActive_; }

	/// @brief エディタを開くか閉じるかを設定する
	/// @param active 
	void SetActive(bool active);

private:

	/// @brief エディタUIを描画する
	void DrawEditorUI();

	/// @brief タイムラインUIを描画する
	void DrawTimelineUI();


	/// @brief エディタを開くときの処理
	void OnEnableEditor();

	/// @brief エディタを閉じるときの処理
	void OnDisableEditor();

	/// @brief 自由カメラの更新
	/// @param dt 
	void UpdateFreeCamera(float dt);

	/// @brief ファイルリストを更新する
	void RefreshFileList();

	/// @brief キーフレームを追加する
	/// @tparam T 
	/// @param track 
	/// @param time 
	/// @param value 
	template<typename T>
	void AddKeyframeToTrack(std::vector<Key<T>>& track, float time, const T& value)
	{
		// 同一時間軸にキーフレームがすでに存在する場合は上書き
		auto it = std::find_if(track.begin(), track.end(),
			[time](const Key<T>& key) {return std::abs(key.time - time) < 0.01f;});

		// すでに存在する場合は上書き
		if (it != track.end())
		{
			it->time = time;
			it->value = value;
		}
		else
		{
			// 新しいキーフレームを追加
			Key<T> newKey;
			newKey.time = time;
			newKey.value = value;
			track.push_back(newKey);
		}

		// 時間順に並び替え
		std::sort(track.begin(), track.end(),
			[](const Key<T>& a, const Key<T>& b) {return a.time < b.time;});
	}


private:

	// カットシーン用カメラ
	MainCamera3D* cutsceneCamera_ = nullptr;

	// メインカメラ
	MainCamera3D* mainCamera_ = nullptr;


	// エディタが現在アクティブかどうか
	bool isActive_ = false;

	// エディタが前フレームでアクティブだったかどうか
	bool wasActive_ = false;


	// キーフレームカットシーンデータ
	KeyframeCutsceneData editingData_;

	// 再生中の時間
	float currentTime_ = 0.0f;

	// 再生中かどうか
	bool isPlaying_ = false;


	// 移動速度
	float moveSpeed_ = 5.0f;

	// 回転速度
	float rotateSpeed_ = 1.0f;


private:

	// カットシーンファイルのリスト
	std::vector<std::string> fileList_;

	// 選択中のファイルのインデックス
	int selectedFileIndex_ = 0;

	// 現在のファイルパス
	std::string currentFilePath_ = "";

	// カットシーンデータを保存するディレクトリパス
	const std::string kCutsceneDir = "./Assets/Parameter/Cutscene/";


private:

	// 現在ドラッグ中のトラック
	DraggingTrack draggingTrack_ = DraggingTrack::None;

	// 現在ドラッグ中のキーフレームインデックス
	int draggingKeyIndex_ = -1;

	/// @brief ドラッグ中のキャラクター名
	std::string draggingCharacterName_ = "";


private:

	// 選択中のトラック
	DraggingTrack selectedTrack_ = DraggingTrack::None;

	// 選択中のキーフレームインデックス
	int selectedKeyIndex_ = -1;


private:

	// 選択中のキャラクター名
	std::string selectedCharacterName_ = "";


private:

	// スナップが有効かどうか
	bool snapEnabled_ = true;

	// スナップのステップ値
	float snapStep_ = 1.0f / 60.0f;
};

