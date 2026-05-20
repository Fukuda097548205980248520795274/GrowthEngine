#include "BasePSOModel.h"
#include <cassert>

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param blendMode 
void Engine::BasePSOModel::Register(ID3D12GraphicsCommandList* commandList, BlendMode blendMode) const
{
	// nullptrチェック
	assert(commandList);

	// ルートシグネチャを設定する
	commandList->SetGraphicsRootSignature(rootSignature_.Get());

	// ブレンドモードに応じたPSOを設定する
	commandList->SetPipelineState(graphicsPipelineState_[static_cast<int32_t>(blendMode)].Get());
}