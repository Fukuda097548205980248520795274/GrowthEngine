#include "OutlinePrefab.hlsli"

// ピクセルシェーダーの出力構造体
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // 定数バッファから色を取得して出力
    output.color = input.color;
    
    return output;
}