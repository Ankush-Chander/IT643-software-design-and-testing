#pragma once
#include "raylib.h"
inline Vector2 Vector2Add(Vector2 a, Vector2 b) { return Vector2{a.x + b.x, a.y + b.y}; }
inline bool Vector2Equals(Vector2 a, Vector2 b) { return a.x == b.x && a.y == b.y; }
