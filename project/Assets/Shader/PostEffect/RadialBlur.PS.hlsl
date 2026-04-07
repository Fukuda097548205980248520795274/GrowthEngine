#include "../Fullscreen/Fullscreen.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct RadialBlurParam
{
    // ブラーの中心座標
    float2 center;
    
    // ブラーのサンプル数
    int samples;

    // ブラーの強さ
    float power;
};
ConstantBuffer<RadialBlurParam> gParams : register(b0);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// シンプルな疑似乱数生成関数
float rand(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898f, 78.233f))) * 43758.5453f);
}

PixelShaderOutput main(VertexShaderOutput input)
{
    // 初期化
    PixelShaderOutput output;
    output.color = float4(0.0f, 0.0f, 0.0f, 1.0f);

    // サンプル数が0以下の場合はブラーを使用しない
    if (gParams.samples <= 0)
    {
        output.color.rgb = gTexture.SampleLevel(gSampler, input.texcoord, 0).rgb;
        return output;
    }

    // ブラーの中心へ向かう方向ベクトル
    float2 direction = gParams.center - input.texcoord;
    
    // ブラーの強さとサンプル数に基づいてステップを計算
    float2 step = (direction * gParams.power) / float(gParams.samples);

    // 乱数を生成してサンプリング位置をランダムにずらす（ジッタリング）
    float jitter = rand(input.texcoord) - 0.5f;

    float3 outputColor = float3(0.0f, 0.0f, 0.0f);

    // サンプル数に基づいてブラーを適用
    for (int i = 0; i < gParams.samples; ++i)
    {
        // サンプリング位置を計算
        float2 texcoord = input.texcoord + step * (float(i) + jitter);
        
        // テクスチャからサンプルを取得して加算
        outputColor += gTexture.SampleLevel(gSampler, texcoord, 0).rgb;
    }

    // 平均化
    output.color.rgb = outputColor * rcp(float(gParams.samples));
    
    return output;
}