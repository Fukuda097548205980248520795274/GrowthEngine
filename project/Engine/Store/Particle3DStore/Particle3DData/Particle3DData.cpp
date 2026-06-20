#include "Particle3DData.h"
#include "PSO/ComputePSO/BaseComputePSO.h"
#include "PSO/PSOModel/BasePSOModel.h"
#include "Store/ModelStore/ModelStore.h"
#include "Store/TextureStore/TextureStore.h"
#include "GrowthEngine.h"
#include "Parameter/Particle3DParameter/Particle3DParameter.h"
#include "Store/Camera3DStore/Camera3DStore.h"
#include "Func/RandomFunc/RandomFunc.h"
#include <numbers>

/// @brief 初期化
/// @param device 
/// @param commandList 
/// @param log 
void Engine::Particle3DData::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Heap* heap, Particle3DParameter* parameter,
	ModelStore* modelStore, TextureStore* textureStore, BasePSOModel* psoDraw, BaseComputePSO* psoInit, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(commandList);
	assert(psoDraw);
	assert(psoInit);
	assert(modelStore);
	assert(textureStore);
	assert(parameter);
	assert(heap);

	// 0は作成できない
	assert(emitterNum_ > 0);

	// 引数を受け取る
	psoDraw_ = psoDraw;
	modelStore_ = modelStore;
	textureStore_ = textureStore;
	parameter_ = parameter;

	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// パラメータの生成と初期化
	param_ = std::make_unique<Particle3D::Param>();
	param_->shape = Particle3D::EmitterShape::Point;
	param_->radius1 = 1.0f;
	param_->radius3 = Vector3(1.0f, 1.0f, 1.0f);
	param_->color.start = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	param_->color.end = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	param_->color.randomColor = false;
	param_->color.startAlpha = 1.0f;
	param_->color.endAlpha = 1.0f;
	param_->scale.start = Vector3(1.0f, 1.0f, 1.0f);
	param_->scale.end = Vector3(1.0f, 1.0f, 1.0f);
	param_->scale.isRandomStart = false;
	param_->scale.startA = Vector3(1.0f, 1.0f, 1.0f);
	param_->scale.startB = Vector3(1.0f, 1.0f, 1.0f);
	param_->scale.isRandomEnd = false;
	param_->scale.endA = Vector3(1.0f, 1.0f, 1.0f);
	param_->scale.endB = Vector3(1.0f, 1.0f, 1.0f);
	param_->rotate.isRandom = false;
	param_->rotate.axis = Vector3(0.0f, 1.0f, 0.0f);
	param_->rotate.start = 0.0f;
	param_->rotate.end = 0.0f;
	param_->lifeTime.min = 2.0f;
	param_->lifeTime.max = 2.0f;
	param_->emitTime.isLoop = true;
	param_->emitTime.pause = 0.0f;
	param_->emitTime.emit = 0.1f;
	param_->speed.start = 6.0f;
	param_->speed.end = 6.0f;
	param_->blendMode = BlendMode::kAdd;
	param_->count = 1;
	param_->frequency = 0.1f;
	param_->enableBillboard = false;
	param_->enableSoftParticle = false;
	param_->alignToDirection = false;
	param_->attract.enableAttract = false;
	param_->attract.positionType = Particle3D::AttractPostitionType::Direction;
	param_->attract.attractCenter = Vector3(0.0f, 0.0f, 0.0f);
	param_->attract.attractDirection = Vector3(0.0f, 1.0f, 0.0f);
	param_->attract.attractLength = 1.0f;
	param_->attract.attractPosition = Vector3(0.0f, 0.0f, 0.0f);
	param_->attract.attractAcceleration = 1.0f;
	param_->attract.swapEmitterAttract = false;
	param_->hModel = hModel_;

	// エミッターのインデックス
	int emitterIndex = 0;

	// エミッターの設定
	param_->emitter.resize(emitterNum_);
	for (auto& emitter : param_->emitter)
	{
		emitter.name = "Emitter" + std::to_string(emitterIndex);
		emitter.position = Vector3(0.0f, 0.0f, 0.0f);
        emitter.isStart = false;
		emitter.timer = 0.0f;

		// エミッターインデックスを加算する
		emitterIndex++;
	}

	// テクスチャを取得する
	hTexture_ = modelStore_->GetModelData(param_->hModel).meshes[0].material.handle;
	param_->hTexture = hTexture_;

	// テクスチャのファイルパスを取得する
	textureFilePath_ = textureStore_->GetFilePath(param_->hTexture);

	// グループを設定する
	group_ = "Particle3D_" + name_;

	if (parameter_)
	{
		emitterIndex = 0;

		for(auto& emitter : param_->emitter)
		{
			parameter_->SetValue(group_, "Emitter" + std::to_string(emitterIndex) + "_Name", &emitter.name);
			parameter_->SetValue(group_, "Emitter" + std::to_string(emitterIndex) + "_Timer", &emitter.timer);
			parameter_->SetValue(group_, "Emitter" + std::to_string(emitterIndex) + "_Position", &emitter.position);
			parameter_->SetValue(group_, "Emitter" + std::to_string(emitterIndex) + "_IsStart", &emitter.isStart);

			emitterIndex++;
		}

		// パラメータを登録する
		parameter_->SetValue(group_, "Shape", &param_->shape);
		parameter_->SetValue(group_, "Radius1", &param_->radius1);
		parameter_->SetValue(group_, "Radius3", &param_->radius3);
		parameter_->SetValue(group_, "StartColor", &param_->color.start);
		parameter_->SetValue(group_, "EndColor", &param_->color.end);
		parameter_->SetValue(group_, "RandomColor", &param_->color.randomColor);
		parameter_->SetValue(group_, "StartAlpha", &param_->color.startAlpha);
		parameter_->SetValue(group_, "EndAlpha", &param_->color.endAlpha);
		parameter_->SetValue(group_, "StartScale", &param_->scale.start);
		parameter_->SetValue(group_, "EndScale", &param_->scale.end);
		parameter_->SetValue(group_, "IsRandomStartScale", &param_->scale.isRandomStart);
		parameter_->SetValue(group_, "IsRandomEndScale", &param_->scale.isRandomEnd);
		parameter_->SetValue(group_, "StartScaleA", &param_->scale.startA);
		parameter_->SetValue(group_, "StartScaleB", &param_->scale.startB);
		parameter_->SetValue(group_, "EndScaleA", &param_->scale.endA);
		parameter_->SetValue(group_, "EndScaleB", &param_->scale.endB);
		parameter_->SetValue(group_, "IsRandomRotate", &param_->rotate.isRandom);
		parameter_->SetValue(group_, "RotateAxis", &param_->rotate.axis);
		parameter_->SetValue(group_, "StartRotate", &param_->rotate.start);
		parameter_->SetValue(group_, "EndRotate", &param_->rotate.end);
		parameter_->SetValue(group_, "MinLifeTime", &param_->lifeTime.min);
		parameter_->SetValue(group_, "MaxLifeTime", &param_->lifeTime.max);
		parameter_->SetValue(group_, "IsLoop", &param_->emitTime.isLoop);
		parameter_->SetValue(group_, "EmitTime", &param_->emitTime.emit);
		parameter_->SetValue(group_, "PauseTime", &param_->emitTime.pause);
		parameter_->SetValue(group_, "StartSpeed", &param_->speed.start);
		parameter_->SetValue(group_, "EndSpeed", &param_->speed.end);
		parameter_->SetValue(group_, "BlendMode", &param_->blendMode);
		parameter_->SetValue(group_, "Count", &param_->count);
		parameter_->SetValue(group_, "Frequency", &param_->frequency);
		parameter_->SetValue(group_, "EnableBillboard", &param_->enableBillboard);
		parameter_->SetValue(group_, "EnableSoftParticle", &param_->enableSoftParticle);
		parameter_->SetValue(group_, "AlignToDirection", &param_->alignToDirection);
		parameter_->SetValue(group_, "EnableAttract", &param_->attract.enableAttract);
		parameter_->SetValue(group_, "AttractDirection", &param_->attract.attractDirection);
		parameter_->SetValue(group_, "AttractLength", &param_->attract.attractLength);
		parameter_->SetValue(group_, "AttractPosition", &param_->attract.attractPosition);
		parameter_->SetValue(group_, "SwapEmitterAttract", &param_->attract.swapEmitterAttract);
		parameter_->SetValue(group_, "AttractPositionType", &param_->attract.positionType);
		parameter_->SetValue(group_, "TextureFilePath", &textureFilePath_);
		
		// 値を反映させる
		parameter_->RegisterGroupDataReflection(group_);
		param_->hTexture = textureStore_->GetHandle(textureFilePath_);
	}


	// パーティクルリソースを生成する
	particleResource_ = std::make_unique<RWSTructuredBufferResource<Particle3DDataForGPU>>();
	particleResource_->Initialize(device, commandList, heap, numInstance_, log);

	// パーティクル数リソースを生成する
	particleNumResource_ = std::make_unique<ConstantBufferResource<ParticleNumDataForGPU>>();
	particleNumResource_->Initialize(device,log);
	particleNumResource_->data_->particleNum = numInstance_;
	particleNumResource_->data_->emitterNum = emitterNum_;

	// パーティクルビューリソースを生成する
	particleViewResource_ = std::make_unique<ConstantBufferResource<ParticlePreViewDataForGPU>>();
	particleViewResource_->Initialize(device, log);

	// 放出設定リソースを生成する
	emitOptionResource_ = std::make_unique<ConstantBufferResource<Particle3DEmitOptionDataForGPU>>();
	emitOptionResource_->Initialize(device, log);
	emitOptionResource_->data_->startColor = param_->color.start;
	emitOptionResource_->data_->endColor = param_->color.end;
	emitOptionResource_->data_->startScale = param_->scale.start;
	emitOptionResource_->data_->endScale = param_->scale.end;
	emitOptionResource_->data_->minLifeTime = param_->lifeTime.min;
	emitOptionResource_->data_->maxLifeTime = param_->lifeTime.max;
	emitOptionResource_->data_->startSpeed = param_->speed.start;
	emitOptionResource_->data_->endSpeed = param_->speed.end;
	emitOptionResource_->data_->startRotation = ToQuaternion(param_->rotate.start, param_->rotate.axis);
	emitOptionResource_->data_->endRotation = ToQuaternion(param_->rotate.end, param_->rotate.axis);

	// エミッター形状リソースを生成する
	particleEmitterShapeResource_ = std::make_unique<ConstantBufferResource<Particle3DEmitterShapeDataForGPU>>();
	particleEmitterShapeResource_->Initialize(device, log);
	particleEmitterShapeResource_->data_->radius1 = param_->radius1;
	particleEmitterShapeResource_->data_->radius3 = param_->radius3;

	// エミッターリソースを生成する
	emitterResource_ = std::make_unique<StructuredBufferResource<Particle3DEmitterDataForGPU>>();
	emitterResource_->Initialize(device, heap, emitterNum_, log);

	// 引力リソースを生成する
	particleAttractResource_ = std::make_unique<ConstantBufferResource<Particle3DAttractDataForGPU>>();
	particleAttractResource_->Initialize(device, log);
	particleAttractResource_->data_->position = param_->attract.attractCenter + (param_->attract.attractDirection * param_->attract.attractLength);
	particleAttractResource_->data_->acceleration = param_->attract.attractAcceleration;

	// パーティクルフレームリソースを生成する
	particlePerFrameResource_ = std::make_unique<ConstantBufferResource<ParticlePerFrameDataForGPU>>();
	particlePerFrameResource_->Initialize(device, log);
	particlePerFrameResource_->data_->deltaTime = 0.0f;
	particlePerFrameResource_->data_->time = 0.0f;

	// フリーリストインデックスリソースを生成する
	freeListIndexResource_ = std::make_unique<RWSTructuredBufferResource<int32_t>>();
	freeListIndexResource_->Initialize(device, commandList, heap, 1, log);

	// フリーリストリソースを生成する
	freeListResource_ = std::make_unique<RWSTructuredBufferResource<uint32_t>>();
	freeListResource_->Initialize(device, commandList, heap, numInstance_, log);

	// 有効化フラグリソースを生成する
	enableResource_ = std::make_unique<ConstantBufferResource<ParticleEnableDataForGPU>>();
	enableResource_->Initialize(device, log);


	/*-----------------------
	    パーティクルの初期化
	-----------------------*/
	
	// PSOの設定
	psoInit->Register(commandList);

	// パーティクルリソースを登録する
	particleResource_->RegisterComputeUAV(commandList, 0);

	// パーティクル数リソースを登録する
	particleNumResource_->RegisterCompute(commandList, 1);

	// フリーリストインデックスリソースを登録する
	freeListIndexResource_->RegisterComputeUAV(commandList, 2);

	// フリーリストリソースを登録する
	freeListResource_->RegisterComputeUAV(commandList, 3);

	// ディスパッチする
	commandList->Dispatch((numInstance_ + 255) / 256, 1, 1);
}

