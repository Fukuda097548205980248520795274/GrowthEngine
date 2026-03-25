#pragma once
#include "../Render2DBaseData.h"

namespace Engine
{
	class Log;
	class BasePSOModel;
	class TextureStore;
	class IndexBufferResource;
	class Render2DParameter;

	class Render2DSpriteData : public Render2DBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param hSprite 
		Render2DSpriteData(Render2DHandle hRender2D, std::string name, Render2DParameter* parameter) 
			: Render2DBaseData(hRender2D, name, parameter) {
			type_ = Render2D::Type::Sprite;
		}

		/// @brief 初期化
		/// @param vertexResource 
		/// @param indexResource 
		/// @param device 
		void Initialize(VertexBufferResource<SpriteVertexData>* vertexResource, IndexBufferResource* indexResource, TextureStore* textureStore, TextureHandle hTexture,
			ID3D12Device* device, Log* log);

		/// @brief リセット
		void Reset() override;

		/// @brief コマンドリストに登録
		/// @param commandList 
		void Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso) override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return param_.get(); }


	public:

		/// @brief デバッグ用パラメータ
		void DebugParameter() override;

		/// @brief デバッグ用ピッキング
		/// @param point 
		/// @param pickList 
		void DebugPicking(const Vector2& point, std::vector<std::pair<float, DebugData::DebugGuizmoData*>>& pickList) override;

		/// @brief Guizmo操作
		/// @param viewMatrix 
		/// @param projMatrix 
		void DebugGuizmo(const Matrix4x4& viewMatrix, const Matrix4x4& projMatrix) override;


	private:

		// マテリアルリソース
		std::unique_ptr<ConstantBufferResource<Sprite::MaterialDataForGPU>> materialResource_ = nullptr;

		// 座標変換リソース
		std::unique_ptr<ConstantBufferResource<Sprite::TransformationDataForGPU>> transformationResource_ = nullptr;


		/// @brief パラメータ
		std::unique_ptr<Render2D::Sprite::Param> param_ = nullptr;

		// テクスチャストア
		TextureStore* textureStore_ = nullptr;
	};
}