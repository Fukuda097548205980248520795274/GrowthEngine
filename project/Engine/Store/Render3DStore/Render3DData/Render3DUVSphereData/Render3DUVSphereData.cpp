#include "Render3DUVSphereData.h"
#include "Store/TextureStore/TextureStore.h"
#include <cassert>
#include "PSO/PSOModel/BasePSOModel.h"
#include "Store/LightStore/LightStore.h"
#include "Store/Camera3DStore/Camera3DStore.h"
#include "Store/SkyboxStore/SkyboxStore.h"
#include <algorithm>

#include <numbers>

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "Parameter/Render3DParameter/Render3DParameter.h"

#include "Func/CollisionFunc/CollisionFunc.h"

#include "GrowthEngine.h"

/// @brief 初期化
/// @param modelStore 
/// @param device 
void Engine::Render3DUVSphereData::Initialize(TextureStore* textureStore, LightStore* lightStore, 
	DX12Heap* heap, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, BaseComputePSO* psoUVSphere, Log* log)
{
	// nullptrチェック
	assert(textureStore);
	assert(lightStore);
	assert(heap);
	assert(device);
	assert(commandList);
	assert(psoUVSphere);

	// 引数を受け取る
	textureStore_ = textureStore;
	lightStore_ = lightStore;
	psoUVSphere_ = psoUVSphere;


	// パラメータの生成
	param_ = std::make_unique<Render3D::UVSphere::Param>();

	// トランスフォーム
	param_->transform.scale = Vector3(1.0f, 1.0f, 1.0f);
	param_->transform.rotate = Vector3(0.0f, 0.0f, 0.0f);
	param_->transform.translate = Vector3(0.0f, 0.0f, 0.0f);

	// マテリアル
	param_->material.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	param_->material.uv.scale = Vector2(1.0f, 1.0f);
	param_->material.uv.radius = 0.0f;
	param_->material.uv.translate = Vector2(0.0f, 0.0f);
	param_->material.environment = 0.0f;
	param_->material.shininess = 10.0f;
	param_->material.enableLighting = true;
	param_->material.enableDiffuse = true;
	param_->material.enableHalfLambert = true;
	param_->material.enableSpecular = true;
	param_->material.enableBlinnPhong = true;
	param_->material.enableShadowMap = true;

	// 分割
	param_->division.slices = 32;
	param_->division.rings = 16;

	// テクスチャ
	param_->material.hTexture = hTexture_;
	textureFilePath_ = textureStore_->GetFilePath(param_->material.hTexture);


	// パラメータの記録
	group_ = "UVSphere_" + name_;
	if (parameter_)
	{
		parameter_->SetValue(group_, "Transform_Scale", &param_->transform.scale);
		parameter_->SetValue(group_, "Transform_Rotate", &param_->transform.rotate);
		parameter_->SetValue(group_, "Transform_Translate", &param_->transform.translate);
		parameter_->SetValue(group_, "Material_Color", &param_->material.color);
		parameter_->SetValue(group_, "Material_UV_Scale", &param_->material.uv.scale);
		parameter_->SetValue(group_, "Material_UV_Rotate", &param_->material.uv.radius);
		parameter_->SetValue(group_, "Material_UV_Translate", &param_->material.uv.translate);
		parameter_->SetValue(group_, "Material_Environment", &param_->material.environment);
		parameter_->SetValue(group_, "Material_Shininess", &param_->material.shininess);
		parameter_->SetValue(group_, "Material_Enable_Lighting", &param_->material.enableLighting);
		parameter_->SetValue(group_, "Material_Enable_Diffuse", &param_->material.enableDiffuse);
		parameter_->SetValue(group_, "Material_Enable_HalfLambert", &param_->material.enableHalfLambert);
		parameter_->SetValue(group_, "Material_Enable_Specular", &param_->material.enableSpecular);
		parameter_->SetValue(group_, "Material_Enable_BlinnPhong", &param_->material.enableBlinnPhong);
		parameter_->SetValue(group_, "Material_Enable_ShadowMap", &param_->material.enableShadowMap);
		parameter_->SetValue(group_, "Material_Texture", &textureFilePath_);
		parameter_->SetValue(group_, "Division_Slices", &param_->division.slices);
		parameter_->SetValue(group_, "Division_Rings", &param_->division.rings);

		// 値を反映させる
		parameter_->RegisterGroupDataReflection(group_);
		param_->material.hTexture = textureStore_->GetHandle(textureFilePath_);
	}

	// 頂点リソースの生成
	vertexResource_ = std::make_unique<RWStructuredVertexBufferResource<VertexDataForGPU>>();
	vertexResource_->Initialize(device, commandList, heap, ((kMaxSlices + 1) * (kMaxRings + 1)), log);

	// インデックスリソースの生成
	indexResource_ = std::make_unique<RWStructuredVertexBufferResource<uint32_t>>();
	indexResource_->Initialize(device, commandList, heap, (kMaxSlices * kMaxRings * 6), log);

	// 分割リソースの生成
	divisionResource_ = std::make_unique<ConstantBufferResource<PrimitiveDataForGPU::UVSphereDivisionDataForGPU>>();
	divisionResource_->Initialize(device, log);

	// 座標変換リソースの生成
	transformationResources_ = std::make_unique<ConstantBufferResource<PrimitiveModelTransformationDataForGPU>>();
	transformationResources_->Initialize(device, log);

	// マテリアルリソースの生成
	materialResources_ = std::make_unique<ConstantBufferResource<PrimitiveModelMaterialDataForGPU>>();
	materialResources_->Initialize(device, log);

	// シャドウマップ用座標変換リソースの生成
	shadowMapTransformationResource_ = std::make_unique<ConstantBufferResource<Matrix4x4>>();
	shadowMapTransformationResource_->Initialize(device, log);
}

