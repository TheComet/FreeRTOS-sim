void vPortTickISR();

void instruction_callback(void) {
  static int mclk;
  static int aclk;
  if (mclk++ % 8 == 0) {
    if (aclk++ % 8 == 0) {
      vPortTickISR();
    }
  }
}
