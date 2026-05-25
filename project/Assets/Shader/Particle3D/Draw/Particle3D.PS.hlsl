#include "Particle3D.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gColorTexture : register(t0);
Texture2D<float> gDepthTexture : register(t1);
SamplerState gSampler : register(s0);

struct Camera
{
    float3 worldPositin;
    float nearZ;
    float farZ;
};
ConstantBuffer<Camera> gCamera : register(b0);

struct Enable
{
    // 0: 通常のパーティクル、1: ソフトパーティクル
    int softParticle;
};
ConstantBuffer<Enable> gEnable : register(b1);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // テクスチャカラーをサンプリング
    float4 textureColor = gColorTexture.Sample(gSampler, input.texcoord);
    float4 finalColor = input.color * textureColor;
    
    // a = 0は描画しない
    if (finalColor.a == 0.0f)
        discard;
    
    if(gEnable.softParticle != 0)
    {
        // 画面UVを計算
        float2 screenUV = (input.clipPos.xy / input.clipPos.w) * 0.5f + 0.5f;
        screenUV.y = 1.0f - screenUV.y; // Y軸を反転

        // シーンの深度を取得
        float sceneDepthZ = gDepthTexture.Sample(gSampler, screenUV);

        // シーンの深度とパーティクルの深度を線形化
        float sceneLinearDepth = (gCamera.nearZ * gCamera.farZ) / (gCamera.farZ - sceneDepthZ * (gCamera.farZ - gCamera.nearZ));
        float particleLinearDepth = (gCamera.nearZ * gCamera.farZ) / (gCamera.farZ - input.clipPos.z / input.clipPos.w * (gCamera.farZ - gCamera.nearZ));

        // 深度差に基づいてフェードを計算
        float fadeScale = 1.0f;
        float depthDiff = sceneLinearDepth - particleLinearDepth;
        float fade = saturate(depthDiff * fadeScale);

        // アルファ値にフェードを適用
        finalColor.a *= fade;
    }

    output.color = finalColor;
    
    return output;
}