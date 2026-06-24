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

		// 位置
		Vector2 position = Vector2(0.0f, 0.0f);

		// 大きさ
		Vector2 scale = Vector2(1.0f, 1.0f);

		/// @brief キャラクター
		Character* character = nullptr;
	};


public:

	/// @brief 仮想デストラクタ
	virtual ~HP();

	/// @brief 初期化
	/// @param initData 
	void Initialize(const InitData& initData);

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;


protected:

	/// @brief キャラクター
	Character* character_ = nullptr;

	// 横幅
	int width_ = 100;

	// 最大体力
	int maxHP_ = 100;

	// hp枠のスプライト
	PrefabInstanceSprite* hpFrameLeftSprite_ = nullptr;
	PrefabInstanceSprite* hpFrameMiddleSprite_ = nullptr;
	PrefabInstanceSprite* hpFrameRightSprite_ = nullptr;

	// hpのスプライト
	PrefabInstanceSprite* hpLeftSprite_ = nullptr;
	PrefabInstanceSprite* hpMiddleSprite_ = nullptr;
	PrefabInstanceSprite* hpRightSprite_ = nullptr;
};

