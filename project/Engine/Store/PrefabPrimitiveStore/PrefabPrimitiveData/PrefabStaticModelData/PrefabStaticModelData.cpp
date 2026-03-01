#include "PrefabStaticModelData.h"
#include "Store/ModelStore/ModelStore.h"
#include "PSO/PSOModel/BasePSOModel.h"
#include "PSO/PSOShadowMap/BasePSOShadowMap.h"

#include "Store/LightStore/LightStore.h"
#include "Store/TextureStore/TextureStore.h"
#include "Store/Camera3DStore/Camera3DStore.h"
#include "Store/SkyboxStore/SkyboxStore.h"

#include "Parameter/PrefabPrimitiveParameter/PrefabPrimitiveParameter.h"

#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

/// @brief コンストラクタ
/// @param name 
/// @param numInstance 
/// @param hPrefab 
/// @param hTexture 
Engine::PrefabStaticModelData::PrefabStaticModelData(const std::string& name, uint32_t numInstance, PrefabPrimitiveHandle hPrefab, ModelHandle hModel, PrefabPrimitiveParameter* parameter)
	: hModel_(hModel), PrefabPrimitiveBaseData(name, numInstance, hPrefab, parameter)
{
	// 種類
	type_ = Prefab::Type::StaticModel;

	// パラメータを生成する
	param_ = std::make_unique<Prefab::StaticModel::Base::Param>();

	// モデルトランスフォームの初期化
	param_->modelTransform.scale = Vector3(1.0f, 1.0f, 1.0f);
	param_->modelTransform.rotate = Vector3(0.0f, 0.0f, 0.0f);
	param_->modelTransform.translate = Vector3(0.0f, 0.0f, 0.0f);
}

/// @brief 初期化
/// @param modelStore 
/// @param textureStore 
/// @param lightStore 
/// @param cameraStore 
/// @param heap 
/// @param device 
/// @param log 
void Engine::PrefabStaticModelData::Initialize(ModelStore* modelStore, TextureStore* textureStore, LightStore* lightStore, Camera3DStore* cameraStore,
	DX12Heap* heap, ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(modelStore);
	assert(textureStore);
	assert(lightStore);
	assert(cameraStore);
	assert(heap);
	assert(device);

	// 引数を受け取る
	modelStore_ = modelStore;
	textureStore_ = textureStore;
	lightStore_ = lightStore;
	cameraStore_ = cameraStore;

	// モデルデータを取得する
	ModelData modelData = modelStore_->GetModelData(hModel_);

	// パラメータの記録
	group_ = "StaticModel_" + name_;
	if (parameter_)
	{
		parameter_->SetValue(group_, "Model_Transform_Scale", &param_->modelTransform.scale);
		parameter_->SetValue(group_, "Model_Transform_Rotate", &param_->modelTransform.rotate);
		parameter_->SetValue(group_, "Model_Transform_Translate", &param_->modelTransform.translate);
	}

	// 領域確保
	param_->meshMaterial.resize(static_cast<int32_t>(modelData.meshes.size()));
	param_->meshTransforms.resize(static_cast<int32_t>(modelData.meshes.size()));
	primitiveResource_.resize(static_cast<int32_t>(modelData.meshes.size()));
	shadowMapTransformationResource_.resize(static_cast<int32_t>(modelData.meshes.size()));

	// ファイルパス
	textureFilePathTable_.resize(static_cast<int32_t>(modelData.meshes.size()));

	// メッシュごとにデータ生成
	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelData.meshes.size()); ++meshIndex)
	{
		// トランスフォーム
		param_->meshTransforms[meshIndex].scale = Vector3(1.0f, 1.0f, 1.0f);
		param_->meshTransforms[meshIndex].rotate = Vector3(0.0f, 0.0f, 0.0f);
		param_->meshTransforms[meshIndex].translate = Vector3(0.0f, 0.0f, 0.0f);

		// マテリアル
		param_->meshMaterial[meshIndex].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		param_->meshMaterial[meshIndex].uv.scale = Vector2(1.0f, 1.0f);
		param_->meshMaterial[meshIndex].uv.radius = 0.0f;
		param_->meshMaterial[meshIndex].uv.translate = Vector2(0.0f, 0.0f);
		param_->meshMaterial[meshIndex].hTexture = modelData.meshes[meshIndex].material.handle;

		// テクスチャファイルパス
		textureFilePathTable_[meshIndex] = textureStore_->GetFilePath(param_->meshMaterial[meshIndex].hTexture);

		// パラメータの記録
		if (parameter_)
		{
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Mesh_Transform_Scale", &param_->meshTransforms[meshIndex].scale);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Mesh_Transform_Rotate", &param_->meshTransforms[meshIndex].rotate);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Mesh_Transform_Translate", &param_->meshTransforms[meshIndex].translate);

			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_Color", &param_->meshMaterial[meshIndex].color);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_UV_Scale", &param_->meshMaterial[meshIndex].uv.scale);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_UV_Rotate", &param_->meshMaterial[meshIndex].uv.radius);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_UV_Translate", &param_->meshMaterial[meshIndex].uv.translate);

			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_Texture", &textureFilePathTable_[meshIndex]);
		}

		// プリミティブ
		primitiveResource_[meshIndex] = std::make_unique<StructuredBufferResource<Prefab::PrimitiveDataForGPU>>();
		primitiveResource_[meshIndex]->Initialize(device, heap, numInstance_, log);

		// シャドウマップ
		shadowMapTransformationResource_[meshIndex] = std::make_unique<StructuredBufferResource<Matrix4x4>>();
		shadowMapTransformationResource_[meshIndex]->Initialize(device, heap, numInstance_, log);
	}

	// 値を反映させる
	if (parameter_)parameter_->RegisterGroupDataReflection(group_);
	for (int32_t meshIndex = 0; meshIndex < modelData.meshes.size(); ++meshIndex)
		param_->meshMaterial[meshIndex].hTexture = textureStore_->GetHandle(textureFilePathTable_[meshIndex]);
}

