#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <Windows.h>

#include "Log/Log.h"
#include "WinApp/WinApp.h"
#include "Input/Input.h"
#include "Store/AudioStore/AudioStore.h"
#include "RenderContext/RenderContext.h"

#include "Math/Vector/Vector3/Vector3.h"

#include "Application/Framework/Framework.h"

#include "Application/Camera/GameCamera3D/GameCamera3D.h"
#include "Application/Camera/GameCamera2D/GameCamera2D.h"

#include "Application/Primitive/PrimitiveStaticModel/PrimitiveStaticModel.h"
#include "Application/Primitive/PrimitiveAnimationModel/PrimitiveAnimationModel.h"
#include "Application/Primitive/PrimitiveSkinningModel/PrimitiveSkinningModel.h"

#include "Application/Sprite/Sprite.h"

#include "Application/PrefabInstance/PrefabInstanceSprite/PrefabInstanceSprite.h"
#include "Application/PrefabBase/PrefabBaseSprite/PrefabBaseSprite.h"

#include "Application/PostEffect/PostEffectRadialBlur/PostEffectRadialBlur.h"

#include "Application/AudioObject/AudioObject.h"

#include "Application/Scene/Scene.h"
#include "Application/SceneManager/SceneManager.h"


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

	/// @brief 3Dカメラの切り替え
	/// @param hCamera 
	void Camera3DSwitch(Camera3DHandle hCamera) const { renderContext_->Camera3DSwitch(hCamera); }

	/// @brief 2Dカメラの切り替え
	/// @param hCamera 
	void Camera2DSwitch(Camera2DHandle hCamera)const { renderContext_->Camera2DSwitch(hCamera); }

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
	TextureHandle LoadTexture(const std::string& filePath) const {return renderContext_->LoadTexture(filePath, log_.get()); }

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
	LightHandle LoadLight(const std::string& name, const std::string& type) const { return renderContext_->LoadLight(name, type, log_.get()); }



	/// @brief オーディオを再生する
	/// @param ah 
	/// @param volume 
	/// @return 
	PlayHandle PlayAudio(AudioHandle ah , float volume)const { return audioStore_->PlayAudio(ah , volume); }

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
	PrimitiveHandle LoadPrimitive(ModelHandle hModel, AnimationHandle hAnimation,SkeletonHandle hSkeleton, const std::string& name, Engine::Primitive::Type type) const
	{
		return renderContext_->LoadPrimitive(hModel, hAnimation, hSkeleton, name, type, log_.get());
	}

	/// @brief プリミティブを描画する
	/// @param handle 
	void DrawPrimitive(PrimitiveHandle handle)const { renderContext_->DrawPrimitive(handle); }

	/// @brief プリミティブのパラメータを取得する
	/// @tparam T 
	/// @param handle 
	/// @return 
	template<typename T>
	T* GetPrimitiveParam(PrimitiveHandle handle)const { return renderContext_->GetPrimitiveParam<T>(handle); }



	/// @brief スプライトを読み込む
	/// @param hTexture 
	/// @param name 
	/// @return 
	SpriteHandle LoadSprite(TextureHandle hTexture, const std::string& name) const
	{
		return renderContext_->LoadSprite(hTexture, name, log_.get());
	}

	/// @brief スプライトを描画する
	/// @param handle 
	void DrawSprite(SpriteHandle handle)const { renderContext_->DrawSprite(handle); }



	/// @brief プレハブスプライトの読み込み
	/// @param name 
	/// @param numInstance 
	/// @param hTexture 
	/// @return 
	PrefabSpriteHandle LoadPrefabSprite(const std::string& name, uint32_t numInstance, TextureHandle hTexture) const
	{
		return renderContext_->LoadPrefabSprite(name, hTexture, numInstance, log_.get());
	}

	/// @brief コマンドリストに登録する
	/// @param hSprite 
	void DrawPrefabSprite(PrefabSpriteHandle hPrefabSprite)const { renderContext_->DrawPrefabSprite(hPrefabSprite); }

	/// @brief スプライト用プレハブのパラメータを取得する
	/// @return 
	Engine::Prefab::Sprite::Base::Param* GetPrefabSpriteParam(PrefabSpriteHandle hPrefabSprite)const { return renderContext_->GetPrefabSpriteParam(hPrefabSprite); }

	/// @brief インスタンスを作成する
	/// @param hPrefabSprite 
	/// @return 
	PrefabInstanceSprite* CreateSpriteInstance(PrefabSpriteHandle hPrefabSprite) const { return renderContext_->CreateSpriteInstance(hPrefabSprite); }



	/// @brief ポストエフェクトを読み込む
	/// @param name 
	/// @param type 
	/// @return 
	PostEffectHandle LoadPostEffect(const std::string& name, Engine::PostEffect::Type type)const { return renderContext_->LoadPostEffect(name, type, log_.get()); }

	/// @brief ポストエフェクトを描画する
	/// @param hPostEffect 
	void DrawPostEffect(PostEffectHandle hPostEffect)const { return renderContext_->DrawPostEffect(hPostEffect); }

	/// @brief 全てのプレハブインスタンスを削除する
	void DestroyAllPrefabInstance() const{}



public:

	/// @brief ライトのパラメータを取得する
	/// @param handle 
	/// @return 
	template <typename T>
	T* GetLightParam(LightHandle handle) const { return renderContext_->GetLightParam<T>(handle); }


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

	// 描画統括
	std::unique_ptr<Engine::RenderContext> renderContext_ = nullptr;


private:


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

