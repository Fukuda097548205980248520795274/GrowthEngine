#include "RenderContext.h"
#include "Log/Log.h"
#include <cassert>

/// @brief 初期化
/// @param log 
void Engine::RenderContext::Initialize(WinApp* winApp, Log* log)
{
	// nullptrチェック
	assert(winApp);

	// DX12Coreの生成と初期化
	core_ = std::make_unique<DX12Core>();
	core_->Initialize(log);

	// DX12Commandの生成と初期化
	command_ = std::make_unique<DX12Command>();
	command_->Initialize(core_->GetDevice(), log);

	// DX12Heapの生成と初期化
	heap_ = std::make_unique<DX12Heap>();
	heap_->Initialize(core_->GetDevice(), log);

	// DX12Bufferingの生成と初期化
	buffering_ = std::make_unique<DX12Buffering>();
	buffering_->Initialize(log, heap_.get(), winApp,
		core_->GetDXGIFactory(), core_->GetDevice(), command_->GetCommandQueue());
}