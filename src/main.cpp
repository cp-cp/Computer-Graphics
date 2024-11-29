#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <limits>
#include <iostream>

// 窗口大小
const int WIDTH = 800;
const int HEIGHT = 600;

// 像素缓冲区
struct Color
{
    float r, g, b;
    Color(float r = 0, float g = 0, float b = 0) : r(r), g(g), b(b) {}
};
std::vector<Color> framebuffer(WIDTH *HEIGHT);

// 定义基础向量类
struct Vector3
{
    float x, y, z;
    Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

    Vector3 operator+(const Vector3 &v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
    Vector3 operator-(const Vector3 &v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
    Vector3 operator*(float s) const { return Vector3(x * s, y * s, z * s); }
    Vector3 operator/(float s) const { return Vector3(x / s, y / s, z / s); }
    float dot(const Vector3 &v) const { return x * v.x + y * v.y + z * v.z; }
    Vector3 normalize() const
    {
        float len = std::sqrt(dot(*this));
        return (*this) / len;
    }
};

// 定义光线
struct Ray
{
    Vector3 origin, direction;
    Ray(const Vector3 &o, const Vector3 &d) : origin(o), direction(d.normalize()) {}
};

// 定义球体
struct Sphere
{
    Vector3 center;
    float radius;
    Color color;
    Sphere(const Vector3 &c, float r, const Color &col) : center(c), radius(r), color(col) {}

    // 光线和球体的相交判断
    bool intersect(const Ray &ray, float &t) const
    {
        Vector3 oc = ray.origin - center;
        float a = ray.direction.dot(ray.direction);
        float b = 2.0f * oc.dot(ray.direction);
        float c = oc.dot(oc) - radius * radius;
        float discriminant = b * b - 4 * a * c;
        if (discriminant < 0)
            return false;
        t = (-b - std::sqrt(discriminant)) / (2.0f * a);
        return t > 0;
    }
};

// 定义平面
struct Plane
{
    Vector3 point, normal;
    Color color;
    Plane(const Vector3 &p, const Vector3 &n, const Color &col) : point(p), normal(n.normalize()), color(col) {}

    // 光线和平面相交判断
    bool intersect(const Ray &ray, float &t) const
    {
        float denom = normal.dot(ray.direction);
        if (std::fabs(denom) > 1e-6)
        {
            t = (point - ray.origin).dot(normal) / denom;
            return t >= 0;
        }
        return false;
    }
};

// Phong光照模型
Color phongIllumination(const Vector3 &point, const Vector3 &normal, const Color &objectColor, const Vector3 &lightPos, const Vector3 &cameraPos)
{
    Vector3 lightDir = (lightPos - point).normalize();
    Vector3 viewDir = (cameraPos - point).normalize();
    Vector3 reflectDir = (normal * 2.0f * lightDir.dot(normal) - lightDir).normalize();

    float ambient = 0.1;
    float diffuse = std::max(0.0f, lightDir.dot(normal));
    float specular = std::pow(std::max(0.0f, reflectDir.dot(viewDir)), 32);

    float r = objectColor.r * (ambient + 0.8f * diffuse + 0.5f * specular);
    float g = objectColor.g * (ambient + 0.8f * diffuse + 0.5f * specular);
    float b = objectColor.b * (ambient + 0.8f * diffuse + 0.5f * specular);
    return Color(std::min(r, 1.0f), std::min(g, 1.0f), std::min(b, 1.0f));
}

// 渲染场景
void renderScene()
{
    Vector3 camera(0, 1, 5);    // 相机位置
    Vector3 lightPos(-5, 5, 5); // 光源位置

    Sphere redSphere(Vector3(-1.5, 0.5, 0), 0.5, Color(1, 0, 0));           // 红色球
    Sphere blueSphere(Vector3(1.5, 0.5, 0), 0.5, Color(0, 0, 1));           // 蓝色球
    Plane ground(Vector3(0, 0, 0), Vector3(0, 1, 0), Color(0.5, 0.3, 0.1)); // 地面
    Plane wall(Vector3(0, 0, -5), Vector3(0, 0, 1), Color(1, 1, 1));        // 墙壁

    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            float u = (x / (float)WIDTH) * 2 - 1;
            float v = (y / (float)HEIGHT) * 2 - 1;

            Ray ray(camera, Vector3(u, v, -1));
            float t = std::numeric_limits<float>::max();
            Color pixelColor;

            // 与场景物体求交
            if (redSphere.intersect(ray, t))
            {
                Vector3 hitPoint = ray.origin + ray.direction * t;
                Vector3 normal = (hitPoint - redSphere.center).normalize();
                pixelColor = phongIllumination(hitPoint, normal, redSphere.color, lightPos, camera);
            }
            else if (blueSphere.intersect(ray, t))
            {
                Vector3 hitPoint = ray.origin + ray.direction * t;
                Vector3 normal = (hitPoint - blueSphere.center).normalize();
                pixelColor = phongIllumination(hitPoint, normal, blueSphere.color, lightPos, camera);
            }
            else if (ground.intersect(ray, t))
            {
                pixelColor = ground.color;
            }
            else if (wall.intersect(ray, t))
            {
                pixelColor = wall.color;
            }

            framebuffer[y * WIDTH + x] = pixelColor;
        }
    }
}

// 显示结果
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_FLOAT, framebuffer.data());
    glutSwapBuffers();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Ray Tracing Scene");

    renderScene();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}