#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 model;
uniform mat4 lightSpaceMat;

void main()
{
	gl_Position = lightSpaceMat * model * vec4(aPosition, 1.0f);
}

#shader fragment
#version 330 core

//out vec4 depth;

void main()
{
//	depth = vec4(gl_FragCoord.z, 0.0, 0.0, 1.0);
}
