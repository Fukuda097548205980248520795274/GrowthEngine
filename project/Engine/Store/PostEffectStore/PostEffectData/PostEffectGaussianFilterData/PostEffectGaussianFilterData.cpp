#include "PostEffectGaussianFilterData.h"
#include "GrowthEngine.h"
#include "Resource/OffscreenResource/OffscreenResource.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "Parameter/PostEffectParameter/PostEffectParameter.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PostEffectGaussianFilterData::Initialize(ID3D12Device* device, Log* log, BasePSOPostEffect* pso)
{
	// nullptrチェック
	assert(device);
	assert(pso);

	// 引数を受け取る
	pso_ = pso;


	// パラメータの生成
	param_ = std::make_unique<PostEffect::GaussianFilter>();
	param_->sigma = 1.0f;
	param_->radius = 2;

	// パラメータを記録する
	group_ = "GaussianFilter_" + name_;
	if (parameter_)
	{
		parameter_->SetValue(group_, "Sigma", &param_->sigma);
		parameter_->SetValue(group_, "Radius", &param_->radius);

		parameter_->RegisterGroupDataReflection(group_);
	}

	// 水平方向のガウスフィルタ用の定数バッファリソースを生成
	horizontalResource_ = std::make_unique<ConstantBufferResource<PostEffect::GaussianFilterDataForGPU>>();
	horizontalResource_->Initialize(device, log);
	horizontalResource_->data_->blurDir = Vector2(1.0f / static_cast<float>(GrowthEngine::GetInstance()->GetScreenWidth()), 0.0f);
	horizontalResource_->data_->sigma = param_->sigma;
	horizontalResource_->data_->radius = param_->radius;

	// 垂直方向のガウスフィルタ用の定数バッファリソースを生成
	verticalResource_ = std::make_unique<ConstantBufferResource<PostEffect::GaussianFilterDataForGPU>>();
	verticalResource_->Initialize(device, log);
	verticalResource_->data_->blurDir = Vector2(0.0f, 1.0f / static_cast<float>(GrowthEngine::GetInstance()->GetScreenHeight()));
	verticalResource_->data_->sigma = param_->sigma;
	verticalResource_->data_->radius = param_->radius;
}

/// @brief リセット
void Engine::PostEffectGaussianFilterData::Reset()
{
	if (parameter_->IsFileFound(group_))
	{
		parameter_->RegisterGroupDataReflection(group_);
	}
	else
	{
		param_->sigma = 1.0f;
		param_->radius = 2;
	}
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::PostEffectGaussianFilterData::Register(const PostEffectRenderContext& context)
{
    ID3D12GraphicsCommandList* commandList = context.commandList;
	OffscreenResource* offscreenPixelShaderResource = context.offscreenPixelShaderResource;
	OffscreenResource* offscreenRenderTargetResource = context.offscreenRenderTargetResource;
	DepthResource* depthResource = context.depthResource;

	assert(commandList);
	assert(offscreenPixelShaderResource);
	assert(offscreenRenderTargetResource);
	assert(depthResource);

	horizontalResource_->data_->blurDir = Vector2(1.0f / static_cast<float>(GrowthEngine::GetInstance()->GetScreenWidth()), 0.0f);
	horizontalResource_->data_->sigma = param_->sigma;
	horizontalResource_->data_->radius = param_->radius;

	verticalResource_->data_->blurDir = Vector2(0.0f, 1.0f / static_cast<float>(GrowthEngine::GetInstance()->GetScreenHeight()));
	verticalResource_->data_->sigma = param_->sigma;
	verticalResource_->data_->radius = param_->radius;


	/*--------------------------
		水平方向のガウスフィルタ
	--------------------------*/

	// PSOの設定
	pso_->Register(commandList);

	// テクスチャの設定
	offscreenPixelShaderResource->RegisterGraphics(commandList, 0);

	// 水平方向のガウスフィルタ用の定数バッファリソースを設定
	horizontalResource_->RegisterGraphics(commandList, 1);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawInstanced(3, 1, 0, 0);


	// リソースのバリアを張る
	offscreenPixelShaderResource->Barrier(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	offscreenPixelShaderResource->ClearRenderTarget(commandList, depthResource->GetDsvCpuHandle());
	offscreenRenderTargetResource->Barrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// リソースを入れ替える
	auto temp = offscreenPixelShaderResource;
	offscreenPixelShaderResource = offscreenRenderTargetResource;
	offscreenRenderTargetResource = temp;


	/*---------------------------
		垂直方向のガウスフィルタ
	---------------------------*/

	// テクスチャの設定
	offscreenPixelShaderResource->RegisterGraphics(commandList, 0);

	// 垂直方向のガウスフィルタ用の定数バッファリソースを設定
	verticalResource_->RegisterGraphics(commandList, 1);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawInstanced(3, 1, 0, 0);
}

/// @brief デバッグ用パラメータ
void Engine::PostEffectGaussianFilterData::DebugParameter()
{
#ifdef DEVELOPMENT

	if (ImGui::TreeNode((name_ + "_GaussianFilter").c_str()))
	{
		ImGui::DragFloat("Sigma", &param_->sigma, 0.01f, 0.01f, 10.0f);
		ImGui::DragInt("Radius", &param_->radius, 1, 0, 10);

		ImGui::Text("\n");

		// 保存ボタン
		if (ImGui::Button("Save"))
		{
			parameter_->SaveFile(group_);
			std::string message = std::format("{} : saved.", group_);
			MessageBoxA(nullptr, message.c_str(), "RecordSetting", 0);
		}

		ImGui::Text("\n");

		// ロードボタン
		if (ImGui::Button("Load"))
		{
			parameter_->RegisterGroupDataReflection(group_);
			std::string message = std::format("{} : loaded.", group_);
			MessageBoxA(nullptr, message.c_str(), "RecordSetting", 0);
		}

		ImGui::Text("\n");

		// 終了
		ImGui::TreePop();
	}

#endif
}