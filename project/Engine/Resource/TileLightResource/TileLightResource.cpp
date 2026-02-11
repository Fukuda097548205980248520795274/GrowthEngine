#include "TileLightResource.h"
#include <cassert>
#include <format>
#include "RenderContext/DX12Heap/DX12Heap.h"
#include "Log/Log.h"
#include "Func/ResourceFunc/ResourceFunc.h"

/// @brief 初期化
/// @param device 
/// @param heap 
/// @param num 
/// @param log 
void Engine::TileLightResource::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Heap* heap, uint32_t num, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(heap);

	// リソース作成
	resource_ = CreateUAVResource(device, sizeof(uint32_t) * num, log , commandList);

	// UAVの設定
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = num;
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	uavDesc.Buffer.StructureByteStride = sizeof(uint32_t);

	// ハンドルを取得する
	srvHandle_.first = heap->GetSrvCPUDescriptorHandle();
	srvHandle_.second = heap->GetSrvGPUDescriptorHandle();

	// ビューの生成
	device->CreateUnorderedAccessView(resource_.Get(),nullptr, &uavDesc, srvHandle_.first);

	// ログ出力
	if (log)
	{
		log->Logging("UAV Format : UNKNOWN");
		log->Logging("UAV ViewDimension : DIMENSION_BUFFER");
		log->Logging("UAV Buffer Flags : NONE");
		log->Logging(std::format("UAV Buffer CounterOffsetInBytes : {}", uavDesc.Buffer.CounterOffsetInBytes));
		log->Logging(std::format("UAV Buffer FirstElement : {}", uavDesc.Buffer.FirstElement));
		log->Logging(std::format("UAV Buffer NumElements : {}", uavDesc.Buffer.NumElements));
		log->Logging(std::format("UAV Buffer StructureByteStride : {} byte", uavDesc.Buffer.StructureByteStride));
		log->Logging("Creat UnorderedAccessView \n");
	}
}


/// @brief コマンドリストに登録する
/// @param commandList 
/// @param rootParameterIndex 
void Engine::TileLightResource::Register(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	commandList->SetComputeRootDescriptorTable(rootParameterIndex, srvHandle_.second);
}