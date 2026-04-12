
// パラメータ
struct Params
{
    // 抽出を開始する基本的な輝度
    float threshold;
    
    // 閾値付近の滑らかさの幅
    float knee;
};
ConstantBuffer<Params> gParams : register(b0);

// 読み込みテクスチャ
Texture2D<float4> gInputTexture : register(t0);

// 書き込みテクスチャ
RWTexture2D<float4> gOutputTexture : register(u0);


// 高輝度抽出関数（ソフトな曲線を使用）
float3 ExtractHighLuminanceSoft(float3 color, float threshold, float knee)
{
    // 輝度を計算（最大値を使用）
    float brightness = max(color.r, max(color.g, color.b));

    // 輝度が閾値を超える部分を滑らかに抽出
    float rq = clamp(brightness - threshold + knee, 0.0, knee * 2.0);
    
    // ソフトな曲線を計算
    float softCurve = (rq * rq) / (4.0 * max(knee, 0.00001));

    // 貢献度を計算
    float contribution = max(brightness - threshold, softCurve);

    // 色に貢献度を適用して抽出
    return color * (contribution / max(brightness, 0.00001));
}


[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // 出力テクスチャのサイズを取得
    uint width, height;
    gOutputTexture.GetDimensions(width, height);

    // スレッドIDがテクスチャの範囲外の場合は処理しない
    if (DTid.x >= width || DTid.y >= height)
        return;

    // 入力テクスチャからカラーをサンプリング
    float4 sourceColor = gInputTexture[DTid.xy];

    // パラメータを使って抽出
    float3 extractedColor = ExtractHighLuminanceSoft(sourceColor.rgb, gParams.threshold, gParams.knee);

    // 結果を出力テクスチャに書き込む
    gOutputTexture[DTid.xy] = float4(extractedColor, 1.0);
}