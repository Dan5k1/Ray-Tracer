#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "variables.h"
#include "math.h"

using namespace glm;

const float pie = 3.14159265;

class Ray {
public:
	vec3 dir;
	vec3 pos;
	Ray(vec3 position, vec3 direction) {
		this->dir = normalize(direction);
		this->pos = position;
	}
	vec3 getPoint(float t) {
		return pos + t * dir;
	}
};
class Intersection {

public:
	float distance;
	vec3 position;
	vec3 normal;
	Primitive* primitive;
	Intersection() {
		this->distance = std::numeric_limits<float>::infinity();
	}

	Intersection intersect(Ray ray, Primitive* obj) {
		Intersection hit = Intersection();
		hit.primitive = obj; // setting the intersection to point to the object
		//mat4 inverseTransform = obj->inverseT;
		ray.pos = vec3(obj->inverseT * vec4(ray.pos, 1.0f));
		ray.dir = vec3(obj->inverseT * vec4(ray.dir, 0.0f));
		//ray.dir = normalize(ray.dir); ///////////////////////////////////////////////////
		if (obj->type == 3) {
			// Triangle
			vec3 A = obj->v1;
			vec3 B = obj->v2;
			vec3 C = obj->v3;
			vec3 BC = C - B;
			vec3 CA = A - C;
			vec3 AC = C - A;
			vec3 AB = B - A;
			vec3 n = normalize(cross(AB, AC));
			
			float t = (dot(A, n) - dot(ray.pos, n)) / dot(ray.dir, n);
			if (t < 0) { // no intersection
				hit.distance = std::numeric_limits<float>::infinity();
				return hit;
			}
			
			/* Using algorithm from UCSD Online Discussion board by ravir11 */
			vec3 pointOfIntersection = ray.pos + t * ray.dir;
			vec3 ApNormal = (cross(n, BC)) / (dot(cross(n, BC), CA));
			vec3 BpNormal = (cross(n, CA)) / (dot(cross(n, CA), AB));
			vec3 CpNormal = (cross(n, AB)) / (dot(cross(n, AB), BC));
			float a = dot(ApNormal, pointOfIntersection) - dot(ApNormal, C);
			float b = dot(BpNormal, pointOfIntersection) - dot(BpNormal, A);
			float c = dot(CpNormal, pointOfIntersection) - dot(CpNormal, B);
			if (a >= 0 && b >= 0 && c >= 0) {
				
				hit.distance = t;
				hit.normal = n;
				hit.position = pointOfIntersection;
				hit.position = vec3(obj->transform * vec4(hit.position, 1.0f));
				vec3 normal = mat3(transpose(obj->inverseT)) * n;
				hit.normal = normalize(normal);
			}
			
		} else if (obj->type == 0) {
			// Sphere
			vec3 e = ray.pos;
			vec3 ctr = obj->center;
			vec3 d = ray.dir;
			//std::cout << glm::length(d) << "\n";///////////
			//std::cout <<"pos: " << ray.pos[0] << ray.pos[1] << ray.pos[2] << "\n";
			//std::cout << "dir: " << ray.dir[0] << ray.dir[1] << ray.dir[2] << "\n";
			
			float a = dot(d, d); // basically = 1
			float b = dot(2.0f * d, (e - ctr));
			float c = dot((e - ctr), (e - ctr)) - (obj->radius * obj->radius);
			if ((b * b) - (4.0f * a * c) < 0) { // no intersection
				//std::cout << hit.distance;
				return hit;
			}

			float dee = sqrt((b * b) - (4.0f * a * c));
			float root1 = (-b - dee) / (2.0f * a);
			float root2 = (-b + dee) / (2.0f * a);
			float t0 = root1;
			float t1 = root2;
			//std::cout << "t0: " << t0 << "\n" << "t1: " << t1 << "\n";
			if (root2 == std::min(root1, root2)) { // ensuring that t0 is smaller
				
				t0 = root2;
				t1 = root1;
			}
			if (t0 > 0 && t1 > 0) { // covers the t0 = t1 case
				
				hit.distance = t0;
				hit.position = ray.pos + t0 * ray.dir;
				//std::cout << hit.distance; 
			}
			else if (t0 < 0 && t1 > 0) { 
				
				hit.distance = t1;
				hit.position = ray.pos + t1 * ray.dir;
				
			}
			
			vec3 n = normalize(hit.position - hit.primitive->center);
			hit.position = vec3(obj->transform * vec4(hit.position, 1.0f));
			vec3 normal = mat3(transpose(obj->inverseT)) * n;
			hit.normal = normalize(normal);
		}
		return hit;
	}

	Intersection findIntersection(Ray ray, vector<Primitive*> scene) {
		float min_distance = std::numeric_limits<float>::infinity(); //start at infinity
		Intersection hit = Intersection();
		hit.distance = std::numeric_limits<float>::infinity();
		for (Primitive* obj : scene) {
			Intersection temp = intersect(ray, obj);
			if (temp.distance < min_distance) {
				min_distance = temp.distance;
				hit = temp;
			}
		}
		return hit;
	}
};

class Camera {
public:
	vec3 upvector;
	vec3 lookat;
	vec3 lookfrom;
	float fovy;
	Camera(vec3 upvector, vec3 lookat, vec3 lookfrom, float fovy) {
		this->upvector = upvector;
		this->lookat = lookat;
		this->lookfrom = lookfrom;
		this->fovy = fovy;
	}
	//This function shoots out a ray for each pixel.
	Ray* generateRay(float i, float j) {

		vec3 w = normalize(lookfrom - lookat); // c = normalize(eye - center)
		//vec3 b = normalize(upvector);
		vec3 u = normalize(cross(upvector, w));
		vec3 v = cross(w, u);

		float theta = (fovy * pie / 180.0) / 2; // in radians
		float alpha = ((float)width / (float)height) * tan(theta) * (j - (width / 2.0f)) / (width / 2.0f);
		float beta = tan(theta) * (i - (height / 2.0f)) / (height / 2.0f);
		vec3 direction = normalize(alpha * u + beta * v - w);

		Ray* ray = new Ray(lookfrom, direction);
		return ray;
	}
};
#endif
