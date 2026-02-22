
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

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // 放出許可が出たら放出
    if(gEmitter.emit != 0)
    {
        // カウント分放出
        for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {
            gParticles[countIndex].scale = float3(0.3f, 0.3f, 0.3f);
            gParticles[countIndex].translate = float3(0.0f, 0.0f, 0.0f);
            gParticles[countIndex].color = float4(1.0f, 0.0f, 0.0f, 1.0f);
        }

    }
}