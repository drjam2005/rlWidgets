#ifndef RLW_H
#define RLW_H

/* RLW: small immediate-mode rectangle layout stack for raylib.
 / Define RLW_IMPLEMENTATION in exactly one translation unit.
 / Padding in PushHorizontal/PushVertical is the gap BETWEEN slots.
 / The first push after Begin uses the root rectangle; nested pushes
 / consume one slot from their parent.
*/

#include "raylib.h"
#include <stdbool.h>

#ifndef RLW_MAX_STACK
#define RLW_MAX_STACK 64
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum RLW_Direction {
    RLW_HORIZONTAL,
    RLW_VERTICAL
} RLW_Direction;

typedef struct RLW_Layout {
    Rectangle bounds;
    RLW_Direction direction;
    int count;
    int index;
    float padding;
} RLW_Layout;

typedef struct RLW_Context {
    Rectangle bounds;
    RLW_Layout stack[RLW_MAX_STACK];
    int stack_count;
    bool begun;
} RLW_Context;

void RLW_Begin_Func(RLW_Context *ctx, Rectangle bounds, const char *filepath, const char *line);
void RLW_End_Func(RLW_Context *ctx, const char *filepath, const char *line);
void RLW_PushHorizontal_Func(RLW_Context *ctx, int columns, float padding, const char *filepath, const char *line);
void RLW_PushVertical_Func(RLW_Context *ctx, int rows, float padding, const char *filepath, const char *line);
void RLW_Pop_Func(RLW_Context *ctx, const char *filepath, const char *line);
Rectangle RLW_Widget_Func(RLW_Context *ctx, const char *filepath, const char *line);
Rectangle RLW_WidgetSpan_Func(RLW_Context *ctx, int span, const char *filepath, const char *line);
Rectangle RLW_Peek_Func(RLW_Context *ctx, const char *filepath, const char *line);
Rectangle RLW_PeekSpan_Func(RLW_Context *ctx, int span, const char *filepath, const char *line);
void RLW_Skip_Func(RLW_Context *ctx, int slots, const char *filepath, const char *line);
int RLW_Remaining_Func(const RLW_Context *ctx, const char *filepath, const char *line);
void RLW_Reset_Func(RLW_Context *ctx, const char *filepath, const char *line);
Rectangle RLW_Inset_Func(Rectangle rect, float amount, const char *filepath, const char *line);

#define RLW__STRINGIFY_IMPL(value) #value
#define RLW__STRINGIFY(value)      RLW__STRINGIFY_IMPL(value)

/* Variadic forwarding keeps C compound literals usable as arguments. */
#define RLW_Begin(...)           RLW_Begin_Func(__VA_ARGS__, __FILE__, RLW__STRINGIFY(__LINE__))
#define RLW_End(...)             RLW_End_Func(__VA_ARGS__, __FILE__, RLW__STRINGIFY(__LINE__))
#define RLW_PushHorizontal(...)  RLW_PushHorizontal_Func(__VA_ARGS__, __FILE__, RLW__STRINGIFY(__LINE__))
#define RLW_PushVertical(...)    RLW_PushVertical_Func(__VA_ARGS__, __FILE__, RLW__STRINGIFY(__LINE__))
#define RLW_Pop(...)             RLW_Pop_Func(__VA_ARGS__, __FILE__, RLW__STRINGIFY(__LINE__))
#define RLW_Widget(...)          RLW_Widget_Func(__VA_ARGS__, __FILE__, RLW__STRINGIFY(__LINE__))
#define RLW_WidgetSpan(...)      RLW_WidgetSpan_Func(__VA_ARGS__, __FILE__, RLW__STRINGIFY(__LINE__))
#define RLW_Peek(...)            RLW_Peek_Func(__VA_ARGS__, __FILE__, RLW__STRINGIFY(__LINE__))
#define RLW_PeekSpan(...)        RLW_PeekSpan_Func(__VA_ARGS__, __FILE__, RLW__STRINGIFY(__LINE__))
#define RLW_Skip(...)            RLW_Skip_Func(__VA_ARGS__, __FILE__, RLW__STRINGIFY(__LINE__))
#define RLW_Remaining(...)       RLW_Remaining_Func(__VA_ARGS__, __FILE__, RLW__STRINGIFY(__LINE__))
#define RLW_Reset(...)           RLW_Reset_Func(__VA_ARGS__, __FILE__, RLW__STRINGIFY(__LINE__))
#define RLW_Inset(...)           RLW_Inset_Func(__VA_ARGS__, __FILE__, RLW__STRINGIFY(__LINE__))

#define RLW_WidgetREC(rec, ctx)           \
    Rectangle rec = RLW_Widget((ctx))

#define RLW_WidgetSpanREC(rec, ctx, span) \
    Rectangle rec = RLW_WidgetSpan((ctx), (span))

#ifdef __cplusplus
}
#endif
#endif /* RLW_H */

#ifdef RLW_IMPLEMENTATION
#ifndef RLW_IMPLEMENTATION_INCLUDED
#define RLW_IMPLEMENTATION_INCLUDED
#ifndef NDEBUG
#include <stdio.h>
#include <stdlib.h>

#define RLW__ASSERT(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%s: RLW assertion failed: %s\n", filepath, line, #condition); \
        abort(); \
    } \
} while (0)
#else
#define RLW__ASSERT(condition) ((void)filepath, (void)line)
#endif

