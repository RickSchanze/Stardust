#include "VulkanDevice.h"

#if STARDUST_RHI_BUILD_VULKAN

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <limits>

#if STARDUST_RHI_ENABLE_VALIDATION
#include <cstdlib>
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "Core/Container/Array.h"
#include "Core/Debug/Debug.h"
#include "Core/Logging/Logger.h"
#include "Core/Profiler/Profiler.h"
#include "Core/String/String.h"

#if STARDUST_RHI_ENABLE_VALIDATION
#include "Core/IO/Directory.h"
#include "Core/IO/Path.h"
#endif

using namespace RHI;

namespace
{
    void LogResourceError(const String& Message, const char* const DebugName)
    {
        if (DebugName != nullptr && DebugName[0] != '\0')
        {
            LogError(RHI, "{} (DebugName={})", Message, DebugName);
            return;
        }

        LogError(RHI, "{}", Message);
    }

    template <typename THandle>
    [[nodiscard]] THandle ToTypedHandle(const GPUHandle Allocated)
    {
        THandle Handle{};
        Handle.Index = Allocated.Index;
        Handle.Generation = Allocated.Generation;
        return Handle;
    }

    [[nodiscard]] VkAttachmentReference MakeAttachmentReference(const RenderPassAttachmentRef& Reference)
    {
        return VkAttachmentReference{
            .attachment = Reference.Attachment,
            .layout = static_cast<VkImageLayout>(Reference.Layout),
        };
    }

