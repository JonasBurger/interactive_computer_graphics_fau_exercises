#pragma once
#include "application.hpp"
#include "helper.hpp"

#include <glm/glm.hpp>

class Heightmap
{
    float* pos;
    int* ids;
    int vertex_data_size, index_data_size;
    GLuint vao;
    GLuint vbo[2];
    int w, h;
    void load_to_gpu();

   public:
    Heightmap(int size);
    //~Heightmap();
    Heightmap(Heightmap const&) = delete;
    //Heightmap& operator=(Heightmap const&) = delete;
    void draw();
    void remove_from_gpu();
};

class Assignment6 : public Application
{
   public:
    // allocate and initialize objects
    Assignment6(std::string const& resource_path);
    Assignment6(Assignment6 const&) = delete;
    Assignment6& operator=(Assignment6 const&) = delete;

    // draw all objects
    void render() override;
    void imgui() override;
    void update(float dt) override {};

   private:
    // common methods
    void initializeShaderPrograms();

    glm::fvec3 lightDir;
    Tex texture;
    int heightmapID;
    Heightmap heightmap;

    float watermax = 0.1f;
    float sandmax = 0.3f;
    float grassmax = 0.5f;
    float stonemax = 0.62f;
    float snowmax = 0.8f;

    float height_scale = 0.05f;
    float tess_const   = 1.0f;
    float tess_factor  = 0.01f;
    bool use_lod =false;
    bool wireframe=false;

    float off_factor = 0;
    float off_units = -5000;
};
