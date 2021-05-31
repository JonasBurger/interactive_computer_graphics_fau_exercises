#include "a05.hpp"

#include <glbinding/gl/bitfield.h>
#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/type_precision.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

#include "shader_loader.hpp"

void Assignment5::update(float dt)
{
    if (bunny_rotate)
    {
        rotation_angle += dt * rotation_speed;
    }
}

void Assignment5::render()
{
    glm::fvec3 lightSource = -lightDir * lightDistance;

    renderShadowMap(lightSource);

    // TODO  additionally transform from clip- to texture-space
    shadowProj = getShadowVP(lightSource);
    //shadowProj = glm::scale(shadowProj, glm::vec3(0.5f, 0.5f, 0.f));
    //shadowProj = glm::translate(shadowProj, glm::vec3(2.f, 2.f, 1.f));



    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader("phong"));
    uniform("phong", "pcf_kernel_size", pcf_kernel_size);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, light_texture.index());
    uniform("phong", "light_shape", 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadow_map_id);
    uniform("phong", "shadow_map", 1);


    // set Camera and light
    uniform("phong", "viewMatrix", viewMatrix());
    uniform("phong", "projMatrix", projectionMatrix());
    uniform("phong", "shadowProj", shadowProj);
    uniform("phong", "lightSource", lightSource);

    // draw green bunny
    uniform("phong", "color", glm::fvec3(0.3f, 0.6f, 0.2f));
    glm::fmat4 M = glm::rotate(rotation_angle, glm::fvec3(0, 1, 0)) * glm::scale(glm::fvec3(0.2f, 0.2f, 0.2f));
    uniform("phong", "modelMatrix", M);
    bunny.draw();

    // draw grey ground plane
    uniform("phong", "color", glm::fvec3(0.5f, 0.5f, 0.5f));
    uniform("phong", "modelMatrix", glm::fmat4{1.0f});
    plane.draw();

    glUseProgram(0);
}
// Shadow

void Assignment5::initShadowMap()
{
    // Create a texture object to be used as depth buffer of a FBO
    {
        glGenTextures(1, &shadow_map_id);

        glBindTexture(GL_TEXTURE_2D, shadow_map_id);

        // Setup parameters for that texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // my opengl driver rejects GL_CLAMP as parameter 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // and https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexParameter.xhtml (GL_CLAMP is not a valid param)

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

        // Allocate memory for the texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_map_size.x, shadow_map_size.y, 0, GL_DEPTH_COMPONENT,
                     GL_UNSIGNED_BYTE, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Create a framebuffer object
    {
        glGenFramebuffers(1, &fbo_id);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

        // We won't use a color texture with the currently bound FBO
        glDrawBuffer(GL_NONE);

        // TODO: a)  Attach the depth texture to the FBO depth attachment point
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_map_id, 0);

        // Hint: Check if everything worked well
        GLenum FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (FBOstatus != GL_FRAMEBUFFER_COMPLETE) std::cerr << "FRAMEBUFFER setup incomplete!" << std::endl;

        // switch back to window-system-provided framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

glm::fmat4 Assignment5::getShadowVP(glm::fvec3 const& light_pos)
{
    const glm::fmat4 P = glm::perspective(20.0, 1.0, 1.0, 50.0);
    const glm::fmat4 V = glm::lookAt(light_pos, glm::fvec3(0.0f, -4.0f, 0.0f), glm::fvec3(0.0f, 1.0f, 0.0f));
    return P * V;
}

void Assignment5::renderShadowMap(glm::fvec3 const& light_pos)
{
    int orig_viewport[4];
    // TODO:
    // Bind the FBO. As we render the shadow map to a texture,
    // with different resolution, the glViewport() must be setup accordingly.
    // Hint: Do not forget to restore the original viewport and FBO afterwards!

    // save viewport
    glGetIntegerv(GL_VIEWPORT, orig_viewport);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
    glViewport(0, 0, shadow_map_size.x, shadow_map_size.y);

    const auto projMat = projectionMatrix();
    const auto viewMat = viewMatrix();


    // TODO:
    // The shadow map must contain the scene rendered from the lights position
    // Setup the matrices and render the scene from the viewpoint of the light
    // (see: render() and getShadowVP(...))
    // Hint: What is Shadow-Acne and how do you usually deal with z-fights?
    // Add a new Element (e.g. Slider + variable in .hpp) to the GUI, to make your solution configurable.
    const glm::fmat4 lightP = glm::perspective(20.0, 1.0, 1.0, 50.0);
    const glm::fmat4 lightV = glm::lookAt(light_pos, glm::fvec3(0.0f, -4.0f, 0.0f), glm::fvec3(0.0f, 1.0f, 0.0f));


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader("shadow"));

    // draw green bunny
    glm::fmat4 M = glm::rotate(rotation_angle, glm::fvec3(0, 1, 0)) * glm::scale(glm::fvec3(0.2f, 0.2f, 0.2f));
    uniform("shadow", "MVP", lightP*lightV*M);
    bunny.draw();

    // draw grey ground plane
    uniform("shadow", "MVP", lightP*lightV*glm::fmat4{1.0f});
    plane.draw();



    // cleanup
    glViewport(orig_viewport[0], orig_viewport[1], orig_viewport[2], orig_viewport[3]);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glUseProgram(0);
}

Assignment5::Assignment5(std::string const& resource_path)
    : Application{resource_path},
      light_texture{m_resource_path + "/data/spotlight.png"},
      bunny{m_resource_path + ".//data/bunny.obj"},
      plane{-2.8f, 12.f},
      lightDir{glm::normalize(glm::fvec3(9.0f, -15.f, -10.f))}
{
    initializeShaderPrograms();
    initShadowMap();
}



// load shader programs
void Assignment5::initializeShaderPrograms()
{
    initializeShader("phong", {{GL_VERTEX_SHADER, m_resource_path + "/shader/phong.vs.glsl"},
                               {GL_FRAGMENT_SHADER, m_resource_path + "/shader/phong.fs.glsl"}});
    initializeShader("shadow", {{GL_VERTEX_SHADER, m_resource_path + "/shader/shadow.vs.glsl"},
                                {GL_FRAGMENT_SHADER, m_resource_path + "/shader/shadow.fs.glsl"}});
}

void Assignment5::imgui()
{
    ImGui::SetNextWindowSize(ImVec2(200, 600), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    if (ImGui::Begin("Assignment 3"))
    {
        imGui_plotFPS();
        if (ImGui::CollapsingHeader("light"))
        {
            ImGui::Direction("lightDir", lightDir);
            ImGui::SliderFloat("lightDistance", &lightDistance, 1, 20);
        }

        ImGui::Checkbox("bunny_rotate", &bunny_rotate);
        if (bunny_rotate)
        {
            ImGui::SliderFloat("rotation_speed", &rotation_speed, -2, 2);
        }
        ImGui::SliderInt("pcf_kernel_size", &pcf_kernel_size, 1, 32);
        ImGui::Separator();

        // TODO: add GUI element(s) to make solution configurable

    }
    ImGui::End();
}

// exe entry point
int main(int argc, char* argv[])
{
    std::string resource_path = read_resource_path(argc, argv);
    Assignment5 application{resource_path};
    application.run(60);  // max fps
}
