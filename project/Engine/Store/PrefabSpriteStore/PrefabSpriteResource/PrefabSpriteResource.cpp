#include "PrefabSpriteResource.h"
#include "Store/TextureStore/TextureStore.h"
#include "PSO/PSOModel/BasePSOModel.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"
#include "Store/Camera2DStore/Camera2DStore.h"

#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

#include "Parameter/PrefabSpriteParameter/PrefabSpriteParameter.h"

/// @brief 初期化
/// @param vertexResource 
/// @param indexResource 
/// @param textureStore 
/// @param heap 
/// @param device 
/// @param log 
void Engine::PrefabSpriteResource::Initialize(VertexBufferResource<SpriteVertexData>* vertexResource, IndexBufferResource* indexResource,
	TextureStore* textureStore, TextureHandle hTexture, Camera2DStore* cameraStore, DX12Heap* heap, ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(vertexResource);
	assert(indexResource);
	assert(textureStore);
	assert(cameraStore);
	assert(heap);
	assert(device);

	// 引数を受け取る
	vertexResource_ = vertexResource;
	indexResource_ = indexResource;
	textureStore_ = textureStore;
	cameraStore_ = cameraStore;

	// パラメータの生成
	param_ = std::make_unique<Prefab::Sprite::Base::Param>();

	// トラスフォーム
	param_->transform.scale = Vector2(1.0f, 1.0f);
	param_->transform.rotate = 0.0f;
	param_->transform.translate = Vector2(0.0f, 0.0f);

	// マテリアル
	param_->material.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	param_->material.uv.scale = Vector2(1.0f, 1.0f);
	param_->material.uv.rotate = 0.0f;
	param_->material.uv.translate = Vector2(0.0f, 0.0f);

	// テクスチャ
	param_->texture.hTexture = hTexture;
	param_->texture.anchor = Vector2(0.0f, 0.0f);
	textureFilePath_ = textureStore_->GetFilePath(hTexture);

	// テクスチャサイズを取得する
	param_->texture.size =
		Vector2(static_cast<float>(textureStore_->GetTextureWidth(param_->texture.hTexture)),
			static_cast<float>(textureStore_->GetTextureHeight(param_->texture.hTexture)));

	// パラメータに記録と反映
	group_ = "Sprite_" + name_;
	if (parameter_)
	{
		// トランスフォーム
		parameter_->SetValue(group_, "Transform_Scale", &param_->transform.scale);
		parameter_->SetValue(group_, "Transform_Rotate", &param_->transform.rotate);
		parameter_->SetValue(group_, "Transform_Translate", &param_->transform.translate);

		// マテリアル
		parameter_->SetValue(group_, "Material_Color", &param_->material.color);
		parameter_->SetValue(group_, "Material_UV_Scale", &param_->material.uv.scale);
		parameter_->SetValue(group_, "Material_UV_Rotate", &param_->material.uv.rotate);
		parameter_->SetValue(group_, "Material_UV_Translate", &param_->material.uv.translate);

		// テクスチャ
		parameter_->SetValue(group_, "Texture_Anchor", &param_->texture.anchor);
		parameter_->SetValue(group_, "Texture_Size", &param_->texture.size);
		parameter_->SetValue(group_, "Texture", &textureFilePath_);

		// 反映させる
		parameter_->RegisterGroupDataReflection(group_);
		param_->texture.hTexture = textureStore_->GetHandle(textureFilePath_);
	}
	
	// リソースの生成と初期化
	resource_ = std::make_unique<StructuredBufferResource<Prefab::SpriteDataForGPU>>();
	resource_->Initialize(device, heap, numInstance_, log);
}

