#include "TrailData.h"
#include <cassert>
#include "PSO/PSOTrail/PSOTrail.h"
#include "Store/Camera3DStore/Camera3DStore.h"
#include "Store/TextureStore/TextureStore.h"

/// @brief 初期化
/// @param device 
/// @param pso 
/// @param textureStore 
void Engine::TrailData::Initialize(ID3D12Device* device, PSOTrail* pso, TextureStore* textureStore, Camera3DStore* camera3DStore)
{
	// nullptrチェック
	assert(device);
	assert(pso);
	assert(textureStore);
	assert(camera3DStore);

	// 引数を受け取る
	textureStore_ = textureStore;
	pso_ = pso;
	camera3DStore_ = camera3DStore;

	// 頂点の最大数を計算する
	maxVertexCount_ = maxHistoryCount_ * 2;

	// インデックスの最大数を計算する
	maxIndexCount_ = (maxHistoryCount_ - 1) * 6;

	// パラメータの生成
	param_ = std::make_unique<Param>();
	param_->basePosition = Vector3(0.0f, 0.0f, 0.0f);
	param_->tipPosition = Vector3(0.0f, 0.0f, 0.0f);
	param_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	param_->easing_ = 1.0f;
	param_->blendMode = BlendMode::kNormal;

	// 頂点リソースの生成
	vertexBufferResource_ = std::make_unique<VertexBufferResource<TrailVertexDataForGPU>>();
	vertexBufferResource_->Initialize(device, maxVertexCount_, nullptr);

	// インデックスリソースの生成
	indexBufferResource_ = std::make_unique<IndexBufferResource>();
	indexBufferResource_->Initialize(device, maxIndexCount_, nullptr);
	for (int32_t i = 0; i < maxHistoryCount_ - 1; ++i) 
	{
		int32_t indexOffset = i * 6;
		int32_t vertexOffset = i * 2;

		indexBufferResource_->data_[indexOffset + 0] = vertexOffset + 0;
		indexBufferResource_->data_[indexOffset + 1] = vertexOffset + 1;
		indexBufferResource_->data_[indexOffset + 2] = vertexOffset + 2;

		indexBufferResource_->data_[indexOffset + 3] = vertexOffset + 2;
		indexBufferResource_->data_[indexOffset + 4] = vertexOffset + 1;
		indexBufferResource_->data_[indexOffset + 5] = vertexOffset + 3;
	}

	// 座標変換リソースの生成
	transformationResource_ = std::make_unique<ConstantBufferResource<Matrix4x4>>();
	transformationResource_->Initialize(device, nullptr);
	*transformationResource_->data_ = MakeIdentityMatrix4x4();
}

/// @brief 更新処理
void Engine::TrailData::Update()
{
	// 現在の位置と色を履歴に追加する
	TrailHistory newHistory = {
		Vector4(param_->basePosition.x, param_->basePosition.y, param_->basePosition.z, 1.0f),
		Vector4(param_->tipPosition.x, param_->tipPosition.y, param_->tipPosition.z, 1.0f),
		param_->color
	};
	histories_.push_back(newHistory);

	// 履歴が最大数を超えていたら古い履歴を削除する
	while (histories_.size() >= maxHistoryCount_)
	{
		histories_.pop_front();
	}

	// 履歴が2つ未満（線が引けない状態）なら頂点更新をスキップ
	if (histories_.size() < 2) return;

	// 頂点データを更新する
	int currentHistoryCount = static_cast<int>(histories_.size());
	for (int i = 0; i < currentHistoryCount; ++i)
	{
		// 0.0(最古) ～ 1.0(最新) の割合
		float progress = static_cast<float>(i) / static_cast<float>(currentHistoryCount - 1);

		// インデックスが大きい(最新)ほど濃く、小さい(古い)ほど透明にする
		float alpha = std::powf(progress, param_->easing_);

		//-----------------------------
		// 基点の頂点 (i * 2 + 0)
		//-----------------------------
		// 座標 (Wは必ず 1.0f にする)
		vertexBufferResource_->data_[i * 2 + 0].position = { histories_[i].basePosition.x, histories_[i].basePosition.y, histories_[i].basePosition.z, 1.0f };
		// UV (Uは時間の進捗、Vは0.0f)
		vertexBufferResource_->data_[i * 2 + 0].texcoord = { progress, 0.0f };
		// 色 (元の色に計算したアルファ値を適用)
		vertexBufferResource_->data_[i * 2 + 0].color = { histories_[i].color.x, histories_[i].color.y, histories_[i].color.z, alpha };

		//-----------------------------
		// 終点の頂点 (i * 2 + 1)
		//-----------------------------
		vertexBufferResource_->data_[i * 2 + 1].position = { histories_[i].tipPosition.x, histories_[i].tipPosition.y, histories_[i].tipPosition.z, 1.0f };
		vertexBufferResource_->data_[i * 2 + 1].texcoord = { progress, 1.0f };
		vertexBufferResource_->data_[i * 2 + 1].color = { histories_[i].color.x, histories_[i].color.y, histories_[i].color.z, alpha };
	}
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::TrailData::Register(ID3D12GraphicsCommandList* commandList)
{
	// 履歴が2つ未満なら描画しない
	if (histories_.size() < 2)
		return;

	// 座標変換を計算する
	*transformationResource_->data_ = camera3DStore_->GetCamera3D().GetCurrentVPMatrix();


	/*-----------------------
	    コマンドリストに登録
	-----------------------*/

	// PSOを登録する
	pso_->Register(commandList, param_->blendMode);

	// 形状を登録する
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点とインデックスを登録する
	vertexBufferResource_->Register(commandList);
	indexBufferResource_->Register(commandList);

	// 座標変換を登録する
	transformationResource_->RegisterGraphics(commandList, 0);

	// テクスチャを登録する
	commandList->SetGraphicsRootDescriptorTable(1, textureStore_->GetSrvGpuHandle(hTexture_));

	// ドローコール
	UINT indexCount = static_cast<UINT>((histories_.size() - 1) * 6);
	commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
}