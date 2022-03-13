#include <OV767X.h>
#include <cstdio>

unsigned short pixels[176 * 144]; // QCIF: 176x144 X 2 bytes per pixel (RGB565)

extern "C"{
void my_main(){

	    if (!Camera.begin(QCIF, RGB565, 1)) {
	      printf("Failed to initialize camera!\r\n");
	      while (1);
	    }

	    printf("Camera settings:\twidth = %d\theight = %d\tbits per pixel = %d\r\n",
	    		Camera.width(), Camera.height(), Camera.bitsPerPixel());

	    printf("Enabling test pattern mode\r\n\n");
	    Camera.testPattern();

	    printf("Reading frame\r\n");
	    Camera.readFrame(pixels);

	    int numPixels = Camera.width() * Camera.height();

	    for (int i = 0; i < numPixels; i++) {
	      unsigned short p = pixels[i];

	      if (p < 0x1000) printf("0");
	      if (p < 0x0100) printf("0");
	      if (p < 0x0010) printf("0");

	      printf("%#08x", p);
	    }

	    printf("\r\n");
	    Camera.end();
}

};
