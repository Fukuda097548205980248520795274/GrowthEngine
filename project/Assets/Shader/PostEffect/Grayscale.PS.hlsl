#include "../Fullscreen/Fullscreen.hlsli"

// ピクセルシェーダ出力
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

// 調整用パラメータ
struct GrayscaleParams
{
    // RGBチャンネルの重み
    float3 colorWeight;
    
    // エフェクトのブレンド強度
    float intensity;
    
    // ティントカラー
    float3 tint;
    
    // コントラスト
    float contrast;
    
    // 明るさ
    float brightness;
};
ConstantBuffer<GrayscaleParams> gParams : register(b0);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // テクスチャカラーをサンプリング
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    // RGBチャンネルの重みを適用して輝度を計算
    float luminance = dot(textureColor.rgb, gParams.colorWeight);
    
    // グレースケールカラーを作成
    float3 processedColor = float3(luminance, luminance, luminance);
    
    // コントラストと明るさ
    processedColor = (processedColor - 0.5f) * gParams.contrast + 0.5f;
    processedColor += gParams.brightness;
    
    // ティント
    processedColor *= gParams.tint;
    
    // 元のカラーとグレースケールカラーをブレンド
    output.color.rgb = lerp(textureColor.rgb, processedColor, gParams.intensity);
    output.color.a = textureColor.a; // アルファ値はそのまま保持
    
    return output;
}