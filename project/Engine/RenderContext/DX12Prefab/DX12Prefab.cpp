#include "DX12Prefab.h"
#include <cassert>
#include "ShaderCompiler/ShaderCompiler.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "Store/TextureStore/TextureStore.h"

#include "Application/PrefabInstance/PrefabInstanceCube/PrefabInstanceCube.h"

/// @brief 初期化
/// @param device 
/// @param compiler 
/// @param heap 
/// @param modelStore 
/// @param textureStore 
/// @param animationStore 
/// @param skeletonStore 
/// @param lightStore 
/// @param cameraStore 
/// @param log 
void Engine::DX12Prefab::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ShaderCompiler* compiler, DX12Heap* heap,
	ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, SkeletonStore* skeletonStore,
	LightStore* lightStore, Camera3DStore* cameraStore, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(compiler);
	assert(heap);
	assert(modelStore);
	assert(textureStore);
	assert(animationStore);
	assert(skeletonStore);
	assert(lightStore);
	assert(cameraStore);


	// プリミティブ用プレハブストアの生成
	prefab3DStore_ = std::make_unique<Prefab3DStore>();
	prefab3DStore_->Initialize(device, compiler, heap, modelStore, textureStore, animationStore, skeletonStore, lightStore, cameraStore, log);

	// スプライト用プレハブストアの生成
	prefab2DStore_ = std::make_unique<Prefab2DStore>();
	prefab2DStore_->Initialize(device, compiler, log);

#ifdef _DEVELOPMENT

	// 立方体を生成とインスタンス確保
	cube_ = std::make_unique<PrefabBaseCube>(textureStore->Load("./Assets/Textures/white2x2.png", heap, device, commandList, log) , 512 , "Debug_Object_Cube");
	cubeInstances_.resize(512);
	for (int i = 0; i < 512; ++i)cubeInstances_[i] = cube_->CreateInstance();

#endif
}

/// @brief 更新処理
void Engine::DX12Prefab::Update()
{
	// 更新
	prefab3DStore_->Update();
	prefab2DStore_->Update();
}

/// @brief リセット
void Engine::DX12Prefab::Reset()
{
#ifdef _DEVELOPMENT

	// 描画数
	cubeNumDraw_ = 0;

#endif
}

/// @brief シーン前のリセット
void Engine::DX12Prefab::PerSceneReset()
{
	prefab3DStore_->PerSceneReset();
	prefab2DStore_->PerSceneReset();
}

/// @brief シャドウマップの描画処理
/// @param viewProjection 
/// @param commandList 
/// @param pso 
void Engine::DX12Prefab::ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
	prefab3DStore_->ShadowMapDraw(viewProjection, commandList, pso);
}

/// @brief モーションベクターの描画処理
/// @param commandList 
/// @param pso 
void Engine::DX12Prefab::DrawMotionVector(ID3D12GraphicsCommandList* commandList, BasePSOMotionVector* pso)
{
	prefab3DStore_->DrawMotionVector(commandList, pso);
}

/// @brief リセット
void Engine::DX12Prefab::PrefabReset()
{
	prefab3DStore_->Reset();
	prefab2DStore_->Reset();
}

/// @brief デバッグ用パラメータ
void Engine::DX12Prefab::DebugParameter()
{
#ifdef _DEVELOPMENT

	// メニューバーを使用する
	if (!ImGui::Begin("Prefab"))
	{
		ImGui::End();
		return;
	}

	ImGui::SeparatorText("Primitive");

	prefab3DStore_->DebugParameter();

	ImGui::Text("\n\n\n\n\n");
	ImGui::SeparatorText("Sprite");

	prefab2DStore_->DebugParameter();

	// 終了
	ImGui::End();

#endif
}




#ifdef _DEVELOPMENT

/// @brief 立方体を描画する
/// @param position 
/// @param rotate 
/// @param scale 
/// @param color 
void Engine::DX12Prefab::DrawDebugCube(const Vector3& position, const Vector3& rotate, const Vector3& scale, const Vector4& color)
{
	// 最大数は超えてはいけない
	if (cubeNumDraw_ >= 512)
		return;

	// 値を渡す
	cubeInstances_[cubeNumDraw_]->param_.transform.translate = position;
	cubeInstances_[cubeNumDraw_]->param_.transform.rotate = rotate;
	cubeInstances_[cubeNumDraw_]->param_.transform.scale = scale;
	cubeInstances_[cubeNumDraw_]->param_.material.color = color;

	// 描画命令
	cubeInstances_[cubeNumDraw_]->Draw();

	// カウントする
	cubeNumDraw_++;
}

#endif