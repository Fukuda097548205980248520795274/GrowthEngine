#include "../Fullscreen/Fullscreen.hlsli"

// ピクセルシェーダ出力
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// 5x5のインデックス
static const float2 kIndex5x5[5][5] =
{
    { float2(-2.0f, -2.0f), float2(-1.0f, -2.0f), float2(0.0f, -2.0f), float2(1.0f, -2.0f), float2(2.0f, -2.0f) },
    { float2(-2.0f, -1.0f), float2(-1.0f, -1.0f), float2(0.0f, -1.0f), float2(1.0f, -1.0f), float2(2.0f, -1.0f) },
    { float2(-2.0f, 0.0f), float2(-1.0f, 0.0f), float2(0.0f, 0.0f), float2(1.0f, 0.0f), float2(2.0f, 0.0f) },
    { float2(-2.0f, 1.0f), float2(-1.0f, 1.0f), float2(0.0f, 1.0f), float2(1.0f, 1.0f), float2(2.0f, 1.0f) },
    { float2(-2.0f, 2.0f), float2(-1.0f, 2.0f), float2(0.0f, 2.0f), float2(1.0f, 2.0f), float2(2.0f, 2.0f) }
};

// 5x5のカーネル
static const float kKernel5x5[5][5] =
{
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f }
};

PixelShaderOutput main(VertexShaderOutput input)
{
    // テクスチャの幅と高さを取得
    uint width, height;
    gTexture.GetDimensions(width, height);
    
    // テクスチャ座標のステップサイズを計算
    float2 upStepSize = float2(rcp(float(width)), rcp(float(height)));
    
    PixelShaderOutput output;
    output.color = float4(0.0f, 0.0f, 0.0f, 1.0f); // 初期化
    
    // 5x5のカーネルを適用
    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 5; x++)
        {
            // テクスチャ座標を計算
            float2 texcoord = input.texcoord + kIndex5x5[y][x] * upStepSize;
            
            // 色に 1/25 を掛けて加算
            float3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            output.color.rgb += fetchColor * kKernel5x5[y][x];
        }
    
    return output;
}