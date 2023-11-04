#!/bin/bash
[ -z "$1" ] &&{
    echo The Chip is Null !!!
    exit
}

[ -z "$2" ] &&{
    echo The Target is Null !!!
    exit
}

[ -z "$3" ] &&{
    echo "The Target_Customer is Null !!! \n"
    echo "hearing_aid_customer or audio_customer "
    exit
}


CHIPID=$1
TARGET_LIST=$2
TARGET_CUSTOMER=$3
RELEASE=$4

SECURITY_ENABLE=$5
INCLUDE_SPAUDIO=$6
INCLUDE_SE=$7

echo "param opt:"
echo "CHIPID=$CHIPID TARGET_LIST=$TARGET_LIST TARGET_CUSTOMER=$TARGET_CUSTOMER RELEASE=$RELEASE SECURITY_ENABLE=$SECURITY_ENABLE INCLUDE_SPAUDIO=$INCLUDE_SPAUDIO INCLUDE_SE=$INCLUDE_SE"


TARGET_DIR="config/"$TARGET_LIST
TARGET_NAME="$TARGET_LIST"

#git clean -d -fx
DATE=`date +%F | sed 's/-//g'`
commitid=`git rev-parse --short HEAD`

export CROSS_COMPILE="arm-none-eabi-"

if [[ $RELEASE == "release" ]];
then
    echo -e "rm old lib"
    LIB_DIR=lib/bes/
    rm build_err.log
    rm -rf $LIB_DIR
    rm out -rf
fi


################################### var define ###################################

BTH_SYS_CFG="BTH_AS_MAIN_MCU=1 BTH_USE_SYS_PERIPH=1 HOST_GEN_ECDH_KEY=1 BT_RAMRUN_NEW=1 BT_RF_PREFER=2M A2DP_AAC_ON=1 "



BTH_CUSTOMER_CFG="BES_OTA=1 IGNORE_POWER_ON_KEY_DURING_BOOT_UP=1 BT_MUTE_A2DP=1 BT_PAUSE_A2DP=0 BT_CLOSE_A2DP=0 \
                IS_AUTOPOWEROFF_ENABLED=0 POWER_ON_ENTER_TWS_PAIRING_ENABLED=1 FREE_TWS_PAIRING_ENABLED=1 \
                AOB_MOBILE_ENABLED=0 IGNORE_POWER_ON_KEY_DURING_BOOT_UP=1 IS_AUTOPOWEROFF_ENABLED=0 \
                BT_FA_ECC=0 \
                TRACE_BUF_SIZE=30*1024 APP_TRACE_RX_ENABLE=1 APP_RX_API_ENABLE=1 AUDIO_DEBUG_CMD=1 BLE=1 BLE_CONNECTION_MAX=2 BT_RAMRUN_NEW=1 OS_TASKCNT=22"
OTA_COPY_OFFSET_CFG="OTA_CODE_OFFSET=0x18000 OTA_BOOT_SIZE=0x10000 OTA_BOOT_INFO_OFFSET=0x10000"

if [[ $CHIPID == "best1600" ]];
then
M55_DECODE_DECODER_CFG="DSP_M55=1 GAF_CODEC_CROSS_CORE=1 GAF_DECODER_CROSS_CORE_USE_M55=1 GAF_ENCODER_CROSS_CORE_USE_M55=1 A2DP_DECODER_CROSS_CORE_USE_M55=1 A2DP_DECODER_CROSS_CORE=1 "

BTH_CAPSENSOR_CFG=""

BTH_DECODE_M55_DECODER_CFG="DSP_M55=1 A2DP_CP_ACCEL=0 SCO_CP_ACCEL=0 CHIP_HAS_CP=0 GAF_CODEC_CROSS_CORE=1 GAF_DECODER_CROSS_CORE_USE_M55=1 GAF_ENCODER_CROSS_CORE_USE_M55=1 A2DP_DECODER_CROSS_CORE_USE_M55=1 A2DP_DECODER_CROSS_CORE=1 "
BTH_DECODE_CP_DECODER_CFG=""
LOW_PWR_CFG=""
BTH_CUSTOMER_CFG="${BTH_CUSTOMER_CFG} FAST_XRAM_SECTION_SIZE=0x19000"

elif [[ $CHIPID == "best1603" ]];
then

BTH_DECODE_CP_DECODER_CFG="A2DP_CP_ACCEL=1 CHIP_HAS_CP=1 AAC_REDUCE_SIZE=1 SBC_REDUCE_SIZE=1 "
# LOW_PWR_CFG="VCODEC_VOLT=1.35V CHIP_HAS_CP=1 A2DP_CP_ACCEL=1 AFH_ASSESS=0 "
LOW_PWR_CFG="CHIP_HAS_CP=1 A2DP_CP_ACCEL=1 AFH_ASSESS=0 "
BTH_CUSTOMER_CFG="${BTH_CUSTOMER_CFG} AUDIO_BUFFER_SIZE=90*1024 FAST_XRAM_SECTION_SIZE=0xd000 HIGH_EFFICIENCY_TX_PWR_CTRL=1 BT_NO_SLEEP=1"
fi

