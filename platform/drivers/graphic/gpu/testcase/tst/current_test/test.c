
/**
 * how to use gpu: 

   gpu_open();

   vg_lite_buffer_t src;
   vg_lite_buffer_t dest;
   gpu_allocate_buffer(&src);
   gpu_allocate_buffer(&dest);

   //gpu_clear(&src, 0xffff0000)
 
   while (testing){
    gpu_blit(&src, &dest)
   }

   gpu_free_buffer(src); 
   gpu_free_buffer(dest);

   gpu_close();
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vg_lite.h"
#include "vg_lite_util.h"
#include "hal_trace.h"
#include "gpu_port.h"
#include "gpu_common.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#define GPU_REP(...)   TR_INFO( TR_MOD(TEST), "//" __VA_ARGS__)
#define GPU_ERR(...)   TR_ERROR( TR_MOD(TEST), "//" __VA_ARGS__)


static uint32_t fb_width = 480;
static uint32_t fb_height = 480;


int gpu_open(void)
{
    gpu_memory_setup();
    gpu_hw_reset();
    set_gpu_done_interrupt_handler();
    vg_lite_init(fb_width, fb_height);
    return 0;
}


void gpu_close(void)
{
   vg_lite_close();
}


int gpu_allocate_buffer(vg_lite_buffer_t *buffer)
{
    return vg_lite_allocate(buffer);
}

int gpu_free_buffer(vg_lite_buffer_t *buffer)
{
    return vg_lite_free(buffer);
}

int gpu_clear(vg_lite_buffer_t *buffer, uint32_t rgba8888)
{
    
    vg_lite_clear(buffer, NULL, rgba8888);
    vg_lite_finish();
    cpu_gpu_data_cache_invalid(buffer->memory, buffer->height * buffer->stride);
    return 0;
}

int gpu_blit(vg_lite_buffer_t *src, vg_lite_buffer_t *dest)
{
    vg_lite_filter_t filter;
    vg_lite_matrix_t matrix;
    // Initialize vglite.
    vg_lite_error_t error = VG_LITE_SUCCESS;

    filter = VG_LITE_FILTER_POINT;

    GPU_REP("%s %d img_src:%p img_dest :%p ", __func__, __LINE__, src->memory, dest->memory);
    TC_INIT
    TC_START
    cpu_cache_flush(src->memory, src->height * src->stride);
    vg_lite_blit(dest, src, 0, VG_LITE_BLEND_NONE, 0, filter);
    vg_lite_finish();
    cpu_gpu_data_cache_invalid(dest->memory, dest->height * dest->stride);
    TC_END
    TC_REP(blit_gcost);
    return 0;
}


void gpu_test_blit(void)
{
   gpu_open();

   vg_lite_buffer_t src;
   vg_lite_buffer_t dest;

   src.width  = fb_width;
   src.height = fb_height;
   src.format = GPU_FB_FORMAT;

   dest.width  = fb_width;
   dest.height = fb_height;
   dest.format = GPU_FB_FORMAT;

   gpu_allocate_buffer(&src);
   gpu_allocate_buffer(&dest);

   gpu_clear(&src, 0xffff0000);
 
   while(1){
    gpu_blit(&src, &dest);
   }

   gpu_free_buffer(&src); 
   gpu_free_buffer(&dest);

   gpu_close();
}