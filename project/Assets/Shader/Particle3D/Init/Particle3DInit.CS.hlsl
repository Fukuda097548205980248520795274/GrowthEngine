#include "../Particle3D.hlsli"

ConstantBuffer<MaxNum> gMaxNum : register(b0);

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // パーティクルのインデックス
    uint particleIndex = DTid.x;
    
    // パーティクルの最大数を超えている場合は処理しない
    if(particleIndex >= gMaxNum.particleNum)
        return;
    
    // パーティクルの初期化
    gParticles[particleIndex] = (Particle) 0;

    // フリーリストに追加
    gFreeList[particleIndex] = particleIndex;
    
    // 最後のパーティクルのインデックスをフリーリストの先頭にする
    if(particleIndex == 0)
    {
        gFreeListIndex[0] = gMaxNum.particleNum - 1;
    }
}