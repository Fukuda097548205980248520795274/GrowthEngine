#include "../../Random/Random.hlsli"

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
    
    // 方向
    float3 direction;
    
    // 放出位置
    float3 emitPos;
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
    
    /// @brief 初期の色
    float4 startColor;

	/// @brief 最後の色
    float4 endColor;
    
    // 初期の大きさ
    float startScale;

	// 最後の大きさ
    float endScale;
    
    // 最小の生存時間
    float minLifeTime;

	// 最大の生存時間
    float maxLifeTime;
    
    // 初期の速度
    float startSpeed;
    
    // 最後の速度
    float endSpeed;
};
ConstantBuffer<Emitter> gEmitter : register(b0);

// パーティクルの最大数
struct ParticleMaxNum
{
    uint num;
};
ConstantBuffer<ParticleMaxNum> gParticleMaxNum : register(b1);

// フレームごとのデータ
struct PerFrame
{
    float deltaTime;
    float time;
};
ConstantBuffer<PerFrame> gPerFrame : register(b2);

RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // 放出フラグが立っていない場合は処理しない
    if (gEmitter.emit == 0)
        return;
    
    // 乱数生成期
    RandomGenerator generator;
    generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;

    for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)
    {
        int freeListIndex;
        
        // フリーリストの先頭からインデックスを取得
        InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
        
        // 取得したインデックスが有効な場合は、フリーリストからパーティクルのインデックスを取得
        if(0 <= freeListIndex && freeListIndex <= gParticleMaxNum.num)
        {
            uint particleIndex = gFreeList[freeListIndex];
            
            gParticles[particleIndex].translate = gEmitter.translate;
            gParticles[particleIndex].emitPos = gParticles[particleIndex].translate;
            gParticles[particleIndex].lifeTime = gEmitter.maxLifeTime - generator.Generate1d() * (gEmitter.maxLifeTime - gEmitter.minLifeTime);
            gParticles[particleIndex].scale = float3(1.0f, 1.0f, 1.0f);
            gParticles[particleIndex].currentTime = 0.0f;
            gParticles[particleIndex].color = gEmitter.startColor;
            
            // Emitter.hlsl 側の修正提案（より綺麗に飛ばすため）
            float3 randomDir = (generator.Generate3d() * 2.0f) - float3(1.0f, 1.0f, 1.0f);

            // ゼロ除算を防ぐため、ごくわずかな値を足してから正規化する
            gParticles[particleIndex].direction = normalize(randomDir + float3(0.001f, 0.0f, 0.0f));

        }
        else
        {
            // フリーリストからインデックスを取得できなかった場合は、放出数を減らす
            InterlockedAdd(gFreeListIndex[0], 1);

            // 放出数を減らした後、放出数が0になった場合は、放出フラグを下ろす
            break;
        }

    }

}