    [[nodiscard]] bool HasExtension(const Array<VkExtensionProperties>& Extensions, const char* const Name)
    {
        for (const VkExtensionProperties& Extension : Extensions)
        {
            if (std::strcmp(Extension.extensionName, Name) == 0)
            {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] std::uint32_t ScorePhysicalDevice(const VkPhysicalDevice PhysicalDevice)
    {
        VkPhysicalDeviceProperties Properties{};
        vkGetPhysicalDeviceProperties(PhysicalDevice, &Properties);
        if (Properties.apiVersion < VK_API_VERSION_1_3)
        {
            return 0;
        }

        std::uint32_t Score = 1;
        if (Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            Score += 1000;
        }
        else if (Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            Score += 100;
        }
        return Score;
    }

    [[nodiscard]] bool FindGraphicsQueueFamily(const VkPhysicalDevice PhysicalDevice, std::uint32_t& OutFamily)
    {
        std::uint32_t Count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &Count, nullptr);
        Array<VkQueueFamilyProperties> Families(Count);
        vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &Count, Families.Data());

        for (std::uint32_t Index = 0; Index < Count; ++Index)
        {
            if ((Families[Index].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
            {
                OutFamily = Index;
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] bool DeviceHasExtension(const VkPhysicalDevice PhysicalDevice, const char* const Name)
    {
        std::uint32_t Count = 0;
        if (vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &Count, nullptr) != VK_SUCCESS)
        {
            return false;
        }

        Array<VkExtensionProperties> Extensions(Count);
        if (vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &Count, Extensions.Data()) != VK_SUCCESS)
        {
            return false;
        }
        return HasExtension(Extensions, Name);
    }

    [[nodiscard]] VkSurfaceFormatKHR ChooseSwapchainFormat(const Array<VkSurfaceFormatKHR>& Formats,
                                                           const PixelFormat Preferred,
                                                           const ColorSpace PreferredColorSpace)
    {
        const auto PreferredVkFormat = static_cast<VkFormat>(Preferred);
        const auto PreferredVkColorSpace = static_cast<VkColorSpaceKHR>(PreferredColorSpace);

        if (Preferred != PixelFormat::Undefined)
        {
            for (const VkSurfaceFormatKHR& Format : Formats)
            {
                if (Format.format == PreferredVkFormat && Format.colorSpace == PreferredVkColorSpace)
                {
                    return Format;
                }
            }

            for (const VkSurfaceFormatKHR& Format : Formats)
            {
                if (Format.format == PreferredVkFormat)
                {
                    return Format;
                }
            }
        }

        for (const VkSurfaceFormatKHR& Format : Formats)
        {
            if (Format.format == VK_FORMAT_B8G8R8A8_SRGB && Format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return Format;
            }
        }

        return Formats[0];
    }

    [[nodiscard]] VkPresentModeKHR ChoosePresentMode(const Array<VkPresentModeKHR>& Modes, const PresentMode Preferred)
    {
        const auto PreferredMode = static_cast<VkPresentModeKHR>(Preferred);
        for (const VkPresentModeKHR Mode : Modes)
        {
            if (Mode == PreferredMode)
            {
                return Mode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    [[nodiscard]] VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& Capabilities,
                                                   const UInt32 RequestedWidth,
                                                   const UInt32 RequestedHeight)
    {
        if (Capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max())
        {
            return Capabilities.currentExtent;
        }

        VkExtent2D Extent{
            .width = RequestedWidth,
            .height = RequestedHeight,
        };
        Extent.width =
            std::max(Capabilities.minImageExtent.width, std::min(Capabilities.maxImageExtent.width, Extent.width));
        Extent.height =
            std::max(Capabilities.minImageExtent.height, std::min(Capabilities.maxImageExtent.height, Extent.height));
        return Extent;
    }

#if STARDUST_RHI_ENABLE_VALIDATION
    [[nodiscard]] bool HasLayer(const Array<VkLayerProperties>& Layers, const char* const Name)
    {
        for (const VkLayerProperties& Layer : Layers)
        {
            if (std::strcmp(Layer.layerName, Name) == 0)
            {
                return true;
            }
        }
        return false;
    }

    void PrependVulkanSdkLayerPath()
    {
        const char* const Sdk = std::getenv("VULKAN_SDK");
        if (Sdk == nullptr || Sdk[0] == '\0')
        {
            return;
        }

        const auto LayerDir = Path::Combine(Sdk, "Bin");
        if (!Directory::Exists(LayerDir))
        {
            LogWarn(RHI, "VULKAN_SDK Bin missing: {}", LayerDir);
            return;
        }

        const char* const Existing = std::getenv("VK_ADD_LAYER_PATH");
        const auto Combined =
            Existing != nullptr && Existing[0] != '\0' ? String::Format("{};{}", LayerDir, Existing) : LayerDir;
        _putenv_s("VK_ADD_LAYER_PATH", Combined.Data());
        LogInfo(RHI, "VK_ADD_LAYER_PATH={}", Combined);
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL
    DebugMessengerCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT Severity,
                           const VkDebugUtilsMessageTypeFlagsEXT Type,
                           const VkDebugUtilsMessengerCallbackDataEXT* const CallbackData,
                           void* const UserData)
    {
        (void)Type;
        (void)UserData;

        const char* const Message =
            CallbackData != nullptr && CallbackData->pMessage != nullptr ? CallbackData->pMessage : "";
        if ((Severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) != 0)
        {
            LogError(RHI, "{}", Message);
        }
        else if ((Severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) != 0)
        {
            LogWarn(RHI, "{}", Message);
        }
        else
        {
            LogDebug(RHI, "{}", Message);
        }
        return VK_FALSE;
    }

    [[nodiscard]] VkDebugUtilsMessengerCreateInfoEXT MakeDebugMessengerCreateInfo()
    {
        return VkDebugUtilsMessengerCreateInfoEXT{
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = 0,
            .messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = DebugMessengerCallback,
            .pUserData = nullptr,
        };
    }
#endif
} // namespace

bool VulkanDevice::Init()
{
    PerformanceCounter InitCounter{"VulkanDevice::Init"};
    Assert(mInstance == VK_NULL_HANDLE);
    mAPI = API::Vulkan;

    if (const auto Result = volkInitialize(); Result != VK_SUCCESS)
    {
        LogCritical(RHI, "Failed to initialize volk");
        return false;
    }

    if (!EnsureSdlVideo())
    {
        UnInit();
        return false;
    }

    if (!SDL_Vulkan_LoadLibrary(nullptr))
    {
        LogCritical(RHI, "Failed to load Vulkan through SDL: {}", SDL_GetError());
        UnInit();
        return false;
    }
    mSdlVulkanLoaded = true;

    if (!CreateInstance())
    {
        UnInit();
        return false;
    }
    volkLoadInstance(mInstance);

#if STARDUST_RHI_ENABLE_VALIDATION
    CreateDebugMessenger();
#endif

    if (!PickPhysicalDevice() || !CreateLogicalDevice())
    {
        UnInit();
        return false;
    }
    volkLoadDevice(mDevice);

    if (!CreateAllocator())
    {
        UnInit();
        return false;
    }

    return true;
}

void VulkanDevice::UnInit()
{
    PerformanceCounter UnInitCounter{"VulkanDevice::UnInit"};

    Assert(mSwapchains.LiveCount() == 0);
    Assert(mSurfaces.LiveCount() == 0);
    Assert(mCommandBuffers.LiveCount() == 0);
    Assert(mCommandPools.LiveCount() == 0);
    Assert(mGraphicsPipelines.LiveCount() == 0);
    Assert(mComputePipelines.LiveCount() == 0);
    Assert(mPipelineLayouts.LiveCount() == 0);
    Assert(mDescriptorSetLayouts.LiveCount() == 0);
    Assert(mShaders.LiveCount() == 0);
    Assert(mRenderPasses.LiveCount() == 0);
    Assert(mSamplers.LiveCount() == 0);
    Assert(mTextureViews.LiveCount() == 0);
    Assert(mTextures.LiveCount() == 0);
    Assert(mBuffers.LiveCount() == 0);

    if (mAllocator != nullptr)
    {
        vmaDestroyAllocator(mAllocator);
        mAllocator = nullptr;
    }

    if (mDevice != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(mDevice);
        vkDestroyDevice(mDevice, nullptr);
        mDevice = VK_NULL_HANDLE;
        mGraphicsQueue = VK_NULL_HANDLE;
    }

#if STARDUST_RHI_ENABLE_VALIDATION
    if (mDebugMessenger != VK_NULL_HANDLE && vkDestroyDebugUtilsMessengerEXT != nullptr)
    {
        vkDestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
        mDebugMessenger = VK_NULL_HANDLE;
    }
    mValidationEnabled = false;
#endif

    if (mInstance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(mInstance, nullptr);
        mInstance = VK_NULL_HANDLE;
    }

    mPhysicalDevice = VK_NULL_HANDLE;
    mGraphicsQueueFamily = 0;

    if (mSdlVulkanLoaded)
    {
        SDL_Vulkan_UnloadLibrary();
        mSdlVulkanLoaded = false;
    }

    if (mSdlVideoOwned)
    {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        mSdlVideoOwned = false;
    }
}

bool VulkanDevice::EnsureSdlVideo()
{
    if (SDL_WasInit(SDL_INIT_VIDEO) != 0)
    {
        return true;
    }

    if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
    {
        LogCritical(RHI, "Failed to init SDL video: {}", SDL_GetError());
        return false;
    }

    mSdlVideoOwned = true;
    return true;
}

bool VulkanDevice::CreateInstance()
{
    std::uint32_t InstanceVersion = 0;
    if (const auto Result = vkEnumerateInstanceVersion(&InstanceVersion);
        Result != VK_SUCCESS || InstanceVersion < VK_API_VERSION_1_3)
    {
        LogCritical(RHI, "Vulkan 1.3 is required");
        return false;
    }

    Array<const char*> EnabledLayers;
    Array<const char*> EnabledExtensions;
    const void* InstancePNext = nullptr;

    std::uint32_t SdlExtensionCount = 0;
    const char* const* SdlExtensions = SDL_Vulkan_GetInstanceExtensions(&SdlExtensionCount);
    if (SdlExtensions == nullptr || SdlExtensionCount == 0)
    {
        LogCritical(RHI, "SDL_Vulkan_GetInstanceExtensions failed: {}", SDL_GetError());
        return false;
    }
    for (std::uint32_t Index = 0; Index < SdlExtensionCount; ++Index)
    {
        EnabledExtensions.Add(SdlExtensions[Index]);
    }

#if STARDUST_RHI_ENABLE_VALIDATION
    mValidationEnabled = false;
    const auto DebugCreateInfo = MakeDebugMessengerCreateInfo();
    VkValidationFeaturesEXT ValidationFeatures{};
    constexpr VkValidationFeatureEnableEXT EnabledFeatures[] = {
        VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
    };

    if (gEnableValidation)
    {
        PrependVulkanSdkLayerPath();

        std::uint32_t LayerCount = 0;
        if (vkEnumerateInstanceLayerProperties(&LayerCount, nullptr) == VK_SUCCESS)
        {
            if (Array<VkLayerProperties> Layers(LayerCount);
                vkEnumerateInstanceLayerProperties(&LayerCount, Layers.Data()) == VK_SUCCESS &&
                HasLayer(Layers, "VK_LAYER_KHRONOS_validation"))
            {
                EnabledLayers.Add("VK_LAYER_KHRONOS_validation");
                mValidationEnabled = true;
            }
            else
            {
                LogError(RHI, "VK_LAYER_KHRONOS_validation not found; install Vulkan SDK and set VULKAN_SDK");
            }
        }

        std::uint32_t ExtensionCount = 0;
        bool DebugUtilsEnabled = false;
        if (vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr) == VK_SUCCESS)
        {
            if (Array<VkExtensionProperties> Extensions(ExtensionCount);
                vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, Extensions.Data()) == VK_SUCCESS &&
                HasExtension(Extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
            {
                EnabledExtensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
                DebugUtilsEnabled = true;
            }
            else
            {
                LogWarn(RHI, "{} unavailable", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }
        }

        if (mValidationEnabled && DebugUtilsEnabled)
        {
            ValidationFeatures = VkValidationFeaturesEXT{
                .sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
                .pNext = &DebugCreateInfo,
                .enabledValidationFeatureCount = 1,
                .pEnabledValidationFeatures = EnabledFeatures,
                .disabledValidationFeatureCount = 0,
                .pDisabledValidationFeatures = nullptr,
            };
            InstancePNext = &ValidationFeatures;
        }
        else if (DebugUtilsEnabled)
        {
            InstancePNext = &DebugCreateInfo;
        }
    }
#endif

    constexpr VkApplicationInfo AppInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "Stardust",
        .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
        .pEngineName = "Stardust",
        .engineVersion = VK_MAKE_VERSION(0, 1, 0),
        .apiVersion = VK_API_VERSION_1_3,
    };

    const VkInstanceCreateInfo CreateInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = InstancePNext,
        .flags = 0,
        .pApplicationInfo = &AppInfo,
        .enabledLayerCount = static_cast<std::uint32_t>(EnabledLayers.Num()),
        .ppEnabledLayerNames = EnabledLayers.Data(),
        .enabledExtensionCount = static_cast<std::uint32_t>(EnabledExtensions.Num()),
        .ppEnabledExtensionNames = EnabledExtensions.Data(),
    };

    if (const auto Result = vkCreateInstance(&CreateInfo, nullptr, &mInstance); Result != VK_SUCCESS)
    {
        LogCritical(RHI, "Failed to create Vulkan instance");
        return false;
    }

#if STARDUST_RHI_ENABLE_VALIDATION
    LogInfo(RHI, "Vulkan instance created, validation={}", mValidationEnabled);
#else
    LogInfo(RHI, "Vulkan instance created, validation=false");
#endif
    return true;
}

#if STARDUST_RHI_ENABLE_VALIDATION
void VulkanDevice::CreateDebugMessenger()
{
    if (!mValidationEnabled || vkCreateDebugUtilsMessengerEXT == nullptr)
    {
        return;
    }

    const auto CreateInfo = MakeDebugMessengerCreateInfo();
    if (const auto Result = vkCreateDebugUtilsMessengerEXT(mInstance, &CreateInfo, nullptr, &mDebugMessenger);
        Result != VK_SUCCESS)
    {
        LogWarn(RHI, "Failed to create Vulkan debug messenger");
    }
}
#endif

bool VulkanDevice::PickPhysicalDevice()
{
    std::uint32_t DeviceCount = 0;
    if (const auto Result = vkEnumeratePhysicalDevices(mInstance, &DeviceCount, nullptr);
        Result != VK_SUCCESS || DeviceCount == 0)
    {
        LogCritical(RHI, "No Vulkan physical device found");
        return false;
    }

    Array<VkPhysicalDevice> Devices(DeviceCount);
    if (const auto Result = vkEnumeratePhysicalDevices(mInstance, &DeviceCount, Devices.Data()); Result != VK_SUCCESS)
    {
        LogCritical(RHI, "Failed to enumerate Vulkan physical devices");
        return false;
    }

    std::uint32_t BestScore = 0;
    VkPhysicalDevice BestDevice = VK_NULL_HANDLE;
    std::uint32_t BestFamily = 0;
    for (const auto PhysicalDevice : Devices)
    {
        if (!DeviceHasExtension(PhysicalDevice, VK_KHR_SWAPCHAIN_EXTENSION_NAME))
        {
            continue;
        }

        std::uint32_t QueueFamily = 0;
        if (!FindGraphicsQueueFamily(PhysicalDevice, QueueFamily))
        {
            continue;
        }

        if (!SDL_Vulkan_GetPresentationSupport(mInstance, PhysicalDevice, QueueFamily))
        {
            continue;
        }

        if (const auto Score = ScorePhysicalDevice(PhysicalDevice); Score > BestScore)
        {
            BestScore = Score;
            BestDevice = PhysicalDevice;
            BestFamily = QueueFamily;
        }
    }

    if (BestDevice == VK_NULL_HANDLE)
    {
        LogCritical(RHI, "No suitable Vulkan 1.3 GPU with graphics+present queue");
        return false;
    }

    mPhysicalDevice = BestDevice;
    mGraphicsQueueFamily = BestFamily;

    VkPhysicalDeviceProperties Properties{};
    vkGetPhysicalDeviceProperties(mPhysicalDevice, &Properties);
    LogInfo(RHI,
            "GPU {} (Vulkan {}.{}.{})",
            Properties.deviceName,
            VK_VERSION_MAJOR(Properties.apiVersion),
            VK_VERSION_MINOR(Properties.apiVersion),
            VK_VERSION_PATCH(Properties.apiVersion));
    return true;
}

bool VulkanDevice::CreateLogicalDevice()
{
    constexpr float QueuePriority = 1.0f;
    const VkDeviceQueueCreateInfo QueueInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = mGraphicsQueueFamily,
        .queueCount = 1,
        .pQueuePriorities = &QueuePriority,
    };

    VkPhysicalDeviceVulkan13Features Features13{};
    Features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    VkPhysicalDeviceFeatures2 Features2{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &Features13,
        .features = {},
    };
    vkGetPhysicalDeviceFeatures2(mPhysicalDevice, &Features2);

    const char* DeviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    const VkDeviceCreateInfo CreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &Features2,
        .flags = 0,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &QueueInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = DeviceExtensions,
        .pEnabledFeatures = nullptr,
    };

    if (const auto Result = vkCreateDevice(mPhysicalDevice, &CreateInfo, nullptr, &mDevice); Result != VK_SUCCESS)
    {
        LogCritical(RHI, "Failed to create Vulkan device");
        return false;
    }

    vkGetDeviceQueue(mDevice, mGraphicsQueueFamily, 0, &mGraphicsQueue);
    return true;
}

bool VulkanDevice::CreateAllocator()
{
    VmaVulkanFunctions VulkanFunctions{};
    VulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
    VulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
    VmaAllocatorCreateInfo CreateInfo{
        .flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT,
        .physicalDevice = mPhysicalDevice,
        .device = mDevice,
        .preferredLargeHeapBlockSize = 0,
        .pAllocationCallbacks = nullptr,
        .pDeviceMemoryCallbacks = nullptr,
        .pHeapSizeLimit = nullptr,
        .pVulkanFunctions = &VulkanFunctions,
        .instance = mInstance,
        .vulkanApiVersion = VK_API_VERSION_1_3,
        .pTypeExternalMemoryHandleTypes = nullptr,
    };

    if (const auto Result = vmaCreateAllocator(&CreateInfo, &mAllocator); Result != VK_SUCCESS)
    {
        CreateInfo.flags = 0;
        if (const auto RetryResult = vmaCreateAllocator(&CreateInfo, &mAllocator); RetryResult != VK_SUCCESS)
        {
            LogCritical(RHI, "Failed to create VMA allocator");
            return false;
        }
    }
    return true;
}

void VulkanDevice::SetObjectDebugName(const VkObjectType ObjectType,
                                      const std::uint64_t ObjectHandle,
                                      const char* const Name) const
{
    if (vkSetDebugUtilsObjectNameEXT == nullptr || Name == nullptr || Name[0] == '\0' || ObjectHandle == 0)
    {
        return;
    }

    const VkDebugUtilsObjectNameInfoEXT NameInfo{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        .pNext = nullptr,
        .objectType = ObjectType,
        .objectHandle = ObjectHandle,
        .pObjectName = Name,
    };
    vkSetDebugUtilsObjectNameEXT(mDevice, &NameInfo);
}

BufferHandle VulkanDevice::CreateBuffer(const BufferDesc& Desc, const char* DebugName)
{
#if STARDUST_RHI_VALIDATE_DESC
    String Message;
    if (!ValidateBufferDesc(Desc, &Message))
    {
        LogResourceError(Message, DebugName);
        return {};
    }
#endif

    Assert(mAllocator != nullptr);

    const VkBufferCreateInfo BufferInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = static_cast<VkBufferCreateFlags>(Desc.CreateFlags),
        .size = Desc.Size,
        .usage = static_cast<VkBufferUsageFlags>(Desc.Usage),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
    };

