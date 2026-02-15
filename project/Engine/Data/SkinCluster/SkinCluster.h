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
		void Initialize(DX12Heap* heap, ID3D12Device* device, const MeshData& meshData, const MeshBoneData& meshBoneData, Skeleton& skeleton, Log* log);

		/// @brief 更新処理
		/// @param skeleton 
		void Update(const Skeleton& skeleton);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param wellRootParameterIndex 
		/// @param influenceRootParameterIndex 
		void Register(ID3D12GraphicsCommandList* commandList, UINT wellRootParameterIndex, UINT influenceRootParameterIndex);


	private:

		std::vector<Matrix4x4> inverseBindPoseMatrices_;

		Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource_;
		std::span<VertexInfluence> mappedInfluence_;
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> influenceSrvHandle_;

		Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource_;
		std::span<WellForGPU> mappedPalette_;
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle_;


	private:

		// インフルエンスの最大数
		static constexpr uint32_t kNumMaxInfluence = 4;
	};
}