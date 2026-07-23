#include "OutlinePrefab.hlsli"

// 頂点シェーダーの入力構造体
struct VertexShaderInput
{
    float4 position : POSITION0;
};

// 座標変換用の定数バッファ
struct Outline
{
    float4x4 worldViewProjection;
    
    // 色の情報を追加（必要に応じて）
    float4 color;
};
StructuredBuffer<Outline> gOutline : register(t0);

VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    
    // 頂点の位置をワールド・ビュー・プロジェクション行列で変換
    output.position = mul(input.position, gOutline[instanceID].worldViewProjection);
    
    // 色の情報を出力（必要に応じて）
    output.color = gOutline[instanceID].color;
   
    return output;
}