#pragma once
#include "GrowthEngine.h"

class Character;
class NPC;

class AIMetricsEditor
{
public:

	/// @brief コンストラクタ
	AIMetricsEditor();

	/// @brief デストラクタ
	~AIMetricsEditor();

	/// @brief 初期化
	/// @param mainCamera 
	void Initialize(MainCamera3D* mainCamera);

	/// @brief 更新処理
	/// @param deltaTime 
	void Update(float deltaTime);

	// ImGuiによる描画処理
	void Draw();


private:

	/// @brief 移動のスムーズさを計測する
	/// @param npc 
	/// @param dt 
	/// @param currentTime 
	void UpdateMovementSmoothness(const NPC* npc, float currentTime);

	/// @brief 壁ヒットの回数を計測する
	/// @param npc 
	/// @param currentTime 
	void UpdateWallHitMetrics(const NPC* npc, float currentTime);


private:

	/// @brief キャラクター選択UIを描画する
	void DrawSelectCharacterUI();

	/// @brief 計測エディタのUIを描画する
	void DrawMeasurementEditorUI();


private:

	// @brief メインカメラ
	MainCamera3D* mainCamera_ = nullptr;

	/// @brief 移動のスムーズさを計測するための履歴
	std::list<std::pair<float, Vector3>> movementHistory_;

	// 急旋回の回数
	int abruptTurnCount_ = 0;

	/// @brief 移動のスムーズさを計測するためのタイマー
	float movementTimer_ = 0.0f;

	
private:

	/// @brief 現在攻撃中のNPCの数
	int currentAttackingNPCs_ = 0;

	/// @brief 次の攻撃までの最短クールダウン時間
	float minNextAttackCooldown_ = 0.0f;

	/// @brief プレイヤーがコンボ中かどうか
	bool isPlayerInCombo_ = false;

	/// @brief プレイヤーの死角攻撃が試みられたかどうか
	bool isBlindSpotAttackAttempted_ = false;


private:

	/// @brief ターゲットまでの平均距離
	float avgDistanceToTarget_ = 0.0f;

	/// @brief 3m以内にいるNPCの数
	int surroundingNPCCount_ = 0;

	/// @brief NPC間の最小距離
	float minDistanceBetweenNPCs_ = 0.0f;

	/// @brief 狙われていないキャラクターの数
	int untargetedCharacterCount_ = 0;

	/// @brief スタック時間の最大値
	float maxStuckTime_ = 0.0f;

	// 一人のターゲットに包囲している人数
	int surroundingCount_ = 0;

	/// @brief 1v1交戦時の静止時間
	float maxStaticPositionTime_ = 0.0f;


private:

	// @brief 前回の位置を保持する変数
	Vector3 lastPosition_ = { 0, 0, 0 };

	// @brief 静止時間を計測する変数
	float staticTime_ = 0.0f;

	// @brief スタック時間を計測する変数
	float stuckTime_ = 0.0f;


private:

	// 壁ヒットのタイムスタンプを保持するリスト
	std::list<float> wallHitTimestamps_;

	/// @brief 10秒間の壁ヒット回数
	int wallHitCountIn10s_ = 0;

	/// @brief 壁にぶつかった回数を計測するためのタイマー
	float wallHitTimer_ = 0.0f;


private:

	/// @brief 1v1時の静止時間を計測するための変数
	float staticTime1v1_ = 0.0f;


private:

	/// @brief エディタ起動からの累計時間
	float editorTotalTime_ = 0.0f;

	/// @brief 最後に迂回状態になった時間
	float lastDetourTime_ = -999.0f;

	/// @brief 前フレームでの迂回状態（立ち上がりエッジ検知用）
	bool isPrevDetouring_ = false;

	/// @brief 1秒未満で連続して迂回した回数
	int frequentDetourCount_ = 0;


private:

	/// @brief ターゲット評価のタイマー
	float targetEvaluationTimer_ = 0.0f;

	/// @brief ターゲット再評価が行われなかった違反カウント
	int targetEvalViolationCount_ = 0;
};

