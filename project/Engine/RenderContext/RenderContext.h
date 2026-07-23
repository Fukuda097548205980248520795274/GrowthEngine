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
#include "DX12Particle/DX12Particle.h"
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
		/// @param pLog 
		void Initialize(WinApp* pWinApp, Log* pLog);

		/// @brief シーン前処理
		void PerScene();

		/// @brief 新フレーム処理
		void NewFrame();

		/// @brief 描画前処理
		void PreDraw();

		/// @brief 描画後処理
		void PostDraw();

		/// @brief 3Dカメラ切り替え
		/// @param hCamera 
		void Camera3DSwitch(Camera3DHandle hCamera) { pCamera3DStore_->Switch(hCamera); }

		/// @brief 3Dカメラ切り替え
		/// @param name 
		void Camera3DSwitch(const std::string& name) { pCamera3DStore_->Switch(name); }

		/// @brief 2Dカメラ切り替え
		/// @param hCamera 
		void Camera2DSwitch(Camera2DHandle hCamera) { pCamera2DStore_->Switch(hCamera); }

		/// @brief 2Dカメラ切り替え
		/// @param name 
		void Camera2DSwitch(const std::string& name) { pCamera2DStore_->Switch(name); }

		/// @brief 3Dカメラを読み込む
		/// @param name 
		/// @return 
		Camera3DHandle LoadCamera3D(const std::string& name) { return pCamera3DStore_->Load(name); }

		/// @brief 2Dカメラを読み込む
		/// @param name 
		/// @return 
		Camera2DHandle LoadCamera2D(const std::string& name) { return pCamera2DStore_->Load(name); }

		/// @brief テクスチャを読み込む
		/// @param filePath 
		/// @param pLog 
		TextureHandle LoadTexture(const std::string& filePath, Log* pLog) { return pTextureStore_->Load(filePath, pHeap_.get(), pCore_->GetDevice(), pCommand_->GetCommandList(), pLog); }

		/// @brief テクスチャのSRVのGPUハンドルを取得する
		/// @param handle 
		/// @return 
		D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvGpuHandle(TextureHandle handle) { return pTextureStore_->GetSrvGpuHandle(handle); }

		/// @brief モデルを読み込む
		/// @param directory 
		/// @param fileName 
		/// @param pLog 
		/// @return 
		ModelHandle LoadModel(const std::string& directory, const std::string& fileName, Log* pLog) { return pModelStore_->Load(directory, fileName, pTextureStore_.get(), pHeap_.get(), pCore_->GetDevice(), pCommandList_, pLog); }

		/// @brief アニメーションを読み込む
		/// @param directory 
		/// @param fileName 
		/// @return 
		AnimationHandle LoadAnimation(const std::string& directory, const std::string& fileName) { return pAnimationStore_->Load(directory, fileName); }

		/// @brief スケルトンを読み込む
		/// @param directory 
		/// @param fileName 
		/// @param pLog 
		/// @return 
		SkeletonHandle LoadSkeleton(const std::string& directory, const std::string& fileName, Log* pLog);

		/// @brief ライトを読み込む
		/// @param name 
		/// @param type 
		/// @param pLog 
		/// @return 
		LightHandle LoadLight(const std::string& name, Light::Type type) { return pLightStore_->Load(name, type); }

		/// @brief フォントを読み込む
		/// @param text 
		/// @param fontName 
		/// @param pixel 
		/// @param pLog 
		/// @return 
		TextHandle LoadFont(const std::string& text, const std::string& fontName, int pixel, Log* pLog) { return pFontStore_->Load(text, fontName, pixel, pCore_->GetDevice(), pCommandList_, pHeap_.get(), pLog); }



		/// @brief 3Dカメラのパラメータを取得する
		/// @param hCamera 
		/// @return 
		Camera3DData::Param* GetCamera3DParam(Camera3DHandle hCamera)const { return pCamera3DStore_->GetParam(hCamera); }

		/// @brief 3Dカメラのパラメータを取得する
		/// @param name 
		/// @return 
		Camera3DData::Param* GetCamera3DParam(const std::string& name)const { return pCamera3DStore_->GetParam(name); }

		/// @brief 3Dカメラのパラメータを取得する
		/// @return 
		Camera3DData::Param* GetCamera3DParam()const { return pCamera3DStore_->GetSelectParam(); }

		/// @brief 2Dカメラのパラメータを取得する
		/// @param hCamera 
		/// @return 
		Camera2DData::Param* GetCamera2DParam(Camera2DHandle hCamera)const { return pCamera2DStore_->GetParam(hCamera); }

		/// @brief 2Dカメラのパラメータを取得する
		/// @param name 
		/// @return 
		Camera2DData::Param* GetCamera2DParam(const std::string& name)const { return pCamera2DStore_->GetParam(name); }

		/// @brief 2Dカメラのパラメータを取得する
		/// @return 
		Camera2DData::Param* GetCamera2DParam()const { return pCamera2DStore_->GetCamera2D().GetParam(); }


		/// @brief 3Dカメラのビュー行列を取得する
		/// @return 
		Matrix4x4 GetCamera3DView()const { return pCamera3DStore_->GetCamera3D().GetViewMatrix(); }

		/// @brief 3Dカメラのプロジェクション行列を取得する
		/// @return 
		Matrix4x4 GetCamera3DProjection()const { return pCamera3DStore_->GetCamera3D().GetProjectionMatrix(); }

		/// @brief 3Dカメラのビュー正射影行列を取得する
		/// @return 
		Matrix4x4 GetCamera3DViewProjection()const { return pCamera3DStore_->GetCamera3D().GetCurrentVPUnJitterMatrix(); }



		/// @brief 2Dカメラのビュー行列を取得する
		/// @return 
		Matrix4x4 GetCamera2DView()const { return pCamera2DStore_->GetCamera2D().GetViewMatrix(); }

		/// @brief 2Dカメラのプロジェクション行列を取得する
		/// @return 
		Matrix4x4 GetCamera2DProjection()const { return pCamera2DStore_->GetCamera2D().GetProjectionMatrix(); }

		/// @brief 2Dカメラのビュー正射影行列を取得する
		/// @return 
		Matrix4x4 GetCamera2DViewProjection()const { return pCamera2DStore_->GetCamera2D().GetViewMatrix() * pCamera2DStore_->GetCamera2D().GetProjectionMatrix(); }



		/// @brief ライトのパラメータを取得する
		/// @tparam T 
		/// @param handle 
		/// @return 
		template <typename T>
		T* GetLightParam(LightHandle handle) { return pLightStore_->GetParam<T>(handle); }

		/// @brief ライトのパラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetLightParam(const std::string& name) { return pLightStore_->GetParam<T>(name); }



		/// @brief プリミティブのパラメータを取得する
		/// @tparam T 
		/// @param handle 
		/// @return 
		template<typename T>
		T* GetRender3DParam(Render3DHandle handle) { return pRender_->GetRender3DParam<T>(handle); }

		/// @brief プリミティブのパラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetRender3DParam(const std::string& name) { return pRender_->GetRender3DParam<T>(name); }

		/// @brief 2D描画のパラメータを取得する
		/// @tparam T 
		/// @param handle 
		/// @return 
		template<typename T>
		T* GetRender2DParam(Render2DHandle handle) { return pRender_->GetRender2DParam<T>(handle); }

		/// @brief 2D描画のパラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetRender2DParam(const std::string& name) { return pRender_->GetRender2DParam<T>(name); }


		/// @brief ボーンのワールド行列を取得する
		/// @param handle 
		/// @param boneName 
		/// @return 
		Matrix4x4 GetBoneWorldMatrix(Render3DHandle handle, const std::string& boneName) { return pRender_->GetBoneWorldMatrix(handle, boneName); }

		/// @brief ボーンのワールド行列を取得する
		/// @param name 
		/// @param boneName 
		/// @return 
		Matrix4x4 GetBoneWorldMatrix(const std::string& name, const std::string& boneName) { return pRender_->GetBoneWorldMatrix(name, boneName); }


		/// @brief 3D描画の親を設定する
		/// @param handle 
		/// @param pParent 
		void SetRender3DParent(Render3DHandle handle, WorldTransform3D* pParent) { pRender_->SetRender3DParent(handle, pParent); }

		/// @brief 3D描画の親を設定する
		/// @param name 
		/// @param pParent 
		void SetRender3DParent(const std::string& name, WorldTransform3D* pParent) { pRender_->SetRender3DParent(name, pParent); }


		/// @brief 2D描画の親を設定する
		/// @param handle 
		/// @param pParent 
		void SetRender2DParent(Render2DHandle handle, WorldTransform2D* pParent) { pRender_->SetRender2DParent(handle, pParent); }

		/// @brief 2D描画の親を設定する
		/// @param name 
		/// @param pParent 
		void SetRender2DParent(const std::string& name, WorldTransform2D* pParent) { pRender_->SetRender2DParent(name, pParent); }



		/// @brief プリミティブ読み込み
		/// @param hModel 
		/// @param name 
		/// @param type 
		/// @param pLog 
		/// @return 
		Render3DHandle LoadRender3D(TextureHandle hTexture, ModelHandle hModel, AnimationHandle hAnimation,SkeletonHandle hSkeleton, const std::string& name, Render3D::Type type, Log* pLog)
		{
			return pRender_->LoadRender3D(pCore_->GetDevice(), pCommandList_, hTexture, hModel, hAnimation, hSkeleton, name, type, pLog);
		}

		/// @brief スプライト読み込み
		/// @param hTexture 
		/// @param name 
		/// @param pLog 
		/// @return 
		Render2DHandle LoadRender2D(const std::string& name, Render2D::Type type, TextureHandle hTexture,TextHandle hText, Log* pLog)
		{
			return pRender_->LoadRender2D(name, type, hTexture, hText, pTextureStore_.get(), pFontStore_.get(), pCore_->GetDevice(), pLog);
		}

		/// @brief トレイル読み込み
		/// @param name 
		/// @param maxLifetime 
		/// @param hTexture 
		/// @param pLog 
		/// @return 
		TrailHandle LoadTrail(const std::string& name,float maxLifetime, TextureHandle hTexture, Log* pLog)
		{
			return pRender_->LoadTrail(name, maxLifetime, hTexture, pCore_->GetDevice(), pLog);
		}

		/// @brief プリミティブ用プレハブを読み込む
		/// @param name 
		/// @param type 
		/// @param numInstance 
		/// @param hModel 
		/// @param hAnimation 
		/// @param hSkeleton 
		/// @param pLog 
		/// @return 
		Prefab3DHandle LoadPrefab3D(const std::string& name, Prefab3D::Type type,uint32_t numInstance, 
			TextureHandle hTexture, ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton, Log* pLog)
		{
			return pPrefab_->Load3D(name, type, numInstance,hTexture, hModel, hAnimation, hSkeleton, pCore_->GetDevice(), pCommandList_, pLog);
		}

		/// @brief スプライト用プレハブを読み込む
		/// @param name 
		/// @param hTexture 
		/// @param numInstance 
		/// @param pLog 
		/// @return 
		Prefab2DHandle LoadPrefab2D(const std::string& name,TextureHandle hTexture, uint32_t numInstance, Log* pLog)
		{
			return pPrefab_->Load2D(name, hTexture, numInstance, pTextureStore_.get(),pCamera2DStore_.get(), pHeap_.get(), pCore_->GetDevice(), pLog);
		}


		/// @brief プリミティブ用プレハブのパラメータを取得する
		/// @tparam T 
		/// @param hPrefabPrimitive 
		/// @return 
		template<typename T>
		T* GetPrefab3DParam(Prefab3DHandle hPrefab3D) { return pPrefab_->GetPrefab3DParam<T>(hPrefab3D); }

		/// @brief プリミティブ用プレハブのパラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetPrefab3DParam(const std::string& name) { return pPrefab_->GetPrefab3DParam(name); }

		/// @brief 2Dプレハブのパラメータを取得する
		/// @tparam T 
		/// @param hPrefab2D 
		/// @return 
		template<typename T>
		T* GetPrefab2DParam(Prefab2DHandle hPrefab2D) { return pPrefab_->GetPrefab2DParam<T>(hPrefab2D); }

		/// @brief 2Dプレハブのパラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetPrefab2DParam(const std::string& name) { return pPrefab_->GetPrefab2DParam<T>(name); }

		/// @brief トレイルのパラメータを取得する
		/// @param hTrail 
		/// @return 
		TrailData::Param* GetTrailParam(TrailHandle hTrail) { return pRender_->GetTrailParam(hTrail); }

		/// @brief トレイルのパラメータを取得する
		/// @param name 
		/// @return 
		TrailData::Param* GetTrailParam(const std::string& name) { return pRender_->GetTrailParam(name); }

		/// @brief プリミティブ用インスタンスを作成する
		/// @tparam T 
		/// @param hPrefabPrimitive 
		/// @return 
		template<typename T>
		T* CreatePrefab3DInstance(Prefab3DHandle hPrefab3D) { return pPrefab_->CreatePrefab3DInstance<T>(hPrefab3D); }

		/// @brief プリミティブ用インスタンスを作成する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* CreatePrefab3DInstance(const std::string& name) { return pPrefab_->CreatePrefab3DInstance<T>(name); }

		/// @brief 2Dプレハブ用インスタンスを作成する
		/// @tparam T 
		/// @param hPrefabSprite 
		/// @return 
		template<typename T>
		T* CreatePrefab2DInstance(Prefab2DHandle hPrefabSprite) { return pPrefab_->CreatePrefab2DInstance<T>(hPrefabSprite); }

		/// @brief 2Dプレハブ用インスタンスを作成する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* CreatePrefab2DInstance(const std::string& name) { return pPrefab_->CreatePrefab2DInstance<T>(name); }


		/// @brief 全ての3Dプレハブの描画処理
		void AllDrawPrefab3D() { pPrefab_->AllDrawPrefab3D(pSkyboxStore_.get(), pCommandList_); }

		/// @brief 3Dプレハブの描画処理
		/// @param hPrefab3D 
		void DrawPrefab3D(Prefab3DHandle hPrefab3D) { pPrefab_->DrawPrefab3D(hPrefab3D, pSkyboxStore_.get(), pCommandList_); }

		/// @brief 3Dプレハブの描画処理
		/// @param name 
		void DrawPrefab3D(const std::string& name) { pPrefab_->DrawPrefab3D(name, pSkyboxStore_.get(), pCommandList_); }

		
		/// @brief 全ての2Dプレハブの描画処理
		void AllDrawPrefab2D() { pPrefab_->AllDrawPrefab2D(pCommandList_); }

		/// @brief 2Dプレハブの描画処理
		/// @param hPrefab2D 
		void DrawPrefab2D(Prefab2DHandle hPrefab2D) { pPrefab_->DrawPrefab2D(hPrefab2D, pCommandList_); }

		/// @brief 2Dプレハブの描画処理
		/// @param name 
		void DrawPrefab2D(const std::string& name) { pPrefab_->DrawPrefab2D(name, pCommandList_); }


	public:

		/// @brief プリミティブの描画処理
		/// @param handle 
		void DrawRender3D(Render3DHandle handle)
		{
			pRender_->DrawRender3D(pCamera3DStore_.get(),pSkyboxStore_.get(), pCommandList_, handle);
		}

		/// @brief プリミティブの描画処理
		/// @param name 
		void DrawRender3D(const std::string& name)
		{
			pRender_->DrawRender3D(pCamera3DStore_.get(), pSkyboxStore_.get(), pCommandList_, name);
		}

		/// @brief スプライトの描画処理
		/// @param handle 
		void DrawRender2D(Render2DHandle handle)
		{
			pRender_->DrawRender2D(handle, pCamera2DStore_.get(), pCommandList_);
		}

		/// @brief スプライトの描画処理
		/// @param name 
		void DrawRender2D(const std::string& name)
		{
			pRender_->DrawRender2D(name, pCamera2DStore_.get(), pCommandList_);
		}

		/// @brief トレイルの描画処理
		/// @param handle 
		void DrawTrail(TrailHandle handle)
		{
			pRender_->DrawTrail(handle, pCommandList_);
		}

		/// @brief トレイルの描画処理
		/// @param name 
		void DrawTrail(const std::string& name)
		{
			pRender_->DrawTrail(name, pCommandList_);
		}



		/// @brief ポストエフェクトを読み込む
		/// @param name 
		/// @param type 
		/// @param pLog 
		/// @return 
		PostEffectHandle LoadPostEffect(const std::string& name, PostEffect::Type type, Log* pLog);

		/// @brief ポストエフェクトを描画する
		/// @param hPostEffect 
		void DrawPostEffect(PostEffectHandle hPostEffect);

		/// @brief ポストエフェクトを描画する
		/// @param name 
		void DrawPostEffect(const std::string& name);

		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param hPostEffect 
		/// @return 
		template<typename T>
		T* GetPostEffectParam(PostEffectHandle hPostEffect) { return pOffscreen_->GetPostEffectParam<T>(hPostEffect); }

		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetPostEffectParam(const std::string& name) { return pOffscreen_->GetPostEffectParam<T>(name); }



		/// @brief 3D衝突読み込み
		/// @param name 
		/// @param type 
		/// @return 
		Collision3DHandle LoadCollision3D(const std::string& name, Collision3D::Type type) { return pCollision3DStore_->Load(name, type); }

		/// @brief インスタンスを作成する
		/// @tparam T 
		/// @param hCollision 
		/// @return 
		template<typename T>
		T* CreateCollision3DInstance(Collision3DHandle hCollision) { return pCollision3DStore_->CreateInstance<T>(hCollision); }

		/// @brief インスタンスを作成する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* CreateCollision3DInstance(const std::string& name) { return pCollision3DStore_->CreateInstance<T>(name); }

		/// @brief 衝突対象の設定
		/// @param hCollision 
		/// @param hTargetCollision 
		void SetCollision3DTarget(Collision3DHandle hCollision, Collision3DHandle hTargetCollision) { pCollision3DStore_->SetCollision3DTarget(hCollision, hTargetCollision); }

		/// @brief 衝突対象の設定
		/// @param name 
		/// @param targetName 
		void SetCollision3DTarget(const std::string& name, const std::string& targetName) { pCollision3DStore_->SetCollision3DTarget(name, targetName); }



		/// @brief 2D衝突読み込み
		/// @param name 
		/// @param type 
		/// @return 
		Collision2DHandle LoadCollision2D(const std::string& name, Collision2D::Type type) { return pCollision2DStore_->Load(name, type); }

		/// @brief インスタンスを作成する
		/// @tparam T 
		/// @param hCollision 
		/// @return 
		template<typename T>
		T* CreateCollision2DInstance(Collision2DHandle hCollision) { return pCollision2DStore_->CreateInstance<T>(hCollision); }

		/// @brief インスタンスを作成する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* CreateCollision2DInstance(const std::string& name) { return pCollision2DStore_->CreateInstance<T>(name); }

		/// @brief 衝突対象の設定
		/// @param hCollision 
		/// @param hTargetCollision 
		void SetCollision2DTarget(Collision2DHandle hCollision, Collision2DHandle hTargetCollision) { pCollision2DStore_->SetCollision2DTarget(hCollision, hTargetCollision); }

		/// @brief 衝突対象の設定
		/// @param name 
		/// @param targetName 
		void SetCollision2DTarget(const std::string& name, const std::string& targetName) { pCollision2DStore_->SetCollision2DTarget(name, targetName); }



		/// @brief 3Dパーティクルを読み込む
		/// @param name 
		/// @param numInstance 
		/// @param hModel 
		/// @param pLog 
		/// @return 
		Particle3DHandle LoadParticle3D(const std::string& name, uint32_t numInstance,uint32_t numEmitter, ModelHandle hModel, Log* pLog)
		{
			return pParticle_->LoadParticle3D(pCore_->GetDevice(), pCommandList_, hModel, name, numInstance,numEmitter, pLog);
		}

		/// @brief 3Dパーティクルを描画する
		/// @param hParticle 
		void DrawParticle3D(Particle3DHandle hParticle)
		{
			pParticle_->Draw3DParticle(pCommandList_, hParticle, pCamera3DStore_.get(),pOffscreen_->GetDestinationResource(), pOffscreen_->GetDepthResource());
		}

		/// @brief 3Dパーティクルを描画する
		/// @param name 
		void DrawParticle3D(const std::string& name)
		{
			pParticle_->Draw3DParticle(pCommandList_, name, pCamera3DStore_.get(), pOffscreen_->GetDestinationResource(), pOffscreen_->GetDepthResource());
		}

		/// @brief 3Dパーティクルのパラメータを取得する
		/// @param hParticle 
		/// @return 
		Particle3D::Param* GetParticle3DParam(Particle3DHandle hParticle) { return pParticle_->Get3DParticleParam(hParticle); }

		/// @brief 3Dパーティクルのパラメータを取得する
		/// @param name 
		/// @return 
		Particle3D::Param* GetParticle3DParam(const std::string& name) { return pParticle_->Get3DParticleParam(name); }

		/// @brief 3Dパーティクルのエミッタのインデックスを取得する
		/// @param hParticle 
		/// @return 
		int32_t GetEmitter3DIndex(Particle3DHandle hParticle) { return pParticle_->Get3DEmitterIndex(hParticle); }

		/// @brief 3Dパーティクルのエミッタのインデックスを取得する
		/// @param name 
		/// @return 
		int32_t GetEmitter3DIndex(const std::string& name) { return pParticle_->Get3DEmitterIndex(name); }

		/// @brief 3Dパーティクルを放出する
		/// @param hParticle 
		/// @param emitterIndex 
		void EmittParticle3D(Particle3DHandle hParticle, int32_t emitterIndex) { pParticle_->Emit3D(hParticle, emitterIndex); }

		/// @brief 3Dパーティクルを放出する
		/// @param name 
		/// @param emitterIndex 
		void EmittParticle3D(const std::string& name, int32_t emitterIndex) { pParticle_->Emit3D(name, emitterIndex); }

		/// @brief 3Dパーティクルを停止する
		/// @param hParticle 
		/// @param emitterIndex 
		void StopParticle3D(Particle3DHandle hParticle, int32_t emitterIndex) { pParticle_->Stop3D(hParticle, emitterIndex); }

		/// @brief 3Dパーティクルを停止する
		/// @param name 
		/// @param emitterIndex 
		void StopParticle3D(const std::string& name, int32_t emitterIndex) { pParticle_->Stop3D(name, emitterIndex); }

		/// @brief 3Dパーティクルのエミッタを取得する
		/// @param hParticle 
		/// @param emitterIndex 
		/// @return 
		Particle3D::Emitter* Get3DEmitter(Particle3DHandle hParticle, int32_t emitterIndex) { return pParticle_->Get3DEmitter(hParticle, emitterIndex); }

		/// @brief 3Dパーティクルのエミッタを取得する
		/// @param name 
		/// @param emitterIndex 
		/// @return 
		Particle3D::Emitter* Get3DEmitter(const std::string& name, int32_t emitterIndex) { return pParticle_->Get3DEmitter(name, emitterIndex); }


		/// @brief アニメーションの時間を取得する
		/// @param hAnimation 
		/// @return 
		float GetAnimationDuration(AnimationHandle hAnimation) { return pAnimationStore_->GetDuration(hAnimation); }


		/// @brief トレイルの履歴を消す
		/// @param hTrail 
		void Trail3DClear(TrailHandle hTrail) { pRender_->Trail3DClear(hTrail); }

		/// @brief トレイルの履歴を消す
		/// @param name 
		void Trail3DClear(const std::string& name) { pRender_->Trail3DClear(name); }



		/// @brief レンダーパスを読み込む
		/// @param name 
		/// @param drawFunc 
		/// @return 
		RenderPassHandle LoadRenderPass(const std::string& name, std::function<void()> drawFunc) { return pOffscreen_->LoadRenderPass(name, drawFunc); }

		/// @brief レンダーパスを実行する
		/// @param hRenderPass 
		void ExecuteRenderPass(RenderPassHandle hRenderPass) { pOffscreen_->ExecuteRenderPass(hRenderPass, pCommandList_); }

		/// @brief レンダーパスを実行する
		/// @param name 
		void ExecuteRenderPass(const std::string& name) { pOffscreen_->ExecuteRenderPass(name, pCommandList_); }

		/// @brief レンダーパスに描画する
		/// @param renderTargetHandle 
		/// @param sourceHandle 
		void DrawToRenderPass(RenderPassHandle renderTargetHandle, RenderPassHandle sourceHandle) { pOffscreen_->DrawToRenderPass(renderTargetHandle, sourceHandle, pCommandList_); }

		/// @brief レンダーパスに描画する
		/// @param renderTargetName 
		/// @param sourceName 
		void DrawToRenderPass(const std::string& renderTargetName, const std::string& sourceName) { pOffscreen_->DrawToRenderPass(renderTargetName, sourceName, pCommandList_); }


		/// @brief レンダーパスのパラメータを取得する
		/// @param handle 
		/// @return 
		RenderPassData::Param* GetRenderPassParam(RenderPassHandle handle) { return pOffscreen_->GetRenderPassParam(handle); }

		/// @brief レンダーパスのパラメータを取得する
		/// @param name 
		/// @return 
		RenderPassData::Param* GetRenderPassParam(const std::string& name) { return pOffscreen_->GetRenderPassParam(name); }


