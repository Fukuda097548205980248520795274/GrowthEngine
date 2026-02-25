#include "PostEffectStore.h"
#include <cassert>
#include "ShaderCompiler/ShaderCompiler.h"

#include "PostEffectData/PostEffectRadialBlurData/PostEffectRadialBlurData.h"

/// @brief コンストラクタ
Engine::PostEffectStore::PostEffectStore()
{
	// パラメータの生成
	parameter_ = std::make_unique<PostEffectParameter>("PostEffect", "./Assets/Parameter/PostEffect/");
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
			return data->GetHandle();
	}

	// ハンドル
	PostEffectHandle handle = static_cast<PostEffectHandle>(dataTable_.size());

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

/// @brief ポストエフェクトを描画する
/// @param hPostEffect 
/// @param commandList 
/// @param offscreenResource 
void Engine::PostEffectStore::DrawPostEffect(PostEffectHandle hPostEffect, ID3D12GraphicsCommandList* commandList, OffscreenResource* offscreenResource)
{
	dataTable_[hPostEffect]->Register(commandList, offscreenResource);
}

/// @brief デバッグ用パラメータ
void Engine::PostEffectStore::DebugParameter()
{

	for (auto& data : dataTable_)data->DebugParameter();
}