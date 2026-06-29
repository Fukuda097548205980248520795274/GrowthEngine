#include "PostEffectBloomData.h"
#include "GrowthEngine.h"
#include "Store/Camera3DStore/Camera3DStore.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "Parameter/PostEffectParameter/PostEffectParameter.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PostEffectBloomData::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Buffering* buffering, DX12Heap* heap,
	BaseComputePSO* highLuminanceExtractionPSO, BaseComputePSO* upsamplePSO, BaseComputePSO* downsamplePSO, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(buffering);
	assert(highLuminanceExtractionPSO);
	assert(upsamplePSO);
	assert(downsamplePSO);

	// 引数を受け取る
	upsamplePSO_ = upsamplePSO;
	downsamplePSO_ = downsamplePSO;
	highLuminanceExtractionPSO_ = highLuminanceExtractionPSO;


	// パラメータの生成
	param_ = std::make_unique<PostEffect::Bloom>();
	param_->threshold = 1.0f;
	param_->knee = 0.5f;

	// パラメータを記録する
	group_ = "Bloom_" + name_;
	if (parameter_)
	{
		parameter_->SetValue(group_, "Threshold", &param_->threshold);
		parameter_->SetValue(group_, "Knee", &param_->knee);

		parameter_->RegisterGroupDataReflection(group_);
	}


	// リソース生成
	resource_ = std::make_unique<ConstantBufferResource<PostEffect::HighLuminanceExtractionDataForGPU>>();
	resource_->Initialize(device, log);
	resource_->data_->threshold = param_->threshold;
	resource_->data_->knee = param_->knee;


	// スワップチェーンの幅と高さを取得
	width_ = buffering->GetSwapChainDesc().Width;
	height_ = buffering->GetSwapChainDesc().Height;

	// デュアルブラー用のテクスチャの幅と高さをスワップチェーンの幅と高さの半分にする
	int width = width_;
	int height = height_;

	// デュアルブラーは幅と高さが半分のテクスチャを複数用意して、順番に縮小サンプルと拡大サンプルをかけていく
	while (width >= 30 && height >= 30)
	{
		std::unique_ptr<RWTexture2DBufferResource> dualBlurTextureResource = std::make_unique<RWTexture2DBufferResource>();
		dualBlurTextureResource->Initialize(device, commandList, heap, width, height, log);
		dualBlurTextureResources_.push_back(std::move(dualBlurTextureResource));

		// 幅と高さを半分にする
		width /= 2;
		height /= 2;

		// レベル数を増やす
		numDualBlurLevels_++;
	}
}

/// @brief リセット
void Engine::PostEffectBloomData::Reset()
{
	if (parameter_->IsFileFound(group_))
	{
		parameter_->RegisterGroupDataReflection(group_);
	}
	else
	{
		param_->threshold = 1.0f;
		param_->knee = 0.5f;
	}
}

