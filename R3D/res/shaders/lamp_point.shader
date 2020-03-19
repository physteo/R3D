#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(aPosition, 1.0f);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec3 lightColor;

void main()
{
	color = vec4(10.0f * lightColor, 1.0);
};

