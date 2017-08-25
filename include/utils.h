#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <sophus/so3.hpp>
#include <sophus/se3.hpp>

/*
// A macro to disallow the copy constructor and operator= functions
// This should be used in the priavte:declarations for a class
#define    DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&);                \
TypeName& operator=(const TypeName&)
*/

namespace mf {

	typedef unsigned int u32;
	typedef unsigned char u8;

	typedef Eigen::Matrix<double, 6, 1> Vector6d;

	class CameraPose {
	public:
		Eigen::Matrix3d intrinsics;
		Sophus::SE3d SE3_Rt;
		Eigen::Matrix3d R, Q;
		Eigen::Vector3d t, q;

		Eigen::Vector3d center;
		Vector6d se3;

		CameraPose(Eigen::Matrix3d A, Eigen::Matrix3d rot, Eigen::Vector3d tran) {
			intrinsics = A;
			R = rot;
			t = tran;
			Q = intrinsics * R;
			q = intrinsics * t;

			SE3_Rt = Sophus::SE3d(R, t);
			se3 = SE3_Rt.log();
			center = -(R.inverse() * t);
		}
		CameraPose() {}

		float* getViewMatrix() {
			float *v = (float*)malloc(sizeof(float) * 4 * 4);
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					v[j * 4 + i] = R(i, j);
				}
				v[3 * 4 + i] = t[i];
			}
			v[0 * 4 + 3] = 0.0; v[1 * 4 + 3] = 0.0; v[2 * 4 + 3] = 0.0; v[3 * 4 + 3] = 1.0;
			return v;
		}

		void refreshByARt() {
			Q = intrinsics * R;
			q = intrinsics * t;
			center = -(R.inverse() * t);
		}
	};

	template<typename T>
	T binterd(T* I_i, double vx, double vy, int size) {
		int z_x = int(vx + 0.00001);  //0~w
		int z_y = int(vy + 0.00001);  //0~h
		double x = vx - z_x;
		double y = vy - z_y;
		//if (z_x < 0 || z_x + 1 >= g_w || z_y < 0 || z_y + 1 >= g_h) return T(0);
		return T((1.0 - x)   * (1.0 - y)  * I_i[z_y	 *size + z_x] +
			x			 * (1.0 - y)  * I_i[z_y	 *size + z_x + 1] +
			x          *  y	      * I_i[(z_y + 1)*size + z_x + 1] +
			(1.0 - x)   *  y	      * I_i[(z_y + 1)*size + z_x]);
	}

	static u32 binterd_u32(u32 *I, double vx, double vy, int size) {
		int z_x = int(vx + 1e-5);
		int z_y = int(vy + 1e-5);
		double x = vx - z_x;
		double y = vy - z_y;

		int a = (1.0 - x)	* (1.0 - y) * (I[(z_y)* size + z_x] >> 24) +
			(x)			* (1.0 - y) * (I[(z_y)* size + z_x + 1] >> 24) +
			(x)			* (y)		* (I[(z_y + 1)* size + z_x + 1] >> 24) +
			(1.0 - x)	* (y)		* (I[(z_y + 1)* size + z_x] >> 24);

		int b = (1.0 - x)	* (1.0 - y) * (I[(z_y)* size + z_x] >> 16 & 0xff) +
			(x)			* (1.0 - y) * (I[(z_y)* size + z_x + 1] >> 16 & 0xff) +
			(x)			* (y)		* (I[(z_y + 1)* size + z_x + 1] >> 16 & 0xff) +
			(1.0 - x)	* (y)		* (I[(z_y + 1)* size + z_x] >> 16 & 0xff);

		int g = (1.0 - x)	* (1.0 - y) * (I[(z_y)* size + z_x] >> 8 & 0xff) +
			(x)			* (1.0 - y) * (I[(z_y)* size + z_x + 1] >> 8 & 0xff) +
			(x)			* (y)		* (I[(z_y + 1)* size + z_x + 1] >> 8 & 0xff) +
			(1.0 - x)	* (y)		* (I[(z_y + 1)* size + z_x] >> 8 & 0xff);

		int r = (1.0 - x)	* (1.0 - y) * (I[(z_y)* size + z_x] & 0xff) +
			(x)			* (1.0 - y) * (I[(z_y)* size + z_x + 1] & 0xff) +
			(x)			* (y)		* (I[(z_y + 1)* size + z_x + 1] & 0xff) +
			(1.0 - x)	* (y)		* (I[(z_y + 1)* size + z_x] & 0xff);

		return u32(a << 24 | b << 16 | g << 8 | r);
	}

	static float rgba2gray(u32 rgba) {
		float a = static_cast<float>(rgba >> 24);
		float b = static_cast<float>(rgba >> 16 & 0xff);
		float g = static_cast<float>(rgba >> 8 & 0xff);
		float r = static_cast<float>(rgba & 0xff);

		if (a == 0.0) return 0.0;
		else return r*0.299 + g*0.587 + b*0.114;
	}

} //namespace mf

#endif //UTILS_H
