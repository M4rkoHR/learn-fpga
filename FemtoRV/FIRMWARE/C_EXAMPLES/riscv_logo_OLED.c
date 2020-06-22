/* 
 * FEMTORV32 - FEMTOSOC demo program:
 * Displaying a rotating RISCV logo
 */ 

#include <femtorv32.h>

/* The RISCV logo, with a tiny resolution
 * (remember, I only got 4Kb of RAM
 * on the IceStick !) 
 */
unsigned char logo[16][16] = {
   {7,7,7,7,7,7,5,3,3,3,3,3,3,3,3,7},
   {7,7,7,7,7,7,7,5,3,3,3,3,3,3,3,7},
   {1,1,1,1,2,7,7,7,3,3,3,3,3,3,3,7},
   {0,0,0,0,0,1,7,7,5,3,3,3,3,3,3,7},
   {0,0,0,0,0,0,7,7,6,3,3,3,3,3,6,7},
   {0,0,0,0,0,0,7,7,5,3,3,3,3,4,7,7},
   {0,0,0,0,0,2,7,7,4,3,3,3,3,7,7,7},
   {0,2,2,2,7,7,7,6,3,3,3,3,6,7,7,7},
   {0,7,7,7,7,7,6,3,3,3,3,5,7,7,2,7},
   {0,1,7,7,7,4,3,3,3,3,3,7,7,7,0,7},
   {0,0,2,7,7,6,3,3,3,3,6,7,7,1,0,7},
   {0,0,0,2,7,7,5,3,3,5,7,7,2,0,0,7},
   {0,0,0,0,7,7,7,5,4,7,7,2,0,0,0,7},
   {0,0,0,0,0,7,7,7,7,7,7,0,0,0,0,7},
   {0,0,0,0,0,1,7,7,7,7,1,0,0,0,0,7},
   {7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7}
};

/* 
 * Convert an RGB color into 16 bit color,
 * as expected by the SSD1351 OLED display.
 */ 
#define make_cmap_entry(R,G,B) \
   { (unsigned char)((R & ~7) | (G >> 5)), \
     (unsigned char)((B >> 3) | (G << 6))  }

unsigned char cmap[8][2] = {
   make_cmap_entry(0x28,0x33,0x74),
   make_cmap_entry(0x7B,0x80,0x9B),
   make_cmap_entry(0xAA,0xAC,0xBC),
   make_cmap_entry(0xF9,0xB1,0x15),
   make_cmap_entry(0xF9,0xBE,0x65),
   make_cmap_entry(0xF9,0xC7,0x82),
   make_cmap_entry(0xFC,0xD8,0xB0),
   make_cmap_entry(0xFA,0xFB,0xF8)
};

/*
 * Generated by TOOLS/make_sintab.c
 */

int sintab[64] = {
   0,25,49,74,97,120,142,162,181,197,212,225,236,244,251,254,
   256,254,251,244,236,225,212,197,181,162,142,120,97,74,49,25,
   0,-25,-49,-74,-97,-120,-142,-162,-181,-197,-212,-225,-236,-244,
   -251,-254,-256,-254,-251,-244,-236,-225,-212,-197,-181,-162,
   -142,-120,-97,-74,-49,-25
};

void main() {
    GL_init();
    int frame = 0;
    for(;;) {
	oled_write_window(0,0,127,127);
       
        int scaling = sintab[frame&63]+400;
        int Ux = scaling*sintab[frame & 63];         // The C compiler generates a call
        int Uy = scaling*sintab[(frame + 16) & 63];  // to __mulsi3 in LIB/femtorv32.s
        int Vx = -Uy;                                // for these two multiplications (and
        int Vy =  Ux;                                // yes, it works !!)

        int X0 = -64*(Ux+Vx); 
        int Y0 = -64*(Uy+Vy); 
			      
	for(int y=0; y<128; ++y) {
	    int X = X0;
	    int Y = Y0;
	    for(int x=0; x<128; ++x) {
	        unsigned char col = logo[(Y >> 18)&15][(X >> 18)&15];
		IO_OUT(IO_OLED_DATA,cmap[col][0]);
	        OLED_WAIT();
		IO_OUT(IO_OLED_DATA,cmap[col][1]);
	        OLED_WAIT();
	        X += Ux;
	        Y += Uy;
	    }
	    X0 += Vx;
	    Y0 += Vy;
	}
	++frame;
        IO_OUT(IO_LEDS,frame >> 4);
    }
}
