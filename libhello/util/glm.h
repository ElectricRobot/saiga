#pragma once

//disable annoying visual studio warnings
#ifdef _MSC_VER
#pragma warning( disable : 4267 )
#endif

#include <string>
#include <iostream>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
//#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/random.hpp>

using std::ostream;

using glm::mat4;
using glm::vec3;
using glm::vec4;
using glm::vec2;
using glm::quat;
using std::cout;
using std::endl;
#define degreesToRadians(x) x*(3.141592f/180.0f)



std::ostream& operator<<(std::ostream& os, const vec4& v);
std::ostream& operator<<(std::ostream& os, const glm::dvec4& v);

std::ostream& operator<<(std::ostream& os, const vec3& v);
std::ostream& operator<<(std::ostream& os, const glm::dvec3& v);

std::ostream& operator<<(std::ostream& os, const vec2& v);

std::ostream& operator<<(std::ostream& os, const mat4& v);
std::ostream& operator<<(std::ostream& os, const glm::dmat4& v);

std::ostream& operator<<(std::ostream& os, const quat& v);

//returns quaternion that rotates v1 to v2
quat getRotation(const vec3& v1, const vec3& v2);


vec3 sampleCone(const vec3& dir, float angle);
//samples cone along the z axis
vec3 sampleUnitCone(float angle);

vec3 snapTo(vec3 v, float snapAngleInDegrees);
