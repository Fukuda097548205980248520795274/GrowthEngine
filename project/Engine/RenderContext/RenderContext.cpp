#include "RenderContext.h"
#include "Log/Log.h"
#include <cassert>
#include "Func/ResourceFunc/ResourceFunc.h"
#include "WinApp/WinApp.h"

/// @brief 初期化
/// @param log 
void Engine::RenderContext::Initialize(WinApp* pWinApp, Log* pLog)
{
	// nullptrチェック
	assert(pWinApp);

	// 引数を受け取る
	pWinApp_ = pWinApp;

	// FPS固定初期化
	InitializeFixFPS();

#ifdef _DEBUG
	// DX12Debugの生成と初期化
	pDebug_ = std::make_unique<DX12Debug>();
	pDebug_->Initialize(pLog);
#endif

	// DX12Coreの生成と初期化
	pCore_ = std::make_unique<DX12Core>();
	pCore_->Initialize(pLog);

#ifdef _DEBUG
	// デバッグモードならデバイス生成後に警告・エラーで停止させる
	pDebug_->Stop(pCore_->GetDevice());
#endif

	// DX12Commandの生成と初期化
	pCommand_ = std::make_unique<DX12Command>();
	pCommand_->Initialize(pCore_->GetDevice(), pLog);

	// コマンドリストの取得
	pCommandList_ = pCommand_->GetCommandList();

	// DX12Heapの生成と初期化
	pHeap_ = std::make_unique<DX12Heap>();
	pHeap_->Initialize(pCore_->GetDevice(), pLog);

	// DX12Bufferingの生成と初期化
	pBuffering_ = std::make_unique<DX12Buffering>();
	pBuffering_->Initialize(pLog, pHeap_.get(), pWinApp_,
		pCore_->GetDXGIFactory(), pCore_->GetDevice(), pCommand_->GetCommandQueue());

	// DX12Fenceの生成と初期化
	pFence_ = std::make_unique<DX12Fence>();
	pFence_->Initialize(pLog, pCore_->GetDevice());

	// シェーダコンパイラの生成と初期化
	pShaderCompiler_ = std::make_unique<ShaderCompiler>();
	pShaderCompiler_->Initialize(pLog);

	// テクスチャストアの生成
	pTextureStore_ = std::make_unique<TextureStore>();

	// DX12Offscreenの生成と初期化
	pOffscreen_ = std::make_unique<DX12Offscreen>();
	pOffscreen_->Initialize(pCore_->GetDevice(), pCommandList_, pHeap_.get(), pBuffering_.get(), pShaderCompiler_.get(), pTextureStore_.get(), pLog);

	// 3Dカメラストア
	pCamera3DStore_ = std::make_unique<Camera3DStore>();
	pCamera3DStore_->Initialize(pCore_->GetDevice(), pLog);

	// 2Dカメラストア
	pCamera2DStore_ = std::make_unique<Camera2DStore>();

	// モデルストアの生成
	pModelStore_ = std::make_unique<ModelStore>();
	pModelStore_->Initilaize(pCore_->GetDevice(), pLog);

	// アニメーションストアの生成
	pAnimationStore_ = std::make_unique<AnimationStore>();

	// スケルトンストアの生成
	pSkeletonStore_ = std::make_unique<SkeletonStore>();

	// ライトストアの生成と初期化
	pLightStore_ = std::make_unique<LightStore>();
	pLightStore_->Initialize(pCore_->GetDevice(), pCommandList_, pHeap_.get(), pShaderCompiler_.get(), pLog);

	// スカイボックスストアの生成と初期化
	pSkyboxStore_ = std::make_unique<SkyboxStore>();
	pSkyboxStore_->Initialize(pCore_->GetDevice(), pCommandList_, pShaderCompiler_.get(), pTextureStore_.get(), pHeap_.get(), pLog);

	// 3D衝突ストアの生成と初期化
	pCollision3DStore_ = std::make_unique<Collision3DStore>();

	// 2D衝突ストアの生成と初期化
	pCollision2DStore_ = std::make_unique<Collision2DStore>();

	// フォントストアの生成と初期化
	pFontStore_ = std::make_unique<FontStore>();

	// DX12Modelの生成と初期化
	pRender_ = std::make_unique<DX12Render>();
	pRender_->Initialize(pCore_->GetDevice(), pShaderCompiler_.get(), pHeap_.get(), pCamera3DStore_.get(),
		pModelStore_.get(), pTextureStore_.get(), pAnimationStore_.get(), pSkeletonStore_.get(), pLightStore_.get(), pLog);

	// DX12Prefabの生成と初期化
	pPrefab_ = std::make_unique<DX12Prefab>();
	pPrefab_->Initialize(pCore_->GetDevice(),pCommandList_, pShaderCompiler_.get(), pHeap_.get(),
		pModelStore_.get(), pTextureStore_.get(), pAnimationStore_.get(), pSkeletonStore_.get(), pLightStore_.get(), pCamera3DStore_.get(), pLog);

	// DX12Particleの生成と初期化
	pParticle_ = std::make_unique<DX12Particle>();
	pParticle_->Initialize(pCore_->GetDevice(), pShaderCompiler_.get(), pHeap_.get(),
		pModelStore_.get(), pTextureStore_.get(), pLog);

	// ビューポートの設定
	viewport_.Width = static_cast<float>(pWinApp_->GetClientWidth());
	viewport_.Height = static_cast<float>(pWinApp_->GetClientHeight());
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	// シザー矩形の設定
	scissorRect_.left = 0;
	scissorRect_.right = pWinApp_->GetClientWidth();
	scissorRect_.top = 0;
	scissorRect_.bottom = pWinApp_->GetClientHeight();

#ifdef DEVELOPMENT

	// DX12Lineの生成と初期化
	pLine_ = std::make_unique<DX12Line>();
	pLine_->Initialize(pCore_->GetDevice(), pHeap_.get(), pShaderCompiler_.get(), pLog);

	// ImGuiの初期設定
	pImguiRender_ = std::make_unique<ImGuiRender>();
	pImguiRender_->Initialize(pCore_->GetDevice(), pWinApp_, pHeap_.get(), pBuffering_.get(), pLog);
#endif

	// 描画前処理のレンダーパスを登録する
	LoadRenderPass("PrevDraw", [&]() 
		{
			// スカイボックスの描画
			pSkyboxStore_->Draw(pCommandList_, pCamera3DStore_->GetCamera3D().GetCurrentVPMatrix());
		}
	);

	// 描画後処理のレンダーパスを登録する
	LoadRenderPass("LastPostDraw", [&]()
		{
			DrawToRenderPass("LastPostDraw", "MainPass");

			// モーションベクトルの描画
			pOffscreen_->DrawMotionVector(pCommandList_, pRender_.get(), pPrefab_.get());

			// モーションブラーの描画
			pOffscreen_->DrawMotionBlur(pCommandList_);

			// 残像の描画
			pOffscreen_->DrawAfterImage(pCommandList_, pCamera3DStore_.get());

			// TAAの描画
			pOffscreen_->DrawTAA(pCommandList_);
		}
	);


#ifdef DEVELOPMENT

	// 線の描画前処理のレンダーパスを登録する
	LoadRenderPass("LineDraw", [&]()
		{
			DrawToRenderPass("LineDraw", "LastPostDraw");

			// 衝突ストアのデバッグ線
			pCollision3DStore_->DebugDrawLine();
			pCollision2DStore_->DebugDrawLine();

			// カメラのデバッグ線
			pCamera3DStore_->DebugDrawLine();

			// ライトのデバッグ線
			pLightStore_->DebugDrawLine();

			// 線の描画
			pLine_->DrawLine3D(pCommandList_, pCamera3DStore_->GetCamera3D().GetCurrentVPUnJitterMatrix());
			pLine_->DrawLine2D(pCommandList_, pCamera2DStore_->GetCamera2D().GetCurrentVPUnJitterMatrix());
			pLine_->DrawTriangle3D(pCommandList_, pCamera3DStore_->GetCamera3D().GetCurrentVPUnJitterMatrix());
		}
	);

#endif
}

