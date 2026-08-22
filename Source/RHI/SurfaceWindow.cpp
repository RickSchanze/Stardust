#include "RHI/SurfaceWindow.h"

#include <cstring>

#include "Core/Debug/Debug.h"
#include "Core/Logging/Logger.h"
#include "Core/String/String.h"
#include "RHI/Device.h"
#include "RHI/RHIConfig.h"

namespace RHI
{
    namespace
    {
        void SetValidationError(String* ErrorMessage, const char* Message)
        {
            if (ErrorMessage != nullptr)
            {
                *ErrorMessage = String{Message};
            }
        }

        void CopyDebugName(char (&Destination)[64], const char* Name)
        {
            Destination[0] = '\0';
            if (Name == nullptr)
            {
                return;
            }

            std::size_t Index = 0;
            while (Index + 1 < 64 && Name[Index] != '\0')
            {
                Destination[Index] = Name[Index];
                ++Index;
            }
            Destination[Index] = '\0';
        }
    } // namespace

    bool ValidateSurfaceWindowDesc(const SurfaceWindowDesc& Desc, String* ErrorMessage)
    {
        if (!ValidateSurfaceDesc(Desc.Surface, ErrorMessage))
        {
            return false;
        }

        if (Desc.CreateSwapchain && Desc.ImageUsage == TextureUsageFlag::None)
        {
            SetValidationError(
                ErrorMessage,
                "SurfaceWindowDesc.ImageUsage must declare at least one usage flag when CreateSwapchain is true.");
            return false;
        }

        return true;
    }

    SurfaceWindow::~SurfaceWindow()
    {
        UnInit();
    }

    bool SurfaceWindow::Init(const SurfaceWindowDesc& Desc, const char* DebugName)
    {
        Assert(!IsValid());

#if STARDUST_RHI_VALIDATE_DESC
        String Message;
        if (!ValidateSurfaceWindowDesc(Desc, &Message))
        {
            LogError(RHI, "{} (DebugName={})", Message, DebugName != nullptr ? DebugName : "");
            return false;
        }
#endif

        Device& Device = GetDevice();
        const SurfaceHandle Surface = Device.CreateSurface(Desc.Surface, DebugName);
        if (Surface.IsNull())
        {
            return false;
        }

        SwapchainHandle Swapchain{};
        if (Desc.CreateSwapchain)
        {
            const SwapchainDesc SwapchainInfo{
                .Surface = Surface,
                .PresentMode = Desc.PresentMode,
                .Format = Desc.Format,
                .ColorSpace = Desc.ColorSpace,
                .Width = Desc.Surface.Width,
                .Height = Desc.Surface.Height,
                .PreferredImageCount = Desc.PreferredImageCount,
                .ImageUsage = Desc.ImageUsage,
            };

            Swapchain = Device.CreateSwapchain(SwapchainInfo, DebugName);
            if (Swapchain.IsNull())
            {
                Device.DestroySurface(Surface);
                return false;
            }
        }

        mDesc = Desc;
        mSurface = Surface;
        mSwapchain = Swapchain;
        CopyDebugName(mDebugName, DebugName);
        return true;
    }

    void SurfaceWindow::UnInit()
    {
        if (!IsValid())
        {
            return;
        }

        Device& Device = GetDevice();
        if (!mSwapchain.IsNull())
        {
            Assert(Device.IsSwapchainValid(mSwapchain));
            Device.DestroySwapchain(mSwapchain);
            mSwapchain = {};
        }

        Assert(Device.IsSurfaceValid(mSurface));
        Device.DestroySurface(mSurface);
        mSurface = {};
        mDebugName[0] = '\0';
        mDesc = {};
    }
} // namespace RHI
