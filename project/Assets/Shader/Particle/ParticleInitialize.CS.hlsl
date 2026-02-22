
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

RWStructuredBuffer<int> gCounter : register(u1);

static const uint kMaxParticles = 1024;

[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint particleIndex = DTid.x;
    
    if(particleIndex < kMaxParticles)
    {
        // Particle構造体の全要素を0で埋める
        gParticles[particleIndex] = (Particle) 0;
    }
    
    if(particleIndex == 0)
    {
        gCounter[0] = 0;
    }
}