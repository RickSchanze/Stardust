#include "RHI/SurfaceWindowManager.h"

#include "Core/Debug/Debug.h"
#include "Core/Memory/Memory.h"

namespace RHI
{
    bool SurfaceWindowManager::Shutdown()
    {
        for (SurfaceWindow* const Window : mWindows)
        {
            if (Window != nullptr)
            {
                Window->UnInit();
                Delete(Window);
            }
        }
        mWindows.Clear();
        return true;
    }

    SurfaceWindow* SurfaceWindowManager::Create(const SurfaceWindowDesc& Desc, const char* DebugName)
    {
        SurfaceWindow* const Created = New<SurfaceWindow>();
        if (!Created->Init(Desc, DebugName))
        {
            Delete(Created);
            return nullptr;
        }

        mWindows.Add(Created);
        return Created;
    }

    void SurfaceWindowManager::Destroy(SurfaceWindow* Window)
    {
        if (Window == nullptr)
        {
            return;
        }

        for (std::size_t Index = 0; Index < mWindows.Num(); ++Index)
        {
            if (mWindows[Index] != Window)
            {
                continue;
            }

            Window->UnInit();
            Delete(Window);
            mWindows.RemoveAt(Index);
            return;
        }

        Assert(false);
    }

} // namespace RHI
