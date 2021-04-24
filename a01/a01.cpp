#include "a01.hpp"
// define M_PI on MSVC
#define _USE_MATH_DEFINES
#include <math.h>

#include <glbinding/gl/functions.h>
#include <glbinding/gl/enum.h>
#include <glbinding/gl/bitfield.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader_loader.hpp"


void Assignment1::update(float dt)
{
    if (splatsRotate)
        rotationAngle += dt * degreesPerSecond;
}

void Assignment1::render()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // generate gbuffer
  fbo.bind();
  {
    glClearColor(0.2f, 0.2f, 0.2f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    renderScene();
  }
  fbo.unbind();

  // shade with primary light source
  {
    glClearColor(0.2f, 0.2f, 0.2f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader("quad"));

    // TODO A1 (a), setup gbuffer textures: see, glActiveTexture, Tex, uniform (glio/helper.h)

    glActiveTexture(GL_TEXTURE0);
    diffuse.bind();
    uniform("quad", "diffuse", 0);
    glActiveTexture(GL_TEXTURE1);
    normal.bind();
    uniform("quad", "normal", 1);
    glActiveTexture(GL_TEXTURE2);
    position.bind();
    uniform("quad", "position", 2);
    glActiveTexture(GL_TEXTURE3);
    depth.bind();
    uniform("quad", "depth", 3);
    
    //uniform("quad", "diffuse", diffuse.index());

    uniform("quad", "debug", debugGbuffer);

    uniform("quad", "lightDir", lightDir);
    uniform("quad", "viewMatrix", viewMatrix());

    quad.draw();
  }

  if (splatting && !debugGbuffer)
  {
    //splat unlit points
    drawSplats(shader("gbuffer"), 0.1f, false);

    // fill splat stencil buffer
    if (stencilCulling)
    {
      glUseProgram(shader("stencil"));
      glEnable(GL_STENCIL_TEST);
      glClear(GL_STENCIL_BUFFER_BIT);

      // TODO A1 (c), setup stencil buffer

      // disable writes to color and depth buffer
      glColorMask(false, false, false, false);
      glDepthMask(false);
      // if depth test fails, replace depth value with ref (=1)
      glStencilFunc(GL_ALWAYS, 1, 0xFF);
      glStencilOp(GL_KEEP, GL_REPLACE, GL_KEEP);
      // render back faces only
      glCullFace(GL_FRONT);
      drawSplats(shader("stencil"), splat_size, false); // render

      // TODO A1 (c), reset to opengl default values
      glColorMask(true,true,true,true);
      glDepthMask(true);
      glCullFace(GL_BACK);
      glStencilFunc(GL_ALWAYS, 0, 0xFF);


      // will be called later anyways
      //drawSplats(shader("splat"), splat_size, true); // with a shader that does the lighting


      //stencil debug
      if (debugStencil)
      {
        if (clearDebugStencil)
        {
          glClearColor(0.f, 0.f, 0.f, 1);
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        // TODO A1 (c), setup depth and stencil setting for masking
        // only render pixels with stencil == 1
        glStencilFunc(GL_EQUAL, 1, 0xFF);

        glUseProgram(shader("stencilDebug"));
        quad.draw();

        glStencilFunc(GL_ALWAYS, 0, 0xFF);

        glDepthFunc(GL_LESS);
      }
    }


    // actual splatting
    if (!stencilCulling || !debugStencil)
    {
      glDepthMask(GL_FALSE);
      glEnable(GL_CULL_FACE);
      glDepthFunc(GL_ALWAYS);
      // TODO A1 (b) & (c), take care of correct culling modes
      // only render pixels with stencil == 1
      glStencilFunc(GL_EQUAL, 1, 0xFF);

      if (stencilCulling)
      {
        glStencilFunc(GL_EQUAL, 1, ~0);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
      }

      glUseProgram(shader("splat"));
      uniform("splat", "resolution", resolution());

      uniform("splat", "diffuse", 0);
      uniform("splat", "normal", 1);
      uniform("splat", "position", 2);
      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE);

      drawSplats(shader("splat"), splat_size, true);

      glDisable(GL_BLEND);
      glDepthMask(GL_TRUE);
      glDisable(GL_CULL_FACE);

      glStencilFunc(GL_EQUAL, 0, 0xFF);
    }
  }

  glDisable(GL_STENCIL_TEST);
  glDepthFunc(GL_LESS);

  glUseProgram(0);
}

void Assignment1::renderScene() const
{
  glUseProgram(shader("gbuffer"));

  uniform("gbuffer", "viewMatrix", viewMatrix());
  uniform("gbuffer", "projMatrix", projectionMatrix());

  static const glm::fvec3 teapotColors[6]{
      glm::fvec3(0.8f, 0.5f, 0.5f),
      glm::fvec3(0.5f, 0.8f, 0.5f),
      glm::fvec3(0.5f, 0.5f, 0.8f),
      glm::fvec3(0.8f, 0.8f, 0.5f),
      glm::fvec3(0.5f, 0.8f, 0.8f),
      glm::fvec3(0.8f, 0.5f, 0.8f)};

  // render pots
  for (int i = 0; i < 10; ++i)
  {
    float rad = ((float)i / 10.f) * (float)M_PI * 2;
    float dist = 8.f;
    float x = glm::cos(rad) * dist;
    float y = glm::sin(rad) * dist;
    // alternating gray and colored teapots
    uniform("gbuffer", "color", i % 2 == 0 ? glm::fvec3(0.8f, 0.8f, 0.8f) : teapotColors[i / 2]);
    uniform("gbuffer", "modelMatrix", glm::translate(glm::fvec3(x, 0, y)) * glm::scale(glm::fmat4{1.0f}, glm::fvec3(2.5, 2.5, 2.5)));
    teaPot.draw();
  }

  // render ground plane
  uniform("gbuffer", "color", glm::fvec3(0.6f, 0.6f, 0.6f));
  uniform("gbuffer", "modelMatrix", glm::fmat4{1.0f});
  plane.draw();

  glUseProgram(0);
}

