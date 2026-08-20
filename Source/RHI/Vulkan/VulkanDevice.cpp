#include "VulkanDevice.h"

#if STARDUST_RHI_BUILD_VULKAN

#include <cstdint>
#include <cstring>

#if STARDUST_RHI_ENABLE_VALIDATION
#include <cstdlib>
#endif

#include "Core/Container/Array.h"
#include "Core/Debug/Debug.h"
#include "Core/Logging/Logger.h"
#include "Core/Profiler/Profiler.h"

#if STARDUST_RHI_ENABLE_VALIDATION
#include "Core/IO/Directory.h"
#include "Core/IO/Path.h"
#include "Core/String/String.h"
#endif

using namespace RHI;

namespace
{
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
        std::uint32_t QueueFamily = 0;
        if (!FindGraphicsQueueFamily(PhysicalDevice, QueueFamily))
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
        LogCritical(RHI, "No suitable Vulkan 1.3 GPU with a graphics queue");
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

    const VkDeviceCreateInfo CreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &Features2,
        .flags = 0,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &QueueInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = nullptr,
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

BufferHandle VulkanDevice::CreateBuffer([[maybe_unused]] const BufferDesc& Desc)
{
    return {};
}

void VulkanDevice::DestroyBuffer([[maybe_unused]] const BufferHandle Handle) {}

TextureHandle VulkanDevice::CreateTexture([[maybe_unused]] const TextureDesc& Desc)
{
    return {};
}

void VulkanDevice::DestroyTexture([[maybe_unused]] const TextureHandle Handle) {}

TextureViewHandle VulkanDevice::CreateTextureView([[maybe_unused]] const TextureViewDesc& Desc)
{
    return {};
}

void VulkanDevice::DestroyTextureView([[maybe_unused]] const TextureViewHandle Handle) {}

SamplerHandle VulkanDevice::CreateSampler([[maybe_unused]] const SamplerDesc& Desc)
{
    return {};
}

void VulkanDevice::DestroySampler([[maybe_unused]] const SamplerHandle Handle) {}

ShaderHandle VulkanDevice::CreateShader([[maybe_unused]] const ShaderDesc& Desc)
{
    return {};
}

void VulkanDevice::DestroyShader([[maybe_unused]] const ShaderHandle Handle) {}

DescriptorSetLayoutHandle VulkanDevice::CreateDescriptorSetLayout([[maybe_unused]] const DescriptorSetLayoutDesc& Desc)
{
    return {};
}

void VulkanDevice::DestroyDescriptorSetLayout([[maybe_unused]] const DescriptorSetLayoutHandle Handle) {}

PipelineLayoutHandle VulkanDevice::CreatePipelineLayout([[maybe_unused]] const PipelineLayoutDesc& Desc)
{
    return {};
}

void VulkanDevice::DestroyPipelineLayout([[maybe_unused]] const PipelineLayoutHandle Handle) {}

GraphicsPipelineHandle VulkanDevice::CreateGraphicsPipeline([[maybe_unused]] const GraphicsPipelineDesc& Desc)
{
    return {};
}

void VulkanDevice::DestroyGraphicsPipeline([[maybe_unused]] const GraphicsPipelineHandle Handle) {}

ComputePipelineHandle VulkanDevice::CreateComputePipeline([[maybe_unused]] const ComputePipelineDesc& Desc)
{
    return {};
}

void VulkanDevice::DestroyComputePipeline([[maybe_unused]] const ComputePipelineHandle Handle) {}

RenderPassHandle VulkanDevice::CreateRenderPass([[maybe_unused]] const RenderPassDesc& Desc)
{
    return {};
}

void VulkanDevice::DestroyRenderPass([[maybe_unused]] const RenderPassHandle Handle) {}

CommandPoolHandle VulkanDevice::CreateCommandPool([[maybe_unused]] const CommandPoolDesc& Desc)
{
    return {};
}

void VulkanDevice::DestroyCommandPool([[maybe_unused]] const CommandPoolHandle Handle) {}

CommandBufferHandle VulkanDevice::CreateCommandBuffer([[maybe_unused]] const CommandBufferDesc& Desc)
{
    return {};
}

void VulkanDevice::DestroyCommandBuffer([[maybe_unused]] const CommandBufferHandle Handle) {}

#endif
