#pragma once
#include <string>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "Handle/Handle.h"

#include <memory>
#include <deque>

#include "PSO/EnumBlendMode/EnumBlendMode.h"

#include "DataForGPU/VertexDataForGPU/VertexDataForGPU.h"
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

#include "Resource/VertexBufferResource/VertexBufferResource.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"
#include "Resource/ConstantBufferResource/ConstantBufferResource.h"

namespace Engine
{
	class PSOTrail;
	class TextureStore;
	class Camera3DStore;

	class TrailData
	{
	public:

		/// @brief トレイルの履歴
		struct TrailHistory
		{
			/// @brief 基点の位置
			Vector4 basePosition;

			/// @brief 終点の位置
			Vector4 tipPosition;

			/// @brief 色
			Vector4 color;
		};

		/// @brief パラメータ
		struct Param
		{
			/// @brief 更新するかどうか
			bool isUpdate_ = true;

			/// @brief 基点の位置
			Vector3 basePosition;

			/// @brief 終点の位置
			Vector3 tipPosition;

			/// @brief 色
			Vector4 color;

			/// @brief ブレンドモード
			BlendMode blendMode;

			/// @brief イージング
			float easing_ = 1.0f;
		};


	public:

		/// @brief コンストラクタ
		/// @param handle 
		/// @param name 
		/// @param maxLifeTime 
		TrailData(TrailHandle hTrail, const std::string& name, float maxLifeTime , TextureHandle hTexture)
			: hTrail_(hTrail), name_(name), maxHistoryCount_(static_cast<int32_t>(maxLifeTime * 60.0f)), hTexture_(hTexture) {
		}

		/// @brief 初期化
		/// @param device 
		/// @param pso 
		/// @param textureStore 
		/// @param camera3DStore 
		void Initialize(ID3D12Device* device, PSOTrail* pso, TextureStore* textureStore, Camera3DStore* camera3DStore);

		/// @brief 更新処理
		void Update();

		/// @brief コマンドリストに登録する
		/// @param commandList 
		void Register(ID3D12GraphicsCommandList* commandList);

		/// @brief トレイルの履歴を消す
		void HistoryClear() { histories_.clear(); }

		/// @brief ハンドルを取得する
		/// @return 
		TrailHandle GetHandle() const { return hTrail_; }

		/// @brief 名前を取得する
		/// @return 
		std::string GetName() const { return name_; }

		/// @brief パラメータを取得する
		/// @return 
		Param* GetParam() { return param_.get(); }


	private:

		/// @brief ハンドル
		TrailHandle hTrail_ = 0;

		/// @brief 名前
		std::string name_{};

		/// @brief 最大記録数
		int32_t maxHistoryCount_ = 0;


		/// @brief 頂点の最大数
		int32_t maxVertexCount_ = 0;

		/// @brief インデックスの最大数
		int32_t maxIndexCount_ = 0;


	private:

		/// @brief パラメータ
		std::unique_ptr<Param> param_ = nullptr;

		/// @brief トレイルの履歴
		std::deque<TrailHistory> histories_;


	private:

		/// @brief 頂点リソース
		std::unique_ptr<VertexBufferResource<TrailVertexDataForGPU>> vertexBufferResource_ = nullptr;

		/// @brief インデックスリソース
		std::unique_ptr<IndexBufferResource> indexBufferResource_ = nullptr;

		/// @brief 座標変換用リソース
		std::unique_ptr<ConstantBufferResource<Matrix4x4>> transformationResource_ = nullptr;


	private:

		/// @brief テクスチャハンドル
		TextureHandle hTexture_ = 0;

		/// @brief テクスチャのファイルパスを取得する
		std::string textureFilePath_{};


	private:
		
		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;

		/// @brief カメラストア
		Camera3DStore* camera3DStore_ = nullptr;

		/// @brief トレイル用PSO
		PSOTrail* pso_ = nullptr;
	};
}