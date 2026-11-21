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

void RLW_Begin(RLW_Context *ctx, Rectangle bounds);
void RLW_End(RLW_Context *ctx);
void RLW_PushHorizontal(RLW_Context *ctx, int columns, float padding);
void RLW_PushVertical(RLW_Context *ctx, int rows, float padding);
void RLW_Pop(RLW_Context *ctx);
Rectangle RLW_Widget(RLW_Context *ctx);
Rectangle RLW_WidgetSpan(RLW_Context *ctx, int span);
Rectangle RLW_Peek(RLW_Context *ctx);
Rectangle RLW_PeekSpan(RLW_Context *ctx, int span);
void RLW_Skip(RLW_Context *ctx, int slots);
int RLW_Remaining(const RLW_Context *ctx);
void RLW_Reset(RLW_Context *ctx);
Rectangle RLW_Inset(Rectangle rect, float amount);

#define RLW_WidgetREC(rec, ctx) \
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
#include <assert.h>

static RLW_Layout *RLW__current(RLW_Context *ctx) {
    assert(ctx && ctx->begun && ctx->stack_count > 0);
    return &ctx->stack[ctx->stack_count - 1];
}

static Rectangle RLW__slot(const RLW_Layout *layout, int index, int span) {
    assert(layout && layout->count > 0);
    assert(index >= 0 && span > 0 && span <= layout->count - index);
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

void RLW_Begin(RLW_Context *ctx, Rectangle bounds) {
    assert(ctx && !ctx->begun);
    assert(bounds.width >= 0 && bounds.height >= 0);
    ctx->bounds = bounds;
    ctx->stack_count = 0;
    ctx->begun = true;
}

void RLW_End(RLW_Context *ctx) {
    assert(ctx && ctx->begun && ctx->stack_count == 0);
    ctx->begun = false;
}

static void RLW__push(RLW_Context *ctx, RLW_Direction direction, int count, float padding) {
    assert(ctx && ctx->begun);
    assert(count > 0 && padding >= 0);
    assert(ctx->stack_count < RLW_MAX_STACK);
    Rectangle bounds = ctx->stack_count ? RLW_Widget(ctx) : ctx->bounds;
    RLW_Layout *layout = &ctx->stack[ctx->stack_count++];
    *layout = (RLW_Layout){ .bounds = bounds, .direction = direction,
                            .count = count, .index = 0, .padding = padding };
}

void RLW_PushHorizontal(RLW_Context *ctx, int columns, float padding) {
    RLW__push(ctx, RLW_HORIZONTAL, columns, padding);
}

void RLW_PushVertical(RLW_Context *ctx, int rows, float padding) {
    RLW__push(ctx, RLW_VERTICAL, rows, padding);
}

void RLW_Pop(RLW_Context *ctx) {
    assert(ctx && ctx->begun && ctx->stack_count > 0);
    --ctx->stack_count;
}

Rectangle RLW_PeekSpan(RLW_Context *ctx, int span) {
    RLW_Layout *layout = RLW__current(ctx);
    return RLW__slot(layout, layout->index, span);
}

Rectangle RLW_Peek(RLW_Context *ctx) {
    return RLW_PeekSpan(ctx, 1);
}

Rectangle RLW_WidgetSpan(RLW_Context *ctx, int span) {
    Rectangle rect = RLW_PeekSpan(ctx, span);
    RLW__current(ctx)->index += span;
    return rect;
}

Rectangle RLW_Widget(RLW_Context *ctx) {
    return RLW_WidgetSpan(ctx, 1);
}

void RLW_Skip(RLW_Context *ctx, int slots) {
    RLW_Layout *layout = RLW__current(ctx);
    assert(slots >= 0 && slots <= layout->count - layout->index);
    layout->index += slots;
}

int RLW_Remaining(const RLW_Context *ctx) {
    assert(ctx && ctx->begun && ctx->stack_count > 0);
    const RLW_Layout *layout = &ctx->stack[ctx->stack_count - 1];
    return layout->count - layout->index;
}

void RLW_Reset(RLW_Context *ctx) {
    RLW__current(ctx)->index = 0;
}

Rectangle RLW_Inset(Rectangle rect, float amount) {
    assert(amount >= 0);
    rect.x += amount;
    rect.y += amount;
    rect.width -= 2 * amount;
    rect.height -= 2 * amount;
    if (rect.width < 0) rect.width = 0;
    if (rect.height < 0) rect.height = 0;
    return rect;
}

#endif /* RLW_IMPLEMENTATION_INCLUDED */
#endif /* RLW_IMPLEMENTATION */
