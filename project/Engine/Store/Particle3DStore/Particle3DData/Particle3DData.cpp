#include "Particle3DData.h"
#include "PSO/ComputePSO/BaseComputePSO.h"
#include "PSO/PSOModel/BasePSOModel.h"
#include "Store/ModelStore/ModelStore.h"
#include "Store/TextureStore/TextureStore.h"
#include "GrowthEngine.h"
#include "Parameter/Particle3DParameter/Particle3DParameter.h"
#include "Store/Camera3DStore/Camera3DStore.h"

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

	// 引数を受け取る
	psoDraw_ = psoDraw;
	modelStore_ = modelStore;
	textureStore_ = textureStore;
	parameter_ = parameter;

	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// パラメータの生成と初期化
	param_ = std::make_unique<Particle3D::Param>();
	param_->position = Vector3(0.0f, 0.0f, 0.0f);
	param_->shape = Particle3D::EmitterShape::Point;
	param_->radius1 = 1.0f;
	param_->radius3 = Vector3(1.0f, 1.0f, 1.0f);
	param_->color.start = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	param_->color.end = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	param_->scale.start = 1.0f;
	param_->scale.end = 1.0f;
	param_->lifeTime.min = 2.0f;
	param_->lifeTime.max = 2.0f;
	param_->speed.start = 6.0f;
	param_->speed.end = 6.0f;
	param_->count = 1;
	param_->frequency = 0.1f;
	param_->enableBillboard = false;
	param_->enableAttract = false;
	param_->attractPos = Vector3(0.0f, 0.0f, 0.0f);
	param_->attractAcceleration = 1.0f;

	// グループを設定する
	group_ = "Particle3D_" + name_;

	if (parameter_)
	{
		// パラメータを登録する
		parameter_->SetValue(group_, "Position", &param_->position);
		parameter_->SetValue(group_, "Shape", &param_->shape);
		parameter_->SetValue(group_, "Radius1", &param_->radius1);
		parameter_->SetValue(group_, "Radius3", &param_->radius3);
		parameter_->SetValue(group_, "StartColor", &param_->color.start);
		parameter_->SetValue(group_, "EndColor", &param_->color.end);
		parameter_->SetValue(group_, "StartScale", &param_->scale.start);
		parameter_->SetValue(group_, "EndScale", &param_->scale.end);
		parameter_->SetValue(group_, "MinLifeTime", &param_->lifeTime.min);
		parameter_->SetValue(group_, "MaxLifeTime", &param_->lifeTime.max);
		parameter_->SetValue(group_, "StartSpeed", &param_->speed.start);
		parameter_->SetValue(group_, "EndSpeed", &param_->speed.end);
		parameter_->SetValue(group_, "Count", &param_->count);
		parameter_->SetValue(group_, "Frequency", &param_->frequency);
		parameter_->SetValue(group_, "EnableBillboard", &param_->enableBillboard);
		parameter_->SetValue(group_, "EnableAttract", &param_->enableAttract);
		parameter_->SetValue(group_, "AttractPos", &param_->attractPos);
		

		// グループを登録及び反映
		parameter_->RegisterGroupDataReflection(group_);
	}


	// パーティクルリソースを生成する
	particleResource_ = std::make_unique<RWSTructuredBufferResource<Particle3DDataForGPU>>();
	particleResource_->Initialize(device, commandList, heap, numInstance_, log);

	// パーティクル数リソースを生成する
	particleNumResource_ = std::make_unique<ConstantBufferResource<ParticleNumDataForGPU>>();
	particleNumResource_->Initialize(device,log);
	particleNumResource_->data_->num = numInstance_;

	// パーティクルビューリソースを生成する
	particleViewResource_ = std::make_unique<ConstantBufferResource<ParticlePreViewDataForGPU>>();
	particleViewResource_->Initialize(device, log);

	// エミッターリソースを生成する
	particleEmitterPointResource_ = std::make_unique<ConstantBufferResource<Particle3DEmitterPointDataForGPU>>();
	particleEmitterPointResource_->Initialize(device, log);

	particleEmitterPointResource_->data_->frequencyTimer = 0.0f;
	particleEmitterPointResource_->data_->emit = 0;

	particleEmitterPointResource_->data_->translate = param_->position;
	particleEmitterPointResource_->data_->count = param_->count;
	particleEmitterPointResource_->data_->frequency = param_->frequency;
	particleEmitterPointResource_->data_->startColor = param_->color.start;
	particleEmitterPointResource_->data_->endColor = param_->color.end;
	particleEmitterPointResource_->data_->startScale = param_->scale.start;
	particleEmitterPointResource_->data_->endScale = param_->scale.end;
	particleEmitterPointResource_->data_->minLifeTime = param_->lifeTime.min;
	particleEmitterPointResource_->data_->maxLifeTime = param_->lifeTime.max;
	particleEmitterPointResource_->data_->startSpeed = param_->speed.start;
	particleEmitterPointResource_->data_->endSpeed = param_->speed.end;

	// 引力リソースを生成する
	particleAttractResource_ = std::make_unique<ConstantBufferResource<Particle3DAttractDataForGPU>>();
	particleAttractResource_->Initialize(device, log);
	particleAttractResource_->data_->position = param_->position + param_->attractPos;
	particleAttractResource_->data_->acceleration = param_->attractAcceleration;

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


	// テクスチャを取得する
	hTexture_ = modelStore_->GetModelData(hModel_).meshes[0].material.handle;


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
	}
	else
	{
		param_->position = Vector3(0.0f, 0.0f, 0.0f);
		param_->shape = Particle3D::EmitterShape::Point;
		param_->radius1 = 1.0f;
		param_->radius3 = Vector3(1.0f, 1.0f, 1.0f);
		param_->color.start = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		param_->color.end = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		param_->scale.start = 1.0f;
		param_->scale.end = 1.0f;
		param_->lifeTime.min = 2.0f;
		param_->lifeTime.max = 2.0f;
		param_->speed.start = 6.0f;
		param_->speed.end = 6.0f;
		param_->count = 1;
		param_->frequency = 0.1f;
		param_->enableBillboard = false;
		param_->enableAttract = false;
		param_->attractPos = Vector3(0.0f, 0.0f, 0.0f);
		param_->attractAcceleration = 1.0f;
	}

	// ロードしたこととする
	isLoad_ = true;
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


	// タイマーを進める
	particleEmitterPointResource_->data_->frequencyTimer += engine_->GetDeltaTime();
	particlePerFrameResource_->data_->time += engine_->GetDeltaTime();

	// デルタタイムを取得する
	particlePerFrameResource_->data_->deltaTime = engine_->GetDeltaTime();

	// タイマーが時間を超えたら放出する
	if(particleEmitterPointResource_->data_->frequencyTimer >= particleEmitterPointResource_->data_->frequency)
	{
		particleEmitterPointResource_->data_->emit = 1;
		particleEmitterPointResource_->data_->frequencyTimer = 0.0f;
	}
	else
	{
		particleEmitterPointResource_->data_->emit = 0;
	}


	// エミッタのパラメータを更新する
	particleEmitterPointResource_->data_->translate = param_->position;
	particleEmitterPointResource_->data_->count = param_->count;
	particleEmitterPointResource_->data_->frequency = param_->frequency;
	particleEmitterPointResource_->data_->startColor = param_->color.start;
	particleEmitterPointResource_->data_->endColor = param_->color.end;
	particleEmitterPointResource_->data_->startScale = param_->scale.start;
	particleEmitterPointResource_->data_->endScale = param_->scale.end;
	particleEmitterPointResource_->data_->minLifeTime = param_->lifeTime.min;
	particleEmitterPointResource_->data_->maxLifeTime = param_->lifeTime.max;
	particleEmitterPointResource_->data_->startSpeed = param_->speed.start;
	particleEmitterPointResource_->data_->endSpeed = param_->speed.end;

	particleAttractResource_->data_->position = param_->position + param_->attractPos;
	particleAttractResource_->data_->acceleration = param_->attractAcceleration;


	/*------------
	   エミッター
	------------*/

	// PSOの設定
	psoEmitter->Register(commandList);

	// パーティクルリソースを登録する
	particleResource_->RegisterComputeUAV(commandList, 0);

	// エミッターリソースを登録する
	particleEmitterPointResource_->RegisterCompute(commandList, 1);

	// パーティクル数リソースを登録する
	particleNumResource_->RegisterCompute(commandList, 2);

	// パーティクルフレームリソースを登録する
	particlePerFrameResource_->RegisterCompute(commandList, 3);

	// フリーリストインデックスリソースを登録する
	freeListIndexResource_->RegisterComputeUAV(commandList, 4);

	// フリーリストリソースを登録する
	freeListResource_->RegisterComputeUAV(commandList, 5);

	// ディスパッチする
	commandList->Dispatch(1, 1, 1);


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

	// エミッターリソースを登録する
	particleEmitterPointResource_->RegisterCompute(commandList, 5);

	if(param_->enableAttract)
	{
		// 引力リソースを登録する
		particleAttractResource_->RegisterCompute(commandList, 6);
	}

	// ディスパッチする
	commandList->Dispatch((numInstance_ + 255) / 256, 1, 1);
}

