#include "PrefabPrimitive.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

// シャドウ用座標変換
struct ShadowTransformation
{
    float4x4 viewProjection;
};
ConstantBuffer<ShadowTransformation> gShadowTransformation : register(b0);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// シャドウマップ用テクスチャ
Texture2D<float> gShadowMap : register(t1);

// シャドウ用比較サンプラー
SamplerComparisonState gShadowSampler : register(s1);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // UV座標を座標変換する
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), input.uvTransform);
    
    // テクスチャの色
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    
    // ワールド座標をライト空間に変換
    float4 shadowPos = mul(float4(input.worldPosition, 1.0f), gShadowTransformation.viewProjection);
    shadowPos.xyz /= shadowPos.w;
        
        // [-1,1] → [0,1]
    float2 shadowUV = ((float2) shadowPos + float2(1, -1)) * float2(0.5, -0.5);
        
    float shadow = gShadowMap.SampleCmpLevelZero(gShadowSampler, shadowUV, shadowPos.z - 0.005f);
    float shadowFactor = lerp(0.5f, 1.0f, shadow); // 影の濃さ調整
    
    
    // 色
    output.color.rgb = input.color.rgb * textureColor.rgb * shadowFactor;
    output.color.a = input.color.a * textureColor.a;
    
    // a = 0は描画しない
    if (output.color.a == 0.0f || textureColor.a == 0.0f)
    {
        discard;
    }
    
    return output;
}