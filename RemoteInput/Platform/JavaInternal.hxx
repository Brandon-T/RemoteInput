#ifndef JAVAINTERNAL_HXX_INCLUDED
#define JAVAINTERNAL_HXX_INCLUDED

/*
 * Copyright (c) 1999, 2010, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * LICENSED UNDER GPL2.
 */

#include <jni.h>

typedef struct {
    jint x1;
    jint y1;
    jint x2;
    jint y2;
} SurfaceDataBounds;

typedef struct {
    SurfaceDataBounds   bounds;
    void                *rasBase;
    jint                pixelBitOffset;
    jint                pixelStride;
    jint                scanStride;
    unsigned int        lutSize;
    jint                *lutBase;
    unsigned char       *invColorTable;
    char                *redErrTable;
    char                *grnErrTable;
    char                *bluErrTable;
    int                 *invGrayTable;
    union {
        void            *align;
        char            data[64];
    } priv;
} SurfaceDataRasInfo;

typedef struct _SurfaceDataOps SurfaceDataOps;

typedef jint LockFunc(JNIEnv *env,
                      SurfaceDataOps *ops,
                      SurfaceDataRasInfo *rasInfo,
                      jint lockflags);

typedef void GetRasInfoFunc(JNIEnv *env,
                            SurfaceDataOps *ops,
                            SurfaceDataRasInfo *pRasInfo);

typedef void ReleaseFunc(JNIEnv *env,
                         SurfaceDataOps *ops,
                         SurfaceDataRasInfo *pRasInfo);

typedef void UnlockFunc(JNIEnv *env,
                        SurfaceDataOps *ops,
                        SurfaceDataRasInfo *pRasInfo);

typedef void SetupFunc(JNIEnv *env,
                       SurfaceDataOps *ops);

typedef void DisposeFunc(JNIEnv *env,
                         SurfaceDataOps *ops);

#define SD_FAILURE              -1
#define SD_SUCCESS              0
#define SD_SLOWLOCK             1
#define SD_LOCK_READ            (1 << 0)
#define SD_LOCK_WRITE           (1 << 1)
#define SD_LOCK_RD_WR           (SD_LOCK_READ | SD_LOCK_WRITE)
#define SD_LOCK_LUT             (1 << 2)
#define SD_LOCK_INVCOLOR        (1 << 3)
#define SD_LOCK_INVGRAY         (1 << 4)
#define SD_LOCK_FASTEST         (1 << 5)
#define SD_LOCK_PARTIAL         (1 << 6)
#define SD_LOCK_PARTIAL_WRITE   (SD_LOCK_WRITE | SD_LOCK_PARTIAL)
#define SD_LOCK_NEED_PIXELS     (SD_LOCK_READ | SD_LOCK_PARTIAL)


struct _SurfaceDataOps {
    LockFunc            *Lock;
    GetRasInfoFunc      *GetRasInfo;
    ReleaseFunc         *Release;
    UnlockFunc          *Unlock;
    SetupFunc           *Setup;
    DisposeFunc         *Dispose;
    jobject             sdObject;
};



#if defined(_WIN32) || defined(_WIN64)
typedef struct _GDIWinSDOps GDIWinSDOps;

typedef HDC GetDCFunc(JNIEnv *env,
                      GDIWinSDOps *wsdo,
                      jint flags,
                      jint *patrop,
                      jobject clip,
                      jobject comp,
                      jint color);

typedef void ReleaseDCFunc(JNIEnv *env,
                           GDIWinSDOps *wsdo,
                           HDC hdc);


typedef void InvalidateSDFunc(JNIEnv *env,
                              GDIWinSDOps *wsdo);

struct _GDIWinSDOps {
    SurfaceDataOps      sdOps;
    LONG                timeStamp;
    jboolean            invalid;
    GetDCFunc           *GetDC;
    ReleaseDCFunc       *ReleaseDC;
    InvalidateSDFunc    *InvalidateSD;
    jint                lockType;
    jint                lockFlags;
    jobject             peer;
    HWND                window;
    RECT                insets;
    jint                depth;
    jint                pixelStride;
    DWORD               pixelMasks[3];
    HBITMAP             bitmap;
    HBITMAP             oldmap;
    HDC                 bmdc;
    int                 bmScanStride;
    int                 bmWidth;
    int                 bmHeight;
    void                *bmBuffer;
    jboolean            bmCopyToScreen;
    void                *brush;
    jint                brushclr;
    void                *pen;
    jint                penclr;
    int                 x, y, w, h;
    void                *surfaceLock;
    void                *device;
};

typedef struct tagBitmapHeader {
    BITMAPINFOHEADER bmiHeader;
    union {
        DWORD dwMasks[3];
        RGBQUAD palette[256];
    } colors;
} BmiType;
#endif

#endif // JAVAINTERNAL_HXX_INCLUDED
