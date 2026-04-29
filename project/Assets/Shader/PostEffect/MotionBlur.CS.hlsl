
Texture2D<float4> ColorTexture : register(t0);
Texture2D<float4> VelocityTexture : register(t1);
RWTexture2D<float4> OutputTexture : register(u0);

SamplerState gSampler : register(s0);

// パラメータ
struct Params
{
    uint numSamples; // ブラーのサンプル数
    
    float blurScale; // ブラーの強さを調整するスケール
};
ConstantBuffer<Params> gParams : register(b0);

// スレッドグループのサイズ
[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint width, height;
    OutputTexture.GetDimensions(width, height);
    if (DTid.x >= width || DTid.y >= height)
        return;

    float2 uv = (float2(DTid.xy) + 0.5f) / float2(width, height);

    // モーションベクトルのテクスチャ(float4)からデータを取得
    float4 mvData = VelocityTexture.SampleLevel(gSampler, uv, 0);
    
    // xyはベクトル、a(w)はブラーをかけるかどうかのマスク(0.0 〜 1.0)として使用
    float2 rawVelocity = mvData.xy;
    float blurMask = mvData.a;

    // ベクトルに全体スケールと、オブジェクトごとのマスクを乗算
    // マスクが0（ブラーをかけない）のピクセルは、ここでvelocityが0になる
    float2 velocity = rawVelocity * gParams.blurScale * blurMask;

    // ベクトルがほぼゼロ（またはマスクが0）の場合は、元の色をそのまま出力して終了
    if (length(velocity) < 0.0001f)
    {
        OutputTexture[DTid.xy] = ColorTexture.Load(int3(DTid.xy, 0));
        return;
    }

    float4 resultColor = float4(0, 0, 0, 0);

    // モーションベクトルに沿って複数回サンプリング
    for (int i = 0; i < gParams.numSamples; ++i)
    {
        // 中心ピクセルを基準に -0.5 から +0.5 の範囲でサンプリング
        float t = (float(i) / float(gParams.numSamples - 1)) - 0.5f;
        float2 sampleUV = uv + velocity * t;

        // 色を加算していく
        resultColor += ColorTexture.SampleLevel(gSampler, sampleUV, 0);
    }

    // 単純平均化
    resultColor /= float(gParams.numSamples);

    OutputTexture[DTid.xy] = resultColor;
}