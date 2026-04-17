#pragma once
#include "../Character.h"

class NPC : public Character
{
public:

	/// @brief コンストラクタ
	/// @param initData 
	NPC(const InitData& initData);

	/// @brief 初期化
	void Initialize();

	/// @brief 更新処理
	virtual void Update() override;


private:


};

