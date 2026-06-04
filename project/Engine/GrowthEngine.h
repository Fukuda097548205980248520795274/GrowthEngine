#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <Windows.h>

#include "Log/Log.h"
#include "WinApp/WinApp.h"
#include "Input/Input.h"

#include "Func/RandomFunc/RandomFunc.h"

#include "Store/AudioStore/AudioStore.h"
#include "Store/SoundStore/SoundStore.h"
#include "Store/InputStore/InputStore.h"

#include "RenderContext/RenderContext.h"

#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Vector/VectorInt3/VectorInt3.h"

#include "Application/Framework/Framework.h"

#include "Application/GameCamera/GameCamera3D/GameCamera3D.h"
#include "Application/GameCamera/GameCamera2D/GameCamera2D.h"

#include "Application/Particle3D/Particle3D.h"

#include "Application/Render3D/Render3DStaticModel/Render3DStaticModel.h"
#include "Application/Render3D/Render3DAnimationModel/Render3DAnimationModel.h"
#include "Application/Render3D/Render3DSkinningModel/Render3DSkinningModel.h"

#include "Application/Sprite/Sprite.h"

#include "Application/PrefabInstance/PrefabInstanceSprite/PrefabInstanceSprite.h"
#include "Application/PrefabBase/PrefabBaseSprite/PrefabBaseSprite.h"

#include "Application/PrefabInstance/PrefabInstanceStaticModel/PrefabInstanceStaticModel.h"
#include "Application/PrefabBase/PrefabBaseStaticModel/PrefabBaseStaticModel.h"

#include "Application/PrefabBase/PrefabBaseCube/PrefabBaseCube.h"
#include "Application/PrefabInstance/PrefabInstanceCube/PrefabInstanceCube.h"

#include "Application/PrefabBase/PrefabBaseTube/PrefabBaseTube.h"
#include "Application/PrefabInstance/PrefabInstanceTube/PrefabInstanceTube.h"

#include "Application/PostEffect/PostEffectRadialBlur/PostEffectRadialBlur.h"
#include "Application/PostEffect/PostEffectGrayscale/PostEffectGrayscale.h"
#include "Application/PostEffect/PostEffectVignetting/PostEffectVignetting.h"

#include "Application/Scene/Scene.h"
#include "Application/SceneManager/SceneManager.h"

#include "Application/Sound/Sound.h"

#include "Application/Collision3DBase/Collision3DBaseSphere/Collision3DBaseSphere.h"
#include "Application/Collision3DBase/Collision3DBaseAABB/Collision3DBaseAABB.h"
#include "Application/Collision3DBase/Collision3DBaseOBB/Collision3DBaseOBB.h"
#include "Application/Collision3DBase/Collision3DBasePlane/Collision3DBasePlane.h"
#include "Application/Collision3DBase/Collision3DBaseLine/Collision3DBaseLine.h"
#include "Application/Collision3DBase/Collision3DBaseRay/Collision3DBaseRay.h"
#include "Application/Collision3DBase/Collision3DBaseSegment/Collision3DBaseSegment.h"

#include "Application/Collision3DInstance/Collision3DInstanceSphere/Collision3DInstanceSphere.h"
#include "Application/Collision3DInstance/Collision3DInstanceAABB/Collision3DInstanceAABB.h"
#include "Application/Collision3DInstance/Collision3DInstanceOBB/Collision3DInstanceOBB.h"
#include "Application/Collision3DInstance/Collision3DInstancePlane/Collision3DInstancePlane.h"
#include "Application/Collision3DInstance/Collision3DInstanceLine/Collision3DInstanceLine.h"
#include "Application/Collision3DInstance/Collision3DInstanceRay/Collision3DInstanceRay.h"
#include "Application/Collision3DInstance/Collision3DInstanceSegment/Collision3DInstanceSegment.h"

#include "Application/Collision2DBase/Collision2DBaseCircle/Collision2DBaseCircle.h"
#include "Application/Collision2DBase/Collision2DBaseSprite/Collision2DBaseSprite.h"

#include "Application/Collision2DInstance/Collision2DInstanceCircle/Collision2DInstanceCircle.h"
#include "Application/Collision2DInstance/Collision2DInstanceSprite/Collision2DInstanceSprite.h"

#include "Application/WorldTransform/WorldTransform2D/WorldTransform2D.h"
#include "Application/WorldTransform/WorldTransform3D/WorldTransform3D.h"

#include "Application/Light/LightDirectional/LightDirectional.h"
#include "Application/Light/LightPoint/LightPoint.h"
#include "Application/Light/LightSpot/LightSpot.h"

#include "Application/Input/InputGamepadButton/InputGamepadButton.h"
#include "Application/Input/InputGamepadLeftStick/InputGamepadLeftStick.h"
#include "Application/Input/InputGamepadLeftTrigger/InputGamepadLeftTrigger.h"
#include "Application/Input/InputGamepadRightStick/InputGamepadRightStick.h"
#include "Application/Input/InputGamepadRightTrigger/InputGamepadRightTrigger.h"
#include "Application/Input/InputKey/InputKey.h"

#include "Func/CollisionFunc/CollisionFunc.h"
#include "Func/RandomFunc/RandomFunc.h"


// マウスボタン
enum MouseButton
{
	// 左ボタン
	kMouseButtonLeft,

	// 右ボタン
	kMouseButtonRight,

	// 中央ボタン
	kMouseButtonCenter
};


