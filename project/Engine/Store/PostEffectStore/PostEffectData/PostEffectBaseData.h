#pragma once
#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <cstdint>
#include "Data/PostEffectData/PostEffectData.h"
#include "DataForGPU/PostEffectDataForGPU/PostEffectDataForGPU.h"
#include "Resource/ConstantBufferResource/ConstantBufferResource.h"
#include "Resource/RWTexture2DBufferResource/RWTexture2DBufferResource.h"
#include "Resource/OffscreenResource/OffscreenResource.h"
#include "Handle/Handle.h"

namespace Engine
{
    class DepthResource;
	class Camera3DStore;
	class BasePSOPostEffect;
	class BaseComputePSO;
	class PostEffectParameter;
	class MotionVectorTextureResource;
	class PSOFullscreen;

	/// @brief 各ポストエフェクトが必要とする入力データの種類
	/// @details 新しい入力が必要になった場合はここにフラグを追加する
	enum class PostEffectInput : uint32_t
	{
		None = 0,
		DepthTexture = 1u << 0,
		ProjectionInverse = 1u << 1,
		Camera3DStore = 1u << 2,
	};

	inline PostEffectInput operator|(PostEffectInput lhs, PostEffectInput rhs)
	{
		return static_cast<PostEffectInput>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
	}

	/// @brief ポストエフェクト描画時に渡す入力データ
	/// @details 将来的に必要な入力が増えた場合はこの構造体へ項目を追加する
	struct PostEffectRenderContext
	{
		ID3D12GraphicsCommandList* commandList = nullptr;
		OffscreenResource* offscreenPixelShaderResource = nullptr;
		OffscreenResource* offscreenRenderTargetResource = nullptr;
		RWTexture2DBufferResource* firstOutputResource = nullptr;
		RWTexture2DBufferResource* secondOutputResource = nullptr;
		DepthResource* depthResource = nullptr;
		Camera3DStore* camera3DStore = nullptr;
		MotionVectorTextureResource* motionVectorTextureResource = nullptr;
		PSOFullscreen* psoFullscreen = nullptr;
	};

	class PostEffectBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param type 
		/// @param hPostEffect 
		PostEffectBaseData(const std::string name, PostEffect::Type type, PostEffectHandle hPostEffect, PostEffectParameter* parameter)
			: name_(name), type_(type), hPostEffect_(hPostEffect), parameter_(parameter) {
		}

		/// @brief 仮想デストラクタ
		virtual ~PostEffectBaseData() = default;

		/// @brief リセット
		virtual void Reset() = 0;

		/// @brief リサイズする
		/// @param width 
		/// @param height 
		virtual void Resize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, int32_t width, int32_t height) {}

		/// @brief 種類を取得する
		/// @return 
		PostEffect::Type GetType()const { return type_; }

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief ハンドルを取得する
		/// @return 
		PostEffectHandle GetHandle()const { return hPostEffect_; }

		/// @brief パラメータを取得する
		/// @return 
		virtual void* GetParam() = 0;

		/// @brief コマンドリストに登録する
		/// @param context
		virtual void Register(const PostEffectRenderContext& context) = 0;

		/// @brief 必要な入力データの種類を取得する
		/// @return RequiredInputフラグ
		/// @details 必要なデータが増えたら派生クラス側でオーバーライドして返す
		virtual PostEffectInput GetRequiredInputs() const { return PostEffectInput::None; }

		/// @brief 補助PSOを設定する
		/// @param key
		/// @param pso
        /// @details keyを使って派生クラス側で自由にPSOを引き回せるようにする
		void SetExtraPSO(const std::string& key, BasePSOPostEffect* pso)
		{
			extraPSOTable_[key] = pso;
		}

		/// @brief 補助PSOを取得する
		/// @param key
		/// @return
		BasePSOPostEffect* GetExtraPSO(const std::string& key) const
		{
			auto it = extraPSOTable_.find(key);
			if (it == extraPSOTable_.end())
			{
				return nullptr;
			}
			return it->second;
		}

		/// @brief デバッグ用パラメータ
		virtual void DebugParameter() = 0;


	private:

		/// @brief 種類
		PostEffect::Type type_;

		// ハンドル
		PostEffectHandle hPostEffect_ = 0;


	protected:

		// 名前
		std::string name_{};

		/// @brief グループ
		std::string group_{};

		/// @brief PSO
		BasePSOPostEffect* pso_ = nullptr;


	protected:

		/// @brief パラメータ
		PostEffectParameter* parameter_ = nullptr;

		/// @brief 補助PSOテーブル
		std::unordered_map<std::string, BasePSOPostEffect*> extraPSOTable_;
	};
}