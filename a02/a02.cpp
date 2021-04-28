#include "a02.hpp"

// define M_PI on MSVC
#define _USE_MATH_DEFINES
#include <glbinding/gl/bitfield.h>
#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>
#include <glm/gtx/transform.hpp>
#include <math.h>

#include "shader_loader.hpp"
#include <glm/gtc/matrix_transform.hpp>

void Assignment2::render()
{
    // base image -> tex
    {
        fbo[0].bind();
        glClearColor(0.2f, 0.2f, 0.2f, 1);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        renderScene(shader("phong"), true);
        fbo[0].unbind();
    }

    // render base -> screen
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader("base"));

        glActiveTexture(GL_TEXTURE0);
        color[0].bind();
        uniform("base", "color", 0);

        glActiveTexture(GL_TEXTURE1);
        opaque_depth.bind();
        uniform("base", "depth", 1);

        quad.draw();

        glUseProgram(0);
    }

    if (usePeeling || debugLastLayer)
    {
        // initialize min-depth buffer to 0, allow all fragments to pass
        fbo[2].bind();
        glClearDepth(0.0);
        glClear(GL_DEPTH_BUFFER_BIT);
        fbo[2].unbind();

        glEnable(GL_POLYGON_OFFSET_FILL);
        int poly_offset = 0;

        for (unsigned i = 1; i <= layers; ++i)
        {
            // TODO A04 (a), copy correct depth buffer to current fbo's depth.
            // Note the function    Assignment2::copyDepthBuffer
            copyDepthBuffer(fbo[i], opaque_depth);

            glPolygonOffset((float)poly_offset, (float)poly_offset);

            // TODO A04 (a), peel layer i

            glUseProgram(shader("peel"));

            glActiveTexture(GL_TEXTURE0);
            depth[(i-1)%2].bind();
            uniform("peel", "minDepth", 0);

            fbo[i].bind();
            {
                glClearColor(0.2f, 0.2f, 0.2f, 0.f);
                //glClearDepth(1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                //glEnable(GL_DEPTH_TEST);
                renderScene(shader("peel"), false);
            }
            fbo[i].unbind();

            poly_offset--;
        }

        glDisable(GL_POLYGON_OFFSET_FILL);
        glClearDepth(1.0);

        if (debugLastLayer)
        {
            // TODO A04 (b), display a single layer (debug mode)

            glClearColor(51.f/255.f, 51.f/255.f, 153.f/255.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glUseProgram(shader("base"));

            //glEnable(GL_BLEND);
            //glDepthMask(GL_FALSE);
            //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            //glDepthFunc(GL_ALWAYS);

//glDepthFunc(GL_ALWAYS);

            glActiveTexture(GL_TEXTURE0);
            color[layers].bind();
            uniform("base", "color", 0);

            glActiveTexture(GL_TEXTURE1);
            depth[layers % 2].bind();
            uniform("base", "depth", 1);

            quad.draw();
            //glDisable(GL_BLEND);
            //glDepthMask(GL_TRUE);
            
//glDepthFunc(GL_LESS);

            glUseProgram(0);


        }
        else
        {
            // TODO A04 (c), blending

            glUseProgram(shader("blend"));

            glEnable(GL_BLEND);
            //glDepthMask(GL_FALSE);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthFunc(GL_ALWAYS);
            
            for (unsigned i = layers; i > 0; --i)
            {
                glActiveTexture(GL_TEXTURE0);
                color[i].bind();
                uniform("blend", "color", 0);

                quad.draw();
            }

            glDisable(GL_BLEND);
            glDepthFunc(GL_LESS);
            //glDepthMask(GL_TRUE);
            
            glUseProgram(0);
        }
    }
    else
    {
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        renderScene(shader("phong"), false);
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
    }
}

void Assignment2::renderScene(const int program, bool opaque) const
{
    glUseProgram(program);

    uniform(program, "viewMatrix", viewMatrix());
    uniform(program, "projMatrix", projectionMatrix());
    uniform(program, "lightDir", lightDir);

    glm::fvec3 colors[5] = {glm::fvec3(1, 1, 0), glm::fvec3(0.91, 0.54, 0), glm::fvec3(1, 0, 0),
                            glm::fvec3(0.4, 0, 0.91), glm::fvec3(0, 0.65, 1)};

    // render pots
    for (int i = 0; i < 10; ++i)
    {
        // opaque-pass rendering only opaque-pots
        // or not-opaque-pass rendering only not-opaque-pots
        if (opaque == (alpha[i] >= 1.0f))
        {
            float rad  = ((float)i / 10.f) * (float)M_PI * 2.0f;
            float dist = 8.f;
            float x    = glm::cos(rad) * dist;
            float y    = glm::sin(rad) * dist;

            uniform(program, "color", glm::vec4(colors[i % 5], alpha[i]));
            uniform(program, "modelMatrix",
                    glm::translate(glm::fvec3(x, 0, y)) * glm::scale(glm::fmat4{1.0f}, glm::fvec3(2.5, 2.5, 2.5)));
            teaPot.draw();
        }
    }

    // render ground plane
    if (opaque)
    {
        uniform(program, "color", glm::vec4(0.8f, 0.8f, 0.8f, 1.f));
        uniform(program, "modelMatrix", glm::fmat4{1.0f});
        plane.draw();
    }

    glUseProgram(0);
}

