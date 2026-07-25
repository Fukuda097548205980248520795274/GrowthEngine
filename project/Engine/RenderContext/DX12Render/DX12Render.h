#pragma once
#include <memory>
#include "Store/Render3DStore/Render3DStore.h"
#include "Store/Render2DStore/Render2DStore.h"
#include "Store/TrailStore/TrailStore.h"
#include "Data/Render3DData/Render3DData.h"

namespace Engine
{
	class ShaderCompiler;
	class Log;
	class BasePSOShadowMap;
	class ModelStore;
	class TextureStore;
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
		void Initialize(ID3D12Device* device, ShaderCompiler* shaderCompiler, DX12Heap* heap, Camera3DStore* cameraStore,
			ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, SkeletonStore* skeletonStore, LightStore* lightStore, Log* log);

		/// @brief 更新処理
		/// @param viewProjection 
		void Update(ID3D12GraphicsCommandList* commandList);

		/// @brief シーン前のリセット
		void PerSceneReset();

		/// @brief シャドウアップ用描画処理
		/// @param commandList 
		void ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso);

		/// @brief モーションベクター用描画処理
		/// @param commandList 
		/// @param pso 
		void DrawMotionVector(ID3D12GraphicsCommandList* commandList, BasePSOMotionVector* pso);

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

		/// @brief トレイルのパラメータを取得する
		/// @param handle 
		/// @return 
		TrailData::Param* GetTrailParam(TrailHandle handle) { return trailStore_->GetParam(handle); }

		/// @brief トレイルのパラメータを取得する
		/// @param name 
		/// @return 
		TrailData::Param* GetTrailParam(const std::string& name) { return trailStore_->GetParam(name); }

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
		/// @param device 
		/// @param log 
		/// @return 
		Render2DHandle LoadRender2D(const std::string& name, Render2D::Type type, TextureHandle hTexture, TextHandle hText,
			TextureStore* textureStore, ID3D12Device* device, Log* log)
		{
			return render2DStore_->Load(name,type, hTexture,hText, textureStore,device, log);
		}

		/// @brief トレイルの読み込み
		/// @param name 
		/// @param maxLifetime 
		/// @param hTexture 
		/// @param device 
		/// @param log 
		/// @return 
		TrailHandle LoadTrail(const std::string& name,float maxLifetime, TextureHandle hTexture, ID3D12Device* device, Log* log)
		{
			return trailStore_->Load(name, maxLifetime, hTexture, device, log);
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

		/// @brief トレイルの描画処理
		/// @param hTrail 
		/// @param commandList 
		void DrawTrail(TrailHandle hTrail, ID3D12GraphicsCommandList* commandList)
		{
			trailStore_->Draw(hTrail, commandList);
		}

		/// @brief トレイルの描画処理
		/// @param name 
		/// @param commandList 
		void DrawTrail(const std::string& name, ID3D12GraphicsCommandList* commandList)
		{
			trailStore_->Draw(name, commandList);
		}

		/// @brief 3D描画の親を設定する
		/// @param handle 
		/// @param parent 
		void SetRender3DParent(Render3DHandle handle, WorldTransform3D* parent) { render3DStore_->SetParent(handle, parent); }

		/// @brief 3D描画の親を設定する
		/// @param name 
		/// @param parent 
		void SetRender3DParent(const std::string& name, WorldTransform3D* parent) { render3DStore_->SetParent(name, parent); }

		/// @brief 2D描画の親を設定する
		/// @param handle 
		/// @param parent 
		void SetRender2DParent(Render2DHandle handle, WorldTransform2D* parent) { render2DStore_->SetParent(handle, parent); }

		/// @brief 2D描画の親を設定する
		/// @param name 
		/// @param parent 
		void SetRender2DParent(const std::string& name, WorldTransform2D* parent) { render2DStore_->SetParent(name, parent); }

		/// @brief ボーンのワールド行列を取得する
		/// @param handle 
		/// @param boneName 
		/// @return 
		Matrix4x4 GetBoneWorldMatrix(Render3DHandle handle, const std::string& boneName) { return render3DStore_->GetBoneWorldMatrix(handle, boneName); }

		/// @brief ボーンのワールド行列を取得する
		/// @param name 
		/// @param boneName 
		/// @return 
		Matrix4x4 GetBoneWorldMatrix(const std::string& name, const std::string& boneName) { return render3DStore_->GetBoneWorldMatrix(name, boneName); }

		/// @brief トレイルの履歴を消す
		/// @param hTrail 
		void Trail3DClear(TrailHandle hTrail) { trailStore_->Clear(hTrail); }

		/// @brief トレイルの履歴を消す
		/// @param name 
		void Trail3DClear(const std::string& name) { trailStore_->Clear(name); }


		/// @brief アウトライン用描画処理
		/// @param commandList 
		/// @param pso 
		void DrawOutline(ID3D12GraphicsCommandList* commandList, BasePSOOutline* pso) { render3DStore_->DrawOutline(commandList, pso); }


	public:

		/// @brief デバッグ用パラメータ
		void DebugParameter();


		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;


	private:

		/// @brief 3D描画ストア
		std::unique_ptr<Render3DStore> render3DStore_ = nullptr;

		/// @brief 2D描画ストア
		std::unique_ptr<Render2DStore> render2DStore_ = nullptr;

		/// @brief トレイルストア
		std::unique_ptr<TrailStore> trailStore_ = nullptr;
	};
}