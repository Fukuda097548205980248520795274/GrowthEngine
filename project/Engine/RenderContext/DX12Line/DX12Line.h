#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "Resource/VertexBufferResource/VertexBufferResource.h"
#include "Resource/StructuredBufferResource/StructuredBufferResource.h"
#include "Resource/ConstantBufferResource/ConstantBufferResource.h"

#include "PSO/PSOTriangle/PSOTriangle.h"
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
		void Reset() { line3D_.drawCount = 0; line2D_.drawCount = 0; triangle3D_.drawCount = 0; }


		/// @brief 3Dラインのドローコール
		/// @param start 
		/// @param end 
		/// @param color 
		void DrawCallLine3D(const Vector3& start, const Vector3& end, const Vector4& color);

		/// @brief 2Dラインのドローコール
		/// @param start 
		/// @param end 
		/// @param color 
		void DrawCallLine2D(const Vector2& start, const Vector2& end, const Vector4& color);

		/// @brief 3D三角形のドローコール
		/// @param v0 
		/// @param v1 
		/// @param v2 
		/// @param color 
		void DrawCallTriangle3D(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector4& color);


		/// @brief 3Dラインの描画処理
		/// @param commandList 
		/// @param viewProjection 
		void DrawLine3D(ID3D12GraphicsCommandList* commandList, const Matrix4x4& viewProjection);

		/// @brief 2Dラインの描画
		/// @param commandList 
		/// @param viewProjection 
		void DrawLine2D(ID3D12GraphicsCommandList* commandList, const Matrix4x4& viewProjection);

		/// @brief 3D三角形の描画
		/// @param commandList 
		/// @param viewProjection 
		void DrawTriangle3D(ID3D12GraphicsCommandList* commandList, const Matrix4x4& viewProjection);


	private:

		// 線データ
		struct LineData
		{
			/// @brief 頂点リソース
			std::unique_ptr<VertexBufferResource<Vector4>> vertexResource = nullptr;

			// 色リソース
			std::unique_ptr<StructuredBufferResource<Vector4>> colorResource = nullptr;

			// 座標変換リソース
			std::unique_ptr<ConstantBufferResource<Matrix4x4>> transformationResource = nullptr;

			// 描画数
			uint32_t drawCount = 0;
		};

		// 三角形データ
		struct TriangleData
		{
			/// @brief 頂点リソース
			std::unique_ptr<VertexBufferResource<Vector4>> vertexResource = nullptr;

			// 色リソース
			std::unique_ptr<StructuredBufferResource<Vector4>> colorResource = nullptr;

			// 座標変換リソース
			std::unique_ptr<ConstantBufferResource<Matrix4x4>> transformationResource = nullptr;

			// 描画数
			uint32_t drawCount = 0;
		};


		// 3D線
		LineData line3D_{};

		// 2D線
		LineData line2D_{};

		// 3D三角形
		TriangleData triangle3D_{};


		// 線PSO
		std::unique_ptr<PSOLine> psoLine_ = nullptr;

		// 三角形PSO
		std::unique_ptr<PSOTriangle> psoTriangle_ = nullptr;


	private:

		// 最大数
		const uint32_t kMaxNumLine = 3000;
	};
}