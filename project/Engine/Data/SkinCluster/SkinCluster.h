#pragma once
#include "Data/SkeletonData/SkeletonData.h"
#include "Data/ModelData/ModelData.h"

namespace Engine
{
	class Log;
	class DX12Heap;

	class SkinCluster
	{
	public:

		/// @brief 初期化
		/// @param heap 
		/// @param device 
		/// @param meshData 
		/// @param skeleton 
		/// @param log 
		void Initialize(DX12Heap* heap, ID3D12Device* device, const MeshData& meshData,const MeshBoneData& meshBoneData, const Skeleton& skeleton, Log* log);

		/// @brief 更新処理
		/// @param skeleton 
		void Update(const Skeleton& skeleton);


	private:

		std::vector<Matrix4x4> inverseBindPoseMatrices;

		Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
		D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
		std::span<VertexInfluence> mappedInfluence;

		Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;
		std::span<WellForGPU> mappedPalette;
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;


	private:

		// インフルエンスの最大数
		static constexpr uint32_t kNumMaxInfluence = 4;
	};
}