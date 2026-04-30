#pragma once
#pragma once
#include <memory>
#include <vector>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "PostEffectData/PostEffectBaseData.h"
#include "Parameter/PostEffectParameter/PostEffectParameter.h"

#include "PSO/PSOPostEffect/PSOCopyImageAdd/PSOCopyImageAdd.h"
#include "PSO/PSOPostEffect/PSORadialBlur/PSORadialBlur.h"
#include "PSO/PSOPostEffect/PSOVignetting/PSOVignetting.h"
#include "PSO/PSOPostEffect/PSOGrayscale/PSOGrayscale.h"
#include "PSO/PSOPostEffect/PSOSmoothing/PSOSmoothing.h"
#include "PSO/PSOPostEffect/PSOGaussianFilter/PSOGaussianFilter.h"
#include "PSO/PSOPostEffect/PSOLuminanceBasedOutline/PSOLuminanceBasedOutline.h"
#include "PSO/PSOPostEffect/PSODepthBasedOutline/PSODepthBasedOutline.h"
#include "PSO/PSOPostEffect/PSODissolve/PSODissolve.h"
#include "PSO/PSOPostEffect/PSOWhiteNoise/PSOWhiteNoise.h"
#include "PSO/PSOPostEffect/PSODOF/PSODOF.h"

#include "PSO/ComputePSO/ComputePSOGaussianFilter/ComputePSOGaussianFilter.h"
#include "PSO/ComputePSO/ComputePSODualBlurDownsample/ComputePSODualBlurDownsample.h"
#include "PSO/ComputePSO/ComputePSODualBlurUpsample/ComputePSODualBlurUpsample.h"
#include "PSO/ComputePSO/ComputePSOHighLuminanceExtraction/ComputePSOHighLuminanceExtraction.h"
#include "PSO/ComputePSO/ComputePSOTAA/ComputePSOTAA.h"
#include "PSO/ComputePSO/ComputePSOMotionBlur/ComputePSOMotionBlur.h"
#include "PSO/ComputePSO/ComputePSOVelocityDilation/ComputePSOVelocityDilation.h"
#include "PSO/ComputePSO/ComputePSOAfterImage/ComputePSOAfterImage.h"

#include "PSO/PSOMotionVector/PSOMotionVectorRender/PSOMotionVectorRender.h"
#include "PSO/PSOMotionVector/PSOMotionVectorPrefab/PSOMotionVectorPrefab.h"

#include "Resource/MotionVectorTextureResource/MotionVectorTextureResource.h"

namespace Engine
{
	class ShaderCompiler;
	class Log;
	class OffscreenResource;
	class DepthResource;
	class TextureStore;
	class Camera3DStore;
	class DX12Buffering;
	class DX12Heap;
	class DX12Render;
	class DX12Prefab;

	class PostEffectStore
	{
	public:

		/// @brief コンストラクタ
		PostEffectStore();

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, IDxcBlob* vertexShaderBlob, 
			DX12Heap* heap,TextureStore* textureStore,int32_t width , int32_t height, Log* log);

