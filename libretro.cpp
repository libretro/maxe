#include "libretro.h"
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <stdint.h>
#include <tchar.h>
#include <math.h>

#include "schip.h"
SChip *Emulator = NULL;


static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

void retro_set_environment(retro_environment_t cb)
{
	environ_cb = cb;
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
	audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
	audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
	input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
	input_state_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
	video_cb = cb;
}


void retro_init(void)
{
	
}

void retro_deinit(void)
{
}

unsigned retro_api_version(void)
{
	return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
	(void)port;
	(void)device;
}

void retro_get_system_info(struct retro_system_info *info)
{
	memset(info, 0, sizeof(*info));
	info->library_name = "maxe";
	info->library_version = "v0.02";
	info->need_fullpath = true;
	info->valid_extensions = "c8|sc8";
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
	struct retro_game_geometry geom = { 128, 64, 128, 64,2.0/1.0 };
	struct retro_system_timing timing = { 60, 44100.0 };
	info->geometry = geom;
	info->timing = timing;
}

void retro_reset(void)
{
	
}


static void render_audio(void)
{
	
}

static int action_lut[16][2] = {
	{RETROK_x,0},
	{RETROK_1,1}, 
	{RETROK_2,2 }, 
	{RETROK_3,3}, 
	{RETROK_q,4 }, 
	{RETROK_w,5 }, 
	{RETROK_e,6 }, 
	{RETROK_a,7 }, 
	{RETROK_s,8 }, 
	{RETROK_d,9 }, 
	{RETROK_z,10 }, 
	{RETROK_c,11 }, 
	{RETROK_4,12 }, 
	{RETROK_r,13 }, 
	{RETROK_f,14 }, 
	{RETROK_v,15 }

};

void handleinput()
{
	static bool old_input[16];
	bool new_input[16];
	memset(new_input,0,sizeof(new_input));
	input_poll_cb();
	for(unsigned int i = 0; i < 16; i++){
	new_input[i] = input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, action_lut[i][0]);
	if(new_input[i] && !old_input[i]){
		Emulator->KeyPressed(action_lut[i][1]);
	}
	if(!new_input[i] && old_input[i]){
		Emulator->KeyReleased(action_lut[i][1]);
	}
	old_input[i] = new_input[i];
	}
}


void run_video()
{
	uint16_t output_video[64][128];
	memset(output_video,0x00,sizeof(output_video));
	for ( int j = 0; j < 64; j++ )
	{
		for ( int i = 0; i < 128; i++ )
		{
			if ( Emulator->ScreenData[ j ][ i ] ) output_video[j][i]=0xFF;
		}
	}
	video_cb(output_video, 128, 64, 256);
}

void retro_run(void)
{
	handleinput();
	Emulator->Update();
	run_video();
	
}

bool retro_load_game(const struct retro_game_info *info)
{
	(void)info;
	Emulator = SChip::CreateInstance();
	Emulator->LoadROM(info->path);

	return true;
}

void retro_unload_game(void)
{
}

unsigned retro_get_region(void)
{
	return RETRO_REGION_NTSC;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
	(void)type;
	(void)info;
	(void)num;
	return false;
}

size_t retro_serialize_size(void)
{
	return 0;
}

bool retro_serialize(void *data_, size_t size)
{
	return false;
}

bool retro_unserialize(const void *data_, size_t size)
{
	return false;
}

void *retro_get_memory_data(unsigned id)
{
	(void)id;
	return false;
}

size_t retro_get_memory_size(unsigned id)
{
	(void)id;
	return 0;
}

void retro_cheat_reset(void)
{}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
	(void)index;
	(void)enabled;
	(void)code;
}
