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

layout(location = 0) out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float exposure;
uniform float gamma;

void main()
{             
	vec3 hdrColor = texture(screenTexture, TexCoords).rgb;
  
    // Exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

    // Gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    FragColor = vec4(mapped, 1.0);
}  