#include <iostream>
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION 1
#define STB_IMAGE_WRITE_IMPLEMENTATION 1
#include <stb_image.h>
#include <stb_image_write.h>

#include "gl_helper.h"
#include "utils.h"
#include "tiny_obj_loader.h"

using namespace mf;
using namespace std;
using namespace jhw_gl;


char g_mesh_path[128] = "mesh/head.obj";
char g_tex_path[128] = "res/head_tex.png";
GLenum image_type = GL_RGBA;
int image_channels = 4;

mf::GlobalCoeff mf::G(640, 480, 700.0, 700.0, 313.0, 256.0, 3.67);

float proj_matrix[16] = { 
	2.0*G.fx/G.w, 0., 0., 0.,
	0., 2.0*G.fy/G.h, 0., 0.,
	2*G.cx/G.w-1, 2*G.cy/G.h-1, 1., 1.,
	0.0f, 0.0f, 0.0f, 0.0f,
};
//get view by CameraPose
float *view_matrix;

const float imatrix[16] = {
	1.000000,0.000000,0.000000,0.000000,
	0.000000,1.000000,0.000000,0.000000,
	0.000000,0.000000,1.000000,0.000000,
	0.000000,-0.000000,4.000000,1.000000,
};


class ModelGLRenderer : public MyGLRenderer {
public:
	void setMeshPath(const char *path) {
		strcpy(m_mesh_path, path);
	}
	//must be triangle mesh
	virtual bool readData() {
		std::vector<tinyobj::shape_t> objects;
		std::vector<tinyobj::material_t> mat_list; //we ignore the information of materials now :)
		objects.clear();
		std::string err;
		bool ret = tinyobj::LoadObj(objects, mat_list, err, m_mesh_path);
		if (!err.empty()) {
			std::cerr << err << std::endl;
		}
		if (!ret) return false;

		//Unordered_map um;
		//um.clear();

		//cout << "objects: " << objects.size() << endl;
		//cout << "indices: " << objects[0].mesh.indices.size() << endl;
		//cout << "positions: " << objects[0].mesh.positions.size() << endl;
		//cout << "textures: " << objects[0].mesh.texcoords.size() << endl;
		
		m_vertice = 0;
		m_triangle = 0;
		for (int i = 0; i < objects.size(); ++i) {
			m_vertice += objects[i].mesh.positions.size() / 3;
			m_triangle += objects[i].mesh.indices.size() / 3;
		}

		if (vertices) { free(vertices); vertices = NULL; }
		vertices = (float*)malloc(sizeof(float) * m_vertice * 5);
		if (ebo) { free(ebo); ebo = NULL; }
		ebo = (u32*)malloc(sizeof(u32) * m_triangle * 3);
		
		int index = 0; // v
		for (int i = 0; i < objects.size(); ++i) {
			for (int j = 0; j < objects[i].mesh.positions.size(); ++j) {
				vertices[index++] = objects[i].mesh.positions[j];
			}
		}
		index = 0; // vt
		for (int i = 0; i < objects.size(); ++i) {
			for (int j = 0; j < objects[i].mesh.texcoords.size(); ++j) {
				vertices[m_vertice * 3 + index++] = objects[i].mesh.texcoords[j];
			}
		}
		index = 0; // f
		for (int i = 0; i < objects.size(); ++i) {
			for (int j = 0; j < objects[i].mesh.indices.size(); ++j) {
				ebo[index++] = objects[i].mesh.indices[j];
			}
		}
		return true;
	}

	virtual void *RenderScence() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glClearDepthf(1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		m_rtt.UseRTT();

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(m_vertice * 3 * sizeof(float)));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
		glDrawElements(GL_TRIANGLES, 3 * m_triangle, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 3 * m_triangle);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		// ½»»»Ç°ºó»º´æ
		//glutSwapBuffers();
		
		void *new_data = malloc(m_rtt_data_size * m_width * m_height * m_rtt_channel);
		m_rtt.GetColorData(new_data, m_rtt_type, m_rtt_data_type);
		return new_data;
	}

private:
	char m_mesh_path[128];
};


int main(int argc, char ** argv) {
	GLInit(G.w, G.h);

	//read texture
	int tex_w, tex_h, tex_n;
	unsigned char *texture = stbi_load(g_tex_path, &tex_w, &tex_h, &tex_n, 4);

	//renderer init
	ModelGLRenderer model_renderer;
	model_renderer.setMeshPath(g_mesh_path);
	model_renderer.init(G.w, G.h);
	model_renderer.setShaderFile("shader/test.vert", "shader/test.frag");
	model_renderer.CreateVertexBuffer();
	model_renderer.CreateIndexBuffer();
	GLTex2d model_tex;
	model_tex = model_renderer.CreateTexture(&model_tex.tex_id, tex_w, tex_h);

	//render 50 frames
	char path_tmp[128];
	for (int i = 0; i < 50; ++i) {
		model_renderer.useRenderer();
		mf::CameraPose pose;
		sprintf(path_tmp, "out/im%02d.png", i);

		//use AngleAxisd to set camera
		/*
		Eigen::Matrix3d R = Eigen::AngleAxisd(M_PI / 5 - i * 0.01, Eigen::Vector3d(0.0, 1.0, 0.0).normalized()).toRotationMatrix();
		cout << "R = " << endl;
		cout << R << endl;
		Eigen::Vector3d t = Eigen::Vector3d(double(i) / 20, 0.0, 5);
		pose = mf::CameraPose(G.Intrinsic, R, t);
		*/

		//use Euler to set camera
		pose = CameraPose(G.Intrinsic, 0.1, 0.78 + i*0.01, 0.1, 0.0, 0, 5);
		
		view_matrix = pose.getViewMatrix();
		model_renderer.setUniform4v("proj", proj_matrix);
		model_renderer.setUniform4v("view", view_matrix);
		model_renderer.setTexSub2D("tex", model_tex, 0, GL_TEXTURE0, texture);
		u32 *result = (u32*)model_renderer.RenderScence();
		stbi_write_png(path_tmp, G.w, G.h, image_channels, result, image_channels * sizeof(unsigned char)*G.w);
	}

	return 0;
}