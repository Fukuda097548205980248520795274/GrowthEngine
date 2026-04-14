#pragma once
#include <cstdint>

namespace Engine
{
	namespace PrimitiveDataForGPU
	{
		// GPUに送るUV球の分割数データ
		struct UVSphereDivisionDataForGPU
		{
			// スライス数
			int32_t slices;

			// リング数
			int32_t rings;

			float padding[2]; // 16バイトアラインメントのためのパディング
		};
	}
}