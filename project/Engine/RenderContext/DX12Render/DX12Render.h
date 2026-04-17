#pragma once
#include <memory>
#include "Store/Render3DStore/Render3DStore.h"
#include "Store/Render2DStore/Render2DStore.h"
#include "Data/Render3DData/Render3DData.h"

namespace Engine
{
	class ShaderCompiler;
	class Log;
	class BasePSOShadowMap;
	class ModelStore;
	class TextureStore;
	class FontStore;
	class Camera3DStore;
	class SkyboxStore;

	class DX12Render
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param shaderCompiler 
		/// @param heap 
		/// @param modelStore 
		/// @param textureStore 
		/// @param animationStore 
		/// @param skeletonStore 
		/// @param lightStore 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* shaderCompiler, DX12Heap* heap,
			ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, SkeletonStore* skeletonStore, LightStore* lightStore, Log* log);

		/// @brief 更新処理
		/// @param viewProjection 
		void Update(ID3D12GraphicsCommandList* commandList);

		/// @brief リセット
		void Reset();

		/// @brief シーン前のリセット
		void PerSceneReset();

		/// @brief シャドウアップ用描画処理
		/// @param commandList 
		void ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso);

		/// @brief 3D描画のパラメータを取得する
		/// @tparam T 
		/// @param handle 
		/// @return 
		template<typename T>
		T* GetRender3DParam(Render3DHandle handle) { return render3DStore_->GetParam<T>(handle); }

		/// @brief 3D描画のパラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetRender3DParam(const std::string& name) { return render3DStore_->GetParam<T>(name); }

		/// @brief 2D描画のパラメータを取得する
		/// @tparam T 
		/// @param handle 
		/// @return 
		template<typename T>
		T* GetRender2DParam(Render2DHandle handle) { return render2DStore_->GetParam<T>(handle); }

		/// @brief 2D描画のパラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetRender2DParam(const std::string& name) { return render2DStore_->GetParam<T>(name); }

		/// @brief プリミティブを読み込む
		/// @param device 
		/// @param commandList 
		/// @param hModel 
		/// @param hAnimation 
		/// @param hSkeleton 
		/// @param name 
		/// @param type 
		/// @param log 
		/// @return 
		Render3DHandle LoadRender3D(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
			TextureHandle hTexture, ModelHandle hModel, AnimationHandle hAnimation,SkeletonHandle hSkeleton,
			const std::string& name, Render3D::Type type, Log* log)
		{
			return render3DStore_->Load(device, commandList, hTexture, hModel, hAnimation, hSkeleton, name, type, log);
		}

		/// @brief 2D描画読み込み
		/// @param name 
		/// @param type 
		/// @param hTexture 
		/// @param hText 
		/// @param textureStore 
		/// @param fontStore 
		/// @param device 
		/// @param log 
		/// @return 
		Render2DHandle LoadRender2D(const std::string& name, Render2D::Type type, TextureHandle hTexture, TextHandle hText,
			TextureStore* textureStore, FontStore* fontStore, ID3D12Device* device, Log* log)
		{
			return render2DStore_->Load(name,type, hTexture,hText, textureStore,fontStore, device, log);
		}


		/// @brief プリミティブ描画処理
		/// @param cameraStore 
		/// @param skyboxStore 
		/// @param commandList 
		/// @param handle 
		void DrawRender3D(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, Render3DHandle handle)
		{
			render3DStore_->Register(cameraStore,skyboxStore, commandList, handle);
		}

		/// @brief プリミティブ描画処理
		/// @param cameraStore 
		/// @param skyboxStore 
		/// @param commandList 
		/// @param name 
		void DrawRender3D(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, const std::string& name)
		{
			render3DStore_->Register(cameraStore, skyboxStore, commandList, name);
		}

		/// @brief スプライトの描画処理
		/// @param hSprite 
		/// @param viewProjection 
		/// @param commandList 
		void DrawRender2D(Render2DHandle hRender2D, Camera2DStore* cameraStore, ID3D12GraphicsCommandList* commandList)
		{
			render2DStore_->Register(hRender2D, cameraStore, commandList);
		}

		/// @brief スプライトの描画処理
		/// @param name 
		/// @param viewProjection 
		/// @param commandList 
		void DrawRender2D(const std::string& name, Camera2DStore* cameraStore, ID3D12GraphicsCommandList* commandList)
		{
			render2DStore_->Register(name, cameraStore, commandList);
		}

		/// @brief 3D描画の親を設定する
		/// @param handle 
		/// @param parent 
		void SetRender3DParent(Render3DHandle handle, WorldTransform3D* parent) { render3DStore_->SetParent(handle, parent); }

		/// @brief 3D描画の親を設定する
		/// @param name 
		/// @param parent 
		void SetRender3DParent(const std::string& name, WorldTransform3D* parent) { render3DStore_->SetParent(name, parent); }


	public:

		/// @brief デバッグ用パラメータ
		void DebugParameter();

		/// @brief デバッグ用レイピッキング
		/// @param ray 
		/// @param pickList 
		void DebugRayPicking(const Collision3D::Ray& ray, std::vector<std::pair<float, DebugData::DebugGuizmoData*>>& pickList) { render3DStore_->DebugRayPicking(ray, pickList); }

		/// @brief デバッグ用ピッキング
		/// @param point 
		/// @param pickList 
		void DebugPicking(const Vector2& point, std::vector<std::pair<float, DebugData::DebugGuizmoData*>>& pickList) { render2DStore_->DebugPicking(point, pickList); }


		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;


	private:

		/// @brief 3D描画ストア
		std::unique_ptr<Render3DStore> render3DStore_ = nullptr;

		/// @brief 2D描画ストア
		std::unique_ptr<Render2DStore> render2DStore_ = nullptr;
	};
}