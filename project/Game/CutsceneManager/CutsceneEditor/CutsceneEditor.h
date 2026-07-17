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
		FOV 
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



private:

	/// @brief キーフレームを追加する
	/// @param time 
	/// @param pos 
	void AddPositionKeyframe(float time, const Vector3& pos);

	/// @brief 回転のキーフレームを追加する
	/// @param time 
	/// @param rot 
	void AddRotationKeyframe(float time, const Vector3& rot);

	/// @brief FOVのキーフレームを追加する
	/// @param time 
	/// @param fov 
	void AddFovKeyframe(float time, float fov);

	/// @brief キャラクターの位置キーフレームを追加する
	/// @param charName 
	/// @param time 
	/// @param pos 
	void AddCharacterPositionKeyframe(const std::string& charName, float time, const Vector3& pos);


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


private:

	// 選択中のキャラクター名
	std::string selectedCharacterName_ = "";
};

