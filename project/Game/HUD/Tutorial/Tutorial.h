#pragma once
#include "../HUD.h"

class Tutorial : public HUD
{
public:

	/// @brief 仮想デストラクタ
	virtual ~Tutorial() = default;

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 描画処理
	virtual void Draw() override;

	/// @brief 有効化を設定する
	/// @param enable 
	void SetEnable(bool enable) { enable_ = enable; }

	/// @brief スプライトを追加する
	/// @param sprite 
	void AddSprite(Sprite* sprite) { sprite->SetParent(worldTransform_.get()); sprites_.push_back(sprite); }


protected:

	// チュートリアルの有効化フラグ
	bool enable_ = false;

	/// @brief 前回の有効化フラグ
	bool prevEnable_ = false;

	/// @brief 0.0f ~ 1.0f の範囲でフェードイン・フェードアウトの進行度を表す値
	float t_ = 0.0f;

	// フェードインの時間
	static constexpr float kFadeInDuration = 0.5f;
	float fadeInTimer_ = 0.0f;

	// フェードアウトの時間
	static constexpr float kFadeOutDuration = 0.5f;
	float fadeOutTimer_ = 0.0f;

	/// @brief チュートリアルのスプライトリスト
	std::list<Sprite*> sprites_;
};

