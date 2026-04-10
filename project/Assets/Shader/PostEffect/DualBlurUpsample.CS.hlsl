
// 読み込みテクスチャ
Texture2D<float4> gInputTexture : register(t0);
SamplerState gClampSampler : register(s0);

// 書き込みテクスチャ
RWTexture2D<float4> gOutputTexture : register(u0);

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // 出力テクスチャの幅と高さを取得
    uint outWidth, outHeight;
    gOutputTexture.GetDimensions(outWidth, outHeight);
    
    uint inWidth, inHeight;
    gInputTexture.GetDimensions(inWidth, inHeight);
    
    if (DTid.x >= outWidth || DTid.y >= outHeight)
        return;
    
    float2 inverseInputSize = float2(rcp(float(inWidth)), rcp(float(inHeight)));
    float2 inverseOutputSize = float2(rcp(float(outWidth)), rcp(float(outHeight)));

    // 出力テクスチャのピクセル位置をUV座標に変換
    float2 uv = (float2(DTid.xy) + 0.5) * inverseOutputSize;
    
    // 入力テクスチャ(小さい方)の半ピクセル分のオフセット
    float2 halfPixel = inverseInputSize;

    // 【8回のサンプリングで広範囲を滑らかに合成】
    float4 sum = 0;
    
    // 上下左右 (距離2.0) : 重み1
    sum += gInputTexture.SampleLevel(gClampSampler, uv + float2(-halfPixel.x * 2.0, 0.0), 0);
    sum += gInputTexture.SampleLevel(gClampSampler, uv + float2(halfPixel.x * 2.0, 0.0), 0);
    sum += gInputTexture.SampleLevel(gClampSampler, uv + float2(0.0, -halfPixel.y * 2.0), 0);
    sum += gInputTexture.SampleLevel(gClampSampler, uv + float2(0.0, halfPixel.y * 2.0), 0);
    
    // 四隅 (距離1.0) : 重み2
    sum += gInputTexture.SampleLevel(gClampSampler, uv + float2(-halfPixel.x, -halfPixel.y), 0) * 2.0;
    sum += gInputTexture.SampleLevel(gClampSampler, uv + float2(halfPixel.x, -halfPixel.y), 0) * 2.0;
    sum += gInputTexture.SampleLevel(gClampSampler, uv + float2(-halfPixel.x, halfPixel.y), 0) * 2.0;
    sum += gInputTexture.SampleLevel(gClampSampler, uv + float2(halfPixel.x, halfPixel.y), 0) * 2.0;

    // 重みの合計(1+1+1+1 + 2+2+2+2 = 12.0)で割って出力
    gOutputTexture[DTid.xy] = sum / 12.0;
}