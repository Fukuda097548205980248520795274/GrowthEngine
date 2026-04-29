#include "MotionVector.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = float4(0.0f, 0.0f, 0.0f, 0.0f); // デフォルト値を0初期化

    const float kMinW = 1.0e-3f;
    const float kMaxVelocity = 0.25f;

    // w成分が0以下の場合は計算不可なため終了
    if (input.currentPos.w <= kMinW || input.prevPos.w <= kMinW)
        return output;
    
    // 現在と前フレームのNDC座標を計算
    float2 currentNDC = input.currentPos.xy / input.currentPos.w;
    float2 prevNDC = input.prevPos.xy / input.prevPos.w;
    
    // UV座標に変換 (Y軸の反転を考慮)
    float2 currentUV = currentNDC * float2(0.5f, -0.5f) + 0.5f;
    float2 prevUV = prevNDC * float2(0.5f, -0.5f) + 0.5f;
    
    // モーションベクトルを計算
    float2 velocity = currentUV - prevUV;

    // 非数/無限、または外れ値（極端に速い動き）の無効化
    float velocitySq = dot(velocity, velocity);
    if (any(isnan(velocity)) || any(isinf(velocity)) || velocitySq > (kMaxVelocity * kMaxVelocity))
        return output;

    output.color = float4(velocity, 0.0f, 1.0f);
    return output;
}