#include "Particle3DStore.h"
#include "RenderContext/DX12Heap/DX12Heap.h"
#include "ShaderCompiler/ShaderCompiler.h"
#include <cassert>

/// @brief 初期化
/// @param device 
/// @param compiler 
/// @param heap 
/// @param modelStore 
/// @param textureStore 
void Engine::Particle3DStore::Initialize(ID3D12Device* device, ShaderCompiler* compiler, DX12Heap* heap, ModelStore* modelStore, TextureStore* textureStore, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(compiler);
	assert(heap);
	assert(modelStore);
	assert(textureStore);

	// 引数を受け取る
	heap_ = heap;
	modelStore_ = modelStore;
	textureStore_ = textureStore;

	// パラメータを生成する
	parameter_ = std::make_unique<Particle3DParameter>("Particle3D");

	// シェーダーをコンパイルする
	vertexShaderBlob_ = compiler->Compile(L"./Assets/Shader/Particle3D/Draw/Particle3D.VS.hlsl", L"vs_6_0");
	pixelShaderBlob_ = compiler->Compile(L"./Assets/Shader/Particle3D/Draw/Particle3D.PS.hlsl", L"ps_6_0");

	// パーティクルPSOを生成する
	psoParticle_ = std::make_unique<PSOParticle>();
	psoParticle_->Initialize(device, vertexShaderBlob_.Get(), pixelShaderBlob_.Get(), log);

	// CSパーティクル初期化PSOを生成する
	computePsoParticle3DInit_ = std::make_unique<ComputePSOParticle3DInit>();
	computePsoParticle3DInit_->Initialize(device, compiler, log);

	// CSパーティクルエミッターポイントPSOを生成する
	computePsoParticle3DEmitterPoint_ = std::make_unique<ComputePSOParticle3DEmitterPoint>();
	computePsoParticle3DEmitterPoint_->Initialize(device, compiler, log);

	// CSパーティクル更新PSOを生成する
	computePsoParticle3DUpdate_ = std::make_unique<ComputePSOParticle3DUpdate>();
	computePsoParticle3DUpdate_->Initialize(device, compiler, log);
}

/// @brief リセット
void Engine::Particle3DStore::Reset()
{
	psoParticle_->ResetBlendMode();
}

/// @brief 読み込む
/// @param device 
/// @param commandList 
/// @param hModel 
/// @param name 
/// @param numInstance 
/// @param log 
Particle3DHandle Engine::Particle3DStore::Load(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ModelHandle hModel,
	const std::string& name, uint32_t numInstance, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(commandList);

	// 同じデータがあるか調べる
	for(auto& data : dataTable_)
		if (name == data->GetName())
		{
			data->Reset();
			return data->GetHandle();
		}

	// ハンドルを取得する
	Particle3DHandle handle = static_cast<Particle3DHandle>(dataTable_.size());

	// 名前を記録する
	nameTable_[name] = handle;

	// データを生成する
	std::unique_ptr<Particle3DData> data = std::make_unique<Particle3DData>(name, hModel, numInstance);
	data->Initialize(device, commandList, heap_,parameter_.get(), modelStore_, textureStore_, psoParticle_.get(), computePsoParticle3DInit_.get(), log);
	dataTable_.push_back(std::move(data));

	return handle;
}

/// @brief 更新処理
/// @param commandList 
void Engine::Particle3DStore::Update(ID3D12GraphicsCommandList* commandList)
{
	for (auto& data : dataTable_)data->Update(commandList, computePsoParticle3DEmitterPoint_.get(), computePsoParticle3DUpdate_.get());
}

/// @brief 描画処理
/// @param commandList 
/// @param viewProjection 
void Engine::Particle3DStore::Draw(ID3D12GraphicsCommandList* commandList, const std::string& name, const Matrix4x4& viewProjection)
{
	dataTable_[nameTable_[name]]->Draw(commandList, viewProjection);
}

/// @brief デバッグパラメータ
void Engine::Particle3DStore::DebugParameter()
{
#ifdef _DEVELOPMENT
	for (auto& data : dataTable_)data->DebugParameter();
#endif
}