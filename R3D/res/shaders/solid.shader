#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTangent;
layout(location = 3) in vec2 aUVs;

// *************** Generic Lights
struct LightAttenuation
{
	float constant;
	float linear;
	float quadratic;
};

//**************** Spot Light ****************
struct SpotLight
{
	float cutOff;
	vec3 eye;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	LightAttenuation attenuation;
};

//**************** Spot Light functions (vs) ****************
SpotLight transformSpotLight(SpotLight sl, mat3 m)
{
	SpotLight outSl;
	outSl = sl;
	outSl.eye = m * sl.eye;
	outSl.direction = m * sl.direction;

	return outSl;
}

//************** Spot Lights instances  **************
uniform SpotLight  spotLight;
out     SpotLight  outSpotLight_tan;

//**************** Sun Light ****************
struct SunLight
{
	vec3 eye;
	vec3 center;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

//**************** Sun Light functions (vs) ****************
SunLight transformSunLight(SunLight sl, mat3 m)
{
	SunLight outSl;
	outSl = sl;
	outSl.eye = m * sl.eye;
	outSl.center = m * sl.center;

	return outSl;
}

//************** Sun Lights instances  **************
uniform SunLight  sunLight;
uniform mat4 sunLightSpaceMat; // shadow
out SunLight  outSunLight_tan;
out vec4 outFragPos_sl; // shadow

//**************** Point Light ****************
struct PointLight
{
	vec3 eye;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	LightAttenuation attenuation;
};

//**************** Point Light functions (vs) ****************
PointLight transformPointLight(PointLight pl, mat3 m)
{
	PointLight outPl;
	outPl = pl;
	outPl.eye = m * pl.eye;

	return outPl;
}

//************** Point Lights instances  **************
uniform PointLight  pointLight;
out     PointLight  outPointLight_tan;

// Matrices
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 normalMat;

// Camera
uniform vec3 cameraPos;

// Out
out vec3 outTangent;
out vec3 outFragPos_tan;
out vec3 outCameraPos_tan;
out vec2 outUVs;

void main()
{
	gl_Position = projection * view * model *  vec4(aPosition, 1.0f);
	vec3 outFragPos = vec3(model * vec4(aPosition, 1.0));
	outFragPos_sl = sunLightSpaceMat * vec4(outFragPos, 1.0); // shadow
	outUVs = aUVs;

	// TBN matrix
	vec3 normal =  normalize(vec3(normalMat * vec4(aNormal, 0.0f)));
	vec3 tangent = normalize(vec3(normalMat * vec4(aTangent, 0.0f)));
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	vec3 bitangent = normalize(cross(normal, tangent));
	mat3 TBN = mat3(tangent, bitangent, normal);
	mat3 iTBN = transpose(TBN);
	
	outFragPos_tan = iTBN * outFragPos;
	outCameraPos_tan = iTBN * cameraPos;
	
	outTangent = tangent;

	// ************ Point Light calculations (vs) ****************
	outPointLight_tan = transformPointLight(pointLight, iTBN);

	// ************ Sun Light calculations (vs) ****************
	outSunLight_tan = transformSunLight(sunLight, iTBN);

	// ************ Spot Light calculations (vs) ****************
	outSpotLight_tan = transformSpotLight(spotLight, iTBN);

	//outFragPos_ls = lightSpaceMatrix[i] * vec4(FragPos, 1.0f); // shadow
}
#shader fragment
#version 330 core

layout (location = 0) out vec4 color;

struct ColorSet 
{
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
};

//************** Materials *************
struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	float shininess;
};
uniform Material material;

// *************** Generic Lights
struct LightAttenuation
{
	float constant;
	float linear;
	float quadratic;
};

ColorSet calcPhongShading(vec3 viewDir, vec3 lightDir, vec3 ambient, vec3 diffuse, vec3 specular, vec3 norm, Material material, vec2 UV)
{
	// diffuse
	float diff = max(dot(norm, lightDir), 0.0);

	// specular
	//vec3 halfwayDir = normalize(lightDir + viewDir);
	//float spec = pow(max(dot(halfwayDir, norm), 0.0), 1 + 0 * material.shininess);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	// combine
	vec3 ambientCol  =         ambient * texture(material.diffuse, UV).rgb;
	vec3 diffuseCol  = diff * diffuse  * texture(material.diffuse, UV).rgb;
	vec3 specularCol = spec * specular * texture(material.specular, UV).rgb;
	
    ColorSet colorSet;
    colorSet.ambient = ambientCol;
    colorSet.diffuse = diffuseCol;
    colorSet.specular = specularCol;

    return colorSet;
}

//**************** Spot Light ****************
struct SpotLight
{
	float cutOff;
	vec3 eye;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	LightAttenuation attenuation;
};