class GrowthEngine
{
public:

	/// @brief インスタンスを取得する
	/// @param screenWidth スクリーン横幅
	/// @param screenHeight スクリーン縦幅
	/// @param title タイトル
	/// @return 
	static GrowthEngine* GetInstance(int32_t screenWidth, int32_t screenHeight, const std::string& title);

	/// @brief インスタンスを取得する
	/// @return 
	static GrowthEngine* GetInstance();

	/// @brief デストラクタ
	~GrowthEngine();

	/// @brief ゲームループ
	/// @return 
	bool GameLoop() { return winApp_->ProcessMessage(); }

	/// @brief シーン前処理
	void PerScene();

	/// @brief 新フレーム処理
	void NewFrame();

	/// @brief 描画前処理
	void PreDraw();

	/// @brief 描画後処理
	void PostDraw();

	/// @brief デルタタイムを取得する
	/// @return 
	float GetDeltaTime()const { return deltaTime_; }

	/// @brief スローモーションを開始する
	/// @param scale 
	/// @param duration 
	void StartSlowMotion(float scale, float duration);

	/// @brief 現在のタイムスケールを取得する
	float GetTimeScale() const { return timeScale_; }

	/// @brief フルスクリーン切り替え
	void FullscreenSwitch()const { winApp_->Fullscreen(); }

	/// @brief ウィンドウを閉じる
	void CloseWindow()const { winApp_->Close(); }

	/// @brief 3Dカメラの切り替え
	/// @param hCamera 
	void Camera3DSwitch(Camera3DHandle hCamera) const { renderContext_->Camera3DSwitch(hCamera); }

	/// @brief 3Dカメラの切り替え
	/// @param name 
	void Camera3DSwitch(const std::string& name)const { renderContext_->Camera3DSwitch(name); }

	/// @brief 2Dカメラの切り替え
	/// @param hCamera 
	void Camera2DSwitch(Camera2DHandle hCamera)const { renderContext_->Camera2DSwitch(hCamera); }

	/// @brief 2Dカメラの切り替え
	/// @param name 
	void Camera2DSwitch(const std::string& name)const { renderContext_->Camera2DSwitch(name); }

	/// @brief 3Dカメラを読み込む
	/// @param name 
	/// @return 
	Camera3DHandle LoadCamera3D(const std::string& name)const { return renderContext_->LoadCamera3D(name); }

	/// @brief 2Dカメラを読み込む
	/// @param name 
	/// @return 
	Camera2DHandle LoadCamera2D(const std::string& name)const { return renderContext_->LoadCamera2D(name); }

	/// @brief テクスチャを読み込む
	/// @param filePath 
	/// @return 
	TextureHandle LoadTexture(const std::string& filePath) const { return renderContext_->LoadTexture(filePath, log_.get()); }

	/// @brief テクスチャのSRVのGPUハンドルを取得する
	/// @param handle 
	/// @return 
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvGpuHandle(TextureHandle handle)const { return renderContext_->GetTextureSrvGpuHandle(handle); }

	/// @brief オーディオを読み込む
	/// @param filePath 
	/// @return 
	AudioHandle LoadAudio(const std::string& filePath) const { return audioStore_->Load(filePath, log_.get()); }

	/// @brief モデルを読み込む
	/// @param directory 
	/// @param fileName 
	/// @return 
	ModelHandle LoadModel(const std::string& directory, const std::string& fileName)const { return renderContext_->LoadModel(directory, fileName, log_.get()); }

	/// @brief アニメーションを読み込む
	/// @param directory 
	/// @param fileName 
	/// @return 
	AnimationHandle LoadAnimation(const std::string& directory, const std::string& fileName)const { return renderContext_->LoadAnimation(directory, fileName); }

	/// @brief スケルトンを読み込む
	/// @param directory 
	/// @param fileName 
	/// @return 
	SkeletonHandle LoadSkeleton(const std::string& directory, const std::string& fileName)const { return renderContext_->LoadSkeleton(directory, fileName, log_.get()); }

	/// @brief ライトを読み込む
	/// @param name 
	/// @param type 
	/// @return 
	LightHandle LoadLight(const std::string& name, Engine::Light::Type type) const { return renderContext_->LoadLight(name, type); }

	/// @brief フォントを読み込む
	/// @param text 
	/// @param fontName 
	/// @param pixel 
	/// @return 
	TextHandle LoadFont(const std::string& text, const std::string& fontName, int pixel)const { return renderContext_->LoadFont(text, fontName, pixel, log_.get()); }

	/// @brief サウンドを読み込む
	/// @param name 
	/// @param hAudio 
	/// @return 
	SoundHandle LoadSound(const std::string& name, AudioHandle hAudio)const { return soundStore_->Load(name, hAudio); }



public:

	/// @brief サウンドを再生する
	/// @param hSound 
	void SoundPlay(SoundHandle hSound)const { soundStore_->Play(hSound); }

	/// @brief サウンドを再生する
	/// @param name 
	void SoundPlay(const std::string& name)const { soundStore_->Play(name); }

	/// @brief サウンドを停止する
	/// @param hSound 
	void SoundStop(SoundHandle hSound)const { soundStore_->Stop(hSound); }

	/// @brief サウンドを停止する
	/// @param name 
	void SoundStop(const std::string& name)const { soundStore_->Stop(name); }

