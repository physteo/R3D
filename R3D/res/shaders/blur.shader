#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
	TexCoords =     aTexCoords;
}

#shader fragment
#version 330 core

layout(location = 0) out vec3 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int axis;
uniform float gaussian[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
	vec2 tex_offset = 1.75 / textureSize(screenTexture, 0);
    float falloff[5] = float[](1, 1, 1, 1, 1);

	vec3 result = texture(screenTexture, TexCoords).rgb * gaussian[0];

	if (axis == 0)
	{
		for (int i = 1; i < 5; ++i)
		{
			result += texture(screenTexture, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * falloff[i] * gaussian[i];
			result += texture(screenTexture, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * falloff[i] * gaussian[i];
		}
	}
	else
	{
		for (int i = 1; i < 5; ++i)
		{
			result += texture(screenTexture, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * falloff[i] * gaussian[i];
			result += texture(screenTexture, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * falloff[i] * gaussian[i];
		}
	}

	FragColor = result;

}
