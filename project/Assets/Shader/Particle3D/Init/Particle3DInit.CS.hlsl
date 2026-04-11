
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

// パーティクルの最大数
struct ParticleMaxNum
{
    uint num;
};
ConstantBuffer<ParticleMaxNum> gParticleMaxNum : register(b0);

RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);


[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // パーティクルのインデックス
    uint particleIndex = DTid.x;
    
    // パーティクルの最大数を超えている場合は処理しない
    if(particleIndex >= gParticleMaxNum.num)
        return;
    
    // パーティクルの初期化
    gParticles[particleIndex] = (Particle) 0;

    // フリーリストに追加
    gFreeList[particleIndex] = particleIndex;
    
    // 最後のパーティクルのインデックスをフリーリストの先頭にする
    if(particleIndex == 0)
    {
        gFreeListIndex[0] = gParticleMaxNum.num - 1;
    }
}