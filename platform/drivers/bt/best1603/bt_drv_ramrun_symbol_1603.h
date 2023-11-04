
/*******************************************************************************************************************************/
/*********************************************bt controller symbol**************************************************************/

#ifndef  __BT_CONTROLLER_SYMBOL_H__
#define  __BT_CONTROLLER_SYMBOL_H__


#define HCI_FC_ENV_ADDR                                               0XC0549BA4
#define LD_ACL_ENV_ADDR                                               0XC0546E70
#define BT_UTIL_BUF_ENV_ADDR                                          0XC0545F14
#define BLE_UTIL_BUF_ENV_ADDR                                         0XC054728C
#define LD_BES_BT_ENV_ADDR                                            0XC0547048
#define DBG_STATE_ADDR                                                0XC0549CC4
#define LC_STATE_ADDR                                                 0XC05465F0
#define LD_SCO_ENV_ADDR                                               0XC0546E8C
#define RX_MONITOR_ADDR                                               0XC054A568
#define LC_ENV_ADDR                                                   0XC05465D4
#define LM_NB_SYNC_ACTIVE_ADDR                                        0XC05465CE
#define LM_ENV_ADDR                                                   0XC0546280
#define LM_KEY_ENV_ADDR                                               0XC05463EC
#define HCI_ENV_ADDR                                                  0XC0549A98
#define LC_SCO_ENV_ADDR                                               0XC05465B0
#define LLM_ENV_ADDR                                                  0XC0547950
#define LD_ENV_ADDR                                                   0XC054691C
#define RWIP_ENV_ADDR                                                 0XC054A734
#define BLE_RX_MONITOR_ADDR                                           0XC054A508
#define LLC_ENV_ADDR                                                  0XC0548C8C
#define RWIP_RF_ADDR                                                  0XC054A6D0
#define LD_ACL_METRICS_ADDR                                           0XC0546970
#define RF_RX_HWGAIN_TBL_ADDR                                         0XC05443EE
#define RF_HWAGC_RSSI_CORRECT_TBL_ADDR                                0XC054430C
#define RF_RX_GAIN_FIXED_TBL_ADDR                                     0XC054A4C6
#define HCI_DBG_EBQ_TEST_MODE_ADDR                                    0XC0549DB8
#define HOST_REF_CLK_ADDR                                             0XC0545EEC
#define DBG_TRC_TL_ENV_ADDR                                           0XC0549E8C
#define DBG_TRC_MEM_ENV_ADDR                                          0XC0549E98
#define DBG_BT_COMMON_SETTING_ADDR                                    0XC0549D24
#define DBG_BT_SCHE_SETTING_ADDR                                      0XC0549DE8
#define DBG_BT_IBRT_SETTING_ADDR                                      0XC0549C48
#define DBG_BT_HW_FEAT_SETTING_ADDR                                   0XC0549CF8
#define HCI_DBG_SET_SW_RSSI_ADDR                                      0XC0549E10
#define DATA_BACKUP_CNT_ADDR                                          0XC0544070
#define DATA_BACKUP_ADDR_PTR_ADDR                                     0XC0544074
#define DATA_BACKUP_VAL_PTR_ADDR                                      0XC0544078
#define SCH_MULTI_IBRT_ADJUST_ENV_ADDR                                0XC0549880
#define RF_HWAGC_RSSI_CORRECT_TBL_ADDR                                0XC054430C
#define RF_RX_GAIN_THS_TBL_LE_ADDR                                    0XC05443B2
#define RF_RX_GAIN_THS_TBL_LE_2M_ADDR                                 0XC05443D0
#define RF_RPL_TX_PW_CONV_TBL_ADDR                                    0XC054431C
#define REPLACE_MOBILE_ADDR_ADDR                                      0XC054685C
#define REPLACE_ADDR_VALID_ADDR                                       0XC0544626
#define PCM_NEED_START_FLAG_ADDR                                      0XC0544B9C
#define RT_SLEEP_FLAG_CLEAR_ADDR                                      0XC0545F04
#define RF_RX_GAIN_THS_TBL_BT_3M_ADDR                                 0XC054A4A8
#define POWER_ADJUST_EN_ADDR                                          0XC0545EF2
#define LD_IBRT_ENV_ADDR                                              0XC0547060
#define LLM_LOCAL_LE_FEATS_ADDR                                       0XC054429E
#define ISOOHCI_ENV_ADDR                                              0XC05498D4
#define RF_RX_GAIN_THS_TBL_BT_ADDR                                    0XC0544358
#define DBG_BT_COMMON_SETTING_T2_ADDR                                 0XC0549D70
#define LLD_CON_ENV_ADDR                                              0XC054920C
#define POWER_ADJUST_EN_ADDR                                          0XC0545EF2
#define LLD_ISO_ENV_ADDR                                              0XC054925C
#define ECC_RX_MONITOR_ADDR                                           0XC054A4A0
#define __STACKLIMIT_ADDR                                             0XC054B320
#define PER_MONITOR_PARAMS_ADDR                                       0XC0544290
#define TX_POWER_VAL_BKUP_ADDR                                        0XC054A660
#define DBG_TRC_ENV_ADDR                                              0XC0549EA8
#define DBG_ENV_ADDR                                                  0XC0549EB0
#define MASTER_CON_SUPPORT_LE_AUDIO_ADDR                              0XC0544624
#define DBG_BT_COMMON_SETTING_T2_ADDR                                 0XC0549D70
#define RX_RECORD_ADDR                                                0XC0546EDA
#define SCH_PROG_DBG_ENV_ADDR                                         0XC0549548
#define RWIP_RST_STATE_ADDR                                           0XC0545EF3
#define RF_RX_GAIN_THS_TBL_ECC_ADDR                                   0XC0544394
#define BTC_PUBLIC_KEY_GENERATE_ENABLE_ADDR                           0XC0544218
#define RX_MONITOR_3M_RXGAIN_ADDR                                     0XC0544439
#define CON_LATENCY_APPLY_ADDR                                        0XC05442B8
#define LLD_PER_ADV_ENV_ADDR                                          0XC0549154
#define MHDT_SWAGC_EN_ADDR                                            0XC0545EF1
#define RF_RX_GAIN_THS_TBL_MHDT_BT2M_1M_ADDR                          0XC054A464
#define RF_RX_GAIN_THS_TBL_MHDT_BT2M_2M_ADDR                          0XC054A4D2
#define RF_RX_GAIN_THS_TBL_MHDT_BT2M_3M_ADDR                          0XC054A578
#define RF_RX_GAIN_THS_TBL_MHDT_BT4M_1M_ADDR                          0XC054A538
#define RF_RX_GAIN_THS_TBL_MHDT_BT4M_2M_ADDR                          0XC054A59C
#define RF_RX_GAIN_THS_TBL_MHDT_BT4M_3M_ADDR                          0XC054A6AC
#define LE_CON_SKIP_3_INTERVAL_ADDR                                   0XC0545DFC
#define LLD_BIS_ENV_ADDR                                              0XC05493A0
#define LLD_BIG_ENV_ADDR                                              0XC0549390
#define LLD_CIS_ENV_ADDR                                              0XC0549340
#define LLD_CIG_ENV_ADDR                                              0XC0549314
#define REPLACE_MOBILE_LE_LINK_ID_ADDR                                0XC05442AC
#define REPLACE_MOBILE_LE_LINK_ID_VALID_ADDR                          0XC0544BD1

