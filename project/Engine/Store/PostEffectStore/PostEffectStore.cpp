#include "PostEffectStore.h"
#include <cassert>
#include "ShaderCompiler/ShaderCompiler.h"

#include "RenderContext/DX12Render/DX12Render.h"
#include "RenderContext/DX12Prefab/DX12Prefab.h"

#include "RenderContext/DX12Offscreen/RenderTargetPool/RenderTargetPool.h"

#include "PostEffectData/PostEffectGrayscaleData/PostEffectGrayscaleData.h"
#include "PostEffectData/PostEffectVignettingData/PostEffectVignettingData.h"
#include "PostEffectData/PostEffectSmoothingData/PostEffectSmoothingData.h"
#include "PostEffectData/PostEffectGaussianFilterData/PostEffectGaussianFilterData.h"
#include "PostEffectData/PostEffectRadialBlurData/PostEffectRadialBlurData.h"
#include "PostEffectData/PostEffectWhiteNoiseData/PostEffectWhiteNoiseData.h"
#include "PostEffectData/PostEffectDOFData/PostEffectDOFData.h"
#include "PostEffectData/PostEffectBloomData/PostEffectBloomData.h"
#include "PostEffectData/PostEffectTAAData/PostEffectTAAData.h"
#include "PostEffectData/PostEffectMotionBlurData/PostEffectMotionBlurData.h"
#include "PostEffectData/PostEffectAfterImageData/PostEffectAfterImageData.h"
#include "PostEffectData/PostEffectBlurShadow2DData/PostEffectBlurShadow2DData.h"
#include "PostEffectData/PostEffectLuminanceBasedOutlineData/PostEffectLuminanceBasedOutlineData.h"
#include "PostEffectData/PostEffectDepthBasedOutlineData/PostEffectDepthBasedOutlineData.h"

bool Engine::PostEffectStore::isLoadMotionBlur_ = false;
bool Engine::PostEffectStore::isLoadAfterImage_ = false;
bool Engine::PostEffectStore::isLoadTAA_ = false;
bool Engine::PostEffectStore::isLoadLuminanceBasedOutline_ = false;
bool Engine::PostEffectStore::isLoadDepthBasedOutline_ = false;

/// @brief コンストラクタ
Engine::PostEffectStore::PostEffectStore()
{
	// パラメータの生成
	parameter_ = std::make_unique<PostEffectParameter>("PostEffect");
}

