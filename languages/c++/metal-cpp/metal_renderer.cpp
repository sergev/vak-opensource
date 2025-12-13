#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <cstddef> // for offsetof
#include <iostream>

struct Vertex {
    float position[3];
    float color[3];
};

class MetalRenderer {
private:
    MTL::Device *device;
    MTL::CommandQueue *commandQueue;
    MTL::RenderPipelineState *pipelineState;
    MTL::Buffer *vertexBuffer;
    CA::MetalLayer *metalLayer;

public:
    MetalRenderer(CA::MetalLayer *layer) : metalLayer(layer)
    {
        NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();
        // Create Metal device
        device = MTL::CreateSystemDefaultDevice();
        if (!device) {
            std::cerr << "Failed to create Metal device" << std::endl;
            exit(1);
        }

        // Create command queue
        commandQueue = device->newCommandQueue();

        // Load shader source (embedded as string literal)
        const char *shaderSourceCode = R"(
#include <metal_stdlib>
using namespace metal;

struct VertexIn {
    float3 position [[attribute(0)]];
    float3 color [[attribute(1)]];
};

struct VertexOut {
    float4 position [[position]];
    float3 color;
};

vertex VertexOut vertex_main(VertexIn in [[stage_in]])
{
    VertexOut out;
    out.position = float4(in.position, 1.0);
    out.color = in.color;
    return out;
}

fragment float4 fragment_main(VertexOut in [[stage_in]])
{
    return float4(in.color, 1.0);
}
)";

        NS::String *shaderSource = NS::String::string(shaderSourceCode, NS::ASCIIStringEncoding);
        // shaderSource is autoreleased, don't manually release

        // Create library from source
        NS::Error *error      = nullptr;
        MTL::Library *library = device->newLibrary(shaderSource, nullptr, &error);
        if (!library) {
            std::cerr << "Failed to create library" << std::endl;
            if (error) {
                std::cerr << "Error code: " << error->code() << std::endl;
            }
            exit(1);
        }

        // Get vertex and fragment functions
        NS::String *vertexName   = NS::String::string("vertex_main", NS::ASCIIStringEncoding);
        NS::String *fragmentName = NS::String::string("fragment_main", NS::ASCIIStringEncoding);
        // vertexName and fragmentName are autoreleased, don't manually release

        MTL::Function *vertexFunction   = library->newFunction(vertexName);
        MTL::Function *fragmentFunction = library->newFunction(fragmentName);

        if (!vertexFunction || !fragmentFunction) {
            std::cerr << "Failed to find shader functions" << std::endl;
            exit(1);
        }

        // Create vertex descriptor to match our Vertex structure
        MTL::VertexDescriptor *vertexDescriptor = MTL::VertexDescriptor::alloc()->init();

        // Vertex buffer layout: stride = sizeof(Vertex) = 24 bytes (3 floats + 3 floats)
        MTL::VertexBufferLayoutDescriptor *layout = vertexDescriptor->layouts()->object(0);
        layout->setStride(sizeof(Vertex));
        layout->setStepFunction(MTL::VertexStepFunctionPerVertex);
        layout->setStepRate(1);

        // Attribute 0: position (float3 at offset 0)
        MTL::VertexAttributeDescriptor *positionAttr = vertexDescriptor->attributes()->object(0);
        positionAttr->setFormat(MTL::VertexFormatFloat3);
        positionAttr->setOffset(0);
        positionAttr->setBufferIndex(0);

        // Attribute 1: color (float3 at offset 12)
        MTL::VertexAttributeDescriptor *colorAttr = vertexDescriptor->attributes()->object(1);
        colorAttr->setFormat(MTL::VertexFormatFloat3);
        colorAttr->setOffset(offsetof(Vertex, color));
        colorAttr->setBufferIndex(0);

        // Create render pipeline descriptor
        MTL::RenderPipelineDescriptor *pipelineDescriptor =
            MTL::RenderPipelineDescriptor::alloc()->init();
        pipelineDescriptor->setVertexFunction(vertexFunction);
        pipelineDescriptor->setFragmentFunction(fragmentFunction);
        pipelineDescriptor->setVertexDescriptor(vertexDescriptor);
        pipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(
            MTL::PixelFormatBGRA8Unorm);

