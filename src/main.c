#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

/* Palm Island: atmospheric exploration prototype
 * C11 + Raylib 5.5. No external art required for this milestone.
 */
#define SW 1280
#define SH 720
#define WORLD_W 5200.0f
#define FLOOR_Y 610.0f
#define PLAYER_W 30.0f
#define PLAYER_H 54.0f
#define MOVE_SPEED 275.0f
#define GROUND_ACCEL 1900.0f
#define AIR_ACCEL 1050.0f
#define DRAG 2200.0f
#define GRAVITY 1550.0f
#define JUMP_VELOCITY 565.0f
#define COYOTE 0.12f
#define JUMP_BUFFER 0.14f

static const Color INK = {20, 31, 39, 255};
static const Color LEAF = {54, 103, 81, 255};
static const Color GOLD_ACCENT = {244, 202, 116, 255};
static const Color MIST = {213, 232, 222, 255};

typedef struct Player {
    Vector2 pos, vel;
    int facing;
    bool grounded;
    float coyote, jumpBuffer, anim, landSquash;
} Player;

typedef struct Platform { Rectangle r; } Platform;
typedef struct Particle { Vector2 p, v; float life, size; } Particle;

static const Platform platforms[] = {
    {{0, FLOOR_Y, 880, 220}}, {{1000, FLOOR_Y, 640, 220}},
    {{1770, FLOOR_Y, 690, 220}}, {{2610, FLOOR_Y, 690, 220}},
    {{3440, FLOOR_Y, 790, 220}}, {{4380, FLOOR_Y, 820, 220}},
    {{520, 505, 180, 22}}, {{740, 430, 170, 22}},
    {{1210, 500, 190, 22}}, {{1490, 435, 180, 22}},
    {{1980, 500, 190, 22}}, {{2250, 420, 180, 22}},
    {{2860, 490, 190, 22}}, {{3150, 415, 190, 22}},
    {{3700, 500, 190, 22}}, {{3980, 430, 190, 22}},
    {{4610, 490, 180, 22}}, {{4880, 415, 190, 22}}
};
#define PLATFORM_COUNT ((int)(sizeof(platforms)/sizeof(platforms[0])))

