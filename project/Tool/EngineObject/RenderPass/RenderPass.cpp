#include "RenderPass.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
/// @param drawFunc 
RenderPass::RenderPass(const std::string& name, std::function<void()> drawFunc)
	: name_(name)
{
	// インスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// レンダーパスを読み込む
	handle_ = engine_->LoadRenderPass(name, drawFunc);

	// レンダーパスのパラメータを取得する
	param_ = engine_->GetRenderPassParam(handle_);
}

/// @brief レンダーパスを実行する
void RenderPass::Execute()
{
	engine_->ExecuteRenderPass(handle_);
}