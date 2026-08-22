#pragma once

template <typename T>
class Singleton
{
public:
    static T& GetInstance()
    {
        return *mInstance;
    }

    virtual ~Singleton() = default;

    virtual bool Startup()
    {
        return true;
    }

    virtual bool Shutdown()
    {
        return true;
    }

    static bool CreateInstance()
    {
        if (mInstance == nullptr)
        {
            mInstance = new T();
            return static_cast<Singleton<T>*>(mInstance)->Startup();
        }
        Assert(false);
        return false;
    }

    static bool DestroyInstance()
    {
        if (mInstance != nullptr)
        {
            const bool Success = static_cast<Singleton<T>*>(mInstance)->Shutdown();
            delete mInstance;
            mInstance = nullptr;
            return Success;
        }
        Assert(false);
        return false;
    }

private:
    inline static T* mInstance = nullptr;
};
