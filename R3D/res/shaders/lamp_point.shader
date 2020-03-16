#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;


uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

//uniform vec3 direction;
//uniform float cameraDistance;
//flat out vec4 gDirection;
//flat out vec4 gOffsetX;
//flat out vec4 gOffsetY;

out vec4 ourColor;

void main()
{
	gl_Position = projection * view * model * vec4(aPosition, 1.0f);
	//float scale = cameraDistance / 10.0f;
	//gDirection = projection * view * model * vec4(normalize(direction) * scale, 0.0f);
	//gOffsetX = projection * view * model * vec4(scale / 2.0f, 0.0f, 0.0f, 0.0f);
	//gOffsetY = projection * view * model * vec4(0.0f, 0.0f, scale / 2.0f, 0.0f);
	ourColor = aColor;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec4 ourColor;

void main()
{
	color = 20.0f * ourColor;
};

