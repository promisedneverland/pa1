#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  //kbd->keycode = 0;
  int cur_keycode;
  // if(kbd->keydown == 0)
  cur_keycode = inl(KBD_ADDR);//可能是通码也可能是断码
  kbd->keydown = (cur_keycode & KEYDOWN_MASK);
  kbd->keycode = cur_keycode & ~KEYDOWN_MASK;//取通码的部分
  // send_key(keycode);
  // kbd->keydown = (kbd->keycode) & KEYDOWN_MASK;
}