    VmaAllocationCreateInfo AllocationInfo{};
    AllocationInfo.usage = VMA_MEMORY_USAGE_AUTO;
    AllocationInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(Desc.MemoryProperties);
    if (HasFlag(Desc.MemoryProperties, MemoryPropertyFlag::HostVisible))
    {
        AllocationInfo.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    }

    VkBuffer Buffer = VK_NULL_HANDLE;
    VmaAllocation Allocation = nullptr;
    if (const auto Result = vmaCreateBuffer(mAllocator, &BufferInfo, &AllocationInfo, &Buffer, &Allocation, nullptr);
        Result != VK_SUCCESS)
    {
        LogResourceError(String{"Failed to create buffer"}, DebugName);
        return {};
    }

    GPUBuffer Resource(Desc);
    Resource.Native = UIntPtr::FromPtr(Buffer);
    Resource.Allocation = UIntPtr::FromPtr(Allocation);
    AssignResourceDebugName(Resource.DebugName, DebugName, Buffer);
    if (Resource.DebugName[0] != '\0')
    {
        SetObjectDebugName(VK_OBJECT_TYPE_BUFFER, reinterpret_cast<std::uint64_t>(Buffer), Resource.DebugName);
        vmaSetAllocationName(mAllocator, Allocation, Resource.DebugName);
    }

    return ToTypedHandle<BufferHandle>(mBuffers.Create(std::move(Resource)));
}

void VulkanDevice::DestroyBuffer(const BufferHandle Handle)
{
    Assert(IsBufferValid(Handle));

    const GPUBuffer& Buffer = GetBuffer(Handle);
    if (Buffer.Native.IsValid())
    {
        Assert(mAllocator != nullptr);
        vmaDestroyBuffer(mAllocator, Buffer.Native.ToPtr<VkBuffer>(), Buffer.Allocation.ToPtr<VmaAllocation>());
    }

    mBuffers.Destroy(Handle);
}

TextureHandle VulkanDevice::CreateTexture(const TextureDesc& Desc, const char* DebugName)
{
#if STARDUST_RHI_VALIDATE_DESC
    String Message;
    if (!ValidateTextureDesc(Desc, &Message))
    {
        LogResourceError(Message, DebugName);
        return {};
    }
#endif

    Assert(mAllocator != nullptr);

    const VkImageCreateInfo ImageInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = static_cast<VkImageCreateFlags>(Desc.CreateFlags),
        .imageType = static_cast<VkImageType>(Desc.Dimension),
        .format = static_cast<VkFormat>(Desc.Format),
        .extent =
            VkExtent3D{
                .width = Desc.Width,
                .height = Desc.Height,
                .depth = Desc.Depth,
            },
        .mipLevels = Desc.MipLevels,
        .arrayLayers = Desc.ArrayLayers,
        .samples = static_cast<VkSampleCountFlagBits>(Desc.Samples),
        .tiling = static_cast<VkImageTiling>(Desc.Tiling),
        .usage = static_cast<VkImageUsageFlags>(Desc.Usage),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = static_cast<VkImageLayout>(Desc.InitialLayout),
    };

    VmaAllocationCreateInfo AllocationInfo{};
    AllocationInfo.usage = VMA_MEMORY_USAGE_AUTO;
    AllocationInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    VkImage Image = VK_NULL_HANDLE;
    VmaAllocation Allocation = nullptr;
    if (const auto Result = vmaCreateImage(mAllocator, &ImageInfo, &AllocationInfo, &Image, &Allocation, nullptr);
        Result != VK_SUCCESS)
    {
        LogResourceError(String{"Failed to create texture image"}, DebugName);
        return {};
    }

    GPUTexture Resource(Desc);
    Resource.Native = UIntPtr::FromPtr(Image);
    Resource.Allocation = UIntPtr::FromPtr(Allocation);
    AssignResourceDebugName(Resource.DebugName, DebugName, Image);
    if (Resource.DebugName[0] != '\0')
    {
        SetObjectDebugName(VK_OBJECT_TYPE_IMAGE, reinterpret_cast<std::uint64_t>(Image), Resource.DebugName);
        vmaSetAllocationName(mAllocator, Allocation, Resource.DebugName);
    }

    return ToTypedHandle<TextureHandle>(mTextures.Create(std::move(Resource)));
}

void VulkanDevice::DestroyTexture(const TextureHandle Handle)
{
    Assert(IsTextureValid(Handle));

    const GPUTexture& Texture = GetTexture(Handle);
    if (Texture.Native.IsValid() && Texture.Allocation.IsValid())
    {
        Assert(mAllocator != nullptr);
        vmaDestroyImage(mAllocator, Texture.Native.ToPtr<VkImage>(), Texture.Allocation.ToPtr<VmaAllocation>());
    }

    mTextures.Destroy(Handle);
}

TextureViewHandle VulkanDevice::CreateTextureView(const TextureViewDesc& Desc, const char* DebugName)
{
#if STARDUST_RHI_VALIDATE_DESC
    String Message;
    if (!ValidateTextureViewDesc(Desc, &Message))
    {
        LogResourceError(Message, DebugName);
        return {};
    }

    if (!IsTextureValid(Desc.Texture))
    {
        LogResourceError(String{"TextureViewDesc.Texture handle is not valid"}, DebugName);
        return {};
    }
#endif

    const GPUTexture& Texture = GetTexture(Desc.Texture);
    const VkImageViewCreateInfo ViewInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = Texture.Native.ToPtr<VkImage>(),
        .viewType = static_cast<VkImageViewType>(Desc.Dimension),
        .format = static_cast<VkFormat>(Desc.Format),
        .components =
            VkComponentMapping{
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
        .subresourceRange =
            VkImageSubresourceRange{
                .aspectMask = static_cast<VkImageAspectFlags>(Desc.Aspect),
                .baseMipLevel = Desc.BaseMipLevel,
                .levelCount = Desc.MipLevelCount,
                .baseArrayLayer = Desc.BaseArrayLayer,
                .layerCount = Desc.ArrayLayerCount,
            },
    };

    VkImageView ImageView = VK_NULL_HANDLE;
    if (const auto Result = vkCreateImageView(mDevice, &ViewInfo, nullptr, &ImageView); Result != VK_SUCCESS)
    {
        LogResourceError(String{"Failed to create texture view"}, DebugName);
        return {};
    }

    GPUTextureView Resource(Desc);
    Resource.Native = UIntPtr::FromPtr(ImageView);
    AssignResourceDebugName(Resource.DebugName, DebugName, ImageView);
    if (Resource.DebugName[0] != '\0')
    {
        SetObjectDebugName(VK_OBJECT_TYPE_IMAGE_VIEW, reinterpret_cast<std::uint64_t>(ImageView), Resource.DebugName);
    }

    return ToTypedHandle<TextureViewHandle>(mTextureViews.Create(std::move(Resource)));
}

