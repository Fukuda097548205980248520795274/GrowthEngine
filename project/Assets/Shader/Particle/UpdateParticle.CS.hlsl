
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

struct PerFrame
{
    // ゲームを起動してからの時間
    float time;
    
    // 1フレームの経過時間
    float deltaTime;
};
ConstantBuffer<PerFrame> gPerFrame : register(b0);

RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);

static const uint kMaxParticle = 1024;

[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint particleIndex = DTid.x;
    
    if(particleIndex < kMaxParticle)
    {
        // alphaが0のパーティクルは更新しない
        if(gParticles[particleIndex].color.a != 0.0f)
        {
            gParticles[particleIndex].translate += gParticles[particleIndex].velocity;
            gParticles[particleIndex].currentTime += gPerFrame.deltaTime;
            float alpha = 1.0f - (gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime);
            gParticles[particleIndex].color.a = saturate(alpha);

            if (gParticles[particleIndex].color.a == 0.0f)
            {
                gParticles[particleIndex].scale = float3(0.0f, 0.0f, 0.0f);
                
                int freeListIndex;
                InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
                
                // 最新のFreeListIndexの場所に死んだParticleのIndexを設定する
                if((freeListIndex + 1) < kMaxParticle)
                {
                    gFreeList[freeListIndex + 1] = particleIndex;
                }
                else
                {
                    InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
                }
            }
        }

    }
}