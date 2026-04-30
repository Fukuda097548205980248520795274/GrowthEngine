// 入力テクスチャ（現在のフレームの色）
Texture2D<float4> InputColor : register(t0);

// モーションベクトル(rg)とマスク(b)を格納したテクスチャ
Texture2D<float4> MotionAndMaskTex : register(t1);

// 前フレームの履歴バッファ（読み込み専用 SRV に変更）
Texture2D<float4> HistoryIn : register(t2);

// 深度テクスチャ
Texture2D<float> depthTexture : register(t3);

// 出力テクスチャ（最終的な画面出力）
RWTexture2D<float4> OutputColor : register(u0);

// 今フレームの履歴バッファ（書き込み専用 UAV に変更）
RWTexture2D<float4> HistoryOut : register(u1);

// サンプラー（リプロジェクション時の補間用）
SamplerState LinearSampler : register(s0);

// パラメータバッファ
cbuffer TemporalParams : register(b0)
{
    float g_Decay;
    
    float3 padding;
    
    float4x4 g_InvCurrentVP; // 現在のフレームの逆ビュー射影行列
    
    float4x4 g_PrevVP; // 前フレームのビュー射影行列
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint width, height;
    InputColor.GetDimensions(width, height);

    // スレッドが画面外に出ないようにする
    if (DTid.x >= width || DTid.y >= height)
        return;

    // 現在のUV座標を計算
    float2 uv = (float2(DTid.xy) + 0.5f) / float2(width, height);

    // 現在のフレームの色を取得
    float4 currentColor = InputColor.Load(int3(DTid.xy, 0));
    
    // モーションベクトルとマスクの値を取得
    float4 mvAndMask = MotionAndMaskTex.Load(int3(DTid.xy, 0));
    float2 velocity = mvAndMask.rg;
    float mask_b = mvAndMask.b;
    
    
    // 深度値を取得
    float depth = depthTexture.Load(int3(DTid.xy, 0)).r;

    // クリップ空間の座標を計算
    float4 clipPos = float4(uv.x * 2.0f - 1.0f, -(uv.y * 2.0f - 1.0f), depth, 1.0f);

    // クリップ空間からワールド空間への変換
    float4 worldPos = mul(clipPos, g_InvCurrentVP);
    worldPos /= worldPos.w;

    // 前フレームのクリップ空間の座標を計算
    float4 prevClipPos = mul(worldPos, g_PrevVP);
    prevClipPos /= prevClipPos.w;

    // 前フレームのUV座標を計算
    float2 prevUV = float2(prevClipPos.x * 0.5f + 0.5f, -prevClipPos.y * 0.5f + 0.5f);

    // カメラの動きから求めた背景の速度を計算
    float2 backgroundVelocity = uv - prevUV;
    

    // マスクの値に基づいて、プレイヤーの速度と背景の速度を線形補間
    float2 finalVelocity = lerp(backgroundVelocity, velocity, ceil(mask_b));

    // 前フレームのUV座標を計算（現在のUVから速度を引く）
    float2 historyUV = uv - finalVelocity;

    // 履歴のサンプリング
    float3 historyTrail = float3(0.0f, 0.0f, 0.0f);
    
    // 画面外のサンプリングを防ぐ
    if (all(historyUV >= 0.0f) && all(historyUV <= 1.0f))
    {
        // ズレた座標から取得するため、SampleLevelとLinearSamplerを使用
        historyTrail = HistoryIn.SampleLevel(LinearSampler, historyUV, 0).rgb;
    }

    // 履歴を減衰させる
    historyTrail *= g_Decay;

    // プレイヤーの速度の大きさを計算
    float speed = length(velocity);

    // 速度が一定以上の場合にのみ残像を表示するためのフラグ
    float isMoving = (speed > 0.001f) ? 1.0f : 0.0f;

    // マスクの値に基づいて、現在のフレームの色を残像として使用するかどうかを決定
    float3 currentTrail = currentColor.rgb * mask_b * isMoving;

    // 次のフレームに渡す履歴を保存
    float3 nextTrail = max(historyTrail, currentTrail);
    HistoryOut[DTid.xy] = float4(nextTrail, 1.0f);

    // 最終的な色の合成
    float3 finalRGB = currentColor.rgb + (historyTrail * (1.0f - mask_b));
    
    // 出力にアルファを維持
    OutputColor[DTid.xy] = float4(finalRGB, currentColor.a);
}