#version 450


layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
out vec3 color;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 normal_matrix;

void main() {  
	vec3 light_direction = vec3(0.0, 0.0, 1.0);
	vec3  new_normal = normalize((normal_matrix * vec4(normal, 1.0)).xyz);
	float diffuse = max(dot(light_direction, new_normal), 0.0);
	color = vec3(0.9, 0.9, 0.9) * diffuse;
    gl_Position = proj * view * vec4(vertex, 1.0);  
} 


/*
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
out vec3 color;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 normal_matrix;

void main() {  
	vec3 light_direction = vec3(0.0, 0.0, 1.0);
	vec3  new_normal = normalize((normal_matrix * vec4(normal, 1.0)).xyz);
	float diffuse = max(dot(light_direction, new_normal), 0.0);
	color = vec3(0.9, 0.9, 0.9) * diffuse;
    gl_Position = proj * view * vec4(vertex * 100.0f, 1.0);  
} 
*/