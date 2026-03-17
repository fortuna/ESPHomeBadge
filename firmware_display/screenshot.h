// Copyright 2025 Vinicius Fortuna
// SPDX-License-Identifier: Apache-2.0
#pragma once

// Saves the current SDL window framebuffer as a BMP file.
// Call from an ESPHome lambda: save_screenshot("/path/to/out.bmp");
#include <SDL.h>
#include <cstdio>

inline void save_screenshot(const char* path) {
    SDL_Window* win = SDL_GetWindowFromID(1);
    if (!win) { ESP_LOGE("screenshot", "No SDL window found"); return; }

    SDL_Renderer* ren = SDL_GetRenderer(win);
    if (!ren) { ESP_LOGE("screenshot", "No SDL renderer found"); return; }

    int w, h;
    SDL_GetWindowSize(win, &w, &h);

    SDL_Surface* surf = SDL_CreateRGBSurface(0, w, h, 32,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!surf) { ESP_LOGE("screenshot", "SDL_CreateRGBSurface failed: %s", SDL_GetError()); return; }

    if (SDL_RenderReadPixels(ren, NULL, surf->format->format, surf->pixels, surf->pitch) != 0) {
        ESP_LOGE("screenshot", "SDL_RenderReadPixels failed: %s", SDL_GetError());
        SDL_FreeSurface(surf);
        return;
    }
    if (SDL_SaveBMP(surf, path) != 0) {
        ESP_LOGE("screenshot", "SDL_SaveBMP failed: %s", SDL_GetError());
    } else {
        ESP_LOGI("screenshot", "Saved: %s", path);
    }
    SDL_FreeSurface(surf);
}
