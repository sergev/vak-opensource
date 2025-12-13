#ifndef METAL_RENDERER_H
#define METAL_RENDERER_H

// Interface functions for Metal rendering
// These functions provide a C++ interface to MetalRenderer functionality

// Create CA::MetalLayer
// Returns: CA::MetalLayer* (as void*)
void *createMetalLayer(void);

// Create renderer with CA::MetalLayer
// Returns: MetalRenderer* (as void*)
void *createMetalRenderer(void *layer);

// Destroy renderer
// renderer: MetalRenderer* (as void*)
void destroyMetalRenderer(void *renderer);

// Render a frame
// renderer: MetalRenderer* (as void*)
void renderMetalRenderer(void *renderer);

// Set drawable size on the layer
// layer: CA::MetalLayer* (as void*)
void setMetalLayerDrawableSize(void *layer, double width, double height);

// Release CA::MetalLayer
// layer: CA::MetalLayer* (as void*)
void releaseMetalLayer(void *layer);

#endif // METAL_RENDERER_H
