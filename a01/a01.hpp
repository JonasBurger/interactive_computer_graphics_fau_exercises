#pragma once
#include "application.hpp"

#include <glm/glm.hpp>

#include "helper.hpp"
#include "models.hpp"

class Assignment1 : public Application {
 public:
  // allocate and initialize objects
  Assignment1(std::string const& resource_path);
  Assignment1(Assignment1 const&) = delete;
  Assignment1& operator=(Assignment1 const&) = delete;

  // draw all objects
  void render() override;
  void imgui() override;

  void update(float dt) override;
 protected:
  // common methods
  void initializeShaderPrograms();
  void initializeObjects();

  void renderScene() const;
  float splatDistFromOrigin() const;
  void drawSplats(const int program, const float radius, const bool asLight) const;
  void resize() override;
  // render objects
  simpleQuad  quad;
  simpleModel teaPot;
  groundPlane plane;
  solidSphere sphere;
  simplePoint point;

  float light_intensity = 1.0f;
  glm::vec3 light_attenuation = glm::vec3(0.5,1,2);

  // frame buffer object
  Fbo fbo;

  // textures
  Tex diffuse;
  Tex normal;
  Tex position;
  Tex depth;

  Timer timer;

  glm::fvec3 lightDir = glm::normalize(-glm::fvec3(0.f, 1.f, 1.f));
  float rotationAngle = 0;
  float degreesPerSecond = 1;
  float splat_size = 8;

  bool splatsRotate = true;
  bool debugGbuffer = false;
  bool stencilCulling = false;
  bool debugStencil = false;
  bool clearDebugStencil = false;
  bool splatting = false;
};
