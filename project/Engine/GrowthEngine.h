#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <Windows.h>

#include "Log/Log.h"
#include "WinApp/WinApp.h"
#include "Input/Input.h"
#include "Store/AudioStore/AudioStore.h"
#include "Store/InputStore/InputStore.h"
#include "RenderContext/RenderContext.h"

#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Vector/VectorInt3/VectorInt3.h"

#include "Application/Framework/Framework.h"

#include "Application/GameCamera/GameCamera3D/GameCamera3D.h"
#include "Application/GameCamera/GameCamera2D/GameCamera2D.h"

#include "Application/Render3D/Render3DStaticModel/Render3DStaticModel.h"
#include "Application/Render3D/Render3DAnimationModel/Render3DAnimationModel.h"
#include "Application/Render3D/Render3DSkinningModel/Render3DSkinningModel.h"

#include "Application/Sprite/Sprite.h"

#include "Application/PrefabInstance/PrefabInstanceSprite/PrefabInstanceSprite.h"
#include "Application/PrefabBase/PrefabBaseSprite/PrefabBaseSprite.h"

#include "Application/PrefabInstance/PrefabInstanceStaticModel/PrefabInstanceStaticModel.h"
#include "Application/PrefabBase/PrefabBaseStaticModel/PrefabBaseStaticModel.h"

#include "Application/PostEffect/PostEffectRadialBlur/PostEffectRadialBlur.h"

#include "Application/AudioObject/AudioObject.h"

#include "Application/Scene/Scene.h"
#include "Application/SceneManager/SceneManager.h"

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

	/// @brief 新フレーム処理
	void NewFrame();

	/// @brief 描画前処理
	void PreDraw();

	/// @brief 描画後処理
	void PostDraw();

	/// @brief デルタタイムを取得する
	/// @return 
	float GetDeltaTime()const { return deltaTime_; }

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



	/// @brief オーディオを再生する
	/// @param ah 
	/// @param volume 
	/// @return 
	PlayHandle PlayAudio(AudioHandle ah, float volume)const { return audioStore_->PlayAudio(ah, volume); }

	/// @brief オーディオを停止する
	/// @param ph 
	void StopAudio(PlayHandle ph)const { audioStore_->StopAudio(ph); }

	/// @brief オーディオが再生されているかどうか
	/// @param ph 
	/// @return 
	bool IsPlayAudio(PlayHandle ph)const { return audioStore_->IsAudioPlay(ph); }

	/// @brief ボリュームの設定
	/// @param ph 
	/// @param volume 
	void SetVolume(PlayHandle ph, float volume)const { audioStore_->SetVolume(ph, volume); }

	/// @brief ピッチの設定
	/// @param ph 
	/// @param pitch 
	void SetPitch(PlayHandle ph, float pitch)const { return audioStore_->SetPitch(ph, pitch); }


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
	Render3DHandle LoadRender3D(ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton, const std::string& name, Engine::Render3D::Type type) const
	{
		return renderContext_->LoadRender3D(hModel, hAnimation, hSkeleton, name, type, log_.get());
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



	/// @brief スプライトを読み込む
	/// @param hTexture 
	/// @param name 
	/// @return 
	Render2DHandle LoadRender2D(TextureHandle hTexture, const std::string& name) const
	{
		return renderContext_->LoadRender2D(hTexture, name, log_.get());
	}

	/// @brief スプライトを描画する
	/// @param handle 
	void DrawRender2D(Render2DHandle handle)const { renderContext_->DrawRender2D(handle); }

	/// @brief スプライトを描画する
	/// @param name 
	void DrawRender2D(const std::string& name)const { renderContext_->DrawRender2D(name); }



	/// @brief プリミティブ用prefabの読み込み
	/// @param name 
	/// @param type 
	/// @param numInstance 
	/// @param hModel 
	/// @param hAnimation 
	/// @param hSkeleton 
	/// @return 
	Prefab3DHandle LoadPrefabPrimitive(const std::string& name, Engine::Prefab3D::Type type,uint32_t numInstance,
		TextureHandle hTexture, ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton) const
	{
		return renderContext_->LoadPrefabPrimitive(name, type, numInstance, hTexture, hModel, hAnimation, hSkeleton, log_.get());
	}

	/// @brief プレハブスプライトの読み込み
	/// @param name 
	/// @param numInstance 
	/// @param hTexture 
	/// @return 
	Prefab2DHandle LoadPrefabSprite(const std::string& name, uint32_t numInstance, TextureHandle hTexture) const
	{
		return renderContext_->LoadPrefabSprite(name, hTexture, numInstance, log_.get());
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

	/// @brief スプライト用プレハブのパラメータを取得する
	/// @return 
	Engine::Prefab2D::Sprite::Base::Param* GetPrefab2DParam(Prefab2DHandle hPrefab2D)const { return renderContext_->GetPrefab2DParam(hPrefab2D); }

	/// @brief スプライト用プレハブのパラメータを取得する
	/// @param name 
	/// @return 
	Engine::Prefab2D::Sprite::Base::Param* GetPrefab2DParam(const std::string& name)const { return renderContext_->GetPrefab2DParam(name); }


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

	/// @brief スプライト用インスタンスを作成する
	/// @param hPrefabSprite 
	/// @return 
	PrefabInstanceSprite* CreatePrefab2DInstance(Prefab2DHandle hPrefab2D) const { return renderContext_->CreatePrefab2DInstance(hPrefab2D); }

	/// @brief スプライト用インスタンスを作成する
	/// @param name 
	/// @return 
	PrefabInstanceSprite* CreatePrefab2DInstance(const std::string& name)const { renderContext_->CreatePrefab2DInstance(name); }



	/// @brief ポストエフェクトを読み込む
	/// @param name 
	/// @param type 
	/// @return 
	PostEffectHandle LoadPostEffect(const std::string& name, Engine::PostEffect::Type type)const { return renderContext_->LoadPostEffect(name, type, log_.get()); }

	/// @brief ポストエフェクトを描画する
	/// @param hPostEffect 
	void DrawPostEffect(PostEffectHandle hPostEffect)const { return renderContext_->DrawPostEffect(hPostEffect); }

	/// @brief パメータを取得する
	/// @tparam T 
	/// @param hPostEffect 
	/// @return 
	template<typename T>
	T* GetPostEffectParam(PostEffectHandle hPostEffect)const { return renderContext_->GetPostEffectParam<T>(hPostEffect); }


	/// @brief 全てのプレハブインスタンスを削除する
	void DestroyAllPrefabInstance() const { renderContext_->DestroyAllInstance(); }



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

	/// @brief ライトを設置する
	/// @param hLight 
	void SetLight(LightHandle hLight)const { renderContext_->SetLight(hLight); }

	/// @brief ライトを設置する
	/// @param name 
	void SetLight(const std::string& name)const { return renderContext_->SetLight(name); }


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

	/// @brief 入力ストア
	std::unique_ptr<Engine::InputStore> inputStore_ = nullptr;

	// 描画統括
	std::unique_ptr<Engine::RenderContext> renderContext_ = nullptr;


private:

	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;

	TimePoint previousTime_;
	float deltaTime_ = 0.0f;


private:

	bool isPushFullscreenButton_ = false;


#ifdef _DEVELOPMENT

	/// @brief メニューバー
	void MenuBer();

	/// @brief シーン追加
	void CreateScene();

	/// @brief シーンファイルを作成する
	/// @param fileName 
	void CreateSceneFile(const std::string& fileName);


	// シーン生成フラグ
	bool isSceneCreate_ = false;

#endif
};

