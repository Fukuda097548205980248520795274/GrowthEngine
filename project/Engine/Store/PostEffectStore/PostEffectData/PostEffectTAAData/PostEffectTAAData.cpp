#include "PostEffectTAAData.h"
#include "GrowthEngine.h"
#include "Resource/OffscreenResource/OffscreenResource.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "Parameter/PostEffectParameter/PostEffectParameter.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PostEffectTAAData::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
	DX12Heap* heap, DX12Buffering* buffering, BaseComputePSO* psoTAA, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(commandList);
	assert(heap);
	assert(buffering);
	assert(psoTAA);

	// 引数を受け取る
	psoTAA_ = psoTAA;

	// パラメータの生成
	param_ = std::make_unique<PostEffect::TAA>();
	param_->blendFactor = 0.25f;
	param_->gamma = 1.5f;

	// パラメータを記録する
	group_ = "TAA_" + name_;
	if (parameter_)
	{
		parameter_->SetValue(group_, "BlendFactor", &param_->blendFactor);
		parameter_->SetValue(group_, "Gamma", &param_->gamma);

		parameter_->RegisterGroupDataReflection(group_);
	}

	// リソース生成
	resource_ = std::make_unique<ConstantBufferResource<PostEffect::TAADataForGPU>>();
	resource_->Initialize(device, log);
	resource_->data_->blendFactor = param_->blendFactor;
	resource_->data_->gamma = param_->gamma;

	// TAA用の出力リソース生成
	outputTAAResource_ = std::make_unique<RWTexture2DBufferResource>();
	outputTAAResource_->Initialize(device, commandList, heap, buffering->GetSwapChainDesc().Width, buffering->GetSwapChainDesc().Height, log);

	// 前フレームのリソース生成
	prevFrameResource_ = std::make_unique<OffscreenResource>();
	prevFrameResource_->Initialize(device, heap,
		static_cast<int32_t>(buffering->GetSwapChainDesc().Width), static_cast<int32_t>(buffering->GetSwapChainDesc().Height), log);
	prevFrameResource_->Barrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

/// @brief リセット
void Engine::PostEffectTAAData::Reset()
{
	if (parameter_->IsFileFound(group_))
	{
		parameter_->RegisterGroupDataReflection(group_);
	}
	else
	{
		param_->blendFactor = 0.25f;
		param_->gamma = 1.5f;
	}
}

/// @brief リサイズ
/// @param device 
/// @param commandList 
/// @param width 
/// @param height 
void Engine::PostEffectTAAData::Resize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, int32_t width, int32_t height)
{
	// リソースのリサイズ
	outputTAAResource_->Resize(device, commandList, width, height);

	
	prevFrameResource_->Resize(device, width, height);
	prevFrameResource_->Barrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::PostEffectTAAData::Register(const PostEffectRenderContext& context)
{
	// データを受け取る
	MotionVectorTextureResource* motionVectorTextureResource = context.motionVectorTextureResource;
	OffscreenResource* offscreenPixelShaderResource = context.offscreenPixelShaderResource;
	OffscreenResource* offscreenRenderTargetResource = context.offscreenRenderTargetResource;
	ID3D12GraphicsCommandList* commandList = context.commandList;
	PSOCopyImage* psoCopyImage = context.psoCopyImage;

	// nullptrチェック
	assert(motionVectorTextureResource);
	assert(offscreenPixelShaderResource);
	assert(commandList);
	assert(psoCopyImage);

	// パラメータをGPU用のデータに転送
	resource_->data_->blendFactor = param_->blendFactor;
	resource_->data_->gamma = param_->gamma;


	/*---------------
		TAAの処理
	---------------*/

	// バリアを張る
	motionVectorTextureResource->Barrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	// PSOを登録
	psoTAA_->Register(commandList);

	// ピクセルシェーダに書くリソースをSRVとして登録する
	offscreenPixelShaderResource->RegisterCompute(commandList, 0);

	// TAA用の前のフレームのリソースをSRVとして登録する
	prevFrameResource_->RegisterCompute(commandList, 1);

	// TAA用のリソースをSRVとして登録する
	motionVectorTextureResource->RegisterComputeSRV(commandList, 2);

	// TAA用の出力リソースをUAVとして登録する
	outputTAAResource_->RegisterComputeUAV(commandList, 3);

	// 定数バッファを登録する
	resource_->RegisterCompute(commandList, 4);

	// ディスパッチ
	commandList->Dispatch((outputTAAResource_->GetWidth() + 7) / 8, (outputTAAResource_->GetHeight() + 7) / 8, 1);

	// バリアを張る
	motionVectorTextureResource->Barrier(commandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);


	/*------------------------------------------------------
	    出力したテクスチャをレンダーターゲットテクスチャに描画する
	------------------------------------------------------*/

	// バリアを張る
	outputTAAResource_->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// PSOを登録
	psoCopyImage->Register(commandList);

	// ピクセルシェーダに書くリソースをSRVとして登録する
	outputTAAResource_->RegisterGraphicsSRV(commandList, 0);

	// ドローコール
	commandList->DrawInstanced(3, 1, 0, 0);

	// バリアを張る
	outputTAAResource_->Barrier(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);


	/*----------------------------------------------------
	    描画したテクスチャを前のフレームのリソースにコピーする
	----------------------------------------------------*/
	
	// バリアを張る
	offscreenRenderTargetResource->Barrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE);
	prevFrameResource_->Barrier(commandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);

	// コピー
	commandList->CopyResource(prevFrameResource_->GetResource(), offscreenRenderTargetResource->GetResource());

	// バリアを張る
	prevFrameResource_->Barrier(commandList, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	offscreenRenderTargetResource->Barrier(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
}

/// @brief デバッグ用パラメータ
void Engine::PostEffectTAAData::DebugParameter()
{
#ifdef _DEVELOPMENT

	// UV
	if (ImGui::TreeNode((name_ + "_TAA").c_str()))
	{
		ImGui::SliderFloat("BlendFactor", &param_->blendFactor, 0.0f, 1.0f);
		ImGui::SliderFloat("Gamma", &param_->gamma, 0.0f, 5.0f);
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