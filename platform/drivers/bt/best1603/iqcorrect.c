#ifndef NOSTD

#if defined (RX_IQ_CAL) || defined(DPD_ONCHIP_CAL) || defined(TX_IQ_CAL)
#include <math.h>
#include <stdio.h>

#include "hal_dma.h"
#include "hal_timer.h"
#include "hal_trace.h"

#include "string.h"
#include "besbt_string.h"
#include "bt_drv.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hal_btdump.h"
#include "iqcorrect.h"

#include "bt_drv_iq_common.h"
#include "bt_drv_internal.h"
#endif
#define LOG_MODULE                          HAL_TRACE_MODULE_APP

#define NTbl (1024)
#define FFT_SIZE 1024//4096//2048//4096
#define CAL_BIN 938//1877*2
#define CAL_OFFSET 2//4*2
//#define USE_FFT_WIN
//#define TIME_CAL_TABLE
#define DPD_FFT_SIZE (1024*4)
#define IQ_CALIB
#define DC_CALIB
//#define IQ_DEBUG
//#define DC_DEBUG
#define DC_ENERGY_THD 200
#define DC_RANGE_MIN -200
#define DC_RANGE_MAX 200
#define IQ_ENERGY_THD 300
#define IQ_RANGE_MIN -200
#define IQ_RANGE_MAX 200
#define SWEEP_STEP 50

#define bitshift 10
#define pi (3.1415926535898)
#define min(a,b)    (((a) < (b)) ? (a) : (b))

#define READ_REG(b,a) *(volatile uint32_t *)(uintptr_t)((uint32_t)(b)+(a))
#define WRITE_REG(v,b,a) *(volatile uint32_t *)(uintptr_t)((uint32_t)(b)+(a)) = v

#define   BUF_SIZE   (1024)

#if defined (RX_IQ_CAL) || defined(DPD_ONCHIP_CAL)
#define MAX_COUNT 5
volatile int iqimb_dma_status = 0;
typedef struct Complexflt_
{
    float re;
    float im;
} ComplexFlt;
typedef struct ComplexShort_
{
    short re;
    short im;
} ComplexShort;
typedef struct FFTCalcuFromDMA_
{
    short *M0data;
    ComplexFlt *Table;
    float *win;
    ComplexShort *Table0;
    ComplexShort *Table1;
    ComplexShort *Table2;
    short fft_len;
} FFTCalcuFromDMA;

FFTCalcuFromDMA *st = NULL;

#if !defined(TIME_CAL_TABLE)
static void Tblgen_new(FFTCalcuFromDMA *st)
{
    short len = st->fft_len;
    ComplexFlt *w = st->Table;
    float target_tone = CAL_BIN;

    for (int j=-CAL_OFFSET; j<= CAL_OFFSET; j++)
    {
        float tone_tmp = target_tone + j;
        for (int i=0; i<len; i++)
        {
            w[i].re = (float)(cos(2*pi*i*tone_tmp/len));
            w[i].im = (float)(-1*sin(2*pi*i*tone_tmp/len));
        }
        w += len;
    }
    BT_DRV_TRACE(1,"[%s]table ok",__func__);
/*
    for (int i=0; i<len; i++)
    {
        st->Table0[i].re = (short)(cos(2*pi*i*target_tone/len)*32767);
        st->Table0[i].im = (short)(-1*sin(2*pi*i*target_tone/len)*32767);
        st->Table1[i].re = (short)(cos(2*pi*i*(target_tone-1)/len)*32767);
        st->Table1[i].im = (short)(-1*sin(2*pi*i*(target_tone-1)/len)*32767);
        st->Table2[i].re = (short)(cos(2*pi*i*(target_tone+1)/len)*32767);
        st->Table2[i].im = (short)(-1*sin(2*pi*i*(target_tone+1)/len)*32767);
    }
*/
}
#else
static void Tblgen_new_offset(FFTCalcuFromDMA *st, int offset)
{
    short len = st->fft_len;
    ComplexFlt *w = st->Table;
    float target_tone = CAL_BIN;

    int j = offset;
    float tone_tmp = target_tone + j;
    for (int i=0; i<len; i++)
    {
        w[i].re = (float)(cosf(2*pi*i*tone_tmp/len));
        w[i].im = (float)(-1*sinf(2*pi*i*tone_tmp/len));
    }
}
#endif
FFTCalcuFromDMA *IQMismatchPreprocessState_init(int fft_size)
{
    BT_DRV_TRACE(0,"malloc ini");
#ifdef TIME_CAL_TABLE
    int sweep_num = 1;
#else
    int sweep_num = 2*CAL_OFFSET + 1;
#endif
    int need_ram = fft_size*2*2 + fft_size*sweep_num*sizeof(ComplexFlt);
    BT_DRV_TRACE(0,"need_ram = %d", need_ram);

    FFTCalcuFromDMA *st = (FFTCalcuFromDMA *)bt_drv_malloc(1 * sizeof(FFTCalcuFromDMA));
    if (st){
        st->M0data = (short*)bt_drv_malloc(fft_size*2 * sizeof(short));
        st->fft_len = fft_size;
        st->Table = (ComplexFlt*)bt_drv_malloc(fft_size*sweep_num * sizeof(ComplexFlt));
#ifdef USE_FFT_WIN
1
        st->win = (float *)bt_drv_malloc(fft_size * sizeof(float));
        for(int i=0;i<fft_size/2;i++)
        {
            st->win[i] = 0.5F*(1-cosf(2*pi*i/(float)fft_size));
            st->win[fft_size - i -1] = st->win[i];
        }
#endif
        BT_DRV_TRACE(1,"[%s]malloc ok",__func__);
    }
    return st;
}

int32_t IQMismatchPreprocessState_destroy(FFTCalcuFromDMA *st)
{
    BT_DRV_TRACE(1,"[%s] start!", __func__);
    bt_drv_free(st->M0data);
    bt_drv_free(st->Table);
    bt_drv_free(st);
    return 0;
}

