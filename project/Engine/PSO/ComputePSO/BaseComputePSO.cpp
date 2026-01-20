#include "BaseComputePSO.h"

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::BaseComputePSO::Register(ID3D12GraphicsCommandList* commandList)
{
	commandList->SetComputeRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(pipelineState_.Get());
}