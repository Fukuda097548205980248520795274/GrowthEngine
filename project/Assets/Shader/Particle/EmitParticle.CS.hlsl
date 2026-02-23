#include "../Random/Random.hlsli"

struct Particle
{
    float3 translate;
    float3 scale;
    float lifeTime;
    float3 velocity;
    float currentTime;
    float4 color;
};
RWStructuredBuffer<Particle> gParticles : register(u0);

struct EmitterSphere
{
    /// 位置
    float3 translate;

    /// 半径
    float radius;

	/// 放出数
    uint count;

	/// 射出間隔
    float frequency;

	/// 射出間隔調整用時間
    float frequencyTime;

	/// 射出許可
    uint emit;
};
ConstantBuffer<EmitterSphere> gEmitter : register(b0);

RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);

struct PerFrame
{
    // ゲームを起動してからの時間
    float time;
    
    // 1フレームの経過時間
    float deltaTime;
};
ConstantBuffer<PerFrame> gPerFrame : register(b1);

static const uint kMaxParticle = 1024;

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // 放出許可が出たら放出
    if(gEmitter.emit != 0)
    {
        RandomGenerator generator;
        generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;
        
        // カウント分放出
        for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {
            int freeListIndex;
            InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
        
            // FreeListのIndexを1つ前に設定し、現在のIndexを取得する
            if (0 <= freeListIndex && freeListIndex < kMaxParticle)
            {
                uint particleIndex = gFreeList[freeListIndex];
                
                gParticles[particleIndex].scale = generator.Generate3d();
                gParticles[particleIndex].translate = generator.Generate3d();
                gParticles[particleIndex].lifeTime = 1.0f;
                gParticles[particleIndex].currentTime = 0.0f;
                gParticles[particleIndex].velocity = generator.Generate3d() * (1.0f / 60.0f);
                gParticles[particleIndex].color.rgb = generator.Generate3d();
                gParticles[particleIndex].color.a = 1.0f;
            }
            else
            {
                // 発生させられなかったら減らしてしまった分、元に戻す
                InterlockedAdd(gFreeListIndex[0], 1);
                
                break;
            }
        }

    }
}