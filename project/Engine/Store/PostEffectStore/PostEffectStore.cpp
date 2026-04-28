#include "PostEffectStore.h"
#include <cassert>
#include "ShaderCompiler/ShaderCompiler.h"

#include "RenderContext/DX12Render/DX12Render.h"
#include "RenderContext/DX12Prefab/DX12Prefab.h"

#include "PostEffectData/PostEffectGrayscaleData/PostEffectGrayscaleData.h"
#include "PostEffectData/PostEffectVignettingData/PostEffectVignettingData.h"
#include "PostEffectData/PostEffectSmoothingData/PostEffectSmoothingData.h"
#include "PostEffectData/PostEffectGaussianFilterData/PostEffectGaussianFilterData.h"
#include "PostEffectData/PostEffectLuminanceBasedOutlineData/PostEffectLuminanceBasedOutlineData.h"
#include "PostEffectData/PostEffectDepthBasedOutlineData/PostEffectDepthBasedOutlineData.h"
#include "PostEffectData/PostEffectRadialBlurData/PostEffectRadialBlurData.h"
#include "PostEffectData/PostEffectDissolveData/PostEffectDissolveData.h"
#include "PostEffectData/PostEffectWhiteNoiseData/PostEffectWhiteNoiseData.h"
#include "PostEffectData/PostEffectDOFData/PostEffectDOFData.h"
#include "PostEffectData/PostEffectBloomData/PostEffectBloomData.h"
#include "PostEffectData/PostEffectTAAData/PostEffectTAAData.h"

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

	// コピー加算PSO
	psoCopyImageAdd_ = std::make_unique<PSOCopyImageAdd>();
	psoCopyImageAdd_->Initialize(device, compiler, vertexShaderBlob, log);

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

	// 輝度ベース輪郭抽出PSO
	psoLuminanceBasedOutline_ = std::make_unique<PSOLuminanceBasedOutline>();
	psoLuminanceBasedOutline_->Initialize(device, compiler, vertexShaderBlob, log);

	// 深度ベース輪郭抽出PSO
	psoDepthBasedOutline_ = std::make_unique<PSODepthBasedOutline>();
	psoDepthBasedOutline_->Initialize(device, compiler, vertexShaderBlob, log);
	
	// ラジアルブラーPSO
	psoRadialBlur_ = std::make_unique<PSORadialBlur>();
	psoRadialBlur_->Initialize(device, compiler, vertexShaderBlob, log);

	// ディゾルブPSO
	psoDissolve_ = std::make_unique<PSODissolve>();
	psoDissolve_->Initialize(device, compiler, vertexShaderBlob, log);

	// ホワイトノイズPSO
	psoWhiteNoise_ = std::make_unique<PSOWhiteNoise>();
	psoWhiteNoise_->Initialize(device, compiler, vertexShaderBlob, log);

	// 被写界深度PSO
	psoDOF_ = std::make_unique<PSODOF>();
	psoDOF_->Initialize(device, compiler, vertexShaderBlob, log);


	// CSデュアルブラー縮小PSO
	computePSODualBlurDownsample_ = std::make_unique<ComputePSODualBlurDownsample>();
	computePSODualBlurDownsample_->Initialize(device, compiler, log);

	// CSデュアルブラー拡大PSO
	computePSODualBlurUpsample_ = std::make_unique<ComputePSODualBlurUpsample>();
	computePSODualBlurUpsample_->Initialize(device, compiler, log);

	// CSガウシアンフィルタPSO
	computePSOGaussianFilter_ = std::make_unique<ComputePSOGaussianFilter>();
	computePSOGaussianFilter_->Initialize(device, compiler, log);

	// CS高輝度抽出PSO
	computePSOHighLuminanceExtraction_ = std::make_unique<ComputePSOHighLuminanceExtraction>();
	computePSOHighLuminanceExtraction_->Initialize(device, compiler, log);

	// CS TAA PSO
	computePSOTAA_ = std::make_unique<ComputePSOTAA>();
	computePSOTAA_->Initialize(device, compiler, log);


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

	// TAAは特別扱いする。理由は、TAAは前フレームの結果を参照するため、同じ名前で複数回読み込むことがあるから。
	if (isLoadTAA_)
	{
		if (PostEffect::Type::TAA == type)
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

	// 輝度ベース輪郭抽出
	if (type == PostEffect::Type::LuminanceBasedOutline)
	{
		std::unique_ptr<PostEffectLuminanceBasedOutlineData> data = std::make_unique<PostEffectLuminanceBasedOutlineData>(name, type, handle, parameter_.get());
		data->Initialize(device, log, psoLuminanceBasedOutline_.get());
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// 深度ベース輪郭抽出
	if (type == PostEffect::Type::DepthBasedOutline)
	{
		std::unique_ptr<PostEffectDepthBasedOutlineData> data = std::make_unique<PostEffectDepthBasedOutlineData>(name, type, handle, parameter_.get());
		data->Initialize(device, log, psoDepthBasedOutline_.get());
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

	// ディゾルブ
	if (type == PostEffect::Type::Dissolve)
	{
		std::unique_ptr<PostEffectDissolveData> data = std::make_unique<PostEffectDissolveData>(name, type, handle, parameter_.get());
		data->Initialize(device, log, psoDissolve_.get(), textureStore_);
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
			computePSOGaussianFilter_.get(), computePSODualBlurUpsample_.get(), computePSODualBlurDownsample_.get(), log);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// ブルーム
	if (type == PostEffect::Type::Bloom)
	{
		std::unique_ptr<PostEffectBloomData> data = std::make_unique<PostEffectBloomData>(name, type, handle, parameter_.get());
		data->Initialize(device, commandList, buffering, heap_, psoCopyImageAdd_.get(),
			computePSOHighLuminanceExtraction_.get(), computePSODualBlurUpsample_.get(), computePSODualBlurDownsample_.get(), log);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// TAA
	if (type == PostEffect::Type::TAA)
	{
		std::unique_ptr<PostEffectTAAData> data = std::make_unique<PostEffectTAAData>(name, type, handle, parameter_.get());
		data->Initialize(device, commandList, heap_, buffering, computePSOTAA_.get(), log);
		dataTable_.push_back(std::move(data));

		// TAAはモーションベクトルを必要とするため、モーションベクトル有効化フラグを立てる
		enableMotionVector_ = true;

		// TAAのハンドルを保存しておく
		isLoadTAA_ = true;
		hTAA_ = handle;

		return handle;
	}

	assert(false);
	return handle;
}

/// @brief シーン前のリセット処理
void Engine::PostEffectStore::PerSceneReset()
{
	// モーションベクトル有効化を初期化
	enableMotionVector_ = false;

	// TAAの読み込みフラグを初期化
	isLoadTAA_ = false;
}

/// @brief モーションベクトルの描画処理をコマンドリストに登録する
/// @param commandList 
/// @param dsvHandle 
void Engine::PostEffectStore::DrawMotionVector(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle,
	DX12Render* render, DX12Prefab* prefab)
{
	// モーションベクトルが必要なポストエフェクトがない場合は描画しない
	if (!enableMotionVector_)return;

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
	if (!enableMotionVector_)return;

	// TAAがない場合は描画しない
	if (!isLoadTAA_)return;

	// TAAの描画処理をコマンドリストに登録する
	PostEffectRenderContext renderContext = context;
	renderContext.motionVectorTextureResource = motionVectorTextureResource_.get();

	// TAAの描画処理をコマンドリストに登録する
	dataTable_[hTAA_]->Register(renderContext);
}

/// @brief デバッグ用パラメータ
void Engine::PostEffectStore::DebugParameter()
{

	for (auto& data : dataTable_)data->DebugParameter();
}