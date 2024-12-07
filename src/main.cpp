#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <limits>

// 窗口大小
const int WIDTH = 800;
const int HEIGHT = 600;

// 像素缓冲区
struct Color
{
    float r, g, b;
    Color(float r = 0, float g = 0, float b = 0) : r(r), g(g), b(b) {}
    Color operator*(float s) const { return Color(r * s, g * s, b * s); }
    Color operator+(const Color &c) const { return Color(r + c.r, g + c.g, b + c.b); }
    Color &operator+=(const Color &c)
    {
        r += c.r;
        g += c.g;
        b += c.b;
        return *this;
    }
};
std::vector<Color> framebuffer(WIDTH * HEIGHT);

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
    Vector3 operator^(const Vector3 &v) const
    {
        return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
};

// 定义光线
struct Ray
{
    Vector3 origin, direction;
    Ray(const Vector3 &o, const Vector3 &d) : origin(o), direction(d.normalize()) {}
};

// 定义有限墙壁
struct Rectangle
{
    Vector3 center, normal, up, right;
    float width, height;
    Color color;

    Rectangle(const Vector3 &c, const Vector3 &n, const Vector3 &u, float w, float h, const Color &col)
        : center(c), normal(n.normalize()), up(u.normalize()), width(w), height(h), color(col)
    {
        right = (normal ^ up).normalize();
    }

    bool intersect(const Ray &ray, float &t, Vector3 &hitPoint, Vector3 &hitNormal) const
    {
        float denom = normal.dot(ray.direction);
        if (std::fabs(denom) > 1e-6)
        {
            float tTemp = (center - ray.origin).dot(normal) / denom;
            if (tTemp >= 0 && tTemp < t)
            {
                Vector3 p = ray.origin + ray.direction * tTemp;
                Vector3 local = p - center;
                float projU = std::fabs(local.dot(up));
                float projR = std::fabs(local.dot(right));
                if (projU <= height / 2 && projR <= width / 2)
                {
                    t = tTemp;
                    hitPoint = p;
                    hitNormal = normal;
                    return true;
                }
            }
        }
        return false;
    }
};

// 定义球体
struct Sphere
{
    Vector3 center;
    float radius;
    Color color;
    Sphere(const Vector3 &c, float r, const Color &col) : center(c), radius(r), color(col) {}

    bool intersect(const Ray &ray, float &t, Vector3 &hitPoint, Vector3 &hitNormal) const
    {
        Vector3 oc = ray.origin - center;
        float a = ray.direction.dot(ray.direction);
        float b = 2.0f * oc.dot(ray.direction);
        float c = oc.dot(oc) - radius * radius;
        float discriminant = b * b - 4 * a * c;
        if (discriminant < 0)
            return false;
        float tTemp = (-b - std::sqrt(discriminant)) / (2.0f * a);
        if (tTemp > 0 && tTemp < t)
        {
            t = tTemp;
            hitPoint = ray.origin + ray.direction * tTemp;
            hitNormal = (hitPoint - center).normalize();
            return true;
        }
        return false;
    }
};

// 全局相机和光源位置
Vector3 cameraPosition(0, 8, 12);
Vector3 cameraLookAt(0, 1, -5);
Vector3 lightPosition(-5, 5, 5);

// Phong���照模型
Color phongShading(const Vector3 &point, const Vector3 &normal, const Vector3 &lightPos, const Color &objectColor, bool inShadow)
{
    if (inShadow)
    {
        // 只考虑环境光
        float ambientStrength = 0.2f;
        return objectColor * ambientStrength;
    }
    else
    {
        Vector3 lightDir = (lightPos - point).normalize();
        Vector3 viewDir = (cameraPosition - point).normalize();
        Vector3 reflectDir = (normal * 2 * normal.dot(lightDir) - lightDir).normalize();

        float ambientStrength = 0.1f;
        float diffuseStrength = 0.7f;
        float specularStrength = 0.2f;
        int shininess = 32;

        Color ambient = objectColor * ambientStrength;
        float diff = std::max(0.0f, normal.dot(lightDir));
        Color diffuse = objectColor * (diff * diffuseStrength);
        float spec = std::pow(std::max(0.0f, viewDir.dot(reflectDir)), shininess);
        Color specular = Color(1, 1, 1) * (spec * specularStrength);

        return ambient + diffuse + specular;
    }
}

// 检查点是否在阴影中
bool isInShadow(const Vector3 &point, const Vector3 &lightPos, const std::vector<Sphere> &spheres, const std::vector<Rectangle> &rectangles)
{
    Vector3 shadowDir = (lightPos - point).normalize();
    Ray shadowRay(point + shadowDir * 1e-4f, shadowDir);
    float tMax = (lightPos - point).dot(lightPos - point);
    
    // 检查球体
    for (const auto &sphere : spheres)
    {
        float t = std::sqrt(tMax);
        Vector3 hitPoint, hitNormal;
        if (sphere.intersect(shadowRay, t, hitPoint, hitNormal))
        {
            return true;
        }
    }

    // 检查墙壁
    for (const auto &rect : rectangles)
    {
        float t = std::sqrt(tMax);
        Vector3 hitPoint, hitNormal;
        if (rect.intersect(shadowRay, t, hitPoint, hitNormal))
        {
            return true;
        }
    }

    return false;
}

