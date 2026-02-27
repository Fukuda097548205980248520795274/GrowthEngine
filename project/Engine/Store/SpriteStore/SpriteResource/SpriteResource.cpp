#include "SpriteResource.h"
#include "PSO/PSOModel/BasePSOModel.h"
#include "Store/TextureStore/TextureStore.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"

#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

#include "Parameter/SpriteParameter/SpriteParameter.h"

/// @brief コンストラクタ
/// @param hSprite 
Engine::SpriteResource::SpriteResource(SpriteHandle hSprite, TextureHandle hTexture, std::string name, SpriteParameter* parameter)
	: hSprite_(hSprite_), name_(name), parameter_(parameter)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<Sprite::Param>();

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
		parameter_->SetValue(group_, "Texture", &textureFilePath_);

		// 反映させる
		parameter_->RegisterGroupDataReflection(group_);
		param_->texture.hTexture = textureStore_->GetHandle(textureFilePath_);
	}
}

/// @brief 初期化
/// @param vertexResource 
/// @param indexResource 
/// @param device 
void Engine::SpriteResource::Initialize(VertexBufferResource<SpriteVertexData>* vertexResource, IndexBufferResource* indexResource, TextureStore* textureStore,
	ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(vertexResource);
	assert(indexResource);
	assert(textureStore);
	assert(device);

	// 引数を受け取る
	vertexResource_ = vertexResource;
	indexResource_ = indexResource;
	textureStore_ = textureStore;

	// テクスチャサイズを取得する
	param_->texture.size =
		Vector2(static_cast<float>(textureStore_->GetTextureWidth(param_->texture.hTexture)),
			static_cast<float>(textureStore_->GetTextureHeight(param_->texture.hTexture)));

	// マテリアルリソースの生成
	materialResource_ = std::make_unique<ConstantBufferResource<Sprite::MaterialDataForGPU>>();
	materialResource_->Initialize(device, log);

	// 座標変換リソースの生成
	transformationResource_ = std::make_unique<ConstantBufferResource<Sprite::TransformationDataForGPU>>();
	transformationResource_->Initialize(device, log);
}

/// @brief コマンドリストに登録
/// @param commandList 
void Engine::SpriteResource::Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
{
	Matrix4x4 worldMatrix = Make2DScaleMatrix4x4(Vector2(param_->transform.scale.x * param_->texture.size.x, param_->transform.scale.y * param_->texture.size.y))
		* Make3DRotateZMatrix4x4(param_->transform.rotate) * Make2DTranslateMatrix4x4(param_->transform.translate);

	// ワールドビュープロジェクション行列
	transformationResource_->data_->worldViewProjectionMatrix = worldMatrix * viewProjection;

	// アンカー
	transformationResource_->data_->anchor = param_->texture.anchor;

	// 色
	materialResource_->data_->color = param_->material.color;

	// UV
	materialResource_->data_->uvMatrix =
		Make2DScaleMatrix4x4(param_->material.uv.scale) * Make3DRotateZMatrix4x4(param_->material.uv.rotate) * Make2DTranslateMatrix4x4(param_->material.uv.translate);


	/*--------------------------
	    コマンドリストに登録する
	--------------------------*/

	// PSOの登録
	pso->Register(commandList);

	// インデックスリソースの設定
	indexResource_->Register(commandList);

	// 頂点リソースの設定
	vertexResource_->Register(commandList);

	// 座標変換リソースの設定
	transformationResource_->RegisterGraphics(commandList, 0);

	// マテリアルリソースの設定
	materialResource_->RegisterGraphics(commandList, 1);

	// テクスチャの設定
	commandList->SetGraphicsRootDescriptorTable(2, textureStore_->GetSrvGpuHandle(param_->texture.hTexture));

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

/// @brief デバッグ用パラメータ
void Engine::SpriteResource::DebugParameter()
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