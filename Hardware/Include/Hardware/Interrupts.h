#pragma once

struct IRQ {
  unsigned GIE : 1;
  unsigned SYSTICK_IF : 1;
};

extern volatile struct IRQ IRQ;
