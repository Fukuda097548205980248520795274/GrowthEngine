#include "Prefab3D.hlsli"

// 入力頂点
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct Primitive
{
    // ワールド行列
    float4x4 world;
    
    // 逆転置行列
    float4x4 worldInverseTranspose;
    
    // 色
    float4 color;
    
    // UVトランスフォーム
    float4x4 uvTransform;
    
    // 環境
    float environment;
    
    // 光沢度
    float shininess;
    
    // ライティング有効化
    int enableLighting;
    
    // ディフューズ
    int enableDiffuse;
    
    // ハーフランバート有効化
    int enableHalfLambert;
    
    // スペキュラー有効化
    int enableSpecular;
    
    // ブリンフォン有効化
    int enableBlinnPhong;
    
    // シャドウ有効化
    int enableShadow;
    
    // 上の半径
    float topRadius;
    
    // 下の半径
    float bottomRadius;
    
    // 高さ
    float height;
};
StructuredBuffer<Primitive> gPrimitive : register(t0);

// ビュー
struct View
{
    float4x4 viewProjection;
};
ConstantBuffer<View> gView : register(b0);

VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    
    // UVのY座標 (上=0.0、下=1.0) を使って、この頂点に適用すべき半径を補間
    float currentRadius = lerp(gPrimitive[instanceID].topRadius, gPrimitive[instanceID].bottomRadius, input.texcoord.y);
    
    // 頂点位置の変形 (XZ平面を半径で、Y軸を高さでスケール)
    float4 localPos;
    localPos.x = input.position.x * currentRadius;
    localPos.y = input.position.y * gPrimitive[instanceID].height;
    localPos.z = input.position.z * currentRadius;
    localPos.w = 1.0f;
    
    // テーパー(円錐台)に対応した法線の再計算
    // 単位シリンダーの position.xz は cos(phi), sin(phi) と同値であることを利用
    float dr = gPrimitive[instanceID].bottomRadius - gPrimitive[instanceID].topRadius;
    float3 localNormal = normalize(float3(gPrimitive[instanceID].height * input.position.x,dr,gPrimitive[instanceID].height * input.position.z));
    
    // 行列計算
    float4 worldPos = mul(localPos, gPrimitive[instanceID].world);
    output.position = mul(worldPos, gView.viewProjection);
    
    // UV座標を渡す
    output.texcoord = input.texcoord;
    
    // ワールド座標に変換
    output.worldPosition = mul(input.position, gPrimitive[instanceID].world).xyz;
    
    // 非均一スケールでワールド座標に変換
    output.normal = normalize(mul(localNormal, (float3x3) gPrimitive[instanceID].worldInverseTranspose));
    
    // 色
    output.color = gPrimitive[instanceID].color;
    
    // UVトランスフォーム
    output.uvTransform = gPrimitive[instanceID].uvTransform;
    
    // 環境
    output.environment = gPrimitive[instanceID].environment;
    
    // 光沢度
    output.shininess = gPrimitive[instanceID].shininess;
    
    // ライティング有効化
    output.enableLighting = gPrimitive[instanceID].enableLighting;
    
    // ディフューズ有効化
    output.enableDiffuse = gPrimitive[instanceID].enableDiffuse;
    
    // ハーフランバート反射有効化
    output.enableHalfLambert = gPrimitive[instanceID].enableHalfLambert;
    
    // スペキュラー有効化
    output.enableSpecular = gPrimitive[instanceID].enableSpecular;
    
    // ブリンフォン有効化
    output.enableBlinnPhong = gPrimitive[instanceID].enableBlinnPhong;
    
    // シャドウ有効化
    output.enableShadow = gPrimitive[instanceID].enableShadow;
    
    return output;
}