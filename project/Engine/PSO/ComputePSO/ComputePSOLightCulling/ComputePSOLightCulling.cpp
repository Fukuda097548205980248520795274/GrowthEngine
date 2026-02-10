#include "ComputePSOLightCulling.h"
#include "ShaderCompiler/ShaderCompiler.h"
#include "Log/Log.h"
#include <cassert>

/// @brief 初期化
/// @param device 
/// @param compiler
/// @param log 
void Engine::ComputePSOLightCulling::Initialize(ID3D12Device* device, ShaderCompiler* compiler, Log* log)
{
	// シェーダのバイナリデータを取得する
	computeShaderBlob_ = compiler->Compile(L"./Assets/Shader/LightCulling/LightCulling.CS.hlsl", L"cs_6_0");
	assert(computeShaderBlob_ != nullptr);


	/*-----------------------
		ディスクリプタレンジ
	-----------------------*/

	D3D12_DESCRIPTOR_RANGE light[1];
	light[0].BaseShaderRegister = 0;
	light[0].RegisterSpace = 0;
	light[0].NumDescriptors = 1;
	light[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	light[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE tileLightIndices[1];
	tileLightIndices[0].BaseShaderRegister = 0;
	tileLightIndices[0].RegisterSpace = 0;
	tileLightIndices[0].NumDescriptors = 1;
	tileLightIndices[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	tileLightIndices[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE tileLightCount[1];
	tileLightCount[0].BaseShaderRegister = 1;
	tileLightCount[0].RegisterSpace = 0;
	tileLightCount[0].NumDescriptors = 1;
	tileLightCount[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	tileLightCount[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;



	/*---------------------
		ルートパラメータ
	---------------------*/

	D3D12_ROOT_PARAMETER rootParameter[4];

	// SRV DescriptorTable t0
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = light;
	rootParameter[0].DescriptorTable.NumDescriptorRanges = _countof(light);

	// UAV DescriptorTable u0
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = tileLightIndices;
	rootParameter[1].DescriptorTable.NumDescriptorRanges = _countof(tileLightIndices);

	// UAV DescriptorTable u1
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = tileLightCount;
	rootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(tileLightCount);

	// CBV b0
	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[3].Descriptor.RegisterSpace = 0;
	rootParameter[3].Descriptor.ShaderRegister = 0;


	/*-------------------------
		ルートシグネチャの生成
	-------------------------*/

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// ルートパラメータを設定する
	descriptionRootSignature.pParameters = rootParameter;
	descriptionRootSignature.NumParameters = _countof(rootParameter);

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