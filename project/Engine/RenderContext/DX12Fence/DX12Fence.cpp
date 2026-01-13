#include "DX12Fence.h"
#include "Log/Log.h"
#include <cassert>

/// <summary>
/// デストラクタ
/// </summary>
Engine::DX12Fence::~DX12Fence()
{
	// イベントハンドルを閉じる
	CloseHandle(fenceEvent_);
}

/// <summary>
/// 初期化
/// </summary>
/// <param name="device"></param>
void Engine::DX12Fence::Initialize(Log* log, ID3D12Device* device)
{
	// nullptrチェック
	assert(device);


	/*------------------
		フェンスの生成
	------------------*/

	HRESULT hr = device->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

	// フェンス生成成功のログ
	if (log)log->Logging("CreateFence \n");



	/*-------------------
		イベントの生成
	-------------------*/

	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);

	// イベント生成成功のログ
	if (log)log->Logging("CreateEvent \n");
}


/// <summary>
/// シグナルを送る
/// </summary>
void Engine::DX12Fence::SendSignal(ID3D12CommandQueue* commandQueue)
{
	assert(commandQueue);

	// フェンスの値を更新
	fenceValue_++;

	// GPUがここまでたどり着いた時に、Fenceの値を、指定した値に代入するようにシグナルを送る
	commandQueue->Signal(fence_.Get(), fenceValue_);
}

/// <summary>
/// GPUを待つ
/// </summary>
void Engine::DX12Fence::WaitGPU()
{
	// フェンスの値が指定したシグナル値にたどりついているか確認する
	if (fence_->GetCompletedValue() < fenceValue_)
	{
		// 指定したシグナル値にたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);

		// イベントを待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
}