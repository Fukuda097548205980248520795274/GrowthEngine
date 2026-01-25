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

	/// @brief 描画前処理
	void PreDraw();

	/// @brief 描画後処理
	void PostDraw();

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

