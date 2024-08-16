#pragma once
#ifndef IMAGEOVERLAY_H //headerÀÌ¸§_H
#define IMAGEOVERLAY_H
#define IMAGEWIDTH 512
#define IMAGEHEIGHT 512
#define CHANNEL 3

#ifdef __cplusplus 
		extern"C" {
#endif

	extern void transform_to_blending(unsigned char* inimage, unsigned char* outimage);
    extern void transform_foreground(unsigned char* inimage, unsigned char* outimage);
    extern void imageoverlay(unsigned char* background, unsigned char* foreground, unsigned char* outimage);

#ifdef __cplusplus

		} /*end of the 'externC' block*/
#endif







#endif	// IMAGEOVERLAY_H