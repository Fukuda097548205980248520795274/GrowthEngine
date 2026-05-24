#pragma once
#include "GrowthEngine.h"

class EffectManager
{
public:

	/// @brief デストラクタ
	~EffectManager() = default;

	/// @brief インスタンスを取得する
	/// @return 
	static EffectManager* GetInstance();

	/// @brief 更新処理
	void Update();

	/// @brief 描画処理
	void Draw();


private:

	/// @brief 初期化処理
	void Initialize();


private:

	/// @brief インスタンス
	static std::unique_ptr<EffectManager> instance_;

	// コンストラクタと代入演算子を削除
	EffectManager() = default;
	EffectManager(const EffectManager&) = delete;
	EffectManager& operator=(const EffectManager&) = delete;
};

