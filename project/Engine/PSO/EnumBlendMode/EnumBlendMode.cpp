#include "EnumBlendMode.h"

/// @brief ブレンドモードを作成する
/// @param blendMode 
/// @return 
D3D12_BLEND_DESC Engine::CreateBlendMode(BlendMode blendMode)
{
	switch (blendMode)
	{
	case BlendMode::kNone:
	default:
		// 合成なし
		return CreateBlendNone();
		break;

	case BlendMode::kNormal:
		// ノーマル合成
		return CreateBlendNormal();
		break;

	case BlendMode::kAdd:
		// 加算合成
		return CreateBlendAdd();
		break;

	case BlendMode::kSubtract:
		// 減算合成
		return CreateBlendSubtract();
		break;

	case BlendMode::kMultiply:
		// 乗算合成
		return CreateBlendMultiply();
		break;

	case BlendMode::kScreen:
		// スクリーン合成
		return CreateBlendScreen();
		break;
	}
}

/// <summary>
/// ブレンドモード作成 : 合成なし
/// </summary>
/// <returns></returns>
D3D12_BLEND_DESC Engine::CreateBlendNone()
{
	D3D12_BLEND_DESC blendDesc{};

	// 全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return blendDesc;
}

/// <summary>
/// ブレンドモード作成 : ノーマル合成
/// </summary>
/// <returns></returns>
D3D12_BLEND_DESC Engine::CreateBlendNormal()
{
	D3D12_BLEND_DESC blendDesc{};

	// RGBを書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_RED | D3D12_COLOR_WRITE_ENABLE_GREEN | D3D12_COLOR_WRITE_ENABLE_BLUE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	return blendDesc;
}

/// <summary>
/// ブレンドモード作成 : 加算合成
/// </summary>
/// <returns></returns>
D3D12_BLEND_DESC Engine::CreateBlendAdd()
{
	D3D12_BLEND_DESC blendDesc{};

	// RGBを書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_RED | D3D12_COLOR_WRITE_ENABLE_GREEN | D3D12_COLOR_WRITE_ENABLE_BLUE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	return blendDesc;
}

/// <summary>
/// ブレンドモード作成 : 減算合成
/// </summary>
/// <returns></returns>
D3D12_BLEND_DESC Engine::CreateBlendSubtract()
{
	D3D12_BLEND_DESC blendDesc{};

	// RGBを書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_RED | D3D12_COLOR_WRITE_ENABLE_GREEN | D3D12_COLOR_WRITE_ENABLE_BLUE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	return blendDesc;
}

/// <summary>
/// ブレンドモード作成 : 乗算合成
/// </summary>
/// <returns></returns>
D3D12_BLEND_DESC Engine::CreateBlendMultiply()
{
	D3D12_BLEND_DESC blendDesc{};

	// RGBを書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_RED | D3D12_COLOR_WRITE_ENABLE_GREEN | D3D12_COLOR_WRITE_ENABLE_BLUE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	return blendDesc;
}

/// <summary>
/// ブレンドモード作成 : スクリーン合成
/// </summary>
/// <returns></returns>
D3D12_BLEND_DESC Engine::CreateBlendScreen()
{
	D3D12_BLEND_DESC blendDesc{};

	// RGBを書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_RED | D3D12_COLOR_WRITE_ENABLE_GREEN | D3D12_COLOR_WRITE_ENABLE_BLUE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	return blendDesc;
}