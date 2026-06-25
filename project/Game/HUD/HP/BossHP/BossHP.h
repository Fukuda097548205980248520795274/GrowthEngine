#pragma once
#include "../HP.h"

class BossHP : public HP
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

		// 前側hpのスプライト
		PrefabInstanceSprite* hpFrontLeftSprite = nullptr;
		PrefabInstanceSprite* hpFrontMiddleSprite = nullptr;
		PrefabInstanceSprite* hpFrontRightSprite = nullptr;

		// 後ろ側hpのスプライト
		PrefabInstanceSprite* hpBackLeftSprite = nullptr;
		PrefabInstanceSprite* hpBackMiddleSprite = nullptr;
		PrefabInstanceSprite* hpBackRightSprite = nullptr;

		// 後ろ側の遅延hpのスプライト
		PrefabInstanceSprite* delayHpBackLeftSprite = nullptr;
		PrefabInstanceSprite* delayHpBackMiddleSprite = nullptr;
		PrefabInstanceSprite* delayHpBackRightSprite = nullptr;

		// 前側の遅延hpのスプライト
		PrefabInstanceSprite* delayHpFrontLeftSprite = nullptr;
		PrefabInstanceSprite* delayHpFrontMiddleSprite = nullptr;
		PrefabInstanceSprite* delayHpFrontRightSprite = nullptr;

		// 位置
		Vector2 position = Vector2(0.0f, 0.0f);

		// 大きさ
		Vector2 scale = Vector2(1.0f, 1.0f);

		// 不透明度
		float alpha = 1.0f;
	};


public:

	/// @brief コンストラクタ
	~BossHP();

	/// @brief 初期化
	/// @param initData 
	void Initialize(const InitData& initData);

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;

	/// @brief 現在の体力を設定する
	/// @param hp 
	void SetCurrentHP(int hp) override;



private:

	// 後ろ側のhpのスプライト
	PrefabInstanceSprite* hpBackLeftSprite_ = nullptr;
	PrefabInstanceSprite* hpBackMiddleSprite_ = nullptr;
	PrefabInstanceSprite* hpBackRightSprite_ = nullptr;

	// 前側の遅延hpのスプライト
	PrefabInstanceSprite* delayHpFrontLeftSprite_ = nullptr;
	PrefabInstanceSprite* delayHpFrontMiddleSprite_ = nullptr;
	PrefabInstanceSprite* delayHpFrontRightSprite_ = nullptr;

	/// @brief 後ろ側のhpの色
	Vector3 backColor_ = Vector3(0.0f, 0.0f, 0.0f);

	// 1ゲージあたりの体力
	static constexpr int32_t kOneGageHp = 100;

	// ゲージの色
	static constexpr Vector3 kGageColor[12] =
	{
		// 赤
		Vector3(1.0f, 0.0f, 0.0f),

		// オレンジ
		Vector3(1.0f, 0.5f, 0.0f),

		// 黄
		Vector3(1.0f, 1.0f, 0.0f),

		// 黄緑
		Vector3(0.5f, 1.0f, 0.0f),

		// 緑
		Vector3(0.0f, 1.0f, 0.0f),

		// 水色
		Vector3(0.0f, 1.0f, 0.5f),

		// 青
		Vector3(0.0f, 0.0f, 1.0f),

		// 青紫
		Vector3(0.5f, 0.0f, 1.0f),

		// 紫
		Vector3(1.0f, 0.0f, 1.0f),

		// ピンク
		Vector3(1.0f, 0.5f, 1.0f),

		// グレー
		Vector3(0.5f, 0.5f, 0.5f),

		// 白
		Vector3(1.0f, 1.0f, 1.0f)
	};

};