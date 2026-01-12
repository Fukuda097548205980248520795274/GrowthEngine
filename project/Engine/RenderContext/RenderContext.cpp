#include "RenderContext.h"
#include "Log/Log.h"

/// @brief 初期化
/// @param log 
void Engine::RenderContext::Initialize(Log* log)
{
	// D3D12Coreの生成と初期化
	core_ = std::make_unique<D3D12Core>();
	core_->Initialize(log);
}