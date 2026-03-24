#include "Render3DStore.h"
#include "Render3DData/Render3DStaticModelData/Render3DStaticModelData.h"
#include "Render3DData/Render3DAnimationModelData/Render3DAnimationModelData.h"
#include "Render3DData/Render3DSkinningModelData/Render3DSkinningModelData.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

/// @brief コンストラクタ
Engine::Render3DStore::Render3DStore()
{
	// パラメータの生成
	parameter_ = std::make_unique<Render3DParameter>("Primitive");
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
	// 静的モデルデータ
	for (auto& data : dataTable_)
	{
		// 静的モデル
		if (data->GetType() == Render3D::Type::StaticModel)
		{
			auto p = static_cast<Render3DStaticModelData*>(data.get());
			p->Register(viewProjection, commandList, pso);
		}

		// アニメーションモデル
		if (data->GetType() == Render3D::Type::AnimationModel)
		{
			auto p = static_cast<Render3DAnimationModelData*>(data.get());
			p->Register(viewProjection, commandList, pso);
		}

		// スキニングモデル
		if (data->GetType() == Render3D::Type::SkinningModel)
		{
			auto p = static_cast<Render3DSkinningModelData*>(data.get());
			p->Register(viewProjection, commandList, pso);
		}
	}
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
	ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton,const std::string& name, Render3D::Type type, Log* log)
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
		std::unique_ptr<Render3DStaticModelData> data = std::make_unique<Render3DStaticModelData>(name, hModel, handle, parameter_.get());
		data->Initialize(modelStore_, textureStore_, lightStore_, device, log);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// アニメーションモデル
	if (type == Render3D::Type::AnimationModel)
	{
		std::unique_ptr<Render3DAnimationModelData> data = std::make_unique<Render3DAnimationModelData>(name, hModel, hAnimation, handle, parameter_.get());
		data->Initialize(modelStore_, textureStore_, animationStore_, lightStore_, device, log);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// スキニングモデル
	if (type == Render3D::Type::SkinningModel)
	{
		std::unique_ptr<Render3DSkinningModelData> data = std::make_unique<Render3DSkinningModelData>(name, hModel,hAnimation, hSkeleton, handle, parameter_.get());
		data->Initialize(modelStore_, textureStore_, animationStore_, skeletonStore_, lightStore_, heap_, device, commandList, log);
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
void Engine::Render3DStore::Register(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, Render3DHandle handle, BasePSOModel* pso)
{
#ifdef _DEVELOPMENT
	// 描画している者のみGuizmo操作可能
	dataTable_[handle]->DebugGuizmo(cameraStore);
#endif

	// 静的モデル
	if (dataTable_[handle]->GetType() == Render3D::Type::StaticModel)
	{
		auto p = static_cast<Render3DStaticModelData*>(dataTable_[handle].get());
		p->Register(cameraStore, skyboxStore, commandList, pso);
		return;
	}

	// アニメーションモデル
	if (dataTable_[handle]->GetType() == Render3D::Type::AnimationModel)
	{
		auto p = static_cast<Render3DAnimationModelData*>(dataTable_[handle].get());
		p->Register(cameraStore, skyboxStore, commandList, pso);
		return;
	}

	// スキニングモデル
	if (dataTable_[handle]->GetType() == Render3D::Type::SkinningModel)
	{
		auto p = static_cast<Render3DSkinningModelData*>(dataTable_[handle].get());
		p->Register(cameraStore, skyboxStore, commandList, pso);
		return;
	}
}

/// @brief コマンドリストに登録する
/// @param cameraStore 
/// @param skyboxStore 
/// @param commandList 
/// @param name 
/// @param pso 
void Engine::Render3DStore::Register(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, const std::string& name, BasePSOModel* pso)
{
#ifdef _DEVELOPMENT
	// 描画している者のみGuizmo操作可能
	dataTable_[nameTable_[name]]->DebugGuizmo(cameraStore);
#endif

	// 静的モデル
	if (dataTable_[nameTable_[name]]->GetType() == Render3D::Type::StaticModel)
	{
		auto p = static_cast<Render3DStaticModelData*>(dataTable_[nameTable_[name]].get());
		p->Register(cameraStore, skyboxStore, commandList, pso);
		return;
	}

	// アニメーションモデル
	if (dataTable_[nameTable_[name]]->GetType() == Render3D::Type::AnimationModel)
	{
		auto p = static_cast<Render3DAnimationModelData*>(dataTable_[nameTable_[name]].get());
		p->Register(cameraStore, skyboxStore, commandList, pso);
		return;
	}

	// スキニングモデル
	if (dataTable_[nameTable_[name]]->GetType() == Render3D::Type::SkinningModel)
	{
		auto p = static_cast<Render3DSkinningModelData*>(dataTable_[nameTable_[name]].get());
		p->Register(cameraStore, skyboxStore, commandList, pso);
		return;
	}
}

/// @brief デバッグパラメータ
void Engine::Render3DStore::DebugParameter()
{
#ifdef _DEVELOPMENT
	// データ更新
	for (auto& data : dataTable_)data->DebugParameter();
#endif
}