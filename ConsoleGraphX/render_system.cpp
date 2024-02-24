#include "render_system.h"
#include <iostream>

/*
Parallelization: If you have a multi-core CPU, you could explore parallelizing the rendering process.
For example, you could divide the screen into smaller regions and assign a separate thread 
to copy pixels for each region. This can improve performance, especially if you have a lot of sprites to render.
*/

void RenderSystem::DrawSprites(const std::vector<Entity*>& entities)
{

    for (Entity* entity : entities)
    {
        Sprite* sprite = entity->GetComponent<Sprite>();
        const Vector3 position = entity->GetWorldPosition();

        if (sprite == NULL)
            throw new std::runtime_error("entity does not have the sprite component");

        if (sprite->IsTransparent())
            RenderSystem::DrawSprite_SP(position, sprite);
        else
            RenderSystem::DrawSprite_SS(position, sprite);
    }
}



void RenderSystem::DrawSprite_SS(const Vector3 position, Sprite* sprite)
{
    if (!sprite)
        return;

    CHAR_INFO* buffer = Screen::GetActiveScreenBuffer_A();
    CHAR_INFO* pixels = sprite->GetPixels();

    const int spriteWidth = sprite->GetWidth();
    const int spriteHeight = sprite->GetHeight();
    const int screenWidth = Screen::GetWidth_A();

    int buffer_offset = position.x + position.y * screenWidth;
    // how much the sprite is off the screen in positive x
    int overlap_px = std::max<int>(0, position.x + spriteWidth - screenWidth);
    // how much the sprite is off the screen in negative x
    int overlap_nx = std::max<int>(0, 0 - position.x);

    // if its off the screen return
    if (position.x > screenWidth || position.x + spriteWidth < 0)
        return;


    int sprite_index_offset = 0;
    for (int y = 0; y < spriteHeight; y++)
    {
        // sprite is the source, the buffer is the destination
        CHAR_INFO* src_start = pixels + sprite_index_offset;
        CHAR_INFO* src_end = pixels + sprite_index_offset + spriteWidth - overlap_px - overlap_nx;
        CHAR_INFO* dest = buffer + buffer_offset + overlap_nx;

        Screen::SetPixels_A(src_start, src_end, dest);

        sprite_index_offset += spriteWidth;
        buffer_offset += screenWidth;
    }
}

void RenderSystem::DrawSprite_SP(const Vector3 position, Sprite* sprite)
{
    if (!sprite)
        return;
  
    CHAR_INFO* pixels = sprite->GetPixels();

    const int spriteWidth = sprite->GetWidth();
    const int spriteHeight = sprite->GetHeight();

    for (int y = 0; y < spriteHeight; y++)
    {
        for (int x = 0; x < spriteWidth; x++)
        {
            Screen::SetPixel_A(position.x + x, position.y + y, *pixels++);
        }
    }
}

void RenderSystem::DrawSprites_SS(const std::vector<Entity*>& entities)
{
    for (Entity* entity : entities)
    {
        Sprite* sprite = entity->GetComponent<Sprite>();
        const Vector3 position = entity->GetWorldPosition();

        RenderSystem::DrawSprite_SS(position, sprite);
    }
   
}

void RenderSystem::DrawSprites_SP(const std::vector<Entity*>& entities)
{
    for (Entity* entity : entities)
    {
        Sprite* sprite = entity->GetComponent<Sprite>();
        const Vector3 position = entity->GetWorldPosition();

        RenderSystem::DrawSprite_SP(position, sprite);
    }
}

void RenderSystem::DrawLine(Vector2 origin, Vector2 end, int color)
{
    CHAR_INFO pixel{ Screen::pixel, color };

    int dx = end.x - origin.x;
    int dy = end.y - origin.y;

    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    float xIncrement = static_cast<float>(dx) / steps;
    float yIncrement = static_cast<float>(dy) / steps;

    float x = origin.x;
    float y = origin.y;

    for (int i = 0; i <= steps; i++) {
        int ix = static_cast<int>(x);
        int iy = static_cast<int>(y);
        
        Screen::SetPixel_A(ix, iy, pixel);
        
        x += xIncrement;
        y += yIncrement;
    }
}



