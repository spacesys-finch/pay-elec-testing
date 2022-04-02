#include <OV767X.h>

extern "C"{
	#include <stdio.h>
}

unsigned short pixels[320 * 240]; // VGA: 640x480 X 2 bytes per pixel (RGB565)

extern "C" void my_main(){

	printf("Sizeof of u short %d\n", sizeof(short));
	    if (!Camera.begin(QVGA, RGB565, 60)) {
	      printf("Failed to initialize camera!\r\n");
	      while (1);
	    }

	    printf("Camera settings:\twidth = %d\theight = %d\tbits per pixel = %d\r\n",
	    		Camera.width(), Camera.height(), Camera.bitsPerPixel());

	   // printf("Enabling test pattern mode\r\n\n");
	    // Camera.testPattern(1);

	    while(true){
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
			HAL_Delay(1000);
	    }


	    Camera.end();
}
