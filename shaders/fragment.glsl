#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture1;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main() {
    // 环境光
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // 漫反射光
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // 镜面光
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    // 计算最终颜色
    vec3 result = (ambient + diffuse + specular) * vec3(texture(texture1, TexCoord));
    FragColor = vec4(result, 1.0);
}