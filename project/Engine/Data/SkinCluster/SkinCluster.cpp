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
	const MeshData& meshData, const MeshBoneData& meshBoneData, Skeleton& skeleton, Log* log)
{
	/*------------------------------
		Palette用のリソースを確保
	------------------------------*/

	paletteResource_ = CreateBufferResource(device, sizeof(WellForGPU) * skeleton.joints.size(), log);
	WellForGPU* mappedPaletteData = nullptr;
	paletteResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedPaletteData));

	// spanを使ってアクセスするようにする
	mappedPalette_ = { mappedPaletteData, skeleton.joints.size() };
	paletteSrvHandle_.first = heap->GetSrvCPUDescriptorHandle();
	paletteSrvHandle_.second = heap->GetSrvGPUDescriptorHandle();

	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{};
	paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	paletteSrvDesc.Buffer.FirstElement = 0;
	paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	paletteSrvDesc.Buffer.NumElements = UINT(skeleton.joints.size());
	paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);

	// ビューの生成
	device->CreateShaderResourceView(paletteResource_.Get(), &paletteSrvDesc, paletteSrvHandle_.first);


	/*-------------------------------
		Influence用のリソースを確保
	-------------------------------*/

	influenceResource_ = CreateBufferResource(device, sizeof(VertexInfluence) * meshData.vertices.size(), log);
	VertexInfluence* mappedInfluenceData = nullptr;
	influenceResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluenceData));

	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC influenceSrvDesc{};
	influenceSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	influenceSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	influenceSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	influenceSrvDesc.Buffer.FirstElement = 0;
	influenceSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	influenceSrvDesc.Buffer.NumElements = UINT(meshData.vertices.size());
	influenceSrvDesc.Buffer.StructureByteStride = sizeof(VertexInfluence);

	// 0埋め
	std::memset(mappedInfluenceData, 0, sizeof(VertexInfluence) * meshData.vertices.size());
	mappedInfluence_ = { mappedInfluenceData, meshData.vertices.size() };

	// SRVハンドルを取得する
	influenceSrvHandle_.first = heap->GetSrvCPUDescriptorHandle();
	influenceSrvHandle_.second = heap->GetSrvGPUDescriptorHandle();

	// ビューの生成
	device->CreateShaderResourceView(influenceResource_.Get(), &influenceSrvDesc, influenceSrvHandle_.first);


	/*----------------------------------------------
		SkinClusterの情報を解析してInfluenceを格納する
	----------------------------------------------*/

	inverseBindPoseMatrices_.resize(skeleton.joints.size());
	std::generate(inverseBindPoseMatrices_.begin(), inverseBindPoseMatrices_.end(), MakeIdentityMatrix4x4);


	for (const auto& jointWeight : meshBoneData.jointWeights)
	{
		// skeletonに対象となるjointが含まれているか判断する
		auto it = skeleton.jointMap.find(jointWeight.first);

		// 存在しない名前のJointは後回し
		if (it == skeleton.jointMap.end())
			continue;

		// 該当のIndexのInverseBindPoseMatrixを代入する
		inverseBindPoseMatrices_[(*it).second] = jointWeight.second.inverseBindPoseMatrix;

		for (const auto& vertexWeight : jointWeight.second.vertexWeights)
		{
			// 該当のvertexIndexのInfluence情報を参照しておく
			auto& currentInfluence = mappedInfluence_[vertexWeight.vertexIndex];

			// 空いているところ (Weight == 0.0f) に入れる
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index)
			{
				if (currentInfluence.weights[index] == 0.0f)
				{
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
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
		assert(jointIndex < inverseBindPoseMatrices_.size());

		mappedPalette_[jointIndex].skeletonSpaceMatrix =
			inverseBindPoseMatrices_[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;

		mappedPalette_[jointIndex].skeletonSpaceInverseTransposeMatrix = (mappedPalette_[jointIndex].skeletonSpaceMatrix).Inverse().Transpose();
	}
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param wellRootParameterIndex 
/// @param influenceRootParameterIndex 
void Engine::SkinCluster::Register(ID3D12GraphicsCommandList* commandList, UINT wellRootParameterIndex, UINT influenceRootParameterIndex)
{
	commandList->SetComputeRootDescriptorTable(wellRootParameterIndex, paletteSrvHandle_.second);
	commandList->SetComputeRootDescriptorTable(influenceRootParameterIndex, influenceSrvHandle_.second);
}