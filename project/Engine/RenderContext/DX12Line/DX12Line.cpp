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

	// 頂点リソースの生成と初期化
	vertexResource_ = std::make_unique<VertexBufferResource<Vector4>>();
	vertexResource_->Initialize(device, 2 * kMaxNumLine, log);

	// 線リソースの生成と初期化
	lineResource_ = std::make_unique<StructuredBufferResource<Vector4>>();
	lineResource_->Initialize(device, heap, kMaxNumLine, log);

	// 座標変換リソースの生成と初期化
	transformationResource_ = std::make_unique<ConstantBufferResource<Matrix4x4>>();
	transformationResource_->Initialize(device, log);
}

/// @brief ドローコール
void Engine::DX12Line::DrawCall(const Vector3& start, const Vector3& diff, const Vector4& color)
{
	// 描画制限
	if (drawCount_ >= kMaxNumLine)return;

	// 頂点の位置
	vertexResource_->data_[drawCount_ * 2] = Vector4(start.x, start.y, start.z, 1.0f);
	vertexResource_->data_[drawCount_ * 2 + 1] = Vector4(start.x + diff.x, start.y + diff.y, start.z + diff.z, 1.0f);

	// 色
	lineResource_->data_[drawCount_] = color;

	// カウントする
	drawCount_++;
}

/// @brief 描画処理
/// @param commandList 
/// @param viewProjection 
void Engine::DX12Line::Draw(ID3D12GraphicsCommandList* commandList, const Matrix4x4& viewProjection)
{
	// 描画していないときは処理しない
	if (drawCount_ <= 0)return;

	// PSOの設定
	psoLine_->Register(commandList);

	// 頂点の設定
	vertexResource_->Register(commandList);

	// 座標変換の設定
	*transformationResource_->data_ = viewProjection;
	transformationResource_->RegisterGraphics(commandList, 0);

	// 線の設定
	lineResource_->RegisterGraphics(commandList, 1);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	// 描画
	commandList->DrawInstanced(drawCount_ * 2, 1, 0, 0);
}