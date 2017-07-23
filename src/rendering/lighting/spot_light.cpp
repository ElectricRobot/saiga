/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#include "saiga/rendering/lighting/spot_light.h"

namespace Saiga {

void SpotLightShader::checkUniforms(){
    AttenuatedLightShader::checkUniforms();
    location_angle = getUniformLocation("angle");
    location_shadowPlanes = getUniformLocation("shadowPlanes");
}


void SpotLightShader::uploadAngle(float angle){
    Shader::upload(location_angle,angle);
}

void SpotLightShader::uploadShadowPlanes(float f , float n){
    Shader::upload(location_shadowPlanes,vec2(f,n));
}

SpotLight::SpotLight(){

}


void SpotLight::calculateCamera(){
    vec3 dir = vec3(this->getUpVector());
    vec3 pos = vec3(getPosition());
    vec3 up = vec3(getRightVector());
    cam.setView(pos,pos-dir,up);
    cam.setProj(2*angle,1,shadowNearPlane,cutoffRadius);

}

void SpotLight::bindUniforms(std::shared_ptr<SpotLightShader> shader, Camera *cam){
    AttenuatedLight::bindUniforms(shader,cam);
    float cosa = glm::cos(glm::radians(angle*0.95f)); //make border smoother
    shader->uploadAngle(cosa);
    shader->uploadShadowPlanes(this->cam.zFar,this->cam.zNear);
    shader->uploadInvProj(glm::inverse(cam->proj));
    if(this->hasShadows()){
        shader->uploadDepthBiasMV(viewToLightTransform(*cam,this->cam));
        shader->uploadDepthTexture(shadowmap.getDepthTexture(0));
        shader->uploadShadowMapSize(shadowmap.getSize());
    }
    assert_no_glerror();
}


void SpotLight::recalculateScale(){
    float l = glm::tan(glm::radians(angle))*cutoffRadius;
    vec3 scale(l,cutoffRadius,l);
    this->setScale(scale);
}

void SpotLight::setRadius(float value)
{
    cutoffRadius = value;
    recalculateScale();
}

void SpotLight::createShadowMap(int resX, int resY) {
    //    Light::createShadowMap(resX,resY);
    //    float farplane = 50.0f;
    shadowmap.createFlat(resX,resY);
}

void SpotLight::setAngle(float value){
    this->angle = value;
    recalculateScale();
}

void SpotLight::setDirection(vec3 dir)
{
    rot = glm::rotation(glm::normalize(dir),vec3(0,-1,0));
}

bool SpotLight::cullLight(Camera *cam)
{
    //do an exact frustum-frustum intersection if this light casts shadows, else do only a quick check.
    if(this->hasShadows())
        this->culled = !this->cam.intersectSAT(cam);
    else
        this->culled = cam->sphereInFrustum(this->cam.boundingSphere)==Camera::OUTSIDE;

    return culled;
}

}
