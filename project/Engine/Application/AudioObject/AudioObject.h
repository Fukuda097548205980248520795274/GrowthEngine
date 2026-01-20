#pragma once
#include "Handle/Handle.h"
#include <string>


class GrowthEngine;

class AudioObject
{
public:

	/// @brief デストラクタ
	~AudioObject();
	
	/// @brief コンストラクタ
	/// @param filePath 
	/// @param volume 
	/// @param isLoop 
	AudioObject(const std::string& filePath, float volume, bool isLoop);

	/// @brief 更新処理
	void Update();

	/// @brief オーディオを流す
	void PlayAudio();

	/// @brief オーディオを止める
	void StopAudio();

	/// @brief ピッチを設定する
	/// @param pitch 
	void SetPitch(float pitch);

	/// @brief ボリュームを設定する
	/// @param volume 
	void SetVolume(float volume);

	/// @brief ループの設定
	/// @param isLoop 
	void SetLoop(bool isLoop);


private:

	// エンジン
	const GrowthEngine* engine_ = nullptr;



	// ボリューム
	float volume_ = 0.0f;

	// ピッチ
	float pitch_ = 1.0f;

	// ループフラグ
	bool isLoop_ = false;


	// オーディオハンドル
	AudioHandle ah_;

	// プレイハンドル
	PlayHandle ph_;


	// 再生するかどうか
	bool isPlay_ = false;
};

