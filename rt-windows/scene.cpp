
/*****************************************************************************/
// This file is readfile.cpp.  It includes helper functions for matrix 
// transformations for a stack (matransform) and to rightmultiply the 
// top of a stack.  
  

/*****************************************************************************/

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Transform.h" 

using namespace std;
#include "variables.h" 
#include "readfile.h"
#include "primitive.h"


// The function below applies the appropriate transform to a 4-vector
void matransform(stack<mat4> &transfstack, GLfloat* values) 
{
    mat4 transform = transfstack.top(); 
    vec4 valvec = vec4(values[0],values[1],values[2],values[3]); 
    vec4 newval = transform * valvec; 
    for (int i = 0; i < 4; i++) values[i] = newval[i]; 
}

void rightmultiply(const mat4 & M, stack<mat4> &transfstack) 
{
    mat4 &T = transfstack.top(); 
    T = T * M; 
}

// Function to read the input data values 
bool readvals(stringstream &s, const int numvals, GLfloat* values) 
{
    for (int i = 0; i < numvals; i++) {
        s >> values[i]; 
        if (s.fail()) {
            cout << "Failed reading value " << i << " will skip\n"; 
            return false;
        }
    }
    return true; 
}

void readfile(const char* filename) 
{
    string str, cmd; 
    ifstream in;
    in.open(filename); 
    if (in.is_open()) {
        stack <mat4> transfstack; 
        transfstack.push(mat4(1.0));  // identity
        getline (in, str); 
        while (in) {
            if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) {
                stringstream s(str);
                s >> cmd; 
                int i; 
                GLfloat values[10]; // Position and color for light, colors for others
                                    // Up to 10 params for cameras.  
                bool validinput; // Validity of input 

               

                // Material Commands 
                // Ambient, diffuse, specular, shininess properties for each object.

                if (cmd == "ambient") {
                    validinput = readvals(s, 3, values);
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            ambient[i] = values[i]; 
                        }
                    }
                } else if (cmd == "diffuse") {
                    validinput = readvals(s, 3, values);
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            diffuse[i] = values[i]; 
                        }
                    }
                } else if (cmd == "specular") {
                    validinput = readvals(s, 3, values);
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            specular[i] = values[i];
                        }
                    }
                } else if (cmd == "emission") {
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            emission[i] = values[i]; 
                        }
                    }
                } else if (cmd == "shininess") {
                    validinput = readvals(s, 1, values); 
                    if (validinput) {
                        shininess = values[0]; 
                    }
                } else if (cmd == "directional") {
                    validinput = readvals(s, 6, values);
                    if (validinput) {
                        vec3 curLightPos(values[0], values[1], values[2]);
                        curLightPos = vec3(transfstack.top() * vec4(curLightPos, 0.0f)); // Need to transform light position before displaying
                        lightpos.push_back(curLightPos);
                        vec3 curLightColor(values[3], values[4], values[5]);
                        lightcol.push_back(curLightColor);
                        lgtType.push_back(0);
                    }
                }
                else if (cmd == "point") {
                    validinput = readvals(s, 6, values);
                    if (validinput) {
                        vec3 curLightPos(values[0], values[1], values[2]);
                        curLightPos = vec3(transfstack.top() * vec4(curLightPos, 1.0f)); // Need to transform light position before displaying
                        lightpos.push_back(curLightPos);
                        vec3 curLightColor(values[3], values[4], values[5]);
                        lightcol.push_back(curLightColor);
                        lgtType.push_back(1);
                    }

                }
                else if (cmd == "attenuation") {
                    validinput = readvals(s, 3, values);
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            attenuation[i] = values[i];
                        }
                    }
                }
                else if (cmd == "size") {
                    validinput = readvals(s, 2, values);
                    if (validinput) {
                        width = (int)values[0]; height = (int)values[1];
                    }
                }
                else if (cmd == "camera") {
                    validinput = readvals(s, 10, values); // 10 values eye cen up fov
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            eyeinit[i] = values[i];
                            center[i] = values[i + 3];
                            upinit[i] = values[i + 6];
                        }
                        fovy = values[9];
                        upinit = Transform::upvector(upinit, glm::normalize(eyeinit - center));
                    }
                }
                    else if (cmd == "size") {
                        validinput = readvals(s, 2, values);
                        if (validinput) {
                            width = values[0];
                            height = values[1];
                        }
                    }
                    else if (cmd == "maxdepth") {
                        validinput = readvals(s, 1, values);
                        if (validinput) {
                            maxdepth = values[0];
                        }
                    }
                    else if (cmd == "sphere") {
                        validinput = readvals(s, 4, values);
                        if (validinput) {
                            Primitive * sphere = new Sphere(values[0], values[1], values[2], values[3]);
                            sphere->transform = transfstack.top();
                            sphere->ambient = ambient;
                            sphere->diffuse = diffuse;
                            sphere->specular = specular;
                            sphere->shininess = shininess;
                            sphere->emission = emission;
                            sphere->inverseT = glm::inverse(transfstack.top());
                            primitives.push_back(sphere);
                        }
                    }
                    else if (cmd == "maxverts") {
                        validinput = readvals(s, 1, values);
                        if (validinput) {
                            maxverts = values[0];
                        }
                    }
                    else if (cmd == "maxvertnorms") {
                        validinput = readvals(s, 1, values);
                        if (validinput) {

                        }
                    }
                    else if (cmd == "vertex") {
                        validinput = readvals(s, 3, values);
                        if (validinput) {
                            vec3 vertex = vec3(values[0], values[1], values[2]);
                            vertices.push_back(vertex);
                        }
                    }
                    else if (cmd == "vertexnormal") {
                        validinput = readvals(s, 6, values);
                        if (validinput) {
                        }
                    }
                    else if (cmd == "tri") {
                        validinput = readvals(s, 3, values);
                        if (validinput) {
                            vec3 v1 = vertices[values[0]];
                            vec3 v2 = vertices[values[1]];
                            vec3 v3 = vertices[values[2]];
                            Primitive* triangle = new Triangle(v1, v2, v3);
                            triangle->transform = transfstack.top();
                            triangle->ambient = ambient;
                            triangle->diffuse = diffuse;
                            triangle->specular = specular;
                            triangle->shininess = shininess;
                            triangle->emission = emission;
                            triangle->inverseT = glm::inverse(transfstack.top());
                            primitives.push_back(triangle);
                        }
                    }
                    else if (cmd == "trinormal") {
                        validinput = readvals(s, 3, values);
                        if (validinput) {
                        }
                    }

                else if (cmd == "translate") {
                    validinput = readvals(s,3,values); 
                    if (validinput) {
                        mat4 trM = Transform::translate(values[0], values[1], values[2]);
                        rightmultiply(trM, transfstack);
                    }
                }
                else if (cmd == "scale") {
                    validinput = readvals(s,3,values); 
                    if (validinput) {

                        
                        mat4 sM = Transform::scale(values[0], values[1], values[2]);
                        rightmultiply(sM, transfstack);
                    }
                }
                else if (cmd == "rotate") {
                    validinput = readvals(s,4,values); 
                    if (validinput) {

                        glm::vec3 axis(values[0], values[1], values[2]);
                        glm::mat3 rMat3 = Transform::rotate(values[3], axis);
                        glm::mat4 rM(rMat3);
                        rightmultiply(rM, transfstack);
                    }
                }
                //Push/pop for matrix stacks
                else if (cmd == "pushTransform") {
                    transfstack.push(transfstack.top()); 
                } else if (cmd == "popTransform") {
                    if (transfstack.size() <= 1) {
                        cerr << "Stack has no elements.  Cannot Pop\n"; 
                    } else {
                        transfstack.pop(); 
                    }
                }

                else {
                    cerr << "Unknown Command: " << cmd << " Skipping \n"; 
                }
            }
            getline (in, str); 
        }

        // Set up initial position for eye, up and amount
        // As well as booleans 

        eye = eyeinit; 
        up = upinit; 
        amount = 5;
        sx = sy = 1.0;  // keyboard controlled scales in x and y 
        tx = ty = 0.0;  // keyboard controllled translation in x and y  
        useGlu = false; // don't use the glu perspective/lookat fns

        //glEnable(GL_DEPTH_TEST);
    } else {
        cerr << "Unable to Open Input Data File " << filename << "\n"; 
        throw 2; 
    }
}

void render() {
    //while (!sampler.generateSample(&sample) {

        //camera.generateRay(sample, Ray &ray);

        //raytracer.trace(Ray ray, &color);

        //film.commit(sample, color);

    //}
    //film.writeImage();
}
