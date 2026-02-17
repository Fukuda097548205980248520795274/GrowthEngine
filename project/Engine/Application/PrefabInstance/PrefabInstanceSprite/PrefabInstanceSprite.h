#pragma once
#include <functional>

class PrefabInstanceSprite
{
public:

	PrefabInstanceSprite(std::function<void()> drawCall);

	void Draw();


private:

	/// @brief 描画処理
	std::function<void()> drawCall_;
};

