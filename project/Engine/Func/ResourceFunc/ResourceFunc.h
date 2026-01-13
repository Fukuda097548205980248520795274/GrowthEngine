#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>

namespace Engine
{
	/// <summary>
	/// トランジションバリアを張る	
	/// </summary>
	/// <param name="resource"></param>
	/// <param name="before"></param>
	/// <param name="after"></param>
	void TransitionBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, ID3D12GraphicsCommandList* commandList);
}