#include "DX12Render.h"
#include "ShaderCompiler/ShaderCompiler.h"
#include <cassert>

#include "RenderContext/ImGuiRender/ImGuiRender.h"

/// @brief 初期化
/// @param device 
/// @param shaderCompiler 
void Engine::DX12Render::Initialize(ID3D12Device* device, ShaderCompiler* shaderCompiler, DX12Heap* heap,
	ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, SkeletonStore* skeletonStore, LightStore* lightStore, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(shaderCompiler);


	// プリミティブストアの生成と初期化
	render3DStore_ = std::make_unique<Render3DStore>();
	render3DStore_->Initialize(device, shaderCompiler, heap, modelStore, textureStore, animationStore, skeletonStore, lightStore, log);

	// スプライトストアの生成と初期化
	render2DStore_ = std::make_unique<Render2DStore>();
	render2DStore_->Initialize(device, shaderCompiler, log);
}

/// @brief 更新処理
/// @param viewProjection 
void Engine::DX12Render::Update(ID3D12GraphicsCommandList* commandList)
{
	render3DStore_->Update(commandList);
	render2DStore_->Update();
}

/// @brief リセット
void Engine::DX12Render::Reset()
{
	render2DStore_->Reset();
	render3DStore_->Reset();
}

/// @brief シーン前のリセット
void Engine::DX12Render::PerSceneReset()
{
	render3DStore_->PerSceneReset();
	render2DStore_->PerSceneReset();
}

/// @brief シャドウアップ用描画処理
/// @param commandList 
void Engine::DX12Render::ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
	// プリミティブストアの描画
	render3DStore_->ShadowMapDraw(viewProjection, commandList, pso);
}

/// @brief モーションベクター用描画処理
/// @param commandList 
/// @param pso 
void Engine::DX12Render::DrawMotionVector(ID3D12GraphicsCommandList* commandList, BasePSOMotionVector* pso)
{
	render3DStore_->DrawMotionVector(commandList, pso);
}

/// @brief デバッグ用パラメータ
void Engine::DX12Render::DebugParameter()
{
#ifdef _DEVELOPMENT

	// メニューバーを使用する
	if (!ImGui::Begin("Model"))
	{
		ImGui::End();
		return;
	}

	ImGui::SeparatorText("Primitive");

	render3DStore_->DebugParameter();

	ImGui::Text("\n\n\n\n\n");
	ImGui::SeparatorText("Sprite");

	render2DStore_->DebugParameter();

	// 終了
	ImGui::End();

#endif
}