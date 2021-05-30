#pragma once
#include "application.hpp"
#include "helper.hpp"
#include "models.hpp"

#include <glm/glm.hpp>

class Assignment5 : public Application
{
   public:
    // allocate and initialize objects
    Assignment5(std::string const& resource_path);
    Assignment5(Assignment5 const&) = delete;
    Assignment5& operator=(Assignment5 const&) = delete;

    // draw all objects
    void render() override;
    void imgui() override;
    void update(float dt) override;

   private:
    // common methods
    void initializeShaderPrograms();

    // special methods
    void initShadowMap();
    void renderShadowMap(glm::vec3 const& light_pos);
    glm::fmat4 getShadowVP(glm::vec3 const& light_pos);

    Tex light_texture;

    // render objects
    simpleModel bunny;
    groundPlane plane;
    glm::fvec3 lightDir;
    float lightDistance = 18.0f;

    // shadow map
    glm::fmat4 shadowProj;
    const glm::ivec2 shadow_map_size = glm::ivec2(1024, 1024);
    unsigned shadow_map_id           = 0;
    unsigned fbo_id                  = 0;


    bool bunny_rotate    = false;
    float rotation_angle = 0;
    float rotation_speed = 1;
    int pcf_kernel_size  = 1;
    // TODO:: add custom variables, to make solution configurable

};