	/// @brief サウンドのパラメータを取得する
	/// @param hSound 
	/// @return 
	Engine::SoundParam* GetSoundParam(SoundHandle hSound)const { return soundStore_->GetParam(hSound); }

	/// @brief サウンドのパラメータを取得する
	/// @param name 
	/// @return 
	Engine::SoundParam* GetSoundParam(const std::string& name)const { return soundStore_->GetParam(name); }

	/// @brief サウンドが再生されているかどうか
	/// @param hSound 
	/// @return 
	bool IsSoundPlay(SoundHandle hSound)const { return soundStore_->IsPlay(hSound); }

	/// @brief サウンドが再生されているかどうか
	/// @param name 
	/// @return 
	bool IsSoundPlay(const std::string& name)const { return soundStore_->IsPlay(name); }


public:

	/// @brief 画面の横幅を取得する
	/// @return 
	int32_t GetScreenWidth()const { return winApp_->GetClientWidth(); }

	/// @brief 画面の縦幅を取得する
	/// @return 
	int32_t GetScreenHeight()const { return winApp_->GetClientHeight(); }


#pragma region カメラ

	/// @brief 3Dカメラのパラメータを取得する
	/// @param hCamera 
	/// @return 
	Engine::Camera3DData::Param* GetCamera3DParam(Camera3DHandle hCamera)const { return renderContext_->GetCamera3DParam(hCamera); }

	/// @brief 3Dカメラのパラメータを取得する
	/// @param name 
	/// @return 
	Engine::Camera3DData::Param* GetCamera3DParam(const std::string& name)const { return renderContext_->GetCamera3DParam(name); }

	/// @brief 3Dカメラのパラメータを取得する
	/// @return 
	Engine::Camera3DData::Param* GetCamera3DParam()const { return renderContext_->GetCamera3DParam(); }

	/// @brief 2Dカメラのパラメータを取得する
	/// @param hCamera 
	/// @return 
	Engine::Camera2DData::Param* GetCamera2DParam(Camera2DHandle hCamera)const { return renderContext_->GetCamera2DParam(hCamera); }

	/// @brief 2Dカメラのパラメータを取得する
	/// @param name 
	/// @return 
	Engine::Camera2DData::Param* GetCamera2DParam(const std::string& name)const { return renderContext_->GetCamera2DParam(name); }

#pragma endregion

#pragma region 入力

	/// @brief キー入力を読み込む
	/// @param name 
	/// @param inputState 
	/// @param key 
	/// @return 
	InputHandle LoadInputKey(const std::string& name, InputState inputState, BYTE key)const { return inputStore_->LoadKey(name, inputState, key); }

	/// @brief ゲームパッドボタン入力を読み込む
	/// @param name 
	/// @param inputState 
	/// @param controller 
	/// @param button 
	/// @return 
	InputHandle LoadInputGamepadButton(const std::string& name, InputState inputState, DWORD controller, DWORD button)const { return inputStore_->LoadGamepadButton(name, inputState, controller, button); }

	/// @brief ゲームパッドスティック読み込み
	/// @param name 
	/// @param inputState 
	/// @param stickType 
	/// @param controller 
	/// @param direction 
	/// @param dot 
	/// @return 
	InputHandle LoadInputGamepadStick(const std::string& name, InputState inputState, StickType stickType, DWORD controller, const Vector2& direction, float dot)const { return inputStore_->LoadGamepadStick(name, inputState, stickType, controller, direction, dot); }

	/// @brief ゲームパッドトリガー読み込み
	/// @param name 
	/// @param inputState 
	/// @param triggerType 
	/// @param controller 
	/// @param threshold 
	/// @return 
	InputHandle LoadInputGamepadTrigger(const std::string& name, InputState inputState, TriggerType triggerType, DWORD controller, float threshold)const { return inputStore_->LoadGamepadTrigger(name, inputState, triggerType, controller, threshold); }

	/// @brief 入力のパラメータを取得する
	/// @tparam T 
	/// @param hInput 
	/// @return 
	template<typename T>
	T* GetInputParam(InputHandle hInput)const { return inputStore_->GetParam<T>(hInput); }

	/// @brief 入力のパラメータを取得する
	/// @tparam T 
	/// @param name 
	/// @return 
	template<typename T>
	T* GetInputParam(const std::string& name)const { return inputStore_->GetParam<T>(name); }

	/// @brief 入力したかどうか
	/// @param hInput 
	/// @return 
	bool IsInput(InputHandle hInput)const { return inputStore_->IsInput(hInput); }

	/// @brief 入力したかどうか
	/// @param name 
	/// @return 
	bool IsInput(const std::string& name)const { return inputStore_->IsInput(name); }

#pragma endregion

#pragma region キーボード入力

	/// <summary>
	/// キー入力（Press）
	/// </summary>
	/// <param name="key">キー</param>
	/// <returns></returns>
	bool GetKeyPress(BYTE key) const { return input_->GetKeyPress(key); }

	/// <summary>
	/// キー入力（Trigger）
	/// </summary>
	/// <param name="key">キー</param>
	/// <returns></returns>
	bool GetKeyTrigger(BYTE key) const { return input_->GetKeyTrigger(key); }

	/// <summary>
	/// キー入力（Release）
	/// </summary>
	/// <param name="key">キー</param>
	/// <returns></returns>
	bool GetKeyRelease(BYTE key) const { return input_->GetKeyRelease(key); }

#pragma endregion

#pragma region マウス入力

