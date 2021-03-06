#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
	TexCoords = aTexCoords;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D brightColors;

uniform float bloom;

void main()
{
	FragColor = vec4(texture(screenTexture, TexCoords).rgb 
		+ bloom * texture(brightColors, TexCoords).rgb, 1.0f);
}