/// @brief リサイズ
/// @param width 
/// @param height 
void Engine::PostEffectBloomData::Resize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, int32_t width, int32_t height)
{
	// nullptrチェック
	assert(device);
	assert(commandList);

	// 引数を受け取る
	width_ = width;
	height_ = height;

	// レベル数を初期化
	numDualBlurLevels_ = 0;

	for (int32_t i = 0; i < static_cast<int32_t>(dualBlurTextureResources_.size()); ++i)
	{
		// テクスチャのサイズをリサイズ
		dualBlurTextureResources_[i]->Resize(device, commandList, width, height);

		// 幅と高さが30以上ならば、デュアルブラーをかける必要があるため、レベル数を増やす
		if (width >= 30 && height >= 30)
			numDualBlurLevels_++;

		// テクスチャのサイズを半分にする
		width /= 2;
		height /= 2;
	}
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::PostEffectBloomData::Register(const PostEffectRenderContext& context)
{
	// サンプルのレベル数が2以下ならば、デュアルブラーをかける必要がないため、処理を抜ける
	if (numDualBlurLevels_ <= 2)
		return;


	ID3D12GraphicsCommandList* commandList = context.commandList;
	OffscreenResource* offscreenRenderTargetResource = context.offscreenRenderTargetResource;
	PSOFullscreen* psoFullscreen = context.psoFullscreen;

	assert(commandList);
	assert(offscreenRenderTargetResource);
	assert(psoFullscreen);
	

	/*-----------------
		データを渡す
	-----------------*/

	resource_->data_->threshold = param_->threshold;
	resource_->data_->knee = param_->knee;


	/*-------------------
	    高輝度抽出を行う
	--------------------*/

	// オフスクリーンのテクスチャにバリアを張る 読み込み -> ComputeShader書き込み
	TransitionBarrier(offscreenRenderTargetResource->GetResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, commandList);

	// PSOの設定
	highLuminanceExtractionPSO_->Register(commandList);

	// 読み込みテクスチャを設定
	offscreenRenderTargetResource->RegisterCompute(commandList, 0);

	// 書き込みテクスチャを設定
	dualBlurTextureResources_[0]->RegisterComputeUAV(commandList, 1);

	// パラメータを設定
	resource_->RegisterCompute(commandList, 2);

	// ディスパッチ
	commandList->Dispatch((dualBlurTextureResources_[0]->GetWidth() + 7) / 8, (dualBlurTextureResources_[0]->GetHeight() + 7) / 8, 1);

	// オフスクリーンのテクスチャにバリアを張る ComputeShader書き込み -> PixelShader書き込み
	TransitionBarrier(offscreenRenderTargetResource->GetResource(),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET, commandList);


	/*----------------------
	    縮小サンプルを行う
	----------------------*/

	// PSOの設定
	downsamplePSO_->Register(commandList);

	for (int i = 1; i < numDualBlurLevels_; i++)
	{
		// ブラー用テクスチャにバリアを張る 書き込み -> Compute読み込み
		TransitionBarrier(dualBlurTextureResources_[i - 1]->GetResource(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, commandList);

		// ブラー用のテクスチャを設定
		dualBlurTextureResources_[i - 1]->RegisterComputeSRV(commandList, 0);

		// ブラー用のテクスチャを設定
		dualBlurTextureResources_[i]->RegisterComputeUAV(commandList, 1);

		// ディスパッチ
		UINT dispatchX = (dualBlurTextureResources_[i]->GetWidth() + 7) / 8;
		UINT dispatchY = (dualBlurTextureResources_[i]->GetHeight() + 7) / 8;
		commandList->Dispatch(dispatchX, dispatchY, 1);

		// ブラー用テクスチャにバリアを張る 書き込み -> Compute読み込み
		TransitionBarrier(dualBlurTextureResources_[i - 1]->GetResource(),
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, commandList);

		// 最後のループならば、拡大サンプルで使うためにバリアを張る
		if (i == numDualBlurLevels_ - 1)
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

	for (int i = numDualBlurLevels_ - 1; i > 0; --i)
	{
		// ブラー用のテクスチャを設定
		dualBlurTextureResources_[i]->RegisterComputeSRV(commandList, 0);

		// ブラー用のテクスチャを設定
		dualBlurTextureResources_[i - 1]->RegisterComputeUAV(commandList, 1);

		// ディスパッチ
		UINT dispatchX = (dualBlurTextureResources_[i - 1]->GetWidth() + 7) / 8;
		UINT dispatchY = (dualBlurTextureResources_[i - 1]->GetHeight() + 7) / 8;
		commandList->Dispatch(dispatchX, dispatchY, 1);

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


	/*---------------------------------
		元画像に加算のコマンドリストに登録
	---------------------------------*/

	// PSOの設定
	psoFullscreen->Register(commandList, BlendMode::kAdd);

	// ブラー用オフスクリーンのテクスチャを設定
	dualBlurTextureResources_[0]->RegisterGraphicsSRV(commandList, 0);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawInstanced(3, 1, 0, 0);


	// ブラー用テクスチャにバリアを張る　読み込み -> 書き込み
	TransitionBarrier(dualBlurTextureResources_[0]->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, commandList);

	// オフスクリーンのテクスチャにバリアを張る ComputeShader書き込み -> PixelShader書き込み
	TransitionBarrier(offscreenRenderTargetResource->GetResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList);

}

/// @brief デバッグ用パラメータ
void Engine::PostEffectBloomData::DebugParameter()
{
#ifdef _DEVELOPMENT

	if (ImGui::TreeNode((name_ + "_Bloom").c_str()))
	{
		ImGui::DragFloat("Threshold", &param_->threshold, 0.01f, 0.0f, 1000000.0f);
		ImGui::DragFloat("Knee", &param_->knee, 0.01f, 0.0f, 1000000.0f);

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