#ifdef DEVELOPMENT

		/// @brief デバッグ用の線を描画する
		/// @param start 
		/// @param end 
		/// @param color 
		void DrawDebugLine3D(const Vector3& start, const Vector3& end, const Vector4& color) { pLine_->DrawCallLine3D(start, end, color); }

		/// @brief デバッグ用の線を描画する
		/// @param start 
		/// @param end 
		/// @param color 
		void DrawDebugLine2D(const Vector2& start, const Vector2& end, const Vector4& color) { pLine_->DrawCallLine2D(start, end, color); }

		/// @brief デバッグ用の三角形を描画する
		/// @param v0 
		/// @param v1 
		/// @param v2 
		/// @param color 
		void DrawDebugTriangle3D(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector4& color) { pLine_->DrawCallTriangle3D(v0, v1, v2, color); }

		/// @brief ビューウィンドウ内のカーソルの位置を取得する
		/// @return 
		Vector2 GetViewWindowCursorPos()const { return pImguiRender_->GetViewWindowCursorPos(); }

		/// @brief ビューウィンドウ内にカーソルがホバーしているかどうか
		/// @return 
		bool IsViewWindowHover()const { return pImguiRender_->IsViewWindowHover(); }

		/// @brief デバッグ用レイピッキング
		void DebugRayPicking();

		/// @brief デバッグ用立方体の描画
		/// @param position 
		/// @param rotate 
		/// @param scale 
		/// @param color 
		void DrawDebugCube(const Vector3& position, const Vector3& rotate, const Vector3& scale, const Vector4& color) { pPrefab_->DrawDebugCube(position, rotate, scale, color); }

