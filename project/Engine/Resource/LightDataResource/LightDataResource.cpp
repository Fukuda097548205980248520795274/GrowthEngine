#include "LightDataResource.h"
#include <cassert>
#include <format>
#include "RenderContext/DX12Heap/DX12Heap.h"
#include "Func/ResourceFunc/ResourceFunc.h"
#include "Log/Log.h"

/// @brief 初期化
/// @param device 
/// @param heap 
/// @param num 
/// @param log 
void Engine::LightDataResource::Initialize(ID3D12Device* device, DX12Heap* heap, uint32_t num, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(heap);

	// リソース作成
	resource_ = CreateBufferResource(device, sizeof(LightDataForGPU) * num, log);

	// データを割りあてる
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));

	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = num;
	srvDesc.Buffer.StructureByteStride = sizeof(LightDataForGPU);

	// ハンドルを取得する
	srvHandle_.first = heap->GetSrvCPUDescriptorHandle();
	srvHandle_.second = heap->GetSrvGPUDescriptorHandle();

	// ビューの生成
	device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_.first);

	// ログ出力
	if (log)
	{
		log->Logging("SRV Format : UNKNOWN");
		log->Logging("SRV Shader4ComponentMapping : DEFAULT_SHADER_4_COMPONENT_MAPPING");
		log->Logging("SRV ViewDimension : DIMENSION_BUFFER");
		log->Logging("SRV Buffer Flags : NONE");
		log->Logging(std::format("SRV Buffer FirstElement : {}", srvDesc.Buffer.FirstElement));
		log->Logging(std::format("SRV Buffer NumElements : {}", srvDesc.Buffer.NumElements));
		log->Logging(std::format("SRV Buffer StructureByteStride : {} byte", srvDesc.Buffer.StructureByteStride));
		log->Logging("Creat ShaderResourceView \n");
	}
}


/// @brief コマンドリストに登録する
/// @param commandList 
/// @param rootParameterIndex 
void Engine::LightDataResource::Register(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	commandList->SetComputeRootDescriptorTable(rootParameterIndex, srvHandle_.second);
}