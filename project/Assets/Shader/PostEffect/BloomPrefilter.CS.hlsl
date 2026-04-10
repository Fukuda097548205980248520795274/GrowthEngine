

Texture2D<float4> gColorTexture : register(t0);
RWTexture2D<float4> gOutputTexture : register(u0);

SamplerState gSampler : register(s0);

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
}