void VulkanDevice::DestroyTextureView(const TextureViewHandle Handle)
{
    Assert(IsTextureViewValid(Handle));

    const GPUTextureView& View = GetTextureView(Handle);
    if (View.Native.IsValid())
    {
        vkDestroyImageView(mDevice, View.Native.ToPtr<VkImageView>(), nullptr);
    }

    mTextureViews.Destroy(Handle);
}

SamplerHandle VulkanDevice::CreateSampler(const SamplerDesc& Desc, const char* DebugName)
{
#if STARDUST_RHI_VALIDATE_DESC
    String Message;
    if (!ValidateSamplerDesc(Desc, &Message))
    {
        LogResourceError(Message, DebugName);
        return {};
    }
#endif

    VkSamplerReductionModeCreateInfo ReductionInfo{
        .sType = VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO,
        .pNext = nullptr,
        .reductionMode = static_cast<VkSamplerReductionMode>(Desc.ReductionMode),
    };

    const VkSamplerCreateInfo SamplerInfo{
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = Desc.ReductionMode == SamplerReductionMode::WeightedAverage ? nullptr : &ReductionInfo,
        .flags = static_cast<VkSamplerCreateFlags>(Desc.CreateFlags),
        .magFilter = static_cast<VkFilter>(Desc.MagFilter),
        .minFilter = static_cast<VkFilter>(Desc.MinFilter),
        .mipmapMode = static_cast<VkSamplerMipmapMode>(Desc.MipmapMode),
        .addressModeU = static_cast<VkSamplerAddressMode>(Desc.AddressModeU),
        .addressModeV = static_cast<VkSamplerAddressMode>(Desc.AddressModeV),
        .addressModeW = static_cast<VkSamplerAddressMode>(Desc.AddressModeW),
        .mipLodBias = Desc.MipLodBias,
        .anisotropyEnable = Desc.AnisotropyEnable ? VK_TRUE : VK_FALSE,
        .maxAnisotropy = Desc.MaxAnisotropy,
        .compareEnable = Desc.CompareEnable ? VK_TRUE : VK_FALSE,
        .compareOp = static_cast<VkCompareOp>(Desc.Compare),
        .minLod = Desc.MinLod,
        .maxLod = Desc.MaxLod,
        .borderColor = static_cast<VkBorderColor>(Desc.Border),
        .unnormalizedCoordinates = VK_FALSE,
    };

    VkSampler Sampler = VK_NULL_HANDLE;
    if (const auto Result = vkCreateSampler(mDevice, &SamplerInfo, nullptr, &Sampler); Result != VK_SUCCESS)
    {
        LogResourceError(String{"Failed to create sampler"}, DebugName);
        return {};
    }

    GPUSampler Resource(Desc);
    Resource.Native = UIntPtr::FromPtr(Sampler);
    AssignResourceDebugName(Resource.DebugName, DebugName, Sampler);
    if (Resource.DebugName[0] != '\0')
    {
        SetObjectDebugName(VK_OBJECT_TYPE_SAMPLER, reinterpret_cast<std::uint64_t>(Sampler), Resource.DebugName);
    }

    return ToTypedHandle<SamplerHandle>(mSamplers.Create(std::move(Resource)));
}

void VulkanDevice::DestroySampler(const SamplerHandle Handle)
{
    Assert(IsSamplerValid(Handle));

    const GPUSampler& Sampler = GetSampler(Handle);
    if (Sampler.Native.IsValid())
    {
        vkDestroySampler(mDevice, Sampler.Native.ToPtr<VkSampler>(), nullptr);
    }

    mSamplers.Destroy(Handle);
}

ShaderHandle VulkanDevice::CreateShader(const ShaderDesc& Desc, const char* DebugName)
{
#if STARDUST_RHI_VALIDATE_DESC
    String Message;
    if (!ValidateShaderDesc(Desc, &Message))
    {
        LogResourceError(Message, DebugName);
        return {};
    }
#endif

    const VkShaderModuleCreateInfo ModuleInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = Desc.Code.Num() * sizeof(std::uint32_t),
        .pCode = Desc.Code.Data(),
    };

    VkShaderModule Module = VK_NULL_HANDLE;
    if (const auto Result = vkCreateShaderModule(mDevice, &ModuleInfo, nullptr, &Module); Result != VK_SUCCESS)
    {
        LogResourceError(String{"Failed to create shader module"}, DebugName);
        return {};
    }

    GPUShader Resource(Desc);
    Resource.Native = UIntPtr::FromPtr(Module);
    AssignResourceDebugName(Resource.DebugName, DebugName, Module);
    if (Resource.DebugName[0] != '\0')
    {
        SetObjectDebugName(VK_OBJECT_TYPE_SHADER_MODULE, reinterpret_cast<std::uint64_t>(Module), Resource.DebugName);
    }

    return ToTypedHandle<ShaderHandle>(mShaders.Create(std::move(Resource)));
}

void VulkanDevice::DestroyShader(const ShaderHandle Handle)
{
    Assert(IsShaderValid(Handle));

    const GPUShader& Shader = GetShader(Handle);
    if (Shader.Native.IsValid())
    {
        vkDestroyShaderModule(mDevice, Shader.Native.ToPtr<VkShaderModule>(), nullptr);
    }

    mShaders.Destroy(Handle);
}

DescriptorSetLayoutHandle VulkanDevice::CreateDescriptorSetLayout(const DescriptorSetLayoutDesc& Desc,
                                                                  const char* DebugName)
{
#if STARDUST_RHI_VALIDATE_DESC
    String Message;
    if (!ValidateDescriptorSetLayoutDesc(Desc, &Message))
    {
        LogResourceError(Message, DebugName);
        return {};
    }
#endif

    Array<VkDescriptorSetLayoutBinding> Bindings;
    Array<VkDescriptorBindingFlags> BindingFlags;
    Array<Array<VkSampler>> ImmutableSamplerStorage;
    Bindings.Reserve(Desc.Bindings.Num());
    BindingFlags.Reserve(Desc.Bindings.Num());
    ImmutableSamplerStorage.Reserve(Desc.Bindings.Num());

    bool UsesBindingFlags = false;
    for (const DescriptorBindingDesc& BindingDesc : Desc.Bindings)
    {
        VkDescriptorSetLayoutBinding Binding{
            .binding = BindingDesc.Binding,
            .descriptorType = static_cast<VkDescriptorType>(BindingDesc.Type),
            .descriptorCount = BindingDesc.DescriptorCount,
            .stageFlags = static_cast<VkShaderStageFlags>(BindingDesc.Stages),
            .pImmutableSamplers = nullptr,
        };

        if (!BindingDesc.ImmutableSamplers.Empty())
        {
            Array<VkSampler>& Samplers = ImmutableSamplerStorage.Emplace();
            Samplers.Reserve(BindingDesc.ImmutableSamplers.Num());
            for (const SamplerHandle SamplerHandleValue : BindingDesc.ImmutableSamplers)
            {
#if STARDUST_RHI_VALIDATE_DESC
                if (!IsSamplerValid(SamplerHandleValue))
                {
                    LogResourceError(String{"Immutable sampler handle is not valid"}, DebugName);
                    return {};
                }
#endif
                Samplers.Add(GetSampler(SamplerHandleValue).Native.ToPtr<VkSampler>());
            }
            Binding.pImmutableSamplers = Samplers.Data();
        }

        Bindings.Add(Binding);
        BindingFlags.Add(static_cast<VkDescriptorBindingFlags>(BindingDesc.Flags));
        UsesBindingFlags = UsesBindingFlags || BindingDesc.Flags != DescriptorBindingFlag::None;
    }

    VkDescriptorSetLayoutBindingFlagsCreateInfo BindingFlagsInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
        .pNext = nullptr,
        .bindingCount = static_cast<std::uint32_t>(BindingFlags.Num()),
        .pBindingFlags = BindingFlags.Data(),
    };

    const VkDescriptorSetLayoutCreateInfo LayoutInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = UsesBindingFlags ? &BindingFlagsInfo : nullptr,
        .flags = static_cast<VkDescriptorSetLayoutCreateFlags>(Desc.CreateFlags),
        .bindingCount = static_cast<std::uint32_t>(Bindings.Num()),
        .pBindings = Bindings.Data(),
    };

    VkDescriptorSetLayout Layout = VK_NULL_HANDLE;
    if (const auto Result = vkCreateDescriptorSetLayout(mDevice, &LayoutInfo, nullptr, &Layout); Result != VK_SUCCESS)
    {
        LogResourceError(String{"Failed to create descriptor set layout"}, DebugName);
        return {};
    }

    GPUDescriptorSetLayout Resource(Desc);
    Resource.Native = UIntPtr::FromPtr(Layout);
    AssignResourceDebugName(Resource.DebugName, DebugName, Layout);
    if (Resource.DebugName[0] != '\0')
    {
        SetObjectDebugName(
            VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, reinterpret_cast<std::uint64_t>(Layout), Resource.DebugName);
    }

    return ToTypedHandle<DescriptorSetLayoutHandle>(mDescriptorSetLayouts.Create(std::move(Resource)));
}

