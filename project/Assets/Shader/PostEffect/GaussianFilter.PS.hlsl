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

// 円周率
static const float PI = 3.14159265f;

// ガウス関数
float gauss(float x, float y , float sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denomnator = 2.0f * PI * sigma * sigma;
    
    // ガウス関数の値を返す
    return exp(exponent) * rcp(denomnator);
}

PixelShaderOutput main(VertexShaderOutput input)
{
    // 5x5カーネル
    float kernel5x5[5][5];
    
    // カーネルの重みの合計
    float weight = 0.0f;
    
    for (int y = 0; y < 5; y++)
        for(int x = 0; x < 5; x++)
        {
            kernel5x5[y][x] = gauss(kIndex5x5[y][x].x, kIndex5x5[y][x].y, 2.0f);
            weight += kernel5x5[y][x];
        }

    
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
            output.color.rgb += fetchColor * kernel5x5[y][x];
        }
    
    output.color.rgb *= rcp(weight); // 重みの合計で割る
    
    return output;
}