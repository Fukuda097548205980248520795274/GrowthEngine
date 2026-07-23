#include "PSOLuminanceBasedOutline.h"
#include <cassert>
#include "Log/Log.h"
#include "ShaderCompiler/ShaderCompiler.h"

/// @brief 初期化
/// @param device 
/// @param compiler 
/// @param vertexShaderBlob 
/// @param log 
void Engine::PSOLuminanceBasedOutline::Initialize(ID3D12Device* device, ShaderCompiler* compiler, IDxcBlob* vertexShaderBlob, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(compiler);
	assert(vertexShaderBlob);


	// ピクセルシェーダのバイナリデータを取得する
	pixelShaderBlob_ = compiler->Compile(L"./Assets/Shader/Outline/LuminanceBasedOutline.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);


	/*------------------------------
		ディスクリプタレンジの設定
	------------------------------*/

	// t0 テクスチャ
	D3D12_DESCRIPTOR_RANGE colorTextureDescriptor[1];
	colorTextureDescriptor[0].BaseShaderRegister = 0;
	colorTextureDescriptor[0].RegisterSpace = 0;
	colorTextureDescriptor[0].NumDescriptors = 1;
	colorTextureDescriptor[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	colorTextureDescriptor[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// t1 深度テクスチャ
	D3D12_DESCRIPTOR_RANGE depthTextureDescriptor[1];
	depthTextureDescriptor[0].BaseShaderRegister = 1;
	depthTextureDescriptor[0].RegisterSpace = 0;
	depthTextureDescriptor[0].NumDescriptors = 1;
	depthTextureDescriptor[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	depthTextureDescriptor[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// t2 アウトライン用テクスチャ
	D3D12_DESCRIPTOR_RANGE outlineTextureDescriptor[1];
	outlineTextureDescriptor[0].BaseShaderRegister = 2;
	outlineTextureDescriptor[0].RegisterSpace = 0;
	outlineTextureDescriptor[0].NumDescriptors = 1;
	outlineTextureDescriptor[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	outlineTextureDescriptor[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	/*-------------------------
		ルートパラメータの設定
	-------------------------*/

	D3D12_ROOT_PARAMETER rootParameter[4];

	// DescriptorTable テクスチャ
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = colorTextureDescriptor;
	rootParameter[0].DescriptorTable.NumDescriptorRanges = _countof(colorTextureDescriptor);

	// DescriptorTable 深度テクスチャ
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = depthTextureDescriptor;
	rootParameter[1].DescriptorTable.NumDescriptorRanges = _countof(depthTextureDescriptor);

	// DescriptorTable アウトライン用テクスチャ
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = outlineTextureDescriptor;
	rootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(outlineTextureDescriptor);

	// CBV b0
	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[3].Descriptor.RegisterSpace = 0;
	rootParameter[3].Descriptor.ShaderRegister = 0;


	/*--------------------
		サンプラーの設定
	--------------------*/

	// s0
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].RegisterSpace = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


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
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

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