int get_cpx_fft(FFTCalcuFromDMA *st)
{
    int i,j;
    ComplexFlt tmp0;
    short fftsize = st->fft_len;
    tmp0.re = 0.0f;
    tmp0.im = 0.0f;
    float tmp_i = 0;
    float tmp_q = 0;
    float sqare_tmp = 0;
    float sqare_max = 0;
    float sqare_sum = 0;
    ComplexFlt *w = st->Table;
    for(j=-CAL_OFFSET;j<=CAL_OFFSET;j++)
    {
#ifdef TIME_CAL_TABLE
        Tblgen_new_offset(st,j);
#endif
        for (i=0; i<fftsize; i++)
        {
#ifdef USE_FFT_WIN
            tmp_i = (float)st->M0data[2*i]*st->win[i];
            tmp_q = (float)st->M0data[2*i + 1]*st->win[i];
#else
            tmp_i = (float)st->M0data[2*i];
            tmp_q = (float)st->M0data[2*i + 1];
#endif
            tmp0.re = tmp0.re + (tmp_i*w[i].re - tmp_q*w[i].im);
            tmp0.im = tmp0.im + (tmp_i*w[i].im + tmp_q*w[i].re);
        }
#if !defined(TIME_CAL_TABLE)
        w += fftsize;
#endif
        TRACE(1,"tmp[%d]=%d+1j*%d",j,(int)tmp0.re,(int)tmp0.im);
        tmp0.re = tmp0.re/sqrtf(fftsize);
        tmp0.im = tmp0.im/sqrtf(fftsize);

        sqare_tmp = tmp0.re*tmp0.re + tmp0.im*tmp0.im;
        sqare_sum += sqare_tmp/fftsize;
        if(sqare_max < sqare_tmp)
            sqare_max = sqare_tmp;
    }

    TRACE(1,"sqare_sum=%de-2",(int)(100*sqare_sum));

/*
    for (i=0; i<fftsize; i++)
    {
        tmp_i = st->M0data[2*i];
        tmp_q = st->M0data[2*i + 1];
        tmp0.re = tmp0.re + (tmp_i*st->Table0[i].re - tmp_q*st->Table0[i].im)/32768;
        tmp0.im = tmp0.im + (tmp_i*st->Table0[i].im + tmp_q*st->Table0[i].re)/32768;
    }
    TRACE(1,"tmp0=%d+1j*%d",(int)tmp0.re,(int)tmp0.im);
*/
   return (int)sqare_max;
}

static struct HAL_DMA_DESC_T iqimb_dma_desc[32];
static uint8_t g_dma_channel = HAL_DMA_CHAN_NONE;

static void iqimb_dma_dout_handler(uint32_t remains, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
    if(g_dma_channel != HAL_DMA_CHAN_NONE)
    {
        hal_audma_free_chan(g_dma_channel);
    }
    hal_btdump_disable();
    iqimb_dma_status = 0;

    return;
}

#define TRANSFER_SIZE    512
int bt_iqimb_dma_enable ( short * dma_dst_data, uint16_t size)
{
    int sRet = 0;
    struct HAL_DMA_CH_CFG_T iqimb_dma_cfg;
    // BT_DRV_TRACE(0,"bt_iqimb_dma_enable :");
    int times = 0;

    times = size/TRANSFER_SIZE;
    iqimb_dma_status = 1;

    sRet = memset_s(&iqimb_dma_cfg, sizeof(iqimb_dma_cfg), 0, sizeof(iqimb_dma_cfg));
    if (sRet){
        TRACE(1, "%s line:%d sRet:%d", __func__, __LINE__, sRet);
    }

    //iqimb_dma_cfg.ch = hal_audma_get_chan(HAL_AUDMA_DSD_RX,HAL_DMA_HIGH_PRIO);

    iqimb_dma_cfg.ch = hal_audma_get_chan(HAL_AUDMA_BTDUMP,HAL_DMA_HIGH_PRIO);
    ASSERT((HAL_DMA_CHAN_NONE != iqimb_dma_cfg.ch), "hal_audma_get_chan failed.");
    g_dma_channel = iqimb_dma_cfg.ch;
    iqimb_dma_cfg.dst_bsize = HAL_DMA_BSIZE_16;
    iqimb_dma_cfg.dst_periph = 0; //useless
    iqimb_dma_cfg.dst_width = HAL_DMA_WIDTH_WORD;
    iqimb_dma_cfg.handler = (HAL_DMA_IRQ_HANDLER_T)iqimb_dma_dout_handler;
    iqimb_dma_cfg.src = 0; // useless
    iqimb_dma_cfg.src_bsize = HAL_DMA_BSIZE_4;

    //iqimb_dma_cfg.src_periph = HAL_AUDMA_DSD_RX;
    iqimb_dma_cfg.src_periph = HAL_AUDMA_BTDUMP;
    iqimb_dma_cfg.src_tsize = TRANSFER_SIZE;//size;//1600; //1600*2/26=123us
    iqimb_dma_cfg.src_width = HAL_DMA_WIDTH_WORD;
    //iqimb_dma_cfg.src_width = HAL_DMA_WIDTH_HALFWORD;
    iqimb_dma_cfg.try_burst = 1;
    iqimb_dma_cfg.type = HAL_DMA_FLOW_P2M_DMA;
    //iqimb_dma_cfg.dst = (uintptr_t)(dma_dst_data);

    //hal_audma_init_desc(&iqimb_dma_desc[0], &iqimb_dma_cfg, 0, 1);

	for(int i = 0;i < times; i++)
	{
        memset(&iqimb_dma_desc[i], 0, sizeof(iqimb_dma_desc[i]));
        iqimb_dma_cfg.dst = (uint32_t)(dma_dst_data + TRANSFER_SIZE * 2 * i);
        if(i < (times - 1))
        {
            hal_audma_init_desc(&iqimb_dma_desc[i], &iqimb_dma_cfg, &iqimb_dma_desc[i+1], 0);
        }
        else
        {
            hal_audma_init_desc(&iqimb_dma_desc[i], &iqimb_dma_cfg, 0, 1);
        }
    }

    hal_audma_sg_start(&iqimb_dma_desc[0], &iqimb_dma_cfg);

    //configed after mismatch parameter done, or apb clock muxed.

    //wait
    for(volatile int i=0; i<5000; i++);
    hal_btdump_enable();

    return 1;

}

