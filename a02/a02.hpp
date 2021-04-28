#pragma once
#include "application.hpp"
#include "helper.hpp"
#include "models.hpp"

#include <array>
#include <glm/glm.hpp>

class Assignment2 : public Application
{
   public:
    // allocate and initialize objects
    Assignment2(std::string const& resource_path);
    Assignment2(Assignment2 const&) = delete;
    Assignment2& operator=(Assignment2 const&) = delete;

    // draw all objects
    void render() override;
    void imgui() override;
    void update(float dt) override{};

   protected:
    // common methods
    void initializeShaderPrograms();
    void initializeObjects();
    void resize() override;

    // special methods
    void renderScene(const int program, bool opaque) const;
    void copyDepthBuffer(Fbo const& target, Tex const& src);

    // render objects
    simpleQuad quad;
    simpleModel teaPot;
    groundPlane plane;

    // frame buffer object
    std::vector<Fbo> fbo;

    // textures
    std::vector<Tex> color;
    std::array<Tex, 2> depth;
    Tex opaque_depth;

    glm::fvec3 lightDir;
    std::array<float, 10> alpha;

    bool debugLastLayer      = false;
    bool usePeeling          = false;
    unsigned layers          = 0;
    const unsigned maxLayers = 10;
};
