#pragma once
#include "../Render2DBaseData.h"

namespace Engine
{
	class FontStore;

	class Render2DTextData : public Engine::Render2DBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param hRender2D 
		/// @param name 
		/// @param parameter 
		Render2DTextData(Render2DHandle hRender2D, std::string name, Render2DParameter* parameter, TextHandle hText)
			: hText_(hText), Render2DBaseData(hRender2D, name, parameter) {
			type_ = Render2D::Type::Text;
		}

		/// @brief 初期化
		/// @param vertexResource 
		/// @param indexResource 
		/// @param device 
		void Initialize(VertexBufferResource<SpriteVertexData>* vertexResource, IndexBufferResource* indexResource, FontStore* fontStore, ID3D12Device* device, Log* log);

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
		void DebugPicking(const Vector2& point, std::vector<std::pair<float, bool*>>& pickList) override;

		/// @brief Guizmo操作
		/// @param viewMatrix 
		/// @param projMatrix 
		void DebugGuizmo(const Matrix4x4& viewMatrix, const Matrix4x4& projMatrix) override;


	private:

		/// @brief パラメータ
		std::unique_ptr<Render2D::Text::Param> param_ = nullptr;

		// テキストハンドル
		TextHandle hText_ = 0;


	private:

		// マテリアルリソース
		std::vector<std::unique_ptr<ConstantBufferResource<Vector4>>> materialResource_;

		// 座標変換リソース
		std::vector<std::unique_ptr<ConstantBufferResource<Sprite::TransformationDataForGPU>>> transformationResource_;


	private:

		/// @brief フォントストア
		FontStore* fontStore_ = nullptr;
	};
}