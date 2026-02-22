#include "PSOParticle.h"
#include "Log/Log.h"
#include <cassert>

/// @brief 初期化
/// @param device 
/// @param vertexShaderBlob 
/// @param pixelShaderBlob 
/// @param log 
void Engine::PSOParticle::Initialize(ID3D12Device* device, IDxcBlob* vertexShaderBlob, IDxcBlob* pixelShaderBlob, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(vertexShaderBlob);
	assert(pixelShaderBlob);


	/*------------------------
		ディスクリプタレンジ
	------------------------*/

	// SRV t0 パーティクル
	D3D12_DESCRIPTOR_RANGE descriptorParticle[1];
	descriptorParticle[0].BaseShaderRegister = 0;
	descriptorParticle[0].RegisterSpace = 0;
	descriptorParticle[0].NumDescriptors = 1;
	descriptorParticle[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorParticle[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// SRV t0 テクスチャ
	D3D12_DESCRIPTOR_RANGE descriptorTexture[1];
	descriptorTexture[0].BaseShaderRegister = 0;
	descriptorTexture[0].RegisterSpace = 0;
	descriptorTexture[0].NumDescriptors = 1;
	descriptorTexture[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorTexture[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	/*-------------------------
		ルートパラメータの設定
	-------------------------*/

	D3D12_ROOT_PARAMETER rootParameter[3];

	// DescriptorTable VertexShader パーティクル
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameter[0].DescriptorTable.pDescriptorRanges = descriptorParticle;
	rootParameter[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorParticle);

	// CBV VertexShader b0 ビュー
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameter[1].Descriptor.RegisterSpace = 0;
	rootParameter[1].Descriptor.ShaderRegister = 0;

	// DescriptorTable PixelShader テクスチャ
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = descriptorTexture;
	rootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorTexture);


	/*--------------------
		サンプラーの設定
	--------------------*/

	D3D12_STATIC_SAMPLER_DESC samplers[1] = {};

	// サンプラー
	samplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	samplers[0].ShaderRegister = 0;
	samplers[0].RegisterSpace = 0;
	samplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


	/*---------------------------------------
		ルートシグネチャのバイナリデータの生成
	---------------------------------------*/

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
		log->Logging(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}



	/*-------------------------
		ルートシグネチャの生成
	-------------------------*/

	// バイナリを元に生成
	hr = device->CreateRootSignature(0,
		signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));


	/*----------------------------
		インプットレイアウトの設定
	----------------------------*/

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};

	// POSITION 0 float4
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	// TEXCOORD 0 float2
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


	/*-----------------------------
		ラスタライザステートの設定
	-----------------------------*/

	D3D12_RASTERIZER_DESC rasterizerDesc{};

	// 裏面をカリングする
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;

	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	/*------------------
		PSOを生成する
	------------------*/

	for (int32_t i = 0; i < static_cast<int32_t>(BlendMode::kNumBlendMode); ++i)
	{
		/*----------------------
			深度ステートの設定
		----------------------*/

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};

		// Depthを有効化する
		depthStencilDesc.DepthEnable = true;

		// 書き込む
		if (i == static_cast<int32_t>(BlendMode::kNone))
		{
			depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		}
		else
		{
			depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		}

		// 比較関数　近ければ描画する
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();
		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
		graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };
		graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };
		graphicsPipelineStateDesc.BlendState = CreateBlendMode(static_cast<BlendMode>(i));
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;

		// 書き込むRTVの情報
		graphicsPipelineStateDesc.NumRenderTargets = 1;
		graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

		// 書き込むDSVの情報
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		// 利用するトポロジ（形状）
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// 画面をうちこむ設定
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		// 生成
		hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_[i]));
		assert(SUCCEEDED(hr));
	}
}