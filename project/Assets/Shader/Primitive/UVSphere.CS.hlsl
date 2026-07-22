
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

struct SphereParam
{
    // スライス数
    uint slices;
    
    // セグメント数
    uint segments;
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

    // 範囲外のスレッドは処理しない
    if (s > gParam.slices || t > gParam.segments)
        return;

    // 球面座標を計算
    float phi = (2.0 * kPI * s) / gParam.slices;
    float theta = (kPI * t) / gParam.segments;

    // 球面座標から直交座標に変換
    float3 pos;
    pos.x = sin(theta) * cos(phi);
    pos.y = cos(theta);
    pos.z = sin(theta) * sin(phi);

    // 法線は球の中心からの方向
    float3 normal = normalize(pos);
    float2 uv = float2((float) s / gParam.slices, (float) t / gParam.segments);

    // 頂点バッファのインデックスを計算
    uint stride = gParam.slices + 1;
    uint vertexIndex = s + t * stride;

    Vertex v;
    v.position = float4(pos, 1.0f);
    v.normal = normalize(normal);
    v.texcoord = uv;
    gVertex[vertexIndex] = v;

    // インデックスバッファの生成
    if (s < gParam.slices && t < gParam.segments)
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