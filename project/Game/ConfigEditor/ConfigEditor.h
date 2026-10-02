#pragma once
#include "GrowthEngine.h"
#include <numbers>

class GameScene;

// @brief ゲームの設定
struct GameConfig
{
	// ピボット中心の追従補間速度
	float pivotFollowSpeed = 10.0f;

	// ピボット回転速度[rad/s]
	float pivotRotateSpeed = 1.5f;

	// ピボットX軸回転の最大角度
	float pivotMaxPitch = 70.0f * (std::numbers::pi_v<float> / 180.0f);

	// ピボット中心の高さオフセット
	Vector3 pivotCenterOffset = Vector3(0.0f, 1.5f, 0.0f);

	/// @brief ロックオン時のピッチ角度の最大値[rad]
	float lockOnMaxPitch = 20.0f * (std::numbers::pi_v<float> / 180.0f);

	/// @brief ロックオン時のカメラ追従速度
	float lockOnCameraFollowSpeed = 3.0f;

	// カメラの縮小速度
	float cameraShrinkSpeed = 25.0f;

	// カメラの拡大速度
	float cameraExpandSpeed = 5.0f;
};

class ConfigEditor
{
public:

	/// @brief コンストラクタ
	/// @param scene 
	ConfigEditor(GameScene* scene) { Initialize(scene); }

	/// @brief デストラクタ
	~ConfigEditor() = default;

	/// @brief UIを描画する
	void DrawUI();

	/// @brief 設定ファイルを読み込む
	/// @param fileName 
	void Load(const std::string& fileName);

	/// @brief 設定ファイルを保存する
	/// @param fileName 
	void Save(const std::string& fileName);


private:

	/// @brief 初期化する
	/// @param scene 
	void Initialize(GameScene* scene);


private:

	/// @brief シーン
	GameScene* scene_ = nullptr;

	/// @brief 設定ファイルの内容
	GameConfig config_;

	/// @brief 現在の設定ファイルの名前
	std::string currentFileName_;

	/// @brief 設定ファイルのディレクトリ
    const std::string kDir = "./Assets/Parameter/Config/";
};