void check_mem_data(void* data, int len)
{
    short* share_mem = (short*)data;
    BT_DRV_TRACE(3,"check_mem_data :share_mem= %p, 0x%x, 0x%x",share_mem,share_mem[0],share_mem[1]);

    int32_t i =0;
    int32_t remain = len;

    while(remain > 0)
    {
        for(i=0; i<32; i++)//output two line
        {
            if (remain >16)
            {
                DUMP16("%04X ",share_mem,16);
                share_mem +=16;
                remain -= 16;
            }
            else
            {
                DUMP16("%04X ",share_mem,remain);
                remain =0;
                return;
            }
        }
        //  BT_DRV_TRACE(0,"\n");
        //BT_DRV_TRACE(1,"addr :0x%08x\n",share_mem);
        hal_sys_timer_delay(MS_TO_TICKS(100));
    }
}

void  bt_iq_fft_calculate_init()
{
    int fftsize = FFT_SIZE;
    uint32_t time_start = hal_sys_timer_get();

    st = IQMismatchPreprocessState_init(fftsize);
#if !defined(TIME_CAL_TABLE)
    Tblgen_new(st);
#endif
    BT_DRV_TRACE(1,"Init use time: %d ms", __TICKS_TO_MS(hal_sys_timer_get()-time_start));
}

void bt_iq_fft_calculate_run()
{
    uint32_t time_start = hal_sys_timer_get();
    int fftsize;

    //config gpadc 26m
    READ_REG(0xd0310000,0x0);

    //fftsize = 1024;
    fftsize = FFT_SIZE;
    time_start = hal_sys_timer_get();
    for (int k = 0; k<1; k++)
    {
        //BT_DRV_TRACE(1,"dump From DMA!");
        bt_iqimb_dma_enable(st->M0data,fftsize);
        while(1)
        {
            if(iqimb_dma_status==0)
                break;
        }
        //BT_DRV_TRACE(1,"GET DMA READY!");
#ifdef RX_IQ_DEBUG
        check_mem_data((uint8_t *)st->M0data, fftsize*2);
#endif
        int energy_val = get_cpx_fft(st);
        BT_DRV_TRACE(1,"----MAX E=%d,E/N=%de-2----",energy_val,(int)(100.0*(float)energy_val/(float)fftsize));

#if 0
        lib_iqmis_getiq((short *)st->M0data,4096,(float *)st->data_i,(float *)st->data_q);
        BT_DRV_TRACE(2,"iq data:0x%x, 0x%x",st->data_i[0],st->data_q[0]);
        BT_DRV_TRACE(2,"iq data1:0x%x, 0x%x",st->data_i[1],st->data_q[1]);
        lib_iqmis_esti((float *)st->data_i,(float *)st->data_q,2048,&iq_gain,&iq_phy);
        BT_DRV_TRACE(2,"gain:0x%x phy: 0x%x",iq_gain,iq_phy);
#endif
        BT_DRV_TRACE(1,"FFT cal use time: %d ms", __TICKS_TO_MS(hal_sys_timer_get()-time_start));
    }
}

void bt_iqimb_test_init()
{
    bt_iq_fft_calculate_init();
}

int bt_iqimb_test_ex (int mismatch_type)
{
    bt_iq_fft_calculate_run();
    return 1;
}
#endif

#ifdef TX_IQ_CAL
static float get_DC_energy(uint32_t addr, short dc_i, short dc_q,int num)
{
    float E_ave = 0;
    for(int cc = 0; cc < num; cc++) {
        int32_t i_data, q_data = 0;
        uint32_t val;

        val &= 0;
        val |= dc_i;
        val &= 0x3FFF;
        BTDIGITAL_REG_SET_FIELD(addr, 0x3FFF, 0, val);
        //BT_DRV_TRACE(1,"2 0x%x=0x%x",addr,BTDIGITAL_REG(addr));

        val &= 0;
        val |= dc_q;
        val &= 0x3FFF;
        BTDIGITAL_REG_SET_FIELD(addr, 0x3FFF, 14, val);
        //BTDIGITAL_REG_SET_FIELD(0xD0350250, 0x1, 31, 0);
        hal_sys_timer_delay(US_TO_TICKS(1));
        //BT_DRV_TRACE(1,"0x%x=0x%x",addr,BTDIGITAL_REG(addr));

        //BTDIGITAL_REG_WR(0xD0220C00, 0x80020000);       //rx modem on
        BTDIGITAL_REG_WR(0xd0350334,0x9e800168);
        hal_sys_timer_delay(US_TO_TICKS(100));
        BTDIGITAL_REG_WR(0xd0350334,0xde800168);
        hal_sys_timer_delay(US_TO_TICKS(150));

        i_data = *(volatile uint32_t *)(0xD0350338);
        q_data = *(volatile uint32_t *)(0xD035033C);
        i_data &= 0x3FFFFF;
        q_data &= 0x3FFFFF;
        if(0x200000 <= i_data)
            i_data -= 0x400000;
        if(0x200000 <= q_data)
            q_data -= 0x400000;
        i_data = i_data/8;
        q_data = q_data/8;
        float E = sqrtf(i_data*i_data + q_data*q_data);
        E_ave = E_ave + E;
    }
    E_ave = E_ave / num;
    return E_ave;
}

static void set_gain_phase(uint32_t addr, short gain, short phase)
{
    BTDIGITAL_REG_SET_FIELD(0xD0350250, 0x1, 31, 0);
    uint32_t val;
    val &= 0;
    val |= gain;
    val &= 0x3FFF;
    BTDIGITAL_REG_SET_FIELD(addr, 0x3FFF, 0, val);
    val &= 0;
    val |= phase;
    val &= 0x3FFF;
    BTDIGITAL_REG_SET_FIELD(addr, 0x3FFF, 16, val);
}

