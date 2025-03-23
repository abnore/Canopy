#ifndef COMMON_H
#define COMMON_H

typedef struct canopy_videomode
{
    /*! The width, in screen coordinates, of the video mode.
     */
    int width;
    /*! The height, in screen coordinates, of the video mode.
     */
    int height;
    /*! The bit depth of the red channel of the video mode.
     */
    int red_bits;
    /*! The bit depth of the green channel of the video mode.
     */
    int green_bits;
    /*! The bit depth of the blue channel of the video mode.
     */
    int blue_bits;
    /*! The refresh rate, in Hz, of the video mode.
     */
    int refresh_rate;
} canopy_videomode;


#endif // COMMON_H
