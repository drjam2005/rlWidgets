# RLW

**RLW** is a lightweight, single-header immediate-mode layout library for [raylib](https://www.raylib.com/), written in C.

It provides a stack-based layout system that assigns `Rectangle` values to widgets. RLW handles positioning; you decide how to draw and interact with each rectangle.

## Features

- Single-header library (`rlw.h`)
- Independent `RLW_Context` instances
- Nested horizontal and vertical layouts
- Equal-sized rows and columns with configurable gaps
- Slot spanning, peeking, skipping, and resetting
- Convenience macros for declaring widget rectangles
- No built-in widget rendering or global UI context

## Installation

Copy `rlw.h` into your project and define `RLW_IMPLEMENTATION` in **exactly one** `.c` file before including it:

```c
#define RLW_IMPLEMENTATION
#include "rlw.h"
```

In other source files, simply use:

```c
#include "rlw.h"
```

RLW requires raylib's `raylib.h` and types. Make sure your compiler can find raylib's headers and that your application links against raylib.

## Example

```c
#define RLW_IMPLEMENTATION
#include "rlw.h"

int main(void) {
    InitWindow(800, 500, "RLW example");
    SetTargetFPS(60);

    RLW_Context ctx = {0};

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        RLW_Begin(&ctx, (Rectangle){20, 20, 760, 460});
        RLW_PushVertical(&ctx, 3, 10);

            DrawRectangleRec(RLW_Widget(&ctx), RED);

            RLW_PushHorizontal(&ctx, 4, 8);
                RLW_WidgetSpanREC(wide, ctx, 2);
                RLW_WidgetREC(middle, ctx);
                RLW_WidgetREC(right, ctx);

                DrawRectangleRec(wide, BLUE);
                DrawRectangleRec(middle, GREEN);
                DrawRectangleRec(right, ORANGE);
            RLW_Pop(&ctx);

            DrawRectangleRec(RLW_Widget(&ctx), GOLD);

        RLW_Pop(&ctx);
        RLW_End(&ctx);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
```

On Linux with raylib available through `pkg-config`:

```sh
cc rlw_example.c -o rlw_example $(pkg-config --cflags --libs raylib) -lm
```

## How layouts work

- `RLW_Begin(&ctx, bounds)` sets the root rectangle for the current frame.
- The first `RLW_PushHorizontal` or `RLW_PushVertical` uses that root rectangle.
- Each nested push **consumes one slot** from its parent layout.
- `RLW_Widget` consumes one slot and returns its rectangle.
- `RLW_WidgetSpan` consumes multiple consecutive slots along the current layout direction.
- The `padding` argument to push functions is the **gap between slots**, not outer padding.
- Call `RLW_Pop` for each push before `RLW_End`.

For example, a horizontal layout with four columns can allocate a double-width widget followed by two regular widgets:

```c
RLW_PushHorizontal(&ctx, 4, 8);
Rectangle wide = RLW_WidgetSpan(&ctx, 2);
Rectangle left = RLW_Widget(&ctx);
Rectangle right = RLW_Widget(&ctx);
RLW_Pop(&ctx);
```

## Rectangle declaration macros

Two convenience macros declare a `Rectangle` and assign the next available slot:

```c
RLW_WidgetREC(name, ctx);
RLW_WidgetSpanREC(name, ctx, span);
```

For example:

```c
RLW_Context ctx = {0};
RLW_Begin(&ctx, (Rectangle){20, 20, 600, 200});
RLW_PushHorizontal(&ctx, 4, 8);

    RLW_WidgetREC(left, ctx);          // 1 column
    RLW_WidgetSpanREC(center, ctx, 2); // 2 columns
    RLW_WidgetREC(right, ctx);         // 1 column

    DrawRectangleRec(left, RED);
    DrawRectangleRec(center, BLUE);
    DrawRectangleRec(right, GREEN);

RLW_Pop(&ctx);
RLW_End(&ctx);
```

These are equivalent to:

```c
Rectangle left = RLW_Widget(&ctx);
Rectangle center = RLW_WidgetSpan(&ctx, 2);
Rectangle right = RLW_Widget(&ctx);
```

**Important:** Pass the context **variable** (`ctx`), not `&ctx`, to these macros. They take its address internally. Use them where C allows a variable declaration, and include the trailing semicolon at the call site. If you already have a context pointer, call `RLW_Widget(ptr)` or `RLW_WidgetSpan(ptr, span)` directly.

## API reference

| Function | Description |
| --- | --- |
| `RLW_Begin(ctx, bounds)` | Begin a layout frame using a root rectangle |
| `RLW_End(ctx)` | End the frame; requires an empty layout stack |
| `RLW_PushHorizontal(ctx, columns, padding)` | Push an equal-width column layout |
| `RLW_PushVertical(ctx, rows, padding)` | Push an equal-height row layout |
| `RLW_Pop(ctx)` | Pop the current layout |
| `RLW_Widget(ctx)` | Consume one slot and return its rectangle |
| `RLW_WidgetSpan(ctx, span)` | Consume and return multiple adjacent slots |
| `RLW_Peek(ctx)` | Return the next slot without consuming it |
| `RLW_PeekSpan(ctx, span)` | Preview a span without consuming slots |
| `RLW_Skip(ctx, slots)` | Skip a number of slots |
| `RLW_Remaining(ctx)` | Return the number of unconsumed slots |
| `RLW_Reset(ctx)` | Reset the current layout's slot index |
| `RLW_Inset(rect, amount)` | Return a rectangle inset on all four sides |
| `RLW_WidgetREC(name, ctx)` | Declare a rectangle from the next slot (macro) |
| `RLW_WidgetSpanREC(name, ctx, span)` | Declare a rectangle spanning multiple slots (macro) |

## Notes

- Initialize each context with `RLW_Context ctx = {0};` before first use.
- You can create multiple contexts; each owns its own layout stack.
- Slot counts must be positive, gaps must be nonnegative, and allocations must not exceed the declared slot count. The implementation uses C `assert` checks for these conditions.
- RLW only calculates rectangles. It does not draw widgets, handle input, clip rendering, or perform automatic content sizing.
- The maximum nesting depth defaults to 64 and can be configured by defining `RLW_MAX_STACK` before including the header.

## License

Add your preferred license here.