/// @brief リセット
void Engine::Particle3DData::Reset()
{
	if (parameter_->IsFileFound(group_))
	{
		// グループを登録及び反映
		parameter_->RegisterGroupDataReflection(group_);
		param_->hTexture = textureStore_->GetHandle(textureFilePath_);
	}
	else
	{
		param_->shape = Particle3D::EmitterShape::Point;
		param_->radius1 = 1.0f;
		param_->radius3 = Vector3(1.0f, 1.0f, 1.0f);
		param_->color.start = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		param_->color.end = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		param_->color.randomColor = false;
		param_->color.startAlpha = 1.0f;
		param_->color.endAlpha = 1.0f;
		param_->scale.start = Vector3(1.0f, 1.0f, 1.0f);
		param_->scale.end = Vector3(1.0f, 1.0f, 1.0f);
		param_->scale.isRandomStart = false;
		param_->scale.isRandomEnd = false;
		param_->scale.startA = Vector3(1.0f, 1.0f, 1.0f);
		param_->scale.startB = Vector3(1.0f, 1.0f, 1.0f);
		param_->scale.endA = Vector3(1.0f, 1.0f, 1.0f);
		param_->scale.endB = Vector3(1.0f, 1.0f, 1.0f);
		param_->rotate.isRandom = false;
		param_->rotate.axis = Vector3(0.0f, 1.0f, 0.0f);
		param_->rotate.start = 0.0f;
		param_->rotate.end = 0.0f;
		param_->lifeTime.min = 2.0f;
		param_->lifeTime.max = 2.0f;
		param_->emitTime.isLoop = true;
		param_->emitTime.pause = 0.0f;
		param_->emitTime.emit = 0.1f;
		param_->speed.start = 6.0f;
		param_->speed.end = 6.0f;
		param_->blendMode = BlendMode::kAdd;
		param_->count = 1;
		param_->frequency = 0.1f;
		param_->enableBillboard = false;
		param_->enableSoftParticle = false;
		param_->alignToDirection = false;
		param_->attract.enableAttract = false;
		param_->attract.attractDirection = Vector3(0.0f, 1.0f, 0.0f);
		param_->attract.attractLength = 1.0f;
		param_->attract.attractAcceleration = 1.0f;
		param_->attract.attractPosition = Vector3(0.0f, 0.0f, 0.0f);
		param_->attract.positionType = Particle3D::AttractPostitionType::Direction;
		param_->attract.swapEmitterAttract = false;
		param_->hModel = hModel_;
		param_->hTexture = hTexture_;

		for (auto& emitter : param_->emitter)
		{
			emitter.position = Vector3(0.0f, 0.0f, 0.0f);
            emitter.isStart = false;
			emitter.timer = 0.0f;
		}
	}

	// エミッターのインデックスをリセットする
	useCurrentEmitterIndex_ = 0;

	// ロードしたこととする
	isLoad_ = true;
}

