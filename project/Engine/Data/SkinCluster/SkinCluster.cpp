#include "SkinCluster.h"
#include "RenderContext/DX12Heap/DX12Heap.h"
#include "Func/ResourceFunc/ResourceFunc.h"
#include "Log/Log.h"
#include <algorithm>
#include <cassert>

/// @brief 初期化
/// @param heap 
/// @param device 
/// @param meshData 
/// @param skeleton 
void Engine::SkinCluster::Initialize(DX12Heap* heap, ID3D12Device* device, 
	const MeshData& meshData, const MeshBoneData& meshBoneData, const Skeleton& skeleton, Log* log)
{
	/*------------------------------
		Palette用のリソースを確保
	------------------------------*/

	paletteResource = CreateBufferResource(device, sizeof(WellForGPU) * skeleton.joints.size(), log);
	WellForGPU* mappedPaletteData = nullptr;
	paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPaletteData));

	// spanを使ってアクセスするようにする
	mappedPalette = { mappedPaletteData, skeleton.joints.size() };
	paletteSrvHandle.first = heap->GetSrvCPUDescriptorHandle();
	paletteSrvHandle.second = heap->GetSrvGPUDescriptorHandle();


	/*--------------------------
		Palette用のSRVを作成
	--------------------------*/

	D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{};
	paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	paletteSrvDesc.Buffer.FirstElement = 0;
	paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	paletteSrvDesc.Buffer.NumElements = UINT(skeleton.joints.size());
	paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);

	device->CreateShaderResourceView(paletteResource.Get(), &paletteSrvDesc, paletteSrvHandle.first);


	/*-------------------------------
		Influence用のリソースを確保
	-------------------------------*/

	influenceResource = CreateBufferResource(device, sizeof(VertexInfluence) * meshData.vertices.size(), log);
	VertexInfluence* mappedInfluenceData = nullptr;
	influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluenceData));

	// 0埋め
	std::memset(mappedInfluenceData, 0, sizeof(VertexInfluence) * meshData.vertices.size());
	mappedInfluence = { mappedInfluenceData, meshData.vertices.size() };


	/*---------------------------
		Influence用のVBVを設定
	---------------------------*/

	influenceBufferView.BufferLocation = influenceResource->GetGPUVirtualAddress();
	influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * meshData.vertices.size());
	influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

	inverseBindPoseMatrices.resize(skeleton.joints.size());
	std::generate(inverseBindPoseMatrices.begin(), inverseBindPoseMatrices.end(), MakeIdentityMatrix4x4);


	/*----------------------------------------------
		SkinClusterの情報を解析してInfluenceを格納する
	----------------------------------------------*/

	for (const auto& joint : skeleton.joints)
	{
		// 該当のIndexのInverseBindPoseMatrixを代入する
		inverseBindPoseMatrices[joint.index] = joint.offsetMatrix;

		for (const auto& vertexWeight : meshBoneData.jointWeights[joint.index].vertexWeights)
		{
			// 該当のvertexIndexのInfluence情報を参照しておく
			auto& currentInfluence = mappedInfluence[vertexWeight.vertexIndex];

			// 空いているところ (Weight == 0.0f) に入れる
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index)
			{
				if (currentInfluence.weights[index] == 0.0f)
				{
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = joint.index;
					break;
				}
			}
		}
	}
}

/// @brief 更新処理
/// @param skeleton 
void Engine::SkinCluster::Update(const Skeleton& skeleton)
{
	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex)
	{
		assert(jointIndex < inverseBindPoseMatrices.size());

		mappedPalette[jointIndex].skeletonSpaceMatrix =
			inverseBindPoseMatrices[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;

		mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix = (mappedPalette[jointIndex].skeletonSpaceMatrix).Inverse().Transpose();
	}
}