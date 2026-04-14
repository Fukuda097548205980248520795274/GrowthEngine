
// 円周率
static const float PI = 3.141592653589f;

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

struct SphereParam
{
    // スライス数
    uint slices;
    
    // リング数
    uint rings;
};

// パラメータ
ConstantBuffer<SphereParam> gParam : register(b0);

// 出力頂点バッファ
RWStructuredBuffer<Vertex> gVertex : register(u0);

// 出力インデックスバッファ
RWStructuredBuffer<uint> gIndexBuffer : register(u1);

[numthreads(16, 16, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    
    uint s = DTid.x; // 経度のインデックス
    uint t = DTid.y; // 緯度のインデックス

    // ----------------------------------------------------
    // 1. 頂点計算 (slices + 1, rings + 1 の範囲で実行)
    // ----------------------------------------------------
    if (s > gParam.slices || t > gParam.rings)
    {
        return; // 完全な範囲外のスレッドはここで終了
    }

    float phi = (2.0 * PI * s) / gParam.slices;
    float theta = (PI * t) / gParam.rings;

    float3 pos;
    pos.x = sin(theta) * cos(phi);
    pos.y = cos(theta);
    pos.z = sin(theta) * sin(phi);

    float3 normal = normalize(pos);
    float2 uv = float2((float) s / gParam.slices, (float) t / gParam.rings);

    uint stride = gParam.slices + 1;
    uint vertexIndex = s + t * stride;

    Vertex v;
    v.position = float4(pos, 1.0f);
    v.normal = normalize(normal);
    v.texcoord = uv;
    gVertex[vertexIndex] = v;

    // ----------------------------------------------------
    // 2. インデックス計算 (slices, rings の範囲でのみ実行)
    // ----------------------------------------------------
    // 端の頂点 (s == slices または t == rings) は
    // 新たな四角形の起点にはならないためスキップする
    if (s < gParam.slices && t < gParam.rings)
    {
        
        uint topLeft = s + t * stride;
        uint topRight = (s + 1) + t * stride;
        uint bottomLeft = s + (t + 1) * stride;
        uint bottomRight = (s + 1) + (t + 1) * stride;

        uint quadIndex = s + t * gParam.slices;
        uint indexOffset = quadIndex * 6;

        // 三角形1
        gIndexBuffer[indexOffset + 0] = topLeft;
        gIndexBuffer[indexOffset + 1] = topRight;
        gIndexBuffer[indexOffset + 2] = bottomLeft;

        // 三角形2
        gIndexBuffer[indexOffset + 3] = bottomLeft;
        gIndexBuffer[indexOffset + 4] = topRight;
        gIndexBuffer[indexOffset + 5] = bottomRight;
    }

}