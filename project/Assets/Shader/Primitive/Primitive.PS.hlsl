#include "Primitive.hlsli"

// 出力ピクセルシェーダ
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

// マテリアル
struct Material
{
    // 色
    float4 color;
    
    // uv行列
    float4x4 uvMatrix;
    
    // 環境
    float environment;
    
    // 光沢度
    float shininess;
    
    // ライティング有効化
    int enableLighting;
    
    // ディフューズ
    int enableDiffuse;
    
    // ハーフランバート有効化
    int enableHalfLambert;
    
    // スペキュラー有効化
    int enableSpecular;
    
    // ブリンフォン有効化
    int enableBlinnPhong;
};
ConstantBuffer<Material> gMaterial : register(b0);

// シャドウ用座標変換
struct ShadowTransformation
{
    float4x4 viewProjection;
};
ConstantBuffer<ShadowTransformation> gShadowTransformation : register(b1);

// カメラ
struct Camera
{
    float3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b2);

// ライト数
struct NumLight
{
    // 平行光源
    int directionalLight;
    
    // ポイントライト
    int pointLight;
    
    // スポットライト
    int spotLight;
};
ConstantBuffer<NumLight> gNumLight : register(b3);


// テクスチャ
Texture2D<float4> gTexture : register(t0);

// シャドウマップ用テクスチャ
Texture2D<float> gShadowMap : register(t1);

// 環境マップテクスチャ
TextureCube<float4> gEnvironmentTexture : register(t2);

// 平行光源
struct DirectionalLight
{
    // 向き
    float3 direction;
    
    // 輝度
    float intensity;
    
     // 色
    float4 color;
};
StructuredBuffer<DirectionalLight> gDirectionalLight : register(t3);

// ポイントライト
struct PointLight
{
     // 色
    float4 color;
    
    // 位置
    float3 position;
    
    // 輝度
    float intensity;
    
    // ライトの届く最大距離
    float radius;
    
    // 減衰率
    float decay;
};
StructuredBuffer<PointLight> gPointLight : register(t4);

// スポットライト
struct SpotLight
{
    // 色
    float4 color;
    
    // 位置
    float3 position;
    
    // 輝度
    float intensity;
    
    // 方向
    float3 direction;
    
    // ライトの届く距離
    float distance;
    
    // 減衰率
    float decay;
    
    // 光の当たる角度
    float cosAngle;
    
    // フォールオフ開始値
    float cosFalloffStart;
};
StructuredBuffer<SpotLight> gSpotLight : register(t5);


// サンプラー
SamplerState gSampler : register(s0);

// シャドウ用比較サンプラー
SamplerComparisonState gShadowSampler : register(s1);




// 平行光源の拡散反射
float3 CreateDirectionalLightDiffuse(DirectionalLight light, VertexShaderOutput input)
{
    // ハーフランバート有効
    if (gMaterial.enableHalfLambert != 0)
    {
        // 光と法線の内積
        float NdotL = dot(normalize(input.normal), -light.direction);
            
        // なだらかにする
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
                
        // 光の拡散反射を返却する
        return light.color.rgb * cos * light.intensity;
    }
    else
    {
        // なだらかにする
        float cos = saturate(dot(normalize(input.normal), -light.direction));
                
        // 光の拡散反射を返却する
        return light.color.rgb * cos * light.intensity;
    }
}

// 平行光源の鏡面反射
float3 CreateDirectionalLightSpecular(DirectionalLight light, float3 toEye, VertexShaderOutput input)
{
    // 反射の強度
    float specularPow = 0.0f;
                    
    // ブリンフォン有効
    if (gMaterial.enableBlinnPhong != 0)
    {
        // ハーフベクトル
        float3 halfVector = normalize(-light.direction + toEye);

        // 法線とハーフベクトルの内積
        float NdotH = dot(normalize(input.normal), halfVector);

        specularPow = pow(saturate(NdotH), gMaterial.shininess);

    }
    else
    {
        // ブリンフォン無効
        
        // 入射光の反射ベクトル
        float3 reflectLight = reflect(light.direction, normalize(input.normal));

        // カメラと反射ベクトルの内積
        float RdotE = dot(reflectLight, toEye);

        specularPow = pow(saturate(RdotE), gMaterial.shininess);
    }

    return light.color.rgb * light.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
}

// ポイントライトの拡散反射
float3 CreatePointLightDiffuse(PointLight light, float3 pointLightDirection, VertexShaderOutput input)
{
    // ハーフランバート有効
    if (gMaterial.enableHalfLambert != 0)
    {
        // 光と法線の内積
        float NdotL = dot(normalize(input.normal), -pointLightDirection);
            
        // なだらかにする
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
                
        // 光の拡散反射を返却する
        return light.color.rgb * cos * light.intensity;
    }
    else
    {
        // なだらかにする
        float cos = saturate(dot(normalize(input.normal), -pointLightDirection));
                
        // 光の拡散反射を返却する
        return light.color.rgb * cos * light.intensity;
    }
}

