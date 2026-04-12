#include "PostEffectStore.h"
#include <cassert>
#include "ShaderCompiler/ShaderCompiler.h"

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
void Engine::PostEffectStore::Initialize(ID3D12Device* device, ShaderCompiler* compiler, IDxcBlob* vertexShaderBlob,TextureStore* textureStore, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(compiler);
	assert(vertexShaderBlob);
	assert(textureStore);

	// 引数を受け取る
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
}

/// @brief 読み込み
/// @param name 
/// @param type 
/// @param device 
/// @param log 
PostEffectHandle Engine::PostEffectStore::Load(const std::string& name, PostEffect::Type type,
	ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Buffering* buffering, DX12Heap* heap, Log* log)
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
		data->Initialize(device, commandList, buffering, heap, psoDOF_.get(),
			computePSOGaussianFilter_.get(), computePSODualBlurUpsample_.get(), computePSODualBlurDownsample_.get(), log);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// ブルーム
	if (type == PostEffect::Type::Bloom)
	{
		std::unique_ptr<PostEffectBloomData> data = std::make_unique<PostEffectBloomData>(name, type, handle, parameter_.get());
		data->Initialize(device, commandList, buffering, heap, psoCopyImageAdd_.get(),
			computePSOHighLuminanceExtraction_.get(), computePSODualBlurUpsample_.get(), computePSODualBlurDownsample_.get(), log);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	assert(false);
	return handle;
}

/// @brief デバッグ用パラメータ
void Engine::PostEffectStore::DebugParameter()
{

	for (auto& data : dataTable_)data->DebugParameter();
}