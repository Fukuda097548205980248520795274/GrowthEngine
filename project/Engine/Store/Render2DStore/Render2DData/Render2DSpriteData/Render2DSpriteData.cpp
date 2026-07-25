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