#include "model_loader.h"

Model::Model(const std::string &filepath)
{
    loadModel(filepath);
}

void Model::loadModel(const std::string &filepath)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
        throw std::runtime_error(warn + err);

    for (const auto &shape : shapes)
    {
        for (const auto &index : shape.mesh.indices)
        {
            Vertex vertex;

            // 顶点
            vertex.position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]};

            // 法线
            if (!attrib.normals.empty())
            {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]};
            }

            vertices.push_back(vertex);
            indices.push_back(indices.size());
        }
    }
}

void Model::draw(const Shader &shader)
{
    // 绑定顶点数组对象并绘制

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}