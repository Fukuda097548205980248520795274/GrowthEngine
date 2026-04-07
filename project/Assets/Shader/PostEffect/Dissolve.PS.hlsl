#include "../Fullscreen/Fullscreen.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSample : register(s0);

Texture2D<float> gMask : register(t1);

// パラメータ
struct DissolveParam
{
    // 閾値
    float threshold;
    
    // エッジの幅
    float edgeDiff;
    
    // エッジの色
    float3 edgeColor;
};
ConstantBuffer<DissolveParam> gParams : register(b0);

// スムースステップ関数
float smoothstep(float edge0, float edge1, float x)
{
    // 0.0から1.0の範囲にxをクランプ
    x = saturate((x - edge0) / (edge1 - edge0));
    
    // スムースステップ関数の計算
    return x * x * (3 - 2 * x);
}

PixelShaderOutput main(VertexShaderOutput input)
{
    float mask = gMask.Sample(gSample, input.texcoord);
    
    // マスクの値が閾値未満の場合はピクセルを破棄
    if(mask <= gParams.threshold)
    {
        discard;
    }
    
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSample, input.texcoord);
    
    // エッジの幅に基づいてエッジの強さを計算
    float edge = 1.0f - smoothstep(gParams.threshold, gParams.threshold + gParams.edgeDiff, mask);
    
    // エッジの色を加算
    output.color.rgb += gParams.edgeColor * edge;
    
    return output;
}