#include "PostEffectDOFData.h"
#include "GrowthEngine.h"
#include "Store/Camera3DStore/Camera3DStore.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "Parameter/PostEffectParameter/PostEffectParameter.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PostEffectDOFData::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Buffering* buffering, DX12Heap* heap,
	BasePSOPostEffect* pso, BaseComputePSO* computePSO, BaseComputePSO* upsamplePSO, BaseComputePSO* downsamplePSO, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(pso);
	assert(buffering);
	assert(computePSO);
	assert(upsamplePSO);
	assert(downsamplePSO);

	// 引数を受け取る
	pso_ = pso;
	upsamplePSO_ = upsamplePSO;
	downsamplePSO_ = downsamplePSO;


	// パラメータの生成
	param_ = std::make_unique<PostEffect::DOF>();
	param_->focusDistance = 10.0f;
	param_->focusRange = 15.0f;
	param_->blurFalloff = 20.0f;

	// パラメータを記録する
	group_ = "DOF_" + name_;
	if (parameter_)
	{
		parameter_->SetValue(group_, "Focus_Distance", &param_->focusDistance);
		parameter_->SetValue(group_, "Focus_Range", &param_->focusRange);
		parameter_->SetValue(group_, "Blur_Falloff", &param_->blurFalloff);

		parameter_->RegisterGroupDataReflection(group_);
	}

	// スワップチェーンの幅と高さを取得
	uint32_t width = buffering->GetSwapChainDesc().Width;
	uint32_t height = buffering->GetSwapChainDesc().Height;

	// リソース生成
	resource_ = std::make_unique<ConstantBufferResource<PostEffect::DOFDataForGPU>>();
	resource_->Initialize(device, log);
	resource_->data_->focusDistance = param_->focusDistance;
	resource_->data_->focusRange = param_->focusRange;
	resource_->data_->blurFalloff = param_->blurFalloff;


	// デュアルブラー用のオフスクリーンリソース生成

	// デュアルブラーは幅と高さが半分のテクスチャを複数用意して、順番に縮小サンプルと拡大サンプルをかけていく
	while (width >= 30 && height >= 30)
	{
		std::unique_ptr<RWTexture2DBufferResource> dualBlurTextureResource = std::make_unique<RWTexture2DBufferResource>();
		dualBlurTextureResource->Initialize(device, commandList, heap, width, height, log);
		dualBlurTextureResources_.push_back(std::move(dualBlurTextureResource));

		// 幅と高さを半分にする
		width /= 2;
		height /= 2;
	}
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


	/*------------------------
	    縮小サンプルをかける
	------------------------*/

	// オフスクリーンのテクスチャにバリアを張る PixelShader書き込み -> ComputeShader書き込み
	TransitionBarrier(offscreenPixelShaderResource->GetResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, commandList);

	// PSOの設定
	downsamplePSO_->Register(commandList);

	// ブラー用のテクスチャを設定
	offscreenPixelShaderResource->ComputeRegister(commandList, 0);

	// ブラー用のテクスチャを設定
	dualBlurTextureResources_[0]->RegisterComputeUAV(commandList, 1);

	// ディスパッチ
	commandList->Dispatch(dualBlurTextureResources_[0]->GetWidth() / 8, dualBlurTextureResources_[0]->GetHeight() / 8, 1);

	// オフスクリーンのテクスチャにバリアを張る PixelShader書き込み -> ComputeShader書き込み
	TransitionBarrier(offscreenPixelShaderResource->GetResource(),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList);



	for (int i = 1; i < static_cast<int32_t>(dualBlurTextureResources_.size()); i++)
	{
		// ブラー用テクスチャにバリアを張る 書き込み -> Compute読み込み
		TransitionBarrier(dualBlurTextureResources_[i - 1]->GetResource(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, commandList);

		// ブラー用のテクスチャを設定
		dualBlurTextureResources_[i - 1]->RegisterComputeSRV(commandList, 0);

		// ブラー用のテクスチャを設定
		dualBlurTextureResources_[i]->RegisterComputeUAV(commandList, 1);

		// ディスパッチ
		commandList->Dispatch(dualBlurTextureResources_[i]->GetWidth() / 8, dualBlurTextureResources_[i]->GetHeight() / 8, 1);

		// ブラー用テクスチャにバリアを張る 書き込み -> Compute読み込み
		TransitionBarrier(dualBlurTextureResources_[i - 1]->GetResource(),
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, commandList);

		// 最後のループならば、拡大サンプルで使うためにバリアを張る
		if (i == static_cast<int32_t>(dualBlurTextureResources_.size()) - 1)
		{
			// ブラー用テクスチャにバリアを張る 書き込み -> Compute読み込み
			TransitionBarrier(dualBlurTextureResources_[i]->GetResource(),
				D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, commandList);
		}
		
	}


	/*-------------------------
	    拡大サンプルをかける
	-------------------------*/

	// PSOの設定
	upsamplePSO_->Register(commandList);

	for (int i = static_cast<int32_t>(dualBlurTextureResources_.size()) - 1; i > 0; --i)
	{


		// ブラー用のテクスチャを設定
		dualBlurTextureResources_[i]->RegisterComputeSRV(commandList, 0);

		// ブラー用のテクスチャを設定
		dualBlurTextureResources_[i - 1]->RegisterComputeUAV(commandList, 1);

		// ディスパッチ
		commandList->Dispatch(dualBlurTextureResources_[i - 1]->GetWidth() / 8, dualBlurTextureResources_[i - 1]->GetHeight() / 8, 1);

		// ブラー用テクスチャにバリアを張る 書き込み -> Compute読み込み
		TransitionBarrier(dualBlurTextureResources_[i]->GetResource(),
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, commandList);


		// 次のブラー用テクスチャにバリアを張る 書き込み -> Compute読み込み
		if (i - 1 != 0)
		{
			TransitionBarrier(dualBlurTextureResources_[i - 1]->GetResource(),
				D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, commandList);
		}
		else
		{
			// 次のブラー用テクスチャにバリアを張る 書き込み -> 読み込み
			TransitionBarrier(dualBlurTextureResources_[i - 1]->GetResource(),
				D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList);
		}
	}


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
	dualBlurTextureResources_[0]->RegisterGraphicsSRV(commandList, 1);

	// 深度用テクスチャの設定
	depthResource->Register(commandList, 2);

	// パラメータの設定
	resource_->RegisterGraphics(commandList, 3);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawInstanced(3, 1, 0, 0);


	// ブラー用テクスチャにバリアを張る　読み込み -> 書き込み
	TransitionBarrier(dualBlurTextureResources_[0]->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, commandList);

}

/// @brief デバッグ用パラメータ
void Engine::PostEffectDOFData::DebugParameter()
{
#ifdef _DEVELOPMENT

	if (ImGui::TreeNode((name_ + "_DOF").c_str()))
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