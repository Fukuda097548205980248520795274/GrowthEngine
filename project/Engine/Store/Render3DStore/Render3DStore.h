#pragma once
#include "Render3DData/Render3DBaseData.h"
#include <memory>
#include <vector>

#include "PSO/PSOModel/PSORender3D/PSORender3D.h"

#include "PSO/ComputePSO/ComputePSOUVSphere/ComputePSOUVSphere.h"
#include "PSO/ComputePSO/ComputePSOSkinning/ComputePSOSkinning.h"

#include "Parameter/Render3DParameter/Render3DParameter.h"

class PrimitiveStaticModel;

namespace Engine
{
	class BasePSOModel;
	class BasePSOShadowMap;

	class ModelStore;
	class TextureStore;
	class AnimationStore;
	class SkeletonStore;
	class LightStore;
	class Log;
	class DX12Heap;
	class ShaderCompiler;
	class Camera3DStore;
	class SkyboxStore;

	class Render3DStore
	{
	public:

		/// @brief コンストラクタ
		Render3DStore();

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param heap 
		/// @param modelStore 
		/// @param textureStore 
		/// @param animationStore 
		/// @param skeletonStore 
		/// @param lightStore 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, DX12Heap* heap,
			ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, SkeletonStore* skeletonStore, LightStore* lightStore, Log* log);

		/// @brief 更新処理
		void Update(ID3D12GraphicsCommandList* commandList);

		/// @brief シーン前のリセット
		void PerSceneReset();

		/// @brief シャドウマップ用の描画処理
		/// @param commandList 
		/// @param pso 
		void ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso);

		/// @brief モーションベクター用の描画処理
		/// @param commandList 
		/// @param pso 
		void DrawMotionVector(ID3D12GraphicsCommandList* commandList, BasePSOMotionVector* pso);

		/// @brief アウトライン用の描画処理
		/// @param commandList 
		/// @param pso 
		void DrawOutline(ID3D12GraphicsCommandList* commandList, BasePSOOutline* pso);

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
		Render3DHandle Load(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
			TextureHandle hTexture, ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton,
			const std::string& name, Render3D::Type type, Log* log);

		/// @brief コマンドリストに登録する
		/// @param cameraStore 
		/// @param skyboxStore 
		/// @param commandList 
		/// @param handle 
		void Register(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, Render3DHandle handle);

		/// @brief コマンドリストに登録する
		/// @param cameraStore 
		/// @param skyboxStore 
		/// @param commandList 
		/// @param name 
		void Register(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, const std::string& name);

		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param handle 
		/// @return 
		template<typename T>
		T* GetParam(Render3DHandle handle)
		{
			Render3DBaseData* data = dataTable_[handle].get();
			return static_cast<T*>(data->GetParam());
		}

		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetParam(const std::string& name)
		{
			Render3DBaseData* data = dataTable_[nameTable_[name]].get();
			return static_cast<T*>(data->GetParam());
		}

		/// @brief 親を設定する
		/// @param handle 
		/// @param parent 
		void SetParent(Render3DHandle handle, WorldTransform3D* parent) { dataTable_[handle]->SetParent(parent); }

		/// @brief 親を設定する
		/// @param name 
		/// @param parent 
		void SetParent(const std::string& name, WorldTransform3D* parent) { dataTable_[nameTable_[name]]->SetParent(parent); }

		/// @brief ボーンのワールド行列を取得する
		/// @param handle 
		/// @param boneName 
		/// @return 
		Matrix4x4 GetBoneWorldMatrix(Render3DHandle handle, const std::string& boneName) { return dataTable_[handle]->GetBoneWorldMatrix(boneName); }

		/// @brief ボーンのワールド行列を取得する
		/// @param name 
		/// @param boneName 
		/// @return 
		Matrix4x4 GetBoneWorldMatrix(const std::string& name, const std::string& boneName) { return dataTable_[nameTable_[name]]->GetBoneWorldMatrix(boneName); }

		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;


	public:

		/// @brief デバッグ用パラメータ
		void DebugParameter();


	private:

		// データテーブル
		std::vector<std::unique_ptr<Render3DBaseData>> dataTable_;

		// 名前テーブル
		std::unordered_map<std::string, Render3DHandle> nameTable_;


		// プリミティブ用パラメータ
		std::unique_ptr<Render3DParameter> parameter_ = nullptr;


	private:

		// 3D描画頂点シェーダ
		ComPtr<IDxcBlob> render3DVS_ = nullptr;

		// 3D描画シェーダ
		ComPtr<IDxcBlob> render3DPS_ = nullptr;


	private:

		// 3DD描画PSO
		std::unique_ptr<PSORender3D> psoRender3D_ = nullptr;


		// CSUV球PSO
		std::unique_ptr<ComputePSOUVSphere> psoUVSphere_ = nullptr;

		// CSスキニングPSO
		std::unique_ptr<ComputePSOSkinning> psoSkinning_ = nullptr;


	private:

		/// @brief ヒープ
		DX12Heap* heap_ = nullptr;

		/// @brief モデルストア
		ModelStore* modelStore_ = nullptr;

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;

		/// @brief アニメーションストア
		AnimationStore* animationStore_ = nullptr;

		/// @brief スケルトンストア
		SkeletonStore* skeletonStore_ = nullptr;

		/// @brief ライトストア
		LightStore* lightStore_ = nullptr;
	};
}