/// @brief 全てのインスタンスを削除する
void Engine::RenderContext::PerScene()
{ 
	// カメラのリセット
	pCamera3DStore_->PerSceneReset();

	// 描画のリセット
	pRender_->PerSceneReset();

	// プレハブのリセット
	pPrefab_->PerSceneReset();

	// オフスクリーンのリセット
	pOffscreen_->PerSceneReset();

	// ライトストアのリセット
	pLightStore_->PerSceneReset();

	// コリジョンのインスタンスを削除する
	pCollision3DStore_->DestroyAllInstance(); 
	pCollision2DStore_->DestroyAllInstance(); 
}

/// @brief 新フレーム処理
void Engine::RenderContext::NewFrame()
{
	// リサイズ処理
	if (pWinApp_->IsResized())
		Resize(pWinApp_->GetClientWidth(), pWinApp_->GetClientHeight());

#ifdef DEVELOPMENT
	// フレームの開始をImGuiに伝える
	pImguiRender_->FrameStart();

	// 線のリセット
	pLine_->Reset();
#endif

	// リセット
	pPrefab_->Reset();
	pLightStore_->FrameReset();

	// コマンドリストの取得
	pCommandList_ = pCommand_->GetCommandList();

	// ビューポート、シザー矩形の設定
	pCommandList_->RSSetViewports(1, &viewport_);
	pCommandList_->RSSetScissorRects(1, &scissorRect_);

	// 描画用のディスクリプタヒープを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { pHeap_->GetSrvDescriptorHeap() };
	pCommandList_->SetDescriptorHeaps(1, descriptorHeaps);

	// プレハブの更新
	pPrefab_->Update();

#ifdef DEVELOPMENT
	// Dockスペースを作成する
	pImguiRender_->CreateDockSpace();

	// テクスチャストアのUI
	pTextureStore_->DrawUI();
#endif 
}

