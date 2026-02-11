#define TILE_SIZE_X 16
#define TILE_SIZE_Y 16
#define MAX_LIGHTS_PER_TILE 64
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

// ライト
struct Light
{
    // 種類
    uint type;
    
    // 位置
    float3 position;
    
    // ライトの届く距離
    float radius;
};
StructuredBuffer<Light> gLight : register(t0);

RWStructuredBuffer<uint> gTileLightIndices : register(u0);
RWStructuredBuffer<uint> gTileLightCount : register(u1);

// パラメータ
struct Param
{
    // ライト数
    uint numLights;
    
    // Xタイル数
    uint numTileX;
    
    // Yタイル数
    uint numTileY;
    
    // 画面サイズ
    float2 screenSize;
    
    // 逆画面サイズ
    float2 invScreenSize;
    
    // 正射影行列
    float4x4 proj;
    
    // 逆正射影行列
    float4x4 invProj;
};
ConstantBuffer<Param> gParam : register(b0);

// タイル → View Space Frustum 計算
float3 ScreenToView(float2 screenPos, float depth)
{
    float4 ndc;
    ndc.xy = screenPos * 2.0f - 1.0f;
    ndc.y *= -1.0f;
    ndc.z = depth;
    ndc.w = 1.0f;

    float4 view = mul(ndc, gParam.invProj);
    return view.xyz / view.w;
}

// ポイントライト vs タイル判定
bool PointLightIntersectsTile(
    Light light,
    float3 frustumMin,
    float3 frustumMax)
{
    // AABB vs Sphere（かなり雑だが速い）
    float3 closest = clamp(light.position, frustumMin, frustumMax);
    float dist2 = dot(closest - light.position,
                      closest - light.position);

    return dist2 <= light.radius * light.radius;
}

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint tileX = DTid.x;
    uint tileY = DTid.y;

    if (tileX >= gParam.numTileX || tileY >= gParam.numTileY)
        return;

    uint tileID = tileY * gParam.numTileX + tileX;

    // 初期化
    gTileLightCount[tileID] = 0;

    // タイルのスクリーン範囲（0-1）
    float2 tileMinUV = float2(
        tileX * TILE_SIZE_X,
        tileY * TILE_SIZE_Y) * gParam.invScreenSize;

    float2 tileMaxUV = float2(
        (tileX + 1) * TILE_SIZE_X,
        (tileY + 1) * TILE_SIZE_Y) * gParam.invScreenSize;

    // View Space の AABB を作る（Z は仮）
    float3 frustumMin = float3(1e8, 1e8, -1e8);
    float3 frustumMax = float3(-1e8, -1e8, 1e8);

    // タイル四隅（Near / Far）
    float depths[2] = { 0.0f, 1.0f };
    
    float2 corners[4] =
    {
        tileMinUV,
        float2(tileMaxUV.x, tileMinUV.y),
        float2(tileMinUV.x, tileMaxUV.y),
        tileMaxUV
    };

    for (uint z = 0; z < 2; ++z)
    {
        float d = depths[z];

        [unroll]
        for (uint i = 0; i < 4; ++i)
        {
            float3 v = ScreenToView(corners[i], d);
            frustumMin = min(frustumMin, v);
            frustumMax = max(frustumMax, v);
        }
    }

    // ライトカリング
    for (uint lightID = 0; lightID < gParam.numLights; ++lightID)
    {
        Light light = gLight[lightID];

        // Directional Light は全タイルに影響
        if (light.type == DIRECTIONAL_LIGHT)
        {
            uint index = gTileLightCount[tileID]++;
            if (index < MAX_LIGHTS_PER_TILE)
                gTileLightIndices[tileID * MAX_LIGHTS_PER_TILE + index] = lightID;
            continue;
        }

        if (PointLightIntersectsTile(light, frustumMin, frustumMax))
        {
            uint index = gTileLightCount[tileID]++;
            if (index < MAX_LIGHTS_PER_TILE)
                gTileLightIndices[tileID * MAX_LIGHTS_PER_TILE + index] = lightID;
        }
    }
}