#pragma once
#include "DX12Debug/DX12Debug.h"
#include "DX12Core/DX12Core.h"
#include "DX12Command/DX12Command.h"
#include "DX12Heap/DX12Heap.h"
#include "DX12Buffering/DX12Buffering.h"
#include "DX12Fence/DX12Fence.h"
#include "DX12Line/DX12Line.h"
#include "ShaderCompiler/ShaderCompiler.h"
#include "DX12Offscreen/DX12Offscreen.h"
#include "DX12Render/DX12Render.h"
#include "DX12Prefab/DX12Prefab.h"
#include "ImGuiRender/ImGuiRender.h"
#include <chrono>
#include <thread>

#include "Store/Camera3DStore/Camera3DStore.h"
#include "Store/Camera2DStore/Camera2DStore.h"
#include "Store/TextureStore/TextureStore.h"
#include "Store/ModelStore/ModelStore.h"
#include "Store/AnimationStore/AnimationStore.h"
#include "Store/SkeletonStore/SkeletonStore.h"
#include "Store/LightStore/LightStore.h"
#include "Store/Collision2DStore/Collision2DStore.h"
#include "Store/Collision3DStore/Collision3DStore.h"
#include "Store/SkyboxStore/SkyboxStore.h"

#include "Store/FontStore/FontStore.h"

namespace Engine
{
	class Log;

	class RenderContext
	{
	public:

		/// @brief 初期化
		/// @param log 
		void Initialize(WinApp* winApp, Log* log);

		/// @brief 新フレーム処理
		void NewFrame();

		/// @brief 描画前処理
		void PreDraw();

		/// @brief 描画後処理
		void PostDraw();

		/// @brief 3Dカメラ切り替え
		/// @param hCamera 
		void Camera3DSwitch(Camera3DHandle hCamera) { camera3DStore_->Switch(hCamera); }

		/// @brief 3Dカメラ切り替え
		/// @param name 
		void Camera3DSwitch(const std::string& name) { camera3DStore_->Switch(name); }

		/// @brief 2Dカメラ切り替え
		/// @param hCamera 
		void Camera2DSwitch(Camera2DHandle hCamera) { camera2DStore_->Switch(hCamera); }

		/// @brief 2Dカメラ切り替え
		/// @param name 
		void Camera2DSwitch(const std::string& name) { camera2DStore_->Switch(name); }

		/// @brief 3Dカメラを読み込む
		/// @param name 
		/// @return 
		Camera3DHandle LoadCamera3D(const std::string& name) { return camera3DStore_->Load(name); }

		/// @brief 2Dカメラを読み込む
		/// @param name 
		/// @return 
		Camera2DHandle LoadCamera2D(const std::string& name) { return camera2DStore_->Load(name); }

		/// @brief テクスチャを読み込む
		/// @param filePath 
		/// @param log 
		TextureHandle LoadTexture(const std::string& filePath, Log* log) { return textureStore_->Load(filePath, heap_.get(), core_->GetDevice(), command_->GetCommandList(), log); }

		/// @brief モデルを読み込む
		/// @param directory 
		/// @param fileName 
		/// @param log 
		/// @return 
		ModelHandle LoadModel(const std::string& directory, const std::string& fileName, Log* log) { return modelStore_->Load(directory, fileName, textureStore_.get(), heap_.get(), core_->GetDevice(), commandList_, log); }

		/// @brief アニメーションを読み込む
		/// @param directory 
		/// @param fileName 
		/// @return 
		AnimationHandle LoadAnimation(const std::string& directory, const std::string& fileName) { return animationStore_->Load(directory, fileName); }

		/// @brief スケルトンを読み込む
		/// @param directory 
		/// @param fileName 
		/// @param log 
		/// @return 
		SkeletonHandle LoadSkeleton(const std::string& directory, const std::string& fileName, Log* log);

