
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
ConstantBuffer<Transformation> gTransformation : register(b0);

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    // 座標変換
    output.position = mul(input.position, gTransformation.worldViewProjection);
    
    return output;
}