	/// <summary>
	/// マウスボタン（Press）
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	bool GetMouseButtonPress(MouseButton mouseButton) const { return input_->GetMousePress(static_cast<uint32_t>(mouseButton)); };

	/// <summary>
	/// マウスボタン（Trigger）
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	bool GetMouseButtonTrigger(MouseButton mouseButton) const { return input_->GetMouseTrigger(static_cast<uint32_t>(mouseButton)); };

	/// <summary>
	/// マウスボタン（Release）
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	bool GetMouseButtonRelease(MouseButton mouseButton) const { return input_->GetMouseRelease(static_cast<uint32_t>(mouseButton)); };

	/// <summary>
	/// マウスの移動量のGetter
	/// </summary>
	/// <returns></returns>
	Vector2 GetMouseVelocity() const { return input_->GetMouseVelocity(); }

	/// <summary>
	/// マウスホイールが上回転しているかどうか
	/// </summary>
	/// <returns></returns>
	bool GetMouseWheelUp() const { return input_->GetMouseWheelUp(); }

	/// <summary>
	/// マウスホイールが下回転しているかどうか
	/// </summary>
	/// <returns></returns>
	bool GetMouseWheelDown()  const { return input_->GetMouseWheelDown(); }

	/// <summary>
	/// マウスホイールの回転量のGetter
	/// </summary>
	/// <returns></returns>
	float GetMouseWheelVelocity() const { return input_->GetMouseWheelVelocity(); }

	/// @brief マウスの位置を取得する
	/// @return 
	Vector2 GetMousePosition()const;

	/// @brief カーソルがウィンドウ内にホバーしているかどうか
	/// @return 
	bool IsCursorWindowHover()const;

#pragma endregion

#pragma region ゲームパッド入力

	/// <summary>
	/// ゲームパッドが有効かどうか
	/// </summary>
	/// <param name="gamepadNumber">ゲームパッドの番号</param>
	/// <returns></returns>
	bool IsGamepadEnable(DWORD gamepadNumber) const { return input_->IsGamepadEnable(gamepadNumber); }

	/// <summary>
	/// ゲームパッドのボタンの入力情報（Press）
	/// </summary>
	/// <param name="gamepadNumber">ゲームパッドの番号</param>
	/// <param name="wButtons">指定のボタン</param>
	/// <returns></returns>
	bool GetGamepadButtonPress(DWORD gamepadNumber, DWORD wButtons) const { return input_->GetGamepadButtonPress(gamepadNumber, wButtons); }

	/// <summary>
	/// ゲームパッドのボタンの入力情報（Press）
	/// </summary>
	/// <param name="gamepadNumber">ゲームパッドの番号</param>
	/// <param name="wButtons">指定のボタン</param>
	/// <returns></returns>
	bool GetGamepadButtonTrigger(DWORD gamepadNumber, DWORD wButtons) const { return input_->GetGamepadButtonTrigger(gamepadNumber, wButtons); }

	/// <summary>
	/// ゲームパッドのボタンの入力情報（Press）
	/// </summary>
	/// <param name="gamepadNumber">ゲームパッドの番号</param>
	/// <param name="wButtons">指定のボタン</param>
	/// <returns></returns>
	bool GetGamepadButtonRelease(DWORD gamepadNumber, DWORD wButtons) const { return input_->GetGamepadButtonRelease(gamepadNumber, wButtons); }

	/// <summary>
	/// ゲームパッドの左スティックの入力情報
	/// </summary>
	/// <param name="gamepadNumber">ゲームパッドの番号</param>
	/// <returns></returns>
	Vector2 GetGamepadLeftStick(DWORD gamepadNumber) const { return input_->GetGamepadLeftStick(gamepadNumber); }

	/// <summary>
	/// ゲームパッドの右スティックの入力情報
	/// </summary>
	/// <param name="gamepadNumber">ゲームパッドの番号</param>
	/// <returns></returns>
	Vector2 GetGamepadRightStick(DWORD gamepadNumber) const { return input_->GetGamepadRightStick(gamepadNumber); }

	/// <summary>
	/// ゲームパッドの左トリガーボタンの入力情報
	/// </summary>
	/// <param name="gamepadNumber">ゲームパッドの番号</param>
	/// <returns></returns>
	float GetGamepadLeftTrigger(DWORD gamepadNumber) const { return input_->GetGamepadLeftTrigger(gamepadNumber); }

	/// <summary>
	/// ゲームパッドの右トリガーボタンの入力情報
	/// </summary>
	/// <param name="gamepadNumber">ゲームパッドの番号</param>
	/// <returns></returns>
	float GetGamepadRightTrigger(DWORD gamepadNumber) const { return input_->GetGamepadRightTrigger(gamepadNumber); }

	/// <summary>
	/// ゲームパッドを振動させる
	/// </summary>
	/// <param name="gamepadNumber"></param>
	/// <param name="leftVibrationPower"></param>
	/// <param name="rightVibrationPower"></param>
	void GamepadVibration(DWORD gamepadNumber, float leftVibrationPower, float rightVibrationPower) const { input_->GamepadVibration(gamepadNumber, leftVibrationPower, rightVibrationPower); }


#pragma endregion


public:

	/// @brief プリミティブを読み込む
	/// @param hModel 
	/// @param name 
	/// @param type 
	/// @return 
	Render3DHandle LoadRender3D(TextureHandle hTexture, ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton, const std::string& name, Engine::Render3D::Type type) const
	{
		return renderContext_->LoadRender3D(hTexture, hModel, hAnimation, hSkeleton, name, type, log_.get());
	}

