// テクスチャレジスタの設定
Texture2D<float4> gMainScene : register(t0); // 元のレンダリング結果
Texture2D<float> gCustomDepth : register(t1); // 以前作成したアウトライン用深度
Texture2D<float4> gCustomColor : register(t2); // 以前作成したアウトライン用カラー
SamplerState gSampler : register(s0);

// 調整用パラメータ
struct OutlineParams
{
    float2 screenResolution; // 画面解像度 (1.0/width, 1.0/height)
    float outlineWidth; // 線の太さ
    float depthThreshold; // 深度の感度
    float colorThreshold; // 色の感度
};
ConstantBuffer<OutlineParams> gParams : register(b0);

struct VS_Output
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

// ピクセルシェーダー
float4 main(VS_Output input) : SV_TARGET
{
    
    float2 uv = input.uv;
    float2 texelSize = gParams.screenResolution * gParams.outlineWidth;

    // 1. 中心（現在のピクセル）の情報を取得
    float centerDepth = gCustomDepth.Sample(gSampler, uv).r;
    float4 centerColor = gCustomColor.Sample(gSampler, uv);
    float4 sceneColor = gMainScene.Sample(gSampler, uv);

    // 2. 周辺（上下左右）のピクセルをサンプリング
    float2 offsets[4] =
    {
        float2(1, 0), float2(-1, 0),
        float2(0, 1), float2(0, -1)
    };

    float edgeMask = 0.0;
    float4 edgeColor = centerColor;

    for (int i = 0; i < 4; i++)
    {
        float2 neighborUV = uv + offsets[i] * texelSize;
        
        // 周辺の深度と色を取得
        float neighborDepth = gCustomDepth.Sample(gSampler, neighborUV).r;
        float4 neighborColor = gCustomColor.Sample(gSampler, neighborUV);

        // --- エッジ検出ロジック ---
        
        // 深度の差分をチェック
        float depthDiff = abs(centerDepth - neighborDepth);
        if (depthDiff > gParams.depthThreshold)
        {
            edgeMask = 1.0;
        }

        // 色の差分をチェック（色が違うオブジェクトが隣接している場合）
        float colorDiff = distance(centerColor.rgb, neighborColor.rgb);
        if (colorDiff > gParams.colorThreshold)
        {
            edgeMask = 1.0;
            // より外側の色を線の色として採用（お好みで調整可能）
            edgeColor = (neighborDepth < centerDepth) ? neighborColor : centerColor;
        }
    }
    
    if (edgeColor.a < 0.01) // アウトライン対象がなければ元のシーンをそのまま返す
    {
        return sceneColor;
    }

    // 3. 最終的な出力
    // エッジ（edgeMask > 0）ならアウトライン用カラー、そうでなければ元のシーンの色
    if (edgeMask > 0.0 && centerDepth < 1.0) // 遠景(1.0)には線を出さない
    {
        return edgeColor;
    }
    
    
    return sceneColor;
}