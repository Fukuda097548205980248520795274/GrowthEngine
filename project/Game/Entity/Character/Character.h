#pragma once
#define NOMINMAX
#include "../Entity.h"
#include "BlackBoard/BlackBoard.h"

class Character : public Entity
{
public:

	/// @brief キャラクターのタグ
	enum class CharacterTag
	{
		PlayerSide,
		EnemySide,
	};

	/// @brief 初期化用データ
	struct InitData
	{
		/// @brief 位置
		Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

		/// @brief 体力
		int hp = 0;
		
		/// @brief モデル
		Render3DSkinningModel* model_ = nullptr;
	};


public:

	/// @brief コンストラクタ
	/// @param initData 
	Character(const InitData& initData);

	/// @brief デストラクタ
	virtual ~Character() override;

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 回避を開始する
	/// @param moveInputDirection
	/// @param hasMoveInput
	/// @param cameraYaw
	void StartAvoid(const Vector2& moveInputDirection, bool hasMoveInput, float cameraYaw);

	/// @brief 回避中の更新処理
	/// @param deltaTime
	void UpdateAvoid(float deltaTime);

    /// @brief 連続回避を試行する
	/// @param moveInputDirection
	/// @param hasMoveInput
	/// @param cameraYaw
	void ReserveNextAvoid(const Vector2& moveInputDirection, bool hasMoveInput, float cameraYaw);

	/// @brief キャラクターのタグを取得する
	/// @return 
	CharacterTag GetCharacterTag()const { return characterTag_; }

	/// @brief 移動を停止させる
	void MoveStop();

	/// @brief XZ平面の移動入力を設定する
	/// @param direction
	/// @param maxSpeed
	void SetMoveInputXZ(const Vector2& direction, float maxSpeed);

	/// @brief 現在向いている方向ベクトルを取得する
	/// @return
	Vector3 GetDirection() const { return direction_; }

	/// @brief 現在位置を取得する
	/// @return
	Vector3 GetPosition() const { return worldTransform_->translate_; }

	/// @brief ロックオンしているターゲットを取得する
	/// @return
	Character* GetLockOnTarget() const { return lockOnTarget_; }


protected:

	// キャラクターのタグ
	CharacterTag characterTag_;

	/// @brief ブラックボード
	std::unique_ptr<Blackboard> blackboard_ = nullptr;


protected:

	/// @brief 方向
	Vector3 direction_ = Vector3(0.0f, 0.0f, 1.0f);


protected:

   // 速度補間の応答速度(1秒あたり)
	float velocityLerpSpeed_ = 6.0f;

	/// @brief 目標速度
	Vector3 targetVelocity_ = Vector3(0.0f, 0.0f, 0.0f);

	/// @brief 現在の速度
	Vector3 currentVelocity_ = Vector3(0.0f, 0.0f, 0.0f);


protected:

	/// @brief ダッシュフラグ
	bool isDash_ = false;


protected:

	/// @brief 回避フラグ
	bool isAvoid_ = false;

	/// @brief 回避開始位置
	Vector3 avoidStartPosition_ = Vector3(0.0f, 0.0f, 0.0f);

	/// @brief 回避終了位置
	Vector3 avoidEndPosition_ = Vector3(0.0f, 0.0f, 0.0f);

	/// @brief 回避経過時間
	float avoidElapsedTime_ = 0.0f;

	/// @brief 回避時間
	float avoidDuration_ = 0.3f;

	/// @brief 回避距離
	float avoidDistance_ = 1.5f;

	/// @brief 連続回避の最大回数
	int maxConsecutiveAvoidCount_ = 3;

	/// @brief 現在の連続回避回数
	int currentAvoidCount_ = 0;

	/// @brief 回避方向を取得する
	/// @param moveInputDirection
	/// @param hasMoveInput
	/// @param cameraYaw
	/// @return
	Vector2 GetAvoidDirection(const Vector2& moveInputDirection, bool hasMoveInput, float cameraYaw) const;


protected:

	/// @brief 目標Y回転
	float targetYaw_ = 0.0f;

	/// @brief 目標回転が有効かどうか
	bool hasTargetYaw_ = false;

	// 入力に応じて目標速度と目標回転を更新する
	static constexpr float kRotateThreshold = 0.0001f;


protected:

	// 構え中かどうか
	bool isStance_ = false;

	/// @brief ロックオンしているターゲット
	Character* lockOnTarget_ = nullptr;

	// 構え中のロックオン候補を更新する
	void UpdateLockOnTargets();

	// Characterインスタンスのリスト
	static std::vector<Character*> characters_;


protected:

	/// @brief 攻撃しているかどうか
	bool isAttack_ = false;


protected:

	/// @brief モデル
	Render3DSkinningModel* model_ = nullptr;
};

