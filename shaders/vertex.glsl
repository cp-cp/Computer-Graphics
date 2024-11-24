#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor; // 添加颜色输入
//layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec3 VertexColor; // 向片段着色器传递颜色
//out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    //TexCoords = aTexCoords;
    VertexColor = aColor; // 将顶点颜色传递给片段着色器

    gl_Position = projection * view * vec4(FragPos, 1.0);
} 