#pragma once
#include "../Action.h"

// 初期化用データ
struct TelegraphInitData
{
	// 予備動作時間
	float time = 0.0f;

	/// @brief アニメーションハンドル
	AnimationHandle hAnimation = 0;
};

class Telegraph : public Action
{
public:

	/// @brief コンストラクタ
	/// @param character 
	/// @param initData 
	Telegraph(Character* character, const TelegraphInitData& initData);

	/// @brief デストラクタ
	~Telegraph();

	/// @brief 実行
	virtual void Exec() override;

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief リセット
	virtual void Reset() override;

	/// @brief 終了、中断
	virtual void Exit() override;

	/// @brief 予備動作中かどうか
	/// @return 
	bool IsUse() const override;


private:

	/// @brief タイマー
	float timer_ = 0.0f;

	/// @brief 予備動作時間
	float time_ = 0.0f;

	/// @brief アニメーションハンドル
	AnimationHandle hAnimation_ = 0;
};

