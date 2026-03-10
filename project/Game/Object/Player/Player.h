#pragma once
#include "../Object.h"

class Context;

class Player : public Object
{
public:

	/// @brief コンストラクタ
	/// @param position 
	Player(const Vector2& position) : Object(position){}

	/// @brief 初期化
	/// @param context 
	void Initialize(Context* context);

	/// @brief 更新処理
	void Update() override;


private:

	/// @brief コンテキスト
	Context* context_ = nullptr;
};
