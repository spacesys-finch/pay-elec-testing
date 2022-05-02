#include <OV767X.h>

extern "C"{
	#include <stdio.h>
}

extern UART_HandleTypeDef huart3;
uint16_t pixels[160 * 120]; // VGA: 640 X 480 X 2 bytes per pixel (RGB565)

extern "C" void my_main(){

	    if (!Camera.begin(QQVGA, RGB565, 5)) {
	      printf("Failed to initialize camera!\r\n");
	      while (1);
	    }

	    //printf("Camera settings:\twidth = %d\theight = %d\tbits per pixel = %d\r\n",
	    		//Camera.width(), Camera.height(), Camera.bitsPerPixel());

	   // printf("Enabling test pattern mode\r\n\n");
	    //Camera.testPattern(1);

	    while(true){
			//printf("Reading frame\r\n");
			Camera.readFrame(pixels);

			// int numPixels = Camera.width() * Camera.height();

			HAL_UART_Transmit(&huart3, (uint8_t *)pixels, 160*120*2, HAL_MAX_DELAY);

//			for (int i = 0; i < numPixels; i++) {
//			  unsigned short p = pixels[i];
//
//			  if (p < 0x1000) printf("0");
//			  if (p < 0x0100) printf("0");
//			  if (p < 0x0010) printf("0");
//
//			  printf("%#08x ", p);
//
//
//			}

			//printf("\r\n");
			//HAL_Delay(1000);
	    }
	    Camera.end();
}