static float get_iq_energy(uint32_t addr, short gain,short phase,int num)
{
//0xD0310000
    float E_ave = 0;
    for(int cc = 0; cc < num; cc++)
    {
        int32_t i_data, q_data = 0;
        uint32_t val;

        BTDIGITAL_REG_SET_FIELD(0xD0350250, 0x1, 31, 0);
        BTDIGITAL_REG_WR(0xD0330058, 0x00800030);
        BTDIGITAL_REG_WR(0xD0330060, 0xFFFFFFFF);       //rst

        val &= 0;
        val |= gain;
        val &= 0x3FFF;
        BTDIGITAL_REG_SET_FIELD(addr, 0x3FFF, 0, val);
        val &= 0;
        val |= phase;
        val &= 0x3FFF;
        BTDIGITAL_REG_SET_FIELD(addr, 0x3FFF, 16, val);
        hal_sys_timer_delay(US_TO_TICKS(1));
        //BT_DRV_TRACE(1,"0xD0310000=0x%x",BTDIGITAL_REG(0xD0310000));
        BTDIGITAL_REG_WR(0xD0330064, 0xFFFFFFFF);       //clr rst
        BTDIGITAL_REG_SET_FIELD(0xD0350250, 0x1, 31, 1);

        //BTDIGITAL_REG_WR(0xD0220C00, 0x80020000);       //rx modem on
        BTDIGITAL_REG_WR(0xd0350334,0x9e000168);
        hal_sys_timer_delay(US_TO_TICKS(100));
        BTDIGITAL_REG_WR(0xd0350334,0xde000168);
        hal_sys_timer_delay(US_TO_TICKS(150));

        i_data = *(volatile uint32_t *)(0xD0350338);
        q_data = *(volatile uint32_t *)(0xD035033C);
        i_data &= 0x3FFFFF;
        q_data &= 0x3FFFFF;
        //BT_DRV_TRACE(2,"i=0x%x,q=0x%x",i_data,q_data);
        if(0x200000 <= i_data)
            i_data -= 0x400000;
        if(0x200000 <= q_data)
            q_data -= 0x400000;

        float E = sqrtf(i_data*i_data + q_data*q_data);
        //BT_DRV_TRACE(2,"i=%d,q=%d",i_data,q_data);
#ifdef IQ_DEBUG
        //BT_DRV_TRACE(3,"n=%d,g=%d,p=%d,e=%d",cc,gain,phase,(int)E);
#endif
        E_ave = E_ave + E;
    }
    E_ave = E_ave / num;
    return E_ave;
}


static int get_best_i(int min_i,float min_energy,int base_q,int step,int* im, float* em)
{
    float energy_left = 0;
    float energy_right = 0;
    float energy_middle = 0;
    uint32_t e_addr = 0xD0350250;
    int i_left = 0;
    int i_right = 0;
    int i_middle = 0;
    int base_i = 0;
    i_left = min_i - step;
    energy_left = get_DC_energy(e_addr, i_left, base_q, 1);
    i_right = min_i + step;
    energy_right = get_DC_energy(e_addr, i_right, base_q, 1);
    while(1)
    {
        i_middle = (i_left + i_right) / 2;
        if(ABS(i_left - i_right) < 17)
        {
            energy_middle = get_DC_energy(e_addr, i_middle, base_q, 3);

        }
        else
        {
            energy_middle = get_DC_energy(e_addr, i_middle, base_q, 2);
        }
#ifdef DC_DEBUG
        BT_DRV_TRACE(4,"Mi=%d,q=%d,e=%d,DC_Emin=%d",i_middle,base_q,(int)energy_middle,(int)*em);
        BT_DRV_TRACE(4,"Li=%d,ri=%d,le=%d,re=%d",i_left,i_right,(int)energy_left,(int)energy_right);
#endif
        if(energy_left > energy_right)
        {
            i_left = i_middle;
            energy_left = energy_middle;
            min_energy = energy_right;
            min_i = i_right;
        }
        else
        {
            i_right = i_middle;
            energy_right = energy_middle;
            min_energy = energy_left;
            min_i = i_left;
        }
        if(min_energy < *em)
        {
            *im = min_i;
            *em = min_energy;
        }
#ifdef DC_DEBUG
        //BT_DRV_TRACE(4,"DICHOTOMY_best_i=%d,best_q=%d,energy_min=%d,DC_Emin=%d",min_i,base_q,(int)min_energy,(int)*em);
#endif
        if(ABS(i_left - i_right) < 4)
            break;
    }

    if(energy_left > energy_right)
    {
        min_energy = energy_right;
        min_i = i_right;
    }
    else
    {
        min_energy = energy_left;
        min_i = i_left;
    }
    if(min_energy < *em)
    {
        *im = min_i;
        *em = min_energy;
    }
#ifdef DC_DEBUG
    BT_DRV_TRACE(4,"LAST_i=%d,q=%d,e=%d,DC_Emin=%d",min_i,base_q,(int)min_energy,(int)*em);
#endif
    if(min_i != *im)
    {
        *em = get_DC_energy(e_addr, *im, base_q, 3);
        if(*em > min_energy)
        {
            *im = min_i;
            *em = min_energy;
        }
    }
    base_i = *im;
    return base_i;
}

