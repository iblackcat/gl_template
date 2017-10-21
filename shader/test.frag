#version 450

in vec2 st;

uniform sampler2D tex;
out vec4 FragColor;  

void main()  
{  
	FragColor = texture2D(tex, st);
	return;

    vec4 C = texture2D(tex, st);
	/*
	vec4 P = view_f * vec4(p_f, 1.0);

	//FragColor = vec4(C.a, C.a, C.a, 1.0);
	//if (C[3] < 0.5) {C[0] = 0.0; C[1] = 0.0; C[2] = 0.0;}
	//C = vec4(0.5,0.0,0.0,1.0);
	
	double depth = P.z;
	int test = int(depth * 256 * 256 * 256);
	float A = float(test % 256) / 255.0;
	float B = float((test / 256) % 256) / 255.0;
	float G = float((test / 256 / 256) % 256) / 255.0;
	float R = float(test / 256 / 256 / 256) / 255.0;

	FragColor = vec4(R, G, B, A);

	//FragColor = vec4(depth/8.0, depth/8.0, depth/8.0, 1.0);

	FragColor = C;

	//P.z = P.z * 10;
	//FragColor = vec4(P.z/255.0, P.z/255.0, P.z/255.0, 1.0);
	*/
}  