void VulkanDevice::DestroyDescriptorSetLayout(const DescriptorSetLayoutHandle Handle)
{
    Assert(IsDescriptorSetLayoutValid(Handle));

    const GPUDescriptorSetLayout& Layout = GetDescriptorSetLayout(Handle);
    if (Layout.Native.IsValid())
    {
        vkDestroyDescriptorSetLayout(mDevice, Layout.Native.ToPtr<VkDescriptorSetLayout>(), nullptr);
    }

    mDescriptorSetLayouts.Destroy(Handle);
}

PipelineLayoutHandle VulkanDevice::CreatePipelineLayout(const PipelineLayoutDesc& Desc, const char* DebugName)
{
#if STARDUST_RHI_VALIDATE_DESC
    String Message;
    if (!ValidatePipelineLayoutDesc(Desc, &Message))
    {
        LogResourceError(Message, DebugName);
        return {};
    }
#endif

    Array<VkDescriptorSetLayout> SetLayouts;
    SetLayouts.Reserve(Desc.SetLayouts.Num());
    for (const DescriptorSetLayoutHandle LayoutHandle : Desc.SetLayouts)
    {
#if STARDUST_RHI_VALIDATE_DESC
        if (!IsDescriptorSetLayoutValid(LayoutHandle))
        {
            LogResourceError(String{"PipelineLayoutDesc.SetLayouts contains an invalid handle"}, DebugName);
            return {};
        }
#endif
        SetLayouts.Add(GetDescriptorSetLayout(LayoutHandle).Native.ToPtr<VkDescriptorSetLayout>());
    }

    Array<VkPushConstantRange> PushConstantRanges;
    PushConstantRanges.Reserve(Desc.PushConstantRanges.Num());
    for (const PushConstantRangeDesc& Range : Desc.PushConstantRanges)
    {
        PushConstantRanges.Add(VkPushConstantRange{
            .stageFlags = static_cast<VkShaderStageFlags>(Range.Stages),
            .offset = Range.Offset,
            .size = Range.Size,
        });
    }

    const VkPipelineLayoutCreateInfo LayoutInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = static_cast<std::uint32_t>(SetLayouts.Num()),
        .pSetLayouts = SetLayouts.Data(),
        .pushConstantRangeCount = static_cast<std::uint32_t>(PushConstantRanges.Num()),
        .pPushConstantRanges = PushConstantRanges.Data(),
    };

    VkPipelineLayout Layout = VK_NULL_HANDLE;
    if (const auto Result = vkCreatePipelineLayout(mDevice, &LayoutInfo, nullptr, &Layout); Result != VK_SUCCESS)
    {
        LogResourceError(String{"Failed to create pipeline layout"}, DebugName);
        return {};
    }

    GPUPipelineLayout Resource(Desc);
    Resource.Native = UIntPtr::FromPtr(Layout);
    AssignResourceDebugName(Resource.DebugName, DebugName, Layout);
    if (Resource.DebugName[0] != '\0')
    {
        SetObjectDebugName(VK_OBJECT_TYPE_PIPELINE_LAYOUT, reinterpret_cast<std::uint64_t>(Layout), Resource.DebugName);
    }

    return ToTypedHandle<PipelineLayoutHandle>(mPipelineLayouts.Create(std::move(Resource)));
}

void VulkanDevice::DestroyPipelineLayout(const PipelineLayoutHandle Handle)
{
    Assert(IsPipelineLayoutValid(Handle));

    const GPUPipelineLayout& Layout = GetPipelineLayout(Handle);
    if (Layout.Native.IsValid())
    {
        vkDestroyPipelineLayout(mDevice, Layout.Native.ToPtr<VkPipelineLayout>(), nullptr);
    }

    mPipelineLayouts.Destroy(Handle);
}

GraphicsPipelineHandle VulkanDevice::CreateGraphicsPipeline(const GraphicsPipelineDesc& Desc, const char* DebugName)
{
#if STARDUST_RHI_VALIDATE_DESC
    String Message;
    if (!ValidateGraphicsPipelineDesc(Desc, &Message))
    {
        LogResourceError(Message, DebugName);
        return {};
    }

    if (!IsPipelineLayoutValid(Desc.Layout) || !IsShaderValid(Desc.VertexShader) ||
        !IsShaderValid(Desc.FragmentShader) || !IsRenderPassValid(Desc.RenderPass))
    {
        LogResourceError(String{"GraphicsPipelineDesc references one or more invalid handles"}, DebugName);
        return {};
    }
#endif

    const GPUShader& VertexShader = GetShader(Desc.VertexShader);
    const GPUShader& FragmentShader = GetShader(Desc.FragmentShader);

    const VkPipelineShaderStageCreateInfo ShaderStages[] = {
        VkPipelineShaderStageCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = static_cast<VkShaderStageFlagBits>(VertexShader.GetDesc().Stage),
            .module = VertexShader.Native.ToPtr<VkShaderModule>(),
            .pName = VertexShader.GetDesc().EntryPoint,
            .pSpecializationInfo = nullptr,
        },
        VkPipelineShaderStageCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = static_cast<VkShaderStageFlagBits>(FragmentShader.GetDesc().Stage),
            .module = FragmentShader.Native.ToPtr<VkShaderModule>(),
            .pName = FragmentShader.GetDesc().EntryPoint,
            .pSpecializationInfo = nullptr,
        },
    };

    const VkPipelineVertexInputStateCreateInfo VertexInput{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr,
    };

    const VkPipelineInputAssemblyStateCreateInfo InputAssembly{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = static_cast<VkPrimitiveTopology>(Desc.Topology),
        .primitiveRestartEnable = VK_FALSE,
    };

    const VkPipelineViewportStateCreateInfo ViewportState{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = nullptr,
        .scissorCount = 1,
        .pScissors = nullptr,
    };

    const VkPipelineRasterizationStateCreateInfo Rasterization{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = static_cast<VkPolygonMode>(Desc.PolygonMode),
        .cullMode = static_cast<VkCullModeFlags>(Desc.CullMode),
        .frontFace = static_cast<VkFrontFace>(Desc.FrontFace),
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0f,
    };

    const VkPipelineMultisampleStateCreateInfo Multisample{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizationSamples = static_cast<VkSampleCountFlagBits>(Desc.Samples),
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    const VkPipelineDepthStencilStateCreateInfo DepthStencil{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthTestEnable = Desc.DepthTestEnable ? VK_TRUE : VK_FALSE,
        .depthWriteEnable = Desc.DepthWriteEnable ? VK_TRUE : VK_FALSE,
        .depthCompareOp = static_cast<VkCompareOp>(Desc.DepthCompareOp),
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
        .front = {},
        .back = {},
        .minDepthBounds = 0.0f,
        .maxDepthBounds = 1.0f,
    };

    Array<VkPipelineColorBlendAttachmentState> BlendAttachments;
    BlendAttachments.Reserve(Desc.ColorAttachments.Num());
    for (const GraphicsColorAttachmentDesc& ColorAttachment : Desc.ColorAttachments)
    {
        BlendAttachments.Add(VkPipelineColorBlendAttachmentState{
            .blendEnable = ColorAttachment.BlendEnable ? VK_TRUE : VK_FALSE,
            .srcColorBlendFactor = static_cast<VkBlendFactor>(ColorAttachment.SrcColorBlendFactor),
            .dstColorBlendFactor = static_cast<VkBlendFactor>(ColorAttachment.DstColorBlendFactor),
            .colorBlendOp = static_cast<VkBlendOp>(ColorAttachment.ColorBlendOp),
            .srcAlphaBlendFactor = static_cast<VkBlendFactor>(ColorAttachment.SrcAlphaBlendFactor),
            .dstAlphaBlendFactor = static_cast<VkBlendFactor>(ColorAttachment.DstAlphaBlendFactor),
            .alphaBlendOp = static_cast<VkBlendOp>(ColorAttachment.AlphaBlendOp),
            .colorWriteMask = static_cast<VkColorComponentFlags>(ColorAttachment.ColorWriteMask),
        });
    }

    const VkPipelineColorBlendStateCreateInfo ColorBlend{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = static_cast<std::uint32_t>(BlendAttachments.Num()),
        .pAttachments = BlendAttachments.Data(),
        .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f},
    };

    constexpr VkDynamicState DynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };
    const VkPipelineDynamicStateCreateInfo DynamicState{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = static_cast<std::uint32_t>(std::size(DynamicStates)),
        .pDynamicStates = DynamicStates,
    };

    const VkGraphicsPipelineCreateInfo PipelineInfo{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stageCount = static_cast<std::uint32_t>(std::size(ShaderStages)),
        .pStages = ShaderStages,
        .pVertexInputState = &VertexInput,
        .pInputAssemblyState = &InputAssembly,
        .pTessellationState = nullptr,
        .pViewportState = &ViewportState,
        .pRasterizationState = &Rasterization,
        .pMultisampleState = &Multisample,
        .pDepthStencilState = &DepthStencil,
        .pColorBlendState = &ColorBlend,
        .pDynamicState = &DynamicState,
        .layout = GetPipelineLayout(Desc.Layout).Native.ToPtr<VkPipelineLayout>(),
        .renderPass = GetRenderPass(Desc.RenderPass).Native.ToPtr<VkRenderPass>(),
        .subpass = Desc.Subpass,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    VkPipeline Pipeline = VK_NULL_HANDLE;
    if (const auto Result = vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &Pipeline);
        Result != VK_SUCCESS)
    {
        LogResourceError(String{"Failed to create graphics pipeline"}, DebugName);
        return {};
    }

    GPUGraphicsPipeline Resource(Desc);
    Resource.Native = UIntPtr::FromPtr(Pipeline);
    AssignResourceDebugName(Resource.DebugName, DebugName, Pipeline);
    if (Resource.DebugName[0] != '\0')
    {
        SetObjectDebugName(VK_OBJECT_TYPE_PIPELINE, reinterpret_cast<std::uint64_t>(Pipeline), Resource.DebugName);
    }

    return ToTypedHandle<GraphicsPipelineHandle>(mGraphicsPipelines.Create(std::move(Resource)));
}