		/// @brief ライトを読み込む
		/// @param name 
		/// @param type 
		/// @param log 
		/// @return 
		LightHandle LoadLight(const std::string& name, Light::Type type) { return lightStore_->Load(name, type); }

		/// @brief フォントを読み込む
		/// @param text 
		/// @param fontName 
		/// @param pixel 
		/// @param log 
		/// @return 
		TextHandle LoadFont(const std::string& text, const std::string& fontName, int pixel, Log* log) { return fontStore_->Load(text, fontName, pixel, core_->GetDevice(), commandList_, heap_.get(), log); }



		/// @brief 3Dカメラのパラメータを取得する
		/// @param hCamera 
		/// @return 
		Camera3DData::Param* GetCamera3DParam(Camera3DHandle hCamera)const { return camera3DStore_->GetParam(hCamera); }

		/// @brief 3Dカメラのパラメータを取得する
		/// @param name 
		/// @return 
		Camera3DData::Param* GetCamera3DParam(const std::string& name)const { return camera3DStore_->GetParam(name); }

		/// @brief 2Dカメラのパラメータを取得する
		/// @param hCamera 
		/// @return 
		Camera2DData::Param* GetCamera2DParam(Camera2DHandle hCamera)const { return camera2DStore_->GetParam(hCamera); }

		/// @brief 2Dカメラのパラメータを取得する
		/// @param name 
		/// @return 
		Camera2DData::Param* GetCamera2DParam(const std::string& name)const { return camera2DStore_->GetParam(name); }



		/// @brief ライトのパラメータを取得する
		/// @tparam T 
		/// @param handle 
		/// @return 
		template <typename T>
		T* GetLightParam(LightHandle handle) { return lightStore_->GetParam<T>(handle); }

		/// @brief ライトのパラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetLightParam(const std::string& name) { return lightStore_->GetParam<T>(name); }


		/// @brief ライトを設置する
		/// @param hLight 
		void SetLight(LightHandle hLight) { lightStore_->Set(hLight); }

		/// @brief ライトを設置する
		/// @param name 
		void SetLight(const std::string& name) { lightStore_->Set(name); }



		/// @brief プリミティブのパラメータを取得する
		/// @tparam T 
		/// @param handle 
		/// @return 
		template<typename T>
		T* GetRender3DParam(Render3DHandle handle) { return render_->GetRender3DParam<T>(handle); }

		/// @brief プリミティブのパラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetRender3DParam(const std::string& name) { return render_->GetRender3DParam<T>(name); }

		/// @brief 2D描画のパラメータを取得する
		/// @tparam T 
		/// @param handle 
		/// @return 
		template<typename T>
		T* GetRender2DParam(Render2DHandle handle) { return render_->GetRender2DParam<T>(handle); }

		/// @brief 2D描画のパラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetRender2DParam(const std::string& name) { return render_->GetRender2DParam<T>(name); }



		/// @brief プリミティブ読み込み
		/// @param hModel 
		/// @param name 
		/// @param type 
		/// @param log 
		/// @return 
		Render3DHandle LoadRender3D(ModelHandle hModel, AnimationHandle hAnimation,SkeletonHandle hSkeleton, const std::string& name, Render3D::Type type, Log* log)
		{
			return render_->LoadRender3D(core_->GetDevice(), commandList_, hModel, hAnimation, hSkeleton, name, type, log);
		}

		/// @brief スプライト読み込み
		/// @param hTexture 
		/// @param name 
		/// @param log 
		/// @return 
		Render2DHandle LoadRender2D(TextureHandle hTexture, const std::string& name, Log* log)
		{
			return render_->LoadRender2D(textureStore_.get(), core_->GetDevice(), hTexture, name, log);
		}