/// @brief 更新処理
void Engine::Render3DUVSphereData::Update()
{
	// 描画を記録する
	isPreDrew_ = isDrew_;
	isDrew_ = false;
}

/// @brief リセット
void Engine::Render3DUVSphereData::Reset()
{
	// jsonファイルがあるかどうか
	if (parameter_->IsFileFound(group_))
	{
		// ファイルがあるとき

		// 値を反映させる
		parameter_->RegisterGroupDataReflection(group_);
		param_->material.hTexture = textureStore_->GetHandle(textureFilePath_);
	}
	else
	{
		// ファイルがないとき

		// トランスフォーム
		param_->transform.scale = Vector3(1.0f, 1.0f, 1.0f);
		param_->transform.rotate = Vector3(0.0f, 0.0f, 0.0f);
		param_->transform.translate = Vector3(0.0f, 0.0f, 0.0f);

		// マテリアル
		param_->material.hTexture = hTexture_;
		param_->material.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		param_->material.uv.scale = Vector2(1.0f, 1.0f);
		param_->material.uv.radius = 0.0f;
		param_->material.uv.translate = Vector2(0.0f, 0.0f);
		param_->material.environment = 0.0f;
		param_->material.shininess = 10.0f;
		param_->material.enableLighting = true;
		param_->material.enableDiffuse = true;
		param_->material.enableHalfLambert = true;
		param_->material.enableSpecular = true;
		param_->material.enableBlinnPhong = true;
		param_->material.enableShadowMap = true;

		// 分割
		param_->division.slices = 32;
		param_->division.rings = 16;
	}

	// 読み込み
	isLoad_ = true;
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param pso 
/// @param textureStore 
void Engine::Render3DUVSphereData::Register(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;


	// PSOの設定
	psoUVSphere_->Register(commandList);

	param_->division.slices = std::clamp(param_->division.slices, 3, kMaxSlices);
	param_->division.rings = std::clamp(param_->division.rings, 3, kMaxRings);

	// 分割の設定
	divisionResource_->data_->slices = param_->division.slices;
	divisionResource_->data_->rings = param_->division.rings;
	divisionResource_->RegisterCompute(commandList, 0);

	// 頂点の設定
	vertexResource_->RegisterCompute(commandList, 1);

	// インデックスの設定
	indexResource_->RegisterCompute(commandList, 2);

	// ディスパッチ
	commandList->Dispatch((param_->division.slices + 1 + 15) / 16, (param_->division.rings + 1 + 15) / 16, 1);




	Quaternion modelQuaternion =
		ToQuaternion(param_->transform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	Matrix4x4 worldMatrix = Make3DAffineMatrix4x4(param_->transform.scale, modelQuaternion, param_->transform.translate);

	// ビュープロジェクション行列を取得する
	Matrix4x4 viewProjection = cameraStore->GetCamera3D().GetViewProjectionMatrix();


	/*------------------------
	   コマンドリストに登録する
	------------------------*/

	// PSOの設定
	pso->Register(commandList);

	// カメラリソースの設定
	cameraStore->RegisterCameraResource(commandList, 5);

	// スカイボックスの設定
	skyboxStore->RegisterCubeMapTexture(commandList, 6);

	// ライトの設定
	lightStore_->LightRegister(commandList, 7, 8, 9, 10);

	// ワールド座標
	transformationResources_->data_->worldMatrix = worldMatrix;

	// ワールドビュー正射影行列
	transformationResources_->data_->worldViewProjectionMatrix =
		transformationResources_->data_->worldMatrix * viewProjection;
	// 逆転置ワールド行列
	transformationResources_->data_->worldInverseTransposeMatrix =
		transformationResources_->data_->worldMatrix.Transpose().Inverse();



	// 色
	materialResources_->data_->color = param_->material.color;

	// 環境
	materialResources_->data_->environment = param_->material.environment;

	// 光沢度
	materialResources_->data_->shininess = param_->material.shininess;

	// UV行列
	materialResources_->data_->uvMatrix =
		Make3DScaleMatrix4x4(Vector3(param_->material.uv.scale.x, param_->material.uv.scale.y, 1.0f)) *
		Make3DRotateZMatrix4x4(param_->material.uv.radius) *
		Make3DTranslateMatrix4x4(Vector3(param_->material.uv.translate.x, param_->material.uv.translate.y, 0.0f));

	// ライティング有効化
	materialResources_->data_->enableLighting = static_cast<int32_t>(param_->material.enableLighting);

	// ディフューズ有効化
	materialResources_->data_->enableDiffuse = static_cast<int32_t>(param_->material.enableDiffuse);

	// ハーフランバード有効化
	materialResources_->data_->enableHalfLambert = static_cast<int32_t>(param_->material.enableHalfLambert);

	// スペキュラー有効化
	materialResources_->data_->enableSpecular = static_cast<int32_t>(param_->material.enableSpecular);

	// ブリンフォン有効化
	materialResources_->data_->enableBlinnPhong = static_cast<int32_t>(param_->material.enableBlinnPhong);

	/*------------------------
		コマンドリストに登録
	------------------------*/

	// バリアを張る
	vertexResource_->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	indexResource_->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_INDEX_BUFFER);



	// 頂点の設定
	D3D12_VERTEX_BUFFER_VIEW vbv = {};
	vbv.BufferLocation = vertexResource_->GetResource()->GetGPUVirtualAddress();
	vbv.SizeInBytes = sizeof(VertexDataForGPU) * ((kMaxSlices + 1) * (kMaxRings + 1));
	vbv.StrideInBytes = sizeof(VertexDataForGPU);
	commandList->IASetVertexBuffers(0, 1, &vbv);

	// インデックスの設定
	D3D12_INDEX_BUFFER_VIEW ibv = {};
	ibv.BufferLocation = indexResource_->GetResource()->GetGPUVirtualAddress();
	ibv.SizeInBytes = sizeof(uint32_t) * kMaxSlices * kMaxRings * 6;
	ibv.Format = DXGI_FORMAT_R32_UINT;
	commandList->IASetIndexBuffer(&ibv);

	// 座標変換の設定
	transformationResources_->RegisterGraphics(commandList, 0);

	// マテリアルの設定
	materialResources_->RegisterGraphics(commandList, 1);

	// テクスチャの設定
	commandList->SetGraphicsRootDescriptorTable(2, textureStore_->GetSrvGpuHandle(param_->material.hTexture));

	// シャドウマップテクスチャの設定
	lightStore_->GetShadowMapTextureResource()->Register(commandList, 3);

	// シャドウ用座標変換の設定
	lightStore_->GetShadowMapTransformationResource()->RegisterGraphics(commandList, 4);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawIndexedInstanced(param_->division.slices * param_->division.rings * 6, 1, 0, 0, 0);



	// バリアを張る
	indexResource_->Barrier(commandList, D3D12_RESOURCE_STATE_INDEX_BUFFER, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	vertexResource_->Barrier(commandList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// 描画した
	isDrew_ = true;

	// 分割数を記録する
	preSlices_ = param_->division.slices;
	preRings_ = param_->division.rings;
}

/// @brief コマンドリスト
/// @param commandList 
/// @param pso 
void Engine::Render3DUVSphereData::Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// シャドウマップを描画しないときは処理しない
	if (!param_->material.enableShadowMap)return;

	// 直前で描画されているときのみ
	if (!IsDrew())return;

	Quaternion modelQuaternion =
		ToQuaternion(param_->transform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	Matrix4x4 worldMatrix = Make3DAffineMatrix4x4(param_->transform.scale, modelQuaternion, param_->transform.translate);



	// PSOの設定
	pso->Register(commandList);

	/*-----------------
		データを渡す
	-----------------*/


	// ワールド座標
	*shadowMapTransformationResource_->data_ = worldMatrix * viewProjection;


	/*------------------------
		コマンドリストに登録
	------------------------*/

	// バリアを張る
	vertexResource_->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	indexResource_->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_INDEX_BUFFER);



	// 頂点の設定
	D3D12_VERTEX_BUFFER_VIEW vbv = {};
	vbv.BufferLocation = vertexResource_->GetResource()->GetGPUVirtualAddress();
	vbv.SizeInBytes = sizeof(VertexDataForGPU) * ((kMaxSlices + 1) * (kMaxRings + 1));
	vbv.StrideInBytes = sizeof(VertexDataForGPU);
	commandList->IASetVertexBuffers(0, 1, &vbv);

	// インデックスの設定
	D3D12_INDEX_BUFFER_VIEW ibv = {};
	ibv.BufferLocation = indexResource_->GetResource()->GetGPUVirtualAddress();
	ibv.SizeInBytes = sizeof(uint32_t) * kMaxSlices * kMaxRings * 6;
	ibv.Format = DXGI_FORMAT_R32_UINT;
	commandList->IASetIndexBuffer(&ibv);

	// 座標変換の設定
	shadowMapTransformationResource_->RegisterGraphics(commandList, 0);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawIndexedInstanced(preSlices_ * preRings_ * 6, 1, 0, 0, 0);


	// バリアを張る
	indexResource_->Barrier(commandList, D3D12_RESOURCE_STATE_INDEX_BUFFER, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	vertexResource_->Barrier(commandList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

/// @brief デバッグ用パラメータ
void Engine::Render3DUVSphereData::DebugParameter()
{
#ifdef _DEVELOPMENT

	// 読み込んでいないと処理しない
	if (!isLoad_)return;

	// モデル名
	if (ImGui::TreeNode(name_.c_str()))
	{
		// モデルトランスフォーム
		if (ImGui::TreeNode("Transform"))
		{
			// 拡縮
			ImGui::DragFloat3("Scale", &param_->transform.scale.x, 0.01f, -100000.0f, 100000.0f);

			// 回転
			ImGui::DragFloat3("Rotate", &param_->transform.rotate.x, 0.01f, -100000.0f, 100000.0f);

			// 平行移動
			ImGui::DragFloat3("Translate", &param_->transform.translate.x, 0.01f, -100000.0f, 100000.0f);

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
				ImGui::DragFloat2("Scale", &param_->material.uv.scale.x, 0.01f, -100000.0f, 100000.0f);

				// 回転
				ImGui::DragFloat("Rotate", &param_->material.uv.radius, 0.01f, -100000.0f, 100000.0f);

				// 平行移動
				ImGui::DragFloat2("Translate", &param_->material.uv.translate.x, 0.01f, -100000.0f, 100000.0f);

				// 終了
				ImGui::TreePop();
			}

			// 色
			ImGui::ColorEdit4("Color", &param_->material.color.x);

			// テクスチャ
			ImGui::Text("Texture");

			ImGui::ImageButton(
				textureStore_->GetFilePath(param_->material.hTexture).c_str(),
				textureStore_->GetSrvGpuHandle(param_->material.hTexture).ptr,
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
					param_->material.hTexture = static_cast<uint32_t>(droppedIndex);
					textureFilePath_ = textureStore_->GetFilePath(param_->material.hTexture);
				}
				ImGui::EndDragDropTarget();
			}



			ImGui::SeparatorText("Lighting");

			// ライティング有効化
			ImGui::Checkbox("Lighting", &param_->material.enableLighting);

			if (param_->material.enableLighting)
			{
				// シャドウマップ有効化
				ImGui::Checkbox("ShadowMap", &param_->material.enableShadowMap);

				// ディフューズ有効化
				ImGui::Checkbox("Diffuse", &param_->material.enableDiffuse);

				if (param_->material.enableDiffuse)
				{
					// ハーフランバート有効化
					ImGui::Checkbox("HalfLambert", &param_->material.enableHalfLambert);
				}

				// スペキュラー有効化
				ImGui::Checkbox("Specular", &param_->material.enableSpecular);
				if (param_->material.enableSpecular)
				{
					// ブリンフォン有効化
					ImGui::Checkbox("BlinnPhong", &param_->material.enableBlinnPhong);

					// 光沢度
					ImGui::DragFloat("Shininess", &param_->material.shininess, 0.1f);
				}

				// 環境
				ImGui::SliderFloat("Environment", &param_->material.environment, 0.0f, 1.0f);
			}

			// 終了
			ImGui::TreePop();
		}


		// 分割数
		if (ImGui::TreeNode("Division"))
		{
			// スライス
			ImGui::DragInt("Slices", &param_->division.slices, 1.0f, 3, kMaxSlices);

			// リング
			ImGui::DragInt("Rings", &param_->division.rings, 1.0f, 3, kMaxRings);

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

/// @brief デバッグ用レイピッキング
/// @param ray 
/// @param pickList 
void Engine::Render3DUVSphereData::DebugRayPicker(const Collision3D::Ray& ray, std::vector<std::pair<float, DebugData::DebugGuizmoData*>>& pickList)
{
	// 選択初期化
	if (guizmoData_.isSelect)
	{
		guizmoData_.isSelect = false;
		return;
	}

	// 読み込んでいないと処理しない
	if (!isLoad_)return;

	Collision3D::AABB aabb;
	aabb.center = param_->transform.translate;
	aabb.radius = Vector3(1.0f, 1.0f, 1.0f);

	if (CollisionCheckFunc(aabb, ray))
	{
		std::pair<float, DebugData::DebugGuizmoData*> pick;
		pick.first = Vector3(aabb.center - ray.start).Length();
		pick.second = &guizmoData_;
		pickList.push_back(pick);
	}
}

/// @brief Guizmo操作
/// @return 
void Engine::Render3DUVSphereData::DebugGuizmo(Camera3DStore* cameraStore)
{
	// 読み込んでいないと処理しない
	if (!isLoad_)return;

	// 選択していないときは処理しない
	if (!guizmoData_.isSelect)
		return;

	// Tキー -> 移動
	if (engine_->GetKeyTrigger(DIK_T))guizmoData_.mode = DebugData::GuizmoMode::Translate;

	// Rキー -> 回転
	if (engine_->GetKeyTrigger(DIK_R))guizmoData_.mode = DebugData::GuizmoMode::Rotate;

	// Sキー -> 拡縮
	if (engine_->GetKeyTrigger(DIK_S))guizmoData_.mode = DebugData::GuizmoMode::Scale;

	Matrix4x4 viewMatrix = cameraStore->GetCamera3D().GetViewMatrix();        // worldMatrix_.Inverse()
	Matrix4x4 projMatrix = cameraStore->GetCamera3D().GetProjectionMatrix();  // いつものやつ

	// 現在のSRTからワールド行列を一度だけ生成する
	Quaternion rotateQ =
		ToQuaternion(param_->transform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	// ワールド行列
	Matrix4x4 worldMatrix =
		Make3DScaleMatrix4x4(param_->transform.scale) *
		Make3DRotateMatrix4x4(rotateQ) *
		Make3DTranslateMatrix4x4(param_->transform.translate);

	// Guizmoの操作モードを決定する
	ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
	switch (guizmoData_.mode)
	{
	case DebugData::GuizmoMode::Translate:
		operation = ImGuizmo::TRANSLATE;
		break;
	case DebugData::GuizmoMode::Rotate:
		operation = ImGuizmo::ROTATE;
		break;
	case DebugData::GuizmoMode::Scale:
		operation = ImGuizmo::SCALE;
		break;
	}

	// 操作モードだけを切り替えて同じワールド行列を編集する
	ImGuizmo::Manipulate(&viewMatrix.m[0][0], &projMatrix.m[0][0], operation, ImGuizmo::LOCAL, &worldMatrix.m[0][0]);

	if (ImGuizmo::IsUsing())
	{
		float translation[3];
		float rotation[3];
		float scale[3];

		// ワールド行列からSRT成分を抽出
		ImGuizmo::DecomposeMatrixToComponents(&worldMatrix.m[0][0], translation, rotation, scale);

		// 度数法(Degrees)から弧度法(Radians)へ変換するための係数
		constexpr float DEG2RAD = std::numbers::pi_v<float> / 180.0f;

		switch (guizmoData_.mode)
		{
		case DebugData::GuizmoMode::Translate:
			// 移動成分抽出
			param_->transform.translate = Vector3(translation[0], translation[1], translation[2]);
			break;
		case DebugData::GuizmoMode::Rotate:
			// 回転成分抽出
			param_->transform.rotate.x = rotation[0] * DEG2RAD;
			param_->transform.rotate.y = rotation[1] * DEG2RAD;
			param_->transform.rotate.z = rotation[2] * DEG2RAD;
			break;
		case DebugData::GuizmoMode::Scale:
			// 拡縮成分抽出
			param_->transform.scale = Vector3(scale[0], scale[1], scale[2]);
			break;
		}
	}
}