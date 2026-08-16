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
#include "Core/Math/Color.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Rect.h"
#include "Core/Math/Vector.h"
#include "Core/Memory/Memory.h"
#include "Core/Profiler/Profiler.h"
#include "Core/String/String.h"
#include "Core/String/StringView.h"

enum class NamedColor
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
                                            NamedColor::Green,
                                            MemberNamed{},
                                            FreeNamed{},
                                            StringView{"Stardust"});
    LogInfo(Test, "{}", Formatted);
}

static void SmokeTestMath()
{
    const Vector3f Origin{0.0f, 1.0f, 0.0f};
    const Vector3f Forward = Vector3f::UnitZ();
    const Vector3f Side = Origin.Cross(Forward).Normalized();
    Assert(Side.IsNormalized());
    Assert(Origin.Dot(Forward) == 0.0f);

    const Vector2f Screen{1920.0f, 1080.0f};
    Assert(Screen.LengthSquared() > 0.0f);

    const Matrix4x4f Transform = Matrix4x4f::Translation(Origin) * Matrix4x4f::Scaling(Vector3f{2.0f});
    const Vector3f Transformed = Transform.TransformPoint(Vector3f::UnitX());
    Assert(Transformed.Equals(Vector3f{2.0f, 1.0f, 0.0f}, 0.0001f));

    const Rectf Box = Rectf::FromCenterSize(Vector2f{0.0f, 0.0f}, Vector2f{2.0f, 2.0f});
    Assert(Box.Contains(Vector2f{0.5f, 0.5f}));
    Assert(Box.Overlaps(Rectf{-1.0f, -1.0f, 0.0f, 0.0f}));

    const Rect3Df Volume{Vector3f{-1.0f}, Vector3f{1.0f}};
    Assert(Volume.Contains(Vector3f{}));
    Assert(Volume.Volume() == 8.0f);

    LogInfo(Test, "origin={} matrix={} rect={}", Origin, Transform, Box);
}

static void SmokeTestColor()
{
    const Colorf Red{1.0f, 0.0f, 0.0f};
    const Color8 Red8{255, 0, 0};
    const Colorf FromBytes = Red8;
    const Color8 FromNormalized = Red;
    Assert(FromBytes == Red);
    Assert(FromNormalized == Red8);
    Assert(Colorf{Vector3f{0.0f, 1.0f, 0.0f}} == Colorf::Green());
    Assert(Color8{Vector3<std::uint8_t>{0, 0, 255}} == Color8::Blue());
    Assert(Colorf(255, 0, 0) == Red);
    Assert(Color8(1.0f, 0.0f, 0.0f) == Red8);

    LogInfo(Test, "color={} color8={}", Red, Red8);
}

static void SmokeTestProfiler()
{
    PerformanceCounter _{};
    PerformanceCounter _{"Named"};
    PerformanceCounter _{"Colored", 0x4488FF};
}

static int Main()
{
    PerformanceCounter _{"Main"};

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

    (void)Title;

    SmokeTestContainersAndFormat();
    SmokeTestMath();
    SmokeTestColor();
    SmokeTestProfiler();

    MarkFrame();
    return 0;
}

int main()
{
    return Main();
}
