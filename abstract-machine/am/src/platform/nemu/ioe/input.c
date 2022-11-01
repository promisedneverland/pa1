#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  //kbd->keycode = 0;
  int cur_keycode;
  // if(kbd->keydown == 0)
  cur_keycode = inl(KBD_ADDR);
  kbd->keydown = (cur_keycode & KEYDOWN_MASK ? true : false);
  kbd->keycode = cur_keycode & ~KEYDOWN_MASK;
  // send_key(keycode);
  // kbd->keydown = (kbd->keycode) & KEYDOWN_MASK;
}
