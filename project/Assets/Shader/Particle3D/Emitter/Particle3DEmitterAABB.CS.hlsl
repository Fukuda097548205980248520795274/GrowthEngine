#include "../../Random/Random.hlsli"
#include "../Particle3D.hlsli"

ConstantBuffer<EmitOption> gEmitOption : register(b0);
ConstantBuffer<MaxNum> gMaxNum : register(b1);
ConstantBuffer<PerFrame> gPerFrame : register(b2);
ConstantBuffer<EmitRadius> gEmitRadius : register(b3);

StructuredBuffer<Emitter> gEmitter : register(t0);

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // エミッターインデックス
    int emitterIndex = DTid.x;
    
    // エミッター最大数を超えたら処理しない
    if(emitterIndex >= gMaxNum.emitterNum)
        return;
    
    // 放出フラグが立っていない場合は処理しない
    if (gEmitter[emitterIndex].emit == 0)
        return;
    
    // 乱数生成期
    RandomGenerator generator;
    generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;

    for (uint countIndex = 0; countIndex < gEmitter[emitterIndex].count; ++countIndex)
    {
        int freeListIndex;
        
        // フリーリストの先頭からインデックスを取得
        InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
        
        // 取得したインデックスが有効な場合は、フリーリストからパーティクルのインデックスを取得
        if (0 <= freeListIndex && freeListIndex <= gMaxNum.particleNum)
        {
            uint particleIndex = gFreeList[freeListIndex];
            
            // 放出位置
            float3 emitPos = gEmitter[emitterIndex].position.xyz + (generator.Generate3d() * 2.0f - float3(1.0f, 1.0f, 1.0f)) * gEmitRadius.radius3;
            
            gParticles[particleIndex].translate = emitPos;
            gParticles[particleIndex].emitPos = emitPos;
            gParticles[particleIndex].lifeTime = gEmitOption.maxLifeTime - generator.Generate1d() * (gEmitOption.maxLifeTime - gEmitOption.minLifeTime);
            gParticles[particleIndex].scale = gEmitOption.startScale;
            gParticles[particleIndex].startScale = gEmitOption.startScale;
            gParticles[particleIndex].endScale = gEmitOption.endScale;
            gParticles[particleIndex].startSpeed = gEmitOption.startSpeed;
            gParticles[particleIndex].endSpeed = gEmitOption.endSpeed;
            gParticles[particleIndex].currentTime = 0.0f;
            gParticles[particleIndex].color = gEmitOption.startColor;
            gParticles[particleIndex].startColor = gEmitOption.startColor;
            gParticles[particleIndex].endColor = gEmitOption.endColor;
            gParticles[particleIndex].rotation = gEmitOption.startRotation;
            gParticles[particleIndex].startRotation = gEmitOption.startRotation;
            gParticles[particleIndex].endRotation = gEmitOption.endRotation;
            
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