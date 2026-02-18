#include "../Fullscreen/Fullscreen.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct Param
{
    float2 resolution;
    float2 center;
    float blur;
    int sampleCount;

    // --- 色補正パラメータ ---
    float saturation; // 彩度 (1.0 = 元のまま)
    float contrast; // コントラスト (1.0 = 元のまま)
    float brightness; // 明るさ (0 = 変化なし)

};
ConstantBuffer<Param> gParam : register(b0);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// 彩度調整
float3 AdjustSaturation(float3 color, float sat)
{
    float gray = dot(color, float3(0.299, 0.587, 0.114));
    return lerp(gray.xxx, color, sat);
}

// コントラスト調整
float3 AdjustContrast(float3 color, float contrast)
{
    return (color - 0.5) * contrast + 0.5;
}

// 明るさ調整
float3 AdjustBrightness(float3 color, float brightness)
{
    return color + brightness;
}



PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    float2 uv = input.texcoord;

    float detal = gParam.blur / gParam.sampleCount;
    float offset = detal;

    float2 velocity = uv - gParam.center;
    velocity *= length(velocity);

    float4 sum = float4(0, 0, 0, 0);

    [loop]
    for (int i = 0; i < gParam.sampleCount; i++)
    {
        sum += gTexture.Sample(gSampler, uv - velocity * offset);
        offset += detal;
    }

    float3 color = (sum / gParam.sampleCount).rgb;

    // --- 色補正処理 ---
    color = AdjustSaturation(color, gParam.saturation);
    color = AdjustContrast(color, gParam.contrast);
    color = AdjustBrightness(color, gParam.brightness);
    
    output.color = float4(color, 1.0f);
    
    return output;
}