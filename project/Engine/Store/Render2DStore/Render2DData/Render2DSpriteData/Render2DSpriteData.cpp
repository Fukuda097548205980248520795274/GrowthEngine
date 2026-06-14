#include "Render2DSpriteData.h"
#include "PSO/PSOModel/BasePSOModel.h"
#include "Store/TextureStore/TextureStore.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"
#include "Parameter/Render2DParameter/Render2DParameter.h"
#include <numbers>

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "Func/CollisionFunc/CollisionFunc.h"

#include "GrowthEngine.h"

/// @brief 初期化
/// @param vertexResource 
/// @param indexResource 
/// @param device 
void Engine::Render2DSpriteData::Initialize(VertexBufferResource<SpriteVertexData>* vertexResource, IndexBufferResource* indexResource,
	TextureStore* textureStore, TextureHandle hTexture, ID3D12Device* device, Log* log)
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


	// パラメータの生成と初期化
	param_ = std::make_unique<Render2D::Sprite::Param>();

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

	// 画面のアンカー
	param_->screenAnchor = Render2D::ScreenAnchor::LeftBottom;

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

		// 画面のアンカー
		parameter_->SetValue(group_, "ScreenAnchor", &param_->screenAnchor);

		// 反映させる
		parameter_->RegisterGroupDataReflection(group_);
		param_->material.hTexture = textureStore_->GetHandle(textureFilePath_);
	}

	// マテリアルリソースの生成
	materialResource_ = std::make_unique<ConstantBufferResource<Sprite::MaterialDataForGPU>>();
	materialResource_->Initialize(device, log);

	// 座標変換リソースの生成
	transformationResource_ = std::make_unique<ConstantBufferResource<Sprite::TransformationDataForGPU>>();
	transformationResource_->Initialize(device, log);
}

/// @brief リセット
void Engine::Render2DSpriteData::Reset()
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

		// 画面のアンカー
		param_->screenAnchor = Render2D::ScreenAnchor::LeftBottom;
	}

	// 読み込む
	isLoad_ = true;
}

