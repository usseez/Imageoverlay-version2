#include "../include/imageoverlay.h"

#define CLIP(value) ((value) > 255 ? 255 : ((value) < 0 ? 0 : (value)))


void normalize(unsigned char *, float *, int) ;




void transform_to_blending(unsigned char* inimage, unsigned char* outimage)//from yuv to bgr
{
    for(int pixel = 0, k = 0; pixel < IMAGEHEIGHT * IMAGEWIDTH * CHANNEL; pixel += 3, k++)
    {
        
        outimage[pixel    ] = (unsigned char)CLIP(1.164 * (float)(inimage[pixel + 0] - 16.0) + 2.018 * (float)(inimage[pixel + 1] - 128.0));//B
        //outimage[pixel    ] = (unsigned char)((float)(inimage[pixel + 0]) + 2.032 * (float)(inimage[pixel + 1]));//B
        outimage[pixel + 1] = (unsigned char)CLIP(1.164 * (float)(inimage[pixel + 0] - 16) - 0.813 * (float)(inimage[pixel + 2] - 128) - 0.391 * (float)(inimage[pixel + 1] - 128));//G
        //outimage[pixel + 1] = (unsigned char)((float)(inimage[pixel + 0]) - 0.395 * (float)inimage[pixel + 1] - 0.581 * (float)(inimage[pixel + 2]));//G
        outimage[pixel + 2] = (unsigned char)CLIP(1.164 * (float)(inimage[pixel + 0] - 16) + 1.596 * (float)(inimage[pixel + 2] - 128));//R
        //outimage[pixel + 2] = (unsigned char)((float)(inimage[pixel + 0]) + 1.13983 * (float)(inimage[pixel + 2]));//R
        //printf("R=%d, G=%d, B=%d\n", outimage[pixel + 2], outimage[pixel + 1], outimage[pixel + 0]);
        if(outimage[k] < 0) outimage[k] = 0;
        if(outimage[k] > 255)outimage[k] = 255;
        printf("Pixel %d: R=%d, G=%d, B=%d, A=%d\n", k, outimage[k + 2], outimage[k + 1], outimage[k + 0], outimage[k + 3]);
      
    }


}


//outimage[pixel    ] : B, outimage[pixel + 1] : G, outimage[pixel + 2] : R로 가정하겠
//from bgr to yuv
void transform_foreground(unsigned char* inimage, unsigned char* outimage)
{
    for(int pixel = 0; pixel < IMAGEHEIGHT * IMAGEWIDTH * CHANNEL; pixel += 3)
    {
        // outimage[pixel    ] =  0; //y
        // outimage[pixel + 1] = ((-38 * inimage[pixel + 2] - 74  * inimage[pixel + 1] + 112 * inimage[pixel]) >> 8 ) + 128 ; //u (Cb)
        // outimage[pixel + 2] =  ((112 * inimage[pixel + 2] - 94  * inimage[pixel + 1] - 18 * inimage[pixel]) >> 8 ) + 128 ; //v (Cr)
        outimage[pixel    ] =  0; //y
        outimage[pixel + 1] = (unsigned char)(-0.148 * (float)inimage[pixel + 2] - 0.291 * (float)inimage[pixel + 1] + 0.439 * (float)inimage[pixel] + 128) ; //u (Cb)
        //outimage[pixel + 1] = (unsigned char)(0.565f * (float)inimage[pixel] - outimage[pixel]);
        
        
        outimage[pixel + 2] = (unsigned char)(0.439 * (float)inimage[pixel + 2] - 0.368 * (float)inimage[pixel + 1] + 0.071 * (float)inimage[pixel] + 128) ; //v (Cr)
        //outimage[pixel + 2] = (unsigned char)(0.713f * (float)(inimage[pixel + 2] -  outimage[pixel    ])) ; //v (Cr)

        
        //printf("Y=%d, U=%d, V=%d\n", outimage[pixel + 0], outimage[pixel + 1], outimage[pixel + 2]);
    
    }
    //printf("%c", outimage[0]);

}





void imageoverlay(unsigned char* background, unsigned char* foreground, unsigned char* outimage)
{


    unsigned char uv_field[IMAGEHEIGHT * IMAGEWIDTH];
    float alpha[IMAGEHEIGHT * IMAGEWIDTH];


    //calculate alpha
    int k = 0;
    for(int uv = 0; uv < IMAGEHEIGHT * IMAGEWIDTH * CHANNEL ; uv+=3)
    {
        uv_field[k] = ((foreground[uv + 1] & 0xF0) | ((foreground[uv + 2] & 0xF0) >> 4));
        //uv_field[k] = (float)(foreground[uv + 1] + foreground[uv + 2]) / 2.0 ;
        k++;
    }
    normalize(uv_field, alpha, IMAGEHEIGHT * IMAGEWIDTH);

    
    //alpha blending  BGRA order
    for (int i = 0, k = 0; i < IMAGEHEIGHT * IMAGEWIDTH * 4; i += 4, k++) 
    {
        // 인덱스 접근이 안전한지 확인
        //printf("%f\n", alpha[k]);
        //float alpha = 0.8f;
        
        outimage[i + 0] = (unsigned char)(alpha[k] * (float)foreground[k * 3 + 0] + (1.0f - alpha[k]) * (float)background[k * 3 + 0]); // U(Cb) -> B
        outimage[i + 1] = (unsigned char)(alpha[k] * (float)foreground[k * 3 + 1] + (1.0f - alpha[k]) * (float)background[k * 3 + 1]); // Y -> G
        //outimage[i + 1] = (1.0f - alpha) * (float)background[k * 3 + 1]; // Y -> G
        outimage[i + 2] = (unsigned char)(alpha[k] * (float)foreground[k * 3 + 2] + (1.0f - alpha[k]) * (float)background[k * 3 + 2]); // V(Cr) -> R
        outimage[i + 3] = (unsigned char)(alpha[k] * 255); // Alpha
        
        // outimage[i + 0] = (unsigned char)(alpha * (float)foreground[k * 3 + 0] + (1.0f - alpha) * (float)background[k * 3 + 0]); // U(Cb) -> B
        // outimage[i + 1] = (unsigned char)(alpha * (float)foreground[k * 3 + 1] + (1.0f - alpha) * (float)background[k * 3 + 1]); // Y -> G
        // //outimage[i + 1] = (1.0f - alpha) * (float)background[k * 3 + 1]; // Y -> G
        // outimage[i + 2] = (unsigned char)(alpha * (float)foreground[k * 3 + 2] + (1.0f - alpha) * (float)background[k * 3 + 2]); // V(Cr) -> R
        // outimage[i + 3] = (unsigned char)(alpha * 255); // Alpha
        
        
    }


}

void normalize(unsigned char *arr, float *norm_arr, int length) 
{

    float min = 1.0;
    float max = 0.0;
    
    //find minimum
    for (int i = 1; i < length; i++) 
    {
        if ((float)arr[i] < min) {
            min = arr[i];
        }
    }
    //find maximum
     for (int i = 1; i < length; i++) 
     {
        if ((float)arr[i] > max) {
            max = arr[i];
        }
    }

    //normalize
    for (int i = 0; i < length; i++) 
    {
        norm_arr[i] = ((float)arr[i] - min) / (max - min);
    }
}
