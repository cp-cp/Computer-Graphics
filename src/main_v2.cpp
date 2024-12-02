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
};
std::vector<Color> framebuffer(WIDTH *HEIGHT);

// 定义基础向量类
struct Vector3
{
    float x, y, z;
    Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

    Vector3 operator+(const Vector3 &v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
    Vector3 operator-(const Vector3 &v) const { return Vector3(x - v.x, y - v.z, z - v.z); }
    Vector3 operator*(float s) const { return Vector3(x * s, y * s, z * s); }
    Vector3 operator/(float s) const { return Vector3(x / s, y / s, z / s); }
    float dot(const Vector3 &v) const { return x * v.x + y * v.y + z * v.z; }
    Vector3 normalize() const
    {
        float len = std::sqrt(dot(*this));
        return (*this) / len;
    }
    Vector3 operator^(const Vector3 &v) const {
        return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
};

// 定义光线
struct Ray
{
    Vector3 origin, direction;
    Ray(const Vector3 &o, const Vector3 &d) : origin(o), direction(d.normalize()) {}
};

// 定义平面
struct Plane
{
    Vector3 point, normal;
    Color color;
    Plane(const Vector3 &p, const Vector3 &n, const Color &col) : point(p), normal(n.normalize()), color(col) {}

    bool intersect(const Ray &ray, float &t) const
    {
        float denom = normal.dot(ray.direction);
        if (std::fabs(denom) > 1e-6)
        {
            float tTemp = (point - ray.origin).dot(normal) / denom;
            if (tTemp >= 0 && tTemp < t)
            {
                t = tTemp;
                return true;
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

    bool intersect(const Ray &ray, float &t) const
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
            return true;
        }
        return false;
    }
};

void renderScene(); // 前向声明

float cameraAngleY = 0.0f; // 摄像机旋转角度

// 更新摄像机位置
Vector3 updateCameraPosition()
{
    return Vector3(std::cos(cameraAngleY) * 5, 1, std::sin(cameraAngleY) * 5);
}
// 处理键盘输入
void keyboard(unsigned char key, int x, int y)
{
    if (key == 'a')
        cameraAngleY -= 0.1f;
    else if (key == 'd')
        cameraAngleY += 0.1f;

    renderScene();
    glutPostRedisplay();
}

// 渲染场景
void renderScene()
{
    Vector3 camera = updateCameraPosition();                                   // 更新摄像机位置
    Vector3 cameraLookAt(0, 1, 0);                                             // 摄像机看向固定点（房间中心）
    Vector3 cameraDirection = (cameraLookAt - camera).normalize();             // 计算摄像机方向
    Vector3 cameraRight = Vector3(0, 1, 0) ^ cameraDirection; // 摄像机右向量
    Vector3 cameraUp = cameraDirection ^ cameraRight;         // 摄像机上向量

    Vector3 lightPos(-5, 5, 5);

    Sphere redSphere(Vector3(-1.5, 0.5, -5), 0.5, Color(1, 0, 0));
    Sphere blueSphere(Vector3(1.5, 0.5, -5), 0.5, Color(0, 0, 1));

    Plane ground(Vector3(0, 0, 0), Vector3(0, 1, 0), Color(0.5, 0.3, 0.1));
    Plane ceiling(Vector3(0, 5, 0), Vector3(0, -1, 0), Color(0.8, 0.8, 0.8));
    Plane backWall(Vector3(0, 0, -10), Vector3(0, 0, 1), Color(0.5, 0.5, 0.5));
    Plane leftWall(Vector3(-5, 0, 0), Vector3(1, 0, 0), Color(1, 0.5, 0.5));
    Plane rightWall(Vector3(5, 0, 0), Vector3(-1, 0, 0), Color(0.5, 0.5, 1));

    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            // 屏幕空间坐标 [-1, 1]
            float u = (x + 0.5f) / WIDTH * 2 - 1;
            float v = (y + 0.5f) / HEIGHT * 2 - 1;

            // 计算射线方向（结合摄像机的旋转）
            Vector3 rayDir = (cameraDirection + cameraRight * u + cameraUp * v).normalize();
            Ray ray(camera, rayDir);

            float t = std::numeric_limits<float>::max();
            Color pixelColor(0.2f, 0.2f, 0.2f); // 默认背景色

            if (redSphere.intersect(ray, t))
            {
                pixelColor = redSphere.color;
            }
            if (blueSphere.intersect(ray, t))
            {
                pixelColor = blueSphere.color;
            }
            if (ground.intersect(ray, t))
            {
                pixelColor = ground.color;
            }
            if (ceiling.intersect(ray, t))
            {
                pixelColor = ceiling.color;
            }
            if (backWall.intersect(ray, t))
            {
                pixelColor = backWall.color;
            }
            if (leftWall.intersect(ray, t))
            {
                pixelColor = leftWall.color;
            }
            if (rightWall.intersect(ray, t))
            {
                pixelColor = rightWall.color;
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
    glutCreateWindow("Ray Tracing Room");

    renderScene();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}