#include<stdio.h>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

const char g_szClassName[] = "myWindowClass";
HWND hwnd;
HDC hdc;
HGLRC hglrc;
MSG messages;
BOOL isRunning;
RECT WinSize;
int eScr, pScr;

typedef struct Color
{
    float r,g,b,a;
}Color;

typedef struct Vel
{
    float x;
    float y;
}Velocity;

typedef struct Quad
{
    Color c;
    Velocity v;
    float x, y;
    float width;
    float height;
}Pedal, Wall;

Pedal player, enemy;
Wall wall1, wall2;
struct Quad ball;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_LBUTTONDOWN:
        {
            char szFileName[MAX_PATH];
            HINSTANCE hInstance = GetModuleHandle(NULL);

            GetModuleFileName(hInstance, szFileName, MAX_PATH);
            MessageBox(hwnd, szFileName, "This program is:", MB_OK | MB_ICONINFORMATION);
        }
        break;

        case WM_KEYDOWN:
        {
            switch(wParam)
            {
                case VK_UP:
                    player.v.y = 0.5;
                    break;
                case VK_DOWN:
                    player.v.y = -0.5;
                    break;
            }
        }
        break;

        case WM_KEYUP:
        {
            switch(wParam)
            {
                case VK_UP:
                    player.v.y = 0.0;
                    break;
                case VK_DOWN:
                    player.v.y = 0.0;
                    break;
            }
        }
        break;

        case WM_CLOSE:
            isRunning = FALSE;
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void createWindow(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASSEX MWindow;         /* Data structure for the window class */
    int iFormat;
    PIXELFORMATDESCRIPTOR pfd;  /* This informs the system how we are going to use the DC. In other words, the features of OpenGl we need active */

    WinSize.left = 0;
    WinSize.top = 0;
    WinSize.right = 600;
    WinSize.bottom = 600;

    // Load the icon
    //icon = LoadIcon(hIn, MAKEINTRESOURCE(ICON_ID));

    // Fill the Window structure
    MWindow.hInstance = hInstance;
    // Class Name of the window class
    MWindow.lpszClassName = "Name";
    // Function called by windows
    MWindow.lpfnWndProc = WndProc;
    // Catch double clicks and own the Device Context(for opengl)
    MWindow.style = CS_DBLCLKS | CS_OWNDC;
    // Size of window structure
    MWindow.cbSize = sizeof (WNDCLASSEX);

    // Use default icon and mouse-pointer
    MWindow.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    MWindow.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
    //MWindow.hIcon = icon;
    //MWindow.hIconSm = icon;
    MWindow.hCursor = LoadCursor(NULL, IDC_ARROW);
    // Set the Resource Menu
    MWindow.lpszMenuName = NULL;//MAKEINTRESOURCE(IDR_MENU1);
    // No extra bytes after the window class
    MWindow.cbClsExtra = 0;
    // Structure or the window instance
    MWindow.cbWndExtra = 0;
    // Use Windows's default colour as the background of the window
    MWindow.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

    // Register the window class, and if it fails quit the program
    if (!RegisterClassEx(&MWindow))
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    // Adjust Window To True Requested Size
    AdjustWindowRectEx(&WinSize, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_APPWINDOW | WS_EX_ACCEPTFILES);

    //  Create the program after registering the class
    if(!(hwnd = CreateWindowEx (
           WS_EX_APPWINDOW | WS_EX_ACCEPTFILES,     /* Extended possibilities for variation */
           "Name",     /* Classname */
           "Pong",           /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           WinSize.right,           /* The programs width */
           WinSize.bottom,          /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           )))
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    // Create Device Context
    if(!(hdc = GetDC(hwnd)))
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    // Zero out the memory(initialize)
    ZeroMemory( &pfd, sizeof( pfd ) );
    pfd.nSize = sizeof( pfd );
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    // Picks the best match for what we requested
    if(!(iFormat = ChoosePixelFormat(hdc, &pfd)))
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    if(!SetPixelFormat(hdc, iFormat, &pfd))
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    // Create Render Context
    if(!(hglrc = wglCreateContext(hdc)))
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    // Try To Activate The Rendering Context
    if(!wglMakeCurrent(hdc, hglrc))
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    SetForegroundWindow(hwnd);
}

void initGL()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, 600, 0, 600, -1.0, 1.0);

    // Select ModelView Matrix
    glMatrixMode(GL_MODELVIEW);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // No need for depth test(extra information) because we are doing 2D
    glDisable(GL_DEPTH_TEST);
}