//**************** Spot Light functions (fs) ****************
vec3 calcSpotLight(SpotLight light, vec3 FragPos, vec3 viewDir, vec3 norm, Material material, vec2 UV)//vec4 FragPosLightSpace, float shadowBias, sampler2D shadowTexture)
{
	vec3 lightDir = normalize(light.eye - FragPos);

	ColorSet cs = calcPhongShading(viewDir, lightDir, light.ambient, light.diffuse, light.specular, norm, material, UV);
    
    float cosTheta = -dot(normalize(lightDir), normalize(light.direction));

    float outerCutOff = light.cutOff + 0.01;
    float epsilon   = light.cutOff - outerCutOff;
    float intensity = clamp((cosTheta - outerCutOff) / epsilon, 0.0, 1.0); 

	float fragDistance = length(light.eye - FragPos);

	float attenuation = 1.0 / (light.attenuation.constant + light.attenuation.linear * fragDistance + light.attenuation.quadratic * (fragDistance * fragDistance));

    return attenuation * (cs.ambient + (1 - intensity) * (cs.diffuse + cs.specular));
}

//************** Spot Lights instances  **************
in SpotLight  outSpotLight_tan;

//**************** Sun Light ****************
struct SunLight
{
	vec3 eye;	
	vec3 center;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

//**************** Sun Light functions (fs) ****************
ColorSet calcSunLight(SunLight light, vec3 viewDir, vec3 norm, Material material, vec2 UV)//vec4 FragPosLightSpace, float shadowBias, sampler2D shadowTexture)
{
	vec3 lightDir = normalize(light.eye - light.center);
	
	return calcPhongShading(viewDir, lightDir, light.ambient, light.diffuse, light.specular, norm, material, UV);
}

//************** Sun Lights instances  **************
in SunLight  outSunLight_tan;
uniform sampler2D shadowMap; // shadow
uniform float minShadowBias;
uniform float maxShadowBias;
in vec4 outFragPos_sl; // shadow

//**************** Point Light ****************
struct PointLight
{
	vec3 eye;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	LightAttenuation attenuation;
};

//**************** Point Light functions (fs) ****************
vec3 calcPointLight(PointLight light, vec3 FragPos, vec3 viewDir, vec3 norm, Material material, vec2 UV)//vec4 FragPosLightSpace, float shadowBias, sampler2D shadowTexture)
{
	vec3 lightDir = normalize(light.eye - FragPos);

	ColorSet cs = calcPhongShading(viewDir, lightDir, light.ambient, light.diffuse, light.specular, norm, material, UV);

    float fragDistance = length(light.eye - FragPos);

    float attenuation = 1.0 / (light.attenuation.constant + light.attenuation.linear * fragDistance + light.attenuation.quadratic * (fragDistance * fragDistance));

    return attenuation * (cs.ambient + cs.diffuse + cs.specular);
}

//************** Point Lights instances  **************
in PointLight  outPointLight_tan;

//************** Shadow functions
float calcShadow(vec4 fragPos_lightSpace, vec3 normal, vec3 lightDir)
{
	vec3 projCoords = fragPos_lightSpace.xyz / fragPos_lightSpace.w; // for perspective
	projCoords = projCoords * 0.5 + 0.5; // NDC
	
	float currentDepth = projCoords.z;

	float bias = max(maxShadowBias * (1.0 - dot(normal, lightDir)), minShadowBias);

	float shadow = 0.0;
	vec2 texelSize = 1.5 / textureSize(shadowMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	if (projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}

// In
in vec3 outTangent;
in vec3 outFragPos_tan;
in vec3 outCameraPos_tan;
in vec2 outUVs;

void main()
{
	vec3 result = vec3(0.0);

	vec3 normal_tan = normalize(2.0 * vec3(texture(material.normal, outUVs)) - 1.0);
	vec3 viewDir_tan = normalize(outCameraPos_tan - outFragPos_tan);

	// ************ Point Light calculations (fs) ****************
	result += calcPointLight(outPointLight_tan, outFragPos_tan, viewDir_tan, normal_tan, material, outUVs);
	
	// ************ Sun Light calculations (fs) ****************
	ColorSet sunCs = calcSunLight(outSunLight_tan, viewDir_tan, normal_tan, material, outUVs);
	// ************ sun shadow calculations (fs) ************
	vec3 lightDir_tan = normalize(outSunLight_tan.eye - outSunLight_tan.center);
    float shadow = calcShadow(outFragPos_sl, normal_tan, lightDir_tan);
    result += sunCs.ambient + (1.0 - shadow) * (sunCs.diffuse + sunCs.specular);
	
	// ************ Spot Light calculations (fs) ****************
	result += calcSpotLight(outSpotLight_tan, outFragPos_tan, viewDir_tan, normal_tan, material, outUVs);
	color = vec4(result.rgb, 1.0f);
	//color = vec4(outUVs, 0.0f, 1.0f);
	//color = vec4(vec3(texture(material.normal, outUVs)), 1.0f);

	// debug stuff
	//vec3 temp = abs(outNormal + 1) / 2.0;
	//color = vec4(temp.rgb, 1.0);// calc_sunlight(sun, );
};