#endif


	private:

#ifdef _DEBUG
		// DX12Debug
		std::unique_ptr<DX12Debug> pDebug_ = nullptr;
#endif

#ifdef DEVELOPMENT
		/// @brief DX12Line
		std::unique_ptr<DX12Line> pLine_ = nullptr;
#endif

		// DX12Core
		std::unique_ptr<DX12Core> pCore_ = nullptr;

		// DX12Command
		std::unique_ptr<DX12Command> pCommand_ = nullptr;

		// DX12Heap
		std::unique_ptr<DX12Heap> pHeap_ = nullptr;

		// DX12Buffering
		std::unique_ptr<DX12Buffering> pBuffering_ = nullptr;

		// DX12Fence
		std::unique_ptr<DX12Fence> pFence_ = nullptr;

		// シェーダコンパイラ
		std::unique_ptr<ShaderCompiler> pShaderCompiler_ = nullptr;

		// DX12Offscreen
		std::unique_ptr<DX12Offscreen> pOffscreen_ = nullptr;

		// DX12Model
		std::unique_ptr<DX12Render> pRender_ = nullptr;

		// DX12Prefab
		std::unique_ptr<DX12Prefab> pPrefab_ = nullptr;

		/// @brief DX12Particle
		std::unique_ptr<DX12Particle> pParticle_ = nullptr;


	private:

		/// @brief FPS固定初期化
		void InitializeFixFPS();

		/// @brief FPS固定更新処理
		void UpdateFixFPS();

		/// @brief 記録時間（FPS固定用）
		std::chrono::steady_clock::time_point reference_;


	private:

		// 3Dカメラストア
		std::unique_ptr<Camera3DStore> pCamera3DStore_ = nullptr;

		// 2Dカメラストア
		std::unique_ptr<Camera2DStore> pCamera2DStore_ = nullptr;

		// テクスチャストア
		std::unique_ptr<TextureStore> pTextureStore_ = nullptr;

		// モデルストア
		std::unique_ptr<ModelStore> pModelStore_ = nullptr;

		/// @brief アニメーションストア
		std::unique_ptr<AnimationStore> pAnimationStore_ = nullptr;

		/// @brief スケルトンストア
		std::unique_ptr<SkeletonStore> pSkeletonStore_ = nullptr;

		/// @brief ライトストア
		std::unique_ptr<LightStore> pLightStore_ = nullptr;

		/// @brief 2D衝突ストア
		std::unique_ptr<Collision2DStore> pCollision2DStore_ = nullptr;

		// 3D衝突ストア
		std::unique_ptr<Collision3DStore> pCollision3DStore_ = nullptr;

		/// @brief スカイボックスストア
		std::unique_ptr<SkyboxStore> pSkyboxStore_ = nullptr;

		/// @brief フォントストア
		std::unique_ptr<FontStore> pFontStore_ = nullptr;


	private:

		/// @brief ビューポート
		D3D12_VIEWPORT viewport_{};

		/// @brief シザー矩形
		D3D12_RECT scissorRect_{};


	private:

		// コマンドリスト
		ID3D12GraphicsCommandList* pCommandList_ = nullptr;

		// コマンドアロケータ
		ID3D12CommandAllocator* pCommandAllocator_ = nullptr;


	private:

		// ImGui用SRVハンドル
#ifdef DEVELOPMENT
		std::unique_ptr<ImGuiRender> pImguiRender_ = nullptr;
#endif

	private:

		/// @brief サイズを作り直す
		/// @param width 
		/// @param height 
		void Resize(int32_t width, int32_t height);

		// ウィンドウアプリケーション
		WinApp* pWinApp_ = nullptr;

	};
}