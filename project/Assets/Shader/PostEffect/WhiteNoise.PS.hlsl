#include "../Fullscreen/Fullscreen.hlsli"

// ピクセルシェーダ出力
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// パラメータを拡張
struct NoiseEffectParam
{
    float time; // 時間
    float noiseIntensity; // ノイズの強さ (0.0 ~ 1.0)
    float noiseScale; // ノイズの細かさ (標準は1.0。大きくすると細かくなる)
    float noiseSpeed; // ノイズの変化速度 (標準は1.0)
    
    float isColorNoise; // 1.0でカラーノイズ、0.0でモノクロノイズ
    float scanlineIntensity; // 走査線の強さ (0.0 ~ 1.0)
    float scanlineCount; // 走査線の本数 (例: 800.0)
    float padding; // HLSLの16バイトアライメント用パディング
    
    float4 noiseColor; // ノイズにかける色 (RGB, Aは未使用)
    float4 screenColorFilter; // 画面全体にかける色 (RGB, Aは未使用)
};
ConstantBuffer<NoiseEffectParam> gParams : register(b0);

// 2Dの値を1Dの乱数に変換する関数
float rand2dTo1d(float2 value, float2 dotDir = float2(12.9898, 78.233))
{
    float2 smallValue = sin(value);
    float random = dot(smallValue, dotDir);
    random = frac(sin(random) * 143758.5453);
    return random;
}

PixelShaderOutput main(VertexShaderOutput input)
{
    // 時間が0のときは1にする
    float time = gParams.time;
    if (time == 0.0f)
        time = 1.0f;
    
    // UV座標と時間から乱数を生成 (0.0 ~ 1.0)
    float random = rand2dTo1d(input.texcoord * time);
    
    // 生成したモノクロノイズに「設定した色」を乗算する
    float3 noise = float3(random, random, random) * gParams.noiseColor.rgb;
    
    // 元の画面（背景）の色を取得
    float4 baseColor = gTexture.Sample(gSampler, input.texcoord);
    
    // lerp関数で「元の画面」と「着色した砂嵐」をブレンドする
    // noiseIntensity が 0.0 なら baseColor (ノイズなし)
    // noiseIntensity が 1.0 なら noise (完全な砂嵐)
    float3 finalColor = lerp(baseColor.rgb, noise, gParams.noiseIntensity);
    
    PixelShaderOutput output;
    // アルファ値は元のテクスチャのものを維持
    output.color = float4(finalColor, baseColor.a);
    return output;
}