#include "D3D12Core.h"
#include "Log/Log.h"
#include <cassert>
#include <format>
#include "Func/ConvertString/ConvertString.h"

/// <summary>
/// 初期化
/// </summary>
/// <param name="logfile"></param>
void Engine::D3D12Core::Initialize(Log* log)
{
	// nullptrチェック
	assert(log);


	/*--------------------------
		DXGIファクトリーの生成
	--------------------------*/

	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(hr));

	// 成功ログ出力
	if(log)log->Logging("CreateDXGIFactory \n");


	/*-----------------------------------
		使用するアダプタ（GPU）を決定する
	-----------------------------------*/

	// 良い順にアダプタを頼む
	for (UINT i = 0;
		dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND;
		++i)
	{
		// アダプタ情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter_->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));

		// ソフトウェアアダプタなら使わない
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			// 採用したアダプタ情報をログに出力
			if (log)log->Logging(ConvertString(std::format(L"Use Adapter : {}", adapterDesc.Description)));
			break;
		}

		useAdapter_ = nullptr;
	}

	// アダプタが見つからなかったら止める
	assert(useAdapter_ != nullptr);


	/*----------------------
		デバイスを生成する
	----------------------*/

	// 機能レベル
	D3D_FEATURE_LEVEL featureLevels[] =
	{ D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0 };

	// ログ出力用文字列
	const char* featureLevelStrings[] = { "12.2" , "12.1" , "12.0" };

	// 高い順に生成できるか試す
	for (size_t i = 0; i < _countof(featureLevels); ++i)
	{
		// 採用したアダプタでデバイスを生成
		hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));

		// 指定した機能レベルで生成できたか確認
		if (SUCCEEDED(hr))
		{
			// 生成したデバイスの機能レベルをログ出力
			if (log)log->Logging(std::format("FeatureLevel : {}", featureLevelStrings[i]));
			break;
		}
	}

	// うまく生成できなかったら停止させる
	assert(device_ != nullptr);
	if (log)log->Logging("D3D12CreateDevice \n");
}