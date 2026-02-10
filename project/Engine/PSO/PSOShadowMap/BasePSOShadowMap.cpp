#include "BasePSOShadowMap.h"
#include <cassert>

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::BasePSOShadowMap::Register(ID3D12GraphicsCommandList* commandList)
{
	// nullptrチェック
	assert(commandList);

	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(graphicsPipelineState_.Get());
}