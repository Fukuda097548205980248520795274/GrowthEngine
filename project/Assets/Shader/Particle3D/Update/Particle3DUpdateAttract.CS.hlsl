#include "../Particle3D.hlsli"


ConstantBuffer<MaxNum> gMaxNum : register(b0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);

// 引き寄せ
struct Attract
{
    // 位置
    float3 position;
    
    // 加速度
    float acceleration;
};
ConstantBuffer<Attract> gAttract : register(b2);

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // パーティクルのインデックス
    uint particleIndex = DTid.x;
    
    // パーティクルの最大数を超えている場合は処理しない
    if (particleIndex >= gMaxNum.particleNum)
        return;
    
    // 寿命が0以下の場合は処理しない
    if (gParticles[particleIndex].lifeTime <= 0.0f)
        return;
 
    // パーティクルの更新
    if (gParticles[particleIndex].currentTime <= gParticles[particleIndex].lifeTime)
    {
        // 補間
        float t = gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime;
        
        // 色
        gParticles[particleIndex].color = lerp(gParticles[particleIndex].startColor, gParticles[particleIndex].endColor, t);
        
        // 移動
        gParticles[particleIndex].translate = lerp(gParticles[particleIndex].emitPos, gAttract.position, pow(t, gAttract.acceleration));
        
        // 回転
        gParticles[particleIndex].rotation = slerp(gParticles[particleIndex].startRotation, gParticles[particleIndex].endRotation, t);
        
        // 大きさ
        float3 scale = lerp(gParticles[particleIndex].startScale, gParticles[particleIndex].endScale, t);
        gParticles[particleIndex].scale = scale;
        
        // タイマーを進める
        gParticles[particleIndex].currentTime += gPerFrame.deltaTime;
        
        // タイマーが寿命を超えた場合は、フリーリストに追加して、パーティクルを消す
        if (gParticles[particleIndex].currentTime > gParticles[particleIndex].lifeTime)
        {
            // 寿命を0にする
            gParticles[particleIndex].lifeTime = 0.0f;
            
            // 大きさを0にする
            gParticles[particleIndex].scale = float3(0.0f, 0.0f, 0.0f);
            gParticles[particleIndex].color = float4(0.0f, 0.0f, 0.0f, 0.0f);
            gParticles[particleIndex].emitPos = float3(0.0f, 0.0f, 0.0f);
            
            int freeListIndex;
            
            // フリーリストに追加
            InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
            
            // フリーリストの次のインデックスに、パーティクルのインデックスを追加
            if ((freeListIndex + 1) < gMaxNum.particleNum)
            {
                gFreeList[freeListIndex + 1] = particleIndex;
            }
            else
            {
                // フリーリストのインデックスが最大数を超えた場合は、フリーリストのインデックスを減らす
                InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
            }

        }
    }
}