float Assignment1::splatDistFromOrigin() const
{
  return (glm::cos(rotationAngle * 0.7f) + 1.0f) * 4.f + 1.0f;
}

void Assignment1::drawSplats(const int program, const float radius, const bool asLight) const
{
  glUseProgram(program);
  uniform(program, "viewMatrix", viewMatrix());
  uniform(program, "projMatrix", projectionMatrix());

  static const glm::fvec3 splatColors[6] = {glm::fvec3(1, 0, 0),
                                            glm::fvec3(1, 1, 0),
                                            glm::fvec3(0, 1, 0),
                                            glm::fvec3(0, 1, 1),
                                            glm::fvec3(0, 0, 1),
                                            glm::fvec3(1, 0, 1)};

  for (int i = 0; i < 6; ++i)
  {
    float rad = ((float)i / 6.f) * (float)M_PI * 2 + rotationAngle;
    float dist = splatDistFromOrigin();
    float x = glm::cos(rad) * dist;
    float y = glm::sin(rad) * dist;
    if (asLight)
    {
      uniform(program, "radius", radius);
      uniform(program, "splatColor", splatColors[i]);
        uniform(program, "attenuation_params", light_attenuation);
        uniform(program, "light_intensity", light_intensity);
    }
    else
      uniform(program, "color", splatColors[i]);
    uniform(program, "modelMatrix", glm::translate(glm::fvec3(x, 2.8, y)) * glm::scale(glm::fmat4{1.0f}, glm::fvec3(radius, radius, radius)));
    sphere.draw();
  }

  glUseProgram(0);
}

Assignment1::Assignment1(std::string const &resource_path)
    : Application{resource_path},
      teaPot{m_resource_path + "/data/teapot.obj"},
      plane{0.f, 12.f},
      sphere{1, 20, 20},
      diffuse{resolution(), GL_RGBA32F},
      normal{resolution(), GL_RGBA32F},
      position{resolution(), GL_RGBA32F},
      depth{resolution(), GL_DEPTH_COMPONENT32}
{
  initializeObjects();
  initializeShaderPrograms();
}

// load shader programs
void Assignment1::initializeShaderPrograms()
{
  initializeShader("quad", {{GL_VERTEX_SHADER, m_resource_path + "/shader/quad.vs.glsl"}, {GL_FRAGMENT_SHADER, m_resource_path + "/shader/quad.fs.glsl"}});
  initializeShader("stencil", {{GL_VERTEX_SHADER, m_resource_path + "/shader/stencil.vs.glsl"}, {GL_FRAGMENT_SHADER, m_resource_path + "/shader/stencil.fs.glsl"}});
  initializeShader("stencilDebug", {{GL_VERTEX_SHADER, m_resource_path + "/shader/stencilDebug.vs.glsl"}, {GL_FRAGMENT_SHADER, m_resource_path + "/shader/stencilDebug.fs.glsl"}});
  initializeShader("splat", {{GL_VERTEX_SHADER, m_resource_path + "/shader/splat.vs.glsl"}, {GL_FRAGMENT_SHADER, m_resource_path + "/shader/splat.fs.glsl"}});
  initializeShader("gbuffer", {{GL_VERTEX_SHADER, m_resource_path + "/shader/gbuffer.vs.glsl"}, {GL_FRAGMENT_SHADER, m_resource_path + "/shader/gbuffer.fs.glsl"}});
}

void Assignment1::initializeObjects()
{
  fbo.bind();
  fbo.addTextureAsColorbuffer(diffuse);
  fbo.addTextureAsColorbuffer(normal);
  fbo.addTextureAsColorbuffer(position);
  fbo.addTextureAsDepthbuffer(depth);
  fbo.check();
  fbo.unbind();
}

void Assignment1::resize()
{
  fbo = Fbo{};
  diffuse = Tex{resolution(), GL_RGBA32F};
  normal = Tex{resolution(), GL_RGBA32F};
  position = Tex{resolution(), GL_RGBA32F};
  depth = Tex{resolution(), GL_DEPTH_COMPONENT32};
  initializeObjects();
}

void Assignment1::imgui()
{
    ImGui::SetNextWindowSize(ImVec2(200, 600), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    if (ImGui::Begin("Assignment 1"))
    {
        imGui_plotFPS();
        ImGui::Direction("lightDir", lightDir);
        ImGui::Separator();
        ImGui::Checkbox("debugGbuffer", &debugGbuffer);
        if( !debugGbuffer ){
            ImGui::Checkbox("splatting", &splatting);
            if( splatting ){
                ImGui::Checkbox("splatsRotate", &splatsRotate);
                ImGui::SliderFloat("splat_size", &splat_size, 1, 20);

                ImGui::InputFloat3("light_attenuation",&light_attenuation[0]);
                ImGui::SliderFloat("light_intensity", &light_intensity, 0, 4);

                ImGui::Checkbox("stencilCulling", &stencilCulling);
                if( stencilCulling ){
                    ImGui::Checkbox("debugStencil", &debugStencil);
                    if( debugStencil ){
                        ImGui::Checkbox("clearDebugStencil", &clearDebugStencil);
                    }
                }
            }
        }
    }
    ImGui::End();
}

// exe entry point
int main(int argc, char *argv[])
{
    std::string resource_path = read_resource_path(argc, argv);
    Assignment1 application{resource_path};
    application.run( 60 );  // max fps
}
