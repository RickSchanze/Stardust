#include <spdlog/spdlog.h>

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <mimalloc-new-delete.h>
#include <mimalloc.h>

#include "Core/Logging/Logger.h"
#include "Core/Memory/Memory.h"
#include "Core/String/String.h"
#include "Core/String/StringView.h"

int Main()
{
    const int MimallocVersion = mi_version();

    // 展开为一次 log("[Engine] [{}:{}] hello from {}", file, line, "Main")
    LogDebug(Engine, "hello from {}", "Main");
    LogDebug(Render, "origin length {}", 0.0f);
    LogInfo(Test, "value={}", 42);

    const StringView Name = "Stardust";
    const String Title{Name};

    int* Value = New<int>(42);
    Delete(Value);

    const glm::vec3 Origin{0.0f, 0.0f, 0.0f};

    return 0;
}

int main()
{
    return Main();
}
