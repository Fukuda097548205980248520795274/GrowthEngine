#include "DX12Line.h"

/// @brief 初期化
/// @param device 
/// @param heap 
/// @param compiler 
/// @param log 
void Engine::DX12Line::Initialize(ID3D12Device* device, DX12Heap* heap, ShaderCompiler* compiler, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(heap);
	assert(compiler);

	// PSOの生成と初期化
	psoLine_ = std::make_unique<PSOLine>();
	psoLine_->Initialize(device, compiler, log);

	// PSOの生成と初期化
	psoTriangle_ = std::make_unique<PSOTriangle>();
	psoTriangle_->Initialize(device, compiler, log);



	// 頂点リソースの生成と初期化
	line3D_.vertexResource = std::make_unique<VertexBufferResource<Vector4>>();
	line3D_.vertexResource->Initialize(device, 2 * kMaxNumLine, log);

	// 線リソースの生成と初期化
	line3D_.colorResource = std::make_unique<StructuredBufferResource<Vector4>>();
	line3D_.colorResource->Initialize(device, heap, kMaxNumLine, log);

	// 座標変換リソースの生成と初期化
	line3D_.transformationResource = std::make_unique<ConstantBufferResource<Matrix4x4>>();
	line3D_.transformationResource->Initialize(device, log);



	// 頂点リソースの生成と初期化
	line2D_.vertexResource = std::make_unique<VertexBufferResource<Vector4>>();
	line2D_.vertexResource->Initialize(device, 2 * kMaxNumLine, log);

	// 線リソースの生成と初期化
	line2D_.colorResource = std::make_unique<StructuredBufferResource<Vector4>>();
	line2D_.colorResource->Initialize(device, heap, kMaxNumLine, log);

	// 座標変換リソースの生成と初期化
	line2D_.transformationResource = std::make_unique<ConstantBufferResource<Matrix4x4>>();
	line2D_.transformationResource->Initialize(device, log);



	// 頂点リソースの生成と初期化
	triangle3D_.vertexResource = std::make_unique<VertexBufferResource<Vector4>>();
	triangle3D_.vertexResource->Initialize(device, 3 * kMaxNumLine, log);

	// 色リソースの生成と初期化
	triangle3D_.colorResource = std::make_unique<StructuredBufferResource<Vector4>>();
	triangle3D_.colorResource->Initialize(device, heap, kMaxNumLine, log);

	// 座標変換リソースの生成と初期化
	triangle3D_.transformationResource = std::make_unique<ConstantBufferResource<Matrix4x4>>();
	triangle3D_.transformationResource->Initialize(device, log);
}

/// @brief ドローコール
void Engine::DX12Line::DrawCallLine3D(const Vector3& start, const Vector3& end, const Vector4& color)
{
	// 描画制限
	if (line3D_.drawCount >= kMaxNumLine)return;

	// 頂点の位置
	line3D_.vertexResource->data_[line3D_.drawCount * 2] = Vector4(start.x, start.y, start.z, 1.0f);
	line3D_.vertexResource->data_[line3D_.drawCount * 2 + 1] = Vector4(end.x, end.y, end.z, 1.0f);

	// 色
	line3D_.colorResource->data_[line3D_.drawCount] = color;

	// カウントする
	line3D_.drawCount++;
}

/// @brief 2Dラインのドローコール
/// @param start 
/// @param diff 
/// @param color 
void Engine::DX12Line::DrawCallLine2D(const Vector2& start, const Vector2& end, const Vector4& color)
{
	// 描画制限
	if (line2D_.drawCount >= kMaxNumLine)return;

	// 頂点の位置
	line2D_.vertexResource->data_[line2D_.drawCount * 2] = Vector4(start.x, start.y, 0.0f, 1.0f);
	line2D_.vertexResource->data_[line2D_.drawCount * 2 + 1] = Vector4(end.x, end.y, 0.0f, 1.0f);

	// 色
	line2D_.colorResource->data_[line2D_.drawCount] = color;

	// カウントする
	line2D_.drawCount++;
}

/// @brief 3D三角形のドローコール
/// @param v0 
/// @param v1 
/// @param v2 
/// @param color 
void Engine::DX12Line::DrawCallTriangle3D(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector4& color)
{
	// 描画制限
	if (triangle3D_.drawCount >= kMaxNumLine)return;

	// 頂点の位置
	triangle3D_.vertexResource->data_[triangle3D_.drawCount * 3] = Vector4(v0.x, v0.y, v0.z, 1.0f);
	triangle3D_.vertexResource->data_[triangle3D_.drawCount * 3 + 1] = Vector4(v1.x, v1.y, v1.z, 1.0f);
	triangle3D_.vertexResource->data_[triangle3D_.drawCount * 3 + 2] = Vector4(v2.x, v2.y, v2.z, 1.0f);

	// 色
	triangle3D_.colorResource->data_[triangle3D_.drawCount] = color;

	// カウントする
	triangle3D_.drawCount++;
}

/// @brief 描画処理
/// @param commandList 
/// @param viewProjection 
void Engine::DX12Line::DrawLine3D(ID3D12GraphicsCommandList* commandList, const Matrix4x4& viewProjection)
{
	// 描画していないときは処理しない
	if (line3D_.drawCount <= 0)return;

	// PSOの設定
	psoLine_->Register(commandList);

	// 頂点の設定
	line3D_.vertexResource->Register(commandList);

	// 座標変換の設定
	*line3D_.transformationResource->data_ = viewProjection;
	line3D_.transformationResource->RegisterGraphics(commandList, 0);

	// 色の設定
	line3D_.colorResource->RegisterGraphics(commandList, 1);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	// 描画
	commandList->DrawInstanced(line3D_.drawCount * 2, 1, 0, 0);
}

/// @brief 2Dラインの描画
/// @param commandList 
/// @param viewProjection 
void Engine::DX12Line::DrawLine2D(ID3D12GraphicsCommandList* commandList, const Matrix4x4& viewProjection)
{
	// 描画していないときは処理しない
	if (line2D_.drawCount <= 0)return;

	// PSOの設定
	psoLine_->Register(commandList);

	// 頂点の設定
	line2D_.vertexResource->Register(commandList);

	// 座標変換の設定
	*line2D_.transformationResource->data_ = viewProjection;
	line2D_.transformationResource->RegisterGraphics(commandList, 0);

	// 色の設定
	line2D_.colorResource->RegisterGraphics(commandList, 1);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	// 描画
	commandList->DrawInstanced(line2D_.drawCount * 2, 1, 0, 0);
}

/// @brief 3D三角形の描画
/// @param commandList 
/// @param viewProjection 
void Engine::DX12Line::DrawTriangle3D(ID3D12GraphicsCommandList* commandList, const Matrix4x4& viewProjection)
{
	// 描画していないときは処理しない
	if (triangle3D_.drawCount <= 0)return;

	// PSOの設定
	psoTriangle_->Register(commandList);

	// 頂点の設定
	triangle3D_.vertexResource->Register(commandList);

	// 座標変換の設定
	*triangle3D_.transformationResource->data_ = viewProjection;
	triangle3D_.transformationResource->RegisterGraphics(commandList, 0);

	// 色の設定
	triangle3D_.colorResource->RegisterGraphics(commandList, 1);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 描画
	commandList->DrawInstanced(triangle3D_.drawCount * 3, 1, 0, 0);
}