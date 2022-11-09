/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <common.h>
#include <device/map.h>
#include <SDL2/SDL.h>


enum {
  reg_freq,
  reg_channels,
  reg_samples,
  reg_sbuf_size,//流缓冲区的大小
  reg_init,
  reg_count,//当前流缓冲区已经使用的大小，即还没有输出的音频缓存
  nr_reg//寄存器个数：6
};

//流缓冲区
static uint8_t *sbuf = NULL;
static uint32_t *audio_base = NULL;
static uint8_t *l_sbuf = NULL;
// static uint8_t *r_sbuf = NULL;
//stream ： 要往声卡输出的
//len ： buffer(stream)的长度
static void audio_play(void *userdata, uint8_t *stream, int len) {
  //nread：将从sbuf中读出的缓存字节数
  int nread = len;

  //缓冲区中剩余音频数据较少
  if (audio_base[reg_count] < len) nread = audio_base[reg_count];
  
  //这是一次缓存清空，因此count减少
  audio_base[reg_count] -= nread;
  for(int i = 1 ; i <= nread; i++)
  {
    *(stream) = *(l_sbuf);
    l_sbuf++;
    if(l_sbuf == sbuf + CONFIG_SB_SIZE) 
      l_sbuf = sbuf;
  }

  // int b = 0;
  // while (b < nread) {
  //   int n = read(rfd, stream, nread);//读最多nread个字节从rfd到stream
  //   if (n > 0) b += n;
  // }

  //需要的数据量过大，超过了缓冲区的数据量
  if (len > nread) {//等价于初始len大于count,此时count=0
    for(int i = 0 ; i < CONFIG_SB_SIZE ; i++)
    {
      sbuf[i] = 0;
    }
  }
}

static void audio_io_handler(uint32_t offset, int len, bool is_write) {
  if(audio_base[reg_init] == 1)
  {
    SDL_AudioSpec s = {};
    s.format = AUDIO_S16SYS;  // 假设系统中音频数据的格式总是使用16位有符号数来表示
    s.userdata = NULL;        // 不使用
    s.freq = audio_base[reg_freq];
    s.channels = audio_base[reg_channels];
    s.samples = audio_base[reg_samples];
    s.callback = audio_play;
    s.userdata = NULL;
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    SDL_OpenAudio(&s, NULL);
    SDL_PauseAudio(0);
    audio_base[reg_init] = 0;
  }
}
//24字节端口
void init_audio() {
  uint32_t space_size = sizeof(uint32_t) * nr_reg;

  audio_base = (uint32_t *)new_space(space_size);
  l_sbuf = audio_base;
#ifdef CONFIG_HAS_PORT_IO
  add_pio_map ("audio", CONFIG_AUDIO_CTL_PORT, audio_base, space_size, audio_io_handler);
#else
  add_mmio_map("audio", CONFIG_AUDIO_CTL_MMIO, audio_base, space_size, audio_io_handler);
#endif

  sbuf = (uint8_t *)new_space(CONFIG_SB_SIZE);
  add_mmio_map("audio-sbuf", CONFIG_SB_ADDR, sbuf, CONFIG_SB_SIZE, NULL);

}