// 光线追踪
Color traceRay(const Ray &ray, const std::vector<Sphere> &spheres, const std::vector<Rectangle> &rectangles, int depth = 20)
{
    if (depth <= 0)
        return Color(0.2f, 0.2f, 0.2f);

    float t = std::numeric_limits<float>::max();
    Color hitColor(0.2f, 0.2f, 0.2f);
    Vector3 hitPoint, hitNormal;
    bool hit = false;
    Color objectColor;

    // 检查所有球体
    for (const auto &sphere : spheres)
    {
        if (sphere.intersect(ray, t, hitPoint, hitNormal))
        {
            hit = true;
            objectColor = sphere.color;
        }
    }

    // 检查所有墙壁
    for (const auto &rect : rectangles)
    {
        if (rect.intersect(ray, t, hitPoint, hitNormal))
        {
            hit = true;
            objectColor = rect.color;
        }
    }

    if (hit)
    {
        // 检查阴影
        bool inShadow = isInShadow(hitPoint, lightPosition, spheres, rectangles);
        hitColor = phongShading(hitPoint, hitNormal, lightPosition, objectColor, inShadow);

        // 反射
        Vector3 reflectDir = (ray.direction - hitNormal * 2 * ray.direction.dot(hitNormal)).normalize();
        Ray reflectRay(hitPoint + reflectDir * 1e-4f, reflectDir);
        Color reflectColor = traceRay(reflectRay, spheres, rectangles, depth - 1);
        hitColor = hitColor * 0.8f + reflectColor * 0.2f;
    }

    return hitColor;
}

// 渲染场景
void renderScene()
{
    Vector3 cameraDirection = (cameraLookAt - cameraPosition).normalize();
    Vector3 cameraRight = Vector3(0, 1, 0) ^ cameraDirection;
    Vector3 cameraUp = cameraDirection ^ cameraRight;

    std::vector<Sphere> spheres = {
        Sphere(Vector3(-1.5, 0.5, -5), 0.5, Color(1, 0, 0)),
        Sphere(Vector3(1.5, 0.5, -5), 0.5, Color(0, 0, 1))};
    
    // 墙壁颜色统一
    Color wallColor(0.7f, 0.7f, 0.7f);
    
    std::vector<Rectangle> rectangles = {
        // 地板
        Rectangle(Vector3(0, 0, -5), Vector3(0, 1, 0), Vector3(1, 0, 0), 10, 10, Color(0.5, 0.3, 0.1)),
        // 后墙
        Rectangle(Vector3(0, 5, -10), Vector3(0, 0, 1), Vector3(0, 1, 0), 10, 10, wallColor),
        // 左墙
        Rectangle(Vector3(-5, 5, -5), Vector3(1, 0, 0), Vector3(0, 1, 0), 10, 10, wallColor),
        // 右墙
        Rectangle(Vector3(5, 5, -5), Vector3(-1, 0, 0), Vector3(0, 1, 0), 10, 10, wallColor),
        // 天顶
        Rectangle(Vector3(0, 10, -5), Vector3(0, -1, 0), Vector3(1, 0, 0), 10, 10, wallColor)
    };

    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            float u = (x + 0.5f) / WIDTH * 2 - 1;
            float v = (y + 0.5f) / HEIGHT * 2 - 1;

            Vector3 rayDir = (cameraDirection + cameraRight * u + cameraUp * v).normalize();
            Ray ray(cameraPosition, rayDir);

            framebuffer[y * WIDTH + x] = traceRay(ray, spheres, rectangles);
        }
    }
}

// 显示函数
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_FLOAT, framebuffer.data());
    glutSwapBuffers();
}

// 键盘回调函数
void handleKeypress(unsigned char key, int x, int y)
{
    float moveStep = 0.5f;
    switch (key)
    {
    case 'w':
        cameraPosition.z -= moveStep;
        cameraLookAt.z -= moveStep;
        break;
    case 's':
        cameraPosition.z += moveStep;
        cameraLookAt.z += moveStep;
        break;
    case 'a':
        cameraPosition.x -= moveStep;
        cameraLookAt.x -= moveStep;
        break;
    case 'd':
        cameraPosition.x += moveStep;
        cameraLookAt.x += moveStep;
        break;
    case 'i':
        lightPosition.y += moveStep;
        break;
    case 'k':
        lightPosition.y -= moveStep;
        break;
    case 'j':
        lightPosition.x -= moveStep;
        break;
    case 'l':
        lightPosition.x += moveStep;
        break;
    case 'u':
        lightPosition.z += moveStep;
        break;
    case 'o':
        lightPosition.z -= moveStep;
        break;
    case 27: // ESC键退出
        exit(0);
    default:
        break;
    }
    renderScene();
    glutPostRedisplay();
}

// 主函数
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Ray Tracing with Shadows");
    glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);

    renderScene();
    glutDisplayFunc(display);
    glutKeyboardFunc(handleKeypress);
    glutMainLoop();
    return 0;
}