		/// @brief プリミティブ用プレハブを読み込む
		/// @param name 
		/// @param type 
		/// @param numInstance 
		/// @param hModel 
		/// @param hAnimation 
		/// @param hSkeleton 
		/// @param log 
		/// @return 
		Prefab3DHandle LoadPrefabPrimitive(const std::string& name, Prefab3D::Type type,uint32_t numInstance, 
			TextureHandle hTexture, ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton, Log* log)
		{
			return prefab_->LoadPrimitive(name, type, numInstance,hTexture, hModel, hAnimation, hSkeleton, core_->GetDevice(), commandList_, log);
		}

		/// @brief スプライト用プレハブを読み込む
		/// @param name 
		/// @param hTexture 
		/// @param numInstance 
		/// @param log 
		/// @return 
		Prefab2DHandle LoadPrefabSprite(const std::string& name,TextureHandle hTexture, uint32_t numInstance, Log* log)
		{
			return prefab_->LoadSprite(name, hTexture, numInstance, textureStore_.get(),camera2DStore_.get(), heap_.get(), core_->GetDevice(), log);
		}


		/// @brief プリミティブ用プレハブのパラメータを取得する
		/// @tparam T 
		/// @param hPrefabPrimitive 
		/// @return 
		template<typename T>
		T* GetPrefab3DParam(Prefab3DHandle hPrefab3D) { return prefab_->GetPrefab3DParam<T>(hPrefab3D); }

		/// @brief プリミティブ用プレハブのパラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetPrefab3DParam(const std::string& name) { return prefab_->GetPrefab3DParam(name); }

		/// @brief スプライト用プレハブのパラメータを取得する
		/// @return 
		Prefab2D::Sprite::Base::Param* GetPrefab2DParam(Prefab2DHandle hPrefab2D) { return prefab_->GetPrefab2DParam(hPrefab2D); }

		/// @brief スプライト用プレハブのパラメータを取得する
		/// @param name 
		/// @return 
		Prefab2D::Sprite::Base::Param* GetPrefab2DParam(const std::string& name) { return prefab_->GetPrefab2DParam(name); }


		/// @brief プリミティブ用インスタンスを作成する
		/// @tparam T 
		/// @param hPrefabPrimitive 
		/// @return 
		template<typename T>
		T* CreatePrefab3DInstance(Prefab3DHandle hPrefab3D) { return prefab_->CreatePrefab3DInstance<T>(hPrefab3D); }

		/// @brief プリミティブ用インスタンスを作成する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* CreatePrefab3DInstance(const std::string& name) { return prefab_->CreatePrefab3DInstance<T>(name); }

		/// @brief スプライト用インスタンスを作成する
		/// @param hPrefabSprite 
		/// @return 
		PrefabInstanceSprite* CreatePrefab2DInstance(Prefab2DHandle hPrefab2D) { return prefab_->CreatePrefab2DInstance(hPrefab2D); }

		/// @brief スプライト用インスタンスを作成する
		/// @param name 
		/// @return 
		PrefabInstanceSprite* CreatePrefab2DInstance(const std::string& name) { return prefab_->CreatePrefab2DInstance(name); }


		/// @brief 全てのインスタンスを削除する
		void DestroyAllInstance() { prefab_->DestroyAllInstance(); collision3DStore_->DestroyAllInstance(); collision2DStore_->DestroyAllInstance(); }




	public:

		/// @brief プリミティブの描画処理
		/// @param handle 
		void DrawRender3D(Render3DHandle handle)
		{
			render_->DrawRender3D(camera3DStore_.get(),skyboxStore_.get(), commandList_, handle);
		}

		/// @brief プリミティブの描画処理
		/// @param name 
		void DrawRender3D(const std::string& name)
		{
			render_->DrawRender3D(camera3DStore_.get(), skyboxStore_.get(), commandList_, name);
		}

		/// @brief スプライトの描画処理
		/// @param handle 
		void DrawRender2D(Render2DHandle handle)
		{
			render_->DrawRender2D(handle, camera2DStore_->GetCamera2D().GetViewProjectionMatrix(), commandList_);
		}