/// @brief 描画前処理
void Engine::RenderContext::PreDraw()
{
	bool isHoverViewWindow = false;

	// パラメータやGuizmo操作などのデバッグ表示
#ifdef DEVELOPMENT
	pCamera3DStore_->DebugParameter();
	pRender_->DebugParameter();
	pPrefab_->DebugParameter();
	pOffscreen_->DebugParameter();
	pParticle_->DebugParameter();

	pLightStore_->DebugGuizmo(pCamera3DStore_->GetCamera3D().GetViewMatrix(), pCamera3DStore_->GetCamera3D().GetProjectionMatrix());
	pLightStore_->DebugParameter();

	// ImGuiDockingのビューウィンドウがホバーしているかどうかを取得する
	isHoverViewWindow = pImguiRender_->IsViewWindowHover();
#endif

	// コマンドリストの取得
	pCommandList_ = pCommand_->GetCommandList();

	// モデル全体の更新
	pRender_->Update(pCommandList_);

	// パーティクルの更新
	pParticle_->Update(pCommandList_);

	// カメラストアの更新
	pCamera3DStore_->Update(isHoverViewWindow);
	pCamera2DStore_->Update(isHoverViewWindow);

	// ライトストアの更新
	pLightStore_->Update();

	// 衝突判定
	pCollision3DStore_->Update();
	pCollision2DStore_->Update();

	// シャドウマップ処理
	pLightStore_->ShadowMap(pCommandList_, pRender_.get(), pPrefab_.get(), pCamera3DStore_->GetCamera3D().GetProjectionMatrix());

	// シャドウマップをテクスチャとして使えるようにする
	pLightStore_->GetShadowMapTextureResource()->Barrier(pCommandList_, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// オフスクリーンのクリア
	pOffscreen_->Clear(pCommandList_);

	// 描画前処理のレンダーパスを呼び出す
	ExecuteRenderPass("PrevDraw");
}

/// @brief 描画後処理
void Engine::RenderContext::PostDraw()
{
	// 描画後ポストエフェクトのレンダーパスを呼び出す
	ExecuteRenderPass("LastPostDraw");

#ifdef DEVELOPMENT
	ExecuteRenderPass("LineDraw");
#endif

	// コマンドリスト・アロケータの取得
	pCommandList_ = pCommand_->GetCommandList();
	pCommandAllocator_ = pCommand_->GetCommandAllocator();

	// バックバッファのインデックス・リソース・RTV用CPUハンドルを取得
	UINT backBufferIndex = pBuffering_->GetSwapChain()->GetCurrentBackBufferIndex();
	ID3D12Resource* backBufferResource = pBuffering_->GetSwapChainResource(backBufferIndex);
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferCPUHandle = pBuffering_->GetSwapChainRtvCPUHandle(backBufferIndex);

	// シャドウマップを深度テクスチャに戻す
	pLightStore_->GetShadowMapTextureResource()->Barrier(pCommandList_,D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	// バックバッファリソース Present -> RenderTarget
	TransitionBarrier(backBufferResource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET, pCommandList_);

	// 描画先のRTVを設定する
	pCommandList_->OMSetRenderTargets(1, &backBufferCPUHandle, false, nullptr);

	// 指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f , 0.1f , 0.1f , 1.0f };
	pCommandList_->ClearRenderTargetView(backBufferCPUHandle, clearColor, 0, nullptr);

	// スワップチェインのリソースにオフスクリーンテクスチャを書き込む
	pOffscreen_->RenderSwapChain(pCommandList_);

	// ImGuiDockingに最終的なオフスクリーンを描画する
#ifdef DEVELOPMENT
	// ImGuiに表示するスクリーンを描画する
	pImguiRender_->DrawImGuiScreen(pOffscreen_->GetCurrentResource()->GetResource(), pOffscreen_->GetCurrentResourceSrvHandle(), pCommandList_);
#endif

	// バックバッファリソース RenderTarget -> Present
	TransitionBarrier(backBufferResource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, pCommandList_);

	// オフスクリーンのフレーム終了処理
	pOffscreen_->EndFrame(pCommandList_);

	// コマンドの内容を確定させる（閉じる）
	HRESULT hr = pCommandList_->Close();
	assert(SUCCEEDED(hr));

	// GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { pCommandList_ };
	pCommand_->GetCommandQueue()->ExecuteCommandLists(1, commandLists);

	// GPUとOSに画面の交換を行うよう通知する
	pBuffering_->GetSwapChain()->Present(0, 0);

	// GPUにシグナルを送る
	pFence_->SendSignal(pCommand_->GetCommandQueue());

	// フェンスの値を確認してGPUを待つ
	pFence_->WaitGPU();

	// FPS固定更新処理
	UpdateFixFPS();

	// 次のフレーム用のコマンドリストを準備
	hr = pCommandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = pCommandList_->Reset(pCommandAllocator_, nullptr);
	assert(SUCCEEDED(hr));

	// プレハブをリセット
	pPrefab_->PrefabReset();
}


/// @brief スケルトンを読み込む
/// @param directory 
/// @param fileName 
/// @param pLog 
/// @return 
SkeletonHandle Engine::RenderContext::LoadSkeleton(const std::string& directory, const std::string& fileName, Log* pLog)
{
	// モデルハンドルを取得する
	ModelHandle modelHandle = pModelStore_->Load(directory, fileName, pTextureStore_.get(), pHeap_.get(), pCore_->GetDevice(), pCommandList_, pLog);
	ModelData modelData = pModelStore_->GetModelData(modelHandle);

	// スケルトンハンドルを取得する
	SkeletonHandle skeletonHandle = pSkeletonStore_->Load(directory, fileName, modelData.nodes);
	return skeletonHandle;
}


/// @brief ポストエフェクトを読み込む
/// @param name 
/// @param type 
/// @param pLog 
/// @return 
PostEffectHandle Engine::RenderContext::LoadPostEffect(const std::string& name, PostEffect::Type type, Log* pLog)
{
	// TAAの場合、カメラストアでジッタリングを有効にする
	if(type == PostEffect::Type::TAA)
		pCamera3DStore_->SetEnableJitter(true);

	return pOffscreen_->LoadPostEffect(name, type, pCore_->GetDevice(), pCommandList_, pLog);
}

/// @brief ポストエフェクトを描画する
/// @param hPostEffect 
void Engine::RenderContext::DrawPostEffect(PostEffectHandle hPostEffect)
{
	PostEffectRenderContext context;
	context.camera3DStore = pCamera3DStore_.get();

	return pOffscreen_->DrawPostEffect(hPostEffect, pCommandList_, context);
}

/// @brief ポストエフェクトを描画する
/// @param name 
void Engine::RenderContext::DrawPostEffect(const std::string& name)
{
	PostEffectRenderContext context;
	context.camera3DStore = pCamera3DStore_.get();

	return pOffscreen_->DrawPostEffect(name, pCommandList_, context);
}


/// @brief FPS固定初期化
void Engine::RenderContext::InitializeFixFPS()
{
	// 現在時間を初期化する
	reference_ = std::chrono::steady_clock::now();
}

/// @brief FPS固定更新処理
void Engine::RenderContext::UpdateFixFPS()
{
	// 1/60 秒ぴったりの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));

	// 現在時間を取得する
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	// 1/60 秒経過するまで、スリープせずにループで待つ
	while (std::chrono::steady_clock::now() - reference_ < kMinTime)
	{
#if defined(_MSC_VER)
		__nop();
#endif
	}

	// 現在の時間を記録する
	reference_ = std::chrono::steady_clock::now();
}

/// @brief サイズを作り直す
/// @param width 
/// @param height 
void Engine::RenderContext::Resize(int32_t width, int32_t height)
{
	if (width == 0 || height == 0) return;

	// GPU待機
	pFence_->WaitGPU();

	// スワップチェーンのリサイズ
	pBuffering_->Resize(pCore_->GetDevice(), width, height);

	// オフスクリーン再生成
	pOffscreen_->Resize(pCore_->GetDevice(),pCommandList_, pBuffering_.get());

#ifdef DEVELOPMENT
	// IMGUIのリサイズ
	pImguiRender_->Resize(width, height);
#endif

	// シャドウマップテクスチャのリサイズ
	pLightStore_->Resize(pCore_->GetDevice(), width, height);

	// ビューポートの設定
	viewport_.Width = static_cast<float>(width);
	viewport_.Height = static_cast<float>(height);
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	// シザー矩形の設定
	scissorRect_.left = 0;
	scissorRect_.right = width;
	scissorRect_.top = 0;
	scissorRect_.bottom = height;
}


#ifdef DEVELOPMENT

/// @brief デバッグ用レイピッキング
void Engine::RenderContext::DebugRayPicking()
{
	// imguiのビューウィンドウ内のカーソルの位置を取得する
	Vector2 mouseScreenPos = pImguiRender_->GetViewWindowCursorPos();

	// 正規化デバイス座標系
	float ndcX = (2.0f * mouseScreenPos.x) / static_cast<float>(1280.0f) - 1.0f;
	float ndcY = 1.0f - (2.0f * mouseScreenPos.y) / static_cast<float>(720.0f);

	// 同次クリップ空間
	Vector4 nearClip = { ndcX, ndcY, 0.0f, 1.0f };
	Vector4 farClip = { ndcX, ndcY, 1.0f, 1.0f };

	// 逆ビュープロジェクション行列
	Matrix4x4 invVP =
		(pCamera3DStore_->GetCamera3D().GetViewMatrix() *
			pCamera3DStore_->GetCamera3D().GetProjectionMatrix()).Inverse();

	Vector4 nearWorld = Transform(nearClip, invVP);
	Vector4 farWorld = Transform(farClip, invVP);

	// レイ
	Collision3D::Ray ray;
	ray.start = Vector3(nearWorld.x, nearWorld.y, nearWorld.z);

	ray.diff = Vector3(farWorld.x - nearWorld.x, farWorld.y - nearWorld.y, farWorld.z - nearWorld.z).Normalize();

	// 2D描画の座標に変換する
	Vector2 render2DCoordinatePos = Vector2(mouseScreenPos.x, static_cast<float>(pWinApp_->GetClientHeight()) - mouseScreenPos.y);

	// リストを作成し、判定
	std::vector<std::pair<float, DebugData::DebugGuizmoData*>> pickList;
	pCamera3DStore_->DebugRayPicking(ray, pickList);
	pLightStore_->DebugRayPicking(ray, pickList);

	if (!pickList.empty())
	{
		// もっとも近いリストを選択する
		std::sort(pickList.begin(), pickList.end());

		pickList[0].second->isSelect = true;
	}

}

#endif