static int get_best_q(int min_q,float min_energy,int base_i,int step,int* qm, float* em)
{
    float energy_left = 0;
    float energy_right = 0;
    float energy_middle = 0;
    uint32_t e_addr = 0xD0350250;
    int q_left = 0;
    int q_right = 0;
    int q_middle = 0;
    int base_q = 0;
    q_left = min_q - step;
    energy_left = get_DC_energy(e_addr, base_i, q_left, 1);
    q_right = min_q + step;
    energy_right = get_DC_energy(e_addr, base_i, q_right, 1);
    while(1)
    {
        q_middle = (q_left+q_right) / 2;
        if(ABS(q_left - q_right) < 17)
        {
            energy_middle = get_DC_energy(e_addr, base_i, q_middle, 3);
        }
        else
        {
            energy_middle = get_DC_energy(e_addr, base_i, q_middle, 2);
        }
#ifdef DC_DEBUG
        BT_DRV_TRACE(4,"Mi=%d,q=%d,e=%d,DC_Emin=%d",base_i,q_middle,(int)energy_middle,(int)*em);
#endif
#ifdef DC_DEBUG
        BT_DRV_TRACE(4,"Lq=%d,rq=%d,le=%d,re=%d",q_left,q_right,(int)energy_left,(int)energy_right);
#endif
        if(energy_left > energy_right)
        {
            q_left = q_middle;
            energy_left = energy_middle;
            min_energy = energy_right;
            min_q = q_right;
        }
        else
        {
            q_right = q_middle;
            energy_right = energy_middle;
            min_energy = energy_left;
            min_q = q_left;
        }
        if(min_energy < *em)
        {
            *qm = min_q;
            *em = min_energy;
        }
#ifdef DC_DEBUG
        //BT_DRV_TRACE(4,"DICHOTOMY_best_i=%d,best_q=%d,energy_min=%d,DC_Emin=%d",base_i,min_q,(int)min_energy,(int)*em);
#endif
        if(ABS(q_left - q_right) < 4)
            break;
    }

    if(energy_left > energy_right)
    {
        min_energy = energy_right;
        min_q = q_right;
    }
    else
    {
        min_energy = energy_left;
        min_q = q_left;
    }
    if(min_energy < *em)
    {
        *qm = min_q;
        *em = min_energy;
    }
#ifdef DC_DEBUG
    BT_DRV_TRACE(4,"LAST_i=%d,q=%d,e=%d,DC_Emin=%d",base_i,min_q,(int)min_energy,(int)*em);
#endif
    if(min_q != *qm)
    {
        *em = get_DC_energy(e_addr, base_i, *qm, 3);
        if(*em > min_energy)
        {
            *qm = min_q;
            *em = min_energy;
        }
    }
    base_q = *qm;
    return base_q;
}

static int get_best_gain(uint32_t addr,int min_gain,float min_energy,int base_phase,int step,int* gm, float* em)
{
    float energy_left = 0;
    float energy_right = 0;
    float energy_middle = 0;
    uint32_t en_addr = addr;
    int gain_left = 0;
    int gain_right = 0;
    int gain_middle = 0;
    int base_gain = 0;
    gain_left = min_gain - step;
    energy_left = get_iq_energy(en_addr, gain_left, base_phase, 2);
    gain_right = min_gain + step;
    energy_right = get_iq_energy(en_addr, gain_right, base_phase, 2);
    while(1)
    {
        gain_middle = (gain_left+gain_right) / 2;
        if(ABS(gain_left - gain_right) < 8)
        {
            energy_middle = get_iq_energy(en_addr, gain_middle, base_phase, 4);
        }
        else
        {
            energy_middle = get_iq_energy(en_addr, gain_middle, base_phase, 2);
        }
#ifdef IQ_DEBUG
        BT_DRV_TRACE(4,"Mg=%d,p=%d,e=%d,IQ_Emin=%d",gain_middle,base_phase,(int)energy_middle,(int)*em);
        BT_DRV_TRACE(4,"Lg=%d,rg=%d,le=%d,re=%d",gain_left,gain_right,(int)energy_left,(int)energy_right);
#endif
        if(energy_left > energy_right)
        {
            gain_left = gain_middle;
            energy_left = energy_middle;
            min_energy = energy_right;
            min_gain = gain_right;
        }
        else
        {
            gain_right = gain_middle;
            energy_right = energy_middle;
            min_energy = energy_left;
            min_gain = gain_left;
        }
        if(min_energy < *em)
        {
            *gm = min_gain;
            *em = min_energy;
        }

#ifdef IQ_DEBUG
        //BT_DRV_TRACE(4,"DICHOTOMY_best_gain=%d,best_phase=%d,energy_min=%d,IQ_Emin=%d",min_gain,base_phase,(int)min_energy,(int)*em);
#endif
        if(ABS(gain_left-gain_right) < 2)
            break;
    }

    if(energy_left > energy_right)
    {
        min_energy = energy_right;
        min_gain = gain_right;
    }
    else
    {
        min_energy = energy_left;
        min_gain = gain_left;
    }
    if(min_energy < *em)
    {
        *gm = min_gain;
        *em = min_energy;
    }
#ifdef IQ_DEBUG
    //BT_DRV_TRACE(4,"LAST_DICHOTOMY_best_gain=%d,best_phase=%d,energy_min=%d,IQ_Emin=%d",min_gain,base_phase,(int)min_energy,(int)*em);
#endif
    if(min_gain != *gm)
    {
        *em = get_DC_energy(en_addr, *gm, base_phase, 3);
        if(*em > min_energy)
        {
            *gm = min_gain;
            *em = min_energy;
        }
    }
    base_gain = *gm;
    return base_gain;
}

static int get_best_phase(uint32_t addr,int min_phase,float min_energy,int base_gain,int step,int* pm, float* em)
{
    float energy_left = 0;
    float energy_right = 0;
    float energy_middle = 0;
    uint32_t en_addr = addr;
    int phase_left = 0;
    int phase_right = 0;
    int phase_middle = 0;
    int base_phase = 0;
    phase_left = min_phase - step;
    energy_left = get_iq_energy(en_addr, base_gain, phase_left, 2);
    phase_right = min_phase + step;
    energy_right = get_iq_energy(en_addr, base_gain, phase_right, 2);
    while(1)
    {
        phase_middle = (phase_left + phase_right) / 2;
        if(ABS(phase_left - phase_right) < 8)
        {
            energy_middle = get_iq_energy(en_addr,base_gain,phase_middle,4);
        }
        else
        {
            energy_middle = get_iq_energy(en_addr,base_gain,phase_middle,2);
        }
#ifdef IQ_DEBUG
        BT_DRV_TRACE(4,"Mg=%d,p=%d,e=%d,IQ_Emin=%d",base_gain,phase_middle,(int)energy_middle,(int)*em);
        BT_DRV_TRACE(4,"Lp=%d,rp=%d,le=%d,re=%d",phase_left,phase_right,(int)energy_left,(int)energy_right);
#endif
        if(energy_left > energy_right)
        {
            phase_left = phase_middle;
            energy_left = energy_middle;
            min_energy = energy_right;
            min_phase = phase_right;
        }
        else
        {
            phase_right = phase_middle;
            energy_right = energy_middle;
            min_energy = energy_left;
            min_phase = phase_left;
        }
        if(min_energy < *em)
        {
            *pm = min_phase;
            *em = min_energy;
        }
#ifdef IQ_DEBUG
        //BT_DRV_TRACE(4,"DICHOTOMY_best_gain=%d,best_phase=%d,energy_min=%d,IQ_Emin=%d",base_gain,min_phase,(int)min_energy,(int)*em);
#endif
        if(ABS(phase_left - phase_right) < 2)
            break;
    }

    if(energy_left > energy_right)
    {
        min_energy = energy_right;
        min_phase = phase_right;
    }
    else
    {
        min_energy = energy_left;
        min_phase = phase_left;
    }
    if(min_energy < *em)
    {
        *pm = min_phase;
        *em = min_energy;
    }
#ifdef IQ_DEBUG
    //BT_DRV_TRACE(4,"LAST_DICHOTOMY_best_gain=%d,best_phase=%d,energy_min=%d,IQ_Emin=%d",base_gain,min_phase,(int)min_energy,(int)*em);
#endif
    if(min_phase != *pm)
    {
        *em = get_DC_energy(en_addr, base_gain, *pm, 3);
        if(*em > min_energy)
        {
            *pm = min_phase;
            *em = min_energy;
        }
    }
    base_phase = *pm;
    return base_phase;
}

