#include "PostEffectRadialBlurData.h"
#include "GrowthEngine.h"
#include "PSO/PSOPostEffect/BasePSOPostEffect/BasePSOPostEffect.h"
#include "Resource/OffscreenResource/OffscreenResource.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PostEffectRadialBlurData::Initialize(ID3D12Device* device, Log* log, BasePSOPostEffect* pso)
{
	// nullptrチェック
	assert(device);
	assert(pso);

	// 引数を受け取る
	pso_ = pso;

	// パラメータの生成
	param_ = std::make_unique<PostEffect::RadialBlur>();
	param_->center = Vector2(0.0f, 0.0f);
	param_->sampleCount = 32;
	param_->blur = 1.0f;
	param_->saturation = 1.0f;
	param_->contrast = 1.0f;
	param_->brightness = 0.5f;

	// リソース生成
	resource_ = std::make_unique<ConstantBufferResource<PostEffect::RadialBlurDataForGPU>>();
	resource_->Initialize(device, log);
	resource_->data_->resolution =
		Vector2(static_cast<float>(GrowthEngine::GetInstance()->GetScreenWidth()), static_cast<float>(GrowthEngine::GetInstance()->GetScreenHeight()));
	resource_->data_->sampleCount = param_->sampleCount;
	resource_->data_->blur = param_->blur;
	resource_->data_->saturation = param_->saturation;
	resource_->data_->contrast = param_->contrast;
	resource_->data_->brightness = param_->brightness;
	resource_->data_->center = param_->center;
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::PostEffectRadialBlurData::Register(ID3D12GraphicsCommandList* commandList, OffscreenResource* offscreenResource)
{
	/*-----------------
	    データを渡す
	-----------------*/

	resource_->data_->sampleCount = param_->sampleCount;
	resource_->data_->blur = param_->blur;
	resource_->data_->saturation = param_->saturation;
	resource_->data_->contrast = param_->contrast;
	resource_->data_->brightness = param_->brightness;
	resource_->data_->center = param_->center;


	/*------------------------
	    コマンドリストに登録
	------------------------*/

	// PSOの設定
	pso_->Register(commandList);

	// テクスチャの設定
	offscreenResource->Register(commandList, 0);

	// パラメータの設定
	resource_->RegisterGraphics(commandList, 1);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawInstanced(3, 1, 0, 0);

}