################################### end of var define ###################################



################################### build  start ###################################

if [[ $RELEASE == "release" ]];
then

make T=prod_test/ota_copy GEN_LIB=1 DEBUG=1 -j40 CHIP=$CHIPID  $OTA_COPY_OFFSET_CFG
make T=prod_test/ota_copy GEN_LIB=1 DEBUG=1 -j40 CHIP=$CHIPID OTA_BIN_COMPRESSED=1 $OTA_COPY_OFFSET_CFG
make T=sensor_hub GEN_LIB=1 CHIP=$CHIPID BTH_AS_MAIN_MCU=1 SENS_TRC_TO_MCU=1 DEBUG=1 VOICE_DETECTOR_EN=0 -j40
#make T=dsp_m55 GEN_LIB=1 CHIP=$CHIPID $M55_SYS_CFG $M55_DECODE_DECODER_CFG $M55_PSAP_SW_CFG -j40

if [[ $CHIPID == "best1600" ]];
then
TARGET_LIST="best1600_ibrt"
elif [[ $CHIPID == "best1603" ]];
then
TARGET_LIST="best1603_ibrt"
fi

### basic ###
make T=$TARGET_LIST GEN_LIB=1 -j40 $BTH_SYS_CFG $BTH_CUSTOMER_CFG $BTH_DECODE_CP_DECODER_CFG BLE=1 

[[ $SECURITY_ENABLE == security_enable ]] &&{
# SECURE_BOOT_FLAGS="SECURE_BOOT=1 USER_SECURE_BOOT=1"
# CRYPT_LOAD_FLAGS="LARGE_SE_RAM=1"
PROJECT_FUNC_DEMO="SE_OTP_DEMO_TEST=1 CMSE_CRYPT_TEST_DEMO=1 FLASH_SECURITY_REGISTER=1 MBEDTLS_CONFIG_FILE="config-rsa.h""

CUSTOMER_LOAD_SECTION_FLAGS="CUSTOMER_LOAD_SRAM_TEXT_RAMX_SECTION_SIZE=0x1000 CUSTOMER_LOAD_RAM_DATA_SECTION_SIZE=0x1000 CUSTOMER_LOAD_ENC_DEC_RECORD_SECTION_SIZE=0x4000"
PROJECT_CFG_FLAGS="FLASH_SIZE=0x800000 SPA_AUDIO_ENABLE=1 SPA_AUDIO_SEC=1 "
# CP_DISABLE_FLAGS="CHIP_HAS_CP=0 A2DP_CP_ACCEL=0 SCO_CP_ACCEL=0 NO_OVERLAY=1"

#EXTRA_PROJECT_FEATURE_CFG_FLAGS="SPEECH_TX_2MIC_NS7=1 SPEECH_TX_MIC_FIR_CALIBRATION=1 TOTA_v2=1 CUSTOMER_APP_BOAT=1 GFPS_ENABLE=1"

CHIP_TYPES="CHIP=best1600 "
CHIP_BASIC_CFG=""

#########################   arm_cmse    #########################
CHIP_BASIC_CFG="DEBUG_PORT=1 CHIP=best1600 USE_MEM_CFG=1 FLASH_SIZE=0x800000 TRACE_BAUD_RATE=1152000"
make T=arm_cmse DEBUG=1 -j40 FLASH_SECURITY_REGISTER=1 CRC32_ROM=1 GEN_LIB=1 \
    $CHIP_TYPES $CHIP_BASIC_CFG\
    $PROJECT_CFG_FLAGS $CRYPT_LOAD_FLAGS $PROJECT_FUNC_DEMO $SECURE_BOOT_FLAGS 

#########################   arm_cmns    #########################
CHIP_BASIC_CFG="CHIP_SUBSYS= BTH_AS_MAIN_MCU=0 BTH_USE_SYS_PERIPH=0 DPD_ONCHIP_CAL=0 NO_MPU_DEFAULT_MAP=0 FLASH_SIZE=0x800000 TRACE_BAUD_RATE=1152000"
make T=best1600_ibrt $CHIP_BASIC_CFG DEBUG=1 -j40 ARM_CMNS=1 GEN_LIB=1 \
    POWER_ON_ENTER_TWS_PAIRING_ENABLED=1 IGNORE_POWER_ON_KEY_DURING_BOOT_UP=1 OTA_CODE_OFFSET=0x80000 \
    $CP_DISABLE_FLAGS \
    $PROJECT_CFG_FLAGS $CRYPT_LOAD_FLAGS $SECURE_BOOT_FLAGS USER_SECURE_BOOT=0
}

else