/// @brief コマンドリストに登録
/// @param commandList 
void Engine::Render2DSpriteData::Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
{
	Vector2 screenSize = Vector2(static_cast<float>(engine_->GetScreenWidth()), static_cast<float>(engine_->GetScreenHeight()));

	switch (param_->screenAnchor)
	{
	case Render2D::ScreenAnchor::LeftBottom:
		// 左下
		screenSize = Vector2(0.0f, 0.0f);
		break;

	case Render2D::ScreenAnchor::LeftTop:
		// 左上
		screenSize.x = 0.0f;
		break;

	case Render2D::ScreenAnchor::RightBottom:
		// 右下
		screenSize.y = 0.0f;
		break;

	case Render2D::ScreenAnchor::Center:
		// 中心
		screenSize /= 2.0f;
		break;

	case Render2D::ScreenAnchor::Left:
		// 左
		screenSize.x = 0.0f;
		screenSize.y /= 2.0f;
		break;

	case Render2D::ScreenAnchor::Right:
		// 右
		screenSize.y /= 2.0f;
		break;

	case Render2D::ScreenAnchor::Top:
		// 上
		screenSize.x /= 2.0f;
		break;

	case Render2D::ScreenAnchor::Bottom:
		// 下
		screenSize.x /= 2.0f;
		screenSize.y = 0.0f;
		break;
	}

	Vector2 translate = param_->transform.translate + screenSize;

	// ワールド行列
	Matrix4x4 worldMatrix = Make2DScaleMatrix4x4(Vector2(param_->transform.scale.x * param_->texture.size.x, param_->transform.scale.y * param_->texture.size.y))
		* Make3DRotateZMatrix4x4(param_->transform.rotate) * Make2DTranslateMatrix4x4(translate);
	if (parent_) worldMatrix = worldMatrix * parent_->GetWorldMatrix();

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
	pso->Register(commandList, param_->blendMode);

	// インデックスリソースの設定
	indexResource_->Register(commandList);

	// 頂点リソースの設定
	vertexResource_->Register(commandList);

	// 座標変換リソースの設定
	transformationResource_->RegisterGraphics(commandList, 0);

	// マテリアルリソースの設定
	materialResource_->RegisterGraphics(commandList, 1);

	// テクスチャの設定
	commandList->SetGraphicsRootDescriptorTable(2, textureStore_->GetSrvGpuHandle(param_->material.hTexture));

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

/// @brief デバッグ用パラメータ
void Engine::Render2DSpriteData::DebugParameter()
{
#ifdef _DEVELOPMENT

	// 読み込んでいないと処理しない
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

		// 画面アンカー
		const char* type[] = { "LeftTop", "Top", "RightTop" , "Left" , "Center" , "Right","LeftBottom", "Bottom", "RightBottom" };
		int32_t shapeIndex = static_cast<int32_t>(param_->screenAnchor);
		if (ImGui::Combo("Screen Anchor", &shapeIndex, type, IM_ARRAYSIZE(type)))
		{
			param_->screenAnchor = static_cast<Render2D::ScreenAnchor>(shapeIndex);
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

			// テクスチャサイズ
			ImGui::DragFloat2("Size", &param_->texture.size.x, 1.0f);

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

/// @brief デバッグ用ピッキング
/// @param point 
/// @param pickList 
void Engine::Render2DSpriteData::DebugPicking(const Vector2& point, std::vector<std::pair<float, DebugData::DebugGuizmoData*>>& pickList)
{
	// 選択初期化
	if (guizmoData_.isSelect)
	{
		guizmoData_.isSelect = false;
		return;
	}

	// 読み込んでいないと処理しない
	if (!isLoad_)return;


	float left = -param_->texture.anchor.x * (param_->texture.size.x * param_->transform.scale.x) + param_->transform.translate.x;
	float right = (1.0f - param_->texture.anchor.x) * (param_->texture.size.x * param_->transform.scale.x) + param_->transform.translate.x;
	float top = (1.0f - param_->texture.anchor.y) * (param_->texture.size.y * param_->transform.scale.y) + param_->transform.translate.y;
	float bottom =  -param_->texture.anchor.y * (param_->texture.size.y * param_->transform.scale.y) + param_->transform.translate.y;

	if (point.x >= left && point.x <= right &&
		point.y <= top && point.y >= bottom)
	{
		// 当たっている

		std::pair<float, DebugData::DebugGuizmoData*> pick;
		pick.first = 0.0f;
		pick.second = &guizmoData_;
		pickList.push_back(pick);
	}
}

/// @brief Guizmo操作
/// @param viewMatrix 
/// @param projMatrix 
void Engine::Render2DSpriteData::DebugGuizmo(const Matrix4x4& viewMatrix, const Matrix4x4& projMatrix)
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

	// ワールド行列
	Matrix4x4 worldMatrix =
		Make2DScaleMatrix4x4(param_->transform.scale) *
		Make3DRotateZMatrix4x4(param_->transform.rotate) *
		Make2DTranslateMatrix4x4(param_->transform.translate);

	// Guizmoの操作モードに応じて、操作する成分を決める
	ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE_X | ImGuizmo::TRANSLATE_Y;
	switch (guizmoData_.mode)
	{
	case DebugData::GuizmoMode::Translate:
		operation = ImGuizmo::TRANSLATE_X | ImGuizmo::TRANSLATE_Y;
		break;
	case DebugData::GuizmoMode::Rotate:
		operation = ImGuizmo::ROTATE_Z;
		break;
	case DebugData::GuizmoMode::Scale:
		operation = ImGuizmo::SCALE_X | ImGuizmo::SCALE_Y;
		break;
	}

	// ImGuizmoを描画して、操作する
	ImGuizmo::Manipulate(&viewMatrix.m[0][0], &projMatrix.m[0][0], operation, ImGuizmo::LOCAL, &worldMatrix.m[0][0]);

	if (ImGuizmo::IsUsing())
	{
		float translation[3];
		float rotation[3];
		float scale[3];

		// ワールド行列から分解して、パラメータに反映させる
		ImGuizmo::DecomposeMatrixToComponents(&worldMatrix.m[0][0], translation, rotation, scale);

		// 度数法(Degrees)から弧度法(Radians)へ変換するための係数
		constexpr float DEG2RAD = std::numbers::pi_v<float> / 180.0f;

		switch (guizmoData_.mode)
		{
		case DebugData::GuizmoMode::Translate:
			// 移動成分抽出
			param_->transform.translate = Vector2(translation[0], translation[1]);
			break;
		case DebugData::GuizmoMode::Rotate:
			// 回転成分抽出
			param_->transform.rotate = rotation[2] * DEG2RAD;
			break;
		case DebugData::GuizmoMode::Scale:
			// 拡縮成分抽出
			param_->transform.scale = Vector2(scale[0], scale[1]);
			break;
		}
	}
}