/// @brief 初期化
/// @param device 
/// @param compiler 
/// @param log 
void Engine::PostEffectStore::Initialize(ID3D12Device* device, ShaderCompiler* compiler, IDxcBlob* vertexShaderBlob,
	DX12Heap* heap, TextureStore* textureStore, int32_t width, int32_t height, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(compiler);
	assert(vertexShaderBlob);
	assert(heap);
	assert(textureStore);

	// 引数を受け取る
	heap_ = heap;
	textureStore_ = textureStore;

	// グレースケールPSO
	psoGrayscale_ = std::make_unique<PSOGrayscale>();
	psoGrayscale_->Initialize(device, compiler, vertexShaderBlob, log);

	// ヴィネッティングPSO
	psoVignetting_ = std::make_unique<PSOVignetting>();
	psoVignetting_->Initialize(device, compiler, vertexShaderBlob, log);

	// 平滑化PSO
	psoSmoothing_ = std::make_unique<PSOSmoothing>();
	psoSmoothing_->Initialize(device, compiler, vertexShaderBlob, log);

	// ガウシアンフィルタPSO
	psoGaussianFilter_ = std::make_unique<PSOGaussianFilter>();
	psoGaussianFilter_->Initialize(device, compiler, vertexShaderBlob, log);
	
	// ラジアルブラーPSO
	psoRadialBlur_ = std::make_unique<PSORadialBlur>();
	psoRadialBlur_->Initialize(device, compiler, vertexShaderBlob, log);

	// ホワイトノイズPSO
	psoWhiteNoise_ = std::make_unique<PSOWhiteNoise>();
	psoWhiteNoise_->Initialize(device, compiler, vertexShaderBlob, log);

	// 被写界深度PSO
	psoDOF_ = std::make_unique<PSODOF>();
	psoDOF_->Initialize(device, compiler, vertexShaderBlob, log);

	// ブラーシャドウ2DPSO
	psoBlurShadow2D_ = std::make_unique<PSOBlurShadow2D>();
	psoBlurShadow2D_->Initialize(device, compiler, vertexShaderBlob, log);

	// 輝度ベースのアウトラインPSO
	psoLuminanceBasedOutline_ = std::make_unique<PSOLuminanceBasedOutline>();
	psoLuminanceBasedOutline_->Initialize(device, compiler, vertexShaderBlob, log);

	// 深度ベースのアウトラインPSO
	psoDepthBasedOutline_ = std::make_unique<PSODepthBasedOutline>();
	psoDepthBasedOutline_->Initialize(device, compiler, vertexShaderBlob, log);


	// CSデュアルブラー縮小PSO
	computePSODualBlurDownsample_ = std::make_unique<ComputePSODualBlurDownsample>();
	computePSODualBlurDownsample_->Initialize(device, compiler, log);

	// CSデュアルブラー拡大PSO
	computePSODualBlurUpsample_ = std::make_unique<ComputePSODualBlurUpsample>();
	computePSODualBlurUpsample_->Initialize(device, compiler, log);

	// CS高輝度抽出PSO
	computePSOHighLuminanceExtraction_ = std::make_unique<ComputePSOHighLuminanceExtraction>();
	computePSOHighLuminanceExtraction_->Initialize(device, compiler, log);

	// CS TAA PSO
	computePSOTAA_ = std::make_unique<ComputePSOTAA>();
	computePSOTAA_->Initialize(device, compiler, log);

	// CSモーションブラーPSO
	computePSOMotionBlur_ = std::make_unique<ComputePSOMotionBlur>();
	computePSOMotionBlur_->Initialize(device, compiler, log);

	// CS速度膨張PSO
	computePSOVelocityDilation_ = std::make_unique<ComputePSOVelocityDilation>();
	computePSOVelocityDilation_->Initialize(device, compiler, log);

	// CS残像PSO
	computePSOAfterImage_ = std::make_unique<ComputePSOAfterImage>();
	computePSOAfterImage_->Initialize(device, compiler, log);


	// モーションベクトルのピクセルシェーダーのコンパイル
	motionVectorPixelShaderBlob_ = compiler->Compile(L"./Assets/Shader/MotionVector/MotionVector.PS.hlsl", L"ps_6_0");

	// モーションベクトル描画用PSOの初期化
	psoMotionVectorRender_ = std::make_unique<PSOMotionVectorRender>();
	psoMotionVectorRender_->Initialize(device, compiler, motionVectorPixelShaderBlob_.Get(), log);

	// モーションベクトルPrefab描画用PSOの初期化
	psoMotionVectorPrefab_ = std::make_unique<PSOMotionVectorPrefab>();
	psoMotionVectorPrefab_->Initialize(device, compiler, motionVectorPixelShaderBlob_.Get(), log);

	// モーションベクトルテクスチャリソースの初期化
	motionVectorTextureResource_ = std::make_unique<MotionVectorTextureResource>();
	motionVectorTextureResource_->Initialize(device, width, height, heap_, log);


	// アウトライン描画用PSOの初期化
	psoOutlineRender_ = std::make_unique<PSOOutlineRender>();
	psoOutlineRender_->Initialize(device, compiler, log);

	// アウトラインPrefab描画用PSOの初期化
	psoOutlinePrefab_ = std::make_unique<PSOOutlinePrefab>();
	psoOutlinePrefab_->Initialize(device, compiler, log);


	// アウトラインテクスチャリソースの初期化
	outlineRenderResource_ = std::make_unique<OffscreenResource>();
	outlineRenderResource_->Initialize(device, heap_,width, height, log);

	// アウトライン深度リソースの初期化
	outlineDepthResource_ = std::make_unique<DepthResource>();
	outlineDepthResource_->Initialize(device, width, height, heap_, log);	
}

/// @brief リサイズ
/// @param device 
/// @param commandList 
/// @param width 
/// @param height 
void Engine::PostEffectStore::Resize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, int32_t width, int32_t height)
{
	// データテーブル内の全てのデータをリサイズする
	for(auto& data : dataTable_)
		data->Resize(device, commandList, width, height);

	// モーションベクトルテクスチャリソースをリサイズする
	if (motionVectorTextureResource_)
		motionVectorTextureResource_->Resize(device, width, height);

	// アウトラインテクスチャリソースをリサイズする
	if (outlineRenderResource_)
		outlineRenderResource_->Resize(device, width, height);

	// アウトライン深度リソースをリサイズする
	if (outlineDepthResource_)
		outlineDepthResource_->Resize(device, width, height);
}

