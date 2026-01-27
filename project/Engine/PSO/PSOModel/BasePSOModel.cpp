#include "BasePSOModel.h"
#include <cassert>

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::BasePSOModel::Register(ID3D12GraphicsCommandList* commandList) const
{
	// nullptrチェック
	assert(commandList);

	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(graphicsPipelineState_[blendMode_].Get());
}