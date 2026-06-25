#pragma once
#include "../HUD.h"

class Character;

class HP : public HUD
{
public:

	/// @brief 初期化データ
	struct InitData
	{
		// 横幅
		int width = 100;

		// hp枠のスプライト
		PrefabInstanceSprite* hpFrameLeftSprite = nullptr;
		PrefabInstanceSprite* hpFrameMiddleSprite = nullptr;
		PrefabInstanceSprite* hpFrameRightSprite = nullptr;

		// hpのスプライト
		PrefabInstanceSprite* hpLeftSprite = nullptr;
		PrefabInstanceSprite* hpMiddleSprite = nullptr;
		PrefabInstanceSprite* hpRightSprite = nullptr;

		// 遅延hpのスプライト
		PrefabInstanceSprite* delayHpLeftSprite = nullptr;
		PrefabInstanceSprite* delayHpMiddleSprite = nullptr;
		PrefabInstanceSprite* delayHpRightSprite = nullptr;

		// 位置
		Vector2 position = Vector2(0.0f, 0.0f);

		// 大きさ
		Vector2 scale = Vector2(1.0f, 1.0f);

		/// @brief 色
		Vector3 color = Vector3(1.0f, 1.0f, 1.0f);

		// 不透明度
		float alpha = 1.0f;
	};


public:

	/// @brief 仮想デストラクタ
	virtual ~HP();

	/// @brief 初期化
	/// @param initData 
	void Initialize(const InitData& initData);

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 描画処理
	virtual void Draw() override;

	/// @brief 現在の体力を設定する
	/// @param hp 
	void SetCurrentHP(int hp);

	/// @brief 最大体力を設定する
	/// @param maxHP 
	void SetMaxHP(int maxHP) { maxHP_ = maxHP;  }

	/// @brief 体力バーを持つキャラクターが死亡したときの処理
	void Death();


protected:

	// 横幅
	int width_ = 100;

	// 最大体力
	int maxHP_ = 100;

	/// @brief 現在の体力
	int currentHP_ = 100;

	/// @brief 色
	Vector3 color_ = Vector3(1.0f, 1.0f, 1.0f);

	/// @brief 不透明度
	float alpha_ = 1.0f;

	// hp枠のスプライト
	PrefabInstanceSprite* hpFrameLeftSprite_ = nullptr;
	PrefabInstanceSprite* hpFrameMiddleSprite_ = nullptr;
	PrefabInstanceSprite* hpFrameRightSprite_ = nullptr;

	// hpのスプライト
	PrefabInstanceSprite* hpLeftSprite_ = nullptr;
	PrefabInstanceSprite* hpMiddleSprite_ = nullptr;
	PrefabInstanceSprite* hpRightSprite_ = nullptr;

	// 遅延hpのスプライト
	PrefabInstanceSprite* delayHpLeftSprite_ = nullptr;
	PrefabInstanceSprite* delayHpMiddleSprite_ = nullptr;
	PrefabInstanceSprite* delayHpRightSprite_ = nullptr;


protected:

	/// @brief 体力が変化したかどうか
	bool isChanged_ = false;

	// 体力変化タイマー
	float changeTimer_ = 0.0f;

	// 遅延して減少する体力
	int delayHP_ = 0;


protected:

	/// @brief 死亡したかどうか
	bool isDeath_ = false;

	// 死亡してからの経過時間
	static constexpr float kDeathTime = 0.2f;

	/// @brief 死亡タイマー
	float deathTimer_ = kDeathTime;
};

