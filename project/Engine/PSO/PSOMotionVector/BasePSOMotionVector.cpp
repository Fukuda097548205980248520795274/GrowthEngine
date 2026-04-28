#include "BasePSOMotionVector.h"
#include <cassert>

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::BasePSOMotionVector::Register(ID3D12GraphicsCommandList* commandList)
{
	// nullptrチェック
	assert(commandList);

	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(graphicsPipelineState_.Get());
}