#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in mat4 aInstanceModelMatrix;
layout(location = 5) in vec4 aInstanceColor;

out vec4 ourColor;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * aInstanceModelMatrix * vec4(aPosition, 1.0f);
	ourColor = aInstanceColor;
};

#shader fragment
#version 330 core

in vec4 ourColor;

out vec4 color;

void main()
{
	color = ourColor;
};
