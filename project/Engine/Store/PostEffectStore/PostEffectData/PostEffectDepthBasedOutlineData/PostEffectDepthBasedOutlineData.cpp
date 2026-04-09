#include "PostEffectDepthBasedOutlineData.h"
#include "GrowthEngine.h"
#include "Resource/OffscreenResource/OffscreenResource.h"
#include "Resource/DepthResource/DepthResource.h"
#include "Store/Camera3DStore/Camera3DStore.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "Parameter/PostEffectParameter/PostEffectParameter.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PostEffectDepthBasedOutlineData::Initialize(ID3D12Device* device, Log* log, BasePSOPostEffect* pso)
{
	// nullptrチェック
	assert(device);
	assert(pso);

	// 引数を受け取る
	pso_ = pso;

	// リソース生成
	resource_ = std::make_unique<ConstantBufferResource<PostEffect::DepthBasedOutlineDataForGPU>>();
	resource_->Initialize(device, log);
}

/// @brief リセット
void Engine::PostEffectDepthBasedOutlineData::Reset()
{
	if (parameter_->IsFileFound(group_))
	{
		parameter_->RegisterGroupDataReflection(group_);
	}
	else
	{
		
	}
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::PostEffectDepthBasedOutlineData::Register(const PostEffectRenderContext& context)
{
	ID3D12GraphicsCommandList* commandList = context.commandList;
	OffscreenResource* offscreenPixelShaderResource = context.offscreenPixelShaderResource;
	DepthResource* depthResource = context.depthResource;
	Camera3DStore* camera3DStore = context.camera3DStore;

	assert(camera3DStore);
	assert(depthResource);

	/*-----------------
		データを渡す
	-----------------*/

	resource_->data_->projectionInverse = camera3DStore->GetCamera3D().GetProjectionMatrix().Inverse();


	/*------------------------
		コマンドリストに登録
	------------------------*/

	// PSOの設定
	pso_->Register(commandList);

	// テクスチャの設定
	offscreenPixelShaderResource->Register(commandList, 0);

	// パラメータの設定
	resource_->RegisterGraphics(commandList, 1);

	// 深度テクスチャの設定
	depthResource->Register(commandList, 2);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawInstanced(3, 1, 0, 0);

}

/// @brief デバッグ用パラメータ
void Engine::PostEffectDepthBasedOutlineData::DebugParameter()
{
#ifdef _DEVELOPMENT

#endif
}