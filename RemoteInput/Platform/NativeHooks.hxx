#ifndef NATIVEHOOKS_HXX_INCLUDED
#define NATIVEHOOKS_HXX_INCLUDED

#include <windows.h>
#include <memory>
#include <thread>
#include "JavaInternal.hxx"
#include "../EIOS.hxx"

void InitialiseHooks();

typedef void (__stdcall *JavaNativeBlit_t)(JNIEnv *env, jobject joSelf, jobject srcData, jobject dstData, jobject clip, jint srcx, jint srcy, jint dstx, jint dsty, jint width, jint height, jint rmask, jint gmask, jint bmask, jboolean needLut);

JavaNativeBlit_t o_JavaNativeBlit;
void __stdcall JavaNativeBlit(JNIEnv *env, jobject joSelf, jobject srcData, jobject dstData, jobject clip, jint srcx, jint srcy, jint dstx, jint dsty, jint width, jint height, jint rmask, jint gmask, jint bmask, jboolean needLut)
{
    //Hook..
    typedef SurfaceDataOps* (__stdcall *SurfaceData_GetOps)(JNIEnv *env, jobject sData);
    static SurfaceData_GetOps GetOps = (SurfaceData_GetOps)GetProcAddress(GetModuleHandle("awt.dll"), "_SurfaceData_GetOps@8");

    if (!GetOps)
    {
        return;
    }

    SurfaceDataOps* srcOps = GetOps(env, srcData);
    SurfaceDataRasInfo srcInfo = {0};
    srcInfo.bounds.x1 = srcx;
    srcInfo.bounds.y1 = srcy;
    srcInfo.bounds.x2 = srcx + width;
    srcInfo.bounds.y2 = srcy + height;

    if (srcOps->Lock(env, srcOps, &srcInfo, needLut ? (SD_LOCK_READ | SD_LOCK_LUT) : SD_LOCK_READ) == SD_SUCCESS)
    {
        srcOps->GetRasInfo(env, srcOps, &srcInfo);
        if (srcInfo.rasBase)
        {
            void *rasBase = reinterpret_cast<std::uint8_t*>(srcInfo.rasBase) + (srcInfo.scanStride * srcy) + (srcInfo.pixelStride * srcx);
            bool isRasterAligned = !(srcInfo.scanStride & 0x03);

            extern std::unique_ptr<MemoryMap<char>> globalMap;
            ImageData* info = reinterpret_cast<ImageData*>(globalMap->data());
            std::uint8_t* dest = reinterpret_cast<std::uint8_t*>(info) + info->imgoff;

            if (isRasterAligned)
            {
                memcpy(dest, rasBase, (srcInfo.scanStride / srcInfo.pixelStride) * height * srcInfo.pixelStride);
            }
            else
            {
                for (int i = 0; i < height; ++i)
                {
                    int offset = (srcInfo.scanStride / srcInfo.pixelStride) * i;
                    memcpy(dest + offset, rasBase, (srcInfo.scanStride / srcInfo.pixelStride));
                    rasBase = static_cast<void*>(reinterpret_cast<std::uint8_t*>(rasBase) + srcInfo.scanStride);
                }
            }

            if (srcOps->Release)
            {
                srcOps->Release(env, srcOps, &srcInfo);
            }
        }

        if (srcOps->Unlock)
        {
            srcOps->Unlock(env, srcOps, &srcInfo);
        }
    }

    //Original
    o_JavaNativeBlit(env, joSelf, srcData, dstData, clip, srcx, srcy, dstx, dsty, width, height, rmask, gmask, bmask, needLut);
}

void StartHook()
{
    InitialiseHooks();
}

void InitialiseHooks()
{
    //std::thread([]{
        while(!GetModuleHandle("awt.dll"))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        typedef int __stdcall (*MH_Initialize)(void);
        typedef int __stdcall (*MH_CreateHook)(LPVOID pTarget, LPVOID pDetour, LPVOID *ppOriginal);
        typedef int __stdcall (*MH_EnableHook)(LPVOID pTarget);

        HMODULE library = LoadLibrary("C:/Users/Brandon/Desktop/MinHook.x86.dll");
        MH_Initialize initialize = (MH_Initialize)GetProcAddress(library, "MH_Initialize");
        MH_CreateHook hook = (MH_CreateHook)GetProcAddress(library, "MH_CreateHook");
        MH_EnableHook enable = (MH_EnableHook)GetProcAddress(library, "MH_EnableHook");


        JavaNativeBlit_t original = (JavaNativeBlit_t)GetProcAddress(GetModuleHandle("awt.dll"), "_Java_sun_java2d_windows_GDIBlitLoops_nativeBlit@60");

        initialize();
        hook((void*)original, (void*)&JavaNativeBlit, (void**)&o_JavaNativeBlit);
        enable((void*)original);
    //}).detach();
}

#endif // NATIVEHOOKS_HXX_INCLUDED