static int global_i = 0;
static int global_q = 0;

static int g_gain[3] = {0};
static int g_phase[3] = {0};

void dc_iq_calib_1603(void)
{
    uint32_t time_start = hal_sys_timer_get();

#ifdef DC_CALIB
    int DC_i_min = 0;
    int DC_q_min = 0;
    uint32_t e_addr = 0xD0350250;
    float DC_energy_min = 1000000;

    int base_i = 0;
    int base_q = 0;
    int min_i = 0;
    int min_q = 0;
    int step = SWEEP_STEP;
    float min_energy = 1000000;
    for (int q1=DC_RANGE_MIN; q1<=DC_RANGE_MAX; q1+=step) {
        float e_tmp = get_DC_energy(e_addr,base_i,q1,1);
        if(min_energy > e_tmp) {
            min_q = q1;
            min_energy = e_tmp;
            DC_q_min = min_q;
            DC_energy_min = min_energy;
        }
#ifdef DC_DEBUG
    //BT_DRV_TRACE(4,"i=%d,q=%d,e_tmp=%d,DC_Emin=%d",base_i,q1,(int)e_tmp,(int)DC_energy_min);
#endif
    }
#ifdef DC_DEBUG
    BT_DRV_TRACE(4,"--------------SWEEP q=%d,e_tmp=%d--------------",min_q,(int)min_energy);
#endif
    base_q = get_best_q(min_q,min_energy,base_i,step,&DC_q_min,&DC_energy_min);
#ifdef DC_DEBUG
    BT_DRV_TRACE(2,"Step1:best_q=%d,energy_min=%d",base_q,(int)DC_energy_min);
#endif

    if(DC_ENERGY_THD < DC_energy_min) {
        min_energy = 1000000;
        for (int i1=DC_RANGE_MIN; i1<=DC_RANGE_MAX; i1+=step) {
            float e_tmp=get_DC_energy(e_addr,i1,base_q,1);
            if(min_energy > e_tmp) {
                min_i = i1;
                min_energy = e_tmp;
                DC_i_min = min_i;
                DC_energy_min = min_energy;
            }
#ifdef DC_DEBUG
            //BT_DRV_TRACE(4,"i=%d,q=%d,e_tmp=%d,DC_Emin=%d",i1,base_q,(int)e_tmp,(int)DC_energy_min);
#endif
        }
#ifdef DC_DEBUG
        BT_DRV_TRACE(4,"--------------SWEEP i=%d,e_tmp=%d--------------",min_i,(int)min_energy);
#endif
        base_i = get_best_i(min_i,min_energy,base_q,step,&DC_i_min,&DC_energy_min);
#ifdef DC_DEBUG
        BT_DRV_TRACE(3,"Step2:abest_i=%d,best_q=%d,energy_min=%d",base_i,base_q,(int)DC_energy_min);
#endif
        if (DC_ENERGY_THD < DC_energy_min) {
            base_q = get_best_q(min_q,min_energy,base_i,step,&DC_q_min,&DC_energy_min);
#ifdef DC_DEBUG
            BT_DRV_TRACE(3,"Step3:abest_i=%d,best_q=%d,energy_min=%d",base_i,base_q,(int)DC_energy_min);
#endif
        }
    }
#endif

    float teste = get_DC_energy(e_addr,base_i,base_q,1);
    BT_DRV_TRACE(3,"FINAL DC best_i=%d,best_q=%d,e=%d,energy_min=%d",
    base_i,base_q,(int)teste,(int)DC_energy_min);

    global_i = base_i;
    global_q = base_q;

    //IQ
    BTDIGITAL_REG_SET_FIELD(0xD0350334, 0x1, 23, 0);
#ifdef IQ_CALIB
    uint32_t en_addr_base = 0xD0310000;
    uint32_t en_addr = 0x0;
    for (int k = 0; k<3; k++) {
        uint32_t freq_add = 0x80020000;
        if(k==0) {
            BTDIGITAL_REG_WR(0xd0350248, 0x8040CCCD);//rate converter 2415
            freq_add += 13;
            BTDIGITAL_REG_WR(0xD0220C00, freq_add);
            en_addr = en_addr_base + 13 * 4;
        } else if(k==1) {
            BTDIGITAL_REG_WR(0xd0350248, 0x80422FC9);//rate converter 2441
            freq_add += 39;
            BTDIGITAL_REG_WR(0xD0220C00, freq_add);
            en_addr = en_addr_base + 39 * 4;
        } else {
            BTDIGITAL_REG_WR(0xd0350248, 0x804392C6);//rate converter 2467
            freq_add += 65;
            BTDIGITAL_REG_WR(0xD0220C00, freq_add);
            en_addr = en_addr_base + 65 * 4;
        }
        int iq_gain_min = 0;
        int iq_phase_min = 0;
        float iq_energy_min = 1000000;
        int base_phase = 0;
        int base_gain = 0;
        int min_phase = 0;
        int min_gain = 0;
        step = SWEEP_STEP;
        min_energy = 1000000;
        for(int gain1=IQ_RANGE_MIN; gain1<=IQ_RANGE_MAX; gain1+=step) {
            float e_tmp=get_iq_energy(en_addr,gain1,base_phase,1);
            if(min_energy > e_tmp) {
                min_gain = gain1;
                min_energy = e_tmp;
                iq_gain_min = min_gain;
                iq_energy_min = min_energy;
            }
#ifdef IQ_DEBUG
        //BT_DRV_TRACE(4,"gain=%d,phase=%d,e_tmp=%d,IQ_Emin=%d",gain1,base_phase,(int)e_tmp,(int)iq_energy_min);
#endif
        }
#ifdef IQ_DEBUG
        BT_DRV_TRACE(4,"--------------SWEEP gain=%d,e_tmp=%d--------------",min_gain,(int)min_energy);
#endif
        base_gain=get_best_gain(en_addr,min_gain,min_energy,base_phase,step,&iq_gain_min,&iq_energy_min);
#ifdef IQ_DEBUG
        BT_DRV_TRACE(2,"Step1:best_gain=%d,energy_min=%d",base_gain,(int)iq_energy_min);
#endif

        if(IQ_ENERGY_THD < iq_energy_min) {
            min_energy = 1000000;
            for(int phase1=IQ_RANGE_MIN; phase1<=IQ_RANGE_MAX; phase1+=step){
                float e_tmp=get_iq_energy(en_addr,base_gain,phase1,1);
                if(min_energy > e_tmp) {
                    min_phase = phase1;
                    min_energy = e_tmp;
                    iq_phase_min = min_phase;
                    iq_energy_min = min_energy;
                }
#ifdef IQ_DEBUG
            //BT_DRV_TRACE(4,"gain=%d,phase=%d,e_tmp=%d,IQ_Emin=%d",base_gain,phase1,(int)e_tmp,(int)iq_energy_min);
#endif
            }
#ifdef IQ_DEBUG
            BT_DRV_TRACE(4,"--------------SWEEP phase=%d,e_tmp=%d--------------",min_phase,(int)min_energy);
#endif
            base_phase=get_best_phase(en_addr,min_phase,min_energy,base_gain,step,&iq_phase_min,&iq_energy_min);
#ifdef IQ_DEBUG
            BT_DRV_TRACE(3,"Step2:best_gain=%d,best_phase=%d,energy_min=%d",base_gain,base_phase,(int)iq_energy_min);
#endif

            if(IQ_ENERGY_THD < iq_energy_min) {
                base_gain=get_best_gain(en_addr,min_gain,min_energy,base_phase,step,&iq_gain_min,&iq_energy_min);
#ifdef IQ_DEBUG
                BT_DRV_TRACE(3,"Step3:best_gain=%d,best_phase=%d,energy_min=%d",base_gain,base_phase,(int)iq_energy_min);
#endif
            }
        }

        float testen=get_iq_energy(en_addr,base_gain,base_phase,1);
        BT_DRV_TRACE(3,"FINAL %d,best_g=%d,best_p=%d,e=%d,energy_min=%d",k,base_gain,base_phase,(int)testen,(int)iq_energy_min);
        g_gain[k]  = base_gain;
        g_phase[k] = base_phase;

        int ch1 = 0;
        int ch2 = 0;
        if(k==0) {
            ch1= 0;
            ch2= 25;
        } else if(k==1) {
            ch1= 26;
            ch2= 51;
        } else {
            ch1= 52;
            ch2= 78;
        }
        for (int ch = ch1; ch <= ch2; ch++) {
            uint32_t addre=en_addr_base+ch*4;
            set_gain_phase(addre,base_gain,base_phase);
        }
    }
#endif

    BTDIGITAL_REG_SET_FIELD(0xD0350250, 0x1, 31, 1);
    BT_DRV_TRACE(2,"%s use time: %d ms", __func__,__TICKS_TO_MS(hal_sys_timer_get()-time_start));
}

