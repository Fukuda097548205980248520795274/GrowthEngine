#include "../Fullscreen/Fullscreen.hlsli"

// ピクセルシェーダの出力
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct Params
{
    // ピントが合う距離
    float focusDistance;
    
    // ピントが合う範囲
    float focusRange;
    
    // ブラーの滑らかさ
    float blurFalloff;
    
    // 近クリップ面
    float zNear;
    
    // 遠クリップ面
    float zFar;
};
ConstantBuffer<Params> gParams : register(b0);

Texture2D<float4> gColorTexture : register(t0);
Texture2D<float4> gBlurTexture : register(t1);
Texture2D<float> gDepthTexture : register(t2);

SamplerState gSampler : register(s0);

// デプス値(0.0~1.0の非線形)を、実際の距離(線形)に変換する関数
float LinearizeDepth(float rawDepth)
{
    return (gParams.zNear * gParams.zFar) / (gParams.zFar - rawDepth * (gParams.zFar - gParams.zNear));
}

PixelShaderOutput main(VertexShaderOutput input)
{
    // 深度値を取得
    float rawDepth = gDepthTexture.SampleLevel(gSampler, input.texcoord, 0);
    float linerDepth = LinearizeDepth(rawDepth);
    
    // ピントからの距離を計算
    float distanceToFocus = abs(linerDepth - gParams.focusDistance);
    
    // ピントからの距離が、ピントが合う範囲を超えているかどうか
    float coc = max(0.0f, distanceToFocus - gParams.focusRange);

    // ブラーの重みを計算
    float blurWeight = saturate(coc / gParams.blurFalloff);
    
    // シャープな画像とブラー画像をサンプリング
    float3 sharpColor = gColorTexture.Sample(gSampler, input.texcoord).rgb;
    float3 blurColor = gBlurTexture.Sample(gSampler, input.texcoord).rgb;
    
    // 最終的な色を、ブラーの重みに応じて線形補間
    float3 finalColor = lerp(sharpColor, blurColor, blurWeight);

    PixelShaderOutput output;
    output.color = float4(finalColor, 1.0f);
    return output;
}