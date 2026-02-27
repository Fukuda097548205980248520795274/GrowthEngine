#include "PrimitiveStaticModelData.h"
#include "Store/ModelStore/ModelStore.h"
#include "Store/TextureStore/TextureStore.h"
#include <cassert>
#include "PSO/PSOModel/BasePSOModel.h"
#include "PSO/PSOShadowMap/BasePSOShadowMap.h"
#include "Store/LightStore/LightStore.h"

#include <numbers>

#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

#include "Parameter/PrimitiveParameter/PrimitiveParameter.h"

/// @brief 初期化
/// @param modelStore 
/// @param device 
void Engine::PrimitiveStaticModelData::Initialize(ModelStore* modelStore, TextureStore* textureStore, LightStore* lightStore, ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(modelStore);
	assert(textureStore);
	assert(lightStore);
	assert(device);

	// 引数を受け取る
	modelStore_ = modelStore;
	textureStore_ = textureStore;
	lightStore_ = lightStore;


	// パラメータの生成
	param_ = std::make_unique<Primitive::StaticModel::Param>();


	// モデルトランスフォーム
	param_->modelTransform.scale = Vector3(1.0f, 1.0f, 1.0f);
	param_->modelTransform.rotate = Vector3(0.0f, 0.0f, 0.0f);
	param_->modelTransform.translate = Vector3(0.0f, 0.0f, 0.0f);

	// パラメータの記録
	group_ = "StaticModel_" + name_;
	if (parameter_)
	{
		parameter_->SetValue(group_, "Model_Transform_Scale", &param_->modelTransform.scale);
		parameter_->SetValue(group_, "Model_Transform_Rotate", &param_->modelTransform.rotate);
		parameter_->SetValue(group_, "Model_Transform_Translate", &param_->modelTransform.translate);
	}



	// モデルデータを取得する
	const ModelData& modelData = modelStore_->GetModelData(hModel_);

	// パラメータ領域確保
	param_->meshTransforms.resize(static_cast<int32_t>(modelData.meshes.size()));
	param_->meshMaterial.resize(static_cast<int32_t>(modelData.meshes.size()));

	// リソース領域確保
	meshTransformationResources_.resize(static_cast<int32_t>(modelData.meshes.size()));
	meshMaterialResources_.resize(static_cast<int32_t>(modelData.meshes.size()));
	shadowMapTransformationResource_.resize(static_cast<int32_t>(modelData.meshes.size()));
	
	// ファイルパス
	textureFilePathTable_.resize(static_cast<int32_t>(modelData.meshes.size()));

	for (int32_t meshIndex = 0; meshIndex < modelData.meshes.size(); ++meshIndex)
	{
		// メッシュトランスフォーム
		param_->meshTransforms[meshIndex].scale = Vector3(1.0f, 1.0f, 1.0f);
		param_->meshTransforms[meshIndex].rotate = Vector3(0.0f, 0.0f, 0.0f);
		param_->meshTransforms[meshIndex].translate = Vector3(0.0f, 0.0f, 0.0f);

		// マテリアルトランスフォーム
		param_->meshMaterial[meshIndex].hTexture = modelData.meshes[meshIndex].material.handle;
		param_->meshMaterial[meshIndex].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		param_->meshMaterial[meshIndex].uv.scale = Vector2(1.0f, 1.0f);
		param_->meshMaterial[meshIndex].uv.radius = 0.0f;
		param_->meshMaterial[meshIndex].uv.translate = Vector2(0.0f, 0.0f);

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

		// 座標変換リソース
		meshTransformationResources_[meshIndex] = std::make_unique<ConstantBufferResource<PrimitiveModelTransformationDataForGPU>>();
		meshTransformationResources_[meshIndex]->Initialize(device, log);

		// マテリアルリソース
		meshMaterialResources_[meshIndex] = std::make_unique<ConstantBufferResource<PrimitiveModelMaterialDataForGPU>>();
		meshMaterialResources_[meshIndex]->Initialize(device, log);

		// シャドウマップ用座標変換リソース
		shadowMapTransformationResource_[meshIndex] = std::make_unique<ConstantBufferResource<Matrix4x4>>();
		shadowMapTransformationResource_[meshIndex]->Initialize(device, log);
	}

	// 値を反映させる
	if (parameter_)parameter_->RegisterGroupDataReflection(group_);
	for (int32_t meshIndex = 0; meshIndex < modelData.meshes.size(); ++meshIndex)
		param_->meshMaterial[meshIndex].hTexture = textureStore_->GetHandle(textureFilePathTable_[meshIndex]);
}

