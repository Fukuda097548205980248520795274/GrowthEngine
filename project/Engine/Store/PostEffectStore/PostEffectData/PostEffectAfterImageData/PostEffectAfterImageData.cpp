#include "PostEffectAfterImageData.h"
#include "GrowthEngine.h"
#include "Resource/OffscreenResource/OffscreenResource.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "Parameter/PostEffectParameter/PostEffectParameter.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PostEffectAfterImageData::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
	DX12Heap* heap, DX12Buffering* buffering, BaseComputePSO* psoAfterImage, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(commandList);
	assert(heap);
	assert(buffering);
	assert(psoAfterImage);

	// 引数を受け取る
	psoAfterImage_ = psoAfterImage;

	// パラメータの生成
	param_ = std::make_unique<PostEffect::AfterImage>();
	param_->decay = 0.9f;
	param_->intensity = 0.5f;
	param_->tintColor = Vector3(1.0f, 1.0f, 1.0f);

	// パラメータを記録する
	group_ = "AfterImage_" + name_;
	if (parameter_)
	{
		parameter_->SetValue(group_, "Decay", &param_->decay);
		parameter_->SetValue(group_, "Intensity", &param_->intensity);
		parameter_->SetValue(group_, "TintColor", &param_->tintColor);

		parameter_->RegisterGroupDataReflection(group_);
	}

	// リソース生成
	resource_ = std::make_unique<ConstantBufferResource<PostEffect::AfterImageDataForGPU>>();
	resource_->Initialize(device, log);
	resource_->data_->decay = param_->decay;
	resource_->data_->intensity = param_->intensity;
	resource_->data_->color = param_->tintColor;
	resource_->data_->InvCurrentViewProjection = MakeIdentityMatrix4x4();
	resource_->data_->PrevViewProjection = MakeIdentityMatrix4x4();

	// 前フレームの残像用リソース生成
	prevFrameOffscreenResource_ = std::make_unique<OffscreenResource>();
	prevFrameOffscreenResource_->Initialize(device, buffering, heap, log);
	prevFrameOffscreenResource_->Barrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	// 出力リソース生成
	outputResource_ = std::make_unique<RWTexture2DBufferResource>();
	outputResource_->Initialize(device, commandList, heap, buffering->GetSwapChainDesc().Width, buffering->GetSwapChainDesc().Height, log);

	// 前フレームの残像出力リソース生成
	prevAfterImageOutputResource_ = std::make_unique<RWTexture2DBufferResource>();
	prevAfterImageOutputResource_->Initialize(device, commandList, heap, buffering->GetSwapChainDesc().Width, buffering->GetSwapChainDesc().Height, log);
}

/// @brief リセット
void Engine::PostEffectAfterImageData::Reset()
{
	if (parameter_->IsFileFound(group_))
	{
		parameter_->RegisterGroupDataReflection(group_);
	}
	else
	{
		param_->decay = 0.9f;
		param_->intensity = 0.5f;
		param_->tintColor = Vector3(1.0f, 1.0f, 1.0f);
	}
}