//commit c0dc616dde4822cbc529b3729b40a6ef505f5a45
//Author: honghaolu <honghaolu@bestechnic.com>
//Date:   Thu Sep 28 15:56:46 2023 +0800
//    Revert "merge1600:optimize cis and BT coexist out of range behave causing assert"
//    
//    This reverts commit 9f070f96574a5d48b2916a76e98c2edf2e64330d.
//    
//    Change-Id: I4d773aa3402e6be2a518fc8bfbcb4129ef840ae2

#ifndef  BT_CONTROLLER_COMMIT_ID
#define  BT_CONTROLLER_COMMIT_ID                            "commit c0dc616dde4822cbc529b3729b40a6ef505f5a45"
#endif
#ifndef  BT_CONTROLLER_COMMIT_AUTHOR
#define  BT_CONTROLLER_COMMIT_AUTHOR                        "Author: honghaolu <honghaolu@bestechnic.com>"
#endif
#ifndef  BT_CONTROLLER_COMMIT_DATE
#define  BT_CONTROLLER_COMMIT_DATE                          "Date:   Thu Sep 28 15:56:46 2023 +0800"
#endif
#ifndef  BT_CONTROLLER_COMMIT_MESSAGE
#define  BT_CONTROLLER_COMMIT_MESSAGE                       "Revert "merge1600:optimize cis and BT coexist out of range behave causing assert"  This reverts commit 9f070f96574a5d48b2916a76e98c2edf2e64330d.  Change-Id: I4d773aa3402e6be2a518fc8bfbcb4129ef840ae2 "
#endif
#ifndef  BT_CONTROLLER_BUILD_TIME
#define  BT_CONTROLLER_BUILD_TIME                           "2023-09-28 16:04:48"
#endif
#ifndef  BT_CONTROLLER_BUILD_OWNER
#define  BT_CONTROLLER_BUILD_OWNER                          "honghaolu"
#endif

#endif
