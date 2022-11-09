#include <am.h>
#include <nemu.h>
#include <fcntl.h>
#include <unistd.h>
#include <klib.h>
// #include <SDL2/SDL.h>
#define AUDIO_FREQ_ADDR      (AUDIO_ADDR + 0x00)
#define AUDIO_CHANNELS_ADDR  (AUDIO_ADDR + 0x04)
#define AUDIO_SAMPLES_ADDR   (AUDIO_ADDR + 0x08)
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c)
#define AUDIO_INIT_ADDR      (AUDIO_ADDR + 0x10)
#define AUDIO_COUNT_ADDR     (AUDIO_ADDR + 0x14)

void __am_audio_init() {

}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) {
  // SDL_AudioSpec s = {};
  // s.format = AUDIO_S16SYS;  // 假设系统中音频数据的格式总是使用16位有符号数来表示
  // s.userdata = NULL;        // 不使用
  // //todo
  // SDL_InitSubSystem(SDL_INIT_AUDIO);
  // SDL_OpenAudio(&s, NULL);
  // SDL_PauseAudio(0);
  cfg->present = true;
}

void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl) {
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {
  stat->count = 0;
}

void __am_audio_play(AM_AUDIO_PLAY_T *ctl) {
}
