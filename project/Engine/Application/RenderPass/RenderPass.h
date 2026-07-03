#pragma once
#include "Store/RenderPassStore/RenderPassData/RenderPassData.h"

class GrowthEngine;

class RenderPass
{
public:

	/// @brief コンストラクタ
	/// @param name 
	/// @param drawFunc 
	RenderPass(const std::string& name, std::function<void()> drawFunc);

	/// @brief デストラクタ
	~RenderPass() = default;

	/// @brief レンダーパスを実行する
	void Execute();

	/// @brief パラメータ
	Engine::RenderPassData::Param* param_ = nullptr;


private:

	/// @brief エンジン
	GrowthEngine* engine_ = nullptr;

	/// @brief ハンドル
	RenderPassHandle handle_ = 0;

	/// @brief レンダーパス名
	std::string name_ = "";
};

