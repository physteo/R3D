#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform vec3 direction;
uniform float cameraDistance;

flat out vec4 gDirection;
flat out vec4 gOffsetX;
flat out vec4 gOffsetY;

void main()
{
	float scale = cameraDistance / 10.0f;
	mat4 mvp = projection * view * model;
	gl_Position =  mvp * vec4(aPosition, 1.0f);
	gDirection = mvp * vec4(normalize(direction) * scale, 0.0f);
    gOffsetX = mvp * vec4(scale / 2.0f, 0.0f, 0.0f, 0.0f);
    gOffsetY = mvp * vec4(0.0f, 0.0f, scale / 2.0f, 0.0f);
};

#shader geometry
#version 330 core

layout (points) in;
layout (line_strip, max_vertices = 8) out;

flat in vec4 gDirection[];
flat in vec4 gOffsetX[];
flat in vec4 gOffsetY[];

void main() {    

    for(int i = 0; i < 2; i++)
    {
       float sign = 2 * i - 1;

       gl_Position = gl_in[0].gl_Position + sign * gOffsetX[0] + sign * gDirection[0];
       EmitVertex();

       gl_Position = gl_in[0].gl_Position + sign * gOffsetX[0] - sign * gDirection[0];
       EmitVertex();
	
	   EndPrimitive();
    }

    for(int i = 0; i < 2; i++)
    {
       float sign = 2 * i - 1;

       gl_Position = gl_in[0].gl_Position + sign * gOffsetY[0] + sign * gDirection[0];
       EmitVertex();

       gl_Position = gl_in[0].gl_Position + sign * gOffsetY[0] - sign * gDirection[0];
       EmitVertex();
	
	   EndPrimitive();
    }

}  

#shader fragment
#version 330 core

out vec4 color;

uniform vec3 lightColor;

void main()
{
	color = vec4(10.0f * lightColor, 1.0f);
};