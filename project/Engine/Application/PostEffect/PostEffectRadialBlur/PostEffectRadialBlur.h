#pragma once
#include <string>
#include "Handle/Handle.h"
#include "Data/PostEffectData/PostEffectData.h"

class GrowthEngine;

class PostEffectRadialBlur
{
public:

	/// @brief コンストラクタ
	/// @param name 名前
	PostEffectRadialBlur(std::string name);

	/// @brief 描画処理
	void Draw();


private:

	// エンジン
	const GrowthEngine* engine_ = nullptr;

	/// @brief 名前
	std::string name_{};

	/// @brief 種類
	Engine::PostEffect::Type type_;

	/// @brief パラメータ
	Engine::PostEffect::RadialBlur* param_ = nullptr;

	/// @brief ハンドル
	PostEffectHandle handle_ = 0;
};