		/// @brief スプライトの描画処理
		/// @param name 
		void DrawRender2D(const std::string& name)
		{
			render_->DrawRender2D(name, camera2DStore_->GetCamera2D().GetViewProjectionMatrix(), commandList_);
		}



		/// @brief ポストエフェクトを読み込む
		/// @param name 
		/// @param type 
		/// @param log 
		/// @return 
		PostEffectHandle LoadPostEffect(const std::string& name, PostEffect::Type type, Log* log) { return offscreen_->LoadPostEffect(name, type, core_->GetDevice(), log); }

		/// @brief ポストエフェクトを描画する
		/// @param hPostEffect 
		void DrawPostEffect(PostEffectHandle hPostEffect) { return offscreen_->DrawPostEffect(hPostEffect, commandList_); }

		/// @brief パメータを取得する
		/// @tparam T 
		/// @param hPostEffect 
		/// @return 
		template<typename T>
		T* GetPostEffectParam(PostEffectHandle hPostEffect) { return offscreen_->GetPostEffectParam<T>(hPostEffect); }



		/// @brief 3D衝突読み込み
		/// @param name 
		/// @param type 
		/// @return 
		Collision3DHandle LoadCollision3D(const std::string& name, Collision3D::Type type) { return collision3DStore_->Load(name, type); }

		/// @brief インスタンスを作成する
		/// @tparam T 
		/// @param hCollision 
		/// @return 
		template<typename T>
		T* CreateCollision3DInstance(Collision3DHandle hCollision) { return collision3DStore_->CreateInstance<T>(hCollision); }

		/// @brief インスタンスを作成する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* CreateCollision3DInstance(const std::string& name) { return collision3DStore_->CreateInstance<T>(name); }

		/// @brief 衝突対象の設定
		/// @param hCollision 
		/// @param hTargetCollision 
		void SetCollision3DTarget(Collision3DHandle hCollision, Collision3DHandle hTargetCollision) { collision3DStore_->SetCollision3DTarget(hCollision, hTargetCollision); }

		/// @brief 衝突対象の設定
		/// @param name 
		/// @param targetName 
		void SetCollision3DTarget(const std::string& name, const std::string& targetName) { collision3DStore_->SetCollision3DTarget(name, targetName); }



		/// @brief 2D衝突読み込み
		/// @param name 
		/// @param type 
		/// @return 
		Collision2DHandle LoadCollision2D(const std::string& name, Collision2D::Type type) { return collision2DStore_->Load(name, type); }

		/// @brief インスタンスを作成する
		/// @tparam T 
		/// @param hCollision 
		/// @return 
		template<typename T>
		T* CreateCollision2DInstance(Collision2DHandle hCollision) { return collision2DStore_->CreateInstance<T>(hCollision); }

		/// @brief インスタンスを作成する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* CreateCollision2DInstance(const std::string& name) { return collision2DStore_->CreateInstance<T>(name); }

		/// @brief 衝突対象の設定
		/// @param hCollision 
		/// @param hTargetCollision 
		void SetCollision2DTarget(Collision2DHandle hCollision, Collision2DHandle hTargetCollision) { collision2DStore_->SetCollision2DTarget(hCollision, hTargetCollision); }

		/// @brief 衝突対象の設定
		/// @param name 
		/// @param targetName 
		void SetCollision2DTarget(const std::string& name, const std::string& targetName) { collision2DStore_->SetCollision2DTarget(name, targetName); }



#ifdef _DEVELOPMENT

		/// @brief デバッグ用の線を描画する
		/// @param start 
		/// @param end 
		/// @param color 
		void DrawDebugLine3D(const Vector3& start, const Vector3& end, const Vector4& color)
		{
			line_->DrawCallLine3D(start, end, color);
		}

		/// @brief デバッグ用の線を描画する
		/// @param start 
		/// @param end 
		/// @param color 
		void DrawDebugLine2D(const Vector2& start, const Vector2& end, const Vector4& color)
		{
			line_->DrawCallLine2D(start, end, color);
		}

