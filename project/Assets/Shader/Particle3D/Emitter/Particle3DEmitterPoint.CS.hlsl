
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

// エミッター
struct Emitter
{
    // 位置
    float3 translate;

	// 放出数
    uint count;

	// 放出間隔の時間
    float frequency;

	// 放出間隔のタイマー
    float frequencyTimer;

    // 放出フラグ
    uint emit;
};
ConstantBuffer<Emitter> gEmitter : register(b0);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // 放出フラグが立っていない場合は処理しない
    if (gEmitter.emit == 0)
        return;

    for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)
    {
        gParticles[countIndex].translate = gEmitter.translate;
        gParticles[countIndex].lifeTime = 1.0f;
        gParticles[countIndex].scale = float3(1.0f, 1.0f, 1.0f);
        gParticles[countIndex].currentTime = 0.0f;
        gParticles[countIndex].color = float4(1.0f, 1.0f, 1.0f, 1.0f);
        gParticles[countIndex].velocity = float3(0.0f, 1.0f, 0.0f);
    }

}