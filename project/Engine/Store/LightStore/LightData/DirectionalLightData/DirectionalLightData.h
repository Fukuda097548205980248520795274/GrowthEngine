#pragma once
#include "../BaseLightData.h"
#include "Data/LightData/LightData.h"
#include "Resource/ShadowMapTextureResource/ShadowMapTextureResource.h"

namespace Engine
{
	class DX12Heap;
	class Log;

	class DirectionalLightData : public BaseLightData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param handle 
		DirectionalLightData(const std::string& name, LightHandle handle);

		/// @brief 初期化
		/// @param heap 
		/// @param device 
		/// @param log 
		void Initialize(DX12Heap* heap, ID3D12Device* device, Log* log);

		/// @brief 
		/// @return 
		void* GetParam() override { return param_.get(); }

		/// @brief 種別名を取得する
		/// @return 
		const char* GetTypeName() const override { return "Directional"; }


	private:

		// パラメータ
		std::unique_ptr<DirectionalLightParam> param_ = nullptr;

		// シャドウマップテクスチャリソース
		std::unique_ptr<ShadowMapTextureResource> shadowMapTextureResource_ = nullptr;
	};
}