void Assignment2::copyDepthBuffer(Fbo const& target, Tex const& src)
{
    target.bind();
    glColorMask(false, false, false, false);
    glDepthFunc(GL_ALWAYS);
    glUseProgram(shader("copy"));

    glActiveTexture(GL_TEXTURE0);
    src.bind();
    uniform("copy", "depth", 0);
    quad.draw();

    glColorMask(true, true, true, true);
    glDepthFunc(GL_LESS);
    target.unbind();
    glUseProgram(0);
}

void GLAPIENTRY MessageCallback( GLenum source,
                    GLenum type,
                    GLuint id,
                    GLenum severity,
                    GLsizei length,
                    const GLchar* message,
                    const void* userParam )
{
    if (type == 0x8251) return;
    fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

Assignment2::Assignment2(std::string const& resource_path)
    : Application{resource_path},
      teaPot{m_resource_path + "/data/teapot.obj"},
      plane{0.f, 12.f},
      depth{Tex(resolution(), GL_DEPTH_COMPONENT32), Tex(resolution(), GL_DEPTH_COMPONENT32)},
      opaque_depth{Tex(resolution(), GL_DEPTH_COMPONENT32)},
      lightDir{glm::normalize(glm::fvec3(0.f, -1.f, -1.f))},
      alpha{0.33f, 0.7f, 1, 0.3f, 0.8f, 1, 1, 0.2f, 1, 0.7f}
{
    initializeObjects();
    initializeShaderPrograms();

    // During init, enable debug output
    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );
}

// load shader programs
void Assignment2::initializeShaderPrograms()
{
    initializeShader("phong", {{GL_VERTEX_SHADER, m_resource_path + "/shader/phong.vs.glsl"},
                               {GL_FRAGMENT_SHADER, m_resource_path + "/shader/phong.fs.glsl"}});
    initializeShader("base", {{GL_VERTEX_SHADER, m_resource_path + "/shader/quad.vs.glsl"},
                              {GL_FRAGMENT_SHADER, m_resource_path + "/shader/base.fs.glsl"}});
    initializeShader("base_test", {{GL_VERTEX_SHADER, m_resource_path + "/shader/quad.vs.glsl"},
                              {GL_FRAGMENT_SHADER, m_resource_path + "/shader/base_test.fs.glsl"}});
    initializeShader("blend", {{GL_VERTEX_SHADER, m_resource_path + "/shader/quad.vs.glsl"},
                               {GL_FRAGMENT_SHADER, m_resource_path + "/shader/blend.fs.glsl"}});
    initializeShader("copy", {{GL_VERTEX_SHADER, m_resource_path + "/shader/quad.vs.glsl"},
                              {GL_FRAGMENT_SHADER, m_resource_path + "/shader/copy.fs.glsl"}});
    initializeShader("peel", {{GL_VERTEX_SHADER, m_resource_path + "/shader/peel.vs.glsl"},
                              {GL_FRAGMENT_SHADER, m_resource_path + "/shader/peel.fs.glsl"}});
}

void Assignment2::initializeObjects()
{
    // fbos
    for (size_t i = 0; i < maxLayers; ++i)
    {
        // color texture
        color.emplace_back(std::move(Tex{resolution(), GL_RGBA32F}));
        // fbo, add textures
        fbo.emplace_back();
        fbo[i].bind();
        fbo[i].addTextureAsColorbuffer(color[i]);
        fbo[i].addTextureAsDepthbuffer(i == 0 ? opaque_depth : depth[i % 2]);
        fbo[i].unbind();
    }
}

void Assignment2::resize()
{
    fbo.clear();
    color.clear();
    depth        = {Tex(resolution(), GL_DEPTH_COMPONENT32), Tex(resolution(), GL_DEPTH_COMPONENT32)};
    opaque_depth = Tex(resolution(), GL_DEPTH_COMPONENT32);
    initializeObjects();
}

void Assignment2::imgui()
{
    ImGui::SetNextWindowSize(ImVec2(200, 600), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    if (ImGui::Begin("Assignment 2"))
    {
        imGui_plotFPS();
        if (ImGui::CollapsingHeader("light"))
        {
            ImGui::Direction("lightDir", lightDir);
        }
        if (ImGui::CollapsingHeader("teapots"))
        {
            ImGui::SliderFloat("alpha[0]", &alpha[0], 0, 1);
            ImGui::SliderFloat("alpha[1]", &alpha[1], 0, 1);
            ImGui::SliderFloat("alpha[2]", &alpha[2], 0, 1);
            ImGui::SliderFloat("alpha[3]", &alpha[3], 0, 1);
            ImGui::SliderFloat("alpha[4]", &alpha[4], 0, 1);
            ImGui::SliderFloat("alpha[5]", &alpha[5], 0, 1);
            ImGui::SliderFloat("alpha[6]", &alpha[6], 0, 1);
            ImGui::SliderFloat("alpha[7]", &alpha[7], 0, 1);
            ImGui::SliderFloat("alpha[8]", &alpha[8], 0, 1);
            ImGui::SliderFloat("alpha[9]", &alpha[9], 0, 1);
        }
        ImGui::Checkbox("usePeeling", &usePeeling);
        if (usePeeling)
        {
            ImGui::SliderInt("layers", (int*)&layers, 0, maxLayers - 1);
            ImGui::Checkbox("debugLastLayer", &debugLastLayer);
        }
    }
    ImGui::End();
}

// exe entry point
int main(int argc, char* argv[])
{
    std::string resource_path = read_resource_path(argc, argv);
    Assignment2 application{resource_path};
    application.run(60);  // max fps
}
