#include "ComputePSODualBlurDownsample.h"
#include "Log/Log.h"
#include "ShaderCompiler/ShaderCompiler.h"
#include <cassert>

/// @brief 初期化
/// @param device 
/// @param compiler 
/// @param log 
void Engine::ComputePSODualBlurDownsample::Initialize(ID3D12Device* device, ShaderCompiler* compiler, Log* log)
{
	// シェーダのバイナリデータを取得する
	computeShaderBlob_ = compiler->Compile(L"./Assets/Shader/PostEffect/DualBlurDownsample.CS.hlsl", L"cs_6_0");
	assert(computeShaderBlob_ != nullptr);


	/*-----------------------
		ディスクリプタレンジ
	-----------------------*/

	// ディスクリプタレンジを設定する1
	D3D12_DESCRIPTOR_RANGE colorTextureDescriptor[1];
	colorTextureDescriptor[0].BaseShaderRegister = 0;
	colorTextureDescriptor[0].RegisterSpace = 0;
	colorTextureDescriptor[0].NumDescriptors = 1;
	colorTextureDescriptor[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	colorTextureDescriptor[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ディスクリプタレンジを設定する2
	D3D12_DESCRIPTOR_RANGE writeTextureDescriptor[1];
	writeTextureDescriptor[0].BaseShaderRegister = 0;
	writeTextureDescriptor[0].RegisterSpace = 0;
	writeTextureDescriptor[0].NumDescriptors = 1;
	writeTextureDescriptor[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	writeTextureDescriptor[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;



	/*---------------------
		ルートパラメータ
	---------------------*/

	D3D12_ROOT_PARAMETER rootParameter[2];

	// SRV DescriptorTable t0
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = colorTextureDescriptor;
	rootParameter[0].DescriptorTable.NumDescriptorRanges = _countof(colorTextureDescriptor);

	// UAV DescriptorTable u0
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = writeTextureDescriptor;
	rootParameter[1].DescriptorTable.NumDescriptorRanges = _countof(writeTextureDescriptor);


	/*--------------------
		サンプラーの設定
	--------------------*/

	// s0
	D3D12_STATIC_SAMPLER_DESC samplers[1] = {};
	samplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	samplers[0].MipLODBias = 0.0f;
	samplers[0].MaxAnisotropy = 1;
	samplers[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplers[0].MinLOD = 0.0f;
	samplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	samplers[0].ShaderRegister = 0;
	samplers[0].RegisterSpace = 0;
	samplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


	/*-------------------------
		ルートシグネチャの生成
	-------------------------*/

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// ルートパラメータを設定する
	descriptionRootSignature.pParameters = rootParameter;
	descriptionRootSignature.NumParameters = _countof(rootParameter);

	// サンプラーを設定する
	descriptionRootSignature.pStaticSamplers = samplers;
	descriptionRootSignature.NumStaticSamplers = _countof(samplers);

	// シリアライズしてバイナリにする
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);

	// エラーのとき、情報を出力し停止させる
	if (FAILED(hr))
	{
		if (log)log->Logging(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}

	// バイナリを元に生成
	hr = device->CreateRootSignature(0,
		signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));


	/*------------------
		PSOを生成する
	------------------*/

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.CS = { computeShaderBlob_->GetBufferPointer(), computeShaderBlob_->GetBufferSize() };
	computePipelineStateDesc.pRootSignature = rootSignature_.Get();

	hr = device->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(hr));
}