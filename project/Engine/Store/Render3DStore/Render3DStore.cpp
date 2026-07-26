#include "Render3DStore.h"
#include "Render3DData/Render3DStaticModelData/Render3DStaticModelData.h"
#include "Render3DData/Render3DAnimationModelData/Render3DAnimationModelData.h"
#include "Render3DData/Render3DSkinningModelData/Render3DSkinningModelData.h"
#include "Render3DData/Render3DUVSphereData/Render3DUVSphereData.h"
#include "Render3DData/Render3DRingData/Render3DRingData.h"
#include "Render3DData/Render3DCylinderData/Render3DCylinderData.h"

#include "ShaderCompiler/ShaderCompiler.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

/// @brief コンストラクタ
Engine::Render3DStore::Render3DStore()
{
}

/// @brief 初期化
/// @param device 
/// @param compiler 
/// @param log 
void Engine::Render3DStore::Initialize(ID3D12Device* device, ShaderCompiler* compiler, DX12Heap* heap,
	ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, SkeletonStore* skeletonStore, LightStore* lightStore, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(compiler);
	assert(heap);
	assert(modelStore);
	assert(textureStore);
	assert(animationStore);
	assert(skeletonStore);
	assert(lightStore);

	// 引数を受け取る
	heap_ = heap;
	modelStore_ = modelStore;
	textureStore_ = textureStore;
	animationStore_ = animationStore;
	skeletonStore_ = skeletonStore;
	lightStore_ = lightStore;



	// 3D描画頂点シェーダ
	render3DVS_ = compiler->Compile(L"./Assets/Shader/Render/Render3D/Render3D.VS.hlsl", L"vs_6_0");
	assert(render3DVS_);

	// 3D描画ピクセルシェーダ
	render3DPS_ = compiler->Compile(L"./Assets/Shader/Render/Render3D/Render3D.PS.hlsl", L"ps_6_0");
	assert(render3DPS_);

	// 3D描画PSOの生成と初期化
	psoRender3D_ = std::make_unique<PSORender3D>();
	psoRender3D_->Initialize(device, render3DVS_.Get(), render3DPS_.Get(), log);

	// スキニングPSOの生成と初期化
	psoSkinning_ = std::make_unique<ComputePSOSkinning>();
	psoSkinning_->Initialize(device, compiler, log);
}

/// @brief 更新処理
void Engine::Render3DStore::Update(ID3D12GraphicsCommandList* commandList)
{
	// データ更新
	for (auto& data : dataTable_)
	{
		data->Update();

		// スキニングモデル
		if (data->GetType() == Render3D::Type::SkinningModel)
		{
			auto p = static_cast<Render3DSkinningModelData*>(data.get());
			p->Skinning(commandList, psoSkinning_.get());
		}
	}
}

/// @brief シーン前のリセット
void Engine::Render3DStore::PerSceneReset()
{
	for (auto& data : dataTable_)data->PerSceneReset();
}

/// @brief シャドウマップ用の描画処理
/// @param commandList 
/// @param pso 
void Engine::Render3DStore::ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
	for (auto& data : dataTable_)
		data->Register(viewProjection, commandList, pso);
}

/// @brief モーションベクター用の描画処理
/// @param commandList 
/// @param pso 
void Engine::Render3DStore::DrawMotionVector(ID3D12GraphicsCommandList* commandList, BasePSOMotionVector* pso)
{
	for(auto& data : dataTable_)
		data->RegisterMotionVector(commandList, pso);
}


/// @brief アウトライン用の描画処理
/// @param commandList 
/// @param pso 
void Engine::Render3DStore::DrawOutline(ID3D12GraphicsCommandList* commandList, BasePSOOutline* pso)
{
	for (auto& data : dataTable_)
		data->RegisterOutline(commandList, pso);
}


/// @brief 読み込み
/// @param modelStore 
/// @param textureStore 
/// @param device 
/// @param hModel 
/// @param name 
/// @param type 
/// @param log 
/// @return 
Render3DHandle Engine::Render3DStore::Load(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
	TextureHandle hTexture, ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton,
	const std::string& name, Render3D::Type type, Log* log)
{
	// 同じデータがあるかどうか
	for (auto& data : dataTable_)
	{
		if (data->GetName() == name && data->GetType() == type)
		{
			// リセット
			data->Reset();
			return data->GetHandle();
		}
	}

	// ハンドル
	Render3DHandle handle = static_cast<Render3DHandle>(dataTable_.size());

	// 名前テーブルに記録する
	nameTable_[name] = handle;


	// 静的モデル
	if (type == Render3D::Type::StaticModel)
	{
		std::unique_ptr<Render3DStaticModelData> data = std::make_unique<Render3DStaticModelData>(name, hModel, handle);
		data->Initialize(modelStore_, textureStore_, lightStore_, device, log);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// アニメーションモデル
	if (type == Render3D::Type::AnimationModel)
	{
		std::unique_ptr<Render3DAnimationModelData> data = std::make_unique<Render3DAnimationModelData>(name, hModel, hAnimation, handle);
		data->Initialize(modelStore_, textureStore_, animationStore_, lightStore_, device, log);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// スキニングモデル
	if (type == Render3D::Type::SkinningModel)
	{
		std::unique_ptr<Render3DSkinningModelData> data = std::make_unique<Render3DSkinningModelData>(name, hModel,hAnimation, hSkeleton, handle);
		data->Initialize(modelStore_, textureStore_, animationStore_, skeletonStore_, lightStore_, heap_, device, commandList, log);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// UV球
	if (type == Render3D::Type::UVSphere)
	{
		std::unique_ptr<Render3DUVSphereData> data = std::make_unique<Render3DUVSphereData>(name, hTexture, handle);
		data->Initialize(textureStore_, lightStore_, device, log);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// リング
	if (type == Render3D::Type::Ring)
	{
		std::unique_ptr<Render3DRingData> data = std::make_unique<Render3DRingData>(name, hTexture, handle);
		data->Initialize(textureStore_, lightStore_, device, log);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// 円柱
	if (type == Render3D::Type::Cylinder)
	{
		std::unique_ptr<Render3DCylinderData> data = std::make_unique<Render3DCylinderData>(name, hTexture, handle);
		data->Initialize(textureStore_, lightStore_, device, log);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	assert(false);
	return handle;
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param handle 
/// @param meshIndex 
void Engine::Render3DStore::Register(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, Render3DHandle handle)
{
	// コマンドリストの登録する
	dataTable_[handle]->Register(cameraStore, skyboxStore, commandList, psoRender3D_.get());
}

/// @brief コマンドリストに登録する
/// @param cameraStore 
/// @param skyboxStore 
/// @param commandList 
/// @param name 
/// @param pso 
void Engine::Render3DStore::Register(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, const std::string& name)
{
	// コマンドリストの登録する
	dataTable_[nameTable_[name]]->Register(cameraStore, skyboxStore, commandList, psoRender3D_.get());
}