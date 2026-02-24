#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "Resource/VertexBufferResource/VertexBufferResource.h"
#include "Resource/StructuredBufferResource/StructuredBufferResource.h"
#include "Resource/ConstantBufferResource/ConstantBufferResource.h"
#include "PSO/PSOLine/PSOLine.h"

#include "Math/Matrix/Matrix4x4/Matrix4x4.h"
#include "Math/Vector/Vector4/Vector4.h"

namespace Engine
{
	class DX12Heap;
	class ShaderCompiler;
	class Log;

	class DX12Line
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param heap 
		/// @param compiler 
		/// @param log 
		void Initialize(ID3D12Device* device, DX12Heap* heap, ShaderCompiler* compiler, Log* log);

		/// @brief リセット
		void Reset() { drawCount_ = 0; }

		/// @brief ドローコール
		/// @param start 
		/// @param diff 
		/// @param color 
		void DrawCall(const Vector3& start, const Vector3& diff, const Vector4& color);

		/// @brief 描画処理
		/// @param commandList 
		/// @param viewProjection 
		void Draw(ID3D12GraphicsCommandList* commandList, const Matrix4x4& viewProjection);


	private:

		/// @brief 頂点リソース
		std::unique_ptr<VertexBufferResource<Vector4>> vertexResource_ = nullptr;

		// 線リソース
		std::unique_ptr<StructuredBufferResource<Vector4>> lineResource_ = nullptr;

		// 座標変換リソース
		std::unique_ptr<ConstantBufferResource<Matrix4x4>> transformationResource_ = nullptr;


		// 線PSO
		std::unique_ptr<PSOLine> psoLine_ = nullptr;


	private:

		// 最大数
		const uint32_t kMaxNumLine = 3000;

		// 描画数
		uint32_t drawCount_ = 0;
	};
}