
// パーティクル
struct Particle
{
    // 位置
    float3 translate;
    
    // 生存時間
    float lifeTime;
    
    // 大きさ
    float3 scale;

    // 現在の時間
    float currentTime;
    
    // 色
    float4 color;
    
    // 速度
    float3 velocity;
};
RWStructuredBuffer<Particle> gParticles : register(u0);

// パーティクルの数
struct ParticleNum
{
    uint num;
};
ConstantBuffer<ParticleNum> gParticleNum : register(b0);


[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // パーティクルのインデックス
    uint particleIndex = DTid.x;
    
    // パーティクルの数を超えている場合は処理しない
    if(particleIndex >= gParticleNum.num)
        return;
    
    // パーティクルの初期化
    gParticles[particleIndex] = (Particle) 0;

}