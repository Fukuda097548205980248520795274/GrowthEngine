
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPosition : POSITION0;
    
    float4 color : COLOR0;
    float4x4 uvTransform : UVTRANSFORM0;
    float environment : ENVIRONMENT0;
    float shininess : SHININESS0;
    
    int enableLighting : ENABLE0;
    int enableDiffuse : ENABLE1;
    int enableHalfLambert : ENABLE2;
    int enableSpecular : ENABLE3;
    int enableBlinnPhong : ENABLE4;
    int enableShadow : ENABLE5;
};