#include "Prefab2DSpriteData.h"
#include "Store/TextureStore/TextureStore.h"
#include "PSO/PSOModel/BasePSOModel.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"
#include "Store/Camera2DStore/Camera2DStore.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "Parameter/Prefab2DParameter/Prefab2DParameter.h"
#include "Application/WorldTransform/WorldTransform2D/WorldTransform2D.h"

/// @brief 初期化
/// @param vertexResource 
/// @param indexResource 
/// @param textureStore 
/// @param heap 
/// @param device 
/// @param log 
void Engine::Prefab2DSpriteData::Initialize(VertexBufferResource<SpriteVertexData>* vertexResource, IndexBufferResource* indexResource,
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
	param_ = std::make_unique<Prefab2D::Sprite::Base::Param>();

	// ブレンドモード
	param_->blendMode = BlendMode::kNormal;

	// トラスフォーム
	param_->transform.scale = Vector2(1.0f, 1.0f);
	param_->transform.rotate = 0.0f;
	param_->transform.translate = Vector2(0.0f, 0.0f);

	// マテリアル
	param_->material.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	param_->material.uv.scale = Vector2(1.0f, 1.0f);
	param_->material.uv.rotate = 0.0f;
	param_->material.uv.translate = Vector2(0.0f, 0.0f);
	param_->material.hTexture = hTexture;

	// テクスチャ
	param_->texture.anchor = Vector2(0.5f, 0.5f);
	textureFilePath_ = textureStore_->GetFilePath(hTexture);

	// テクスチャサイズを取得する
	param_->texture.size =
		Vector2(static_cast<float>(textureStore_->GetTextureWidth(param_->material.hTexture)),
			static_cast<float>(textureStore_->GetTextureHeight(param_->material.hTexture)));

	// パラメータに記録と反映
	group_ = "Sprite_" + name_;
	if (parameter_)
	{
		// ブレンドモード
		parameter_->SetValue(group_, "BlendMode", &param_->blendMode);

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
		param_->material.hTexture = textureStore_->GetHandle(textureFilePath_);
	}
	
	// リソースの生成と初期化
	resource_ = std::make_unique<StructuredBufferResource<Prefab::SpriteDataForGPU>>();
	resource_->Initialize(device, heap, numInstance_, log);
}

/// @brief 更新処理
void Engine::Prefab2DSpriteData::Update()
{
	// 終了したインスタンスを削除する
	instanceTable_.remove_if([](std::unique_ptr<PrefabInstanceSprite>& instance) {if (instance->isDelete_) { return true; }return false; });
}

/// @brief リセット
void Engine::Prefab2DSpriteData::Reset()
{
	if (parameter_->IsFileFound(group_))
	{
		// 反映させる
		parameter_->RegisterGroupDataReflection(group_);
		param_->material.hTexture = textureStore_->GetHandle(textureFilePath_);
	}
	else
	{
		// ブレンドモード
		param_->blendMode = BlendMode::kNormal;

		// トラスフォーム
		param_->transform.scale = Vector2(1.0f, 1.0f);
		param_->transform.rotate = 0.0f;
		param_->transform.translate = Vector2(0.0f, 0.0f);

		// マテリアル
		param_->material.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		param_->material.uv.scale = Vector2(1.0f, 1.0f);
		param_->material.uv.rotate = 0.0f;
		param_->material.uv.translate = Vector2(0.0f, 0.0f);
		param_->material.hTexture = textureStore_->GetHandle(textureFilePath_);

		// テクスチャ
		param_->texture.anchor = Vector2(0.5f, 0.5f);
		param_->texture.size =
			Vector2(static_cast<float>(textureStore_->GetTextureWidth(param_->material.hTexture)),
				static_cast<float>(textureStore_->GetTextureHeight(param_->material.hTexture)));
	}

	// 読み込まれたことにする
	isLoad_ = true;
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param pso 
void Engine::Prefab2DSpriteData::Register(ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
{
	// 読み込まれていないと処理しない
	if (!isLoad_)return;

	// インスタンスがないときは処理しない
	if (useInstance_ <= 0)
		return;


	// PSOの設定
	pso->Register(commandList, param_->blendMode);

	// インデックスの設定
	indexResource_->Register(commandList);

	// 頂点の設定
	vertexResource_->Register(commandList);

	// パラメータの設定
	resource_->RegisterGraphics(commandList, 0);

	// テクスチャの設定
	commandList->SetGraphicsRootDescriptorTable(1, textureStore_->GetSrvGpuHandle(param_->material.hTexture));

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawIndexedInstanced(6, useInstance_, 0, 0, 0);
}

/// @brief インスタンスを作成する
/// @return 
void* Engine::Prefab2DSpriteData::CreateInstance()
{
	// インスタンスを生成する
	std::unique_ptr<PrefabInstanceSprite> instance =
		std::make_unique<PrefabInstanceSprite>([this](const Prefab2D::Sprite::Instance::Param* param) {InstanceDrawCall(param); }, param_.get());

	// ポインタを保存する
	PrefabInstanceSprite* pInstance = instance.get();

	// テーブルに追加する
	instanceTable_.push_back(std::move(instance));

	return pInstance;
}

/// @brief インスタンスのドローコール
/// @param param 
void Engine::Prefab2DSpriteData::InstanceDrawCall(const Prefab2D::Sprite::Instance::Param* param)
{
	// nullptrチェック
	assert(param);

	// 読み込まれていないと処理しない
	if (!isLoad_)return;

	// インスタンス数以上のドローコールは処理しない
	if (useInstance_ >= numInstance_)
		return;

	// ワールド行列
	Matrix4x4 worldMatrix = Make2DScaleMatrix4x4((Vector2(param->transform.scale.x * param->texture.size.x, param->transform.scale.y * param->texture.size.y)))
		* Make3DRotateZMatrix4x4(param->transform.rotate) * Make2DTranslateMatrix4x4(param->transform.translate);

	// 親がいるときは親のワールド行列を掛ける
	if (param->parent)worldMatrix = worldMatrix * param->parent->GetWorldMatrix();

	// ワールドビュープロジェクション行列
	resource_->data_[useInstance_].worldViewProjection = worldMatrix * cameraStore_->GetCamera2D().GetCurrentVPMatrix();

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
void Engine::Prefab2DSpriteData::DebugParameter()
{
#ifdef _DEVELOPMENT

	// 読み込まれていないと処理しない
	if (!isLoad_)return;

	// モデル名
	if (ImGui::TreeNode(name_.c_str()))
	{
		// ブレンドモード
		const char* blendModeStr[] = { "None", "Normal", "Add", "Subtract", "Multiply", "Screen" };
		int32_t currentBlendMode = static_cast<int32_t>(param_->blendMode);
		if (ImGui::Combo("BlendMode", &currentBlendMode, blendModeStr, IM_ARRAYSIZE(blendModeStr)))
		{
			param_->blendMode = static_cast<BlendMode>(currentBlendMode);
		}

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


			// テクスチャ
			ImGui::Text("\n");

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