void initQuad(struct Quad *q, float x, float y, float width, float height, float r, float g, float b, float a, float vx, float vy)
{
    q->x = x;
    q->y = y;
    q->width = width;
    q->height = height;
    (q->c).r = r;
    (q->c).g = g;
    (q->c).b = b;
    (q->c).a = a;
    (q->v).x = vx;
    (q->v).y = vy;
}

void initGame()
{
    eScr = pScr = 0;
    initQuad(&player, 0.0, (float)(600 / 2 - 70 / 2), 10.0, 70.0, 1.0, 0.3, 0.1, 1.0, 0.0, 0.0);
    initQuad(&enemy, 600 - 10, 600 / 2 - 70 / 2, 10, 70, 1.0, 0.3, 0.1, 1.0, 0.0, 0.0);
    initQuad(&wall1, 0, 0, 600, 30, 0.7, 0.3, 0.3, 1.0, 0.0, 0.0);
    initQuad(&wall2, 0, 600 - 30, 600, 30, 0.7, 0.3, 0.3, 1.0, 0.0, 0.0);
    initQuad(&ball, 600 / 2 - 10 / 2, 600 / 2 - 10 / 2, 10, 10, 0.0, 0.5, 0.7, 1.0, -0.2, 0.0);
}

void updateQuadLocation(struct Quad *q)
{
    q->x += (q->v).x;
    q->y += (q->v).y;
}

int areColliding(struct Quad q1, struct Quad q2)
{
    if(q1.x + q1.width >= q2.x && q1.x <= q2.x + q2.width && q1.y <= q2.y + q2.height && q1.y + q1.height >= q2.y)
        return 1;
    else
        return 0;
}

void reset(struct quad *q)
{
    initQuad(q, 600 / 2 - 10 / 2, 600 / 2 - 10 / 2, 10, 10, 0.0, 0.5, 0.7, 1.0, -0.2, 0.0);
}

void update()
{
    if(areColliding(ball, wall1) || areColliding(ball, wall2))
    {
        ball.v.y *= -1;
    }
    if(areColliding(ball, player))
    {
        ball.v.x *= -1;
        ball.v.y = ((ball.height / 2 + ball.y) - (player.height / 2 + player.y)) * 0.005;
    }
    if(areColliding(ball, enemy))
    {
        ball.v.x *= -1;
        ball.v.y = ((ball.height / 2 + ball.y) - (enemy.height / 2 + enemy.y)) * 0.005;
    }

    float p = ((float)rand()/10000.0);
    if(p < 0.5f)
        p = 0.5f;
    else if(p > 1.2f)
        p = 1.2f;

    enemy.v.y = ((ball.height / 2 + ball.y) - (enemy.height / 2 + enemy.y)) * 0.004 * p;

    updateQuadLocation(&ball);
    updateQuadLocation(&player);
    updateQuadLocation(&enemy);

    if(ball.x < 0)
    {
        eScr++;
        printf("Scores: Player = %d, Enemy = %d\n", pScr, eScr);
        reset(&ball);
    }
    if(ball.x > 600)
    {
        pScr++;
        printf("Scores: Player = %d, Enemy = %d\n", pScr, eScr);
        reset(&ball);
    }
}

void drawRect(struct Quad q)
{
    glColor4f(q.c.r, q.c.g, q.c.b, q.c.a);

    glPushMatrix();

    glTranslatef(q.x, q.y, 0.0);

    glBegin(GL_QUADS);

    glVertex2f(0.0, 0.0);
    glVertex2f(0.0, q.height);
    glVertex2f(q.width, q.height);
    glVertex2f(q.width, 0.0);

    glEnd();

    glPopMatrix();
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the framebuffer

    glLoadIdentity();

    drawRect(player);
    drawRect(enemy);
    drawRect(wall1);
    drawRect(wall2);
    drawRect(ball);

    //render game
    SwapBuffers(hdc);
}

void cleanUp()
{

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    createWindow(hInstance, nCmdShow);
    initGL();

    initGame();

    isRunning = TRUE;

    while(isRunning)
    {
        //input();
        update();
        render();

        if (PeekMessage(&messages,NULL,0,0,PM_REMOVE))// Is There A Message Waiting?
        {
            if (messages.message == WM_QUIT)// Have We Received A Quit Message?
            {
                isRunning = FALSE;
            }
            else
            {
                /* Translate virtual-key messages into character messages */
                TranslateMessage(&messages);
                /* Send(Dispatch) message to WindowProcedure */
                DispatchMessage(&messages);
            }
        }
    }

    cleanUp();
}
