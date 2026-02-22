#include "Particle.hlsli"

// 頂点入力
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

struct Particle
{
    float3 translate;
    float3 scale;
    float lifeTime;
    float3 velocity;
    float currentTime;
    float4 color;
};
StructuredBuffer<Particle> gParticles : register(t0);

struct PreView
{
    float4x4 viewProjection;
    float4x4 billboardMatrix;
};
ConstantBuffer<PreView> gPreView : register(b0);

VertexShaderOutput main(VertexShaderInput input , uint instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    
    Particle particle = gParticles[instanceID];
    
    float4x4 worldMatrix = gPreView.billboardMatrix;
    worldMatrix[0] *= particle.scale.x;
    worldMatrix[1] *= particle.scale.y;
    worldMatrix[2] *= particle.scale.z;
    worldMatrix[3].xyz = particle.translate;
    
    output.position = mul(input.position, mul(worldMatrix, gPreView.viewProjection));
    output.texcoord = input.texcoord;
    output.color = particle.color;
    
    return output;
}