        // Create render pipeline state
        pipelineState = device->newRenderPipelineState(pipelineDescriptor, &error);
        if (!pipelineState) {
            std::cerr << "Failed to create pipeline state" << std::endl;
            if (error) {
                std::cerr << "Error code: " << error->code() << std::endl;
                NS::String *errorDesc = error->localizedDescription();
                if (errorDesc) {
                    const char *errorStr = errorDesc->utf8String();
                    std::cerr << "Error: " << errorStr << std::endl;
                }
            }
            exit(1);
        }

        // Create vertex buffer with triangle data
        Vertex vertices[] = {
            { { 0.0f, 0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f } },   // Top vertex - Red
            { { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f } }, // Bottom left - Green
            { { 0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f } }   // Bottom right - Blue
        };

        vertexBuffer =
            device->newBuffer(vertices, sizeof(vertices), MTL::ResourceStorageModeShared);

        // Configure Metal layer
        metalLayer->setDevice(device);
        metalLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);

        // Cleanup - only release objects we own (created with alloc/new/copy/Create)
        // Objects created with factory methods (like string()) are autoreleased
        vertexFunction->release();     // newFunction returns owned object
        fragmentFunction->release();   // newFunction returns owned object
        vertexDescriptor->release();   // alloc()->init() returns owned object
        pipelineDescriptor->release(); // alloc()->init() returns owned object
        library->release();            // newLibrary returns owned object
        // shaderSource, vertexName, fragmentName are autoreleased - don't release
        pool->release();
    }

    ~MetalRenderer()
    {
        if (vertexBuffer)
            vertexBuffer->release();
        if (pipelineState)
            pipelineState->release();
        if (commandQueue)
            commandQueue->release();
        if (device)
            device->release();
    }

    void render()
    {
        NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();
        // Get drawable
        CA::MetalDrawable *drawable = metalLayer->nextDrawable();
        if (!drawable) {
            return;
        }

        // Create render pass descriptor
        MTL::RenderPassDescriptor *renderPassDescriptor =
            MTL::RenderPassDescriptor::alloc()->init();
        MTL::RenderPassColorAttachmentDescriptor *colorAttachment =
            renderPassDescriptor->colorAttachments()->object(0);
        colorAttachment->setTexture(drawable->texture());
        colorAttachment->setLoadAction(MTL::LoadActionClear);
        colorAttachment->setClearColor(MTL::ClearColor::Make(0.2, 0.2, 0.2, 1.0));
        colorAttachment->setStoreAction(MTL::StoreActionStore);

        // Create command buffer
        MTL::CommandBuffer *commandBuffer = commandQueue->commandBuffer();

        // Create render command encoder
        MTL::RenderCommandEncoder *renderEncoder =
            commandBuffer->renderCommandEncoder(renderPassDescriptor);
        renderEncoder->setRenderPipelineState(pipelineState);
        renderEncoder->setVertexBuffer(vertexBuffer, 0, 0);
        renderEncoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));
        renderEncoder->endEncoding();

        // Present drawable
        commandBuffer->presentDrawable(drawable);
        commandBuffer->commit();

        // Cleanup
        renderPassDescriptor->release();
        pool->release();
    }
};

// Interface functions
// Create CA::MetalLayer
// Returns: CA::MetalLayer* (as void*)
void *createMetalLayer()
{
    CA::MetalLayer *metalLayer = CA::MetalLayer::layer();
    metalLayer->retain();
    return metalLayer;
}

// Create renderer with CA::MetalLayer
void *createMetalRenderer(void *metalLayerPtr)
{
    CA::MetalLayer *metalLayer = (CA::MetalLayer *)metalLayerPtr;
    return new MetalRenderer(metalLayer);
}

void destroyMetalRenderer(void *renderer)
{
    delete (MetalRenderer *)renderer;
}

void renderMetalRenderer(void *renderer)
{
    ((MetalRenderer *)renderer)->render();
}

// Set drawable size on the layer
void setMetalLayerDrawableSize(void *metalLayerPtr, double width, double height)
{
    CA::MetalLayer *metalLayer = (CA::MetalLayer *)metalLayerPtr;
    CGSize size                = { (CGFloat)width, (CGFloat)height };
    metalLayer->setDrawableSize(size);
}

// Release CA::MetalLayer
void releaseMetalLayer(void *metalLayerPtr)
{
    CA::MetalLayer *metalLayer = (CA::MetalLayer *)metalLayerPtr;
    metalLayer->release();
}
