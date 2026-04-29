#include "Render2DTextData.h"
#include "Store/FontStore/FontStore.h"
#include "Parameter/Render2DParameter/Render2DParameter.h"
#include "PSO/PSOModel/BasePSOModel.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"

#include "Func/CollisionFunc/CollisionFunc.h"

#include <numbers>

#include "GrowthEngine.h"

/// @brief 初期化
/// @param vertexResource 
/// @param indexResource 
/// @param device 
void Engine::Render2DTextData::Initialize(VertexBufferResource<SpriteVertexData>* vertexResource, IndexBufferResource* indexResource,
	FontStore* fontStore, ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(vertexResource);
	assert(indexResource);
	assert(fontStore);
	assert(device);

	// 引数を受け取る
	vertexResource_ = vertexResource;
	indexResource_ = indexResource;
	fontStore_ = fontStore;

	// パラメータの生成と初期化
	param_ = std::make_unique<Render2D::Text::Param>();

	// トランスフォーム
	param_->transform.scale = Vector2(1.0f, 1.0f);
	param_->transform.rotate = 0.0f;
	param_->transform.translate = Vector2(0.0f, 0.0f);

	// テクスチャ
	param_->texture.anchor = Vector2(0.0f, 0.0f);

	// 画面のアンカー
	param_->screenAnchor = Render2D::ScreenAnchor::LeftBottom;


	// パラメータに記録と反映
	group_ = "Text_" + name_;
	if (parameter_)
	{
		// トランスフォーム
		parameter_->SetValue(group_, "Transform_Scale", &param_->transform.scale);
		parameter_->SetValue(group_, "Transform_Rotate", &param_->transform.rotate);
		parameter_->SetValue(group_, "Transform_Translate", &param_->transform.translate);

		// テクスチャ
		parameter_->SetValue(group_, "Texture_Anchor", &param_->texture.anchor);

		// 画面のアンカー
		parameter_->SetValue(group_, "ScreenAnchor", &param_->screenAnchor);
	}


	// テキストデータを取得する
	TextData* textData = fontStore_->GetTextData(hText_);

	// 領域確保
	param_->charTransform.resize(static_cast<int32_t>(textData->hCharTable.size()));
	param_->charTexture.resize(static_cast<int32_t>(textData->hCharTable.size()));
	param_->charMaterial.resize(static_cast<int32_t>(textData->hCharTable.size()));
	materialResource_.resize(static_cast<int32_t>(textData->hCharTable.size()));
	transformationResource_.resize(static_cast<int32_t>(textData->hCharTable.size()));


	// 描画基準点のX座標
	float penX = 0.0f;

	for (int i = 0; i < static_cast<int32_t>(textData->hCharTable.size()); ++i)
	{
		// 文字データを取得する
		CharData* charData = fontStore_->GetCharData(textData->hCharTable[i]);

		// 本当の描画開始位置
		float drawX = penX + static_cast<float>(charData->bearing.x);
		float drawY = 0.0f;

		// トランスフォーム
		param_->charTransform[i].scale = Vector2(static_cast<float>(charData->size.x), static_cast<float>(charData->size.y));
		param_->charTransform[i].rotate = 0.0f;
		param_->charTransform[i].translate = Vector2(drawX, drawY);

		// advanceは「1/64ピクセル単位」で格納されているので、64で割る（>> 6）必要があります
		penX += static_cast<float>(charData->advance >> 6);

		// テクスチャ
		param_->charTexture[i].anchor = Vector2(0.0f, 1.0f);

		// マテリアル
		param_->charMaterial[i].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

		if (parameter_)if (charData->size.x > 0 && charData->size.y > 0)
		{
			// トランスフォーム
			parameter_->SetValue(group_, charData->str + "_Transform_Scale", &param_->charTransform[i].scale);
			parameter_->SetValue(group_, charData->str + "_Transform_Rotate", &param_->charTransform[i].rotate);
			parameter_->SetValue(group_, charData->str + "_Transform_Translate", &param_->charTransform[i].translate);

			// マテリアル
			parameter_->SetValue(group_, charData->str + "_Material_Color", &param_->charMaterial[i].color);

			// テクスチャ
			parameter_->SetValue(group_, charData->str + "_Texture_Anchor", &param_->charTexture[i].anchor);
		}

		// マテリアルリソースの生成と初期化
		materialResource_[i] = std::make_unique<ConstantBufferResource<Vector4>>();
		materialResource_[i]->Initialize(device, log);

		// 座標変換リソースの生成と初期化
		transformationResource_[i] = std::make_unique<ConstantBufferResource<Sprite::TransformationDataForGPU>>();
		transformationResource_[i]->Initialize(device, log);
	}

	// 反映させる
	parameter_->RegisterGroupDataReflection(group_);
}

