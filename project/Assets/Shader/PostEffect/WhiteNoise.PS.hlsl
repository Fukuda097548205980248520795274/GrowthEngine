#include "../Fullscreen/Fullscreen.hlsli"

// ピクセルシェーダ出力
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct WhiteNoiseParam
{
    // 時間
    float time;
};
ConstantBuffer<WhiteNoiseParam> gParams : register(b0);

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
    if(gParams.time == 0.0f)
        time = 1.0f;
    
    // テクスチャ座標で乱数を生成
    float random = rand2dTo1d(input.texcoord * time);
    
    PixelShaderOutput output;
    output.color = float4(random, random, random, 1.0) * gTexture.Sample(gSampler, input.texcoord);
    return output;
}