/// @brief シーン前のリセット処理
void Engine::Particle3DData::PerSceneReset()
{
	isLoad_ = false;
}

/// @brief 更新処理
/// @param commandList 
/// @param psoEmitter 
/// @param psoUpdate 
void Engine::Particle3DData::Update(ID3D12GraphicsCommandList* commandList, BaseComputePSO* psoEmitter, BaseComputePSO* psoUpdate)
{
	// ロードしていなかったら処理しない
	if (!isLoad_)return;

	// nullptrチェック
	assert(commandList);
	assert(psoEmitter);
	assert(psoUpdate);

	// デルタタイム
	float dt = engine_->GetDeltaTime() * engine_->GetTimeScale();

	// エミッターを更新する
	for (int i = 0; i < static_cast<int32_t>(emitterNum_); ++i)
	{
		if (param_->emitter[i].isStart)
		{
			// 放出時間のタイマーを進める
			param_->emitter[i].timer += dt;

			if (param_->emitter[i].timer >= param_->emitTime.pause)
			{
				// 放出時間を過ぎたときの処理
				if (param_->emitter[i].timer >= param_->emitTime.pause + param_->emitTime.emit)
				{
					if (param_->emitTime.isLoop)
					{
						// タイマーをリセットする
						param_->emitter[i].timer = 0.0f;
					}
					else
					{
						// 放出終了
						param_->emitter[i].isStart = false;
					}
				}

				// エミッターの位置と放出数、放出間隔を更新する
				emitterResource_->data_[i].position = Vector4(param_->emitter[i].position.x, param_->emitter[i].position.y, param_->emitter[i].position.z, 1.0f);
				emitterResource_->data_[i].count = param_->count;
				emitterResource_->data_[i].frequency = param_->frequency;

				// 放出間隔のタイマーを進める
				emitterResource_->data_[i].frequencyTimer += dt;
				emitterResource_->data_[i].frequencyTimer = std::min(emitterResource_->data_[i].frequencyTimer, emitterResource_->data_[i].frequency);

				// タイマーが時間を超えたら放出する
				if (emitterResource_->data_[i].frequencyTimer >= emitterResource_->data_[i].frequency)
				{
					emitterResource_->data_[i].emit = 1;
					emitterResource_->data_[i].frequencyTimer = 0.0f;
				}
				else
				{
					// 放出しない
					emitterResource_->data_[i].emit = 0;
				}
			}
		}
		else
		{
			// 放出しない
			emitterResource_->data_[i].emit = 0;
		}
	}


	particlePerFrameResource_->data_->time += dt;
	particlePerFrameResource_->data_->time = std::fmod(particlePerFrameResource_->data_->time, 1000.0f); // 時間が大きくなりすぎないようにする

	// デルタタイムを取得する
	particlePerFrameResource_->data_->deltaTime = dt;

	// 色のランダムを有効にしているかどうかで処理を分ける
	if (param_->color.randomColor)
	{
		emitOptionResource_->data_->startColor = Vector4(GetRandomRange(0.0f, 1.0f), GetRandomRange(0.0f, 1.0f), GetRandomRange(0.0f, 1.0f), param_->color.startAlpha);
		emitOptionResource_->data_->endColor = Vector4(GetRandomRange(0.0f, 1.0f), GetRandomRange(0.0f, 1.0f), GetRandomRange(0.0f, 1.0f), param_->color.endAlpha);
	}
	else
	{
		// エミッタの色を更新する
		emitOptionResource_->data_->startColor = param_->color.start;
		emitOptionResource_->data_->endColor = param_->color.end;
	}

	// 大きさのランダムを有効にしているかどうかで処理を分ける
	if (param_->scale.isRandomStart)
	{
		emitOptionResource_->data_->startScale.x = GetRandomRange(param_->scale.startA.x, param_->scale.startB.x);
		emitOptionResource_->data_->startScale.y = GetRandomRange(param_->scale.startA.y, param_->scale.startB.y);
		emitOptionResource_->data_->startScale.z = GetRandomRange(param_->scale.startA.z, param_->scale.startB.z);
	}
	else
	{
		emitOptionResource_->data_->startScale = param_->scale.start;
	}

	// 大きさのランダムを有効にしているかどうかで処理を分ける
	if (param_->scale.isRandomEnd)
	{
		emitOptionResource_->data_->endScale.x = GetRandomRange(param_->scale.endA.x, param_->scale.endB.x);
		emitOptionResource_->data_->endScale.y = GetRandomRange(param_->scale.endA.y, param_->scale.endB.y);
		emitOptionResource_->data_->endScale.z = GetRandomRange(param_->scale.endA.z, param_->scale.endB.z);
	}
	else
	{
		emitOptionResource_->data_->endScale = param_->scale.end;
	}

	// エミッタのパラメータを更新する
	emitOptionResource_->data_->minLifeTime = param_->lifeTime.min;
	emitOptionResource_->data_->maxLifeTime = param_->lifeTime.max;
	emitOptionResource_->data_->startSpeed = param_->speed.start;
	emitOptionResource_->data_->endSpeed = param_->speed.end;

	// 回転はクォータニオンに変換して渡す
	if (param_->rotate.isRandom)
	{
		if (param_->enableBillboard)
		{
			float randomStart = GetRandomRange(0.0f, std::numbers::pi_v<float>);
			emitOptionResource_->data_->startRotation = ToQuaternion(randomStart, Vector3(0.0f, 0.0f, 1.0f));
			emitOptionResource_->data_->endRotation = ToQuaternion(randomStart, Vector3(0.0f, 0.0f, 1.0f));
		}
		else
		{
			Vector3 randomAxis = Vector3(GetRandomRange(-1.0f, 1.0f), GetRandomRange(-1.0f, 1.0f), GetRandomRange(-1.0f, 1.0f)).Normalize();
			float randomStart = GetRandomRange(0.0f, std::numbers::pi_v<float>);
			emitOptionResource_->data_->startRotation = ToQuaternion(randomStart, randomAxis);
			emitOptionResource_->data_->endRotation = ToQuaternion(randomStart, randomAxis);
		}
	}
	else
	{
		param_->rotate.axis = param_->rotate.axis.Normalize();
		emitOptionResource_->data_->startRotation = ToQuaternion(param_->rotate.start, param_->rotate.axis);
		emitOptionResource_->data_->endRotation = ToQuaternion(param_->rotate.end, param_->rotate.axis);
	}

	// エミッター形状のパラメータを更新する
	particleEmitterShapeResource_->data_->radius1 = param_->radius1;
	particleEmitterShapeResource_->data_->radius3 = param_->radius3;

	// 引力の位置のタイプによって引力の位置を更新する
	switch (param_->attract.positionType)
	{
	case Particle3D::AttractPostitionType::Direction:
		// 引力の位置をエミッターの位置から引力の方向と長さで計算する
		param_->attract.attractDirection = param_->attract.attractDirection.Normalize();
		particleAttractResource_->data_->position = param_->emitter[0].position + (param_->attract.attractDirection * param_->attract.attractLength);
		break;

	case Particle3D::AttractPostitionType::Position:
		// 引力の位置を引力の位置で設定する
		particleAttractResource_->data_->position = param_->attract.attractPosition;
		break;
	}

	particleAttractResource_->data_->acceleration = param_->attract.attractAcceleration;

	if(param_->attract.swapEmitterAttract)
	{
		// エミッターと引力の位置を入れ替える
		Vector3 temp = particleAttractResource_->data_->position;
		particleAttractResource_->data_->position = param_->emitter[0].position;
		param_->emitter[0].position = temp;
	}


	/*------------
	   エミッター
	------------*/

	// PSOの設定
	psoEmitter->Register(commandList);

	// パーティクルリソースを登録する
	particleResource_->RegisterComputeUAV(commandList, 0);

	// 放出設定リソースを登録する
	emitOptionResource_->RegisterCompute(commandList, 1);

	// パーティクル数リソースを登録する
	particleNumResource_->RegisterCompute(commandList, 2);

	// パーティクルフレームリソースを登録する
	particlePerFrameResource_->RegisterCompute(commandList, 3);

	// フリーリストインデックスリソースを登録する
	freeListIndexResource_->RegisterComputeUAV(commandList, 4);

	// フリーリストリソースを登録する
	freeListResource_->RegisterComputeUAV(commandList, 5);

	// エミッターリソースを登録する
	emitterResource_->RegisterCompute(commandList, 6);

	if (param_->shape != Particle3D::EmitterShape::Point)
	{
		// エミッター形状リソースを登録する
		particleEmitterShapeResource_->RegisterCompute(commandList, 7);
	}

	// ディスパッチする
	commandList->Dispatch((emitterNum_ + 255) / 256, 1, 1);


	// UAVバリアを張る
	UAVBarrier(particleResource_->GetResource(), commandList);
	UAVBarrier(freeListIndexResource_->GetResource(), commandList);
	UAVBarrier(freeListResource_->GetResource(), commandList);


	/*-------------
	    更新処理
	-------------*/

	// PSOの設定
	psoUpdate->Register(commandList);

	// パーティクルリソースを登録する
	particleResource_->RegisterComputeUAV(commandList, 0);

	// パーティクル数リソースを登録する
	particleNumResource_->RegisterCompute(commandList, 1);

	// パーティクルフレームリソースを登録する
	particlePerFrameResource_->RegisterCompute(commandList, 2);

	// フリーリストインデックスリソースを登録する
	freeListIndexResource_->RegisterComputeUAV(commandList, 3);

	// フリーリストリソースを登録する
	freeListResource_->RegisterComputeUAV(commandList, 4);

	if(param_->attract.enableAttract)
	{
		// 引力リソースを登録する
		particleAttractResource_->RegisterCompute(commandList, 5);
	}

	// ディスパッチする
	commandList->Dispatch((numInstance_ + 255) / 256, 1, 1);
}

