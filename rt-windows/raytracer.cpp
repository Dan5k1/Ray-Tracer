


#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Transform.h"
#include <FreeImage.h>
#include <conio.h>
#include <algorithm> 
#include <time.h> 
#include <chrono>
using namespace std; 

// Main variables in the program.  
#define MAINPROGRAM 
//#include "variables.h" 
#include "readfile.h" // prototypes for readfile.cpp  
#include "Camera.h"
#include "Sampler.cpp"

vec3 ComputeLight(vec3 direction, vec3 lightcolor, vec3 normal, vec3 halfvec, vec3 mydiffuse,vec3 myspecular,float myshininess, bool lgttype, float r) {

	float nDotL = dot(normal, direction);
	vec3 lambert = mydiffuse * lightcolor * std::max(nDotL, 0.0f);

	float nDotH = dot(normal, halfvec);
	vec3 phong = myspecular * lightcolor * pow(std::max(nDotH, 0.0f), myshininess);

	vec3 retval = lambert + phong;

	if (lgttype == 1) { // Calculate attenuation for point light
		retval = retval / (attenuation[0] + attenuation[1] * r + attenuation[2]*(r* r));
	}
	return retval;
}

vec3 findColor(Intersection hit, Ray* ray, int depth) { 
	// A couple predefined colors
	/*RGBQUAD red = {0, 0, 255, 0};
	RGBQUAD green = { 0.0f, 255.0f, 0.0f, 0.0f };
	RGBQUAD blue = { 255.0f, 0.0f, 0.0f, 0.0f };
	RGBQUAD black = { 0.0f, 0.0f, 0.0f, 0.0f };
	RGBQUAD white = { 255.0f, 255.0f, 255.0f, 0.0f };*/

	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0) {
		return { 0.0f, 0.0f, 0.0f };
	}

	// no intersection
	if (hit.distance == std::numeric_limits<float>::infinity()) { 
		return { 0.0f, 0.0f, 0.0f };
	}
	// intersection
	else { 
		// Lighting 
		vec3 normal = hit.normal;
		vec3 finalcolor;
		vec3 eyepos = ray->pos;
		vec3 mypos = hit.position;
		vec3 eyedirn = normalize(eyepos - mypos);
			
		finalcolor = hit.primitive->ambient + hit.primitive->emission;
		Intersection dummy = Intersection();
		// Add up all the lights
		for (int i = 0; i < lightpos.size(); i++) {

			float r = 1.0f;
			Ray ray = Ray(hit.position + 0.01f * normalize(lightpos[i] - hit.position), lightpos[i] - hit.position);
			bool Vi = false;
			Intersection ShadowIntersect = hit.findIntersection(ray, primitives);
			vec3 direction;
			// directional light
			if (lgtType[i] == 0) {
				if (ShadowIntersect.distance >= std::numeric_limits<float>::infinity()) {
					Vi = true;
				}
				direction = normalize(lightpos[i]);
			}
			//point light
			else {
				if (ShadowIntersect.distance >= distance(lightpos[i], ray.pos)) {
					Vi = true;
				}
				direction = normalize(lightpos[i] - mypos);
				r = distance(hit.position, lightpos[i]);
			}
			
			
			// directional light
			if (lgtType[i] == 0) { 
				
			}
			// point light
			else { 
				
			}
			//cout << lightcol[i][0] << "," << lightcol[i][1] << "," << lightcol[i][2] << "\n";
			vec3 halfAng = normalize(direction + eyedirn);
			
			//float Vi = 1;
			Ray vRay = Ray(hit.position + 0.001f * direction, direction);
			//Intersection dummy = Intersection();
			Intersection vInt = dummy.findIntersection(vRay, primitives);
			// only add if light is visible
			if (Vi == true) {
				vec3 curCol = ComputeLight(direction, lightcol[i], normal, halfAng, hit.primitive->diffuse, hit.primitive->specular, hit.primitive->shininess, lgtType[i], r);
				finalcolor = finalcolor + curCol;
			}
		}
		Ray* reflectedR = new Ray(hit.position + normal * 0.001f, normalize(2.0f*(glm::dot(normal, -normalize(ray->dir))*normal) + ray->dir));
		
		vec3 recursivecol = hit.primitive->specular * findColor(dummy.findIntersection(*reflectedR, primitives), reflectedR, depth - 1);
		finalcolor += recursivecol;
		return finalcolor;
		
	}
}


void init() {
	maxdepth = 5;
	attenuation = vec3(1.0, 0.0, 0.0); // only for point lights
	filename = "sc1.png";
	for (Primitive* obj : primitives) {
		obj->inverseT = inverse(obj->transform);
	}
}
void Clear() // clears the console
{
	system("cls");
}
void progressBar(float percentage, float remaining) {
	cout << "[";
	for (int g = 0; g < floor(percentage / 10); g++) {
		cout << "|";
	}
	for (int g = 0; g < floor(100 - percentage) / 10; g++) {
		cout << ".";
	}
	cout << "]";
	cout << percentage << "%\n" << "time remaining: " << remaining << " s \n";
}
int main(int argc, char* argv[]) {
  init();
  Intersection* intersect = new Intersection();
  readfile("scene4-ambient.test");
  FreeImage_Initialise();
  FIBITMAP* image = FreeImage_Allocate(width, height, 24);
	Camera* cam = new Camera(upinit, center, eyeinit, fovy);
	modelview = Transform::lookAt(eyeinit, center, upinit);
	int percentage = 0;
	int percentPrev = 0;
	auto t_start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < height; i++) {
		if (i % 10 == 0) { // updates progress bar once every 60 height pixels
			auto t_end = std::chrono::high_resolution_clock::now();
			percentage = ((float)i / height) * 100; // calculates percentage rendered based on height
			int percChange = percentage -percentPrev ;
			int percentDiff = 100 - percentage;
			Clear();
			double elapsed = std::chrono::duration<double, std::milli>(t_end - t_start).count();
			float remaining = (int)(elapsed * percentDiff/percChange)/1000;
			progressBar(percentage, remaining); 
			percentPrev = percentage;
			t_start = std::chrono::high_resolution_clock::now();

		}
		for (int j = 0; j < width; j++) {
			Ray* ray = cam->generateRay((float)(i + 0.5f), (float)(j + 0.5f));
			Intersection hit = intersect->findIntersection(*ray, primitives);
			vec3 finalcolor = findColor(hit, ray, maxdepth);
			RGBQUAD color = { finalcolor[2] * 255.0f, finalcolor[1] * 255.0f, finalcolor[0] * 255.0f, 0 };
			FreeImage_SetPixelColor(image, j, i, &color);
		}
	}
	FreeImage_Save(FIF_PNG, image, filename.c_str(), 0);
	FreeImage_DeInitialise();
	return 0;
}