/// @brief リサイズ
/// @param device 
/// @param commandList 
/// @param width 
/// @param height 
void Engine::PostEffectAfterImageData::Resize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, int32_t width, int32_t height)
{
	// リソースのリサイズ
	outputResource_->Resize(device, commandList, width, height);
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::PostEffectAfterImageData::Register(const PostEffectRenderContext& context)
{
	// データを受け取る
	MotionVectorTextureResource* motionVectorTextureResource = context.motionVectorTextureResource;
	OffscreenResource* offscreenPixelShaderResource = context.offscreenPixelShaderResource;
	OffscreenResource* offscreenRenderTargetResource = context.offscreenRenderTargetResource;
	ID3D12GraphicsCommandList* commandList = context.commandList;
	PSOCopyImage* psoCopyImage = context.psoCopyImage;
	DepthResource* depthResource = context.depthResource;
	Camera3DStore* camera3DStore = context.camera3DStore;

	// nullptrチェック
	assert(motionVectorTextureResource);
	assert(offscreenPixelShaderResource);
	assert(commandList);
	assert(psoCopyImage);
	assert(depthResource);

	// パラメータをGPU用のデータに転送
	resource_->data_->decay = param_->decay;
	resource_->data_->intensity = param_->intensity;
	resource_->data_->color = param_->tintColor;
	resource_->data_->InvCurrentViewProjection = camera3DStore->GetCamera3D().GetCurrentVPUnJitterMatrix().Inverse();
	resource_->data_->PrevViewProjection = camera3DStore->GetCamera3D().GetPrevVPUnJitterMatrix();


	/*----------------------
		AfterImageの処理
	----------------------*/

	// バリアを張る
	motionVectorTextureResource->Barrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	depthResource->Barrier(commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	// PSOを登録
	psoAfterImage_->Register(commandList);

	// ピクセルシェーダに書くリソースをSRVとして登録する
	offscreenPixelShaderResource->RegisterCompute(commandList, 0);

	// AfterImage用のリソースをSRVとして登録する
	motionVectorTextureResource->RegisterComputeSRV(commandList, 1);

	// 前のフレームの残像用リソースをSRVとして登録する
	prevFrameOffscreenResource_->RegisterCompute(commandList, 2);

	// 深度リソースをSRVとして登録する
	depthResource->RegisterComputeSRV(commandList, 3);

	// AfterImage用の出力リソースをUAVとして登録する
	outputResource_->RegisterComputeUAV(commandList, 4);

	// AfterImage用の前のフレームの出力リソースをUAVとして登録する
	prevAfterImageOutputResource_->RegisterComputeUAV(commandList, 5);

	// 定数バッファを登録する
	resource_->RegisterCompute(commandList, 6);

	// ディスパッチ
	commandList->Dispatch((outputResource_->GetWidth() + 7) / 8, (outputResource_->GetHeight() + 7) / 8, 1);

	// バリアを張る
	depthResource->Barrier(commandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	motionVectorTextureResource->Barrier(commandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);


	/*---------------------------------------------------
		残像の履歴を前のフレームの残像用リソースにコピーする
	---------------------------------------------------*/

	// バリアを張る
	prevAfterImageOutputResource_->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	prevFrameOffscreenResource_->Barrier(commandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// レンダーターゲットの設定
	prevFrameOffscreenResource_->ClearRenderTarget(commandList, context.depthResource->GetDsvCpuHandle());

	// PSOを登録
	psoCopyImage->Register(commandList);

	// ピクセルシェーダに書くリソースをSRVとして登録する
	prevAfterImageOutputResource_->RegisterGraphicsSRV(commandList, 0);

	// ドローコール
	commandList->DrawInstanced(3, 1, 0, 0);

	// バリアを張る
	prevFrameOffscreenResource_->Barrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	prevAfterImageOutputResource_->Barrier(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);


	/*------------------------------------------------------
		出力したテクスチャをレンダーターゲットテクスチャに描画する
	------------------------------------------------------*/

	// バリアを張る
	outputResource_->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// レンダーターゲットと深度ステンシルの設定
	offscreenRenderTargetResource->ClearRenderTarget(commandList, context.depthResource->GetDsvCpuHandle());

	// PSOを登録
	psoCopyImage->Register(commandList);

	// ピクセルシェーダに書くリソースをSRVとして登録する
	outputResource_->RegisterGraphicsSRV(commandList, 0);

	// ドローコール
	commandList->DrawInstanced(3, 1, 0, 0);

	// バリアを張る
	outputResource_->Barrier(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

/// @brief デバッグ用パラメータ
void Engine::PostEffectAfterImageData::DebugParameter()
{
#ifdef _DEVELOPMENT

	// UV
	if (ImGui::TreeNode((name_ + "_AfterImage").c_str()))
	{
		ImGui::DragFloat("Decay", &param_->decay, 0.001f, 0.0f, 20.0f);
		ImGui::DragFloat("Intensity", &param_->intensity, 0.001f, 0.0f, 20.0f);
		ImGui::ColorEdit3("TintColor", &param_->tintColor.x);

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