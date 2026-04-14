#include "Particle3D.hlsli"

// 頂点シェーダ入力
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

// パーティクル
struct Particle
{
    // 位置
    float3 translate;
    
    // 生存時間
    float lifeTime;
    
    // 大きさ
    float3 scale;

    // 現在の時間
    float currentTime;
    
    // 色
    float4 color;
    
    // 速度
    float3 direction;
    
    // 放出位置
    float3 emitPos;
};
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
    
    // ワールド行列を作成
    float4x4 worldMatrix = gView.billboard;
    worldMatrix[0] *= particle.scale.x;
    worldMatrix[1] *= particle.scale.y;
    worldMatrix[2] *= particle.scale.z;
    worldMatrix[3].xyz = particle.translate;
    
    output.position = mul(input.position, mul(worldMatrix, gView.viewProjection));
    output.texcoord = input.texcoord;
    output.color = particle.color;
    
    return output;
}