/// @brief 更新処理
void Engine::PrimitiveStaticModelData::Update()
{

}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param pso 
/// @param textureStore 
void Engine::PrimitiveStaticModelData::Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
{
	// モデルデータを取得する
	const ModelData& modelData = modelStore_->GetModelData(hModel_);

	Quaternion modelQuaternion =
		ToQuaternion(param_->modelTransform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param_->modelTransform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param_->modelTransform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	Matrix4x4 worldMatrix = Make3DAffineMatrix4x4(param_->modelTransform.scale, modelQuaternion, param_->modelTransform.translate);



	// PSOの設定
	pso->Register(commandList);

	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()); meshIndex++)
	{
		/*-----------------
		    データを渡す
		-----------------*/

		// ノード行列
		Matrix4x4 nodeMatrix = MakeIdentityMatrix4x4();
		if (!modelData.nodes.empty())nodeMatrix = modelData.nodes[static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()) - 1 - meshIndex].worldMatrix;

		Quaternion meshQuaternion =
			ToQuaternion(param_->meshTransforms[meshIndex].rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
			ToQuaternion(param_->meshTransforms[meshIndex].rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
			ToQuaternion(param_->meshTransforms[meshIndex].rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

		Matrix4x4 localMatrix = Make3DAffineMatrix4x4(param_->meshTransforms[meshIndex].scale, meshQuaternion, param_->meshTransforms[meshIndex].translate);


		// ワールド座標
		meshTransformationResources_[meshIndex]->data_->worldMatrix =
			localMatrix * nodeMatrix * worldMatrix;

		// ワールドビュー正射影行列
		meshTransformationResources_[meshIndex]->data_->worldViewProjectionMatrix =
			meshTransformationResources_[meshIndex]->data_->worldMatrix * viewProjection;

		// 逆転置ワールド行列
		meshTransformationResources_[meshIndex]->data_->worldInverseTransposeMatrix =
			meshTransformationResources_[meshIndex]->data_->worldMatrix.Transpose().Inverse();



		// 色
		meshMaterialResources_[meshIndex]->data_->color = param_->meshMaterial[meshIndex].color;

		// UV行列
		meshMaterialResources_[meshIndex]->data_->uvMatrix =
			Make3DScaleMatrix4x4(Vector3(param_->meshMaterial[meshIndex].uv.scale.x, param_->meshMaterial[meshIndex].uv.scale.y, 1.0f)) *
			Make3DRotateZMatrix4x4(param_->meshMaterial[meshIndex].uv.radius) *
			Make3DTranslateMatrix4x4(Vector3(param_->meshMaterial[meshIndex].uv.translate.x, param_->meshMaterial[meshIndex].uv.translate.y, 0.0f));



		/*------------------------
		    コマンドリストに登録
		------------------------*/

		// 頂点の設定
		modelStore_->Register(commandList, hModel_, meshIndex);

		// 座標変換の設定
		meshTransformationResources_[meshIndex]->RegisterGraphics(commandList, 0);

		// マテリアルの設定
		meshMaterialResources_[meshIndex]->RegisterGraphics(commandList, 1);

		// テクスチャの設定
		commandList->SetGraphicsRootDescriptorTable(2, textureStore_->GetSrvGpuHandle(param_->meshMaterial[meshIndex].hTexture));

		// シャドウマップテクスチャの設定
		lightStore_->GetShadowMapTextureResource()->Register(commandList, 3);

		// シャドウ用座標変換の設定
		lightStore_->GetShadowMapTransformationResource()->RegisterGraphics(commandList, 4);

		// 形状の設定
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// ドローコール
		commandList->DrawIndexedInstanced(static_cast<UINT>(modelStore_->GetModelData(hModel_).meshes[meshIndex].indices.size()), 1, 0, 0, 0);
	}
}

/// @brief コマンドリスト
/// @param commandList 
/// @param pso 
void Engine::PrimitiveStaticModelData::Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
	// モデルデータを取得する
	const ModelData& modelData = modelStore_->GetModelData(hModel_);

	Quaternion modelQuaternion =
		ToQuaternion(param_->modelTransform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param_->modelTransform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param_->modelTransform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	Matrix4x4 worldMatrix = Make3DAffineMatrix4x4(param_->modelTransform.scale, modelQuaternion, param_->modelTransform.translate);



	// PSOの設定
	pso->Register(commandList);

	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()); meshIndex++)
	{
		/*-----------------
		    データを渡す
		-----------------*/


		// ノード行列
		Matrix4x4 nodeMatrix = MakeIdentityMatrix4x4();
		if (!modelData.nodes.empty())nodeMatrix = modelData.nodes[static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()) - 1 - meshIndex].worldMatrix;

		Quaternion meshQuaternion =
			ToQuaternion(param_->meshTransforms[meshIndex].rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
			ToQuaternion(param_->meshTransforms[meshIndex].rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
			ToQuaternion(param_->meshTransforms[meshIndex].rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

		Matrix4x4 localMatrix = Make3DAffineMatrix4x4(param_->meshTransforms[meshIndex].scale, meshQuaternion, param_->meshTransforms[meshIndex].translate);


		// ワールド座標
		*shadowMapTransformationResource_[meshIndex]->data_ = localMatrix * nodeMatrix * worldMatrix * viewProjection;


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
		commandList->DrawIndexedInstanced(static_cast<UINT>(modelStore_->GetModelData(hModel_).meshes[meshIndex].indices.size()), 1, 0, 0, 0);
	}
}

/// @brief デバッグ用パラメータ
void Engine::PrimitiveStaticModelData::DebugParameter()
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