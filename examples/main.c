#include "raylib.h"
#define RLW_IMPLEMENTATION
#include "rlw.h"

int main(void) {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 500, "RLW layout demo");
    SetTargetFPS(60);
    RLW_Context ctx = {0};
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        RLW_Begin(&ctx, (Rectangle){20, 20, GetScreenWidth()-40, GetScreenHeight()-40});
        RLW_PushVertical(&ctx, 3, 10);
            DrawRectangleRec(RLW_Widget(&ctx), RED);
            RLW_PushHorizontal(&ctx, 4, 8);
				DrawRectangleRec(RLW_WidgetSpan(&ctx, 2), BLUE);
                DrawRectangleRec(RLW_Widget(&ctx), GREEN);
                DrawRectangleRec(RLW_Widget(&ctx), ORANGE);
            RLW_Pop(&ctx);
            DrawRectangleRec(RLW_Widget(&ctx), GOLD);
        RLW_Pop(&ctx);
        RLW_End(&ctx);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

