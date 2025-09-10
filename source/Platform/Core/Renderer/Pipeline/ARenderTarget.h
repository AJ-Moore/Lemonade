#pragma once

#include <Platform/Core/Renderer/RenderBlock/ARenderBlock.h>
#include <Platform/Vulkan/WindowManager/LWindow.h>
#include <LCommon.h>

namespace Lemonade
{
    enum class LColourAttachment : uint
    {
        LEMON_COLOR_ATTACHMENT0  = 0x8CE0,
        LEMON_COLOR_ATTACHMENT1  = 0x8CE1,
        LEMON_COLOR_ATTACHMENT2  = 0x8CE2,
        LEMON_COLOR_ATTACHMENT3  = 0x8CE3,
        LEMON_COLOR_ATTACHMENT4  = 0x8CE4,
        LEMON_COLOR_ATTACHMENT5  = 0x8CE5,
        LEMON_COLOR_ATTACHMENT6  = 0x8CE6,
        LEMON_COLOR_ATTACHMENT7  = 0x8CE7,
        LEMON_COLOR_ATTACHMENT8  = 0x8CE8,
        LEMON_COLOR_ATTACHMENT9  = 0x8CE9,
        LEMON_COLOR_ATTACHMENT10 = 0x8CEA,
        LEMON_COLOR_ATTACHMENT11 = 0x8CEB,
        LEMON_COLOR_ATTACHMENT12 = 0x8CEC,
        LEMON_COLOR_ATTACHMENT13 = 0x8CED,
        LEMON_COLOR_ATTACHMENT14 = 0x8CEE,
        LEMON_COLOR_ATTACHMENT15 = 0x8CEF,
    };

    enum class LBufferBit : uint
    {
        COLOUR = 0x1,
        DEPTH = 0x2,
    };

    enum UFormat
    {
        U_RGBA32F = 0x8814
    };

    /* A-Abstract Render Target - Wraps framebuffer objects, uninitialised framebuffers will use framebuffer 0/ Screen*/
    class ARenderTarget
    {
    public:
        ARenderTarget(){}
        ARenderTarget(glm::ivec2 dimensions){m_dimensions = dimensions;}
        virtual ~ARenderTarget();
        virtual bool Init() = 0;
        virtual void InitAsDefault() { m_bDoneInit = true; }
        virtual void BindColourAttachments() = 0;
        virtual void BindColourAttachment(LColourAttachment colourAttachment, uint activeTarget = 0) = 0;
        virtual void bindDepthAttachment(uint activeTarget = 0) = 0;
        virtual void BeginRenderPass() = 0;
        virtual void EndRenderPass() = 0;

        virtual void blit(ARenderTarget& target) = 0;
        virtual void blit(unsigned int target) = 0;
        virtual void blitToScreen() = 0;
        virtual void setDimensions(glm::ivec2 dimensions) = 0;
        virtual void SetColourAttachments(const std::vector<LColourAttachment> attachments, bool multisampled = false) = 0;
        virtual void SetColourAttachments(int count, bool multisampled) = 0;
        virtual void AddDepthAttachment(bool useRenderBufferStorage = true, int layers = 1) = 0;
        virtual void addMultiSampledDepthAttachment() = 0;
        virtual uint CreateColourAttachment(LColourAttachment colourAttachment, bool multisampled = false, int internalFormat = U_RGBA32F) = 0;

        void setClearColour(glm::vec4 clearColour) { m_clearColour = clearColour; }
        virtual void Clear(uint clearFlags) = 0;

        // Gets the screen target for a given window.
        virtual ARenderTarget* GetScreenTarget(LWindow* window) = 0;
        void SetRenderBlock(ARenderBlock* block ){ m_renderBlock = block; }
    protected:
        ARenderBlock* m_renderBlock = nullptr;
        bool m_bDoneInit = false;
        glm::ivec2 m_dimensions;
        glm::vec4 m_clearColour = glm::vec4(1,0,0,1);
    };
}
