#include "a06.hpp"

#include <glbinding/gl/bitfield.h>
#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>
#include <glbinding/gl/types.h>
#include <glm/gtx/transform.hpp>
#include <iostream>

#include "shader_loader.hpp"


//! Here we generate the quad-mesh for the heightmap.
Heightmap::Heightmap(int size)
{
    int r_factor     = 64;  // texel per patch
    w                = size / r_factor + 1;
    h                = size / r_factor + 1;
    vertex_data_size = w * h * 3;
    index_data_size  = (w - 1) * (h - 1) * 4;
    pos              = new float[vertex_data_size];
    ids              = new int[index_data_size];

    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            pos[3 * (y * w + x) + 0] = float(x * r_factor) / float(size);
            pos[3 * (y * w + x) + 1] = 0.f;
            pos[3 * (y * w + x) + 2] = float(y * r_factor) / float(size);
        }
    }

    int i = 0;
    for (int y = 0; y < h - 1; ++y)
    {
        for (int x = 0; x < w - 1; ++x)
        {
            //  note the order of quad ids defined here:
            //  y
            //  ^
            //  3---2
            //  |   |
            //  0---1 -> x
            ids[i++] = y * w + x;
            ids[i++] = y * w + (x + 1);
            ids[i++] = (y + 1) * w + (x + 1);
            ids[i++] = (y + 1) * w + x;
        }
    }
    load_to_gpu();
}

//! Upload vertex data.
void Heightmap::load_to_gpu()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);

    glGenBuffers(2, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertex_data_size * sizeof(float), pos, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data_size * sizeof(int), ids, GL_STATIC_DRAW);
    glBindVertexArray(0);
}

//! Delete vertex data.
void Heightmap::remove_from_gpu()
{
    glBindVertexArray(vao);
    delete pos;
    delete ids;
    glDeleteBuffers(2, vbo);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);
}

//! Just the draw call. Shader setup should be done at the calling site.
void Heightmap::draw()
{
    glBindVertexArray(vao);

    // TODO A1 Setup Tessellation and call glDrawElements(...)
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawElements(GL_PATCHES, static_cast<GLsizei>(index_data_size), GL_UNSIGNED_INT, 0); 

    glBindVertexArray(0);
}


void Assignment6::render()
{
    // Clear color == diffuse lit water
    glm::fvec3 up        = glm::fvec3(0, 1, 0);
    float water_ndotl    = glm::dot(up, -lightDir);
    glm::fvec3 water_col = glm::fvec3(0.1f, 0.4f, 1.0f) * water_ndotl;
    glClearColor(water_col.x, water_col.y, water_col.z, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glUseProgram(shader("shader"));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.index());
    uniform("shader", "heightmap", 0);

    uniform("shader", "light_dir", lightDir);
    uniform("shader", "cam_pos", glm::fvec3(glm::inverse(viewMatrix()) * glm::fvec4{0, 0, 0, 1}));
    uniform("shader", "projection", projectionMatrix());
    uniform("shader", "view", viewMatrix());
    uniform("shader", "model", glm::scale(glm::fvec3{51.2f}));

    uniform("shader", "height_scale", height_scale);
    uniform("shader", "use_lod", use_lod);
    uniform("shader", "resolution", resolution());
    uniform("shader", "tess_const", tess_const);
    uniform("shader", "tess_factor", tess_factor);
    uniform("shader", "watermax", watermax * height_scale);
    uniform("shader", "sandmax", sandmax * height_scale);
    uniform("shader", "grassmax", grassmax * height_scale);
    uniform("shader", "stonemax", stonemax * height_scale);
    uniform("shader", "snowmax", snowmax * height_scale);

    uniform("shader", "wireframe", false);
    heightmap.draw();

    if (wireframe)
    {
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(off_factor, off_units);

        uniform("shader", "wireframe", true);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        heightmap.draw();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glUseProgram(0);
}



Assignment6::Assignment6(std::string const& resource_path)
    : Application{resource_path},
      lightDir{glm::normalize(glm::fvec3(-0.4f, -1.0f, 0.0f))},
      texture{m_resource_path + "/data/isle-of-man-512.png"},
      heightmap{(int)texture.dimensions().x}
{
    m_cam = cameraSystem{glm::fvec3(10, 5, 10), glm::fvec3(3, -1, 3), glm::fvec3(0, 1, 0)};
    updateCamera();
    initializeShaderPrograms();
}

// load shader programs
void Assignment6::initializeShaderPrograms()
{
    initializeShader("shader", {{GL_VERTEX_SHADER, m_resource_path + "/shader/vertex.glsl"},
                                {GL_TESS_CONTROL_SHADER, m_resource_path + "/shader/tes_control.glsl"},
                                {GL_TESS_EVALUATION_SHADER, m_resource_path + "/shader/tes_eval.glsl"},
                                {GL_FRAGMENT_SHADER, m_resource_path + "/shader/fragment.glsl"}});
}

void Assignment6::imgui()
{
    ImGui::SetNextWindowSize(ImVec2(200, 600), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    if (ImGui::Begin("Assignment 6"))
    {
        imGui_plotFPS();
        if (ImGui::CollapsingHeader("light"))
        {
            ImGui::Direction("lightDir", lightDir);
            ImGui::SliderFloat("off_factor", &off_factor, -5, 0);
            ImGui::SliderFloat("off_units", &off_units, -10000, 0);
        }
        const char* textures[] = {"isle-of-man-512.png", "falkland-islands-512.png"};
        if (ImGui::Combo("heightmap", &heightmapID, textures, 2))
        {
            heightmap.remove_from_gpu();
            texture   = Tex(m_resource_path + "/data/" + textures[heightmapID]);
            heightmap = Heightmap((int)texture.dimensions().x);
        }
        if (ImGui::CollapsingHeader("color ranges"))
        {
            ImGui::SliderFloat("watermax", &watermax, 0, sandmax);
            ImGui::SliderFloat("sandmax", &sandmax, watermax, grassmax);
            ImGui::SliderFloat("grassmax", &grassmax, sandmax, stonemax);
            ImGui::SliderFloat("stonemax", &stonemax, grassmax, snowmax);
            ImGui::SliderFloat("snowmax", &snowmax, stonemax, 1);
        }
        ImGui::Separator();
        ImGui::Checkbox("wireframe", &wireframe);
        ImGui::SliderFloat("height_scale", &height_scale, 0.01f, 1.0f);
        ImGui::Checkbox("use_lod", &use_lod);
        if (!use_lod)
        {
            ImGui::SliderFloat("tess_const", &tess_const, 1, 64, "%.3f", ImGuiSliderFlags_Logarithmic);
        }
        else
        {
            ImGui::SliderFloat("tess_factor", &tess_factor, 0.001f, 1, "%.3f", ImGuiSliderFlags_Logarithmic);
        }
    }
    ImGui::End();
}

// exe entry point
int main(int argc, char* argv[])
{
    std::string resource_path = read_resource_path(argc, argv);
    Assignment6 application{resource_path};
    application.run(60);  // max fps
}