void btdrv_dc_datawrite(void)
{
    BTDIGITAL_REG_SET_FIELD(0xD0350250, 0x3FFF, 0,  global_i);
    BTDIGITAL_REG_SET_FIELD(0xD0350250, 0x3FFF, 14, global_q);
}

void btdrv_iq_datawrite(void)
{
    uint32_t en_addr_base = 0xD0310000;

    for (int k = 0; k<3; k++) {
        int ch1 = 0;
        int ch2 = 0;
        if(k==0) {
            ch1= 0;
            ch2= 25;
        } else if(k==1) {
            ch1= 26;
            ch2= 51;
        } else {
            ch1= 52;
            ch2= 78;
        }

        TRACE(3, "[k] gain:%d, phase:%d", g_gain[k], g_phase[k]);

        for (int ch = ch1; ch <= ch2; ch++) {
            uint32_t addre=en_addr_base+ch*4;
            set_gain_phase(addre,g_gain[k],g_phase[k]);
            //TRACE_IMM(2, "reg:0x%x = 0x%x", addre, BTDIGITAL_REG(addre));
        }
    }
    BTDIGITAL_REG_SET_FIELD(0xD0350250, 0x1, 31, 1);
}


#endif

#if defined(DPD_ONCHIP_CAL)

typedef struct DPDState_
{
    short *M0data;
    int *data_i;
    int *data_q;
} DPDState;

DPDState *DPDState_init(int fft_size)
{
    BT_DRV_TRACE(1,"%s malloc ini", __func__);
    DPDState *st = (DPDState *)bt_drv_malloc(1 * sizeof(DPDState));
    if (st) {
        st->M0data = (short*)bt_drv_malloc(fft_size * sizeof(short));
        st->data_i = (int*)bt_drv_malloc(fft_size/2 * sizeof(int));
        st->data_q = (int*)bt_drv_malloc(fft_size/2 * sizeof(int));

        BT_DRV_TRACE(1,"%s malloc ok", __func__);
    }
    return st;
}

int32_t DPDState_destroy(DPDState *st)
{
    bt_drv_free(st->M0data);
    bt_drv_free(st->data_i);
    bt_drv_free(st->data_q);
    bt_drv_free(st);
    return 0;
}

