
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

        }
    }
}