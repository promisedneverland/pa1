#include <NDL.h>
#include <SDL.h>

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {
  TODO();
  return 0;
}

void SDL_CloseAudio() {
  TODO();
}

void SDL_PauseAudio(int pause_on) {
  TODO();
}

void SDL_MixAudio(uint8_t *dst, uint8_t *src, uint32_t len, int volume) {
  TODO();
}

SDL_AudioSpec *SDL_LoadWAV(const char *file, SDL_AudioSpec *spec, uint8_t **audio_buf, uint32_t *audio_len) {
  TODO();
  return NULL;
}

void SDL_FreeWAV(uint8_t *audio_buf) {
  TODO();
}

void SDL_LockAudio() {
  TODO();
}

void SDL_UnlockAudio() {
  TODO();
}

