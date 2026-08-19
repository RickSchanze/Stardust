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

    virtual void Startup() {}

    virtual void Shutdown() {}

    static void CreateInstance()
    {
        if (mInstance == nullptr)
        {
            mInstance = new T();
        }
    }

    static void DestroyInstance()
    {
        if (mInstance != nullptr)
        {
            delete mInstance;
            mInstance = nullptr;
        }
    }

private:
    inline static T* mInstance = nullptr;
};
