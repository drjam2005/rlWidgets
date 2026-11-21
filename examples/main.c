#include "raylib.h"
#define RLW_IMPLEMENTATION
#include "rlWidgets.h"

int main(void) {
    InitWindow(800, 500, "RLW layout demo");
    SetTargetFPS(60);
    RLW_Context ctx = {0};
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        RLW_Begin(&ctx, (Rectangle){20, 20, 760, 460});
        RLW_PushVertical(&ctx, 3, 10);
            DrawRectangleRec(RLW_Widget(&ctx), RED);
            RLW_PushHorizontal(&ctx, 4, 8);
				RLW_WidgetSpanREC(rec, &ctx, 2);
				if(CheckCollisionPointRec(GetMousePosition(), rec))
					DrawRectangleRec(rec, ColorBrightness(BLUE, .2f));
				else
					DrawRectangleRec(rec, BLUE);
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

