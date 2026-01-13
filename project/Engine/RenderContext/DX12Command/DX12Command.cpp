#include "DX12Command.h"
#include "Log/Log.h"
#include <cassert>

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::DX12Command::Initialize(ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(device);

	/*---------------------------
		コマンドキューを生成する
	---------------------------*/

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	HRESULT hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(hr));

	// コマンドキュー生成成功のログ
	if(log)log->Logging("CreateCommandQueue \n");



	/*-----------------------------
		コマンドアロケータを生成する
	-----------------------------*/

	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(hr));

	// コマンドアロケータ生成成功のログ
	if (log)log->Logging("CreateCommandAllocator \n");


	/*--------------------------
		コマンドリストを生成する
	--------------------------*/

	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	assert(SUCCEEDED(hr));

	// コマンドリスト生成成功のログ
	if (log)log->Logging("CreateCommandList \n");
}