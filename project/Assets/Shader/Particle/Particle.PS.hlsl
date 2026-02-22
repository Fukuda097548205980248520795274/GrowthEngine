#include "Particle.hlsli"

// 出力ピクセル
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    output.color.rgb = input.color.rgb * textureColor.rgb;
    output.color.a = input.color.a * textureColor.a;
    
    if(output.color.a == 0.0f)
    {
        discard;
    }
    
    return output;
}