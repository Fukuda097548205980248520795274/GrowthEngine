
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float4 currentPos : TEXCOORD0;
    float4 prevPos : TEXCOORD1;
};