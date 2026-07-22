Texture2D<float4> currentTex : register(t0);
Texture2D<float4> prevTex : register(t1);
Texture2D<float4> velocityTex : register(t2);

RWTexture2D<float4> outputTex : register(u0);

SamplerState gSampler : register(s0);

// パラメータ構造体
struct Params
{
    // ブレンドファクター（0.0fなら完全に履歴、1.0fなら完全に現在のフレーム）
    float blendFactor;
    
    // ガンマ値（クランプ範囲を広げるために使用）
    float gamma;
};
ConstantBuffer<Params> gParams : register(b0);

// トーンマップ関数（圧縮）
float4 Tonemap(float4 c)
{
    return c / (1.0f + max(max(c.r, c.g), c.b));
}
// 逆トーンマップ関数
float4 InverseTonemap(float4 c)
{
    float maxCol = max(max(c.r, c.g), c.b);
    
    // 最大値を0.9999などに制限し、分母がゼロ以下になるのを防ぐ
    maxCol = min(maxCol, 0.9999f);
    return c / (1.0f - maxCol);
}

// トーンマップ空間の履歴色をAABBでクリップする関数（関数の外に定義）
float4 ClipAABB(float4 cMin, float4 cMax, float4 history, float4 current)
{
    float4 p_clip = 0.5f * (cMax + cMin);
    float4 e_clip = 0.5f * (cMax - cMin) + 0.000001f; // ゼロ除算防止
    float4 v_clip = history - p_clip;
    float4 v_unit = v_clip / e_clip;
    float4 a_unit = abs(v_unit);
    float ma_unit = max(max(a_unit.x, a_unit.y), max(a_unit.z, a_unit.w));

    if (ma_unit > 1.0f)
        return p_clip + v_clip / ma_unit;
    else
        return history; // AABBの内側ならそのまま
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    // テクスチャのサイズを取得
    uint width, height;
    currentTex.GetDimensions(width, height);
    
    // 画面外のスレッドは処理しない
    if (DTid.x >= width || DTid.y >= height)
        return;

    // 現在のピクセルのUV座標を計算
    int2 pos = int2(DTid.xy);
    float2 uv = (float2(pos) + 0.5f) / float2(width, height);
    
    // テクスチャの最大座標
    int2 maxPos = int2(width - 1, height - 1);

    // 現在のピクセルの色を取得
    float4 linearCurrentColor = currentTex[pos];

    // ベロシティテクスチャから現在のピクセルのベロシティを取得
    float2 bestVelocity = velocityTex[pos].xy;
    float maxVelocitySq = dot(bestVelocity, bestVelocity);

    // 3x3の近傍をサンプリングして最大のベロシティを見つける
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            // 近傍のベロシティをサンプリングして最大のものを見つける
            int2 samplePos = clamp(pos + int2(x, y), int2(0, 0), maxPos);
            float2 v = velocityTex[samplePos].xy;
            float vSq = dot(v, v);
            if (vSq > maxVelocitySq)
            {
                maxVelocitySq = vSq;
                bestVelocity = v;
            }
        }
    }
    
    // ベロシティに基づいて履歴テクスチャから色をサンプリング
    float2 historyUV = uv - bestVelocity;
    float4 historyColor = prevTex.SampleLevel(gSampler, historyUV, 0);



    // 現在の色と履歴の色をトーンマップしてからモーメント計算に使用
    float4 tmCurrent = Tonemap(linearCurrentColor);
    float4 tmHistory = Tonemap(historyColor);

    float4 m1 = 0.0f; // 1次のモーメント（平均用）
    float4 m2 = 0.0f; // 2次のモーメント（分散用）

    // 3x3の近傍ピクセルをサンプリングしてモーメントを計算
    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            int2 samplePos = clamp(pos + int2(dx, dy), int2(0, 0), maxPos);
            
            // 近傍ピクセルもトーンマップしてからモーメントに加算
            float4 c = Tonemap(currentTex[samplePos]);
            m1 += c;
            m2 += c * c;
        }
    }

    // 平均と分散を計算
    float4 mean = m1 / 9.0f;
    float4 variance = abs((m2 / 9.0f) - (mean * mean));
    float4 stddev = sqrt(variance);

    // ガンマ値を上げてクランプ範囲を広げる
    float4 cMin = mean - gParams.gamma * stddev;
    float4 cMax = mean + gParams.gamma * stddev;

    // 履歴の色をAABBでクリップ
    tmHistory = ClipAABB(cMin, cMax, tmHistory, mean);
    
    // ブレンド
    float4 tmFinal = lerp(tmHistory, tmCurrent, gParams.blendFactor);

    // 逆トーンマップして最終色を出力
    float4 finalColor = InverseTonemap(tmFinal);

    // 履歴UVが画面外の場合は履歴を使わず現在の色をそのまま出力
    if (historyUV.x < 0.0f || historyUV.x > 1.0f || historyUV.y < 0.0f || historyUV.y > 1.0f)
    {
        finalColor = linearCurrentColor;
    }

    outputTex[pos] = finalColor;
}