#include "PostEffectStore.h"
#include <cassert>
#include "ShaderCompiler/ShaderCompiler.h"

#include "PostEffectData/PostEffectGrayscaleData/PostEffectGrayscaleData.h"
#include "PostEffectData/PostEffectVignettingData/PostEffectVignettingData.h"
#include "PostEffectData/PostEffectSmoothingData/PostEffectSmoothingData.h"
#include "PostEffectData/PostEffectGaussianFilterData/PostEffectGaussianFilterData.h"
#include "PostEffectData/PostEffectLuminanceBasedOutlineData/PostEffectLuminanceBasedOutlineData.h"
#include "PostEffectData/PostEffectRadialBlurData/PostEffectRadialBlurData.h"

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
void Engine::PostEffectStore::Initialize(ID3D12Device* device, ShaderCompiler* compiler, IDxcBlob* vertexShaderBlob, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(compiler);
	assert(vertexShaderBlob);

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
}

/// @brief 読み込み
/// @param name 
/// @param type 
/// @param device 
/// @param log 
PostEffectHandle Engine::PostEffectStore::Load(const std::string& name, PostEffect::Type type, ID3D12Device* device, Log* log)
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

	// ラジアルブラー
	if (type == PostEffect::Type::RadialBlur)
	{
		std::unique_ptr<PostEffectRadialBlurData> data = std::make_unique<PostEffectRadialBlurData>(name, type, handle, parameter_.get());
		data->Initialize(device, log, psoRadialBlur_.get());
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