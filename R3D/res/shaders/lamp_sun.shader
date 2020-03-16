#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;


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
	gl_Position = projection * view * model * vec4(aPosition, 1.0f);
	gDirection = projection * view * model * vec4(normalize(direction) * scale, 0.0f);
    gOffsetX = projection * view * model * vec4(scale / 2.0f, 0.0f, 0.0f, 0.0f);
    gOffsetY = projection * view * model * vec4(0.0f, 0.0f, scale / 2.0f, 0.0f);
	//ourColor = aColor;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
//in vec4 gColor;

void main()
{
	color = 20.0f * vec4(1.0f, 1.0f, 1.0f, 1.0f);//* gColor;
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
