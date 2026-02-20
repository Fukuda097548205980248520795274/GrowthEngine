#include "PrimitiveStore.h"
#include "PrimitiveData/PrimitiveStaticModelData/PrimitiveStaticModelData.h"
#include "PrimitiveData/PrimitiveAnimationModelData/PrimitiveAnimationModelData.h"
#include "PrimitiveData/PrimitiveSkinningModelData/PrimitiveSkinningModelData.h"

/// @brief 初期化
/// @param device 
/// @param compiler 
/// @param log 
void Engine::PrimitiveStore::Initialize(ID3D12Device* device, ShaderCompiler* compiler, DX12Heap* heap,
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
void Engine::PrimitiveStore::Update(ID3D12GraphicsCommandList* commandList)
{
	// データ更新
	for (auto& data : dataTable_)
	{
		data->Update();

		// スキニングモデル
		if (data->GetType() == Primitive::Type::SkinningModel)
		{
			auto p = static_cast<PrimitiveSkinningModelData*>(data.get());
			p->Skinning(commandList, psoSkinning_.get());
		}
	}
}

/// @brief シャドウマップ用の描画処理
/// @param commandList 
/// @param pso 
void Engine::PrimitiveStore::ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
	// 静的モデルデータ
	for (auto& data : dataTable_)
	{
		// 静的モデル
		if (data->GetType() == Primitive::Type::StaticModel)
		{
			auto p = static_cast<PrimitiveStaticModelData*>(data.get());
			p->Register(viewProjection, commandList, pso);
		}

		// アニメーションモデル
		if (data->GetType() == Primitive::Type::AnimationModel)
		{
			auto p = static_cast<PrimitiveAnimationModelData*>(data.get());
			p->Register(viewProjection, commandList, pso);
		}

		// スキニングモデル
		if (data->GetType() == Primitive::Type::SkinningModel)
		{
			auto p = static_cast<PrimitiveSkinningModelData*>(data.get());
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
PrimitiveHandle Engine::PrimitiveStore::Load(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, 
	ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton,const std::string& name, Primitive::Type type, Log* log)
{
	// 同じデータがあるかどうか
	for (auto& data : dataTable_)
	{
		if (data->GetName() == name && data->GetType() == type)
			return data->GetHandle();
	}

	// ハンドル
	PrimitiveHandle handle = static_cast<PrimitiveHandle>(dataTable_.size());


	// 静的モデル
	if (type == Primitive::Type::StaticModel)
	{
		std::unique_ptr<PrimitiveStaticModelData> data = std::make_unique<PrimitiveStaticModelData>(name, hModel, handle);
		data->Initialize(modelStore_, textureStore_, lightStore_, device, log);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// アニメーションモデル
	if (type == Primitive::Type::AnimationModel)
	{
		std::unique_ptr<PrimitiveAnimationModelData> data = std::make_unique<PrimitiveAnimationModelData>(name, hModel, hAnimation, handle);
		data->Initialize(modelStore_, textureStore_, animationStore_, lightStore_, device, log);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// スキニングモデル
	if (type == Primitive::Type::SkinningModel)
	{
		std::unique_ptr<PrimitiveSkinningModelData> data = std::make_unique<PrimitiveSkinningModelData>(name, hModel,hAnimation, hSkeleton, handle);
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
void Engine::PrimitiveStore::Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, PrimitiveHandle handle, BasePSOModel* pso)
{
	// 静的モデル
	if (dataTable_[handle]->GetType() == Primitive::Type::StaticModel)
	{
		auto p = static_cast<PrimitiveStaticModelData*>(dataTable_[handle].get());
		p->Register(viewProjection, commandList, pso);
	}

	// アニメーションモデル
	if (dataTable_[handle]->GetType() == Primitive::Type::AnimationModel)
	{
		auto p = static_cast<PrimitiveAnimationModelData*>(dataTable_[handle].get());
		p->Register(viewProjection, commandList, pso);
	}

	// スキニングモデル
	if (dataTable_[handle]->GetType() == Primitive::Type::SkinningModel)
	{
		auto p = static_cast<PrimitiveSkinningModelData*>(dataTable_[handle].get());
		p->Register(viewProjection, commandList, pso);
	}
}