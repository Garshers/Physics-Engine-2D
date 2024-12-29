#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <random>
#include <string>
#include <chrono>

#include <World.h>

float zoom = 20.0f;
FlatVector cameraPosition(0.0f, 0.0f);
World MyWorld;
FlatVector clickPosition(0.0f, 0.0f);
bool isMousePressed = false;

float RandomFloatInRange(float min, float max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

void CreateBoundaries(World& MyWorld) {
    FlatVector Vector1 = FlatVector(-20.0f, -15.0f);
    FlatVector Vector2 = FlatVector(20.0f, -15.5f);
    Materials steelMaterial = Materials::CreateSteel();
    Bodies body1 = Bodies(Bodies::CreatePolygonBody(Vector1, Vector2, 0.2f, 1, steelMaterial));
    MyWorld.AddBody(body1);

    Vector1 = FlatVector(-21.0f, -15.0f);
    Vector2 = FlatVector(-20.0f, 15.5f);
    MyWorld.AddBody(Bodies::CreatePolygonBody(Vector1, Vector2, 0.8f, 1, steelMaterial));

    Vector1 = FlatVector(20.0f, -15.0f);
    Vector2 = FlatVector(21.0f, 15.5f);
    Bodies body3 = Bodies::CreatePolygonBody(Vector1, Vector2, 0.8f, 1, steelMaterial);
    MyWorld.AddBody(body3);
}
void CreateBodies(World& MyWorld) {
    CreateBoundaries(MyWorld);

    for (int i = 0; i <10; i++) {
        FlatVector Position = FlatVector(RandomFloatInRange(-10, 10), RandomFloatInRange(-15, 15));
        float radius = RandomFloatInRange(0.3f,1.1f);
        if (i % 2) {
            MyWorld.AddBody(Bodies::CreateCircleBody(Position, radius, 0.8f, 0, Materials::CreateBirch()));
        }
        else if (i % 3) {
            MyWorld.AddBody(Bodies::CreateCircleBody(Position, radius, 0.8f, 0, Materials::CreateGlass()));
        }
        else { MyWorld.AddBody(Bodies::CreateCircleBody(Position, radius, 0.8f, 0, Materials::CreateSteel())); }
        
    }
    for (int j = 0; j < 5; j++) {
        FlatVector Position = FlatVector(RandomFloatInRange(-10, 10), RandomFloatInRange(-10, 10));
        float radiusBox = RandomFloatInRange(1, 2);
        auto body = Bodies::CreatePolygonBody(4, Position, radiusBox, 0.4f, 0, Materials::CreateBirch());
        //body.Rotate(3.1415 * j / 3 + j);
        //body.Rotate(3.1415 /4);
        MyWorld.AddBody(body);
    }
    for (int j = 0; j < 0; j++) {
        FlatVector Position = FlatVector(RandomFloatInRange(-5, 5), RandomFloatInRange(-5, 5));
        FlatVector Rand = FlatVector(RandomFloatInRange(-4, -0.1), RandomFloatInRange(-4, -0.1));
        FlatVector Vec0 = Rand+Position;
        Rand = FlatVector(RandomFloatInRange(0.1, 4), RandomFloatInRange(0.1, 4));
        FlatVector Vec1 = Rand+Position;

        Bodies body = Bodies::CreatePolygonBody(Vec0, Vec1, 0.4f, 0, Materials::CreateBirch());
        body.Rotate(3.1415 * j / 3 + j);
        //body.Rotate(3.1415 /4);
        MyWorld.AddBody(body);
    }
    //std::vector<FlatVector> WaterBoundries = { FlatVector(-20.0f,0.0f), FlatVector(20.0f,0.0f), FlatVector(20.0f,-15.0f), FlatVector(-20.0f,-15.0f)};
    //MyWorld.AddLiquid(Liquids::CreateBodyOfWater(WaterBoundries));
}

void DrawCircle(const FlatVector& center, float radius, std::vector<float> color) {
    int numSegments = 100;
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(color[0], color[1], color[2]);
    glVertex2f(center.x, center.y);
    for (int i = 0; i <= numSegments; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(numSegments);
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        glVertex2f(center.x + x, center.y + y);
    }
    glEnd();
}

void DrawPolygon(FlatVector& Position, std::vector<FlatVector>& Vertices, std::vector<float> color) {
    glBegin(GL_POLYGON);
    glColor3f(color[0], color[1], color[2]);
    for (auto& vertex : Vertices) {
        glVertex2f(Position.x + vertex.x, Position.y + vertex.y);
    }
    glEnd();
}

void DrawLiquid(std::vector<FlatVector>& Vertices) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float transparency = 0.2f;

    glBegin(GL_POLYGON);
    glColor4f(0.0f, 0.0f, 1.0f, transparency);
    for (auto& vertex : Vertices) {
        glVertex2f(vertex.x, vertex.y);
    }
    glEnd();

    glDisable(GL_BLEND);
}

