#include <spdlog/spdlog.h>

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <mimalloc-new-delete.h>
#include <mimalloc.h>

int Main()
{
    const int MimallocVersion = mi_version();
    spdlog::info("Stardust boot, mimalloc {}", MimallocVersion);

    const glm::vec3 Origin{0.0f, 0.0f, 0.0f};
    spdlog::info("origin length {}", glm::length(Origin));

    return 0;
}

// 入口保持小写 main，满足 C++ 链接约定；业务入口使用大驼峰 Main。
int main()
{
    return Main();
}