/// @brief リセット
void Engine::Render2DTextData::Reset()
{
	if (parameter_->IsFileFound(group_))
	{
		// 反映させる
		parameter_->RegisterGroupDataReflection(group_);
	}
	else
	{
		// トランスフォーム
		param_->transform.scale = Vector2(1.0f, 1.0f);
		param_->transform.rotate = 0.0f;
		param_->transform.translate = Vector2(0.0f, 0.0f);

		// テクスチャ
		param_->texture.anchor = Vector2(0.0f, 0.0f);

		// 画面のアンカー
		param_->screenAnchor = Render2D::ScreenAnchor::LeftBottom;

		// テキストデータを取得する
		TextData* textData = fontStore_->GetTextData(hText_);

		// 描画基準点のX座標
		float penX = 0.0f;

		for (int i = 0; i < static_cast<int32_t>(textData->hCharTable.size()); ++i)
		{
			// 文字データを取得する
			CharData* charData = fontStore_->GetCharData(textData->hCharTable[i]);

			// 本当の描画開始位置
			float drawX = penX + static_cast<float>(charData->bearing.x);
			float drawY = 0.0f;

			param_->charTransform[i].scale = Vector2(static_cast<float>(charData->size.x), static_cast<float>(charData->size.y));
			param_->charTransform[i].rotate = 0.0f;
			param_->charTransform[i].translate = Vector2(drawX, drawY);

			// advanceは「1/64ピクセル単位」で格納されているので、64で割る（>> 6）必要があります
			penX += static_cast<float>(charData->advance >> 6);

			param_->charTexture[i].anchor = Vector2(0.0f, 1.0f);

			param_->charMaterial[i].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	// 読み込む
	isLoad_ = true;
}

/// @brief コマンドリストに登録
/// @param commandList 
void Engine::Render2DTextData::Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
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
	Matrix4x4 worldMatrix =
		Make2DScaleMatrix4x4(param_->transform.scale) * Make3DRotateZMatrix4x4(param_->transform.rotate) * Make2DTranslateMatrix4x4(translate);

	// テキストデータを取得する
	TextData* textData = fontStore_->GetTextData(hText_);


	float minX = 0.0f;
	float maxX = 0.0f;
	float minY = 0.0f;
	float maxY = 0.0f;
	bool hasRect = false;

	for (int i = 0; i < static_cast<int32_t>(textData->hCharTable.size()); ++i)
	{
		CharData* charData = fontStore_->GetCharData(textData->hCharTable[i]);
		if (charData->size.x <= 0 || charData->size.y <= 0)
			continue;

		const Vector2& anchor = param_->charTexture[i].anchor;
		const Vector2& size = Vector2(static_cast<float>(charData->size.x), static_cast<float>(charData->size.y));
		const Vector2& translate = param_->charTransform[i].translate;

		const float left = -anchor.x * size.x + translate.x;
		const float right = (1.0f - anchor.x) * size.x + translate.x;
		const float top = anchor.y * size.y + translate.y;
		const float bottom = (1.0f - anchor.y) * size.y + translate.y;

		if (!hasRect)
		{
			minX = left;
			maxX = right;
			minY = bottom;
			maxY = top;
			hasRect = true;
		}
		else
		{
			minX = (std::min)(minX, left);
			maxX = (std::max)(maxX, right);
			minY = (std::min)(minY, bottom);
			maxY = (std::max)(maxY, top);
		}
	}

	Vector2 textAnchorOffset = Vector2(0.0f, 0.0f);
	if (hasRect)
	{
		textAnchorOffset = Vector2((maxX - minX) * param_->texture.anchor.x, (maxY - minY) * param_->texture.anchor.y);
	}


	// psoの設定
	pso->Register(commandList);

	// 頂点の設定
	indexResource_->Register(commandList);
	vertexResource_->Register(commandList);

	for (int i = 0; i < static_cast<int32_t>(textData->hCharTable.size()); ++i)
	{
		// 文字データを取得する
		CharData* charData = fontStore_->GetCharData(textData->hCharTable[i]);

		// スペース以外
		if (charData->size.x <= 0 || charData->size.y <= 0)
			continue;

		// マテリアル
		*materialResource_[i]->data_ = param_->charMaterial[i].color;
		materialResource_[i]->RegisterGraphics(commandList, 1);

		const Vector2 translate = Vector2(
			param_->charTransform[i].translate.x - textAnchorOffset.x,
			param_->charTransform[i].translate.y - textAnchorOffset.y);

		// ローカル行列
		Matrix4x4 localMatrix =
			Make2DScaleMatrix4x4(param_->charTransform[i].scale) *
			Make3DRotateZMatrix4x4(param_->charTransform[i].rotate) *
			Make2DTranslateMatrix4x4(translate);

		// 座標変換
		transformationResource_[i]->data_->anchor = param_->charTexture[i].anchor;
		transformationResource_[i]->data_->worldViewProjectionMatrix = (localMatrix * worldMatrix) * viewProjection;
		transformationResource_[i]->RegisterGraphics(commandList, 0);

		// テクスチャ
		commandList->SetGraphicsRootDescriptorTable(2, fontStore_->GetCharData(textData->hCharTable[i])->srvHandle.second);

		// 形状
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// ドローコール
		commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}
}

/// @brief デバッグ用パラメータ
void Engine::Render2DTextData::DebugParameter()
{
#ifdef _DEVELOPMENT

	// 読み込んでいないと処理しない
	if (!isLoad_)return;

	// モデル名
	if (ImGui::TreeNode(name_.c_str()))
	{
		// 画面アンカー
		const char* type[] = { "LeftTop", "Top", "RightTop" , "Left" , "Center" , "Right","LeftBottom", "Bottom", "RightBottom" };
		int32_t shapeIndex = static_cast<int32_t>(param_->screenAnchor);
		if (ImGui::Combo("Screen Anchor", &shapeIndex, type, IM_ARRAYSIZE(type)))
		{
			param_->screenAnchor = static_cast<Render2D::ScreenAnchor>(shapeIndex);
		}


		// トランスフォーム
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

		// テクスチャ
		if (ImGui::TreeNode("Texture"))
		{
			// 拡縮
			ImGui::DragFloat2("Anchor", &param_->texture.anchor.x, 0.01f, -100000.0f, 100000.0f);

			// 終了
			ImGui::TreePop();
		}

		// テキストデータを取得する
		TextData* textData = fontStore_->GetTextData(hText_);

		for (int i = 0; i < static_cast<int32_t>(textData->hCharTable.size()); ++i)
		{
			// 文字データを取得する
			CharData* charData = fontStore_->GetCharData(textData->hCharTable[i]);

			// スペース以外
			if (charData->size.x <= 0 || charData->size.y <= 0)
				continue;

			// 文字ごとに
			if (ImGui::TreeNode(charData->str.c_str()))
			{
				// トランスフォーム
				if (ImGui::TreeNode("Transform"))
				{
					// 拡縮
					ImGui::DragFloat2("Scale", &param_->charTransform[i].scale.x, 0.01f, -100000.0f, 100000.0f);
					// 回転
					ImGui::DragFloat("Rotate", &param_->charTransform[i].rotate, 0.01f, -100000.0f, 100000.0f);
					// 平行移動
					ImGui::DragFloat2("Translate", &param_->charTransform[i].translate.x, 0.01f, -100000.0f, 100000.0f);
					// 終了
					ImGui::TreePop();
				}

				// マテリアル
				if (ImGui::TreeNode("Material"))
				{
					// 色
					ImGui::ColorEdit4("Color", &param_->charMaterial[i].color.x);

					// 終了
					ImGui::TreePop();
				}

				// テクスチャ
				if (ImGui::TreeNode("Texture"))
				{
					// アンカー
					ImGui::DragFloat2("Anchor", &param_->charTexture[i].anchor.x, 0.01f);

					// 終了
					ImGui::TreePop();
				}

				// 終了
				ImGui::TreePop();
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

/// @brief デバッグ用ピッキング
/// @param point 
/// @param pickList 
void Engine::Render2DTextData::DebugPicking(const Vector2& point, std::vector<std::pair<float, DebugData::DebugGuizmoData*>>& pickList)
{
	// 選択初期化
	if (guizmoData_.isSelect)
	{
		guizmoData_.isSelect = false;
		return;
	}

	// 読み込んでいないと処理しない
	if (!isLoad_)return;

	// テキストデータを取得する
	TextData* textData = fontStore_->GetTextData(hText_);
	
	float minX = 0.0f;
	float maxX = 0.0f;
	float minY = 0.0f;
	float maxY = 0.0f;
	bool hasRect = false;

	for (int i = 0; i < static_cast<int32_t>(textData->hCharTable.size()); ++i)
	{
		CharData* charData = fontStore_->GetCharData(textData->hCharTable[i]);
		if (charData->size.x <= 0 || charData->size.y <= 0)
			continue;

		const Vector2& anchor = param_->charTexture[i].anchor;
		const Vector2& size = Vector2(static_cast<float>(charData->size.x), static_cast<float>(charData->size.y));
		const Vector2& translate = param_->charTransform[i].translate;

		const float left = -anchor.x * size.x + translate.x;
		const float right = (1.0f - anchor.x) * size.x + translate.x;
		const float top = anchor.y * size.y + translate.y;
		const float bottom = (1.0f - anchor.y) * size.y + translate.y;

		if (!hasRect)
		{
			minX = left;
			maxX = right;
			minY = bottom;
			maxY = top;
			hasRect = true;
		}
		else
		{
			minX = (std::min)(minX, left);
			maxX = (std::max)(maxX, right);
			minY = (std::min)(minY, bottom);
			maxY = (std::max)(maxY, top);
		}
	}

	Vector2 textAnchorOffset = Vector2(0.0f, 0.0f);
	if (hasRect)
	{
		textAnchorOffset = Vector2((maxX - minX) * param_->texture.anchor.x, (maxY - minY) * param_->texture.anchor.y);
	}

	// テキスト行列
	Matrix4x4 textMatrix =
		Make2DScaleMatrix4x4(param_->transform.scale) * Make3DRotateZMatrix4x4(param_->transform.rotate) * Make2DTranslateMatrix4x4(param_->transform.translate);

	for (int i = 0; i < static_cast<int32_t>(param_->charTransform.size()); ++i)
	{
		CharData* charData = fontStore_->GetCharData(textData->hCharTable[i]);
		if (charData->size.x <= 0 || charData->size.y <= 0)
			continue;

		const Vector2 charTranslate = Vector2(
			param_->charTransform[i].translate.x - textAnchorOffset.x,
			param_->charTransform[i].translate.y - textAnchorOffset.y);

		// 文字行列
		Matrix4x4 charMatrix =
			Make2DScaleMatrix4x4(param_->charTransform[i].scale) * Make3DRotateZMatrix4x4(param_->charTransform[i].rotate) * Make2DTranslateMatrix4x4(charTranslate);

		// ワールド行列
		Matrix4x4 worldMatrix = charMatrix * textMatrix;

		Vector2 pos = Vector2(worldMatrix.m[3][0], worldMatrix.m[3][1]);

		// サイズ
		Vector2 size = Vector2(static_cast<float>(charData->size.x), static_cast<float>(charData->size.y));

		float left = -param_->charTexture[i].anchor.x * (size.x * param_->transform.scale.x) + pos.x;
		float right = (1.0f - param_->charTexture[i].anchor.x) * (size.x * param_->transform.scale.x) + pos.x;
		float top = param_->charTexture[i].anchor.y * (size.y * param_->transform.scale.y) + pos.y;
		float bottom = (1.0f - param_->charTexture[i].anchor.y) * (size.y * param_->transform.scale.y) + pos.y;

		if (point.x >= left && point.x <= right &&
			point.y <= top && point.y >= bottom)
		{
			// 当たっている

			std::pair<float, DebugData::DebugGuizmoData*> pick;
			pick.first = 0.0f;
			pick.second = &guizmoData_;
			pickList.push_back(pick);

			return;
		}
	}
}

/// @brief Guizmo操作
/// @param viewMatrix 
/// @param projMatrix 
void Engine::Render2DTextData::DebugGuizmo(const Matrix4x4& viewMatrix, const Matrix4x4& projMatrix)
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

	// Guizmo操作の切り替え
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

	// Guizmo操作
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
			// 回転成分抽出（Z軸回転のみ）
			param_->transform.rotate = rotation[2] * DEG2RAD;
			break;
		case DebugData::GuizmoMode::Scale:
			// 拡縮成分抽出
			param_->transform.scale = Vector2(scale[0], scale[1]);
			break;
		}
	}
}