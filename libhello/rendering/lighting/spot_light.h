#pragma once

#include "libhello/opengl/mesh_object.h"
#include "libhello/opengl/shader.h"
#include "libhello/geometry/sphere.h"
#include "libhello/geometry/plane.h"
#include "libhello/geometry/triangle_mesh_generator.h"


#include "libhello/rendering/lighting/point_light.h"

class SpotLightShader : public PointLightShader{
public:
    GLuint location_direction,location_angle;
    SpotLightShader(const string &multi_file) : PointLightShader(multi_file){}
    virtual void checkUniforms();
    void uploadDirection(vec3 &direction);
    void uploadAngle(float angle);
};



class SpotLight :  public PointLight
{
private:
    float angle=60.0f;
public:

    SpotLight();
    virtual ~SpotLight(){}
    void bindUniforms(SpotLightShader& shader);
    void bindUniformsStencil(MVPShader& shader) override;


    void setRadius(float value) override;

    void recalculateScale();
    void setAngle(float value);
    float getAngle() const{return angle;}
};
