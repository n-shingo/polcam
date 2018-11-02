#ifndef __POLCAM_H__
#define __POLCAM_H__


#define TAB1 "  "
#define TAB2 "    "
#define TAB3 "      "


#ifdef _WIN32
#define KEY_U 2490368
#define KEY_D 2621440
#define KEY_L 2424832
#define KEY_R 2555904
#elif __linux__
#define KEY_U 65362
#define KEY_D 65364
#define KEY_L 65361
#define KEY_R 65363
#endif

enum ShowMode {
    Average,
    Polarization,
    Stokes,
    XoLP,
    All
};

#endif // !__POLCAM_H__

