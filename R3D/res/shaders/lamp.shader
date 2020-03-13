#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;

out vec4 ourColor;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(aPosition, 1.0f);
	ourColor = aColor;
};

#shader fragment
#version 330 core

in vec4 ourColor;
out vec4 color;

void main()
{
	color = ourColor;
};
