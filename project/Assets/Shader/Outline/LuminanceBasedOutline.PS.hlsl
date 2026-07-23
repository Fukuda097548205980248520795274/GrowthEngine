
// テクスチャレジスタの設定
Texture2D<float4> gMainScene : register(t0);
Texture2D<float4> gCustomColor : register(t1);
SamplerState gSampler : register(s0);

// 調整用パラメータ
struct OutlineParams
{
    float2 screenResolution; // 画面解像度 (1.0/width, 1.0/height)
    float outlineWidth; // 線の太さ
    float colorThreshold; // シーンカラーのエッジ検出感度
};
ConstantBuffer<OutlineParams> gParams : register(b0);

struct VS_Output
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

// RGBから輝度（Luminance）を計算するヘルパー関数
float GetLuminance(float3 color)
{
    // 人間の目に合わせた標準的な輝度変換の係数
    return dot(color, float3(0.299, 0.587, 0.114));
}

// ピクセルシェーダー
float4 main(VS_Output input) : SV_TARGET
{
    float2 uv = input.uv;
    float2 texelSize = gParams.screenResolution * gParams.outlineWidth;

    float4 sceneColor = gMainScene.Sample(gSampler, uv);
    
    // 対象キャラクターの判定用（中心のカスタムカラー）
    float4 centerCustomColor = gCustomColor.Sample(gSampler, uv);

    // 3x3のサンプリング用オフセット
    float2 offsets[9] =
    {
        float2(-1, -1), float2(0, -1), float2(1, -1),
        float2(-1, 0), float2(0, 0), float2(1, 0),
        float2(-1, 1), float2(0, 1), float2(1, 1)
    };

    // ソーベルフィルタのカーネル（X方向・Y方向）
    float sobelX[9] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
    float sobelY[9] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };

    float edgeX = 0.0;
    float edgeY = 0.0;
    float customMask = 0.0; // キャラクター領域判定用

    for (int i = 0; i < 9; i++)
    {
        float2 neighborUV = uv + offsets[i] * texelSize;
        
        // 1. シーンの色を取得し、輝度（モノクロ）に変換
        float3 neighborScene = gMainScene.Sample(gSampler, neighborUV).rgb;
        float luminance = GetLuminance(neighborScene);

        // 2. ソーベルフィルタの計算に加算
        edgeX += luminance * sobelX[i];
        edgeY += luminance * sobelY[i];

        // 3. キャラクター領域（アウトラインを描画すべき場所）かどうかの判定
        float4 neighborCustomColor = gCustomColor.Sample(gSampler, neighborUV);
        if (neighborCustomColor.a > 0.01)
        {
            customMask = 1.0;
            // 中心のカスタムカラーが空（背景）の場合、周辺のキャラの色を線の色として保持
            if (centerCustomColor.a < 0.01)
            {
                centerCustomColor = neighborCustomColor;
            }
        }
    }

    // 周辺にキャラクターが一切いなければ、計算を打ち切って元のシーンを返す
    if (customMask < 0.5)
    {
        return sceneColor;
    }

    // エッジの強度を計算（XとYのベクトルの長さ）
    float edgeStrength = sqrt((edgeX * edgeX) + (edgeY * edgeY));

    // エッジ強度が閾値を超えていればアウトラインを描画
    if (edgeStrength > gParams.colorThreshold)
    {
        // 線の色はキャラクターごとに設定したカスタムカラーを使用
        return float4(centerCustomColor.rgb, 1.0);
    }
    
    return sceneColor;
}