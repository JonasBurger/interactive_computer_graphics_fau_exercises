#include "a04.hpp"

#include <glbinding/gl/bitfield.h>
#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>
#include <glm/gtx/transform.hpp>

#include "shader_loader.hpp"

void Assignment4::render()
{
    renderMap();
    switch (assignmentNr)
    {
        case 0:
            renderPot(shader("shaderA"));
            break;
        case 1:
            renderPot(shader("shaderB"));
            break;
        case 2:
            glUseProgram(shader("shaderC"));
            uniform("shaderC", "glossyRays", glossyRays);
            uniform("shaderC", "roughness", roughness);
            renderPot(shader("shaderC"));
            break;
    }
}

void Assignment4::renderMap() const
{
    glClearColor(0.2f, 0.2f, 0.2f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader("map"));

    uniform("map", "envMap", 0);
    uniform("map", "debugUV", debugUV);

    uniform("map", "viewMatrix", viewMatrix());
    uniform("map", "projMatrix", projectionMatrix());
    uniform("map", "modelMatrix", glm::mat4(1));

    sphere.draw();

    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(0);
}

void Assignment4::renderPot(GLuint program) const
{
    glUseProgram(program);

    uniform(program, "envMap", 0);
    uniform(program, "cylindricMapping", useCylindricMapping);
    uniform(program, "debugUV", debugUV);

    uniform(program, "viewMatrix", viewMatrix());
    uniform(program, "projMatrix", projectionMatrix());
    uniform(program, "modelMatrix",
            glm::translate(glm::fvec3(0, -0.33, 0)) * glm::rotate(teapotRotation, glm::fvec3(0, 1, 0)));

    teaPot.draw();

    glUseProgram(0);
}

Assignment4::Assignment4(std::string const& resource_path)
    : Application{resource_path},
      envMap{m_resource_path + "/data/waterfall.png"},
      teaPot{m_resource_path + "/data/teapot.obj"},
      sphere{60, 1000, 1000}
{
    m_cam = cameraSystem{glm::fvec3(1.5f, 1.5f, 1.5f)};
    updateCamera();

    initializeObjects();
    initializeShaderPrograms();
}


// load shader programs
void Assignment4::initializeShaderPrograms()
{
    initializeShader("map", {{GL_VERTEX_SHADER, m_resource_path + "/shader/map.vs.glsl"},
                             {GL_FRAGMENT_SHADER, m_resource_path + "/shader/map.fs.glsl"}});
    initializeShader("shaderA", {{GL_VERTEX_SHADER, m_resource_path + "/shader/shaderA.vs.glsl"},
                                 {GL_FRAGMENT_SHADER, m_resource_path + "/shader/shaderA.fs.glsl"}});
    initializeShader("shaderB", {{GL_VERTEX_SHADER, m_resource_path + "/shader/shaderB.vs.glsl"},
                                 {GL_FRAGMENT_SHADER, m_resource_path + "/shader/shaderB.fs.glsl"}});
    initializeShader("shaderC", {{GL_VERTEX_SHADER, m_resource_path + "/shader/shaderC.vs.glsl"},
                                 {GL_FRAGMENT_SHADER, m_resource_path + "/shader/shaderC.fs.glsl"}});
}

void Assignment4::initializeObjects()
{
    glActiveTexture(GL_TEXTURE0);
    envMap.bind();
}


void Assignment4::imgui()
{
    ImGui::SetNextWindowSize(ImVec2(200, 600), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    if (ImGui::Begin("Assignment 4"))
    {
        imGui_plotFPS();
        const char* textures[] = {"waterfall.png", "panorama.png"};
        if (ImGui::Combo("envMap", &envMapID, textures, 2))
        {
            useCylindricMapping = envMapID == 1;
            envMap = Tex(m_resource_path + "/data/" + textures[envMapID]);
            initializeObjects();
        }
        ImGui::Checkbox("debugUV", &debugUV);
        ImGui::SliderFloat("teapotRotation", &teapotRotation, 0, 2 * M_PI);
        ImGui::Separator();
        const char* names[] = {"shaderA", "shaderB", "shaderC"};
        ImGui::Combo("assignment", &assignmentNr, names, 3);
        if (assignmentNr == 2)
        {
            ImGui::SliderFloat("roughness", &roughness, 0, 0.1f);
            ImGui::SliderInt("glossyRays", &glossyRays, 1, 128);
        }
    }
    ImGui::End();
}
// exe entry point
int main(int argc, char* argv[])
{
    std::string resource_path = read_resource_path(argc, argv);
    Assignment4 application{resource_path};
    application.run(60);  // max fps
}
