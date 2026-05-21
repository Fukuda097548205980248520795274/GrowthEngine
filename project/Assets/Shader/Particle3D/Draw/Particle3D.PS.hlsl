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

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // 1. パーティクルのテクスチャ色を取得
    float4 textureColor = gColorTexture.Sample(gSampler, input.texcoord);
    float4 finalColor = input.color * textureColor;
    
    // 2. スクリーン座標（UV）の計算
    // clipPosをパースペクティブ除算してNDC座標（-1 ～ 1）にし、UV座標（0 ～ 1）に変換
    float2 screenUV = (input.clipPos.xy / input.clipPos.w) * 0.5f + 0.5f;
    screenUV.y = 1.0f - screenUV.y; // Y軸を反転

    // 3. 深度バッファからシーンの深度を取得
    float sceneDepthZ = gDepthTexture.Sample(gSampler, screenUV);

    // 4. 深度値のリニア化（パースペクティブ投影の場合）
    float sceneLinearDepth = (gCamera.nearZ * gCamera.farZ) / (gCamera.farZ - sceneDepthZ * (gCamera.farZ - gCamera.nearZ));
    float particleLinearDepth = (gCamera.nearZ * gCamera.farZ) / (gCamera.farZ - input.clipPos.z / input.clipPos.w * (gCamera.farZ - gCamera.nearZ));

    // 5. ソフトパーティクルのフェード計算
    // fadeFactorの係数（例: 1.0f）を調整することで、境界のぼやけ具合を変更できます
    float fadeScale = 1.0f;
    float depthDiff = sceneLinearDepth - particleLinearDepth;
    float fade = saturate(depthDiff * fadeScale);

    // アルファ値にフェードを適用
    finalColor.a *= fade;

    output.color = finalColor;
    
    return output;
}