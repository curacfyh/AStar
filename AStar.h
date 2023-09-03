#ifndef ASTAR_H
#define ASTAR_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

extern "C" __declspec(dllexport) char *FindPath(char *mapFilePath, int startX, int startY, int endX, int endY);

#ifdef __cplusplus
}
#endif

#endif // ASTAR_H