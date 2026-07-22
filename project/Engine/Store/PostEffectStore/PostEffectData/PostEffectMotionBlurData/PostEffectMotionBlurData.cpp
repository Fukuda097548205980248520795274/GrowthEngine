#include "PostEffectMotionBlurData.h"
#include "GrowthEngine.h"
#include "Resource/OffscreenResource/OffscreenResource.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "Parameter/PostEffectParameter/PostEffectParameter.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PostEffectMotionBlurData::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Heap* heap, DX12Buffering* buffering,
	BaseComputePSO* psoMotionBlur, BaseComputePSO* psoVelocityDilation, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(commandList);
	assert(heap);
	assert(buffering);
	assert(psoMotionBlur);
	assert(psoVelocityDilation);

	// 引数を受け取る
	psoMotionBlur_ = psoMotionBlur;
	psoVelocityDilation_ = psoVelocityDilation;

	// パラメータの生成
	param_ = std::make_unique<PostEffect::MotionBlur>();
	param_->numSamples = 10;
	param_->blurScale = 3.0f;

	// パラメータを記録する
	group_ = "MotionBlur_" + name_;
	if (parameter_)
	{
		parameter_->SetValue(group_, "NumSamples", &param_->numSamples);
		parameter_->SetValue(group_, "BlurScale", &param_->blurScale);

		parameter_->RegisterGroupDataReflection(group_);
	}

	// リソース生成
	resource_ = std::make_unique<ConstantBufferResource<PostEffect::MotionBlurDataForGPU>>();
	resource_->Initialize(device, log);
	resource_->data_->numSamples = param_->numSamples;
	resource_->data_->blurScale = param_->blurScale;

	// 速度膨張用のリソース生成
	velocityDilationResource_ = std::make_unique<RWTexture2DBufferResource>();
	velocityDilationResource_->Initialize(device, commandList, heap, buffering->GetSwapChainDesc().Width, buffering->GetSwapChainDesc().Height, log);

	// 出力リソース生成
	outputResource_ = std::make_unique<RWTexture2DBufferResource>();
	outputResource_->Initialize(device, commandList, heap, buffering->GetSwapChainDesc().Width, buffering->GetSwapChainDesc().Height, log);
}

/// @brief リセット
void Engine::PostEffectMotionBlurData::Reset()
{
	if (parameter_->IsFileFound(group_))
	{
		parameter_->RegisterGroupDataReflection(group_);
	}
	else
	{
		param_->numSamples = 10;
		param_->blurScale = 3.0f;
	}
}

/// @brief リサイズ
/// @param device 
/// @param commandList 
/// @param width 
/// @param height 
void Engine::PostEffectMotionBlurData::Resize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, int32_t width, int32_t height)
{
	// リソースのリサイズ
	outputResource_->Resize(device, commandList, width, height);
	velocityDilationResource_->Resize(device, commandList, width, height);
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::PostEffectMotionBlurData::Register(const PostEffectRenderContext& context)
{
	// データを受け取る
	MotionVectorTextureResource* motionVectorTextureResource = context.motionVectorTextureResource;
	OffscreenResource* offscreenPixelShaderResource = context.offscreenPixelShaderResource;
	ID3D12GraphicsCommandList* commandList = context.commandList;
	PSOFullscreen* psoFullscreen = context.psoFullscreen;

	// nullptrチェック
	assert(motionVectorTextureResource);
	assert(offscreenPixelShaderResource);
	assert(commandList);
	assert(psoFullscreen);

	// パラメータをGPU用のデータに転送
	resource_->data_->numSamples = param_->numSamples;
	resource_->data_->blurScale = param_->blurScale;


	/*---------------
	    速度膨張
	---------------*/

	// バリアを張る
	motionVectorTextureResource->Barrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	// PSOを登録
	psoVelocityDilation_->Register(commandList);

	// ピクセルシェーダに書くリソースをSRVとして登録する
	motionVectorTextureResource->RegisterComputeSRV(commandList, 0);

	// TAA用の出力リソースをUAVとして登録する
	velocityDilationResource_->RegisterComputeUAV(commandList, 1);

	commandList->Dispatch((velocityDilationResource_->GetWidth() + 7) / 8, (velocityDilationResource_->GetHeight() + 7) / 8, 1);

	// バリアを張る
	motionVectorTextureResource->Barrier(commandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);


	/*---------------
		TAAの処理
	---------------*/

	// バリアを張る
	velocityDilationResource_->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	// PSOを登録
	psoMotionBlur_->Register(commandList);

	// ピクセルシェーダに書くリソースをSRVとして登録する
	offscreenPixelShaderResource->RegisterCompute(commandList, 0);

	// TAA用のリソースをSRVとして登録する
	velocityDilationResource_->RegisterComputeSRV(commandList, 1);

	// TAA用の出力リソースをUAVとして登録する
	outputResource_->RegisterComputeUAV(commandList, 2);

	// 定数バッファを登録する
	resource_->RegisterCompute(commandList, 3);

	// ディスパッチ
	commandList->Dispatch((outputResource_->GetWidth() + 7) / 8, (outputResource_->GetHeight() + 7) / 8, 1);

	// バリアを張る
	velocityDilationResource_->Barrier(commandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);


	/*------------------------------------------------------
		出力したテクスチャをレンダーターゲットテクスチャに描画する
	------------------------------------------------------*/

	// バリアを張る
	outputResource_->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// PSOを登録
	psoFullscreen->Register(commandList, BlendMode::kNone);

	// ピクセルシェーダに書くリソースをSRVとして登録する
	outputResource_->RegisterGraphicsSRV(commandList, 0);

	// ドローコール
	commandList->DrawInstanced(3, 1, 0, 0);

	// バリアを張る
	outputResource_->Barrier(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

/// @brief デバッグ用パラメータ
void Engine::PostEffectMotionBlurData::DebugParameter()
{
#ifdef DEVELOPMENT

	// UV
	if (ImGui::TreeNode((name_ + "_MotionBlur").c_str()))
	{
		// サンプル数
		int numSamples = param_->numSamples;
		ImGui::DragInt("NumSamples", &numSamples, 1, 1, 100);
		param_->numSamples = numSamples;

		// ブラーの強さ
		ImGui::DragFloat("BlurScale", &param_->blurScale, 0.1f, 0.0f, 50.0f);

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