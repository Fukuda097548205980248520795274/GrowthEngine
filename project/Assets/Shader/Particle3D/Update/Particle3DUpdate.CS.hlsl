
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

// パーティクルの最大数
struct ParticleMaxNum
{
    uint num;
};
ConstantBuffer<ParticleMaxNum> gParticleMaxNum : register(b0);

// フレームごとのデータ
struct PerFrame
{
    float deltaTime;
    float time;
};
ConstantBuffer<PerFrame> gPerFrame : register(b1);

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
ConstantBuffer<Emitter> gEmitter : register(b2);

RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // パーティクルのインデックス
    uint particleIndex = DTid.x;
    
    // パーティクルの最大数を超えている場合は処理しない
    if(particleIndex >= gParticleMaxNum.num)
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
        gParticles[particleIndex].color = lerp(gEmitter.startColor, gEmitter.endColor, t);
        
        // 移動
        float speed = lerp(gEmitter.startSpeed, gEmitter.endSpeed, t);
        gParticles[particleIndex].translate += gParticles[particleIndex].velocity * speed * gPerFrame.deltaTime;
        
        // 大きさ
        float scale = lerp(gEmitter.startScale, gEmitter.endScale, t);
        gParticles[particleIndex].scale = float3(scale, scale, scale);
        
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
            
            int freeListIndex;
            
            // フリーリストに追加
            InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
            
            // フリーリストの次のインデックスに、パーティクルのインデックスを追加
            if ((freeListIndex + 1) < gParticleMaxNum.num)
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