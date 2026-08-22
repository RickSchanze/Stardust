#pragma once

#include "RHI/Resources.h"

namespace RHI
{

    // 默认创建 Surface（可顺带创建平台窗口）与 Swapchain
    struct SurfaceWindowDesc
    {
        SurfaceDesc Surface{};
        bool CreateSwapchain = true;
        PresentMode PresentMode = PresentMode::Fifo;
        PixelFormat Format = gDefaultSurfaceFormat;
        ColorSpace ColorSpace = gDefaultSurfaceColorSpace;
        UInt32 PreferredImageCount = gDefaultSwapchainImageCount;
        TextureUsageFlag ImageUsage = TextureUsageFlag::ColorAttachment;
    };

    class SurfaceWindow
    {
    public:
        SurfaceWindow() = default;
        ~SurfaceWindow();

        SurfaceWindow(const SurfaceWindow&) = delete;
        SurfaceWindow& operator=(const SurfaceWindow&) = delete;
        SurfaceWindow(SurfaceWindow&&) = delete;
        SurfaceWindow& operator=(SurfaceWindow&&) = delete;

        [[nodiscard]] bool Init(const SurfaceWindowDesc& Desc, const char* DebugName = nullptr);
        void UnInit();

        [[nodiscard]] bool IsValid() const noexcept
        {
            return !mSurface.IsNull();
        }

        [[nodiscard]] const SurfaceWindowDesc& GetDesc() const noexcept
        {
            return mDesc;
        }

        [[nodiscard]] SurfaceHandle GetSurface() const noexcept
        {
            return mSurface;
        }

        [[nodiscard]] SwapchainHandle GetSwapchain() const noexcept
        {
            return mSwapchain;
        }

        [[nodiscard]] const char* GetDebugName() const noexcept
        {
            return mDebugName;
        }

    private:
        SurfaceWindowDesc mDesc{};
        SurfaceHandle mSurface{};
        SwapchainHandle mSwapchain{};
        char mDebugName[64]{};
    };

    [[nodiscard]] bool ValidateSurfaceWindowDesc(const SurfaceWindowDesc& Desc, String* ErrorMessage = nullptr);

} // namespace RHI
