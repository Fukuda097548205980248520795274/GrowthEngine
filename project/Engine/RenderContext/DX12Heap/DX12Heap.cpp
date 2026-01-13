#include "DX12Heap.h"
#include "Log/Log.h"
#include <cassert>
#include <format>

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::DX12Heap::Initialize(ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(device);

	// 引数を受け取る
	device_ = device;
	log_ = log;


	// RTVディスクリプタヒープの生成
	rtvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, rtvDescriptorNum_, false);
	if (log_)log_->Logging("CreateDescriptorHeap Rtv \n");

	// SRVディスクリプタヒープの生成
	srvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, srvDescriptorNum_, true);
	if (log_)log_->Logging("CreateDescriptorHeap Srv \n");

	// DSVディスクリプタヒープの生成
	dsvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, dsvDescriptorNum_, false);
	if (log_)log_->Logging("CreateDescriptorHeap Dsv \n");
}

/// @brief RTV用CPUハンドルを取得する
/// @return 
D3D12_CPU_DESCRIPTOR_HANDLE Engine::DX12Heap::GetRtvCPUDescriptorHandle()
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * useRtvCPUDescriptor_;

	// ログを出力する
	if (log_)
	{
		log_->Logging("View : RTV");
		log_->Logging("Handle : CPU");
		log_->Logging(std::format("Num : {}", useRtvCPUDescriptor_));
		log_->Logging(std::format("Ptr : {}", cpuHandle.ptr));
		log_->Logging("GetRtvCPUDescriptorHandle \n");
	}

	// 数をカウントする
	useRtvCPUDescriptor_++;

	return cpuHandle;
}

/// @brief SRV用CPUハンドルを取得する
/// @return 
D3D12_CPU_DESCRIPTOR_HANDLE Engine::DX12Heap::GetSrvCPUDescriptorHandle()
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * useSrvCPUDescriptor_;

	// ログを出力する
	if (log_)
	{
		log_->Logging("View : SRV");
		log_->Logging("Handle : CPU");
		log_->Logging(std::format("Num : {}", useSrvCPUDescriptor_));
		log_->Logging(std::format("Ptr : {}", cpuHandle.ptr));
		log_->Logging("GetSrvCPUDescriptorHandle \n");
	}

	// 数をカウントする
	useSrvCPUDescriptor_++;

	return cpuHandle;
}

/// @brief SRV用GPUハンドルを取得する
/// @return 
D3D12_GPU_DESCRIPTOR_HANDLE Engine::DX12Heap::GetSrvGPUDescriptorHandle()
{
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	gpuHandle.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * useSrvGPUDescriptor_;

	// ログを出力する
	if (log_)
	{
		log_->Logging("View : SRV");
		log_->Logging("Handle : GPU");
		log_->Logging(std::format("Num : {}", useSrvGPUDescriptor_));
		log_->Logging(std::format("Ptr : {}", gpuHandle.ptr));
		log_->Logging("GetSrvGPUDescriptorHandle \n");
	}

	// 数をカウントする
	useSrvGPUDescriptor_++;

	return gpuHandle;
}


/// @brief DSV用CPUハンドルを取得する
/// @return 
D3D12_CPU_DESCRIPTOR_HANDLE Engine::DX12Heap::GetDsvCPUDescriptorHandle()
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV) * useDsvCPUDescriptor_;

	// ログを出力する
	if (log_)
	{
		log_->Logging("View : DSV");
		log_->Logging("Handle : CPU");
		log_->Logging(std::format("Num : {}", useDsvCPUDescriptor_));
		log_->Logging(std::format("Ptr : {}", cpuHandle.ptr));
		log_->Logging("GetDsvCPUDescriptorHandle \n");
	}

	// 数をカウントする
	useDsvCPUDescriptor_++;

	return cpuHandle;
}


/// @brief ディスクリプタヒープを生成する
/// @param heapType 
/// @param descriptorNum 
/// @param shaderVisible 
/// @return 
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Engine::DX12Heap::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT descriptorNum, bool shaderVisible)
{
	// ディスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;

	// ディスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType; // ヒープの種類
	descriptorHeapDesc.NumDescriptors = descriptorNum; // ディスクリプタの数
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	// 生成
	HRESULT hr = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));

	// ログ出力
	if (log_)
	{
		if (heapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		{
			log_->Logging(std::format("HeapType : D3D12_DESCRIPTOR_HEAP_TYPE_RTV"));
		}
		else if (heapType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		{
			log_->Logging(std::format("HeapType : D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV"));
		}
		else if (heapType == D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
		{
			log_->Logging(std::format("HeapType : D3D12_DESCRIPTOR_HEAP_TYPE_DSV"));
		}

		log_->Logging(std::format("NumDescriptors : {}", descriptorNum));
		log_->Logging(std::format("ShaderVisible : {}", shaderVisible));
	}

	return descriptorHeap;
}