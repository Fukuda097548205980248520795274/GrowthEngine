#include "../Fullscreen/Fullscreen.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// ガウスブラーのパラメータ
struct Param
{
    // ブラー方向（例：水平ブラーなら(1, 0)、垂直ブラーなら(0, 1)）
    float2 blurDir;
    
    // ガウス関数の標準偏差
    float sigma;
    
    // ガウス関数の半径
    int radius;
};
ConstantBuffer<Param> gParam : register(b0);

// 1Dガウス関数
float gauss1D(float x, float sigma)
{
    return exp(-(x * x) / (2.0f * sigma * sigma));
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = float4(0.0f, 0.0f, 0.0f, 1.0f);

    float totalWeight = 0.0f;

    // 指定された半径で1Dブラーを実行
    [loop]
    for (int i = -gParam.radius; i <= gParam.radius; ++i)
    {
        float weight = gauss1D((float) i, gParam.sigma);
        float2 uvOffset = gParam.blurDir * (float) i;
        
        float3 color = gTexture.Sample(gSampler, input.texcoord + uvOffset).rgb;
        output.color.rgb += color * weight;
        totalWeight += weight;
    }

    // 重みの総和で正規化
    output.color.rgb /= totalWeight;

    return output;
}