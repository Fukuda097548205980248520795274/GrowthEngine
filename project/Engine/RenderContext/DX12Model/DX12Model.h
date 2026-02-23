#pragma once
#include <memory>
#include "PSO/PSOModel/PSOPrimitive/PSOPrimitive.h"
#include "PSO/PSOModel/PSOSprite/PSOSprite.h"
#include "Store/PrimitiveStore/PrimitiveStore.h"
#include "Store/SpriteStore/SpriteStore.h"
#include "Data/PrimitiveData/PrimitiveData.h"

#include "Resource/RWStructuredBufferResource/RWStructuredBufferResource.h"
#include "DataForGPU/ParticleDataForGPU/ParticleDataForGPU.h"
#include "PSO/ComputePSO/ComputePSOParticleInitialize/ComputePSOParticleInitialize.h"
#include "PSO/ComputePSO/ComputePSOEmitParticle/ComputePSOEmitParticle.h"
#include "PSO/ComputePSO/ComputePSOUpdateParticle/ComputePSOUpdateParticle.h"
#include "PSO/PSOModel/PSOParticle/PSOParticle.h"

namespace Engine
{
	class ShaderCompiler;
	class Log;
	class BasePSOShadowMap;
	class ModelStore;
	class TextureStore;
	class Camera3D;

	class DX12Model
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param shaderCompiler 
		/// @param heap 
		/// @param modelStore 
		/// @param textureStore 
		/// @param animationStore 
		/// @param skeletonStore 
		/// @param lightStore 
		/// @param log 
		void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ShaderCompiler* shaderCompiler, DX12Heap* heap,
			ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, SkeletonStore* skeletonStore, LightStore* lightStore, Log* log);

		/// @brief 更新処理
		/// @param viewProjection 
		void Update(ID3D12GraphicsCommandList* commandList);

		/// @brief シャドウアップ用描画処理
		/// @param commandList 
		void ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso);

		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param handle 
		/// @return 
		template<typename T>
		T* GetParam(PrimitiveHandle handle) { return primitiveStore_->GetParam<T>(handle); }

		/// @brief プリミティブを読み込む
		/// @param device 
		/// @param commandList 
		/// @param hModel 
		/// @param hAnimation 
		/// @param hSkeleton 
		/// @param name 
		/// @param type 
		/// @param log 
		/// @return 
		PrimitiveHandle LoadPrimitive(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,ModelHandle hModel, AnimationHandle hAnimation,SkeletonHandle hSkeleton,
			const std::string& name, Primitive::Type type, Log* log)
		{
			return primitiveStore_->Load(device, commandList, hModel, hAnimation, hSkeleton, name, type, log);
		}

		/// @brief スプライト読み込み
		/// @param textureStore 
		/// @param device 
		/// @param hTexture 
		/// @param name 
		/// @param log 
		/// @return 
		SpriteHandle LoadSprite(TextureStore* textureStore, ID3D12Device* device, TextureHandle hTexture, const std::string& name, Log* log)
		{
			return spriteStore_->Load(name, hTexture, textureStore, device, log);
		}


		/// @brief プリミティブの描画処理
		/// @param commandList 
		/// @param handle 
		void DrawPrimitive(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, PrimitiveHandle handle) 
		{
			primitiveStore_->Register(viewProjection, commandList, handle, psoPrimitive_.get());
		}

		/// @brief スプライトの描画処理
		/// @param hSprite 
		/// @param viewProjection 
		/// @param commandList 
		void DrawSprite(SpriteHandle hSprite, const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList)
		{
			spriteStore_->Register(hSprite, viewProjection, commandList, psoSprite_.get());
		}


		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

	private:

		// プリミティブ頂点シェーダ
		ComPtr<IDxcBlob> primitiveVS_ = nullptr;

		// プリミティブピクセルシェーダ
		ComPtr<IDxcBlob> primitivePS_ = nullptr;


		// スプライト頂点シェーダ
		ComPtr<IDxcBlob> spriteVS_ = nullptr;

		// スプライトピクセルシェーダ
		ComPtr<IDxcBlob> spritePS_ = nullptr;


	private:

		// プリミティブPSO
		std::unique_ptr<PSOPrimitive> psoPrimitive_ = nullptr;

		// スプライトPSO
		std::unique_ptr<PSOSprite> psoSprite_ = nullptr;


	private:

		/// @brief プリミティブストア
		std::unique_ptr<PrimitiveStore> primitiveStore_ = nullptr;

		/// @brief スプライトストア
		std::unique_ptr<SpriteStore> spriteStore_ = nullptr;


	private:

		/// @brief パーティクルリソース
		std::unique_ptr<RWStructuredBufferResource<ParticleCS>> particleResource_ = nullptr;

		/// @brief 初期化用パーティクルPSO
		std::unique_ptr<ComputePSOParticleInitialize> psoParticleInitialize_ = nullptr;

		/// @brief 放出用パーティクルPSO
		std::unique_ptr<ComputePSOEmitParticle> psoEmitParticle_ = nullptr;

		/// @brief 更新用パーティクルPSO
		std::unique_ptr<ComputePSOUpdateParticle> psoUpdateParticle_ = nullptr;

		/// @brief パーティクル初期化フラグ
		bool isParticleInitialize = false;

		/// @brief パーティクル初期化
		void ParticleInitialize(ID3D12GraphicsCommandList* commandList);


	private:

		// パーティクルインデックスリソース
		std::unique_ptr<IndexBufferResource> particleIndexResource_ = nullptr;

		/// @brief パーティクル頂点リソース
		std::unique_ptr<VertexBufferResource<SpriteVertexData>> particleVertexResource_ = nullptr;

		/// @brief エミッターリソース
		std::unique_ptr<ConstantBufferResource<EmitterSphere>> emitterResource_ = nullptr;

		/// @brief フレームリソース
		std::unique_ptr<ConstantBufferResource<PerFrameDataForGPU>> frameResource_ = nullptr;

		/// @brief カウンターリソース
		std::unique_ptr<RWStructuredBufferResource<int32_t>> counterResource_ = nullptr;

		/// @brief フリーリストリソース
		std::unique_ptr<RWStructuredBufferResource<uint32_t>> freeListResource_ = nullptr;


	private:

		// パーティクル頂点シェーダ
		ComPtr<IDxcBlob> particleVS_ = nullptr;

		// パーティクルピクセルシェーダ
		ComPtr<IDxcBlob> particlePS_ = nullptr;

		// パーティクルPSO
		std::unique_ptr<PSOParticle> psoParticle_ = nullptr;


		// ビューリソース
		std::unique_ptr<ConstantBufferResource<PreViewDataForGPU>> viewResource_ = nullptr;


	public:

		/// @brief パーティクルを描画する
		/// @param commandList 
		/// @param camera3d 
		void DrawParticle(ID3D12GraphicsCommandList* commandList, const Camera3D& camera3d, TextureStore* textureStore);
	};
}