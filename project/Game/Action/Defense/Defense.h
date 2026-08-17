#pragma once
#include "../Action.h"

/// @brief パリィの種類
enum class GuardType
{
	Guard, // ガード
	Deflect, // 受け流し
	Repel, // 弾き
};

/// @brief 防御アクションの初期化データ
struct DefenseInitData
{
	// @brief 防御する時間
	float defenseTime = 1.0f;
};

class Defense : public Action
{
public:

	/// @brief コンストラクタ
	/// @param character 
	/// @param initData 
	Defense(Character* character, const DefenseInitData& initData);

	/// @brief 実行
	virtual void Exec() override;

	/// @brief 更新処理
	virtual void Update() override {}

	/// @brief リセット
	virtual void Reset() override;

	/// @brief 終了、中断
	virtual void Exit() override;


private:

	/// @brief 防御を継続する時間
	float defenseTime_ = 1.0f;
};

