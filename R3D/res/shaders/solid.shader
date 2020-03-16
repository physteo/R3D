#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTangent;
layout(location = 3) in vec4 aColor;

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
out     SunLight  outSunLight_tan;


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
//uniform mat4 lightSpaceMatrix; // shadow

// Matrices
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 normalMat;

// Camera
uniform vec3 cameraPos;

// Out
out vec4 outColor;
out vec3 outTangent;
out vec3 outFragPos;
out vec3 outFragPos_tan;
//out vec3 outFragPos_ls; // shadow
out vec3 outCameraPos_tan;

void main()
{
	gl_Position = projection * view * model *  vec4(aPosition, 1.0f);
	outFragPos = vec3(model * vec4(aPosition, 1.0));
	
	// TBN matrix
	vec3 normal =  normalize(vec3(normalMat * vec4(aNormal, 0.0f)));
	vec3 tangent = normalize(vec3(normalMat * vec4(aTangent, 0.0f)));
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	vec3 bitangent = normalize(cross(normal, tangent));
	mat3 TBN = mat3(tangent, bitangent, normal);
	mat3 iTBN = transpose(TBN);
	
	outFragPos_tan = iTBN * outFragPos;
	outCameraPos_tan = iTBN * cameraPos;
	
	outColor = aColor;
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
	vec3 color;
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

ColorSet calcPhongShading(vec3 viewDir, vec3 lightDir, vec3 ambient, vec3 diffuse, vec3 specular, vec3 norm, Material material)
{
	// diffuse
	float diff = max(dot(norm, lightDir), 0.0);

	// specular
	//vec3 halfwayDir = normalize(lightDir + viewDir);
	//float spec = pow(max(dot(halfwayDir, norm), 0.0), shininess);//material.shininess);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	// combine
	vec3 ambientCol = ambient  * material.color;          //vec3(texture(material.diffuse, TexCoords));
	vec3 diffuseCol = diffuse  * (diff * material.color); //vec3(texture(material.diffuse, TexCoords)));
	vec3 specularCol = specular * (spec * material.color);//vec3(texture(material.specular, TexCoords)));

	float shadow = 0.0;// ShadowCalculation(FragPosLightSpace, shadowBias, shadowTexture);
	
    ColorSet colorSet;
    colorSet.ambient = ambientCol;
    colorSet.diffuse = (1.0 - shadow) * diffuseCol;
    colorSet.specular = (1.0 - shadow) * specularCol;
    
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
vec3 calcSpotLight(SpotLight light, vec3 FragPos, vec3 viewDir, vec3 norm, Material material)//vec4 FragPosLightSpace, float shadowBias, sampler2D shadowTexture)
{
	vec3 lightDir = normalize(light.eye - FragPos);

	ColorSet cs = calcPhongShading(viewDir, lightDir, light.ambient, light.diffuse, light.specular, norm, material);
    
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
vec3 calcSunLight(SunLight light, vec3 viewDir, vec3 norm, Material material)//vec4 FragPosLightSpace, float shadowBias, sampler2D shadowTexture)
{
	vec3 lightDir = normalize(light.eye - light.center);
	
	ColorSet cs = calcPhongShading(viewDir, lightDir, light.ambient, light.diffuse, light.specular, norm, material);
    return cs.ambient + cs.diffuse + cs.specular;
}

//************** Sun Lights instances  **************
in SunLight  outSunLight_tan;

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
vec3 calcPointLight(PointLight light, vec3 FragPos, vec3 viewDir, vec3 norm, Material material)//vec4 FragPosLightSpace, float shadowBias, sampler2D shadowTexture)
{
	vec3 lightDir = normalize(light.eye - FragPos);

	ColorSet cs = calcPhongShading(viewDir, lightDir, light.ambient, light.diffuse, light.specular, norm, material);

    float fragDistance = length(light.eye - FragPos);

    float attenuation = 1.0 / (light.attenuation.constant + light.attenuation.linear * fragDistance + light.attenuation.quadratic * (fragDistance * fragDistance));

    return attenuation * (cs.ambient + cs.diffuse + cs.specular);
}

//************** Point Lights instances  **************
in PointLight  outPointLight_tan;

// In
in vec4 outColor;
in vec3 outTangent;
in vec3 outFragPos;
in vec3 outFragPos_tan;
in vec3 outCameraPos_tan;

void main()
{
	vec3 result = vec3(0.0);
	vec3 normal_tan = vec3(0.0, 0.0, 1.0);

	vec3 viewDir_tan = normalize(outCameraPos_tan - outFragPos_tan);

	//float shadowBias = max(0.002 * (1.0 - dot(outNormal, fs_in.sun_tan[i].direction)), 0.002);
	
	// ************ Point Light calculations (fs) ****************
	result += calcPointLight(outPointLight_tan, outFragPos_tan, viewDir_tan, normal_tan, material);
	
	// ************ Sun Light calculations (fs) ****************
	result += calcSunLight(outSunLight_tan, viewDir_tan, normal_tan, material);

	// ************ Spot Light calculations (fs) ****************
	result += calcSpotLight(outSpotLight_tan, outFragPos_tan, viewDir_tan, normal_tan, material);

	color = vec4(result.rgb, 1.0f);
	// debug stuff
	//vec3 temp = abs(outNormal + 1) / 2.0;
	//color = vec4(temp.rgb, 1.0);// calc_sunlight(sun, );
};
