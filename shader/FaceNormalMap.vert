#version 450

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
out vec3 color;
uniform mat4 view;
uniform mat4 proj;

void main() {  
	color = normal;
    gl_Position = proj * view * vec4(vertex, 1.0);  
} 