	/// @brief プリミティブを描画する
	/// @param handle 
	void DrawRender3D(Render3DHandle handle)const { renderContext_->DrawRender3D(handle); }

	/// @brief プリミティブを描画する
	/// @param name 
	void DrawRender3D(const std::string& name)const { renderContext_->DrawRender3D(name); }

	/// @brief プリミティブのパラメータを取得する
	/// @tparam T 
	/// @param handle 
	/// @return 
	template<typename T>
	T* GetRender3DParam(Render3DHandle handle)const { return renderContext_->GetRender3DParam<T>(handle); }

	/// @brief プリミティブのパラメータを取得する
	/// @tparam T 
	/// @param name 
	/// @return 
	template<typename T>
	T* GetRender3DParam(const std::string& name)const { return renderContext_->GetRender3DParam<T>(name); }

	/// @brief 2D描画のパラメータを取得する
	/// @tparam T 
	/// @param handle 
	/// @return 
	template<typename T>
	T* GetRender2DParam(Render2DHandle handle)const { return renderContext_->GetRender2DParam<T>(handle); }

	/// @brief 2D描画のパラメータを取得する
	/// @tparam T 
	/// @param name 
	/// @return 
	template<typename T>
	T* GetRender2DParam(const std::string& name)const { return renderContext_->GetRender2DParam<T>(name); }


	/// @brief ボーンのワールド行列を取得する
	/// @param handle 
	/// @param boneName 
	/// @return 
	Matrix4x4 GetBoneWorldMatrix(Render3DHandle handle, const std::string& boneName)const { return renderContext_->GetBoneWorldMatrix(handle, boneName); }

	/// @brief ボーンのワールド行列を取得する
	/// @param name 
	/// @param boneName 
	/// @return 
	Matrix4x4 GetBoneWorldMatrix(const std::string& name, const std::string& boneName)const { return renderContext_->GetBoneWorldMatrix(name, boneName); }


	/// @brief スプライトを読み込む
	/// @param hTexture 
	/// @param name 
	/// @return 
	Render2DHandle LoadRender2D(const std::string& name, Engine::Render2D::Type type, TextureHandle hTexture, TextHandle hText) const
	{
		return renderContext_->LoadRender2D(name, type, hTexture, hText, log_.get());
	}

	/// @brief スプライトを描画する
	/// @param handle 
	void DrawRender2D(Render2DHandle handle)const { renderContext_->DrawRender2D(handle); }

	/// @brief スプライトを描画する
	/// @param name 
	void DrawRender2D(const std::string& name)const { renderContext_->DrawRender2D(name); }

	/// @brief トレイルを読み込む
	/// @param name 
	/// @param maxLifetime 
	/// @param hTexture 
	/// @return 
	TrailHandle LoadTrail(const std::string& name,float maxLifetime, TextureHandle hTexture) const { return renderContext_->LoadTrail(name, maxLifetime, hTexture, log_.get()); }

	/// @brief トレイルのパラメータを取得する
	/// @param handle 
	/// @return 
	Engine::TrailData::Param* GetTrailParam(TrailHandle handle)const { return renderContext_->GetTrailParam(handle); }

	/// @brief トレイルのパラメータを取得する
	/// @param name 
	/// @return 
	Engine::TrailData::Param* GetTrailParam(const std::string& name)const { return renderContext_->GetTrailParam(name); }

	/// @brief トレイルを描画する
	/// @param handle 
	void DrawTrail(TrailHandle handle)const { renderContext_->DrawTrail(handle); }

	/// @brief トレイルを描画する
	/// @param name 
	void DrawTrail(const std::string& name)const { renderContext_->DrawTrail(name); }



	/// @brief 3D描画の親を設定する
	/// @param handle 
	/// @param parent 
	void SetRender3DParent(Render3DHandle handle, WorldTransform3D* parent) const { renderContext_->SetRender3DParent(handle, parent); }

	/// @brief 3D描画の親を設定する
	/// @param name 
	/// @param parent 
	void SetRender3DParent(const std::string& name, WorldTransform3D* parent) const { renderContext_->SetRender3DParent(name, parent); }



	/// @brief プリミティブ用prefabの読み込み
	/// @param name 
	/// @param type 
	/// @param numInstance 
	/// @param hModel 
	/// @param hAnimation 
	/// @param hSkeleton 
	/// @return 
	Prefab3DHandle LoadPrefab3D(const std::string& name, Engine::Prefab3D::Type type,uint32_t numInstance,
		TextureHandle hTexture, ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton) const
	{
		return renderContext_->LoadPrefab3D(name, type, numInstance, hTexture, hModel, hAnimation, hSkeleton, log_.get());
	}

	/// @brief プレハブスプライトの読み込み
	/// @param name 
	/// @param numInstance 
	/// @param hTexture 
	/// @return 
	Prefab2DHandle LoadPrefab2D(const std::string& name, uint32_t numInstance, TextureHandle hTexture) const
	{
		return renderContext_->LoadPrefab2D(name, hTexture, numInstance, log_.get());
	}


	/// @brief プリミティブ用プレハブのパラメータを取得する
	/// @tparam T 
	/// @param hPrefabPrimitive 
	/// @return 
	template<typename T>
	T* GetPrefab3DParam(Prefab3DHandle hPrefab3D)const { return renderContext_->GetPrefab3DParam<T>(hPrefab3D); }

