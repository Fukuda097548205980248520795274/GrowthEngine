#include "PSODepthBasedOutline.h"
#include <cassert>
#include "Log/Log.h"
#include "ShaderCompiler/ShaderCompiler.h"

/// @brief 初期化
/// @param device 
/// @param compiler 
/// @param vertexShaderBlob 
/// @param log 
void Engine::PSODepthBasedOutline::Initialize(ID3D12Device* device, ShaderCompiler* compiler, IDxcBlob* vertexShaderBlob, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(compiler);
	assert(vertexShaderBlob);


	// ピクセルシェーダのバイナリデータを取得する
	pixelShaderBlob_ = compiler->Compile(L"./Assets/Shader/PostEffect/DepthBasedOutline.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);


	/*------------------------------
		ディスクリプタレンジの設定
	------------------------------*/

	D3D12_DESCRIPTOR_RANGE descriptorRange[1];
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].RegisterSpace = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// 深度テクスチャ用のディスクリプタレンジ
	D3D12_DESCRIPTOR_RANGE depthDescriptor[1];
	depthDescriptor[0].BaseShaderRegister = 1;
	depthDescriptor[0].RegisterSpace = 0;
	depthDescriptor[0].NumDescriptors = 1;
	depthDescriptor[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	depthDescriptor[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;



	/*-------------------------
		ルートパラメータの設定
	-------------------------*/

	D3D12_ROOT_PARAMETER rootParameter[3];

	// DescriptorTable PixelShader t0 テクスチャ
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameter[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	// ConstantBuffer PixelShader b0 パラメータ
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[1].Descriptor.ShaderRegister = 0;
	rootParameter[1].Descriptor.RegisterSpace = 0;

	// DescriptorTable PixelShader t1 テクスチャ
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = depthDescriptor;
	rootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(depthDescriptor);


	/*--------------------
		サンプラーの設定
	--------------------*/

	D3D12_STATIC_SAMPLER_DESC samplers[2] = {};

	// 線形フィルタ、テクスチャの繰り返し、比較なし、最大LOD
	samplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	samplers[0].ShaderRegister = 0;
	samplers[0].RegisterSpace = 0;
	samplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	// ポイントフィルタ、テクスチャの繰り返し、比較なし、最大LOD
	samplers[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	samplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplers[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplers[1].MaxLOD = D3D12_FLOAT32_MAX;
	samplers[1].ShaderRegister = 1;
	samplers[1].RegisterSpace = 0;
	samplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


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
		if (log)log->Logging(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
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

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;


	/*-------------------------
		ブレンドステートの設定
	-------------------------*/

	D3D12_BLEND_DESC blendDesc{};

	// 全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;


	/*-----------------------------
		ラスタライザステートの設定
	-----------------------------*/

	D3D12_RASTERIZER_DESC rasterizerDesc{};

	// 裏面をカリングする
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;

	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	/*----------------------
		深度ステートの設定
	----------------------*/

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};

	// Depthは無効にする
	depthStencilDesc.DepthEnable = false;


	/*------------------
		PSOを生成する
	------------------*/

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
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
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));
}