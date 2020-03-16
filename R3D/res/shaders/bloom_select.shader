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

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform vec3 invBloomThreshold;

void main()
{
	color = texture(screenTexture, TexCoords);
	if (dot(color.rgb, invBloomThreshold) > 1.0f)
	{
		brightColor = color;
	}
	else
	{
		brightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
}

