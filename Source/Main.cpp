#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <mimalloc-new-delete.h>
#include <mimalloc.h>
#include <spdlog/spdlog.h>

#include "Core/Container/Array.h"
#include "Core/Container/FixedArray.h"
#include "Core/Container/HashMap.h"
#include "Core/Container/Map.h"
#include "Core/Container/Set.h"
#include "Core/Container/Span.h"
#include "Core/Debug/Debug.h"
#include "Core/Logging/Logger.h"
#include "Core/Memory/Memory.h"
#include "Core/String/String.h"
#include "Core/String/StringView.h"

enum class Color
{
    Red,
    Green,
    Blue,
};

struct MemberNamed
{
    [[nodiscard]] String ToString() const
    {
        return String{"MemberNamed"};
    }
};

struct FreeNamed
{
};

[[nodiscard]] inline String ToString(const FreeNamed&)
{
    return String{"FreeNamed"};
}

static void SmokeTestContainersAndFormat()
{
    DebugAssert(true);

    Array<int> Numbers;
    Numbers.Add(1);
    Numbers.Add(2);
    Numbers.Add(3);
    Assert(Numbers.Num() == 3);
    Assert(Numbers[1] == 2);
    Numbers.RemoveAt(1);
    Assert(Numbers.Contains(3));

    FixedArray<int, 3> Fixed{1, 2, 3};
    Assert(Fixed[0] == 1);

    const Span<int> NumberSpan = Numbers.AsSpan();
    Assert(NumberSpan.Num() == 2);

    HashMap<String, int> Lookup;
    Lookup.Add(String{"hp"}, 100);
    Assert(Lookup.Contains(String{"hp"}));
    Assert(*Lookup.Find(String{"hp"}) == 100);

    Map<int, String> Ordered;
    Ordered.Add(2, String{"b"});
    Ordered.Add(1, String{"a"});
    Assert(Ordered.Begin()->first == 1);

    Set<int> Unique;
    Assert(Unique.Add(1));
    Assert(!Unique.Add(1));
    Assert(Unique.Contains(1));

    const String Formatted = String::Format("n={:04d} ok={} color={} member={} free={} name={}",
                                            42,
                                            true,
                                            Color::Green,
                                            MemberNamed{},
                                            FreeNamed{},
                                            StringView{"Stardust"});
    LogInfo(Test, "{}", Formatted);
}

static int Main()
{
    StringView View = "Main";
    LogDebug(Engine, "hello from {}", View);
    LogDebug(Render, "origin length {}", 0.0f);
    LogInfo(Test, "value={}", 42);
    LogError(Test, "value={}", 42);
    LogWarn(Test, "value={}", 42);
    LogCritical(Test, "value={}", 42);

    const StringView Name = "Stardust";
    const String Title{Name};

    int* Value = New<int>(42);
    Delete(Value);

    const glm::vec3 Origin{0.0f, 0.0f, 0.0f};
    (void)Origin;
    (void)Title;

    SmokeTestContainersAndFormat();

    return 0;
}

int main()
{
    return Main();
}