/// @brief 更新処理
void Engine::PrefabStaticModelData::Update()
{
	// 削除されたインスタンスをリストから除外する
	instanceTable_.remove_if([](std::unique_ptr<PrefabInstanceStaticModel>& instance) {if (instance->IsDelete()) { return true; }return false; });
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param pso 
void Engine::PrefabStaticModelData::Register(SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
{
	// モデルデータを取得する
	ModelData modelData = modelStore_->GetModelData(hModel_);


	// PSOの設定
	pso->Register(commandList);

	// カメラの設定
	cameraStore_->RegisterCameraResource(commandList, 4);

	// スカイボックスの設定
	skyboxStore->RegisterCubeMapTexture(commandList, 5);

	// メッシュごとに処理
	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelData.meshes.size()); meshIndex++)
	{
		// 頂点の設定
		modelStore_->Register(commandList, hModel_, meshIndex);

		// プリミティブの設定
		primitiveResource_[meshIndex]->RegisterGraphics(commandList, 0);

		// テクスチャの設定
		commandList->SetGraphicsRootDescriptorTable(1, textureStore_->GetSrvGpuHandle(param_->meshMaterial[meshIndex].hTexture));

		// シャドウマップテクスチャの設定
		lightStore_->GetShadowMapTextureResource()->Register(commandList, 2);

		// シャドウ用座標変換の設定
		lightStore_->GetShadowMapTransformationResource()->RegisterGraphics(commandList, 3);

		// 形状の設定
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// ドローコール
		commandList->DrawIndexedInstanced(static_cast<UINT>(modelStore_->GetModelData(hModel_).meshes[meshIndex].indices.size()), numUseInstance_, 0, 0, 0);
	}
}

