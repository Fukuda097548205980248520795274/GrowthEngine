#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>

// ブレンドモード
enum class BlendMode
{
	// 合成なし
	kNone,

	// ノーマル
	kNormal,

	// 加算合成
	kAdd,

	// 減算合成
	kSubtract,

	// 乗算合成
	kMultiply,

	// スクリーン合成
	kScreen,

	kNumBlendMode
};

namespace Engine
{
	/// @brief ブレンドモードを作成する
	/// @param blendMode 
	/// @return 
	D3D12_BLEND_DESC CreateBlendMode(BlendMode blendMode);


	/// @brief ブレンドモード作成 : 合成なし
	/// @return 
	D3D12_BLEND_DESC CreateBlendNone();

	/// @brief ブレンドモード作成 : ノーマル合成
	/// @return 
	D3D12_BLEND_DESC CreateBlendNormal();

	/// @brief ブレンドモード作成 : 加算合成
	/// @return 
	D3D12_BLEND_DESC CreateBlendAdd();

	/// @brief ブレンドモード作成 : 減算合成
	/// @return 
	D3D12_BLEND_DESC CreateBlendSubtract();

	/// @brief ブレンドモード作成 : 乗算合成
	/// @return 
	D3D12_BLEND_DESC CreateBlendMultiply();

	/// @brief ブレンドモード作成 : スクリーン合成
	/// @return 
	D3D12_BLEND_DESC CreateBlendScreen();
}