		/// @brief リサイズ
		/// @param device 
		/// @param commandList 
		/// @param width 
		/// @param height 
		void Resize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, int32_t width, int32_t height);

		/// @brief 読み込み
		/// @param name 
		/// @param type 
		/// @param device 
		/// @param log 
		PostEffectHandle Load(const std::string& name, PostEffect::Type type,
			ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Buffering* buffering, Log* log);

		/// @brief シーン前のリセット処理
		void PerSceneReset();

        /// @brief 描画処理をコマンドリストに登録する
		/// @param hPostEffect
		/// @param context
		void DrawPostEffect(PostEffectHandle hPostEffect, const PostEffectRenderContext& context) { dataTable_[hPostEffect]->Register(context); }

        /// @brief 描画処理をコマンドリストに登録する
		/// @param name
		/// @param context
		void DrawPostEffect(const std::string& name, const PostEffectRenderContext& context) { dataTable_[nameTable_[name]]->Register(context); }

		/// @brief モーションベクトルの描画処理をコマンドリストに登録する
		/// @param commandList 
		/// @param dsvHandle 
		void DrawMotionVector(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, DX12Render* render, DX12Prefab* prefab);

		/// @brief TAAの描画処理をコマンドリストに登録する
		/// @param context 
		void DrawTAA(const PostEffectRenderContext& context);

		/// @brief モーションブラーの描画処理をコマンドリストに登録する
		/// @param context 
		void DrawMotionBlur(const PostEffectRenderContext& context);

		/// @brief 残像の描画処理をコマンドリストに登録する
		/// @param context 
		void DrawAfterImage(const PostEffectRenderContext& context);



		/// @brief 指定入力が必要かどうか
		/// @param hPostEffect
		/// @param input
		/// @return
		bool IsRequiredInput(PostEffectHandle hPostEffect, PostEffectInput input) const
		{
			return (static_cast<uint32_t>(dataTable_[hPostEffect]->GetRequiredInputs()) & static_cast<uint32_t>(input)) != 0;
		}

		/// @brief 指定のポストエフェクトがBloomかどうか
		/// @param hPostEffect 
		/// @return 
		bool IsBloom(PostEffectHandle hPostEffect) const { return dataTable_[hPostEffect]->GetType() == PostEffect::Type::Bloom; }

		/// @brief 指定のポストエフェクトがBloomかどうか
		/// @param name 
		/// @return 
		bool IsBloom(const std::string& name) { return dataTable_[nameTable_[name]]->GetType() == PostEffect::Type::Bloom; }

		/// @brief TAAを読み込んでいるかどうか
		/// @return 
		bool IsLoadTAA() { return isLoadTAA_; }

		/// @brief モーションブラーを読み込んでいるかどうか
		/// @return 
		static bool IsLoadMotionBlur() { return isLoadMotionBlur_; }

		/// @brief 残像を読み込んでいるかどうか
		/// @return 
		static bool IsLoadAfterImage() { return isLoadAfterImage_; }

		/// @brief 指定入力が必要かどうか
		/// @param name
		/// @param input
		/// @return
		bool IsRequiredInput(const std::string& name, PostEffectInput input) const
		{
			return (static_cast<uint32_t>(dataTable_[nameTable_.at(name)]->GetRequiredInputs()) & static_cast<uint32_t>(input)) != 0;
		}

		/// @brief DX12Offscreen側のRTV/DSV設定を使うかどうか
		/// @param hPostEffect
		/// @return
		bool IsUseOffscreenRenderTarget(PostEffectHandle hPostEffect) const { return dataTable_[hPostEffect]->GetType() != PostEffect::Type::DOF; }

		/// @brief モーションベクトルが有効かどうか
		/// @return 
		static bool IsEnableMotionVector() { return enableMotionVector_; }


		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param hPostEffect 
		/// @return 
		template<typename T>
		T* GetParam(PostEffectHandle hPostEffect)
		{
			return static_cast<T*>(dataTable_[hPostEffect]->GetParam());
		}

		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetParam(const std::string& name)
		{
			return static_cast<T*>(dataTable_[nameTable_[name]]->GetParam());
		}

		/// @brief デバッグ用パラメータ
		void DebugParameter();


	private:

		// データテーブル
		std::vector<std::unique_ptr<PostEffectBaseData>> dataTable_;

		// 名前テーブル
		std::unordered_map<std::string, PostEffectHandle> nameTable_;

		/// @brief パラメータ
		std::unique_ptr<PostEffectParameter> parameter_ = nullptr;


	private:

		/// @brief コピー加算PSO
		std::unique_ptr<PSOCopyImageAdd> psoCopyImageAdd_ = nullptr;

		/// @brief グレースケールPSO
		std::unique_ptr<PSOGrayscale> psoGrayscale_ = nullptr;

		/// @brief ヴィネッティングPSO
		std::unique_ptr<PSOVignetting> psoVignetting_ = nullptr;

		/// @brief 平滑化PSO
		std::unique_ptr<PSOSmoothing> psoSmoothing_ = nullptr;

		/// @brief ガウシアンフィルタPSO
		std::unique_ptr<PSOGaussianFilter> psoGaussianFilter_ = nullptr;

		/// @brief 輝度ベース輪郭抽出PSO
		std::unique_ptr<PSOLuminanceBasedOutline> psoLuminanceBasedOutline_ = nullptr;

		/// @brief 深度ベース輪郭抽出PSO
		std::unique_ptr<PSODepthBasedOutline> psoDepthBasedOutline_ = nullptr;

		/// @brief ラジアルブラーPSO
		std::unique_ptr<PSORadialBlur> psoRadialBlur_ = nullptr;

		/// @brief ディゾルブPSO
		std::unique_ptr<PSODissolve> psoDissolve_ = nullptr;

		/// @brief ホワイトノイズPSO
		std::unique_ptr<PSOWhiteNoise> psoWhiteNoise_ = nullptr;

		/// @brief 被写界深度PSO
		std::unique_ptr<PSODOF> psoDOF_ = nullptr;


	private:

		/// @brief デュアルブラー縮小PSO
		std::unique_ptr<ComputePSODualBlurDownsample> computePSODualBlurDownsample_ = nullptr;

		/// @brief デュアルブラー拡大PSO
		std::unique_ptr<ComputePSODualBlurUpsample> computePSODualBlurUpsample_ = nullptr;

		/// @brief CSガウシアンフィルタPSO
		std::unique_ptr<ComputePSOGaussianFilter> computePSOGaussianFilter_ = nullptr;

		/// @brief CS高輝度抽出PSO
		std::unique_ptr<ComputePSOHighLuminanceExtraction> computePSOHighLuminanceExtraction_ = nullptr;

		/// @brief CS TAA PSO
		std::unique_ptr<ComputePSOTAA> computePSOTAA_ = nullptr;

		/// @brief CSモーションブラーPSO
		std::unique_ptr<ComputePSOMotionBlur> computePSOMotionBlur_ = nullptr;

		/// @brief CS速度膨張PSO
		std::unique_ptr<ComputePSOVelocityDilation> computePSOVelocityDilation_ = nullptr;

		/// @brief CS残像PSO
		std::unique_ptr<ComputePSOAfterImage> computePSOAfterImage_ = nullptr;


	private:

		/// @brief モーションベクトルのピクセルシェーダデータ
		Microsoft::WRL::ComPtr<IDxcBlob> motionVectorPixelShaderBlob_ = nullptr;

		/// @brief モーションベクトル描画PSO
		std::unique_ptr<PSOMotionVectorRender> psoMotionVectorRender_ = nullptr;

		/// @brief モーションベクトルPrefab描画PSO
		std::unique_ptr<PSOMotionVectorPrefab> psoMotionVectorPrefab_ = nullptr;


		/// @brief モーションベクトルテクスチャリソース
		std::unique_ptr<MotionVectorTextureResource> motionVectorTextureResource_ = nullptr;

		/// @brief モーションベクトルを有効にするかどうか
		static bool enableMotionVector_;


	private:

		/// @brief TAAのハンドル
		PostEffectHandle hTAA_ = 0;

		/// @brief TAAを読み込んでいるかどうか
		bool isLoadTAA_;


	private:

		/// @brief モーションブラーのハンドル
		PostEffectHandle hMotionBlur_ = 0;

		/// @brief モーションブラーを読み込んでいるかどうか
		static bool isLoadMotionBlur_;


	private:

		/// @brief 残像のハンドル
		PostEffectHandle hAfterImage_ = 0;

		/// @brief 残像を読み込んでいるかどうか
		static bool isLoadAfterImage_;


	private:

		/// @brief DX12ヒープ
		DX12Heap* heap_ = nullptr;

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;
	};
}