/// @brief シャドウマップを描画する
/// @param viewProjection 
/// @param commandList 
/// @param pso 
void Engine::PrefabStaticModelData::DrawShadowMap(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
	// モデルデータを取得する
	ModelData modelData = modelStore_->GetModelData(hModel_);

	// PSOの設定
	pso->Register(commandList);

	// メッシュごとに処理
	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelData.meshes.size()); meshIndex++)
	{
		UINT useInstance = 0;

		// インスタンスごとに処理
		for (auto& instance : instanceTable_)
		{
			// インスタンス数を越えたら処理しない
			if (useInstance >= numInstance_)
				break;

			Quaternion modelQuaternion =
				ToQuaternion(instance->param_.modelTransform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
				ToQuaternion(instance->param_.modelTransform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
				ToQuaternion(instance->param_.modelTransform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

			Matrix4x4 worldMatrix = Make3DAffineMatrix4x4(instance->param_.modelTransform.scale, modelQuaternion, instance->param_.modelTransform.translate);

			// ノード行列
			Matrix4x4 nodeMatrix = MakeIdentityMatrix4x4();
			if (!modelData.nodes.empty())nodeMatrix = modelData.nodes[static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()) - 1 - meshIndex].worldMatrix;

			Quaternion meshQuaternion =
				ToQuaternion(instance->param_.meshTransforms[meshIndex].rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
				ToQuaternion(instance->param_.meshTransforms[meshIndex].rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
				ToQuaternion(instance->param_.meshTransforms[meshIndex].rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

			Matrix4x4 localMatrix = Make3DAffineMatrix4x4(instance->param_.meshTransforms[meshIndex].scale, meshQuaternion, instance->param_.meshTransforms[meshIndex].translate);


			// ワールド座標
			shadowMapTransformationResource_[meshIndex]->data_[useInstance] = localMatrix * nodeMatrix * worldMatrix * viewProjection;

			// 使用インスタンスをカウントする
			useInstance++;
		}


		// 使用インスタンスがなかったら処理しない
		if (useInstance <= 0)
			continue;


		/*------------------------
			コマンドリストに登録
		------------------------*/

		// 頂点の設定
		modelStore_->Register(commandList, hModel_, meshIndex);

		// 座標変換の設定
		shadowMapTransformationResource_[meshIndex]->RegisterGraphics(commandList, 0);

		// 形状の設定
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// ドローコール
		commandList->DrawIndexedInstanced(static_cast<UINT>(modelStore_->GetModelData(hModel_).meshes[meshIndex].indices.size()), useInstance, 0, 0, 0);
	}
}

/// @brief インスタンスを生成する
/// @return 
void* Engine::PrefabStaticModelData::CreateInstance()
{
	// インスタンスを生成する
	std::unique_ptr<PrefabInstanceStaticModel> instance =
		std::make_unique<PrefabInstanceStaticModel>([this](const Prefab::StaticModel::Instance::Param* param) {DrawCallInstance(param); }, param_.get());

	// ポインタを保存する
	PrefabInstanceStaticModel* pInstance = instance.get();

	// テーブルに追加する
	instanceTable_.push_back(std::move(instance));

	return pInstance;
}

/// @brief 全てのインスタンスを削除する
void Engine::PrefabStaticModelData::DestroyAllInstance()
{
	instanceTable_.clear();
}

/// @brief デバッグ用パラメータ
void Engine::PrefabStaticModelData::DebugParameter()
{
#ifdef _DEVELOPMENT

	// モデル名
	if (ImGui::TreeNode(name_.c_str()))
	{
		// モデルトランスフォーム
		if (ImGui::TreeNode("Model_Transform"))
		{
			// 拡縮
			ImGui::DragFloat3("Scale", &param_->modelTransform.scale.x, 0.01f, -100000.0f, 100000.0f);

			// 回転
			ImGui::DragFloat3("Rotate", &param_->modelTransform.rotate.x, 0.01f, -100000.0f, 100000.0f);

			// 平行移動
			ImGui::DragFloat3("Translate", &param_->modelTransform.translate.x, 0.01f, -100000.0f, 100000.0f);

			// 終了
			ImGui::TreePop();
		}


		// モデルデータを取得する
		const ModelData& modelData = modelStore_->GetModelData(hModel_);

		// メッシュ
		if (ImGui::TreeNode("Mesh"))
		{
			for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelData.meshes.size()); ++meshIndex)
			{
				// メッシュ
				if (ImGui::TreeNode(modelData.meshNames[meshIndex].c_str()))
				{
					// トランスフォーム
					if (ImGui::TreeNode("Transform"))
					{
						// 拡縮
						ImGui::DragFloat3("Scale", &param_->meshTransforms[meshIndex].scale.x, 0.01f, -100000.0f, 100000.0f);

						// 回転
						ImGui::DragFloat3("Rotate", &param_->meshTransforms[meshIndex].rotate.x, 0.01f, -100000.0f, 100000.0f);

						// 平行移動
						ImGui::DragFloat3("Translate", &param_->meshTransforms[meshIndex].translate.x, 0.01f, -100000.0f, 100000.0f);

						// 終了
						ImGui::TreePop();
					}

					// マテリアル
					if (ImGui::TreeNode("Material"))
					{
						// UV
						if (ImGui::TreeNode("UV"))
						{
							// 拡縮
							ImGui::DragFloat2("Scale", &param_->meshMaterial[meshIndex].uv.scale.x, 0.01f, -100000.0f, 100000.0f);

							// 回転
							ImGui::DragFloat("Rotate", &param_->meshMaterial[meshIndex].uv.radius, 0.01f, -100000.0f, 100000.0f);

							// 平行移動
							ImGui::DragFloat2("Translate", &param_->meshMaterial[meshIndex].uv.translate.x, 0.01f, -100000.0f, 100000.0f);

							// 終了
							ImGui::TreePop();
						}

						// 色
						ImGui::ColorEdit4("Color", &param_->meshMaterial[meshIndex].color.x);

						// テクスチャ
						ImGui::Text("Texture");

						ImGui::ImageButton(
							textureStore_->GetFilePath(param_->meshMaterial[meshIndex].hTexture).c_str(),
							textureStore_->GetSrvGpuHandle(param_->meshMaterial[meshIndex].hTexture).ptr,
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
								param_->meshMaterial[meshIndex].hTexture = static_cast<uint32_t>(droppedIndex);
								textureFilePathTable_[meshIndex] = textureStore_->GetFilePath(param_->meshMaterial[meshIndex].hTexture);
							}
							ImGui::EndDragDropTarget();
						}

						// 終了
						ImGui::TreePop();
					}

					// 終了
					ImGui::TreePop();
				}
			}

			// 終了
			ImGui::TreePop();
		}

		// インスタンス量
		ImGui::Text("Instance \n");
		ImGui::ProgressBar(static_cast<float>(numUseInstance_) / static_cast<float>(numInstance_), ImVec2(200.0f, 20.0f),
			std::format("{} / {}", numUseInstance_, numInstance_).c_str());

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

/// @brief インスタンスのドローコール
void Engine::PrefabStaticModelData::DrawCallInstance(const Engine::Prefab::StaticModel::Instance::Param* param)
{
	if (numUseInstance_ >= numInstance_)
		return;

	// モデルデータを取得する
	const ModelData& modelData = modelStore_->GetModelData(hModel_);

	Quaternion modelQuaternion =
		ToQuaternion(param->modelTransform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param->modelTransform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param->modelTransform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	Matrix4x4 worldMatrix = Make3DAffineMatrix4x4(param->modelTransform.scale, modelQuaternion, param->modelTransform.translate);

	for (int meshIndex = 0; meshIndex < static_cast<int32_t>(modelData.meshes.size()); meshIndex++)
	{
		// ノード行列
		Matrix4x4 nodeMatrix = MakeIdentityMatrix4x4();
		if (!modelData.nodes.empty())nodeMatrix = modelData.nodes[static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()) - 1 - meshIndex].worldMatrix;

		Quaternion meshQuaternion =
			ToQuaternion(param->meshTransforms[meshIndex].rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
			ToQuaternion(param->meshTransforms[meshIndex].rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
			ToQuaternion(param->meshTransforms[meshIndex].rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

		Matrix4x4 localMatrix = Make3DAffineMatrix4x4(param->meshTransforms[meshIndex].scale, meshQuaternion, param->meshTransforms[meshIndex].translate);


		// ワールド座標
		primitiveResource_[meshIndex]->data_[numUseInstance_].world =
			localMatrix * nodeMatrix * worldMatrix;

		// ワールドビュー正射影行列
		primitiveResource_[meshIndex]->data_[numUseInstance_].worldViewProjection =
			primitiveResource_[meshIndex]->data_[numUseInstance_].world * cameraStore_->GetCamera3D().GetViewProjectionMatrix();

		// 逆転置ワールド行列
		primitiveResource_[meshIndex]->data_[numUseInstance_].worldInverseTranspose =
			primitiveResource_[meshIndex]->data_[numUseInstance_].world.Transpose().Inverse();



		// 色
		primitiveResource_[meshIndex]->data_[numUseInstance_].color = param_->meshMaterial[meshIndex].color;

		// UV行列
		primitiveResource_[meshIndex]->data_[numUseInstance_].uvTransform =
			Make3DScaleMatrix4x4(Vector3(param_->meshMaterial[meshIndex].uv.scale.x, param_->meshMaterial[meshIndex].uv.scale.y, 1.0f)) *
			Make3DRotateZMatrix4x4(param_->meshMaterial[meshIndex].uv.radius) *
			Make3DTranslateMatrix4x4(Vector3(param_->meshMaterial[meshIndex].uv.translate.x, param_->meshMaterial[meshIndex].uv.translate.y, 0.0f));
	}

	numUseInstance_++;
}