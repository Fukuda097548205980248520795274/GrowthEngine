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


	// パラメータに記録と反映
	group_ = "Text_" + name_;
	if (parameter_)
	{
		// トランスフォーム
		parameter_->SetValue(group_, "Transform_Scale", &param_->transform.scale);
		parameter_->SetValue(group_, "Transform_Rotate", &param_->transform.rotate);
		parameter_->SetValue(group_, "Transform_Translate", &param_->transform.translate);
	}


	// テキストデータを取得する
	TextData* textData = fontStore_->GetTextData(hText_);

	// 領域確保
	param_->charTransform.resize(static_cast<int32_t>(textData->hCharTable.size()));
	param_->charTexture.resize(static_cast<int32_t>(textData->hCharTable.size()));
	param_->charMaterial.resize(static_cast<int32_t>(textData->hCharTable.size()));
	materialResource_.resize(static_cast<int32_t>(textData->hCharTable.size()));
	transformationResource_.resize(static_cast<int32_t>(textData->hCharTable.size()));

	for (int i = 0; i < static_cast<int32_t>(textData->hCharTable.size()); ++i)
	{
		// 文字データを取得する
		CharData* charData = fontStore_->GetCharData(textData->hCharTable[i]);

		// トランスフォーム
		param_->charTransform[i].scale = Vector2(static_cast<float>(charData->width), static_cast<float>(charData->height));
		param_->charTransform[i].rotate = 0.0f;

		float space = 0.0f;
		if (i > 0)
		{
			float space = param_->charTransform[i - 1].translate.x;
			param_->charTransform[i].translate = Vector2(static_cast<float>(charData->pixel) + space, 0.0f);
		}
		else
		{
			param_->charTransform[i].translate = Vector2(0.0f, 0.0f);
		}

		// テクスチャ
		param_->charTexture[i].anchor = Vector2(0.0f, 1.0f);

		// マテリアル
		param_->charMaterial[i].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

		if (parameter_)
		{
			// トランスフォーム
			parameter_->SetValue(group_, charData->c + "_Transform_Scale", &param_->charTransform[i].scale);
			parameter_->SetValue(group_, charData->c + "_Transform_Rotate", &param_->charTransform[i].rotate);
			parameter_->SetValue(group_, charData->c + "_Transform_Translate", &param_->charTransform[i].translate);

			// マテリアル
			parameter_->SetValue(group_, charData->c + "_Material_Color", &param_->charMaterial[i].color);

			// テクスチャ
			parameter_->SetValue(group_, charData->c + "_Texture_Anchor", &param_->charTexture[i].anchor);
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

		// テキストデータを取得する
		TextData* textData = fontStore_->GetTextData(hText_);

		for (int i = 0; i < static_cast<int32_t>(textData->hCharTable.size()); ++i)
		{
			// 文字データを取得する
			CharData* charData = fontStore_->GetCharData(textData->hCharTable[i]);

			param_->charTransform[i].scale = Vector2(static_cast<float>(charData->width), static_cast<float>(charData->height));
			param_->charTransform[i].rotate = 0.0f;
			param_->charTransform[i].translate = Vector2(static_cast<float>(charData->width * i), 0.0f);

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
	// ワールド行列
	Matrix4x4 worldMatrix = 
		Make2DScaleMatrix4x4(param_->transform.scale) * Make3DRotateZMatrix4x4(param_->transform.rotate) * Make2DTranslateMatrix4x4(param_->transform.translate);

	// テキストデータを取得する
	TextData* textData = fontStore_->GetTextData(hText_);


	// psoの設定
	pso->Register(commandList);

	// 頂点の設定
	indexResource_->Register(commandList);
	vertexResource_->Register(commandList);

	for (int i = 0; i < static_cast<int32_t>(textData->hCharTable.size()); ++i)
	{
		// マテリアル
		*materialResource_[i]->data_ = param_->charMaterial[i].color;
		materialResource_[i]->RegisterGraphics(commandList, 1);

		// ローカル行列
		Matrix4x4 localMatrix =
			Make2DScaleMatrix4x4(param_->charTransform[i].scale) *
			Make3DRotateZMatrix4x4(param_->charTransform[i].rotate) *
			Make2DTranslateMatrix4x4(param_->charTransform[i].translate);

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

	Collision2D::Sprite sprite;
	sprite.center = Vector2(param_->transform.translate.x, param_->transform.translate.y);
	sprite.radius = Vector2(30.0f, 30.0f);

	if (CollisionCheckFunc(point, sprite))
	{
		std::pair<float, DebugData::DebugGuizmoData*> pick;
		pick.first = 0.0f;
		pick.second = &guizmoData_;
		pickList.push_back(pick);
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
	Matrix4x4 worldMatrix;

	switch (guizmoData_.mode)
	{
	case DebugData::GuizmoMode::Translate:
		// 移動

		// 移動
		worldMatrix = Make2DTranslateMatrix4x4(param_->transform.translate);

		// Guizmo描画
		ImGuizmo::Manipulate(&viewMatrix.m[0][0], &projMatrix.m[0][0], ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, &worldMatrix.m[0][0]);

		// Gizmo を動かしている間だけ、結果を自分の行列系に戻す
		if (ImGuizmo::IsUsing())
		{
			// 平行移動
			param_->transform.translate = Vector2(worldMatrix.m[3][0], worldMatrix.m[3][1]);
		}

		break;

	case DebugData::GuizmoMode::Rotate:
		// 回転

		// 回転 * 移動
		worldMatrix = Make3DRotateZMatrix4x4(param_->transform.rotate) * Make2DTranslateMatrix4x4(param_->transform.translate);

		// Guizmo描画
		ImGuizmo::Manipulate(&viewMatrix.m[0][0], &projMatrix.m[0][0], ImGuizmo::ROTATE, ImGuizmo::LOCAL, &worldMatrix.m[0][0]);

		// Gizmo を動かしている間だけ、結果を自分の行列系に戻す
		if (ImGuizmo::IsUsing())
		{
			float translation[3];
			float rotation[3];
			float scale[3];

			ImGuizmo::DecomposeMatrixToComponents(
				&worldMatrix.m[0][0],
				translation,
				rotation,
				scale
			);

			// 度数法(Degrees)から弧度法(Radians)へ変換するための係数
			constexpr float DEG2RAD = std::numbers::pi_v<float> / 180.0f;

			// rotation[] は度数法（degrees）なので、ラジアンに変換して代入する
			param_->transform.rotate = rotation[2] * DEG2RAD;
		}

		break;

	case DebugData::GuizmoMode::Scale:
		// 拡縮

		// 拡縮 * 移動
		worldMatrix = Make2DScaleMatrix4x4(param_->transform.scale) * Make2DTranslateMatrix4x4(param_->transform.translate);

		// Guizmo描画
		ImGuizmo::Manipulate(&viewMatrix.m[0][0], &projMatrix.m[0][0], ImGuizmo::SCALE, ImGuizmo::LOCAL, &worldMatrix.m[0][0]);

		// Gizmo を動かしている間だけ、結果を自分の行列系に戻す
		if (ImGuizmo::IsUsing())
		{
			param_->transform.scale.x =
				std::sqrt(worldMatrix.m[0][0] * worldMatrix.m[0][0] + worldMatrix.m[0][1] * worldMatrix.m[0][1] + worldMatrix.m[0][2] * worldMatrix.m[0][2]);
			param_->transform.scale.y =
				std::sqrt(worldMatrix.m[1][0] * worldMatrix.m[1][0] + worldMatrix.m[1][1] * worldMatrix.m[1][1] + worldMatrix.m[1][2] * worldMatrix.m[1][2]);
		}

		break;
	}
}