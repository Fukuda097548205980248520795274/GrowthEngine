#include "OutlineRender.hlsli"

// ピクセルシェーダーの出力構造体
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

// 色の定数バッファ
struct Color
{
    float4 color;
};
ConstantBuffer<Color> gColor : register(b0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // 定数バッファから色を取得して出力
    output.color = gColor.color;
    
    return output;
}