	/// @brief プリミティブ用プレハブのパラメータを取得する
	/// @tparam T 
	/// @param name 
	/// @return 
	template<typename T>
	T* GetPrefab3DParam(const std::string& name)const { return renderContext_->GetPrefab3DParam<T>(name); }

	/// @brief 2Dプレハブのパラメータを取得する
	/// @tparam T 
	/// @param hPrefab2D 
	/// @return 
	template<typename T>
	T* GetPrefab2DParam(Prefab2DHandle hPrefab2D) const { return renderContext_->GetPrefab2DParam<T>(hPrefab2D); }

	/// @brief 2Dプレハブのパラメータを取得する
	/// @tparam T 
	/// @param name 
	/// @return 
	template<typename T>
	T* GetPrefab2DParam(const std::string& name) const { return renderContext_->GetPrefab2DParam<T>(name); }


	/// @brief プリミティブ用インスタンスを作成する
	/// @tparam T 
	/// @param hPrefabPrimitive 
	/// @return 
	template<typename T>
	T* CreatePrefab3DInstance(Prefab3DHandle hPrefab3D)const { return renderContext_->CreatePrefab3DInstance<T>(hPrefab3D); }

	/// @brief プリミティブ用インスタンスを作成する
	/// @tparam T 
	/// @param name 
	/// @return 
	template<typename T>
	T* CreatePrefab3DInstance(const std::string& name)const { return renderContext_->CreatePrefab3DInstance<T>(name); }

	/// @brief 2Dプレハブ用インスタンスを作成する
	/// @tparam T 
	/// @param hPrefabSprite 
	/// @return 
	template<typename T>
	T* CreatePrefab2DInstance(Prefab2DHandle hPrefabSprite) const { return renderContext_->CreatePrefab2DInstance<T>(hPrefabSprite); }

	/// @brief 2Dプレハブ用インスタンスを作成する
	/// @tparam T 
	/// @param name 
	/// @return 
	template<typename T>
	T* CreatePrefab2DInstance(const std::string& name) const { return renderContext_->CreatePrefab2DInstance<T>(name); }


	/// @brief 全ての3Dプレハブを描画する
	void AllDrawPrefab3D()const { renderContext_->AllDrawPrefab3D(); }

	/// @brief 3Dプレハブを描画する
	/// @param hPrefab3D 
	void DrawPrefab3D(Prefab3DHandle hPrefab3D)const { renderContext_->DrawPrefab3D(hPrefab3D); }

	/// @brief 3Dプレハブを描画する
	/// @param name 
	void DrawPrefab3D(const std::string& name)const { renderContext_->DrawPrefab3D(name); }

	/// @brief 全ての2Dプレハブを描画する
	void AllDrawPrefab2D()const { renderContext_->AllDrawPrefab2D(); }

	/// @brief 2Dプレハブを描画する
	/// @param hPrefab2D 
	void DrawPrefab2D(Prefab2DHandle hPrefab2D)const { renderContext_->DrawPrefab2D(hPrefab2D); }

	/// @brief 2Dプレハブを描画する
	/// @param name 
	void DrawPrefab2D(const std::string& name)const { renderContext_->DrawPrefab2D(name); }



	/// @brief ポストエフェクトを読み込む
	/// @param name 
	/// @param type 
	/// @return 
	PostEffectHandle LoadPostEffect(const std::string& name, Engine::PostEffect::Type type)const { return renderContext_->LoadPostEffect(name, type, log_.get()); }

	/// @brief ポストエフェクトを描画する
	/// @param hPostEffect 
	void DrawPostEffect(PostEffectHandle hPostEffect)const { return renderContext_->DrawPostEffect(hPostEffect); }

	/// @brief ポストエフェクトを描画する
	/// @param name 
	void DrawPostEffect(const std::string& name)const { return renderContext_->DrawPostEffect(name); }

	/// @brief パラメータを取得する
	/// @tparam T 
	/// @param hPostEffect 
	/// @return 
	template<typename T>
	T* GetPostEffectParam(PostEffectHandle hPostEffect)const { return renderContext_->GetPostEffectParam<T>(hPostEffect); }

	/// @brief パラメータを取得する
	/// @tparam T 
	/// @param name 
	/// @return 
	template<typename T>
	T* GetPostEffectParam(const std::string& name)const { return renderContext_->GetPostEffectParam<T>(name); }



	/// @brief 3D衝突読み込み
	/// @param name 
	/// @param type 
	/// @return 
	Collision3DHandle LoadCollision3D(const std::string& name, Engine::Collision3D::Type type)const
	{
		return renderContext_->LoadCollision3D(name, type);
	}

	/// @brief インスタンスを作成する
	/// @tparam T 
	/// @param hCollision 
	/// @return 
	template<typename T>
	T* CreateCollision3DInstance(Collision3DHandle hCollision)const { return renderContext_->CreateCollision3DInstance<T>(hCollision); }

	/// @brief インスタンスを作成する
	/// @tparam T 
	/// @param name 
	/// @return 
	template<typename T>
	T* CreateCollision3DInstance(const std::string& name)const { return renderContext_->CreateCollision3DInstance<T>(name); }

	/// @brief 衝突対象の設定
	/// @param hCollision 
	/// @param hTargetCollision 
	void SetCollision3DTarget(Collision3DHandle hCollision, Collision3DHandle hTargetCollision)const { renderContext_->SetCollision3DTarget(hCollision, hTargetCollision); }