void VulkanDevice::DestroyGraphicsPipeline(const GraphicsPipelineHandle Handle)
{
    Assert(IsGraphicsPipelineValid(Handle));

    const GPUGraphicsPipeline& Pipeline = GetGraphicsPipeline(Handle);
    if (Pipeline.Native.IsValid())
    {
        vkDestroyPipeline(mDevice, Pipeline.Native.ToPtr<VkPipeline>(), nullptr);
    }

    mGraphicsPipelines.Destroy(Handle);
}

ComputePipelineHandle VulkanDevice::CreateComputePipeline(const ComputePipelineDesc& Desc, const char* DebugName)
{
#if STARDUST_RHI_VALIDATE_DESC
    String Message;
    if (!ValidateComputePipelineDesc(Desc, &Message))
    {
        LogResourceError(Message, DebugName);
        return {};
    }

    if (!IsPipelineLayoutValid(Desc.Layout) || !IsShaderValid(Desc.ComputeShader))
    {
        LogResourceError(String{"ComputePipelineDesc references one or more invalid handles"}, DebugName);
        return {};
    }
#endif

    const GPUShader& ComputeShader = GetShader(Desc.ComputeShader);
    const VkComputePipelineCreateInfo PipelineInfo{
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage =
            VkPipelineShaderStageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stage = static_cast<VkShaderStageFlagBits>(ComputeShader.GetDesc().Stage),
                .module = ComputeShader.Native.ToPtr<VkShaderModule>(),
                .pName = ComputeShader.GetDesc().EntryPoint,
                .pSpecializationInfo = nullptr,
            },
        .layout = GetPipelineLayout(Desc.Layout).Native.ToPtr<VkPipelineLayout>(),
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    VkPipeline Pipeline = VK_NULL_HANDLE;
    if (const auto Result = vkCreateComputePipelines(mDevice, VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &Pipeline);
        Result != VK_SUCCESS)
    {
        LogResourceError(String{"Failed to create compute pipeline"}, DebugName);
        return {};
    }

    GPUComputePipeline Resource(Desc);
    Resource.Native = UIntPtr::FromPtr(Pipeline);
    AssignResourceDebugName(Resource.DebugName, DebugName, Pipeline);
    if (Resource.DebugName[0] != '\0')
    {
        SetObjectDebugName(VK_OBJECT_TYPE_PIPELINE, reinterpret_cast<std::uint64_t>(Pipeline), Resource.DebugName);
    }

    return ToTypedHandle<ComputePipelineHandle>(mComputePipelines.Create(std::move(Resource)));
}

void VulkanDevice::DestroyComputePipeline(const ComputePipelineHandle Handle)
{
    Assert(IsComputePipelineValid(Handle));

    const GPUComputePipeline& Pipeline = GetComputePipeline(Handle);
    if (Pipeline.Native.IsValid())
    {
        vkDestroyPipeline(mDevice, Pipeline.Native.ToPtr<VkPipeline>(), nullptr);
    }

    mComputePipelines.Destroy(Handle);
}

RenderPassHandle VulkanDevice::CreateRenderPass(const RenderPassDesc& Desc, const char* DebugName)
{
#if STARDUST_RHI_VALIDATE_DESC
    String Message;
    if (!ValidateRenderPassDesc(Desc, &Message))
    {
        LogResourceError(Message, DebugName);
        return {};
    }
#endif

    Array<VkAttachmentDescription> Attachments;
    Attachments.Reserve(Desc.Attachments.Num());
    for (const RenderPassAttachmentDesc& Attachment : Desc.Attachments)
    {
        Attachments.Add(VkAttachmentDescription{
            .flags = 0,
            .format = static_cast<VkFormat>(Attachment.Format),
            .samples = static_cast<VkSampleCountFlagBits>(Attachment.Samples),
            .loadOp = static_cast<VkAttachmentLoadOp>(Attachment.Load),
            .storeOp = static_cast<VkAttachmentStoreOp>(Attachment.Store),
            .stencilLoadOp = static_cast<VkAttachmentLoadOp>(Attachment.StencilLoad),
            .stencilStoreOp = static_cast<VkAttachmentStoreOp>(Attachment.StencilStore),
            .initialLayout = static_cast<VkImageLayout>(Attachment.InitialLayout),
            .finalLayout = static_cast<VkImageLayout>(Attachment.FinalLayout),
        });
    }

    struct SubpassStorage
    {
        Array<VkAttachmentReference> Input;
        Array<VkAttachmentReference> Color;
        Array<VkAttachmentReference> Resolve;
        Array<std::uint32_t> Preserve;
        VkAttachmentReference DepthStencil{};
        bool HasDepthStencil = false;
    };

    Array<SubpassStorage> SubpassStorages;
    SubpassStorages.Reserve(Desc.Subpasses.Num());

    for (const RenderPassSubpassDesc& SubpassDesc : Desc.Subpasses)
    {
        SubpassStorage& Storage = SubpassStorages.Emplace();
        Storage.Input.Reserve(SubpassDesc.InputAttachments.Num());
        Storage.Color.Reserve(SubpassDesc.ColorAttachments.Num());
        Storage.Resolve.Reserve(SubpassDesc.ResolveAttachments.Num());
        Storage.Preserve.Reserve(SubpassDesc.PreserveAttachments.Num());

        for (const RenderPassAttachmentRef& Reference : SubpassDesc.InputAttachments)
        {
            Storage.Input.Add(MakeAttachmentReference(Reference));
        }
        for (const RenderPassAttachmentRef& Reference : SubpassDesc.ColorAttachments)
        {
            Storage.Color.Add(MakeAttachmentReference(Reference));
        }
        for (const RenderPassAttachmentRef& Reference : SubpassDesc.ResolveAttachments)
        {
            Storage.Resolve.Add(MakeAttachmentReference(Reference));
        }
        for (const UInt32 PreserveAttachment : SubpassDesc.PreserveAttachments)
        {
            Storage.Preserve.Add(PreserveAttachment);
        }

        Storage.HasDepthStencil = SubpassDesc.DepthStencilAttachment.Attachment != gUnusedAttachmentIndex;
        if (Storage.HasDepthStencil)
        {
            Storage.DepthStencil = MakeAttachmentReference(SubpassDesc.DepthStencilAttachment);
        }
    }

    Array<VkSubpassDescription> Subpasses;
    Subpasses.Reserve(SubpassStorages.Num());
    for (SubpassStorage& Storage : SubpassStorages)
    {
        Subpasses.Add(VkSubpassDescription{
            .flags = 0,
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .inputAttachmentCount = static_cast<std::uint32_t>(Storage.Input.Num()),
            .pInputAttachments = Storage.Input.Data(),
            .colorAttachmentCount = static_cast<std::uint32_t>(Storage.Color.Num()),
            .pColorAttachments = Storage.Color.Data(),
            .pResolveAttachments = Storage.Resolve.Empty() ? nullptr : Storage.Resolve.Data(),
            .pDepthStencilAttachment = Storage.HasDepthStencil ? &Storage.DepthStencil : nullptr,
            .preserveAttachmentCount = static_cast<std::uint32_t>(Storage.Preserve.Num()),
            .pPreserveAttachments = Storage.Preserve.Data(),
        });
    }

    Array<VkSubpassDependency> Dependencies;
    Dependencies.Reserve(Desc.Dependencies.Num());
    for (const RenderPassSubpassDependencyDesc& Dependency : Desc.Dependencies)
    {
        Dependencies.Add(VkSubpassDependency{
            .srcSubpass = Dependency.SourceSubpass == gExternalSubpassIndex
                              ? VK_SUBPASS_EXTERNAL
                              : static_cast<std::uint32_t>(Dependency.SourceSubpass),
            .dstSubpass = Dependency.DestinationSubpass == gExternalSubpassIndex
                              ? VK_SUBPASS_EXTERNAL
                              : static_cast<std::uint32_t>(Dependency.DestinationSubpass),
            .srcStageMask = static_cast<VkPipelineStageFlags>(Dependency.SourceStageMask),
            .dstStageMask = static_cast<VkPipelineStageFlags>(Dependency.DestinationStageMask),
            .srcAccessMask = static_cast<VkAccessFlags>(Dependency.SourceAccessMask),
            .dstAccessMask = static_cast<VkAccessFlags>(Dependency.DestinationAccessMask),
            .dependencyFlags = 0,
        });
    }

    const VkRenderPassCreateInfo RenderPassInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .attachmentCount = static_cast<std::uint32_t>(Attachments.Num()),
        .pAttachments = Attachments.Data(),
        .subpassCount = static_cast<std::uint32_t>(Subpasses.Num()),
        .pSubpasses = Subpasses.Data(),
        .dependencyCount = static_cast<std::uint32_t>(Dependencies.Num()),
        .pDependencies = Dependencies.Data(),
    };

    VkRenderPass RenderPass = VK_NULL_HANDLE;
    if (const auto Result = vkCreateRenderPass(mDevice, &RenderPassInfo, nullptr, &RenderPass); Result != VK_SUCCESS)
    {
        LogResourceError(String{"Failed to create render pass"}, DebugName);
        return {};
    }

    GPURenderPass Resource(Desc);
    Resource.Native = UIntPtr::FromPtr(RenderPass);
    AssignResourceDebugName(Resource.DebugName, DebugName, RenderPass);
    if (Resource.DebugName[0] != '\0')
    {
        SetObjectDebugName(VK_OBJECT_TYPE_RENDER_PASS, reinterpret_cast<std::uint64_t>(RenderPass), Resource.DebugName);
    }

    return ToTypedHandle<RenderPassHandle>(mRenderPasses.Create(std::move(Resource)));
}