/// @brief エミッターのインデックスを名前から取得する
/// @param name 
/// @return 
int32_t Engine::Particle3DData::GetEmitterIndex(const std::string& name) const
{
	for (size_t i = 0; i < param_->emitter.size(); ++i)
	{
		if (param_->emitter[i].name == name)
		{
			return static_cast<int32_t>(i);
		}
	}

	// 見つからなかったら-1を返す
	return -1;
}

/// @brief エミッターのパラメータを取得する（自動でインデックスを進める）
/// @return 
int32_t Engine::Particle3DData::GetEmitterIndex()
{
	// 現在のインデックスを一時保存
	int32_t index = useCurrentEmitterIndex_;

	// 次のインデックスに進める（エミッターの最大数を超えたら0に戻す）
	useCurrentEmitterIndex_ = (useCurrentEmitterIndex_ + 1) % emitterNum_;

	return index;
}

/// @brief 描画処理
/// @param commandList 
/// @param psoDraw 
void Engine::Particle3DData::Draw(ID3D12GraphicsCommandList* commandList, const Camera3DStore* cameraStore, OffscreenResource* offscreenResource, DepthResource* depthResource)
{
	// ロードしていなかったら処理しない
	if (!isLoad_)return;


	// nullptrチェック
	assert(commandList);

	// レンダーターゲットと深度バッファをセットする
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = offscreenResource->GetRtvCpuHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthResource->GetDsvReadOnlyCpuHandle(); // Read-Only版
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	// バリアを張る
	particleResource_->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	depthResource->Barrier(commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// データを渡す
	particleViewResource_->data_->viewProjection = cameraStore->GetCamera3D().GetCurrentVPMatrix();
	enableResource_->data_->softParticle = static_cast<int32_t>(param_->enableSoftParticle);
	enableResource_->data_->alignToDirection = static_cast<int32_t>(param_->alignToDirection);

	// ビルボードの有効化
	if (param_->enableBillboard)
	{
		particleViewResource_->data_->billboard = Make3DRotateMatrix4x4(cameraStore->GetCamera3D().GetQuaternion());
	}
	else
	{
		particleViewResource_->data_->billboard = MakeIdentityMatrix4x4();
	}



	/*------------------------
	    コマンドリストに登録する
	------------------------*/

	// PSOの設定
	psoDraw_->Register(commandList , param_->blendMode);

	// 頂点を登録する
	modelStore_->Register(commandList, param_->hModel, 0);

	// パーティクルリソースを登録する
	particleResource_->RegisterGraphicsSRV(commandList, 0);

	// パーティクルビューリソースを登録する
	particleViewResource_->RegisterGraphics(commandList, 1);

	// テクスチャを登録する
	commandList->SetGraphicsRootDescriptorTable(2, textureStore_->GetSrvGpuHandle(param_->hTexture));

	// 深度リソースを登録する
	depthResource->Register(commandList, 3);

	// カメラを登録する
	cameraStore->RegisterCameraResource(commandList, 4);

	// 有効化フラグを登録する
	enableResource_->RegisterGraphics(commandList, 5);

	// カメラを登録する
	cameraStore->RegisterCameraResource(commandList, 6);

	// 有効化フラグを登録する
	enableResource_->RegisterGraphics(commandList, 7);

	// ドローコール
	commandList->DrawIndexedInstanced(static_cast<UINT>(modelStore_->GetModelData(param_->hModel).meshes[0].indices.size()), numInstance_, 0, 0, 0);



	// バリアを張る
	depthResource->Barrier(commandList, D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	particleResource_->Barrier(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// 描画後は通常の深度バッファをセットする
	D3D12_CPU_DESCRIPTOR_HANDLE originalDsvHandle = depthResource->GetDsvCpuHandle(); // 通常版
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &originalDsvHandle);
}

/// @brief デバッグパラメータ
void Engine::Particle3DData::DebugParameter()
{
#ifdef _DEVELOPMENT

	// 読み込んでいないと処理しない
	if (!isLoad_)return;

	// モデル名
	if (ImGui::TreeNode(name_.c_str()))
	{
		// ブレンドモード
		const char* blendMode[] = { "None", "Normal", "Add", "Subtract", "Multiply", "Screen" };
		int32_t blendModeIndex = static_cast<int32_t>(param_->blendMode);
		if (ImGui::Combo("BlendMode", &blendModeIndex, blendMode, IM_ARRAYSIZE(blendMode)))
		{
			param_->blendMode = static_cast<BlendMode>(blendModeIndex);
		}
		
		ImGui::Text("\n");

		// エミッター
		if (ImGui::TreeNode("Emitter"))
		{
			for (auto& emitter : param_->emitter)
			{
				if (ImGui::TreeNode(emitter.name.c_str()))
				{
					// 放出開始
					if (ImGui::Button("Emit Start"))
					{
						emitter.isStart = true;
						emitter.timer = 0.0f;
					}

					// 位置
					ImGui::DragFloat3("Position", &emitter.position.x, 0.01f, -1000000.0f, 1000000.0f);

					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		
		ImGui::Text("\n");

		// テクスチャ
		ImGui::Text("Texture");

		ImGui::ImageButton(
			textureStore_->GetFilePath(param_->hTexture).c_str(),
			textureStore_->GetSrvGpuHandle(param_->hTexture).ptr,
			ImVec2(32.0f, 32.0f),
			ImVec2(0, 0),
			ImVec2(1, 1),
			ImVec4(0.2f, 0.2f, 0.2f, 1.0f),
			ImVec4(1, 1, 1, 1)
		);

		// --- ドロップ処理 ---
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_ID"))
			{
				int droppedIndex = *(const int*)payload->Data;

				// droppedIndex が dataTable_ の index
				// ここでマテリアルなどに設定する
				param_->hTexture = static_cast<uint32_t>(droppedIndex);
				textureFilePath_ = textureStore_->GetFilePath(param_->hTexture);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::Text("\n");

		// エミッターの図形
		const char* emitterType[] = { "Point", "AABB", "Sphere" };
		int32_t shapeIndex = static_cast<int32_t>(param_->shape);
		if (ImGui::Combo("Shape", &shapeIndex, emitterType, IM_ARRAYSIZE(emitterType)))
		{
			param_->shape = static_cast<Particle3D::EmitterShape>(shapeIndex);
		}

		// 図形ごとのパラメータ
		switch(param_->shape)
		{
		case Particle3D::EmitterShape::AABB:
			
			// 半径
			ImGui::DragFloat3("Radius", &param_->radius3.x, 0.01f, 0.0f, 100000.0f);

			break;
		case Particle3D::EmitterShape::Sphere:
			
			// 半径
			ImGui::DragFloat("Radius", &param_->radius1, 0.01f, 0.0f, 100000.0f);

			break;
		}

		ImGui::Text("\n");

		// 放出数
		ImGui::DragInt("Count", &param_->count, 1.0f, 0, static_cast<int32_t>(numInstance_));

		// 放出間隔
		ImGui::DragFloat("Frequency", &param_->frequency, 0.01f, 0.0f, 100000.0f);

		// ビルボード有効化
		ImGui::Checkbox("Billboard", &param_->enableBillboard);

		// ソフトパーティクル有効化
		ImGui::Checkbox("SoftParticle", &param_->enableSoftParticle);

		// 方向に合わせる
		ImGui::Checkbox("AlignToDirection", &param_->alignToDirection);


		// 生存期間
		if (ImGui::TreeNode("LifeTime"))
		{
			// 最小
			ImGui::DragFloat("Min", &param_->lifeTime.min, 0.01f, 0.0f, 100000.0f);

			// 最大
			ImGui::DragFloat("Max", &param_->lifeTime.max, 0.01f, 0.0f, 100000.0f);

			// 終了
			ImGui::TreePop();
		}

		// 放出時間
		if (ImGui::TreeNode("EmitTime"))
		{
			// ループ
			ImGui::Checkbox("Loop", &param_->emitTime.isLoop);

			// ポーズ
			ImGui::DragFloat("Pause", &param_->emitTime.pause, 0.01f, 0.0f, 100000.0f);

			// 放出
			ImGui::DragFloat("Emit", &param_->emitTime.emit, 0.01f, 0.0f, 100000.0f);

			// 終了
			ImGui::TreePop();
		}

		// 色
		if (ImGui::TreeNode("Color"))
		{
			// ランダムカラー
			ImGui::Checkbox("RandomColor", &param_->color.randomColor);

			// ランダムカラーでないときは開始色と終了色を設定できるようにする
			if (!param_->color.randomColor)
			{
				// 開始
				ImGui::ColorEdit4("Start", &param_->color.start.x);

				// 終了
				ImGui::ColorEdit4("End", &param_->color.end.x);
			}
			else
			{
				// ランダムカラーのときはアルファ値のみ設定できるようにする

				// 開始
				ImGui::SliderFloat("Start_Alpha", &param_->color.startAlpha, 0.0f, 1.0f);

				// 終了
				ImGui::SliderFloat("End_Alpha", &param_->color.endAlpha, 0.0f, 1.0f);
			}

			// 終了
			ImGui::TreePop();
		}

		// 大きさ
		if (ImGui::TreeNode("Scale"))
		{
			ImGui::Checkbox("RandomStart", &param_->scale.isRandomStart);

			if (param_->scale.isRandomStart)
			{
				ImGui::DragFloat3("StartA", &param_->scale.startA.x, 0.01f, 0.0f, 100000.0f);
				ImGui::DragFloat3("StartB", &param_->scale.startB.x, 0.01f, 0.0f, 100000.0f);
			}
			else
			{
				// 開始
				ImGui::DragFloat3("Start", &param_->scale.start.x, 0.01f, 0.0f, 100000.0f);
			}

			ImGui::Checkbox("RandomEnd", &param_->scale.isRandomEnd);

			if (param_->scale.isRandomEnd)
			{
				ImGui::DragFloat3("EndA", &param_->scale.endA.x, 0.01f, 0.0f, 100000.0f);
				ImGui::DragFloat3("EndB", &param_->scale.endB.x, 0.01f, 0.0f, 100000.0f);
			}
			else
			{
				// 終了
				ImGui::DragFloat3("End", &param_->scale.end.x, 0.01f, 0.0f, 100000.0f);
			}

			// 終了
			ImGui::TreePop();
		}

		// 回転
		if (ImGui::TreeNode("Rotate"))
		{
			ImGui::Checkbox("Random", &param_->rotate.isRandom);

			if (!param_->rotate.isRandom)
			{
				// 軸
				ImGui::DragFloat3("Axis", &param_->rotate.axis.x, 0.01f, -1.0f, 1.0f);

				// 開始
				ImGui::DragFloat("Start", &param_->rotate.start, 0.01f, -360.0f, 360.0f);

				// 終了
				ImGui::DragFloat("End", &param_->rotate.end, 0.01f, -360.0f, 360.0f);
			}

			// 終了
			ImGui::TreePop();
		}

		if (!param_->attract.enableAttract)
		{
			// 速度
			if (ImGui::TreeNode("Speed"))
			{
				// 開始
				ImGui::DragFloat("Start", &param_->speed.start, 0.01f, 0.0f, 100000.0f);

				// 終了
				ImGui::DragFloat("End", &param_->speed.end, 0.01f, 0.0f, 100000.0f);

				// 終了
				ImGui::TreePop();
			}
		}

		ImGui::Text("\n");

		// 引力有効化
		ImGui::Checkbox("Attract", &param_->attract.enableAttract);

		if (param_->attract.enableAttract)
		{
			// 引力と放出の場所を入れ替える
			ImGui::Checkbox("SwapEmitterAttract", &param_->attract.swapEmitterAttract);

			// 引力位置の種類
			const char* atrractPositionType[] = { "Direction", "Position" };
			int32_t atrractPositionIndex = static_cast<int32_t>(param_->attract.positionType);
			if (ImGui::Combo("AttractPositionType", &atrractPositionIndex, atrractPositionType, IM_ARRAYSIZE(atrractPositionType)))
			{
				param_->attract.positionType = static_cast<Particle3D::AttractPostitionType>(atrractPositionIndex);
			}

			switch (param_->attract.positionType)
			{
			case Particle3D::AttractPostitionType::Position:

				// 引力の位置
				ImGui::DragFloat3("AttractPosition", &param_->attract.attractPosition.x, 0.01f, -100000.0f, 100000.0f);

				break;

			case Particle3D::AttractPostitionType::Direction:

				// 引力の方向
				ImGui::DragFloat3("AttractDirection", &param_->attract.attractDirection.x, 0.01f, -1.0f, 1.0f);

				// 引力の距離
				ImGui::DragFloat("AttractLength", &param_->attract.attractLength, 0.01f, 0.0f, 100000.0f);

				break;
			}

			// 吸引加速度
			ImGui::DragFloat("AttractAcceleration", &param_->attract.attractAcceleration, 0.01f, 0.01f, 100000.0f);
		}

		

		ImGui::Text("\n");

		// 保存ボタン
		if (ImGui::Button("Save"))
		{
			parameter_->SaveFile(group_);
			std::string message = std::format("{} : saved.", group_);
			MessageBoxA(nullptr, message.c_str(), "RecordSetting", 0);
		}

		ImGui::Text("\n");

		// ロードボタン
		if (ImGui::Button("Load"))
		{
			parameter_->RegisterGroupDataReflection(group_);
			std::string message = std::format("{} : loaded.", group_);
			MessageBoxA(nullptr, message.c_str(), "RecordSetting", 0);
		}

		ImGui::Text("\n");

		// 終了
		ImGui::TreePop();
	}

#endif
}