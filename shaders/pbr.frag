#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
	mat4 modelInvTr;
	vec4 camPos;
	vec4 lightPos;
	vec4 lightCol;
	float metallic;
	float roughness;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform sampler2D texSampler2;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNor;
layout(location = 2) in vec3 fragTan;
layout(location = 3) in vec3 fragBit;
layout(location = 4) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

const float PI = 3.14159f;
const float GAMMA = 0.45454545454f;

vec3 fresnelReflectance(float cosViewingAngle, vec3 R) {
    float cosTheta = clamp(1 - cosViewingAngle, 0.f, 1.f);
    return R + ((1.0f - R) * pow(cosTheta, 5.0f));


}

float microfacetSelfShadow(float roughness, vec3 wo, vec3 wi, vec3 n) {
    float k = ((roughness + 1.0f) * (roughness + 1.0f)) / 8.0f;
    float nDotWo = max(0.f, dot(n, wo));
    float nDotWi = max(0.f, dot(n, wi));
    float schlickWo = nDotWo / ((nDotWo * (1.0f - k)) + k);
    float schlickWi = nDotWi / ((nDotWi * (1.0f - k)) + k);
    return schlickWo * schlickWi;


}

float microfacetDistrib(float roughness, vec3 n, vec3 wh) {
    float nDotWh = max(0.f, dot(n, wh));
    float roughnessSq = roughness * roughness;
    float alphaSq = roughnessSq * roughnessSq;
    float innerDenom = (nDotWh * nDotWh * (alphaSq - 1.0f)) + 1.0f;
    float denom = PI * innerDenom * innerDenom;
    return alphaSq / denom;
}

void main()
{
    vec3 normal = normalize(fragNor);
	vec3 mappedNor = texture(texSampler2, fragUV).rgb;
	mappedNor = normalize((2.0 * mappedNor) - 1.0);
	mappedNor.x = clamp(mappedNor.x, 0, 1);
	mappedNor.y = clamp(mappedNor.y, 0, 1);
	mappedNor.z = clamp(mappedNor.z, 0, 1);
	mappedNor = normalize(mappedNor);
	vec3 normalizedTan = normalize(fragTan);
	vec3 normalizedBit = normalize(fragBit);
	mat3 surfaceToWorldN = mat3(normalizedTan, normalizedBit, normal);
	mappedNor = surfaceToWorldN * mappedNor;
	normal = normalize(mappedNor);

	vec3 albedo = vec3(texture(texSampler, fragUV));

    // output light color of fragment
    vec3 Lo = vec3(0,0,0);

	// attenuate intensity inverse square law
	vec3 fragToLightPos = ubo.lightPos.xyz - fragPos;
	float distanceSq = dot(fragToLightPos, fragToLightPos);
	float inverseDistanceSq = 1.0f / distanceSq;
	vec3 ithLightIrradiance = ubo.lightCol.rgb * inverseDistanceSq;

	// get wi, wo, wh
	vec3 wi = normalize(fragToLightPos);
	vec3 wo = normalize(ubo.camPos.rgb - fragPos);
	vec3 wh = normalize(wo + wi);

	// R term
	vec3 R = mix(vec3(0.04), albedo, ubo.metallic);

	// F term
	vec3 F = fresnelReflectance(max(dot(wh, wo), 0.0), R);

	// G term
	float G = microfacetSelfShadow(ubo.roughness, wo, wi, normal);

	// D term
	float D = microfacetDistrib(ubo.roughness, normal, wh);

	// compute cook-torrance
	vec3 f_cook_torrance = vec3(0,0,0);
	if (dot(normal, wi) >= 0.00001 && dot(normal, wo) >= 0.00001) {
		f_cook_torrance = (D*G*F) / (4.0f * dot(normal, wo) * dot(normal, wi));
	}

	// compute lambert weighting
	vec3 kd = vec3(1.0f) - F;

	kd *= (1.0f - ubo.metallic);

	vec3 f_lambert = albedo * 0.31831015504887652430775499030746f;
	vec3 f = kd * f_lambert + f_cook_torrance;

	// accumulate total lighting on fragment
	Lo += f * ithLightIrradiance * max(0.f, dot(wi, normal));

	// reinhard (HDR)
	Lo = Lo / (Lo + vec3(1.0f, 1.0f, 1.0f));

	// gamma correct
	Lo = pow(Lo, vec3(GAMMA, GAMMA, GAMMA));

	outColor = vec4(Lo, 1);
}