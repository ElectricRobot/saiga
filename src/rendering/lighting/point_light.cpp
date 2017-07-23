/**
 * Copyright (c) 2017 Darius Rückert 
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#include "saiga/rendering/lighting/point_light.h"
#include "saiga/util/error.h"
#include "saiga/util/assert.h"

namespace Saiga {

void PointLightShader::checkUniforms(){
    AttenuatedLightShader::checkUniforms();
    location_shadowPlanes = getUniformLocation("shadowPlanes");
}



void PointLightShader::uploadShadowPlanes(float f , float n){
    Shader::upload(location_shadowPlanes,vec2(f,n));
}


PointLight::PointLight()
{


}


PointLight& PointLight::operator=(const PointLight& light){
    model = light.model;
    colorDiffuse = light.colorDiffuse;
    colorSpecular = light.colorSpecular;
    attenuation = light.attenuation;
    cutoffRadius = light.cutoffRadius;
    return *this;
}


float PointLight::getRadius() const
{
    return cutoffRadius;
}


void PointLight::setRadius(float value)
{
    cutoffRadius = value;
    this->setScale(vec3(cutoffRadius));
}

void PointLight::bindUniforms(std::shared_ptr<PointLightShader> shader, Camera *cam){
    AttenuatedLight::bindUniforms(shader,cam);
    shader->uploadShadowPlanes(this->cam.zFar,this->cam.zNear);
    shader->uploadInvProj(glm::inverse(cam->proj));
    if(this->hasShadows()){
        shader->uploadDepthBiasMV(viewToLightTransform(*cam,this->cam));
        shader->uploadDepthTexture(shadowmap.getDepthTexture(0));
        shader->uploadShadowMapSize(shadowmap.getSize());
    }
    assert_no_glerror();
}




void PointLight::createShadowMap(int resX, int resY) {
    shadowmap.createCube(resX,resY);
}





struct CameraDirection
{
    GLenum CubemapFace;
    vec3 Target;
    vec3 Up;
};

static const CameraDirection gCameraDirections[] =
{
    { GL_TEXTURE_CUBE_MAP_POSITIVE_X, vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f) },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_X, vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f) },
    { GL_TEXTURE_CUBE_MAP_POSITIVE_Y, vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f) },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f) },
    { GL_TEXTURE_CUBE_MAP_POSITIVE_Z, vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f) },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f) }
};


void PointLight::bindFace(int face){
    shadowmap.bindCubeFace(gCameraDirections[face].CubemapFace);
}

void PointLight::calculateCamera(int face){
    vec3 pos(this->getPosition());
    vec3 dir(gCameraDirections[face].Target);
    vec3 up(gCameraDirections[face].Up);
    cam.setView(pos,pos+dir,up);
    cam.setProj(90.0f,1,shadowNearPlane,cutoffRadius);
}

bool PointLight::cullLight(Camera *cam)
{
    Sphere s(getPosition(),cutoffRadius);
    this->culled = cam->sphereInFrustum(s)==Camera::OUTSIDE;
//    this->culled = false;
//    cout<<culled<<endl;
    return culled;
}

}