// ポイントライトの鏡面反射
float3 CreatePointLightSpecular(PointLight light, float3 pointLightDirection, float3 toEye, VertexShaderOutput input)
{
    // 反射の強度
    float specularPow = 0.0f;
                    
    // ブリンフォン有効
    if (gMaterial.enableBlinnPhong != 0)
    {
        // ハーフベクトル
        float3 halfVector = normalize(-pointLightDirection + toEye);

        // 法線とハーフベクトルの内積
        float NdotH = dot(normalize(input.normal), halfVector);

        specularPow = pow(saturate(NdotH), gMaterial.shininess);

    }
    else
    {
        // ブリンフォン無効
        
        // 入射光の反射ベクトル
        float3 reflectLight = reflect(-pointLightDirection, normalize(input.normal));

        // カメラと反射ベクトルの内積
        float RdotE = dot(reflectLight, toEye);

        specularPow = pow(saturate(RdotE), gMaterial.shininess);
    }

    return light.color.rgb * light.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
}

// スポットライトの拡散反射
float3 CreateSpotLightDiffuse(SpotLight light, float3 spotLightDirectionOnSurface, VertexShaderOutput input)
{
    // ハーフランバート有効
    if (gMaterial.enableHalfLambert != 0)
    {
        // 光と法線の内積
        float NdotL = dot(normalize(input.normal), -spotLightDirectionOnSurface);
            
        // なだらかにする
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
                
        // 光の拡散反射を返却する
        return light.color.rgb * cos * light.intensity;
    }
    else
    {
        // なだらかにする
        float cos = saturate(dot(normalize(input.normal), -spotLightDirectionOnSurface));
                
        // 光の拡散反射を返却する
        return light.color.rgb * cos * light.intensity;
    }
}

// スポットライトの拡散反射
float3 CreateSpotLightSpecular(SpotLight light, float3 spotLightDirectionOnSurface, float3 toEye, VertexShaderOutput input)
{
    // 反射の強度
    float specularPow = 0.0f;
                    
    // ブリンフォン有効
    if (gMaterial.enableBlinnPhong != 0)
    {
        // ハーフベクトル
        float3 halfVector = normalize(-spotLightDirectionOnSurface + toEye);

        // 法線とハーフベクトルの内積
        float NdotH = dot(normalize(input.normal), halfVector);

        specularPow = pow(saturate(NdotH), gMaterial.shininess);

    }
    else
    {
        // ブリンフォン無効
        
        // 入射光の反射ベクトル
        float3 reflectLight = reflect(-spotLightDirectionOnSurface, normalize(input.normal));

        // カメラと反射ベクトルの内積
        float RdotE = dot(reflectLight, toEye);

        specularPow = pow(saturate(RdotE), gMaterial.shininess);
    }

    return light.color.rgb * light.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
}




PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // UV座標を座標変換する
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvMatrix);
    
    // テクスチャの色
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    
    if(gMaterial.enableLighting != 0)
    {
        // ワールド座標をライト空間に変換
        float4 shadowPos = mul(float4(input.worldPosition, 1.0f), gShadowTransformation.viewProjection);
        shadowPos.xyz /= shadowPos.w;
        
        // [-1,1] → [0,1]
        float2 shadowUV = ((float2) shadowPos + float2(1, -1)) * float2(0.5, -0.5);
        
        float shadow = gShadowMap.SampleCmpLevelZero(gShadowSampler, shadowUV, shadowPos.z - 0.005f);
        float shadowFactor = lerp(0.3f, 1.0f, shadow); // 影の濃さ調整
    
        // 環境光
        float3 cameraToPosition = normalize(input.worldPosition - gCamera.worldPosition);
        float3 reflectedVector = reflect(cameraToPosition, normalize(input.normal));
        float4 environmentColor = gEnvironmentTexture.Sample(gSampler, reflectedVector) * gMaterial.environment;
        
        // カメラへの方向を算出
        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        
        ///
        /// 平行光源
        ///
        
        // 平行光源の拡散反射
        float3 directionalLightDiffuse = float3(0.0f, 0.0f, 0.0f);
        
        // 平行光源の鏡面反射
        float3 directionalLightSpecular = float3(0.0f, 0.0f, 0.0f);

        // 使用している平行光源から値を取得する
        for (uint directionalLightIndex = 0; directionalLightIndex < gNumLight.directionalLight; ++directionalLightIndex)
        {
            // ディフューズ有効化
            if (gMaterial.enableDiffuse != 0)
            {
                // 光の拡散反射を加算する
                directionalLightDiffuse += CreateDirectionalLightDiffuse(gDirectionalLight[directionalLightIndex], input);
            }
                
            // スペキュラー有効
            if (gMaterial.enableSpecular != 0)
            {
                // 光の鏡面反射を加算する
                directionalLightSpecular += CreateDirectionalLightSpecular(gDirectionalLight[directionalLightIndex], toEye, input);
            }
        }
        
        
        ///
        /// ポイントライト
        ///
        
        // ポイントライトの拡散反射
        float3 pointLightDiffuse = float3(0.0f, 0.0f, 0.0f);
        
        // ポイントライトの鏡面反射
        float3 pointLightSpecular = float3(0.0f, 0.0f, 0.0f);
        
        // 使用しているポイントライトから値を取得する
        for (uint pointLightIndex = 0; pointLightIndex < gNumLight.pointLight; ++pointLightIndex)
        {
            // ポイントライトの光の向きを取得する
            float3 pointLightDirection = normalize(input.worldPosition - gPointLight[pointLightIndex].position);
            
            // ポイントライトの距離
            float distance = length(gPointLight[pointLightIndex].position - input.worldPosition);
            
            // 逆二乗則による減衰係数
            float factor = pow(saturate(-distance / gPointLight[pointLightIndex].radius + 1.0f), gPointLight[pointLightIndex].decay);
            
            // ディフューズ有効化
            if (gMaterial.enableDiffuse != 0)
            {
                // 拡散反射を加算する
                pointLightDiffuse += CreatePointLightDiffuse(gPointLight[pointLightIndex], pointLightDirection, input) * factor;
            }

            // スペキュラー有効
            if (gMaterial.enableSpecular != 0)
            {
                // 鏡面反射を加算する
                pointLightSpecular += CreatePointLightSpecular(gPointLight[pointLightIndex], pointLightDirection, toEye, input) * factor;

            }
        }
        
        
        ///
        /// スポットライト
        ///
        
        // スポットライトの拡散反射
        float3 spotLightDiffuse = float3(0.0f, 0.0f, 0.0f);
        
        // スポットライトの鏡面反射
        float3 spotLightSpecular = float3(0.0f, 0.0f, 0.0f);
        
        // 使用しているスポットライトから値を取得する
        for (uint spotLightIndex = 0; spotLightIndex < gNumLight.spotLight; ++spotLightIndex)
        {
            // スポットライトの光の向きを取得する
            float3 spotLightDirectionOnSurface = normalize(input.worldPosition - gSpotLight[spotLightIndex].position);
            
            
            // 光の当たる角度
            float cosAngle = dot(spotLightDirectionOnSurface, gSpotLight[spotLightIndex].direction);
            
            // フォールオフ
            float falloffFactor =
            saturate((cosAngle - gSpotLight[spotLightIndex].cosAngle) / (gSpotLight[spotLightIndex].cosFalloffStart - gSpotLight[spotLightIndex].cosAngle));
            
            
            // ポイントライトの距離
            float distance = length(gSpotLight[spotLightIndex].position - input.worldPosition);
            
            // 逆二乗則による減衰係数
            float factor = pow(saturate(-distance / gSpotLight[spotLightIndex].distance + 1.0f), gSpotLight[spotLightIndex].decay);
            
            // ディフューズ有効化
            if (gMaterial.enableDiffuse != 0)
            {
                // 拡散反射を加算する
                spotLightDiffuse += CreateSpotLightDiffuse(gSpotLight[spotLightIndex], spotLightDirectionOnSurface, input) * factor * falloffFactor;
            }
            
            // スペキュラー有効
            if (gMaterial.enableSpecular != 0)
            {
                // 鏡面反射の加算する
                spotLightSpecular += CreateSpotLightSpecular(gSpotLight[spotLightIndex], spotLightDirectionOnSurface, toEye, input) * factor * falloffFactor;
            }
        }
    
    
        // 色
        float3 baseColor = gMaterial.color.rgb * textureColor.rgb;
        
        // 光
        float3 directLight = directionalLightDiffuse + directionalLightSpecular + pointLightDiffuse + pointLightSpecular + spotLightDiffuse + spotLightSpecular;
        directLight *= shadowFactor;
        
        // 合成
        output.color.rgb = baseColor * (directLight + environmentColor.rgb);

    
        // アルファ
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        // 色
        output.color = gMaterial.color * textureColor;
    }
    
    // a = 0は描画しない
    if (output.color.a == 0.0f || textureColor.a == 0.0f)
    {
        discard;
    }
    
    return output;
}