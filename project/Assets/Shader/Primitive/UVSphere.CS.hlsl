
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
    // 範囲外のスレッドは書き込みを行わずに終了する
    if (DTid.x > gParam.slices || DTid.y > gParam.rings)
    {
        return;
    }
    
    uint slicesIndex = DTid.x;
    uint ringsIndex = DTid.y;
    
    // スライス数とリング数の範囲内であれば頂点を生成
    if(slicesIndex <= gParam.slices && ringsIndex <= gParam.rings)
    {
        // UV座標の計算
        float u = (float) slicesIndex / (float) gParam.slices;
        float v = (float) ringsIndex / (float) gParam.rings;
        
        // 球面座標の計算
        float theta = u * 2.0f * PI;
        float phi = v * PI;
        
        // 三角関数の計算
        float sinPhi = sin(phi);
        float cosPhi = cos(phi);
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);
        
        // 位置と法線の計算
        float3 normal = float3(sinPhi * cosTheta, cosPhi, sinPhi * sinTheta);
        float4 position = float4(normal, 1.0f);
        
        // 頂点の書き込み
        Vertex vertex;
        vertex.position = position;
        vertex.normal = normal;
        vertex.texcoord = float2(u, v);
        
        // 頂点バッファへの書き込み
        uint vertexIndex = ringsIndex * (gParam.slices + 1) + slicesIndex;
        gVertex[vertexIndex] = vertex;
    }
    
    // スライス数とリング数の範囲内であればインデックスを生成
    if (slicesIndex < gParam.slices && ringsIndex < gParam.rings)
    {
        // 頂点インデックスの計算
        uint v0 = ringsIndex * (gParam.slices + 1) + slicesIndex;
        uint v1 = v0 + 1;
        uint v2 = (ringsIndex + 1) * (gParam.slices + 1) + slicesIndex;
        uint v3 = v2 + 1;
        
        // インデックスの書き込み
        uint indexOffset = (ringsIndex * gParam.slices + slicesIndex) * 6;
        
        // 三角形1
        gIndexBuffer[indexOffset + 0] = v0;
        gIndexBuffer[indexOffset + 1] = v1;
        gIndexBuffer[indexOffset + 2] = v2;

        // 三角形2
        gIndexBuffer[indexOffset + 3] = v1;
        gIndexBuffer[indexOffset + 4] = v3;
        gIndexBuffer[indexOffset + 5] = v2;
    }

}