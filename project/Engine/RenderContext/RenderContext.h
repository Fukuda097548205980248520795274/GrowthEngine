#pragma once
#include "DX12Debug/DX12Debug.h"
#include "DX12Core/DX12Core.h"
#include "DX12Command/DX12Command.h"
#include "DX12Heap/DX12Heap.h"
#include "DX12Buffering/DX12Buffering.h"
#include "DX12Fence/DX12Fence.h"
#include "ShaderCompiler/ShaderCompiler.h"
#include "DX12Offscreen/DX12Offscreen.h"
#include "DX12Primitive/DX12Primitive.h"
#include "ImGuiRender/ImGuiRender.h"
#include <chrono>
#include <thread>

#include "Store/CameraStore/CameraStore.h"
#include "Store/TextureStore/TextureStore.h"
#include "Store/ModelStore/ModelStore.h"
#include "Store/AnimationStore/AnimationStore.h"
#include "Store/SkeletonStore/SkeletonStore.h"
#include "Store/LightStore/LightStore.h"

class GameCamera;

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

		/// @brief カメラ切り替え
		/// @param hCamera 
		void CameraSwitch(CameraHandle hCamera) { cameraStore_->Switch(hCamera); }

		/// @brief カメラを読み込む
		/// @param gameCamera 
		/// @param name 
		/// @return 
		CameraHandle LoadCamera(GameCamera* gameCamera, const std::string& name) { return cameraStore_->Load(gameCamera, name); }

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
		T* GetPrimitiveParam(PrimitiveHandle handle) { return primitive_->GetParam<T>(handle); }



		/// @brief プリミティブ読み込み
		/// @param hModel 
		/// @param name 
		/// @param type 
		/// @param log 
		/// @return 
		PrimitiveHandle LoadPrimitive(ModelHandle hModel, AnimationHandle hAnimation, const std::string& name, const std::string& type, Log* log)
		{
			return primitive_->Load(modelStore_.get(), textureStore_.get(), animationStore_.get(), core_->GetDevice(), hModel, hAnimation, name, type, log);
		}




	public:

		/// @brief プリミティブモデルの描画処理
		/// @param commandList 
		/// @param handle 
		void DrawPrimitiveModel(PrimitiveHandle handle)
		{
			primitive_->DrawModel(cameraStore_->GetCamera3D().GetViewProjectionMatrix(), commandList_, handle, lightStore_.get());
		}


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

		// DX12Primitive
		std::unique_ptr<DX12Primitive> primitive_ = nullptr;


	private:

		/// @brief FPS固定初期化
		void InitializeFixFPS();

		/// @brief FPS固定更新処理
		void UpdateFixFPS();

		/// @brief 記録時間（FPS固定用）
		std::chrono::steady_clock::time_point reference_;


	private:

		// カメラストア
		std::unique_ptr<CameraStore> cameraStore_ = nullptr;

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