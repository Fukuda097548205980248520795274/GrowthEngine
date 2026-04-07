#include "../Fullscreen/Fullscreen.hlsli"

// ピクセルシェーダ出力
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// 定数バッファ
struct OutlineParam
{
    // RGBチャンネルの重み
    float3 colorWeight;
    
    // 閾値
    float intensity;
};
ConstantBuffer<OutlineParam> gParams : register(b0);

// 3x3のインデックス
static const float2 kIndex3x3[3][3] =
{
    { float2(-1.0f, -1.0f), float2(0.0f, -1.0f), float2(1.0f, -1.0f)},
    { float2(-1.0f, 0.0f), float2(0.0f, 0.0f), float2(1.0f, 0.0f) },
    { float2(-1.0f, 1.0f), float2(0.0f, 1.0f), float2(1.0f, 1.0f) }
};

// Prewittフィルタのカーネル
static const float kPrewittHorizontalKernel[3][3] =
{
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f }
};

// Prewittフィルタのカーネル
static const float kPrewittVerticalKernel[3][3] =
{
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f }
};

// 輝度の計算
float Luminance(float3 color)
{
    return dot(color, gParams.colorWeight);
}

PixelShaderOutput main(VertexShaderOutput input)
{
    // テクスチャの幅と高さを取得
    uint width, height;
    gTexture.GetDimensions(width, height);
    
    // テクスチャ座標のステップサイズを計算
    float2 upStepSize = float2(rcp(float(width)), rcp(float(height)));
    
    // 縦横のそれぞれの畳み込みの結果を格納する変数
    float2 difference = float2(0.0f, 0.0f);
    
    for (int y = 0; y < 3; y++)
        for (int x = 0; x < 3; x++)
        {
            // テクスチャ座標を計算
            float2 texcoord = input.texcoord + kIndex3x3[y][x] * upStepSize;
            
            // テクスチャから色をサンプリング
            float3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            
            // 輝度を計算
            float luminance = Luminance(fetchColor);
            
            // Prewittフィルタのカーネルを適用して、縦横のそれぞれの畳み込みの結果を計算
            difference.x += luminance * kPrewittHorizontalKernel[y][x];
            difference.y += luminance * kPrewittVerticalKernel[y][x];
        }
    
    // 畳み込みの結果からエッジの強さを計算
    float weight = length(difference);
    weight = saturate(weight * gParams.intensity);
    
    PixelShaderOutput output;
    output.color.rgb = (1.0f - weight) * gTexture.Sample(gSampler, input.texcoord).rgb;
    output.color.a = 1.0f;
    
    return output;
}