static float ClampFloat(float value, float minValue, float maxValue) {
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}
static float Approach(float a, float b, float amount) {
    if (a < b) return fminf(a + amount, b);
    return fmaxf(a - amount, b);
}
static Rectangle PlayerRect(const Player *p) { return (Rectangle){p->pos.x, p->pos.y, PLAYER_W, PLAYER_H}; }
static void ResetPlayer(Player *p) {
    p->pos = (Vector2){90, FLOOR_Y - PLAYER_H}; p->vel = (Vector2){0,0};
    p->facing = 1; p->grounded = true; p->coyote = COYOTE; p->jumpBuffer = 0;
    p->anim = 0; p->landSquash = 0;
}
static bool JumpHeld(void) { return IsKeyDown(KEY_SPACE)||IsKeyDown(KEY_W)||IsKeyDown(KEY_UP)||(IsGamepadAvailable(0)&&IsGamepadButtonDown(0,GAMEPAD_BUTTON_RIGHT_FACE_DOWN)); }
static bool JumpPressed(void) { return IsKeyPressed(KEY_SPACE)||IsKeyPressed(KEY_W)||IsKeyPressed(KEY_UP)||(IsGamepadAvailable(0)&&IsGamepadButtonPressed(0,GAMEPAD_BUTTON_RIGHT_FACE_DOWN)); }
static float HorizontalInput(void) {
    float x = 0;
    if (IsKeyDown(KEY_A)||IsKeyDown(KEY_LEFT)) x -= 1;
    if (IsKeyDown(KEY_D)||IsKeyDown(KEY_RIGHT)) x += 1;
    if (IsGamepadAvailable(0)) { float a=GetGamepadAxisMovement(0,GAMEPAD_AXIS_LEFT_X); if(fabsf(a)>.2f)x=a; if(IsGamepadButtonDown(0,GAMEPAD_BUTTON_LEFT_FACE_LEFT))x=-1; if(IsGamepadButtonDown(0,GAMEPAD_BUTTON_LEFT_FACE_RIGHT))x=1; }
    return x;
}
static void UpdatePlayer(Player *p, float dt) {
    float input=HorizontalInput();
    if (input != 0) { p->vel.x=Approach(p->vel.x,input*MOVE_SPEED,(p->grounded?GROUND_ACCEL:AIR_ACCEL)*dt); p->facing=input>0?1:-1; }
    else p->vel.x=Approach(p->vel.x,0,(p->grounded?DRAG:DRAG*.30f)*dt);
    if(JumpPressed()) p->jumpBuffer=JUMP_BUFFER; else p->jumpBuffer=fmaxf(0,p->jumpBuffer-dt);
    if(p->grounded) p->coyote=COYOTE; else p->coyote=fmaxf(0,p->coyote-dt);
    if(p->jumpBuffer>0 && p->coyote>0) { p->vel.y=-JUMP_VELOCITY; p->grounded=false; p->coyote=0; p->jumpBuffer=0; }
    if(!JumpHeld() && p->vel.y < -170) p->vel.y=-170;

    /* Substeps prevent tunnelling at high frame times and separate axis collisions. */
    int steps=(int)ceilf(fmaxf(fabsf(p->vel.x),fabsf(p->vel.y))*dt/8.0f); if(steps<1)steps=1; if(steps>12)steps=12;
    float step=dt/steps;
    for(int s=0;s<steps;s++) {
        p->pos.x += p->vel.x*step;
        Rectangle r=PlayerRect(p);
        for(int i=0;i<PLATFORM_COUNT;i++) if(CheckCollisionRecs(r,platforms[i].r)) {
            if(p->vel.x>0) p->pos.x=platforms[i].r.x-PLAYER_W;
            else if(p->vel.x<0) p->pos.x=platforms[i].r.x+platforms[i].r.width;
            p->vel.x=0; r=PlayerRect(p);
        }
        p->vel.y += GRAVITY*step;
        float oldY=p->pos.y; p->pos.y += p->vel.y*step; r=PlayerRect(p);
        p->grounded=false;
        for(int i=0;i<PLATFORM_COUNT;i++) {
            Rectangle q=platforms[i].r;
            if(r.x+ r.width<=q.x || r.x>=q.x+q.width) continue;
            if(p->vel.y>=0 && oldY+PLAYER_H<=q.y && p->pos.y+PLAYER_H>=q.y) {
                p->pos.y=q.y-PLAYER_H; p->vel.y=0; p->grounded=true; p->landSquash=.12f;
            } else if(p->vel.y<0 && oldY>=q.y+q.height && p->pos.y<=q.y+q.height) {
                p->pos.y=q.y+q.height; p->vel.y=0;
            }
        }
        if(p->pos.y>SH+500) ResetPlayer(p);
    }
    p->pos.x=ClampFloat(p->pos.x,0,WORLD_W-PLAYER_W);
    p->anim += fabsf(p->vel.x)*dt/42.0f; p->landSquash=fmaxf(0,p->landSquash-dt);
}
static void DrawCloud(float x,float y,float scale,Color c) {
    DrawCircleV((Vector2){x,y},22*scale,c); DrawCircleV((Vector2){x+25*scale,y-8*scale},29*scale,c); DrawCircleV((Vector2){x+54*scale,y},20*scale,c); DrawRectangle((int)x,(int)y-4*scale,(int)(54*scale),(int)(22*scale),c);
}
static void DrawBackground(float camX,float time) {
    DrawRectangleGradientV(0,0,SW,SH,(Color){18,43,65,255},(Color){94,153,151,255});
    /* distant sun and atmospheric bloom */
    DrawCircleGradient(1010,142,98,(Color){255,226,166,100},(Color){255,226,166,0});
    DrawCircle(1010,142,43,(Color){255,225,164,235});
    for(int i=0;i<6;i++) { float x=fmodf(i*310.0f-camX*.10f+SW+400,SW+500)-150; DrawCloud(x,95+(i%3)*39,0.72f+(i%2)*.25f,(Color){209,226,216,95}); }
    /* distant island ridges */
    for(int i=0;i<15;i++) { float x=i*190.0f-camX*.19f; float h=125+(i*47%105); DrawTriangle((Vector2){x-80,560},(Vector2){x+65,560-h},(Vector2){x+220,560},(Color){47,100,104,190}); }
    /* jungle canopy layers, each moving at a different parallax rate */
    for(int layer=0;layer<2;layer++) {
        float rate=layer?.47f:.31f;
        for(int i=0;i<27;i++) {
            float x=i*105.0f-fmodf(camX*rate,105.0f)-70;
            float h=(layer?105:150)+(i*31%70);
            Color c=layer?(Color){29,76,72,235}:(Color){36,91,84,210};
            DrawRectangle((int)x, (int)(580-h*.35f), 18, (int)(h*.35f), c);
            DrawCircle((int)(x+8),(int)(580-h*.52f),(float)(h*.36f),c);
            DrawCircle((int)(x-8),(int)(580-h*.38f),(float)(h*.25f),c);
        }
    }
    /* fireflies / drifting motes */
    for(int i=0;i<26;i++) { float x=fmodf(i*73.0f+time*(9+(i%4)*2)-camX*.08f,SW); float y=235+(i*67%280)+sinf(time*1.4f+i)*9; DrawCircle((int)x,(int)y,1.6f,(Color){255,220,135,145}); }
}
static void DrawPlatforms(float time) {
    for(int i=0;i<PLATFORM_COUNT;i++) {
        Rectangle r=platforms[i].r;
        DrawRectangleRec((Rectangle){r.x,r.y+10,r.width,r.height-10},(Color){65,55,49,255});
        DrawRectangleRec((Rectangle){r.x,r.y+20,r.width,r.height-20},(Color){77,61,51,255});
        DrawRectangleRec((Rectangle){r.x,r.y,r.width,12},(Color){54,117,80,255});
        DrawRectangleRec((Rectangle){r.x,r.y,r.width,4},(Color){120,166,100,255});
        for(float x=r.x+18;x<r.x+r.width-10;x+=39) {
            int seed=((int)x*13+i*7)%17;
            DrawLine((int)x,(int)r.y+14,(int)x-3-seed%5,(int)r.y+25+seed,(Color){48,87,65,210});
        }
        if(r.height<40) { DrawLine((int)r.x+8,(int)r.y+15,(int)r.x+28,(int)r.y+19,(Color){150,127,91,160}); }
    }
    /* old stone arch on the horizon path */
    DrawRectangle(2380,444,26,166,(Color){76,92,82,255}); DrawRectangle(2514,444,26,166,(Color){76,92,82,255});
    DrawRectangle(2365,426,164,25,(Color){94,111,95,255});
    DrawRectangle(2401,454,104,11,(Color){35,66,65,255});
    for(int i=0;i<5;i++) DrawCircle(2393+i*28,438,3,(Color){215,188,117,210});
    /* luminous ancient marker */
    float pulse=0.5f+0.5f*sinf(time*2.1f);
    DrawCircleGradient(2700,558,52,(Color){106,211,192,(unsigned char)(35+35*pulse)},(Color){106,211,192,0});
    DrawRectangle(2687,535,26,48,(Color){77,111,100,255});
    DrawTriangle((Vector2){2682,535},(Vector2){2700,513},(Vector2){2718,535},(Color){116,154,130,255});
    DrawCircle(2700,541,4,(Color){173,240,207,255});
}
static void DrawPlayer(const Player *p) {
    float cx=p->pos.x+PLAYER_W*.5f, y=p->pos.y;
    float bob=(p->grounded && fabsf(p->vel.x)>20)?sinf(p->anim*2.0f)*2.0f:0;
    float squash=p->landSquash>0?1.0f+p->landSquash*1.5f:1.0f;
    DrawEllipse((int)cx,(int)(p->pos.y+PLAYER_H+2),18,5,(Color){8,24,28,95});
    /* scarf silhouette and wind response */
    float scarf=(p->facing>0?-1:1)*(5+fminf(fabsf(p->vel.x)*.035f,10));
    DrawTriangle((Vector2){cx-4,y+18+bob},(Vector2){cx+scarf,y+29+bob},(Vector2){cx-5,y+34+bob},(Color){153,62,66,255});
    DrawTriangle((Vector2){cx-5,y+23+bob},(Vector2){cx+scarf*1.35f,y+38+bob},(Vector2){cx-1,y+35+bob},(Color){201,83,79,255});
    /* legs */
    float stride=p->grounded?sinf(p->anim*2)*4:0;
    DrawRectangle((int)(cx-9),(int)(y+39+bob),7,14,(Color){31,47,53,255});
    DrawRectangle((int)(cx+2),(int)(y+39+bob),7,14,(Color){31,47,53,255});
    DrawRectangle((int)(cx-10+stride),(int)(y+49+bob),10,5,(Color){21,32,37,255});
    DrawRectangle((int)(cx+1-stride),(int)(y+49+bob),10,5,(Color){21,32,37,255});
    /* coat and shoulder */
    DrawRectangle((int)(cx-11),(int)(y+19+bob),22,(int)(21*squash),(Color){42,78,88,255});
    DrawRectangle((int)(cx-11),(int)(y+34+bob),22,5,(Color){226,174,99,255});
    DrawRectangle((int)(cx-13),(int)(y+21+bob),5,14,(Color){31,59,69,255});
    /* face, hair and directional eye */
    DrawRectangle((int)(cx-9),(int)(y+5+bob),18,16,(Color){225,181,142,255});
    DrawRectangle((int)(cx-11),(int)(y+3+bob),22,8,(Color){25,31,39,255});
    DrawTriangle((Vector2){cx-11,y+8+bob},(Vector2){cx-17,y+14+bob},(Vector2){cx-8,y+13+bob},(Color){25,31,39,255});
    DrawTriangle((Vector2){cx-3,y+4+bob},(Vector2){cx+4,y-2+bob},(Vector2){cx+5,y+6+bob},(Color){25,31,39,255});
    DrawTriangle((Vector2){cx+5,y+5+bob},(Vector2){cx+13,y+2+bob},(Vector2){cx+9,y+11+bob},(Color){25,31,39,255});
    DrawCircle((int)(cx+p->facing*4),(int)(y+14+bob),1.7f,(Color){27,38,42,255});
    DrawLine((int)(cx+p->facing*2),(int)(y+17+bob),(int)(cx+p->facing*5),(int)(y+17+bob),(Color){152,94,78,255});
}
static void DrawHUD(const Player *p,bool found,bool nearby,float time) {
    DrawRectangleGradientV(0,0,SW,94,(Color){13,28,37,225},(Color){13,28,37,0});
    DrawText("PALM ISLAND",28,20,22,(Color){244,220,166,255});
    DrawText("FIELD NOTES  /  THE FORGOTTEN COAST",30,48,12,(Color){167,197,187,255});
    DrawLine(28,70,245,70,(Color){211,176,105,150});
    DrawText("A",SW-176,23,18,(Color){246,215,158,255}); DrawText("MOVE",SW-150,27,12,MIST);
    DrawText("SPACE",SW-176,48,18,(Color){246,215,158,255}); DrawText("JUMP",SW-118,52,12,MIST);
    if(nearby&&!found) {
        DrawRectangleRounded((Rectangle){SW/2-205,SH-105,410,62},.18f,8,(Color){12,27,34,220});
        DrawRectangleRoundedLines((Rectangle){SW/2-205,SH-105,410,62},.18f,8,(Color){227,194,123,210});
        DrawText("E",SW/2-175,SH-85,24,GOLD_ACCENT); DrawText("EXAMINE THE ANCIENT MARK",SW/2-137,SH-82,16,MIST);
    }
    if(found) {
        DrawRectangleRounded((Rectangle){SW/2-235,116,470,84},.12f,8,(Color){12,27,34,225});
        DrawText("A MARK THAT REMEMBERS",SW/2-205,132,17,GOLD_ACCENT);
        DrawText("Five lines point inward. One has been erased.",SW/2-205,160,15,MIST);
        DrawText("FIELD NOTE ADDED",SW/2-205,181,11,(Color){146,210,183,255});
    }
    DrawRectangle(0,SH-24,SW,24,(Color){12,27,34,150});
    DrawText("THE ISLAND IS QUIET. THAT DOES NOT MEAN IT IS EMPTY.",28,SH-19,11,(Color){196,211,195,220});
    char coords[64]; snprintf(coords,sizeof(coords),"COAST  /  %04d m",(int)(p->pos.x/10)); DrawText(coords,SW-155,SH-19,11,(Color){196,211,195,220});
    (void)time;
}
int main(void) {
    SetConfigFlags(FLAG_VSYNC_HINT|FLAG_MSAA_4X_HINT);
    InitWindow(SW,SH,"Palm Island | The Forgotten Coast"); SetTargetFPS(120);
    Player player={0}; ResetPlayer(&player);
    Camera2D camera={0}; camera.offset=(Vector2){SW*.5f,SH*.5f}; camera.target=(Vector2){SW*.5f,SH*.5f}; camera.zoom=1;
    bool clueFound=false; float worldTime=0; float cameraY=SH*.5f;
    while(!WindowShouldClose()) {
        float dt=fminf(GetFrameTime(),1.0f/30.0f); worldTime+=dt;
        if(IsKeyPressed(KEY_R)) { ResetPlayer(&player); clueFound=false; }
        UpdatePlayer(&player,dt);
        float targetX=player.pos.x+PLAYER_W*.5f;
        targetX=ClampFloat(targetX,SW*.5f,WORLD_W-SW*.5f);
        camera.target.x += (targetX-camera.target.x)*(1-expf(-5.5f*dt));
        float targetY=SH*.5f+(player.pos.y-(FLOOR_Y-PLAYER_H))*.22f;
        cameraY += (targetY-cameraY)*(1-expf(-3.0f*dt)); camera.target.y=cameraY;
        float markerX=2700, markerY=535;
        float dx=(player.pos.x+PLAYER_W*.5f)-markerX, dy=(player.pos.y+PLAYER_H*.5f)-markerY;
        bool nearby=(dx*dx+dy*dy)<115.0f*115.0f;
        if(nearby && !clueFound && (IsKeyPressed(KEY_E)||IsGamepadButtonPressed(0,GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))) clueFound=true;
        BeginDrawing();
        DrawBackground(camera.target.x,worldTime);
        BeginMode2D(camera);
        DrawPlatforms(worldTime);
        /* foreground fronds create depth without obscuring the play lane */
        for(int i=0;i<16;i++) { float x=i*360.0f+80; DrawTriangle((Vector2){x,FLOOR_Y+55},(Vector2){x+28,FLOOR_Y-45-(i%3)*18},(Vector2){x+55,FLOOR_Y+55},(Color){24,65,57,190}); }
        DrawPlayer(&player);
        EndMode2D();
        DrawHUD(&player,clueFound,nearby,worldTime);
        EndDrawing();
    }
    CloseWindow(); return 0;
}
