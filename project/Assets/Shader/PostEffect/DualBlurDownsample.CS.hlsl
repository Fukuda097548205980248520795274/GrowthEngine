
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
    
    if (DTid.x >= outWidth || DTid.y >= outHeight || DTid.x >= inWidth || DTid.y >= inHeight)
        return;
    
    float2 inverseInputSize = float2(rcp(float(inWidth)), rcp(float(inHeight)));
    float2 inverseOutputSize = float2(rcp(float(outWidth)), rcp(float(outHeight)));
    
    // 出力先ピクセルの中心UV座標を計算
    float2 uv = (float2(DTid.xy) + 0.5) * inverseOutputSize;
    
    // 入力テクスチャの半ピクセル分のオフセット
    float2 halfPixel = inverseInputSize;

    // 【5回のサンプリングで13ピクセル分を合成】
    // 中心は重み4、四隅は重み1 (合計8)
    float4 sum = gInputTexture.SampleLevel(gClampSampler, uv, 0) * 4.0;
    sum += gInputTexture.SampleLevel(gClampSampler, uv - halfPixel, 0);
    sum += gInputTexture.SampleLevel(gClampSampler, uv + halfPixel, 0);
    sum += gInputTexture.SampleLevel(gClampSampler, uv + float2(halfPixel.x, -halfPixel.y), 0);
    sum += gInputTexture.SampleLevel(gClampSampler, uv + float2(-halfPixel.x, halfPixel.y), 0);
    
    // 重みの合計(8.0)で割って出力
    gOutputTexture[DTid.xy] = sum / 8.0;
}