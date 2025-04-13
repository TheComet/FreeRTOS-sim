#pragma once

struct SR {
  unsigned GIE : 1;
};

struct IRQ {
  unsigned SYSTICK_IF : 1;
  unsigned ENDSCHED_IF : 1;
};

extern volatile struct SR SR;
extern volatile struct IRQ IRQ;