void VulkanDevice::DestroyRenderPass(const RenderPassHandle Handle)
{
    Assert(IsRenderPassValid(Handle));

    const GPURenderPass& RenderPass = GetRenderPass(Handle);
    if (RenderPass.Native.IsValid())
    {
        vkDestroyRenderPass(mDevice, RenderPass.Native.ToPtr<VkRenderPass>(), nullptr);
    }

    mRenderPasses.Destroy(Handle);
}

CommandPoolHandle VulkanDevice::CreateCommandPool(const CommandPoolDesc& Desc, const char* DebugName)
{
#if STARDUST_RHI_VALIDATE_DESC
    String Message;
    if (!ValidateCommandPoolDesc(Desc, &Message))
    {
        LogResourceError(Message, DebugName);
        return {};
    }
#endif

    const VkCommandPoolCreateInfo PoolInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = static_cast<VkCommandPoolCreateFlags>(Desc.CreateFlags),
        .queueFamilyIndex = mGraphicsQueueFamily,
    };

    VkCommandPool Pool = VK_NULL_HANDLE;
    if (const auto Result = vkCreateCommandPool(mDevice, &PoolInfo, nullptr, &Pool); Result != VK_SUCCESS)
    {
        LogResourceError(String{"Failed to create command pool"}, DebugName);
        return {};
    }

    GPUCommandPool Resource(Desc);
    Resource.Native = UIntPtr::FromPtr(Pool);
    AssignResourceDebugName(Resource.DebugName, DebugName, Pool);
    if (Resource.DebugName[0] != '\0')
    {
        SetObjectDebugName(VK_OBJECT_TYPE_COMMAND_POOL, reinterpret_cast<std::uint64_t>(Pool), Resource.DebugName);
    }

    return ToTypedHandle<CommandPoolHandle>(mCommandPools.Create(std::move(Resource)));
}

void VulkanDevice::DestroyCommandPool(const CommandPoolHandle Handle)
{
    Assert(IsCommandPoolValid(Handle));

    const GPUCommandPool& Pool = GetCommandPool(Handle);
    if (Pool.Native.IsValid())
    {
        vkDestroyCommandPool(mDevice, Pool.Native.ToPtr<VkCommandPool>(), nullptr);
    }

    mCommandPools.Destroy(Handle);
}

CommandBufferHandle VulkanDevice::CreateCommandBuffer(const CommandBufferDesc& Desc, const char* DebugName)
{
#if STARDUST_RHI_VALIDATE_DESC
    String Message;
    if (!ValidateCommandBufferDesc(Desc, &Message))
    {
        LogResourceError(Message, DebugName);
        return {};
    }

    if (!IsCommandPoolValid(Desc.Pool))
    {
        LogResourceError(String{"CommandBufferDesc.Pool handle is not valid"}, DebugName);
        return {};
    }
#endif

    const VkCommandBufferAllocateInfo AllocateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = GetCommandPool(Desc.Pool).Native.ToPtr<VkCommandPool>(),
        .level = static_cast<VkCommandBufferLevel>(Desc.Type),
        .commandBufferCount = 1,
    };

    VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
    if (const auto Result = vkAllocateCommandBuffers(mDevice, &AllocateInfo, &CommandBuffer); Result != VK_SUCCESS)
    {
        LogResourceError(String{"Failed to allocate command buffer"}, DebugName);
        return {};
    }

    GPUCommandBuffer Resource(Desc);
    Resource.Native = UIntPtr::FromPtr(CommandBuffer);
    AssignResourceDebugName(Resource.DebugName, DebugName, CommandBuffer);
    if (Resource.DebugName[0] != '\0')
    {
        SetObjectDebugName(
            VK_OBJECT_TYPE_COMMAND_BUFFER, reinterpret_cast<std::uint64_t>(CommandBuffer), Resource.DebugName);
    }

    return ToTypedHandle<CommandBufferHandle>(mCommandBuffers.Create(std::move(Resource)));
}

void VulkanDevice::DestroyCommandBuffer(const CommandBufferHandle Handle)
{
    Assert(IsCommandBufferValid(Handle));

    const GPUCommandBuffer& CommandBuffer = GetCommandBuffer(Handle);
    if (CommandBuffer.Native.IsValid())
    {
        const CommandPoolHandle PoolHandle = CommandBuffer.GetDesc().Pool;
        Assert(IsCommandPoolValid(PoolHandle));
        const VkCommandBuffer Native = CommandBuffer.Native.ToPtr<VkCommandBuffer>();
        vkFreeCommandBuffers(mDevice, GetCommandPool(PoolHandle).Native.ToPtr<VkCommandPool>(), 1, &Native);
    }

    mCommandBuffers.Destroy(Handle);
}

SurfaceHandle VulkanDevice::CreateSurface(const SurfaceDesc& Desc, const char* DebugName)
{
#if STARDUST_RHI_VALIDATE_DESC
    String Message;
    if (!ValidateSurfaceDesc(Desc, &Message))
    {
        LogResourceError(Message, DebugName);
        return {};
    }
#endif

    Assert(mInstance != VK_NULL_HANDLE);
    if (!EnsureSdlVideo())
    {
        return {};
    }

    SDL_Window* Window = static_cast<SDL_Window*>(Desc.NativeWindow);
    bool OwnsNativeWindow = Desc.OwnsNativeWindow;
    if (Window == nullptr)
    {
        SDL_WindowFlags Flags = SDL_WINDOW_VULKAN;
        if (Desc.Resizable)
        {
            Flags |= SDL_WINDOW_RESIZABLE;
        }

        Window = SDL_CreateWindow(Desc.Title, static_cast<int>(Desc.Width), static_cast<int>(Desc.Height), Flags);
        if (Window == nullptr)
        {
            LogResourceError(String::Format("Failed to create SDL window: {}", SDL_GetError()), DebugName);
            return {};
        }
        OwnsNativeWindow = true;
    }

    VkSurfaceKHR Surface = VK_NULL_HANDLE;
    if (!SDL_Vulkan_CreateSurface(Window, mInstance, nullptr, &Surface) || Surface == VK_NULL_HANDLE)
    {
        LogResourceError(String::Format("Failed to create Vulkan surface: {}", SDL_GetError()), DebugName);
        if (OwnsNativeWindow)
        {
            SDL_DestroyWindow(Window);
        }
        return {};
    }

    VkBool32 PresentSupported = VK_FALSE;
    if (vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, mGraphicsQueueFamily, Surface, &PresentSupported) !=
            VK_SUCCESS ||
        PresentSupported != VK_TRUE)
    {
        LogResourceError(String{"Selected graphics queue does not support presentation to this surface"}, DebugName);
        SDL_Vulkan_DestroySurface(mInstance, Surface, nullptr);
        if (OwnsNativeWindow)
        {
            SDL_DestroyWindow(Window);
        }
        return {};
    }

    GPUSurface Resource(Desc);
    Resource.Native = UIntPtr::FromPtr(Surface);
    Resource.NativeWindow = UIntPtr::FromPtr(Window);
    Resource.OwnsNativeWindow = OwnsNativeWindow;
    AssignResourceDebugName(Resource.DebugName, DebugName, Surface);
    if (Resource.DebugName[0] != '\0')
    {
        SetObjectDebugName(VK_OBJECT_TYPE_SURFACE_KHR, reinterpret_cast<std::uint64_t>(Surface), Resource.DebugName);
    }

    return ToTypedHandle<SurfaceHandle>(mSurfaces.Create(std::move(Resource)));
}

void VulkanDevice::DestroySurface(const SurfaceHandle Handle)
{
    Assert(IsSurfaceValid(Handle));

    const GPUSurface& Surface = GetSurface(Handle);
    if (Surface.Native.IsValid())
    {
        SDL_Vulkan_DestroySurface(mInstance, Surface.Native.ToPtr<VkSurfaceKHR>(), nullptr);
    }
    if (Surface.OwnsNativeWindow && Surface.NativeWindow.IsValid())
    {
        SDL_DestroyWindow(Surface.NativeWindow.ToPtr<SDL_Window*>());
    }

    mSurfaces.Destroy(Handle);
}

