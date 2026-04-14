#include "ComputePSOUVSphere.h"
#include "Log/Log.h"
#include "ShaderCompiler/ShaderCompiler.h"
#include <cassert>

/// @brief 初期化
/// @param device 
/// @param compiler 
/// @param log 
void Engine::ComputePSOUVSphere::Initialize(ID3D12Device* device, ShaderCompiler* compiler, Log* log)
{
	// シェーダのバイナリデータを取得する
	computeShaderBlob_ = compiler->Compile(L"./Assets/Shader/Primitive/UVSphere.CS.hlsl", L"cs_6_0");
	assert(computeShaderBlob_ != nullptr);


	/*-----------------------
		ディスクリプタレンジ
	-----------------------*/

	D3D12_DESCRIPTOR_RANGE vertexDescription[1];
	vertexDescription[0].BaseShaderRegister = 0;
	vertexDescription[0].RegisterSpace = 0;
	vertexDescription[0].NumDescriptors = 1;
	vertexDescription[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	vertexDescription[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE indexDescription[1];
	indexDescription[0].BaseShaderRegister = 1;
	indexDescription[0].RegisterSpace = 0;
	indexDescription[0].NumDescriptors = 1;
	indexDescription[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	indexDescription[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;



	/*---------------------
		ルートパラメータ
	---------------------*/

	D3D12_ROOT_PARAMETER rootParameter[3];

	// CBV b0
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].Descriptor.RegisterSpace = 0;
	rootParameter[0].Descriptor.ShaderRegister = 0;

	// UAV DescriptorTable u0
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = vertexDescription;
	rootParameter[1].DescriptorTable.NumDescriptorRanges = _countof(vertexDescription);

	// UAV DescriptorTable u1
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = indexDescription;
	rootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(indexDescription);


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