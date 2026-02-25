#pragma once
#include <memory>
#include <vector>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "PostEffectData/PostEffectBaseData.h"
#include "Parameter/PostEffectParameter/PostEffectParameter.h"

#include "PSO/PSOPostEffect/BasePSOPostEffect/PSORadialBlur/PSORadialBlur.h"

namespace Engine
{
	class ShaderCompiler;
	class Log;
	class OffscreenResource;

	class PostEffectStore
	{
	public:

		/// @brief コンストラクタ
		PostEffectStore();

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, IDxcBlob* vertexShaderBlob, Log* log);

		/// @brief 読み込み
		/// @param name 
		/// @param type 
		/// @param device 
		/// @param log 
		PostEffectHandle Load(const std::string& name, PostEffect::Type type, ID3D12Device* device, Log* log);

		/// @brief ポストエフェクトを描画する
		/// @param hPostEffect 
		/// @param commandList 
		/// @param offscreenResource 
		void DrawPostEffect(PostEffectHandle hPostEffect, ID3D12GraphicsCommandList* commandList, OffscreenResource* offscreenResource);

		/// @brief パメータを取得する
		/// @tparam T 
		/// @param hPostEffect 
		/// @return 
		template<typename T>
		T* GetParam(PostEffectHandle hPostEffect)
		{
			return static_cast<T*>(dataTable_[hPostEffect]->GetParam());
		}

		/// @brief デバッグ用パラメータ
		void DebugParameter();


	private:

		// データテーブル
		std::vector<std::unique_ptr<PostEffectBaseData>> dataTable_;

		/// @brief パラメータ
		std::unique_ptr<PostEffectParameter> parameter_ = nullptr;


	private:

		// ラジアルブラーPSO
		std::unique_ptr<PSORadialBlur> psoRadialBlur_ = nullptr;
	};
}