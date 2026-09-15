// Minimal headless raylib stub for unit testing (no graphics/audio).
#pragma once
#include <string>
#include <vector>
#include <map>
#include <cstdarg>
#include <cstdio>

typedef struct Color { unsigned char r, g, b, a; } Color;
typedef struct Vector2 { float x, y; } Vector2;
typedef struct Rectangle { float x, y, width, height; } Rectangle;
typedef struct Image { void* data; int width, height, mipmaps, format; } Image;
typedef struct Texture2D { unsigned int id; int width, height, mipmaps, format; } Texture2D;
typedef struct Sound { int id; } Sound;

#define BLACK Color{0,0,0,255}
enum { KEY_UP = 265, KEY_DOWN = 264, KEY_LEFT = 263, KEY_RIGHT = 262 };

// ---- test-controllable state ----
namespace stub {
    extern double time;
    extern double timeStep;                 // added to `time` after every frame (EndDrawing)
    extern std::vector<int> randomValues;   // consumed FIFO by GetRandomValue
    extern std::map<int,bool> keys;
    extern int framesUntilClose;
    extern int drawCalls, textureDraws, textDraws, soundPlays, rectDraws;
    extern int textureLoads, textureUnloads;
    void reset();
}

inline void InitWindow(int, int, const char*) {}
inline void CloseWindow() {}
inline void SetTargetFPS(int) {}
inline bool WindowShouldClose() { return stub::framesUntilClose-- <= 0; }
inline void InitAudioDevice() {}
inline void CloseAudioDevice() {}
inline Sound LoadSound(const char*) { return Sound{1}; }
inline void PlaySound(Sound) { stub::soundPlays++; }
inline double GetTime() { return stub::time; }
inline int GetRandomValue(int lo, int hi) {
    // When the scripted queue is exhausted, walk through the range instead of
    // always returning lo: a fixed (0,0) sits on the default snake body and makes
    // Food::grp()'s retry loop spin forever.
    if (stub::randomValues.empty()) { static int n = 0; return lo + (n++ % (hi - lo + 1)); }
    int v = stub::randomValues.front(); stub::randomValues.erase(stub::randomValues.begin()); return v;
}
inline bool IsKeyPressed(int k) { auto it = stub::keys.find(k); return it != stub::keys.end() && it->second; }
inline Image LoadImage(const char*) { return Image{nullptr, 30, 30, 1, 0}; }
inline Texture2D LoadTextureFromImage(Image img) { stub::textureLoads++; return Texture2D{1, img.width, img.height, 1, 0}; }
inline void UnloadImage(Image) {}
inline void UnloadTexture(Texture2D) { stub::textureUnloads++; }
inline void BeginDrawing() {}
inline void EndDrawing() { stub::time += stub::timeStep; }
inline void ClearBackground(Color) {}
inline void DrawTextureEx(Texture2D, Vector2, float, float, Color) { stub::textureDraws++; }
inline void DrawRectangleRounded(Rectangle, float, int, Color) { stub::rectDraws++; }
inline void DrawRectangleLinesEx(Rectangle, float, Color) {}
inline void DrawText(const char*, int, int, int, Color) { stub::textDraws++; }
inline const char* TextFormat(const char* fmt, ...) {
    static char buf[256]; va_list ap; va_start(ap, fmt); vsnprintf(buf, sizeof buf, fmt, ap); va_end(ap); return buf;
}
