#shader vertex
#version 330 core

layout(location = 0) in vec3 inPos;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(inPos, 1.0f);
}

#shader fragment
#version 330 core

out vec4 color;

void main()
{
	color = vec4(10.0, 0.0, 0.0, 1.0);
}