static int *dpd_am_lut_base;
static int *dpd_am_lut;
static int *dpd_pm_lut;

#define FREQ_NUM        3
//#define DPD_LOG

void btdrv_dpd_cal_entity(void)
{
    int fftsize;
    DPDState * st = NULL;
    int data_valid_flag = 0;
    int k=0;

    uint32_t time_start = hal_sys_timer_get();

    dpd_am_lut_base = (int*)bt_drv_malloc(128 * FREQ_NUM * sizeof(int));
    dpd_am_lut      = (int*)bt_drv_malloc(128 * FREQ_NUM * sizeof(int));
    dpd_pm_lut      = (int*)bt_drv_malloc(128 * FREQ_NUM * sizeof(int));

    fftsize = DPD_FFT_SIZE;
    st = DPDState_init(fftsize);

    for (k=0; k<FREQ_NUM; k++) {
        BTDIGITAL_REG_WR(0xD0220C00, 0x0);
        osDelay(1);

        if(0 == k) {
            BTDIGITAL_REG_WR(0xd0350248, 0x8040CCCD);//rate converter 2415
            BTDIGITAL_REG_WR(0xD0220C00, 0x800A000D);
        } else if (1 == k) {
            BTDIGITAL_REG_WR(0xd0350248, 0x80422FC9);//rate converter 2441
            BTDIGITAL_REG_WR(0xD0220C00, 0x800A0027);
        } else {
            BTDIGITAL_REG_WR(0xd0350248, 0x804392C6);//rate converter 2467
            BTDIGITAL_REG_WR(0xD0220C00, 0x800A0041);
        }
        osDelay(1);

        bt_iqimb_dma_enable(st->M0data, fftsize/2);
        while(1) {
            if(0 == iqimb_dma_status)
            break;
        }

        //check_mem_data((uint8_t *)st->M0data, fftsize);

        lib_iqmis_getiq((short *)st->M0data, fftsize, (float *)st->data_i, (float *)st->data_q);

        if (0 == k) {
            lib_bt_cal_dpd_coef_core((float *)st->data_i,(float *)st->data_q,fftsize/2,dpd_am_lut_base,dpd_am_lut,dpd_pm_lut, &data_valid_flag);
#ifdef DPD_LOG
            for (int i=0; i<128; i++) {
                TRACE_IMM(3, "[i=%d]dpd_am_lut_base=%d, dpd_am_lut=%d, dpd_pm_lut=%d", i,dpd_am_lut_base[i], dpd_am_lut[i], dpd_pm_lut[i]);
            }
#endif
        } else if (1 == k) {
            lib_bt_cal_dpd_coef_core((float *)st->data_i,(float *)st->data_q,fftsize/2,dpd_am_lut_base+128,dpd_am_lut+128,dpd_pm_lut+128, &data_valid_flag);
#ifdef DPD_LOG
            for (int i=128; i<256; i++) {
                TRACE_IMM(3, "[i=%d]dpd_am_lut_base=%d, dpd_am_lut=%d, dpd_pm_lut=%d", i,dpd_am_lut_base[i], dpd_am_lut[i], dpd_pm_lut[i]);
            }
#endif
        } else {
            lib_bt_cal_dpd_coef_core((float *)st->data_i,(float *)st->data_q,fftsize/2,dpd_am_lut_base+256,dpd_am_lut+256,dpd_pm_lut+256, &data_valid_flag);
#ifdef DPD_LOG
            for (int i=256; i<384; i++) {
                TRACE_IMM(3, "[i=%d]dpd_am_lut_base=%d, dpd_am_lut=%d, dpd_pm_lut=%d", i,dpd_am_lut_base[i], dpd_am_lut[i], dpd_pm_lut[i]);
            }
#endif
        }

        if (1 != data_valid_flag) {
            TRACE(1, "%s WARNING!!! DPD calib data invalid!",__func__);
        }
    }

    BT_DRV_TRACE(2,"%s use time: %d ms",__func__ ,__TICKS_TO_MS(hal_sys_timer_get()-time_start));

    DPDState_destroy(st);
}

void btdrv_dpd_datawrite(void)
{
    int i=0;
    int j=0;

    if ((NULL == dpd_am_lut_base) || (NULL == dpd_am_lut) || (NULL == dpd_pm_lut)) {
        TRACE(1, "%s data invalid!!!", __func__);
    }

    BTDIGITAL_REG_SET_FIELD(0xD035003C, 0x1, 20, 0x1);
    osDelay(1);
    BTDIGITAL_REG_SET_FIELD(0xD0330008, 0x1, 1, 0x1);//bit 1 should be set 0 after mem write
    osDelay(1);

    //write
    for (i=0; i<128; i++) {
        BTDIGITAL_REG(0xD0380600+i*4) = dpd_am_lut_base[i];
        BTDIGITAL_REG(0xD0380000+i*4) = dpd_am_lut[i];
        BTDIGITAL_REG(0xD0380800+i*4) = dpd_pm_lut[i];
    }

    for (i=128; i<256; i++) {
        j = i-128;
        BTDIGITAL_REG(0xD0380200+j*4) = dpd_am_lut[i];
        BTDIGITAL_REG(0xD0380A00+j*4) = dpd_pm_lut[i];
    }

    for (i=256; i<384; i++) {
        j = i-256;
        BTDIGITAL_REG(0xD0380400+j*4) = dpd_am_lut[i];
        BTDIGITAL_REG(0xD0380C00+j*4) = dpd_pm_lut[i];
    }
    BTDIGITAL_REG_SET_FIELD(0xD035003C, 0x1, 20, 0x0);
    BTDIGITAL_REG_SET_FIELD(0xD035003C, 0x1, 24, 0x1);
    BTDIGITAL_REG_SET_FIELD(0xD0330008, 0x1, 1, 0x0);
    BTDIGITAL_REG_SET_FIELD(0xD0350324, 0x1, 0, 0x0);

    bt_drv_free(dpd_am_lut_base);
    bt_drv_free(dpd_am_lut);
    bt_drv_free(dpd_pm_lut);
}

#endif
#endif