static RLW_Layout *RLW__current(RLW_Context *ctx, const char *filepath, const char *line) {
    RLW__ASSERT(ctx && ctx->begun && ctx->stack_count > 0);
    return &ctx->stack[ctx->stack_count - 1];
}

static Rectangle RLW__slot(const RLW_Layout *layout, int index, int span, const char *filepath, const char *line) {
    RLW__ASSERT(layout && layout->count > 0);
    RLW__ASSERT(index >= 0 && span > 0 && span <= layout->count - index);
    Rectangle rect = layout->bounds;
    if (layout->direction == RLW_HORIZONTAL) {
        float cell = (rect.width - layout->padding * (layout->count - 1)) / layout->count;
        rect.x += index * (cell + layout->padding);
        rect.width = cell * span + layout->padding * (span - 1);
    } else {
        float cell = (rect.height - layout->padding * (layout->count - 1)) / layout->count;
        rect.y += index * (cell + layout->padding);
        rect.height = cell * span + layout->padding * (span - 1);
    }
    return rect;
}

void RLW_Begin_Func(RLW_Context *ctx, Rectangle bounds, const char *filepath, const char *line) {
    RLW__ASSERT(ctx && !ctx->begun);
    RLW__ASSERT(bounds.width >= 0 && bounds.height >= 0);
    ctx->bounds = bounds;
    ctx->stack_count = 0;
    ctx->begun = true;
}

void RLW_End_Func(RLW_Context *ctx, const char *filepath, const char *line) {
    RLW__ASSERT(ctx && ctx->begun && ctx->stack_count == 0);
    ctx->begun = false;
}

static void RLW__push(RLW_Context *ctx, RLW_Direction direction, int count, float padding, const char *filepath, const char *line) {
    RLW__ASSERT(ctx && ctx->begun);
    RLW__ASSERT(count > 0 && padding >= 0);
    RLW__ASSERT(ctx->stack_count < RLW_MAX_STACK);
    Rectangle bounds = ctx->stack_count ? RLW_Widget_Func(ctx, filepath, line) : ctx->bounds;
    RLW_Layout *layout = &ctx->stack[ctx->stack_count++];
    *layout = (RLW_Layout){ .bounds = bounds, .direction = direction,
                            .count = count, .index = 0, .padding = padding };
}

void RLW_PushHorizontal_Func(RLW_Context *ctx, int columns, float padding, const char *filepath, const char *line) {
    RLW__push(ctx, RLW_HORIZONTAL, columns, padding, filepath, line);
}

void RLW_PushVertical_Func(RLW_Context *ctx, int rows, float padding, const char *filepath, const char *line) {
    RLW__push(ctx, RLW_VERTICAL, rows, padding, filepath, line);
}

void RLW_Pop_Func(RLW_Context *ctx, const char *filepath, const char *line) {
    RLW__ASSERT(ctx && ctx->begun && ctx->stack_count > 0);
    --ctx->stack_count;
}

Rectangle RLW_PeekSpan_Func(RLW_Context *ctx, int span, const char *filepath, const char *line) {
    RLW_Layout *layout = RLW__current(ctx, filepath, line);
    return RLW__slot(layout, layout->index, span, filepath, line);
}

Rectangle RLW_Peek_Func(RLW_Context *ctx, const char *filepath, const char *line) {
    return RLW_PeekSpan_Func(ctx, 1, filepath, line);
}

Rectangle RLW_WidgetSpan_Func(RLW_Context *ctx, int span, const char *filepath, const char *line) {
    Rectangle rect = RLW_PeekSpan_Func(ctx, span, filepath, line);
    RLW__current(ctx, filepath, line)->index += span;
    return rect;
}

Rectangle RLW_Widget_Func(RLW_Context *ctx, const char *filepath, const char *line) {
    return RLW_WidgetSpan_Func(ctx, 1, filepath, line);
}

void RLW_Skip_Func(RLW_Context *ctx, int slots, const char *filepath, const char *line) {
    RLW_Layout *layout = RLW__current(ctx, filepath, line);
    RLW__ASSERT(slots >= 0 && slots <= layout->count - layout->index);
    layout->index += slots;
}

int RLW_Remaining_Func(const RLW_Context *ctx, const char *filepath, const char *line) {
    RLW__ASSERT(ctx && ctx->begun && ctx->stack_count > 0);
    const RLW_Layout *layout = &ctx->stack[ctx->stack_count - 1];
    return layout->count - layout->index;
}

void RLW_Reset_Func(RLW_Context *ctx, const char *filepath, const char *line) {
    RLW__current(ctx, filepath, line)->index = 0;
}

Rectangle RLW_Inset_Func(Rectangle rect, float amount, const char *filepath, const char *line) {
    RLW__ASSERT(amount >= 0);
    rect.x += amount;
    rect.y += amount;
    rect.width -= 2 * amount;
    rect.height -= 2 * amount;
    if (rect.width < 0) rect.width = 0;
    if (rect.height < 0) rect.height = 0;
    return rect;
}

#undef RLW__ASSERT

#endif /* RLW_IMPLEMENTATION_INCLUDED */
#endif /* RLW_IMPLEMENTATION */