	/// @brief 衝突対象の設定
	/// @param name 
	/// @param targetName 
	void SetCollision3DTarget(const std::string& name, const std::string& targetName)const { return renderContext_->SetCollision3DTarget(name, targetName); }



	/// @brief 2D衝突読み込み
	/// @param name 
	/// @param type 
	/// @return 
	Collision2DHandle LoadCollision2D(const std::string& name, Engine::Collision2D::Type type)const
	{
		return renderContext_->LoadCollision2D(name, type);
	}

	/// @brief インスタンスを作成する
	/// @tparam T 
	/// @param hCollision 
	/// @return 
	template<typename T>
	T* CreateCollision2DInstance(Collision2DHandle hCollision) const { return renderContext_->CreateCollision2DInstance<T>(hCollision); }

	/// @brief インスタンスを作成する
	/// @tparam T 
	/// @param name 
	/// @return 
	template<typename T>
	T* CreateCollision2DInstance(const std::string& name) const { return renderContext_->CreateCollision2DInstance<T>(name); }

	/// @brief 衝突対象の設定
	/// @param hCollision 
	/// @param hTargetCollision 
	void SetCollision2DTarget(Collision2DHandle hCollision, Collision2DHandle hTargetCollision)const { renderContext_->SetCollision2DTarget(hCollision, hTargetCollision); }

	/// @brief 衝突対象の設定
	/// @param name 
	/// @param targetName 
	void SetCollision2DTarget(const std::string& name, const std::string& targetName)const { return renderContext_->SetCollision2DTarget(name, targetName); }



	/// @brief 3Dパーティクルを読み込む
	/// @param name 
	/// @param numInstance 
	/// @param hModel 
	/// @return 
	Particle3DHandle LoadParticle3D(const std::string& name, uint32_t numInstance, uint32_t numEmitter, ModelHandle hModel) const
	{
		return renderContext_->LoadParticle3D(name, numInstance,numEmitter, hModel, log_.get());
	}

	/// @brief 3Dパーティクルを描画する
	/// @param hParticle 
	void DrawParticle3D(Particle3DHandle hParticle)const { renderContext_->DrawParticle3D(hParticle); }

	/// @brief 3Dパーティクルを描画する
	/// @param name 
	void DrawParticle3D(const std::string& name)const { renderContext_->DrawParticle3D(name); }

	/// @brief 3Dパーティクルのパラメータを取得する
	/// @param hParticle 
	/// @return 
	Engine::Particle3D::Param* GetParticle3DParam(Particle3DHandle hParticle)const { return renderContext_->GetParticle3DParam(hParticle); }

	/// @brief 3Dパーティクルのパラメータを取得する
	/// @param name 
	/// @return 
	Engine::Particle3D::Param* GetParticle3DParam(const std::string& name)const { return renderContext_->GetParticle3DParam(name); }

	/// @brief 3Dパーティクルを放出する
	/// @param hParticle 
	/// @param emitterIndex 
	void EmittParticle3D(Particle3DHandle hParticle, int32_t emitterIndex)const { renderContext_->EmittParticle3D(hParticle, emitterIndex); }

	/// @brief 3Dパーティクルを放出する
	/// @param name 
	/// @param emitterIndex 
	void EmittParticle3D(const std::string& name, int32_t emitterIndex)const { renderContext_->EmittParticle3D(name, emitterIndex); }

	/// @brief 3Dパーティクルを停止する
	/// @param hParticle 
	/// @param emitterIndex 
	void StopParticle3D(Particle3DHandle hParticle, int32_t emitterIndex)const { renderContext_->StopParticle3D(hParticle, emitterIndex); }

	/// @brief 3Dパーティクルを停止する
	/// @param name 
	/// @param emitterIndex 
	void StopParticle3D(const std::string& name, int32_t emitterIndex)const { renderContext_->StopParticle3D(name, emitterIndex); }



	/// @brief アニメーションの時間を取得する
	/// @param hAnimation 
	/// @return 
	float GetAnimationTime(AnimationHandle hAnimation)const { return renderContext_->GetAnimationDuration(hAnimation); }



	/// @brief デバッグ用の線を描画する
	/// @param start 
	/// @param end 
	/// @param color 
	void DrawDebugLine3D(const Vector3& start, const Vector3& end, const Vector4& color) const
	{
#ifdef _DEVELOPMENT
		renderContext_->DrawDebugLine3D(start, end, color);
#endif
	}

	/// @brief デバッグ用の線を描画する
	/// @param start 
	/// @param end 
	/// @param color 
	void DrawDebugLine2D(const Vector2& start, const Vector2& end, const Vector4& color) const
	{
#ifdef _DEVELOPMENT
		renderContext_->DrawDebugLine2D(start, end, color);
#endif
	}

	/// @brief デバッグ用の三角形を描画する
	/// @param v1 
	/// @param v2 
	/// @param v3 
	/// @param color 
	void DrawDebugTriangle3D(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector4& color) const
	{
#ifdef _DEVELOPMENT
		renderContext_->DrawDebugTriangle3D(v1, v2, v3, color);
#endif
	}

	/// @brief デバッグ用立方体の描画
	/// @param position 
	/// @param rotate 
	/// @param scale 
	/// @param color 
	void DrawDebugCube(const Vector3& position, const Vector3& rotate, const Vector3& scale, const Vector4& color) const
	{
#ifdef _DEVELOPMENT
		renderContext_->DrawDebugCube(position, rotate, scale, color);
#endif
	}



public:

	/// @brief ライトのパラメータを取得する
	/// @param handle 
	/// @return 
	template <typename T>
	T* GetLightParam(LightHandle handle) const { return renderContext_->GetLightParam<T>(handle); }

	/// @brief ライトのパラメータを取得する
	/// @tparam T 
	/// @param name 
	/// @return 
	template<typename T>
	T* GetLightParam(const std::string& name)const { return renderContext_->GetLightParam<T>(name); }


public:


	/// @brief プリミティブの静的モデルを読み込む
	/// @param hModel 
	/// @param name 
	/// @return 
	Render3DHandle LoadPrimitiveStaticModel(ModelHandle hModel, const std::string& name) const
	{
		return renderContext_->LoadRender3D(0,hModel, 0, 0, name, Engine::Render3D::Type::StaticModel, log_.get());
	}

	/// @brief プリミティブのアニメーションモデルを読み込む
	/// @param hModel 
	/// @param hAnimation 
	/// @param name 
	/// @return 
	Render3DHandle LoadPrimitiveAnimationModel(ModelHandle hModel, AnimationHandle hAnimation, const std::string& name) const
	{
		return renderContext_->LoadRender3D(0,hModel, hAnimation, 0, name, Engine::Render3D::Type::AnimationModel, log_.get());
	}

	/// @brief プリミティブのスキニングモデルを読み込む
	/// @param hModel 
	/// @param hAnimation 
	/// @param hSkeleton 
	/// @param name 
	/// @return 
	Render3DHandle LoadPrimitiveSkinningModel(ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton, const std::string& name) const
	{
		return renderContext_->LoadRender3D(0, hModel, hAnimation, hSkeleton, name, Engine::Render3D::Type::SkinningModel, log_.get());
	}

	/// @brief UV球を読み込む
	/// @param hTexture 
	/// @param name 
	/// @return 
	Render3DHandle LoadUVSphere(TextureHandle hTexture, const std::string& name) const
	{
		return renderContext_->LoadRender3D(hTexture, 0, 0, 0, name, Engine::Render3D::Type::UVSphere, log_.get());
	}

	/// @brief リングを読み込む
	/// @param hTexture 
	/// @param name 
	/// @return 
	Render3DHandle LoadRing(TextureHandle hTexture, const std::string& name) const
	{
		return renderContext_->LoadRender3D(hTexture, 0, 0, 0, name, Engine::Render3D::Type::Ring, log_.get());
	}

	/// @brief 円柱を読み込む
	/// @param hTexture 
	/// @param name 
	/// @return 
	Render3DHandle LoadCylinder(TextureHandle hTexture, const std::string& name) const
	{
		return renderContext_->LoadRender3D(hTexture, 0, 0, 0, name, Engine::Render3D::Type::Cylinder, log_.get());
	}

	/// @brief スプライトを読み込む
	/// @param hTexture 
	/// @param name 
	/// @return 
	Render2DHandle LoadSprite(TextureHandle hTexture, const std::string& name) const
	{
		return renderContext_->LoadRender2D(name, Engine::Render2D::Type::Sprite, hTexture, 0, log_.get());
	}

	/// @brief テキストを読み込む
	/// @param hText 
	/// @param name 
	/// @return 
	Render2DHandle LoadText(TextHandle hText, const std::string& name) const
	{
		return renderContext_->LoadRender2D(name, Engine::Render2D::Type::Text, 0, hText, log_.get());
	}


private:

	// 生成できないようにする
	GrowthEngine() = default;
	GrowthEngine(GrowthEngine&) = delete;
	GrowthEngine& operator=(GrowthEngine&) = delete;

	// インスタンス
	static std::unique_ptr<GrowthEngine> instance_;

	/// @brief 初期化
	/// @param screenWidth 
	/// @param screenHeight 
	/// @param title 
	void Initialize(int32_t screenWidth, int32_t screenHeight, const std::string& title);


private:

	// ログ
	std::unique_ptr<Engine::Log> log_ = nullptr;

	// ウィンドウアプリケーション
	std::unique_ptr<Engine::WinApp> winApp_ = nullptr;

	// 入力
	std::unique_ptr<Engine::Input> input_ = nullptr;

	// オーディオストア
	std::unique_ptr<Engine::AudioStore> audioStore_ = nullptr;

	/// @brief サウンドストア
	std::unique_ptr<Engine::SoundStore> soundStore_ = nullptr;

	/// @brief 入力ストア
	std::unique_ptr<Engine::InputStore> inputStore_ = nullptr;

	// 描画統括
	std::unique_ptr<Engine::RenderContext> renderContext_ = nullptr;


private:

	/// @brief タイムスケールのタイマーを更新する
	/// @param realDeltaTime 
	void UpdateTimeScale();

	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;

	// 前のフレームの時間
	TimePoint previousTime_;

	// デルタタイム
	float deltaTime_ = 0.0f;

	/// @brief 時間のスケール
	float timeScale_ = 1.0f;

	/// @brief スロー状態の継続時間
	float slowDuration_ = 0.0f;

	// 最初の2フレームはデルタタイムは0にするためのフラグ
	bool isDeltaTimeFirst_ = true;
	bool isDeltaTimeSecond_ = true;


private:

	bool isPushFullscreenButton_ = false;
};

