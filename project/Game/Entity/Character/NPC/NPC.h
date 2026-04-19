#pragma once
#include "../Character.h"
#include "BehaviorTree/BehaviorTree.h"

class NPC : public Character
{
public:

	/// @brief コンストラクタ
	/// @param initData 
	/// @param characterTag 
	NPC(const InitData& initData, CharacterTag characterTag);

	/// @brief 初期化
	void Initialize();

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 描画処理
	void Draw();


private:

	/// @brief 戦闘中かどうか
	bool isFighting_ = false;

	/// @brief ビヘイビアツリー
	std::unique_ptr<BehaviorTree> behaviorTree_ = nullptr;
};

