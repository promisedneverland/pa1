#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  //kbd->keycode = 0;
  inl(KBD_ADDR);
  kbd->keydown = 1;
  // if(kbd->keydown == 0)
  inl(KBD_ADDR);
  // send_key(keycode);
  // kbd->keydown = (kbd->keycode) & KEYDOWN_MASK;
}
