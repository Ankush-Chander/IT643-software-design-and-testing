#include "raylib.h"
using namespace std;
#include <deque>
#include <raymath.h>
Color green = {173,204,96,255};
Color dgreen = {43,51,24,255};

int cell =30;
int cellcount =25;
int offset =75;
int sc = 0;
double lup = 0;
bool run = true;

bool eid(Vector2 e,deque<Vector2> d)
{
    for(unsigned int i=0;i<d.size();i++)
    {
        if (Vector2Equals(e, d[i])) return true;
    }
    return false;
}

bool eventTrigger(double in)
{
    double  ct = GetTime();
    if(ct - lup>= in)
    {
        lup=ct;
        return true;
    }
    return false;
}
class Food{
    public:
    Vector2 p;
    Texture2D f;
    Food(deque<Vector2> sb)
    {
        Image img = LoadImage("food.png");
        f = LoadTextureFromImage(img);
        UnloadImage(img);
        p = grp(sb);
    }
    ~Food()
    {
        UnloadTexture(f);
    }
    void Draw()
    {
        Vector2 xy = { offset + p.x * cell,offset + p.y * cell };
        DrawTextureEx(f,xy, 0.0f,(float)cell/f.width, BLACK); 
    }
    Vector2 grp(deque<Vector2> sbody)
    {
        float x = GetRandomValue(0,cellcount-1);
        float y = GetRandomValue(0,cellcount-1);
        Vector2 p = {x,y};
        while(eid(p,sbody))
        {
                float x = GetRandomValue(0,cellcount-1);
                float y = GetRandomValue(0,cellcount-1);
                p = {x,y};
        }
        return p;
    }
};
class Snake{
    public:
    deque<Vector2> body = {Vector2{1,0},Vector2{0,0}};
    Vector2 direction = {1,0};
    bool addb = false;
    void Draw()
    {
        for(unsigned int i=0;i<body.size();i++)
        {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle s = Rectangle{offset +x*cell,offset +y*cell,(float)cell,(float)cell};
            DrawRectangleRounded(s,0.5,7,dgreen);
        }
    }
    void update()
    {
         body.push_front(Vector2Add(body[0], direction));
        if (addb == true)
        {
            addb = false;
        }
        else
        {
            body.pop_back();
        }
    }
    void reset()
    {
        body = {Vector2{1,0},Vector2{0,0}};
        direction = {1,0};
        run = false;
    }
};

void go(Snake &s, Food &f)
{
    s.reset();                // reset snake
    f.p = f.grp(s.body);      // place food again
    sc=0;
}

int main(void)
{
    InitWindow(2 * offset + cell * cellcount, 2 * offset + cell * cellcount, "Snake As A Reincarnation");
    SetTargetFPS(60);
    InitAudioDevice();    
    Sound eat = LoadSound("eat.mp3");
    Sound wall = LoadSound("wall.mp3");
    Snake s = Snake();
    Food food = Food(s.body);
    while (!WindowShouldClose())
    {
        BeginDrawing();
            if(eventTrigger(0.3))
            {if(run){s.update();}}
            if(IsKeyPressed(KEY_UP) && (s.direction.y!=1))
            {
                s.direction={0,-1};
                run=true;
            }
            if(IsKeyPressed(KEY_DOWN) && (s.direction.y!=-1))
            {
                s.direction={0,1};
                run=true;
            }
            if(IsKeyPressed(KEY_LEFT) && (s.direction.x!=1))
            {
                s.direction={-1,0};
                run=true;
            }
            if(IsKeyPressed(KEY_RIGHT) && (s.direction.x!=-1))
            {
                s.direction={1,0};
                run=true;
            }
            if(Vector2Equals(s.body[0],food.p))
            {
                food.p=food.grp(s.body);
                s.addb = true;
                sc++;
                PlaySound(eat);
            }
            if(s.body[0].x==cellcount||s.body[0].x==-1)
            {
                go(s, food);
                PlaySound(wall);
            }
            if(s.body[0].y==cellcount ||s.body[0].y==-1)
            {
                go(s, food);
                PlaySound(wall);
            }
            deque<Vector2> hb = s.body;
            hb.pop_front();
            if(eid(s.body[0],hb))
            {
                go(s,food);
            }
            ClearBackground(green);
            DrawRectangleLinesEx(Rectangle{(float)offset-5,(float)offset-5,(float)cell*cellcount+10,(float)cell*cellcount+10},5,BLACK);
            DrawText("Snake As A Reincarnation", offset - 5, 20, 40, dgreen);
            DrawText(TextFormat("%i", sc), offset - 5, offset + cell * cellcount + 10, 40, dgreen);
            food.Draw();
            s.Draw();
        EndDrawing();
    }
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
