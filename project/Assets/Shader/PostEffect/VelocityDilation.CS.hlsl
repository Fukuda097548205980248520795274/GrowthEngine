Texture2D<float4> InputVelocityTexture : register(t0);
RWTexture2D<float4> OutputVelocityTexture : register(u0);

// 検索する半径（1なら3x3=9ピクセル、2なら5x5=25ピクセルを検索）
#define DILATION_RADIUS 2

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint width, height;
    OutputVelocityTexture.GetDimensions(width, height);
    
    // 画面外の処理をスキップ
    if (DTid.x >= width || DTid.y >= height)
        return;

    float maxSpeedSq = -1.0f;
    float4 maxVelocityData = float4(0, 0, 0, 0);

    // 周囲のピクセルをループして最大速度を探す
    for (int y = -DILATION_RADIUS; y <= DILATION_RADIUS; ++y)
    {
        for (int x = -DILATION_RADIUS; x <= DILATION_RADIUS; ++x)
        {
            // サンプリングするピクセルの座標を計算
            int2 samplePos = int2(DTid.xy) + int2(x, y);

            // 画面外をサンプリングしないようにクランプ（端の処理）
            samplePos.x = clamp(samplePos.x, 0, width - 1);
            samplePos.y = clamp(samplePos.y, 0, height - 1);

            // Load関数を使って、ピクセル座標から直接データを取得
            float4 vData = InputVelocityTexture.Load(int3(samplePos, 0));
            
            // ベクトルの長さ（速度）を計算。
            // length()は内部で平方根(sqrt)を使い重いため、比較だけなら内積(dot)による「長さの2乗」を使うのが定石です。
            float speedSq = dot(vData.xy, vData.xy);

            // 今までの最大速度よりも大きければ更新
            if (speedSq > maxSpeedSq)
            {
                maxSpeedSq = speedSq;
                maxVelocityData = vData; // 方向やマスク(a)も含めて丸ごと保持
            }
        }
    }

    // 自分の周囲で一番速かったピクセルのデータを、自分のデータとして書き出す
    OutputVelocityTexture[DTid.xy] = maxVelocityData;
}