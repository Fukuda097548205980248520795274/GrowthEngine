#include "../Fullscreen/Fullscreen.hlsli"

// ピクセルシェーダ出力
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

// ビネット調整用
struct VignetteParams
{
    // 色
    float3 color;
    
    // ブレンド強度
    float intensity;
    
    // 減衰カーブ
    float power;
};
ConstantBuffer<VignetteParams> gParams : register(b0);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // テクスチャカラーをサンプリング
    float4 originalColor = gTexture.Sample(gSampler, input.texcoord);
    
    // 周囲を0.0fに、中心になるほど1.0fに近づくようにする
    float2 correct = input.texcoord * (1.0f - input.texcoord);
    
    // 16.0fは、中心が1.0fになるようにするための補正値
    float vignette = correct.x * correct.y * 16.0f;
    
    // 減衰カーブを適用する
    vignette = saturate(pow(vignette, gParams.power));
    
    // ヴィネットカラーとテクスチャカラーをブレンドする
    float3 vignetteAppliedColor = lerp(gParams.color, originalColor.rgb, vignette);
    
    // ブレンド強度を適用する
    output.color.rgb = lerp(originalColor.rgb, vignetteAppliedColor, gParams.intensity);
    output.color.a = originalColor.a;
    
    return output;
}