
// 円周率
static const float kPI = 3.141592653589f;

// 頂点
struct Vertex
{
    // 位置
    float4 position;
    
    // UV座標
    float2 texcoord;
    
    // 法線
    float3 normal;
};

struct TubeParam
{
    // スライス数
    uint slices;
};

// パラメータ
ConstantBuffer<TubeParam> gParam : register(b0);

// 出力頂点バッファ
RWStructuredBuffer<Vertex> gVertex : register(u0);

// 出力インデックスバッファ
RWStructuredBuffer<uint> gIndexBuffer : register(u1);

[numthreads(16, 2, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint s = DTid.x; // 円周方向のインデックス
    uint t = DTid.y; // 高さ方向のインデックス (常に 0 か 1)

    // スライス範囲外、または高さインデックスが2以上の場合は終了
    if (s > gParam.slices || t > 1)
        return;

    // 円周方向の角度
    float phi = (2.0f * kPI * s) / gParam.slices;
    
    // vは上(t=0)なら0.0、下(t=1)なら1.0になる
    float v = (float) t;

    // 円柱の頂点位置の計算 (zは 奥=0.5, 手前=-0.5)
    float z = 0.5f - v;
    float3 pos = float3(cos(phi), sin(phi), z);

    // 基準の法線
    float3 normal = float3(pos.x, pos.y, 0.0f);

    // 頂点インデックスの計算 (strideは slices + 1)
    uint stride = gParam.slices + 1;
    uint vertexIndex = s + t * stride;
    
    // 頂点の設定
    Vertex vert;
    vert.position = float4(pos, 1.0f);
    vert.normal = normal;
    vert.texcoord = float2((float) s / gParam.slices, v);
    gVertex[vertexIndex] = vert;

    // インデックスの計算
    // t == 0 (上の段) の時だけ四角形(三角形2つ)を生成すれば側面が完成する
    if (s < gParam.slices && t == 0)
    {
        uint topLeft = s + 0 * stride; // t=0
        uint topRight = (s + 1) + 0 * stride;
        uint bottomLeft = s + 1 * stride; // t=1
        uint bottomRight = (s + 1) + 1 * stride;

        uint quadIndex = s;
        uint indexOffset = quadIndex * 6;

        gIndexBuffer[indexOffset + 0] = topLeft;
        gIndexBuffer[indexOffset + 1] = topRight;
        gIndexBuffer[indexOffset + 2] = bottomLeft;
        gIndexBuffer[indexOffset + 3] = bottomLeft;
        gIndexBuffer[indexOffset + 4] = topRight;
        gIndexBuffer[indexOffset + 5] = bottomRight;
    }
}