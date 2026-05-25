
struct Particle
{
    float3 translate;
    
    float lifeTime;
    
    float3 scale;

    float currentTime;
    
    float4 color;
    
    float4 startColor;

    float4 endColor;
    
    float3 direction;
    
    float startSpeed;
    
    float3 emitPos;
    
    float endSpeed;
    
    float4 rotation;
    
    float4 startRotation;
    
    float4 endRotation;
    
    float3 startScale;
    
    float3 endScale;
};

struct EmitOption
{
    float4 startColor;

    float4 endColor;
    
    float3 startScale;
    
    float minLifeTime;
    
    float3 endScale;

    float maxLifeTime;
    
    float startSpeed;
    
    float endSpeed;
    
    float4 startRotation;
    
    float4 endRotation;
};

struct Emitter
{
    float4 position;
    
    uint count;

    float frequency;

    float frequencyTimer;

    uint emit;
};

struct EmitRadius
{
    float radius1;
    
    float3 radius3;
};

struct MaxNum
{
    uint particleNum;
    
    uint emitterNum;
};

struct PerFrame
{
    float deltaTime;
    
    float time;
};

float4 slerp(float4 q1, float4 q2, float t)
{
    float cosOmega = dot(q1, q2);

    if (cosOmega < 0.0)
    {
        q2 = -q2;
        cosOmega = -cosOmega;
    }

    float4 result;

    if (cosOmega > 0.9999)
    {
        result = lerp(q1, q2, t);
        return normalize(result);
    }

    float omega = acos(cosOmega);
    float sinOmega = sqrt(1.0 - cosOmega * cosOmega);

    float weight1 = sin((1.0 - t) * omega) / sinOmega;
    float weight2 = sin(t * omega) / sinOmega;

    result = q1 * weight1 + q2 * weight2;
    
    return normalize(result);
}

float3x3 QuaternionToMatrix(float4 q)
{
    float xx = q.x * q.x;
    float yy = q.y * q.y;
    float zz = q.z * q.z;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;

    return float3x3(
        1.0 - 2.0 * (yy + zz), 2.0 * (xy - wz), 2.0 * (xz + wy),
              2.0 * (xy + wz), 1.0 - 2.0 * (xx + zz), 2.0 * (yz - wx),
              2.0 * (xz - wy), 2.0 * (yz + wx), 1.0 - 2.0 * (xx + yy)
    );
}