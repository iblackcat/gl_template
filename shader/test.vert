#version 450

layout(location = 0) in vec3 p;  
layout(location = 1) in vec2 texCoord;
uniform mat4 proj;
uniform mat4 view;

out vec2 st;
out vec4 color;
out mat4 view_f;
out vec3 p_f;

void main() {  
	//vec4 T0 = view*vec4(p[0], p[1], p[2], 1.0);
    vec4 T = proj*view*vec4(p[0], p[1], p[2], 1.0);
	gl_Position = vec4(T[0]/T[3], -T[1]/T[3], T[2]/1000.0, 1.0);
	//gl_Position[3] = 1.0;
	st = vec2(texCoord[0], 1.0-texCoord[1]);
	view_f = view;
	p_f = p;
} 