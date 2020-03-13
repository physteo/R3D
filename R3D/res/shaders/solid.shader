#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTangent;
layout(location = 3) in vec4 aColor;

// Structs
struct Sun {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

// Uniforms
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 normalMat;
uniform vec3 cameraPos;
uniform Sun  sun;
//uniform mat4 lightSpaceMatrix; // shadow

// Out
out vec4 outColor;
out vec3 outTangent;
out vec3 outFragPos;
out vec3 outFragPos_tan;
//out vec3 outFragPos_ls; // shadow
out vec3 outCameraPos_tan;
out Sun  outSun_tan;

void main()
{

	gl_Position = projection * view * model *  vec4(aPosition, 1.0f);
	outFragPos = vec3(model * vec4(aPosition, 1.0));
	
	vec3 normal = normalize(normalMat * vec4(aNormal, 0.0f)).xyz;
	vec3 tangent = normalize(normalMat * vec4(aTangent, 0.0f)).xyz;
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	vec3 bitangent = normalize(cross(normal, tangent));
	mat3 TBN = mat3(tangent, bitangent, normal);
	mat3 iTBN = transpose(TBN);
	
	outFragPos_tan = iTBN * outFragPos;
	outCameraPos_tan = iTBN * cameraPos;
	
	outColor = aColor;
	outTangent = tangent;

	// lights
	outSun_tan.direction = normalize(iTBN * sun.direction);
	outSun_tan.ambient   = sun.ambient;
	outSun_tan.diffuse   = sun.diffuse;
	outSun_tan.specular  = sun.specular;
	//outFragPos_ls = lightSpaceMatrix[i] * vec4(FragPos, 1.0f); // shadow
}
#shader fragment
#version 330 core

struct Sun {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

// Uniforms
uniform vec3 uniColor;

// In
in vec4 outColor;
in vec3 outTangent;
in vec3 outFragPos;
in vec3 outFragPos_tan;
in vec3 outCameraPos_tan;
in Sun  outSun_tan;

// Out
out vec4 color;

vec3 calc_sunlight(Sun light, vec3 viewDir, vec3 norm, vec3 color)//vec4 FragPosLightSpace, float shadowBias, sampler2D shadowTexture)
{
	float shininess = 32; // TODO: get from the material of the object

	vec3 lightDir = normalize(-light.direction);

	// diffuse
	float diff = max(dot(norm, lightDir), 0.0);

	// specular
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(halfwayDir, norm), 0.0), shininess);//material.shininess);
	
	// combine
	vec3 ambient = light.ambient  * color;          //vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse  * (diff * color); //vec3(texture(material.diffuse, TexCoords)));
	vec3 specular = light.specular * (spec * color);//vec3(texture(material.specular, TexCoords)));

	float shadow = 0.0;// ShadowCalculation(FragPosLightSpace, shadowBias, shadowTexture);
	return (ambient + (1.0 - shadow) * (diffuse + specular));
}

void main()
{
	vec3 result = vec3(0.0);
	vec3 normal = vec3(0.0, 0.0, 1.0);

	vec3 viewDir_tan = normalize(outCameraPos_tan - outFragPos_tan);

	//float shadowBias = max(0.002 * (1.0 - dot(outNormal, fs_in.sun_tan[i].direction)), 0.002);
	result += calc_sunlight(outSun_tan, viewDir_tan, normal, uniColor.rgb);
	color = vec4(result.rgb, 1.0);

	//vec3 temp = abs(outNormal + 1) / 2.0;
	//color = vec4(temp.rgb, 1.0);// calc_sunlight(sun, );
};
