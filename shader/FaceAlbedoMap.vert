#version 450

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uv;
out vec3 color;
uniform mat4 view;
uniform mat4 proj;
uniform sampler2D tex;

void main() {  
	color = texture2D(tex, vec2(1.0f - uv.x, 1.0f - uv.y)).xyz;
	//color = texture2D(tex, uv).xyz;
    gl_Position = proj * view * vec4(vertex, 1.0);  
}