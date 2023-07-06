static const unsigned char quarter_sine_lookup[64] = {
   /* [round(255*(-math.cos(x)/2+0.5)) for x in np.linspace(0,math.pi/2,64)] */
   0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 17, 18, 20, 22, 24, 25, 27,
   29, 31, 34, 36, 38, 40, 42, 45, 47, 50, 52, 55, 57, 60, 63, 66, 68, 71, 74, 77, 80, 83, 86, 89,
   92, 95, 98, 101, 104, 107, 110, 113, 116, 120, 123, 126
};

unsigned char byte_sine (unsigned char x) {
   switch (x>>6) {
      default:       // for unsigned chars, ~x == 255-x
      case 0: return quarter_sine_lookup[x];
      case 1: return ~quarter_sine_lookup[~x&63];
      case 2: return ~quarter_sine_lookup[x&63];
      case 3: return quarter_sine_lookup[~x&63];
   }
}