/// @brief 更新処理
void Engine::PrefabSpriteResource::Update()
{
	// 終了したインスタンスを削除する
	instanceTable_.remove_if([](std::unique_ptr<PrefabInstanceSprite>& instance)
		{
			if (instance->IsDelete()) 
			{ 
				return true; 
			}
			return false; 
		}
	);
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param pso 
void Engine::PrefabSpriteResource::Register(ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
{
	// インスタンスがないときは処理しない
	if (useInstance_ <= 0)
		return;

	// PSOの設定
	pso->Register(commandList);

	// インデックスの設定
	indexResource_->Register(commandList);

	// 頂点の設定
	vertexResource_->Register(commandList);

	// パラメータの設定
	resource_->RegisterGraphics(commandList, 0);

	// テクスチャの設定
	commandList->SetGraphicsRootDescriptorTable(1, textureStore_->GetSrvGpuHandle(param_->texture.hTexture));

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawIndexedInstanced(6, useInstance_, 0, 0, 0);
}

/// @brief インスタンスを作成する
/// @return 
PrefabInstanceSprite* Engine::PrefabSpriteResource::CreateInstance()
{
	// インスタンスを生成する
	std::unique_ptr<PrefabInstanceSprite> instance =
		std::make_unique<PrefabInstanceSprite>([this](const Prefab::Sprite::Instance::Param* param) {InstanceDrawCall(param); }, param_.get());

	// ポインタを保存する
	PrefabInstanceSprite* pInstance = instance.get();

	// テーブルに追加する
	instanceTable_.push_back(std::move(instance));

	return pInstance;
}

/// @brief インスタンスのドローコール
/// @param param 
void Engine::PrefabSpriteResource::InstanceDrawCall(const Prefab::Sprite::Instance::Param* param)
{
	// nullptrチェック
	assert(param);

	// インスタンス数以上のドローコールは処理しない
	if (useInstance_ >= numInstance_)
		return;

	// ワールドビュープロジェクション行列
	resource_->data_[useInstance_].worldViewProjection =
		Make2DScaleMatrix4x4((Vector2(param->transform.scale.x * param->texture.size.x, param->transform.scale.y * param->texture.size.y)))
		* Make3DRotateZMatrix4x4(param->transform.rotate) * Make2DTranslateMatrix4x4(param->transform.translate)
		* cameraStore_->GetCamera2D().GetViewProjectionMatrix();

	// アンカー
	resource_->data_[useInstance_].anchor = param_->texture.anchor;

	// 色
	resource_->data_[useInstance_].color = param->material.color;

	// UV
	resource_->data_[useInstance_].uvTransform =
		Make2DScaleMatrix4x4(param->material.uv.scale) * Make3DRotateZMatrix4x4(param->material.uv.rotate) * Make2DTranslateMatrix4x4(param->material.uv.translate);


	// 使用インスタンス数をカウントする
	useInstance_++;
}

/// @brief デバッグ用パラメータ
void Engine::PrefabSpriteResource::DebugParameter()
{
#ifdef _DEVELOPMENT

	// モデル名
	if (ImGui::TreeNode(name_.c_str()))
	{
		// モデルトランスフォーム
		if (ImGui::TreeNode("Transform"))
		{
			// 拡縮
			ImGui::DragFloat2("Scale", &param_->transform.scale.x, 0.01f, -100000.0f, 100000.0f);

			// 回転
			ImGui::DragFloat("Rotate", &param_->transform.rotate, 0.01f, -100000.0f, 100000.0f);

			// 平行移動
			ImGui::DragFloat2("Translate", &param_->transform.translate.x, 0.01f, -100000.0f, 100000.0f);

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
				ImGui::DragFloat("Rotate", &param_->material.uv.rotate, 0.01f, -100000.0f, 100000.0f);

				// 平行移動
				ImGui::DragFloat2("Translate", &param_->material.uv.translate.x, 0.01f, -100000.0f, 100000.0f);

				// 終了
				ImGui::TreePop();
			}

			// 色
			ImGui::ColorEdit4("Color", &param_->material.color.x);

			// 終了
			ImGui::TreePop();
		}

		// テクスチャ
		if (ImGui::TreeNode("Texture"))
		{
			// アンカー
			ImGui::DragFloat2("Anchor", &param_->texture.anchor.x, 0.01f);

			// サイズ
			ImGui::DragFloat2("Size", &param_->texture.anchor.x, 1.0f);

			// テクスチャ
			ImGui::Text("\n");

			ImGui::ImageButton(
				textureStore_->GetFilePath(param_->texture.hTexture).c_str(),
				textureStore_->GetSrvGpuHandle(param_->texture.hTexture).ptr,
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
					param_->texture.hTexture = static_cast<uint32_t>(droppedIndex);
					textureFilePath_ = textureStore_->GetFilePath(param_->texture.hTexture);
				}
				ImGui::EndDragDropTarget();
			}

			// 終了
			ImGui::TreePop();
		}

		ImGui::Text("\n");

		// インスタンス量
		ImGui::Text("Instance \n");
		ImGui::ProgressBar(static_cast<float>(useInstance_) / static_cast<float>(numInstance_), ImVec2(200.0f, 20.0f),
			std::format("{} / {}", useInstance_, numInstance_).c_str());

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