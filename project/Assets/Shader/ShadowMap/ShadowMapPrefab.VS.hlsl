
// 頂点シェーダ入力
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

// 頂点シェーダ出力
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
};

// 座標変換
struct Transformation
{
    float4x4 worldViewProjection;
};
StructuredBuffer<Transformation> gTransformation : register(t0);

VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    
    // 座標変換
    output.position = mul(input.position, gTransformation[instanceID].worldViewProjection);
    
    return output;
}