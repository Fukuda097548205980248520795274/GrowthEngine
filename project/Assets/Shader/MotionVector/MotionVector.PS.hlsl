#include "MotionVector.hlsli"

struct PixelShaderOutput
{
    float2 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    const float kMinW = 1.0e-3f;
    const float kMaxVelocity = 0.25f;
    
    // w成分が0以下の場合は、モーションベクトルを(0, 0)として出力
    if (input.currentPos.w <= kMinW || input.prevPos.w <= kMinW)
    {
        output.color = float2(0.0f, 0.0f);
        return output;
    }
    
    // 現在のNDC座標と前のNDC座標を計算
    float2 currentNDC = input.currentPos.xy / input.currentPos.w;
    float2 prevNDC = input.prevPos.xy / input.prevPos.w;
    float currentZ = input.currentPos.z / input.currentPos.w;
    float prevZ = input.prevPos.z / input.prevPos.w;

    // 視錐台外や数値不安定な値は無効化
    float currentNdcAbsMax = max(abs(currentNDC.x), abs(currentNDC.y));
    float prevNdcAbsMax = max(abs(prevNDC.x), abs(prevNDC.y));
    if (currentNdcAbsMax > 1.0f || prevNdcAbsMax > 1.0f)
    {
        output.color = float2(0.0f, 0.0f);
        return output;
    }

    // 近/遠クリップ外やクリップ面跨ぎ由来の不安定値を無効化
    if (currentZ < 0.0f || currentZ > 1.0f || prevZ < 0.0f || prevZ > 1.0f)
    {
        output.color = float2(0.0f, 0.0f);
        return output;
    }
    
    // UV座標に変換
    float2 currentUV = currentNDC * float2(0.5f, -0.5f) + 0.5f;
    float2 prevUV = prevNDC * float2(0.5f, -0.5f) + 0.5f;
    
    // モーションベクトルを計算
    float2 velocity = currentUV - prevUV;

    // 非数/無限を無効化
    if (any(isnan(velocity)) || any(isinf(velocity)))
    {
        output.color = float2(0.0f, 0.0f);
        return output;
    }

    // 外れ値を抑制（巨大ベクトルによる三角形ゴースト防止）
    float velocitySq = dot(velocity, velocity);
    if (velocitySq > (kMaxVelocity * kMaxVelocity))
    {
        velocity = float2(0.0f, 0.0f);
    }
    
    // モーションベクトルを色として出力
    output.color = velocity;
    
    return output;
}