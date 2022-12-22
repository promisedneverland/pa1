#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#define keyname(k) #k,
uint8_t keystate[100];
static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  assert(0);
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  char buf[64];
  int len = 64;
  if(NDL_PollEvent(buf,len))
  {
    // printf("%s\n",buf);
    if(buf[0] == 'k')
    {
      ev->key.keysym.sym = atoi(buf + 3);
      if(buf[1] == 'u')
      {
        keystate[ev->key.keysym.sym] = 0;
        ev->type = SDL_KEYUP;
        ev->key.type = SDL_KEYUP;
      }
      else
      {
        keystate[ev->key.keysym.sym] = 1;
        ev->type = SDL_KEYDOWN;
        ev->key.type = SDL_KEYDOWN;
      }
      // printf("%d\n",ev->key.keysym.sym);
      return 1;
    }
    
  }
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {

  while(1)
  {
    if(SDL_PollEvent(event))
    {
      break;
    }
  }
  // assert(0);
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  
  if(numkeys)
    *numkeys = sizeof(keyname) / sizeof(keyname[0]);
  
  int numkey = sizeof(keyname) / sizeof(keyname[0]);
 
 
  return keystate;
}
