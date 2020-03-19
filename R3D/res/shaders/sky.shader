#shader vertex
#version 330

layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 sun_pos;

out vec3 fragPos;
out vec3 sun_norm;

void main()
{
	gl_Position = projection *  view * model * vec4(aPos, 1.0);
    fragPos = normalize(aPos);
	sun_norm = normalize(sun_pos);
}
#shader fragment
#version 330

in vec3 sun_norm;
in vec3 fragPos;

out vec4 color;

vec3 get_sky(vec2 uv, float sun_theta)
{
    float atmosphere = pow(uv.y,8);
    vec3 skyColor = vec3(0.2,0.4,0.8);
    
    float scatter = pow(sun_theta,1.0 / 15.0);
    scatter = 1.0 - clamp(scatter,0.8,1.0);
    
    vec3 scatterColor = mix(vec3(1.0),vec3(7.0,0.0,0.0),scatter);
    return mix(skyColor,vec3(scatterColor),atmosphere / 1.3);
    
}

vec3 get_sun(vec2 uv, vec2 uv_sun, vec3 sun_default_color){
	float sun = 1.0 - distance(uv,uv_sun);
    sun = clamp(sun,0.0,1.0);
    
    float glow = sun;
    glow = clamp(glow,0.0,1.0);
    
    sun = pow(sun,100.0);
    sun *= 100.0;
    sun = clamp(sun,0.0,1.0);
    
    glow = pow(glow,6.0) * 1.0;
    glow = pow(glow,(uv.y));
    glow = clamp(glow,0.0,1.0);
    
    sun *= pow(dot(uv.y, uv.y), 1.0 / 1.65);
    
    glow *= pow(dot(uv.y, uv.y), 1.0 / 2.0);
    
    sun += glow;
    
    vec3 sunColor = sun_default_color * sun;
    return vec3(sunColor);
}

vec2 to_uv(vec3 vec_in)
{
	float u = 2*(0.5 + atan(vec_in.z, vec_in.x) / 3.14159265);
	float v = -2*(-0.5 + asin(vec_in.y) / 3.14159265);
    return vec2(u,v);
}

void main() {    
	
    // Compute u and v
    vec2 uv = to_uv(fragPos);
    vec2 uv_sun = to_uv(sun_norm);

    // Sky
    vec3 skyColor = get_sky(uv, asin(sun_norm.y));
    color = (1.0 + 1.5 * asin(sun_norm.y)) * vec4(skyColor, 1.0);
    
    // Sun
    vec3 sunColorBase =  mix(vec3(1.0,0.6,0.05), vec3(4.0,0.0,0.0), pow(1-asin(sun_norm.y), 2)) ;
    vec3 sunColor = get_sun(uv, uv_sun, sunColorBase);
    color += vec4(sunColor, 1.0);
    
	// Expose sun
	float radius = length(fragPos - sun_norm);
	if (radius < 0.05)
    {
		radius = radius / 0.05;
		if (radius < 1.0 - 0.001)
        {
            color *= 10;
		}	
	}		

}