/// @brief 読み込み
/// @param name 
/// @param type 
/// @param device 
/// @param log 
PostEffectHandle Engine::PostEffectStore::Load(const std::string& name, PostEffect::Type type,
	ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Buffering* buffering, Log* log)
{
	// 同じデータがあるかどうか
	for (auto& data : dataTable_)
	{
		if (name == data->GetName() && type == data->GetType())
		{
			data->Reset();
			return data->GetHandle();
		}
	}

	// TAAやモーションブラーはモーションベクトルを必要とするため、すでに読み込まれている場合はリセットしてハンドルを返す
	if (IsEnableMotionVector())
	{
		if (PostEffect::Type::TAA == type || PostEffect::Type::MotionBlur == type)
		{
			for (auto& data : dataTable_)
			{
				if (type == data->GetType())
				{
					data->Reset();
					return data->GetHandle();
				}
			}
		}
	}

	// アウトラインはアウトライン用のリソースを必要とするため、すでに読み込まれている場合はリセットしてハンドルを返す
	if (IsLoadOutline())
	{
		if (PostEffect::Type::LuminanceBasedOutline == type || PostEffect::Type::DepthBasedOutline == type)
		{
			for (auto& data : dataTable_)
			{
				if (type == data->GetType())
				{
					data->Reset();
					return data->GetHandle();
				}
			}
		}
	}


	// ハンドル
	PostEffectHandle handle = static_cast<PostEffectHandle>(dataTable_.size());

	// 名前テーブルに登録
	nameTable_[name] = handle;

	// グレースケール
	if (type == PostEffect::Type::Grayscale)
	{
		std::unique_ptr<PostEffectGrayscaleData> data = std::make_unique<PostEffectGrayscaleData>(name, type, handle, parameter_.get());
		data->Initialize(device, log, psoGrayscale_.get());
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// ヴィネッティング
	if (type == PostEffect::Type::Vignetting)
	{
		std::unique_ptr<PostEffectVignettingData> data = std::make_unique<PostEffectVignettingData>(name, type, handle, parameter_.get());
		data->Initialize(device, log, psoVignetting_.get());
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// 平滑化
	if (type == PostEffect::Type::Smoothing)
	{
		std::unique_ptr<PostEffectSmoothingData> data = std::make_unique<PostEffectSmoothingData>(name, type, handle, parameter_.get());
		data->Initialize(device, log, psoSmoothing_.get());
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// ガウシアンフィルター
	if (type == PostEffect::Type::GaussianFilter)
	{
		std::unique_ptr<PostEffectGaussianFilterData> data = std::make_unique<PostEffectGaussianFilterData>(name, type, handle, parameter_.get());
		data->Initialize(device, log, psoGaussianFilter_.get());
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// ラジアルブラー
	if (type == PostEffect::Type::RadialBlur)
	{
		std::unique_ptr<PostEffectRadialBlurData> data = std::make_unique<PostEffectRadialBlurData>(name, type, handle, parameter_.get());
		data->Initialize(device, log, psoRadialBlur_.get());
		dataTable_.push_back(std::move(data));

		return handle;
	}

	// ホワイトノイズ
	if (type == PostEffect::Type::WhiteNoise)
	{
		std::unique_ptr<PostEffectWhiteNoiseData> data = std::make_unique<PostEffectWhiteNoiseData>(name, type, handle, parameter_.get());
		data->Initialize(device, log, psoWhiteNoise_.get());
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// 被写界深度
	if (type == PostEffect::Type::DOF)
	{
		std::unique_ptr<PostEffectDOFData> data = std::make_unique<PostEffectDOFData>(name, type, handle, parameter_.get());
		data->Initialize(device, commandList, buffering, heap_, psoDOF_.get(),
			computePSODualBlurUpsample_.get(), computePSODualBlurDownsample_.get(), log);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// ブルーム
	if (type == PostEffect::Type::Bloom)
	{
		std::unique_ptr<PostEffectBloomData> data = std::make_unique<PostEffectBloomData>(name, type, handle, parameter_.get());
		data->Initialize(device, commandList, buffering, heap_,
			computePSOHighLuminanceExtraction_.get(), computePSODualBlurUpsample_.get(), computePSODualBlurDownsample_.get(), log);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// ブラーシャドウ2D
	if (type == PostEffect::Type::BlurShadow2D)
	{
		std::unique_ptr<PostEffectBlurShadow2DData> data = std::make_unique<PostEffectBlurShadow2DData>(name, type, handle, parameter_.get());
		data->Initialize(device, log, psoBlurShadow2D_.get());
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// TAA
	if (type == PostEffect::Type::TAA)
	{
		std::unique_ptr<PostEffectTAAData> data = std::make_unique<PostEffectTAAData>(name, type, handle, parameter_.get());
		data->Initialize(device, commandList, heap_, buffering, computePSOTAA_.get(), log);
		dataTable_.push_back(std::move(data));

		// TAAのハンドルを保存しておく
		isLoadTAA_ = true;
		hTAA_ = handle;

		return handle;
	}

	// モーションブラー
	if (type == PostEffect::Type::MotionBlur)
	{
		std::unique_ptr<PostEffectMotionBlurData> data = std::make_unique<PostEffectMotionBlurData>(name, type, handle, parameter_.get());
		data->Initialize(device, commandList, heap_, buffering, computePSOMotionBlur_.get(), computePSOVelocityDilation_.get(), log);
		dataTable_.push_back(std::move(data));

		// モーションブラーのハンドルを保存しておく
		isLoadMotionBlur_ = true;
		hMotionBlur_ = handle;

		return handle;
	}

	// 残像
	if (type == PostEffect::Type::AfterImage)
	{
		std::unique_ptr<PostEffectAfterImageData> data = std::make_unique<PostEffectAfterImageData>(name, type, handle, parameter_.get());
		data->Initialize(device, commandList, heap_, buffering, computePSOAfterImage_.get(), log);
		dataTable_.push_back(std::move(data));

		// 残像のハンドルを保存しておく
		isLoadAfterImage_ = true;
		hAfterImage_ = handle;

		return handle;
	}

	// 輝度ベースのアウトライン
	if (type == PostEffect::Type::LuminanceBasedOutline)
	{
		std::unique_ptr<PostEffectLuminanceBasedOutlineData> data = std::make_unique<PostEffectLuminanceBasedOutlineData>(name, type, handle, parameter_.get());
		data->Initialize(device, log, psoLuminanceBasedOutline_.get());
		dataTable_.push_back(std::move(data));

		// 輝度ベースのアウトラインのハンドルを保存しておく
		isLoadLuminanceBasedOutline_ = true;
		hLuminanceBasedOutline_ = handle;
		return handle;
	}

	// 深度ベースのアウトライン
	if (type == PostEffect::Type::DepthBasedOutline)
	{
		std::unique_ptr<PostEffectDepthBasedOutlineData> data = std::make_unique<PostEffectDepthBasedOutlineData>(name, type, handle, parameter_.get());
		data->Initialize(device, log, psoDepthBasedOutline_.get());
		dataTable_.push_back(std::move(data));

		// 深度ベースのアウトラインのハンドルを保存しておく
		isLoadDepthBasedOutline_ = true;
		hDepthBasedOutline_ = handle;
		return handle;
	}

	assert(false);
	return handle;
}

/// @brief シーン前のリセット処理
void Engine::PostEffectStore::PerSceneReset()
{
	// 読み込みフラグを初期化
	isLoadTAA_ = false;
	isLoadMotionBlur_ = false;
	isLoadAfterImage_ = false;
	isLoadLuminanceBasedOutline_ = false;
	isLoadDepthBasedOutline_ = false;
}

/// @brief モーションベクトルの描画処理をコマンドリストに登録する
/// @param commandList 
/// @param dsvHandle 
void Engine::PostEffectStore::DrawMotionVector(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle,
	DX12Render* render, DX12Prefab* prefab)
{
	// モーションベクトルが必要なポストエフェクトがない場合は描画しない
	if (!IsEnableMotionVector())return;

	// モーションベクトルテクスチャをレンダーターゲットに設定
	motionVectorTextureResource_->ClearRenderTarget(commandList, dsvHandle);

	// レンダーとプレハブを使ってモーションベクトルを描画
	render->DrawMotionVector(commandList, psoMotionVectorRender_.get());
	prefab->DrawMotionVector(commandList, psoMotionVectorPrefab_.get());
}

/// @brief TAAの描画処理をコマンドリストに登録する
/// @param context 
void Engine::PostEffectStore::DrawTAA(const PostEffectRenderContext& context)
{
	// モーションベクトルが必要なポストエフェクトがない場合は描画しない
	if (!IsEnableMotionVector())return;

	// TAAがない場合は描画しない
	if (!isLoadTAA_)return;

	// TAAの描画処理をコマンドリストに登録する
	PostEffectRenderContext renderContext = context;
	renderContext.motionVectorTextureResource = motionVectorTextureResource_.get();

	// TAAの描画処理をコマンドリストに登録する
	dataTable_[hTAA_]->Register(renderContext);
}

/// @brief モーションブラーの描画処理をコマンドリストに登録する
/// @param context 
void Engine::PostEffectStore::DrawMotionBlur(const PostEffectRenderContext& context)
{
	// モーションベクトルが必要なポストエフェクトがない場合は描画しない
	if (!IsEnableMotionVector())return;

	// モーションブラーがない場合は描画しない
	if (!isLoadMotionBlur_)return;

	// モーションブラーの描画処理をコマンドリストに登録する
	PostEffectRenderContext renderContext = context;
	renderContext.motionVectorTextureResource = motionVectorTextureResource_.get();

	// モーションブラーの描画処理をコマンドリストに登録する
	dataTable_[hMotionBlur_]->Register(renderContext);
}

/// @brief 残像の描画処理をコマンドリストに登録する
/// @param context 
void Engine::PostEffectStore::DrawAfterImage(const PostEffectRenderContext& context)
{
	// モーションベクトルが必要なポストエフェクトがない場合は描画しない
	if (!IsEnableMotionVector())return;

	// 残像がない場合は描画しない
	if (!isLoadAfterImage_)return;

	// 残像の描画処理をコマンドリストに登録する
	PostEffectRenderContext renderContext = context;
	renderContext.motionVectorTextureResource = motionVectorTextureResource_.get();

	// 残像の描画処理をコマンドリストに登録する
	dataTable_[hAfterImage_]->Register(renderContext);
}

/// @brief アウトライン描画処理をコマンドリストに登録する
/// @param context 
void Engine::PostEffectStore::DrawOutline(const PostEffectRenderContext& context)
{
	// アウトラインがない場合は描画しない
	if (!IsLoadOutline())return;

	ID3D12GraphicsCommandList* commandList = context.commandList;
	OffscreenResource* offscreenRenderTargetResource = context.offscreenRenderTargetResource;
	OffscreenResource* offscreenPixelShaderResource = context.offscreenPixelShaderResource;
	DepthResource* depthResource = context.depthResource;
	DX12Render* dx12Render = context.dx12Render;
	DX12Prefab* dx12Prefab = context.dx12Prefab;

	assert(commandList);
	assert(offscreenRenderTargetResource);
	assert(offscreenPixelShaderResource);
	assert(depthResource);
	assert(dx12Render);
	assert(dx12Prefab);

	PostEffectRenderContext renderContext = context;
	renderContext.outlineTextureResource = outlineRenderResource_.get();
	renderContext.outlineDepthResource = outlineDepthResource_.get();

	// アウトライン描画のレンダーターゲットと深度ステンシルをクリアする
	outlineRenderResource_->ClearRenderTarget(commandList, outlineDepthResource_->GetDsvCpuHandle());
	outlineDepthResource_->ClearDepthStencil(commandList);

	// アウトライン描画の描画処理をコマンドリストに登録する
	dx12Render->DrawOutline(commandList, psoOutlineRender_.get());
	dx12Prefab->DrawOutline(commandList, psoOutlinePrefab_.get());

	// レンダーターゲットの設定
	offscreenRenderTargetResource->SetRenderTarget(commandList, outlineDepthResource_->GetDsvCpuHandle());

	// バリアを張る
	outlineRenderResource_->Barrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	outlineDepthResource_->Barrier(commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// 深度ベースのアウトライン描画の描画処理をコマンドリストに登録する
	if(isLoadDepthBasedOutline_)
		dataTable_[hDepthBasedOutline_]->Register(renderContext);

	// 輝度ベースのアウトライン描画の描画処理をコマンドリストに登録する
	if(isLoadLuminanceBasedOutline_)
		dataTable_[hLuminanceBasedOutline_]->Register(renderContext);

	// バリアを張る
	outlineRenderResource_->Barrier(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	outlineDepthResource_->Barrier(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

/// @brief デバッグ用パラメータ
void Engine::PostEffectStore::DebugParameter()
{

	for (auto& data : dataTable_)data->DebugParameter();
}