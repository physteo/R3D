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

float r = 0.001;

void main()
{
	FragColor = vec3(0.0f);
	for(int i = -1; i <= 1; i++)
    {
	   for(int j = -1; j <= 1; j++)
       {
		   FragColor += texture(screenTexture, TexCoords + r * vec2(i,j)).rgb;
       }	
    }
    FragColor /= 9.0f;
}