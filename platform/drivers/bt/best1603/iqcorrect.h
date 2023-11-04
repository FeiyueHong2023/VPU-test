#ifndef __IQ_CORRECT_H__
#define __IQ_CORRECT_H__

#ifdef __cplusplus
extern "C" {
#endif


// configure the bt iq calibration on system boot-up

void dc_iq_calib_1603(void);

void btdrv_iq_datawrite(void);
void btdrv_dc_datawrite(void);

void btdrv_dpd_cal_entity(void);

void btdrv_dpd_datawrite(void);

#ifdef __cplusplus
}
#endif

#endif // __IQ_CORRECT_H__