SwapchainHandle VulkanDevice::CreateSwapchain(const SwapchainDesc& Desc, const char* DebugName)
{
#if STARDUST_RHI_VALIDATE_DESC
    String Message;
    if (!ValidateSwapchainDesc(Desc, &Message))
    {
        LogResourceError(Message, DebugName);
        return {};
    }

    if (!IsSurfaceValid(Desc.Surface))
    {
        LogResourceError(String{"SwapchainDesc.Surface handle is not valid"}, DebugName);
        return {};
    }
#endif

    Assert(mDevice != VK_NULL_HANDLE);
    const GPUSurface& SurfaceResource = GetSurface(Desc.Surface);
    const VkSurfaceKHR Surface = SurfaceResource.Native.ToPtr<VkSurfaceKHR>();

    VkSurfaceCapabilitiesKHR Capabilities{};
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, Surface, &Capabilities) != VK_SUCCESS)
    {
        LogResourceError(String{"Failed to query surface capabilities"}, DebugName);
        return {};
    }

    std::uint32_t FormatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, Surface, &FormatCount, nullptr);
    if (FormatCount == 0)
    {
        LogResourceError(String{"No surface formats available"}, DebugName);
        return {};
    }
    Array<VkSurfaceFormatKHR> Formats(FormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, Surface, &FormatCount, Formats.Data());

    std::uint32_t PresentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, Surface, &PresentModeCount, nullptr);
    if (PresentModeCount == 0)
    {
        LogResourceError(String{"No present modes available"}, DebugName);
        return {};
    }
    Array<VkPresentModeKHR> PresentModes(PresentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, Surface, &PresentModeCount, PresentModes.Data());

    UInt32 RequestedWidth = Desc.Width;
    UInt32 RequestedHeight = Desc.Height;
    if (RequestedWidth == 0 || RequestedHeight == 0)
    {
        int DrawableWidth = 0;
        int DrawableHeight = 0;
        if (SurfaceResource.NativeWindow.IsValid())
        {
            SDL_GetWindowSizeInPixels(
                SurfaceResource.NativeWindow.ToPtr<SDL_Window*>(), &DrawableWidth, &DrawableHeight);
        }
        if (RequestedWidth == 0)
        {
            RequestedWidth = DrawableWidth > 0 ? static_cast<UInt32>(DrawableWidth)
                                               : static_cast<UInt32>(Capabilities.currentExtent.width);
        }
        if (RequestedHeight == 0)
        {
            RequestedHeight = DrawableHeight > 0 ? static_cast<UInt32>(DrawableHeight)
                                                 : static_cast<UInt32>(Capabilities.currentExtent.height);
        }
    }

    const VkSurfaceFormatKHR SurfaceFormat = ChooseSwapchainFormat(Formats, Desc.Format, Desc.ColorSpace);
    const VkPresentModeKHR PresentMode = ChoosePresentMode(PresentModes, Desc.PresentMode);
    const VkExtent2D Extent = ChooseSwapchainExtent(Capabilities, RequestedWidth, RequestedHeight);
    if (Extent.width == 0 || Extent.height == 0)
    {
        LogResourceError(String{"Swapchain extent is zero (window minimized?)"}, DebugName);
        return {};
    }

    std::uint32_t ImageCount = Desc.PreferredImageCount;
    if (ImageCount == 0)
    {
        ImageCount = Capabilities.minImageCount + 1;
    }
    ImageCount = std::max(ImageCount, Capabilities.minImageCount);
    if (Capabilities.maxImageCount > 0)
    {
        ImageCount = std::min(ImageCount, Capabilities.maxImageCount);
    }

    const VkSwapchainCreateInfoKHR CreateInfo{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = Surface,
        .minImageCount = ImageCount,
        .imageFormat = SurfaceFormat.format,
        .imageColorSpace = SurfaceFormat.colorSpace,
        .imageExtent = Extent,
        .imageArrayLayers = 1,
        .imageUsage = static_cast<VkImageUsageFlags>(Desc.ImageUsage),
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .preTransform = Capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = PresentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    VkSwapchainKHR Swapchain = VK_NULL_HANDLE;
    if (const auto Result = vkCreateSwapchainKHR(mDevice, &CreateInfo, nullptr, &Swapchain); Result != VK_SUCCESS)
    {
        LogResourceError(String{"Failed to create swapchain"}, DebugName);
        return {};
    }

    std::uint32_t SwapchainImageCount = 0;
    vkGetSwapchainImagesKHR(mDevice, Swapchain, &SwapchainImageCount, nullptr);
    Array<VkImage> SwapchainImages(SwapchainImageCount);
    vkGetSwapchainImagesKHR(mDevice, Swapchain, &SwapchainImageCount, SwapchainImages.Data());

    GPUSwapchain Resource(Desc);
    Resource.Native = UIntPtr::FromPtr(Swapchain);
    Resource.Format = static_cast<PixelFormat>(SurfaceFormat.format);
    Resource.ColorSpace = static_cast<ColorSpace>(SurfaceFormat.colorSpace);
    Resource.Width = Extent.width;
    Resource.Height = Extent.height;
    Resource.Images.Reserve(SwapchainImageCount);
    Resource.Views.Reserve(SwapchainImageCount);
    AssignResourceDebugName(Resource.DebugName, DebugName, Swapchain);
    if (Resource.DebugName[0] != '\0')
    {
        SetObjectDebugName(
            VK_OBJECT_TYPE_SWAPCHAIN_KHR, reinterpret_cast<std::uint64_t>(Swapchain), Resource.DebugName);
    }

    for (std::uint32_t Index = 0; Index < SwapchainImageCount; ++Index)
    {
        TextureDesc ImageDesc{};
        ImageDesc.Dimension = TextureDimension::Dim2D;
        ImageDesc.Format = Resource.Format;
        ImageDesc.Width = Resource.Width;
        ImageDesc.Height = Resource.Height;
        ImageDesc.Depth = 1;
        ImageDesc.MipLevels = 1;
        ImageDesc.ArrayLayers = 1;
        ImageDesc.Samples = SampleCount::Count1;
        ImageDesc.Usage = Desc.ImageUsage;
        ImageDesc.Tiling = TextureTiling::Optimal;
        ImageDesc.InitialLayout = TextureLayout::Undefined;

        GPUTexture ImageResource(ImageDesc);
        ImageResource.Native = UIntPtr::FromPtr(SwapchainImages[Index]);
        ImageResource.Allocation = UIntPtr::Null();
        const String ImageName = String::Format("{}[{}]", Resource.DebugName, Index);
        char ImageDebugName[64]{};
        AssignResourceDebugName(ImageDebugName, ImageName.CStr(), SwapchainImages[Index]);
        AssignResourceDebugName(ImageResource.DebugName, ImageDebugName, nullptr);
        if (ImageResource.DebugName[0] != '\0')
        {
            SetObjectDebugName(
                VK_OBJECT_TYPE_IMAGE, reinterpret_cast<std::uint64_t>(SwapchainImages[Index]), ImageResource.DebugName);
        }

        const TextureHandle ImageHandle = ToTypedHandle<TextureHandle>(mTextures.Create(std::move(ImageResource)));

        TextureViewDesc ViewDesc{};
        ViewDesc.Texture = ImageHandle;
        ViewDesc.Dimension = TextureViewDimension::Dim2D;
        ViewDesc.Format = Resource.Format;
        ViewDesc.Aspect = TextureAspectFlag::Color;
        ViewDesc.BaseMipLevel = 0;
        ViewDesc.MipLevelCount = 1;
        ViewDesc.BaseArrayLayer = 0;
        ViewDesc.ArrayLayerCount = 1;

        const TextureViewHandle ViewHandle =
            CreateTextureView(ViewDesc, ImageDebugName[0] != '\0' ? ImageDebugName : nullptr);
        if (ViewHandle.IsNull())
        {
            DestroyTexture(ImageHandle);
            for (const TextureViewHandle ExistingView : Resource.Views)
            {
                DestroyTextureView(ExistingView);
            }
            for (const TextureHandle ExistingImage : Resource.Images)
            {
                DestroyTexture(ExistingImage);
            }
            vkDestroySwapchainKHR(mDevice, Swapchain, nullptr);
            LogResourceError(String{"Failed to create swapchain image view"}, DebugName);
            return {};
        }

        Resource.Images.Add(ImageHandle);
        Resource.Views.Add(ViewHandle);
    }

    return ToTypedHandle<SwapchainHandle>(mSwapchains.Create(std::move(Resource)));
}

void VulkanDevice::DestroySwapchain(const SwapchainHandle Handle)
{
    Assert(IsSwapchainValid(Handle));

    GPUSwapchain& Swapchain = GetSwapchain(Handle);
    for (const TextureViewHandle View : Swapchain.Views)
    {
        if (IsTextureViewValid(View))
        {
            DestroyTextureView(View);
        }
    }
    Swapchain.Views.Clear();

    for (const TextureHandle Image : Swapchain.Images)
    {
        if (IsTextureValid(Image))
        {
            DestroyTexture(Image);
        }
    }
    Swapchain.Images.Clear();

    if (Swapchain.Native.IsValid())
    {
        vkDestroySwapchainKHR(mDevice, Swapchain.Native.ToPtr<VkSwapchainKHR>(), nullptr);
    }

    mSwapchains.Destroy(Handle);
}

#endif