		/// @brief デバッグ用レイピッキング
		/// @param mouseScreenPos マウスのスクリーン座標
		void DebugRayPicking(const Vector2& mouseScreenPos);

		/// @brief デバッグ用立方体の描画
		/// @param position 
		/// @param rotate 
		/// @param scale 
		/// @param color 
		void DrawDebugCube(const Vector3& position, const Vector3& rotate, const Vector3& scale, const Vector4& color)
		{
			prefab_->DrawDebugCube(position, rotate, scale, color);
		}

#endif


	private:

#ifdef _DEBUG
		// DX12Debug
		std::unique_ptr<DX12Debug> debug_ = nullptr;
#endif

#ifdef _DEVELOPMENT
		/// @brief DX12Line
		std::unique_ptr<DX12Line> line_ = nullptr;
#endif

		// DX12Core
		std::unique_ptr<DX12Core> core_ = nullptr;

		// DX12Command
		std::unique_ptr<DX12Command> command_ = nullptr;

		// DX12Heap
		std::unique_ptr<DX12Heap> heap_ = nullptr;

		// DX12Buffering
		std::unique_ptr<DX12Buffering> buffering_ = nullptr;

		// DX12Fence
		std::unique_ptr<DX12Fence> fence_ = nullptr;

		// シェーダコンパイラ
		std::unique_ptr<ShaderCompiler> shaderCompiler_ = nullptr;

		// DX12Offscreen
		std::unique_ptr<DX12Offscreen> offscreen_ = nullptr;

		// DX12Model
		std::unique_ptr<DX12Render> render_ = nullptr;

		// DX12Prefab
		std::unique_ptr<DX12Prefab> prefab_ = nullptr;


	private:

		/// @brief FPS固定初期化
		void InitializeFixFPS();

		/// @brief FPS固定更新処理
		void UpdateFixFPS();

		/// @brief 記録時間（FPS固定用）
		std::chrono::steady_clock::time_point reference_;


	private:

		// 3Dカメラストア
		std::unique_ptr<Camera3DStore> camera3DStore_ = nullptr;

		// 2Dカメラストア
		std::unique_ptr<Camera2DStore> camera2DStore_ = nullptr;

		// テクスチャストア
		std::unique_ptr<TextureStore> textureStore_ = nullptr;

		// モデルストア
		std::unique_ptr<ModelStore> modelStore_ = nullptr;

		/// @brief アニメーションストア
		std::unique_ptr<AnimationStore> animationStore_ = nullptr;

		/// @brief スケルトンストア
		std::unique_ptr<SkeletonStore> skeletonStore_ = nullptr;

		/// @brief ライトストア
		std::unique_ptr<LightStore> lightStore_ = nullptr;

		/// @brief 2D衝突ストア
		std::unique_ptr<Collision2DStore> collision2DStore_ = nullptr;

		// 3D衝突ストア
		std::unique_ptr<Collision3DStore> collision3DStore_ = nullptr;

		/// @brief スカイボックスストア
		std::unique_ptr<SkyboxStore> skyboxStore_ = nullptr;

		/// @brief フォントストア
		std::unique_ptr<FontStore> fontStore_ = nullptr;


	private:

		/// @brief ビューポート
		D3D12_VIEWPORT viewport_{};

		/// @brief シザー矩形
		D3D12_RECT scissorRect_{};


	private:

		// コマンドリスト
		ID3D12GraphicsCommandList* commandList_ = nullptr;

		// コマンドアロケータ
		ID3D12CommandAllocator* commandAllocator_ = nullptr;


	private:

		// ImGui用SRVハンドル
#ifdef _DEVELOPMENT
		std::unique_ptr<ImGuiRender> imguiRender_ = nullptr;
#endif

	private:

		/// @brief サイズを作り直す
		/// @param width 
		/// @param height 
		void Resize(int32_t width, int32_t height);

		// ウィンドウアプリケーション
		WinApp* winApp_ = nullptr;

	};
}