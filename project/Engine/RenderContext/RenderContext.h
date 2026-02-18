#pragma once
#include "DX12Debug/DX12Debug.h"
#include "DX12Core/DX12Core.h"
#include "DX12Command/DX12Command.h"
#include "DX12Heap/DX12Heap.h"
#include "DX12Buffering/DX12Buffering.h"
#include "DX12Fence/DX12Fence.h"
#include "ShaderCompiler/ShaderCompiler.h"
#include "DX12Offscreen/DX12Offscreen.h"
#include "DX12Model/DX12Model.h"
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

namespace Engine
{
	class Log;

	class RenderContext
	{
	public:

		/// @brief 初期化
		/// @param log 
		void Initialize(WinApp* winApp, Log* log);

		/// @brief 描画前処理
		void PreDraw();

		/// @brief 描画後処理
		void PostDraw();

		/// @brief 3Dカメラ切り替え
		/// @param hCamera 
		void Camera3DSwitch(Camera3DHandle hCamera) { camera3DStore_->Switch(hCamera); }

		/// @brief 2Dカメラ切り替え
		/// @param hCamera 
		void Camera2DSwitch(Camera2DHandle hCamera) { camera2DStore_->Switch(hCamera); }

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
		LightHandle LoadLight(const std::string& name, const std::string& type, Log* log) { return lightStore_->Load(name, type, heap_.get(), core_->GetDevice(), log); }


		/// @brief ライトのパラメータを取得する
		/// @tparam T 
		/// @param handle 
		/// @return 
		template <typename T>
		T* GetLightParam(LightHandle handle) { return lightStore_->GetParam<T>(handle); }

		/// @brief プリミティブのパラメータを取得する
		/// @tparam T 
		/// @param handle 
		/// @return 
		template<typename T>
		T* GetPrimitiveParam(PrimitiveHandle handle) { return model_->GetParam<T>(handle); }



		/// @brief プリミティブ読み込み
		/// @param hModel 
		/// @param name 
		/// @param type 
		/// @param log 
		/// @return 
		PrimitiveHandle LoadPrimitive(ModelHandle hModel, AnimationHandle hAnimation,SkeletonHandle hSkeleton, const std::string& name, Primitive::Type type, Log* log)
		{
			return model_->LoadPrimitive(modelStore_.get(), textureStore_.get(), animationStore_.get(), skeletonStore_.get(),
				core_->GetDevice(),commandList_, hModel, hAnimation, hSkeleton, heap_.get(), name, type, log);
		}

		/// @brief スプライト読み込み
		/// @param hTexture 
		/// @param name 
		/// @param log 
		/// @return 
		SpriteHandle LoadSprite(TextureHandle hTexture, const std::string& name, Log* log)
		{
			return model_->LoadSprite(textureStore_.get(), core_->GetDevice(), hTexture, name, log);
		}

		/// @brief プレハブスプライトを読み込む
		/// @param name 
		/// @param numInstance 
		/// @param log 
		/// @return 
		PrefabSpriteHandle LoadPrefabSprite(const std::string& name,TextureHandle hTexture, uint32_t numInstance, Log* log)
		{
			return prefab_->LoadSprite(name, hTexture, numInstance, textureStore_.get(),camera2DStore_.get(), heap_.get(), core_->GetDevice(), log);
		}


		/// @brief スプライトのドローコール関数を取得する
		/// @return 
		std::function<void(const Prefab::Sprite::Instance::Param*)> GetSpriteDrawCall(PrefabSpriteHandle hPrefabSprite)
		{
			return prefab_->GetSpriteDrawCall(hPrefabSprite);
		}

		/// @brief コマンドリストに登録する
		/// @param hSprite 
		/// @param commandList 
		void DrawPrefabSprite(PrefabSpriteHandle hPrefabSprite) { prefab_->DrawPrefabSprite(hPrefabSprite, commandList_); }

		/// @brief スプライト用プレハブのパラメータを取得する
		/// @return 
		Prefab::Sprite::Base::Param* GetPrefabSpriteParam(PrefabSpriteHandle hPrefabSprite) { return prefab_->GetSpriteParam(hPrefabSprite); }




	public:

		/// @brief プリミティブモデルの描画処理
		/// @param handle 
		void DrawPrimitive(PrimitiveHandle handle)
		{
			model_->DrawPrimitive(camera3DStore_->GetCamera3D().GetViewProjectionMatrix(), commandList_, handle, lightStore_.get());
		}

		/// @brief スプライトの描画処理
		/// @param handle 
		void DrawSprite(SpriteHandle handle)
		{
			model_->DrawSprite(handle, camera2DStore_->GetCamera2D().GetViewProjectionMatrix(), commandList_);
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


	private:

#ifdef _DEBUG
		// DX12Debug
		std::unique_ptr<DX12Debug> debug_ = nullptr;
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
		std::unique_ptr<DX12Model> model_ = nullptr;

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
	};
}