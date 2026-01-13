#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <cstdint>

namespace Engine
{
	class Log;

	class DX12Heap
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param log 
		void Initialize(ID3D12Device* device, Log* log);

		/// @brief RTV用ディスクリプタヒープを取得する
		/// @return 
		ID3D12DescriptorHeap* GetRtvDescriptorHeap()const { return rtvDescriptorHeap_.Get(); }

		/// @brief SRV用ディスクリプタヒープを取得する
		/// @return 
		ID3D12DescriptorHeap* GetSrvDescriptorHeap()const { return srvDescriptorHeap_.Get(); }
		
		/// @brief DSV用ディスクリプタヒープを取得する
		/// @return 
		ID3D12DescriptorHeap* GetDsvDescriptorHeap()const { return dsvDescriptorHeap_.Get(); }



		/// @brief RTV用CPUハンドルを取得する
		/// @return 
		D3D12_CPU_DESCRIPTOR_HANDLE GetRtvCPUDescriptorHandle();


		/// @brief SRV用CPUハンドルを取得する
		/// @return 
		D3D12_CPU_DESCRIPTOR_HANDLE GetSrvCPUDescriptorHandle();

		/// @brief SRV用GPUハンドルを取得する
		/// @return 
		D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGPUDescriptorHandle();


		/// @brief DSV用CPUハンドルを取得する
		/// @return 
		D3D12_CPU_DESCRIPTOR_HANDLE GetDsvCPUDescriptorHandle();

		// Microsoft::WRL 省略
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


	private:

		/// @brief ディスクリプタヒープを生成する
		/// @param heapType 
		/// @param descriptorNum 
		/// @param shaderVisible 
		/// @return 
		ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT descriptorNum, bool shaderVisible);

		// デバイス
		ID3D12Device* device_ = nullptr;

		// ログ
		Log* log_ = nullptr;


		// RTV用ディスクリプタヒープ
		ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;

		// RTV用ディスクリプタ数
		UINT rtvDescriptorNum_ = 8;

		// RTV用CPUディスクリプタ使用数
		int32_t useRtvCPUDescriptor_ = 0;



		// SRV用ディスクリプタヒープ
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;

		// SRV用ディスクリプタ数
		UINT srvDescriptorNum_ = 512;

		// SRV用CPUディスクリプタ使用数
		int32_t useSrvCPUDescriptor_ = 1;

		// SRV用GPUディスクリプタ使用数
		int32_t useSrvGPUDescriptor_ = 1;



		// DSV用ディスクリプタヒープ
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;

		// DSV用ディスクリプタ数
		UINT dsvDescriptorNum_ = 8;

		// DSV用CPUディスクリプタ使用数
		int32_t useDsvCPUDescriptor_ = 0;

		// DSV用GPUディスクリプタ使用数
		int32_t useDsvGPUDescriptor_ = 0;
	};
}