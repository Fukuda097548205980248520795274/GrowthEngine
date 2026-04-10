#include "PostEffectDOFData.h"
#include "GrowthEngine.h"
#include "Store/Camera3DStore/Camera3DStore.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "Parameter/PostEffectParameter/PostEffectParameter.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PostEffectDOFData::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Buffering* buffering, DX12Heap* heap,
	BasePSOPostEffect* pso, BaseComputePSO* computePSO, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(pso);
	assert(computePSO);

	// 引数を受け取る
	pso_ = pso;
	psoGaussianBlur_ = computePSO;


	// パラメータの生成
	param_ = std::make_unique<PostEffect::DOF>();
	param_->focusDistance = 10.0f;
	param_->focusRange = 15.0f;
	param_->blurFalloff = 20.0f;

	// パラメータを記録する
	group_ = "Grayscale_" + name_;
	if (parameter_)
	{
		parameter_->SetValue(group_, "Focus_Distance", &param_->focusDistance);
		parameter_->SetValue(group_, "Focus_Range", &param_->focusRange);
		parameter_->SetValue(group_, "Blur_Falloff", &param_->blurFalloff);

		parameter_->RegisterGroupDataReflection(group_);
	}

	// リソース生成
	resource_ = std::make_unique<ConstantBufferResource<PostEffect::DOFDataForGPU>>();
	resource_->Initialize(device, log);
	resource_->data_->focusDistance = param_->focusDistance;
	resource_->data_->focusRange = param_->focusRange;
	resource_->data_->blurFalloff = param_->blurFalloff;

	// ブラーをかけるためのオフスクリーンリソース生成
	blurTextureResource_ = std::make_unique<RWTexture2DBufferResource>();
	blurTextureResource_->Initialize(device, commandList, heap, buffering->GetSwapChainDesc().Width, buffering->GetSwapChainDesc().Height, log);
}

/// @brief リセット
void Engine::PostEffectDOFData::Reset()
{
	if (parameter_->IsFileFound(group_))
	{
		parameter_->RegisterGroupDataReflection(group_);
	}
	else
	{
		param_->focusDistance = 10.0f;
		param_->focusRange = 15.0f;
		param_->blurFalloff = 20.0f;
	}
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::PostEffectDOFData::Register(const PostEffectRenderContext& context)
{
	ID3D12GraphicsCommandList* commandList = context.commandList;
	OffscreenResource* offscreenPixelShaderResource = context.offscreenPixelShaderResource;
	OffscreenResource* offscreenRenderTargetResource = context.offscreenRenderTargetResource;
	DepthResource* depthResource = context.depthResource;
	Camera3DStore* camera3DStore = context.camera3DStore;


	/*------------------------------
	    ガウシアンフィルターをかける
	------------------------------*/

	// オフスクリーンのテクスチャにバリアを張る 読み込み -> 書き込み
	TransitionBarrier(offscreenPixelShaderResource->GetResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,commandList);

	// ガウシアンフィルターのPSOをセット
	psoGaussianBlur_->Register(commandList);

	// テクスチャの設定
	offscreenPixelShaderResource->ComputeRegister(commandList, 0);

	// ブラー用テクスチャを設定
	blurTextureResource_->RegisterCompute(commandList, 1);

	// ディスパッチ
	commandList->Dispatch(blurTextureResource_->GetWidth() / 8, blurTextureResource_->GetHeight() / 8, 1);


	// ブラー用テクスチャにバリアを張る 書き込み -> 読み込み
	TransitionBarrier(blurTextureResource_->GetResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList);


	/*-----------------
		データを渡す
	-----------------*/

	resource_->data_->focusDistance = param_->focusDistance;
	resource_->data_->focusRange = param_->focusRange;
	resource_->data_->blurFalloff = param_->blurFalloff;
	resource_->data_->zNear = camera3DStore->GetCamera3D().GetParam()->setting.nearClip;
	resource_->data_->zFar = camera3DStore->GetCamera3D().GetParam()->setting.farClip;


	/*---------------------------------
		被写界深度のコマンドリストに登録
	---------------------------------*/

	// PSOの設定
	pso_->Register(commandList);

	// テクスチャの設定
	offscreenPixelShaderResource->Register(commandList, 0);

	// ブラー用オフスクリーンのテクスチャを設定
	blurTextureResource_->RegisterGraphics(commandList, 1);

	// 深度用テクスチャの設定
	depthResource->Register(commandList, 2);

	// パラメータの設定
	resource_->RegisterGraphics(commandList, 3);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawInstanced(3, 1, 0, 0);


	// ブラー用テクスチャにバリアを張る　読み込み -> 書き込み
	TransitionBarrier(blurTextureResource_->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, commandList);

	// オフスクリーンのテクスチャにバリアを張る 読み込み -> 書き込み
	TransitionBarrier(offscreenPixelShaderResource->GetResource(),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList);

}

/// @brief デバッグ用パラメータ
void Engine::PostEffectDOFData::DebugParameter()
{
#ifdef _DEVELOPMENT

	if (ImGui::TreeNode((name_ + "_Grayscale").c_str()))
	{
		ImGui::DragFloat("Focus Distance", &param_->focusDistance, 0.01f, 0.0f, 1000000.0f);
		ImGui::DragFloat("Focus Range", &param_->focusRange, 0.01f, 0.0f, 1000000.0f);
		ImGui::DragFloat("Blur Falloff", &param_->blurFalloff, 0.01f, 0.0f, 10000.0f);

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