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
    
    // 1. スケールとローカル回転を適用した行列を作成
    float3x3 rotMatrix = QuaternionToMatrix(particle.rotation);
    float4x4 localMatrix = float4x4(
        float4(rotMatrix[0] * particle.scale.x, 0.0f),
        float4(rotMatrix[1] * particle.scale.y, 0.0f),
        float4(rotMatrix[2] * particle.scale.z, 0.0f),
        float4(0.0f, 0.0f, 0.0f, 1.0f)
    );
    
    // 2. 「平行移動を入れる前」にビルボード行列を掛けて、常にカメラを向くようにする
    float4x4 billboardedMatrix = mul(localMatrix, gView.billboard);
    
    // 3. 最後に平行移動（ワールド座標の位置）を入れる
    billboardedMatrix[3].xyz = particle.translate;
    
    // 4. ビュープロジェクションを掛けて画面上の位置を計算
    output.position = mul(input.position, mul(billboardedMatrix, gView.viewProjection));
    
    output.texcoord = input.texcoord;
    output.color = particle.color;
    output.clipPos = output.position;
    
    return output;
}