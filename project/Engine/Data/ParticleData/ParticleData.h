#pragma once
#include <cmath>
#include <vector>
#include <string>
#include "Handle/Handle.h"
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Vector/Vector4/Vector4.h"
#include "PSO/EnumBlendMode/EnumBlendMode.h"

namespace Engine
{
	namespace Particle3D
	{
		// エミッターの図形
		enum class EmitterShape
		{
			Point,
			AABB,
			Sphere
		};

		// 引力の位置のタイプ
		enum AttractPostitionType
		{
			Direction,
			Position
		};

		struct Emitter
		{
			// エミッターの名前
			std::string name;

			// 位置
			Vector3 position;

			// 放出開始
			bool isStart;

			// タイマー
			float timer;
		};

		/// @brief 色
		struct Color
		{
			// ランダムな色を使用するかどうか
			bool randomColor;

			Vector4 start;

			Vector4 end;

			// アルファ値の開始値
			float startAlpha;

			// アルファ値の終了値
			float endAlpha;
		};

		/// @brief 大きさ
		struct Scale
		{
			Vector3 start;

			Vector3 end;
		};

		/// @brief 速度
		struct Speed
		{
			float start;

			float end;
		};

		/// @brief 生存時間
		struct LifeTime
		{
			float min;

			float max;
		};

		// 放出時間
		struct EmitTime
		{
			// ループするかどうか
			bool isLoop;

			// 放出時間
			float emit;

			// 待機時間
			float pause;
		};

		// 回転
		struct Rotate
		{
			// ランダム
			bool isRandom;

			// 回転の軸
			Vector3 axis;

			// 回転の初期値
			float start;

			// 回転の最後の値
			float end;
		};

		struct Attract
		{
			// 引力有効化
			bool enableAttract;

			// 引力の位置のタイプ
			AttractPostitionType positionType;

			// 引力の加速度
			float attractAcceleration;


			// 引力の中心点
			Vector3 attractCenter;

			// 引力の方向
			Vector3 attractDirection;

			// 引力の位置
			float attractLength;


			// 引力の位置
			Vector3 attractPosition;


			// エミッターと引力の位置を入れ替える
			bool swapEmitterAttract;
		};

		/// @brief パラメータ
		struct Param
		{
			// ブレンドモード
			BlendMode blendMode = BlendMode::kAdd;

			// 位置
			std::vector<Emitter> emitter;

			// エミッターの図形
			EmitterShape shape;

			/// @brief 放出位置の半径
			float radius1;

			/// @brief 放出位置の半径
			Vector3 radius3;


			/// @brief 色
			Color color;

			/// @brief 大きさ
			Scale scale;

			/// @brief 速度
			Speed speed;

			/// @brief 生存期間
			LifeTime lifeTime;

			/// @brief 放出時間
			EmitTime emitTime;

			// 回転
			Rotate rotate;

			/// @brief 放出数
			int32_t count;

			/// @brief 放出間隔
			float frequency;

			/// @brief ビルボード有効化
			bool enableBillboard;

			/// @brief ソフトパーティクル有効化
			bool enableSoftParticle;


			// 引力
			Attract attract;


			/// @brief テクスチャハンドル
			TextureHandle hTexture;

			/// @brief モデルハンドル
			ModelHandle hModel;
		};
	}
}