void DrawBodies(World& MyWorld) {
    
    for (int i = 0; i < MyWorld.BodyListSize(); i++) {
        Bodies* Body = MyWorld.GetBody(i);

        if (Body->Type == Bodies::Circle) {
            DrawCircle(Body->Position, Body->Radius, Body->Material.Color);
        }
        else if (Body->Type == Bodies::Polygon) {
            DrawPolygon(Body->Position, Body->Vertices, Body->Material.Color);
        }
    }
    for (int i = 0; i < MyWorld.LiquidListSize(); i++) {
        Liquids* Liquid = MyWorld.GetLiquid(i);
        DrawLiquid(Liquid->FluidBoundries);
    }
}
void drawAxes() {
    glBegin(GL_LINES);

    glColor3f(1.0f, 0.0f, 0.0f);
    for (float x = -10.0f; x <= 10.0f; x += 1.0f) {
        glVertex3f(x, -0.1f, 0.0f);
        glVertex3f(x, 0.1f, 0.0f);
    }

    glColor3f(0.0f, 1.0f, 0.0f);
    for (float y = -10.0f; y <= 10.0f; y += 1.0f) {
        glVertex3f(-0.1f, y, 0.0f);
        glVertex3f(0.1f, y, 0.0f); 
    }
    glEnd();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (yoffset > 0) {
        zoom += 0.1f;
    }
    else {
        zoom -= 0.1f;
        if (zoom < 0.1f) {
            zoom = 0.1f;
        }
    }
}
void HandleArrowKeys(GLFWwindow* window, Bodies& BodyToMove) {
    float movementSpeed = 0.01f;
    FlatVector movementAmount = { 0.0f, 0.0f };
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        movementAmount.y = movementSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        movementAmount.y = -movementSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        movementAmount.x = -movementSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        movementAmount.x = movementSpeed;
    }
    BodyToMove.Move(movementAmount);
}
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    static double lastX = xpos, lastY = ypos;
    double deltaX = xpos - lastX;
    double deltaY = ypos - lastY;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        cameraPosition.x -= static_cast<float>(deltaX) * 0.01f;
        cameraPosition.y += static_cast<float>(deltaY) * 0.01f;
    }

    lastX = xpos;
    lastY = ypos;
}

int main(void) {
  
    if (!glfwInit()) return -1;                                                             // Initialize GLFW library

    int width = 1600, height = 1200;
    GLFWwindow* window = glfwCreateWindow(width, height, "Physics Engine 2D", NULL, NULL);  // Create a windowed mode window and its OpenGL context

    if (!window) {                                                                          // If the window couldn't be created, terminate GLFW
        glfwTerminate();
        return -1;
    }

    CreateBodies(MyWorld);                                                                  // Create the bodies for the physics simulation

    glfwMakeContextCurrent(window);                                                         // Make the window's context current
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);                      // Set the framebuffer size callback
    glfwSetScrollCallback(window, scroll_callback);                                         // Set the scroll callback
    glfwSetCursorPosCallback(window, cursor_pos_callback);                                  // Set the cursor position callback

    if (glewInit() != GLEW_OK) return -1;                                                   // Initialize GLEW library

    std::this_thread::sleep_for(std::chrono::seconds(2));                                   // waiting 2sec before phisics simulation
    std::thread intersectionThread(&World::IntersectionThread, &MyWorld);                   // Create and start the intersection thread

    while (!glfwWindowShouldClose(window)) {                                                // Main loop until the window should close
        
        glClear(GL_COLOR_BUFFER_BIT);                                                       // Clear the color buffer

        glfwGetFramebufferSize(window, &width, &height);                                    // Get the framebuffer size

        glMatrixMode(GL_PROJECTION);                                                        // Set the projection matrix
        glLoadIdentity();
        glOrtho(-zoom * static_cast<float>(width) / static_cast<float>(height) + cameraPosition.x,
            zoom * static_cast<float>(width) / static_cast<float>(height) + cameraPosition.x,
            -zoom + cameraPosition.y, zoom + cameraPosition.y, -1.0f, 1.0f);
        glMatrixMode(GL_MODELVIEW);

        //Uncomment to move a specific body with arrow keys
        //Bodies* BodyToMove = MyWorld.GetBody(1);
        //HandleArrowKeys(window, *BodyToMove);

        drawAxes();                     // Draw the coordinate axes
        DrawBodies(MyWorld);            // Draw all the bodies in the world

        glfwSwapBuffers(window);        // Swap the front and back buffers
        glfwPollEvents();               // Poll for and process events
    }

    MyWorld.StopIntersectionThread();   // Stop the intersection thread and wait for it to finish
    intersectionThread.join();

    
    glfwTerminate();                    // Terminate GLFW
    return 0;
}