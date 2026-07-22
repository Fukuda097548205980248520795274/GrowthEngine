
// 入力テクスチャとサンプラー
Texture2D<float4> gInputTexture : register(t0);
SamplerState gSampler : register(s0);

// 出力用テクスチャ (UAV)
RWTexture2D<float4> gOutputTexture : register(u0);

// 5x5のインデックス
static const float2 kIndex5x5[5][5] =
{
    { float2(-2.0f, -2.0f), float2(-1.0f, -2.0f), float2(0.0f, -2.0f), float2(1.0f, -2.0f), float2(2.0f, -2.0f) },
    { float2(-2.0f, -1.0f), float2(-1.0f, -1.0f), float2(0.0f, -1.0f), float2(1.0f, -1.0f), float2(2.0f, -1.0f) },
    { float2(-2.0f, 0.0f), float2(-1.0f, 0.0f), float2(0.0f, 0.0f), float2(1.0f, 0.0f), float2(2.0f, 0.0f) },
    { float2(-2.0f, 1.0f), float2(-1.0f, 1.0f), float2(0.0f, 1.0f), float2(1.0f, 1.0f), float2(2.0f, 1.0f) },
    { float2(-2.0f, 2.0f), float2(-1.0f, 2.0f), float2(0.0f, 2.0f), float2(1.0f, 2.0f), float2(2.0f, 2.0f) }
};

// 円周率
static const float kPI = 3.14159265f;

// ガウス関数
float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denomnator = 2.0f * kPI * sigma * sigma;
    
    // ガウス関数の値を返す
    return exp(exponent) * rcp(denomnator);
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    // テクスチャの幅と高さを取得
    uint width, height;
    gInputTexture.GetDimensions(width, height);
    
    // 画面外の処理をスキップ (境界チェック)
    if (DTid.x >= width || DTid.y >= height)
    {
        return;
    }

    // 5x5カーネル
    float kernel5x5[5][5];
    
    // カーネルの重みの合計
    float weight = 0.0f;
    
    // 5x5のガウスカーネルを計算
    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 5; x++)
        {
            kernel5x5[y][x] = gauss(kIndex5x5[y][x].x, kIndex5x5[y][x].y, 2.0f);
            weight += kernel5x5[y][x];
        }
    }

    // テクスチャ座標のステップサイズを計算
    float2 upStepSize = float2(rcp(float(width)), rcp(float(height)));
    
    // 現在のスレッドが担当するピクセルのUV座標を計算 (ピクセルの中心座標を取得するために +0.5f)
    float2 baseTexcoord = (float2(DTid.xy) + 0.5f) * upStepSize;
    
    float3 resultColor = float3(0.0f, 0.0f, 0.0f);
    
    // 5x5のカーネルを適用
    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 5; x++)
        {
            // テクスチャ座標を計算
            float2 texcoord = baseTexcoord + kIndex5x5[y][x] * upStepSize;
            
            // Compute Shaderでは偏微分が計算できないため、SampleLevelを使用する
            float3 fetchColor = gInputTexture.SampleLevel(gSampler, texcoord, 0).rgb;
            resultColor += fetchColor * kernel5x5[y][x];
        }
    }
    
    // 重みの合計で割る
    resultColor *= rcp(weight);
    
    // RWTexture2Dに結果を書き込む (アルファ値は1.0fで固定)
    gOutputTexture[DTid.xy] = float4(resultColor, 1.0f);
}