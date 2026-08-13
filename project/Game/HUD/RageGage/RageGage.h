#pragma once
#include "../HUD.h"

class RageGage : public HUD
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

		// 体力ゲージの区切り
		PrefabInstanceSprite* hpSeparatorSprite = nullptr;

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
	~RageGage();

	/// @brief 初期化
	/// @param initData 
	void Initialize(const InitData& initData);

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;

	/// @brief 現在のゲージを設定する
	/// @param gage 
	void SetCurrentGage(int gage);

	/// @brief 最大ゲージを設定する
	/// @param maxGage 
	void SetMaxGage(int maxGage) { maxGage_ = maxGage; }

	/// @brief 色を設定する
	/// @param color 
	void SetColor(const Vector3& color) { color_ = color; }

	/// @brief 体力バーを持つキャラクターが死亡したときの処理
	void Death();


protected:

	// 横幅
	int width_ = 0;

	// 最大ゲージ
	int maxGage_ = 0;

	/// @brief 現在のゲージ
	int currentGage_ = 0;

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

	// 体力ゲージの区切り
	PrefabInstanceSprite* hpSeparatorSprite_ = nullptr;


protected:

	/// @brief 死亡したかどうか
	bool isDeath_ = false;

	// 死亡してからの経過時間
	static constexpr float kDeathTime = 0.2f;

	/// @brief 死亡タイマー
	float deathTimer_ = kDeathTime;
};

