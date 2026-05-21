#include "Particle3D.hlsli"
#include "../Particle3D.hlsli"

// 頂点シェーダ入力
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

// パーティクル
StructuredBuffer<Particle> gParticles : register(t0);

// ビュー
struct PerView
{
    // ビュープロジェクション行列
    float4x4 viewProjection;
    
    // ビルボード行列
    float4x4 billboard;
};
ConstantBuffer<PerView> gView : register(b0);

VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    
    // パーティクルの情報を取得
    Particle particle = gParticles[instanceID];
    
    
    float3x3 rotMatrix = QuaternionToMatrix(particle.rotation);
    
    float4x4 worldMatrix = float4x4(
        float4(rotMatrix[0], 0.0f),
        float4(rotMatrix[1], 0.0f),
        float4(rotMatrix[2], 0.0f),
        float4(0.0f, 0.0f, 0.0f, 1.0f)
    );
    
    worldMatrix[0] *= particle.scale.x;
    worldMatrix[1] *= particle.scale.y;
    worldMatrix[2] *= particle.scale.z;
    
    worldMatrix[3].xyz = particle.translate;
    
    output.position = mul(input.position, mul(mul(worldMatrix, gView.billboard), gView.viewProjection));
    output.texcoord = input.texcoord;
    output.color = particle.color;
    output.clipPos = output.position;
    
    return output;
}