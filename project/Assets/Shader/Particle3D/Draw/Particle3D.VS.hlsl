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

// カメラ情報（視線ビルボード用）
struct Camera
{
    float3 worldPositin;
    float nearZ;
    float farZ;
};
ConstantBuffer<Camera> gCamera : register(b1);

// フラグ
struct Enable
{
    int softParticle;
    int alignToDirection; 
};
ConstantBuffer<Enable> gEnable : register(b2);

VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    Particle particle = gParticles[instanceID];
    
    float4x4 localMatrix = (float4x4) 0;

    // 進行方向に向けるフラグが有効な場合
    if (gEnable.alignToDirection != 0)
    {
        // カメラの右方向と上方向のベクトルをビルボード行列から抽出
        float3 camRight = gView.billboard[0].xyz;
        float3 camUp = gView.billboard[1].xyz;

        // パーティクルの進行方向（particle.direction）をカメラの画面（2D平面）に投影
        float dirX = dot(particle.direction, camRight);
        float dirY = dot(particle.direction, camUp);
        float2 screenDir = float2(dirX, dirY);

        float3x3 rotMatrix = float3x3(
            float3(1, 0, 0),
            float3(0, 1, 0),
            float3(0, 0, 1)
        );

        // 画面上での移動成分がある場合のみ、2D回転行列を組み立てる
        if (length(screenDir) > 0.0001f)
        {
            float2 d = normalize(screenDir);
            
            // 2D回転行列を作成（Z軸は固定）
            rotMatrix = float3x3(
                float3(d.y, -d.x, 0.0f), // 新しいX軸（右）
                float3(d.x, d.y, 0.0f), // 新しいY軸（上）
                float3(0.0f, 0.0f, 1.0f) // 新しいZ軸（正面はカメラに向けたまま固定）
            );
        }

        // パーティクルのスケールを適用
        localMatrix = float4x4(
            float4(rotMatrix[0] * particle.scale.x, 0.0f),
            float4(rotMatrix[1] * particle.scale.y, 0.0f),
            float4(rotMatrix[2] * particle.scale.z, 0.0f),
            float4(0.0f, 0.0f, 0.0f, 1.0f)
        );
    }
    // 通常のビルボードの場合（元の処理と100%同じ）
    else
    {
        // パーティクルの回転を行列に変換
        float3x3 rotMatrix = QuaternionToMatrix(particle.rotation);
        localMatrix = float4x4(
            float4(rotMatrix[0] * particle.scale.x, 0.0f),
            float4(rotMatrix[1] * particle.scale.y, 0.0f),
            float4(rotMatrix[2] * particle.scale.z, 0.0f),
            float4(0.0f, 0.0f, 0.0f, 1.0f)
        );
    }

    // ビルボード行列を適用して、パーティクルのローカル座標をワールド座標に変換
    float4x4 billboardedMatrix = mul(localMatrix, gView.billboard);
    billboardedMatrix[3].xyz = particle.translate;

    // 最終座標変換
    output.position = mul(input.position, mul(billboardedMatrix, gView.viewProjection));
    
    output.texcoord = input.texcoord;
    output.color = particle.color;
    output.clipPos = output.position;

    return output;
}