/// @brief 描画処理
/// @param commandList 
/// @param psoDraw 
void Engine::Particle3DData::Draw(ID3D12GraphicsCommandList* commandList, const Camera3DStore* cameraStore)
{
	// ロードしていなかったら処理しない
	if (!isLoad_)return;


	// nullptrチェック
	assert(commandList);

	// バリアを張る
	particleResource_->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);


	// データを渡す
	particleViewResource_->data_->viewProjection = cameraStore->GetCamera3D().GetViewProjectionMatrix();

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
	psoDraw_->Register(commandList);

	// 頂点を登録する
	modelStore_->Register(commandList, hModel_, 0);

	// パーティクルリソースを登録する
	particleResource_->RegisterGraphicsSRV(commandList, 0);

	// パーティクルビューリソースを登録する
	particleViewResource_->RegisterGraphics(commandList, 1);

	// テクスチャを登録する
	commandList->SetGraphicsRootDescriptorTable(2, textureStore_->GetSrvGpuHandle(hTexture_));

	// ドローコール
	commandList->DrawIndexedInstanced(static_cast<UINT>(modelStore_->GetModelData(hModel_).meshes[0].indices.size()), numInstance_, 0, 0, 0);



	// バリアを張る
	particleResource_->Barrier(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
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
		// 位置
		ImGui::DragFloat3("Position" , &param_->position.x, 0.01f, -100000.0f, 100000.0f);
		
		ImGui::Text("\n");

		// エミッターの図形
		const char* type[] = { "Point", "AABB", "Sphere" };
		int32_t shapeIndex = static_cast<int32_t>(param_->shape);
		if (ImGui::Combo("Shape", &shapeIndex, type, IM_ARRAYSIZE(type)))
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

		// 色
		if (ImGui::TreeNode("Color"))
		{
			// 開始
			ImGui::ColorEdit4("Start", &param_->color.start.x);

			// 終了
			ImGui::ColorEdit4("End", &param_->color.end.x);

			// 終了
			ImGui::TreePop();
		}

		// 大きさ
		if (ImGui::TreeNode("Scale"))
		{
			// 開始
			ImGui::DragFloat("Start", &param_->scale.start, 0.01f, 0.0f, 100000.0f);

			// 終了
			ImGui::DragFloat("End", &param_->scale.end, 0.01f, 0.0f, 100000.0f);

			// 終了
			ImGui::TreePop();
		}

		if (!param_->enableAttract)
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
		ImGui::Checkbox("Attract", &param_->enableAttract);

		if (param_->enableAttract)
		{
			// 引力の位置
			ImGui::DragFloat3("AttractPos", &param_->attractPos.x, 0.01f, -100000.0f, 100000.0f);

			// 吸引加速度
			ImGui::DragFloat("AttractAcceleration", &param_->attractAcceleration, 0.01f, 0.01f, 100000.0f);
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