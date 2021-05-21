#pragma once
#include "application.hpp"
#include "helper.hpp"
#include "models.hpp"

#include <glm/glm.hpp>

class Assignment4 : public Application
{
   public:
    // allocate and initialize objects
    Assignment4(std::string const& resource_path);
    Assignment4(Assignment4 const&) = delete;
    Assignment4& operator=(Assignment4 const&) = delete;

    // draw all objects
    void render() override;
    void imgui() override;
    void update(float dt) override{};

   private:
    // common methods
    void initializeShaderPrograms();
    void initializeObjects();

    // special methods
    void renderMap() const;
    void renderPot(GLuint program) const;

    Tex envMap;

    // render objects
    simpleModel teaPot;
    solidSphere sphere;

    bool debugUV = false;
    bool useCylindricMapping = false;

    int assignmentNr = 0;
    int envMapID     = 0;
    float teapotRotation = 0;
    int glossyRays   = 16;
    float roughness  = 0.0f;
};