hearing_aid_customer=" \
                    $LOW_PWR_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_DECODE_M55_DECODER_CFG \
                    $BTH_ALGO_MCPP $BTH_LEA_ENABLE_CFG \
                    $BTH_PSAP_SW_CFG \
                    PMU_LOW_POWER_CLOCK=1 "
audio_customer=" \
                    $LOW_PWR_CFG $BTH_DECODE_CP_DECODER_CFG $BTH_LEA_ENABLE_CFG"

make T=prod_test/ota_copy  DEBUG=1 -j40 CHIP=$CHIPID $OTA_COPY_OFFSET_CFG
make T=sensor_hub  CHIP=$CHIPID BTH_AS_MAIN_MCU=1 SENS_TRC_TO_MCU=1 DEBUG=1 VOICE_DETECTOR_EN=0 -j40

if [[ $TARGET_CUSTOMER == "audio_customer" ]];
then
#m55_build_cmd="make T=dsp_m55  CHIP=$CHIPID $M55_DECODE_DECODER_CFG  $M55_SYS_CFG  $M55_ALGO_MCPP -j40 "

bth_build_cmd="make T=$TARGET_LIST CHIP=$CHIPID $BTH_SYS_CFG $BTH_CUSTOMER_CFG  -j40 \
            $audio_customer"
elif [[ $TARGET_CUSTOMER == "hearing_aid_customer" ]];
then
m55_build_cmd="make T=dsp_m55  CHIP=$CHIPID $M55_DECODE_DECODER_CFG  $M55_SYS_CFG $M55_ALGO_MCPP $M55_PSAP_SW_CFG -j40 "

bth_build_cmd="make T=$TARGET_LIST CHIP=$CHIPID $BTH_SYS_CFG $BTH_CUSTOMER_CFG  -j40 \
            $hearing_aid_customer"
fi

$m55_build_cmd
$bth_build_cmd
echo "m55_build_cmd: "
echo "$m55_build_cmd"
echo "bth_build_cmd: "
echo "$bth_build_cmd"
fi

################################### end of build  start ###################################

################################### release script ###################################
if [[ $RELEASE == "release" ]];
then
PROGRAMMER_SRC="NO"
BT_SRC="NO"
SM_SRC="NO"
USB_SRC="NO"
BLE_STACK_SRC="NO"
BLE_GFPS_SRC="NO"
INTERSYSHCI_SRC="NO"
BRIDGE_SRC="NO"
ENCRYPT="NO"
GAF_COER="NO"


EXPORT_LIB="$EXPORT_LIB
`cat <<-EOF
EOF
`
"

EXPORT_FILE_DIR="$EXPORT_FILE_DIR
`cat <<-EOF
config/prod_test/ota_copy
config/sensor_hub
config/dsp_m55
tools/build_1600_1603_cmd.sh
EOF
`
"


[[ $CHIPID == best1600 ]] &&{
EXPORT_FILE_DIR="$EXPORT_FILE_DIR
`cat <<-EOF
config/best1600
config/best1600_ibrt
config/best1600_ibrt_anc
EOF
`
"
}

[[ $CHIPID == best1603 ]] &&{
EXPORT_FILE_DIR="$EXPORT_FILE_DIR
`cat <<-EOF
config/best1603
config/best1603_ibrt
config/best1603_ibrt_anc
EOF
`
"
}


[[ $INCLUDE_SPAUDIO == INCLUDE_SPAUDIO ]] &&{
EXPORT_FILE_DIR="$EXPORT_FILE_DIR
`cat <<-EOF
thirdparty/userapi/spa_dummy_app/
EOF
`
"
}

[[ $INCLUDE_SE == INCLUDE_SE ]] &&{
EXPORT_FILE_DIR="$EXPORT_FILE_DIR
`cat <<-EOF
Makefile
config/Makefile
config/common.mk
config/_default_cfg_src_
config/noapp_test
config/arm_cmns
config/arm_cmse
include
platform/cmsis
platform/drivers/Makefile
platform/drivers/graphic/*graphic_conf.h
platform/drivers/mpc/Makefile
platform/drivers/mpc/*.[ch]
platform/drivers/mpc/$CHIPID
platform/drivers/norflash
platform/drivers/spi_norflash
platform/drivers/usb/usb_dev/inc
platform/hal/Makefile
platform/hal/$CHIPID
platform/hal/*.[ch]
platform/main
tests/hwtest/hwtest.h
tests/hwtest/tz_test.c
tools/bin2ascii.pl
tools/fill_sec_base.pl
tools/1600_tz_build.sh
utils/boot_struct
utils/build_info
utils/crash_catcher
utils/heap
utils/hexdump
utils/hwtimer_list
utils/libc
utils/rom_utils/export_fn_rom.h
utils/xyzmodem
utils/mbedtls
utils/system_info
utils/user_secure_boot
EOF
`
"
}

sed -i 's/\r//' `dirname $0`/relsw_ibrt_common
. `dirname $0`/relsw_ibrt_common

fi
################################### end of release script ###################################
