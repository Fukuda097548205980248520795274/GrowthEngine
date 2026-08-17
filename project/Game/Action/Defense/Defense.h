#pragma once
#include "../Action.h"

/// @brief パリィの種類
enum class ParryType
{
	Guard, // ガード
	Deflect, // 受け流し
	Repel, // 弾き
};

namespace
{
	/// @brief パリィの種類を文字列で表す配列
	const char* ParryTypeNames[] = {
		"ガード",
		"受け流し",
		"弾き"
	};
}

/// @brief 防御アクションの初期化データ
struct DefenseInitData
{
	// @brief 防御する時間
	float defenseTime = 1.0f;

	// @brief パリィの種類
	ParryType parryType = ParryType::Guard;
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

	// @brief パリィの種類
	ParryType parryType_ = ParryType::Guard;
};

