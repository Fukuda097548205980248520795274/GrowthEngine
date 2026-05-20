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

	// CSパーティクルポイントエミッターPSOを生成する
	computePsoParticle3DEmitterPoint_ = std::make_unique<ComputePSOParticle3DEmitterPoint>();
	computePsoParticle3DEmitterPoint_->Initialize(device, compiler, log);

	// CSパーティクルAABBエミッターPSOを生成する
	computePsoParticle3DEmitterAABB_ = std::make_unique<ComputePSOParticle3DEmitterAABB>();
	computePsoParticle3DEmitterAABB_->Initialize(device, compiler, log);

	// CSパーティクル球エミッターPSOを生成する
	computePsoParticle3DEmitterSphere_ = std::make_unique<ComputePSOParticle3DEmitterSphere>();
	computePsoParticle3DEmitterSphere_->Initialize(device, compiler, log);

	// CSパーティクル速度更新PSOを生成する
	computePsoParticle3DUpdateVelocity_ = std::make_unique<ComputePSOParticle3DUpdateVelocity>();
	computePsoParticle3DUpdateVelocity_->Initialize(device, compiler, log);

	// CSパーティクル引力更新PSOを生成する
	computePsoParticle3DUpdateAttract_ = std::make_unique<ComputePSOParticle3DUpdateAttract>();
	computePsoParticle3DUpdateAttract_->Initialize(device, compiler, log);
}

/// @brief 読み込む
/// @param device 
/// @param commandList 
/// @param hModel 
/// @param name 
/// @param numInstance 
/// @param log 
Particle3DHandle Engine::Particle3DStore::Load(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ModelHandle hModel, const std::string& name,
	uint32_t numInstance, uint32_t emitterNum, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(commandList);

 // 同じ名前がある場合はデータを作り直す（emitter数変更を反映するため）
	for (size_t i = 0; i < dataTable_.size(); ++i)
	{
		if (name != dataTable_[i]->GetName())
		{
			continue;
		}

		const Particle3DHandle handle = static_cast<Particle3DHandle>(i);
		std::unique_ptr<Particle3DData> data = std::make_unique<Particle3DData>(name, hModel, numInstance, emitterNum);
		data->Initialize(device, commandList, heap_, parameter_.get(), modelStore_, textureStore_, psoParticle_.get(), computePsoParticle3DInit_.get(), log);
		dataTable_[i] = std::move(data);
		nameTable_[name] = handle;
		return handle;
	}

	// ハンドルを取得する
	Particle3DHandle handle = static_cast<Particle3DHandle>(dataTable_.size());

	// 名前を記録する
	nameTable_[name] = handle;

	// データを生成する
	std::unique_ptr<Particle3DData> data = std::make_unique<Particle3DData>(name, hModel, numInstance, emitterNum);
	data->Initialize(device, commandList, heap_,parameter_.get(), modelStore_, textureStore_, psoParticle_.get(), computePsoParticle3DInit_.get(), log);
	dataTable_.push_back(std::move(data));

	return handle;
}

/// @brief シーン前のリセット処理
void Engine::Particle3DStore::PerSceneReset()
{
	for (auto& data : dataTable_)data->PerSceneReset();
}

/// @brief 更新処理
/// @param commandList 
void Engine::Particle3DStore::Update(ID3D12GraphicsCommandList* commandList)
{
	for (auto& data : dataTable_)
	{
		// 放出用PSO
		BaseComputePSO* psoEmit = nullptr;

		switch (data->GetShape())
		{
		case Particle3D::EmitterShape::Point:
		default:
			// ポイント
			psoEmit = computePsoParticle3DEmitterPoint_.get();
			break;

		case Particle3D::EmitterShape::AABB:
			// AABB
			psoEmit = computePsoParticle3DEmitterAABB_.get();
			break;

		case Particle3D::EmitterShape::Sphere:
			// 球
			psoEmit = computePsoParticle3DEmitterSphere_.get();
			break;
		}


		// 更新用PSO
		BaseComputePSO* psoUpdate = nullptr;

		if (data->EnableAttract())
		{
			psoUpdate = computePsoParticle3DUpdateAttract_.get();
		}
		else
		{
			psoUpdate = computePsoParticle3DUpdateVelocity_.get();
		}

		// 更新処理
		data->Update(commandList, psoEmit, psoUpdate);
	}
}

/// @brief 描画処理
/// @param commandList 
/// @param handle 
/// @param cameraStore 
void Engine::Particle3DStore::Draw(ID3D12GraphicsCommandList* commandList, Particle3DHandle handle, const Camera3DStore* cameraStore)
{

}

/// @brief 描画処理
/// @param commandList 
/// @param viewProjection 
void Engine::Particle3DStore::Draw(ID3D12GraphicsCommandList* commandList, const std::string& name, const Camera3DStore* cameraStore)
{
	dataTable_[nameTable_[name]]->Draw(commandList, cameraStore);
}

/// @brief デバッグパラメータ
void Engine::Particle3DStore::DebugParameter()
{
#ifdef _DEVELOPMENT
	for (auto& data : dataTable_)data->DebugParameter();
#endif
}