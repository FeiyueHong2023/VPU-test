/***************************************************************************
 *
 * Copyright 2020-2022 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef __REG_CODEC_BEST1603_H__
#define __REG_CODEC_BEST1603_H__

#include "plat_types.h"

struct CODEC_T {
    __IO uint32_t REG_000;
    __IO uint32_t REG_004;
    __IO uint32_t REG_008;
    __IO uint32_t REG_00C;
    __IO uint32_t REG_010;
    __IO uint32_t REG_014;
    __IO uint32_t REG_018;
    __IO uint32_t REG_01C;
    __IO uint32_t REG_020;
    __IO uint32_t REG_024;
    __IO uint32_t REG_028;
    __IO uint32_t REG_02C;
    __IO uint32_t REG_030;
    __IO uint32_t REG_034;
    __IO uint32_t REG_038;
    __IO uint32_t REG_03C;
    __IO uint32_t REG_040;
    __IO uint32_t REG_044;
    __IO uint32_t REG_048;
    __IO uint32_t REG_04C;
    __IO uint32_t REG_050;
    __IO uint32_t REG_054;
    __IO uint32_t REG_058;
    __IO uint32_t REG_05C;
    __IO uint32_t REG_060;
    __IO uint32_t REG_064;
    __IO uint32_t REG_068;
    __IO uint32_t REG_06C;
    __IO uint32_t REG_070;
    __IO uint32_t REG_074;
    __IO uint32_t REG_078;
    __IO uint32_t REG_07C;
    __IO uint32_t REG_080;
    __IO uint32_t REG_084;
    __IO uint32_t REG_088;
    __IO uint32_t REG_08C;
    __IO uint32_t REG_090;
    __IO uint32_t REG_094;
    __IO uint32_t REG_098;
    __IO uint32_t REG_09C;
    __IO uint32_t REG_0A0;
    __IO uint32_t REG_0A4;
    __IO uint32_t REG_0A8;
    __IO uint32_t REG_0AC;
    __IO uint32_t REG_0B0;
    __IO uint32_t REG_0B4;
    __IO uint32_t REG_0B8;
    __IO uint32_t REG_0BC;
    __IO uint32_t REG_0C0;
    __IO uint32_t REG_0C4;
    __IO uint32_t REG_0C8;
    __IO uint32_t REG_0CC;
    __IO uint32_t REG_0D0;
    __IO uint32_t REG_0D4;
    __IO uint32_t REG_0D8;
    __IO uint32_t REG_0DC;
    __IO uint32_t REG_0E0;
    __IO uint32_t REG_0E4;
    __IO uint32_t REG_0E8;
    __IO uint32_t REG_0EC;
    __IO uint32_t REG_0F0;
    __IO uint32_t REG_0F4;
    __IO uint32_t REG_0F8;
    __IO uint32_t REG_0FC;
    __IO uint32_t REG_100;
    __IO uint32_t REG_104;
    __IO uint32_t REG_108;
    __IO uint32_t REG_10C;
    __IO uint32_t REG_110;
    __IO uint32_t REG_114;
    __IO uint32_t REG_118;
    __IO uint32_t REG_11C;
    __IO uint32_t REG_120;
    __IO uint32_t REG_124;
    __IO uint32_t REG_128;
    __IO uint32_t REG_12C;
    __IO uint32_t REG_130;
    __IO uint32_t REG_134;
    __IO uint32_t REG_138;
    __IO uint32_t REG_13C;
    __IO uint32_t REG_140;
    __IO uint32_t REG_144;
    __IO uint32_t REG_148;
    __IO uint32_t REG_14C;
    __IO uint32_t REG_150;
    __IO uint32_t REG_154;
    __IO uint32_t REG_158;
    __IO uint32_t REG_15C;
    __IO uint32_t REG_160;
    __IO uint32_t REG_164;
    __IO uint32_t REG_168;
    __IO uint32_t REG_16C;
    __IO uint32_t REG_170;
    __IO uint32_t REG_174;
    __IO uint32_t REG_178;
    __IO uint32_t REG_17C;
    __IO uint32_t REG_180;
    __IO uint32_t REG_184;
    __IO uint32_t REG_188;
    __IO uint32_t REG_18C;
    __IO uint32_t REG_190;
    __IO uint32_t REG_194;
    __IO uint32_t REG_198;
    __IO uint32_t REG_19C;
    __IO uint32_t REG_1A0;
    __IO uint32_t REG_1A4;
    __IO uint32_t REG_1A8;
    __IO uint32_t REG_1AC;
    __IO uint32_t REG_1B0;
    __IO uint32_t REG_1B4;
    __IO uint32_t REG_1B8;
    __IO uint32_t REG_1BC;
    __IO uint32_t REG_1C0;
    __IO uint32_t REG_1C4;
    __IO uint32_t REG_1C8;
    __IO uint32_t REG_1CC;
    __IO uint32_t REG_1D0;
    __IO uint32_t REG_1D4;
    __IO uint32_t REG_1D8;
    __IO uint32_t REG_1DC;
    __IO uint32_t REG_1E0;
    __IO uint32_t REG_1E4;
    __IO uint32_t REG_1E8;
    __IO uint32_t REG_1EC;
    __IO uint32_t REG_1F0;
    __IO uint32_t REG_1F4;
    __IO uint32_t REG_1F8;
    __IO uint32_t REG_1FC;
    __IO uint32_t REG_200;
    __IO uint32_t REG_204;
    __IO uint32_t REG_208;
    __IO uint32_t REG_20C;
    __IO uint32_t REG_210;
    __IO uint32_t REG_214;
    __IO uint32_t REG_218;
    __IO uint32_t REG_21C;
    __IO uint32_t REG_220;
    __IO uint32_t REG_224;
    __IO uint32_t REG_228;
    __IO uint32_t REG_22C;
    __IO uint32_t REG_230;
    __IO uint32_t REG_234;
    __IO uint32_t REG_238;
    __IO uint32_t REG_23C;
    __IO uint32_t REG_240;
    __IO uint32_t REG_244;
    __IO uint32_t REG_248;
    __IO uint32_t REG_24C;
    __IO uint32_t REG_250;
    __IO uint32_t REG_254;
    __IO uint32_t REG_258;
    __IO uint32_t REG_25C;
    __IO uint32_t REG_260;
    __IO uint32_t REG_264;
    __IO uint32_t REG_268;
    __IO uint32_t REG_26C;
    __IO uint32_t REG_270;
    __IO uint32_t REG_274;
    __IO uint32_t REG_278;
    __IO uint32_t REG_27C;
    __IO uint32_t REG_280;
    __IO uint32_t REG_284;
    __IO uint32_t REG_288;
    __IO uint32_t REG_28C;
    __IO uint32_t REG_290;
    __IO uint32_t REG_294;
    __IO uint32_t REG_298;
    __IO uint32_t REG_29C;
    __IO uint32_t REG_2A0;
    __IO uint32_t REG_2A4;
    __IO uint32_t REG_2A8;
    __IO uint32_t REG_2AC;
    __IO uint32_t REG_2B0;
    __IO uint32_t REG_2B4;
    __IO uint32_t REG_2B8;
    __IO uint32_t REG_2BC;
    __IO uint32_t REG_2C0;
    __IO uint32_t REG_2C4;
    __IO uint32_t REG_2C8;
    __IO uint32_t REG_2CC;
    __IO uint32_t REG_2D0;
    __IO uint32_t REG_2D4;
    __IO uint32_t REG_2D8;
    __IO uint32_t REG_2DC;
    __IO uint32_t REG_2E0;
    __IO uint32_t REG_2E4;
    __IO uint32_t REG_2E8;
    __IO uint32_t REG_2EC;
    __IO uint32_t REG_2F0;
    __IO uint32_t REG_2F4;
    __IO uint32_t REG_2F8;
    __IO uint32_t REG_2FC;
    __IO uint32_t REG_300;
    __IO uint32_t REG_304;
    __IO uint32_t REG_308;
    __IO uint32_t REG_30C;
    __IO uint32_t REG_310;
    __IO uint32_t REG_314;
    __IO uint32_t REG_318;
    __IO uint32_t REG_31C;
    __IO uint32_t REG_320;
    __IO uint32_t REG_324;
    __IO uint32_t REG_328;
    __IO uint32_t REG_32C;
    __IO uint32_t REG_330;
    __IO uint32_t REG_334;
    __IO uint32_t REG_338;
    __IO uint32_t REG_33C;
    __IO uint32_t REG_340;
    __IO uint32_t REG_344;
    __IO uint32_t REG_348;
    __IO uint32_t REG_34C;
    __IO uint32_t REG_350;
    __IO uint32_t REG_354;
    __IO uint32_t REG_358;
    __IO uint32_t REG_35C;
    __IO uint32_t REG_360;
    __IO uint32_t REG_364;
    __IO uint32_t REG_368;
    __IO uint32_t REG_36C;
    __IO uint32_t REG_370;
    __IO uint32_t REG_374;
    __IO uint32_t REG_378;
    __IO uint32_t REG_37C;
    __IO uint32_t REG_380;
    __IO uint32_t REG_384;
    __IO uint32_t REG_388;
    __IO uint32_t REG_38C;
    __IO uint32_t REG_390;
    __IO uint32_t REG_394;
    __IO uint32_t REG_398;
    __IO uint32_t REG_39C;
    __IO uint32_t REG_3A0;
    __IO uint32_t REG_3A4;
    __IO uint32_t REG_3A8;
    __IO uint32_t REG_3AC;
    __IO uint32_t REG_3B0;
    __IO uint32_t REG_3B4;
    __IO uint32_t REG_3B8;
    __IO uint32_t REG_3BC;
    __IO uint32_t REG_3C0;
    __IO uint32_t REG_3C4;
    __IO uint32_t REG_3C8;
    __IO uint32_t REG_3CC;
    __IO uint32_t REG_3D0;
    __IO uint32_t REG_3D4;
    __IO uint32_t REG_3D8;
    __IO uint32_t REG_3DC;
    __IO uint32_t REG_3E0;
    __IO uint32_t REG_3E4;
    __IO uint32_t REG_3E8;
    __IO uint32_t REG_3EC;
    __IO uint32_t REG_3F0;
    __IO uint32_t REG_3F4;
    __IO uint32_t REG_3F8;
    __IO uint32_t REG_3FC;
    __IO uint32_t REG_400;
    __IO uint32_t REG_404;
    __IO uint32_t REG_408;
    __IO uint32_t REG_40C;
    __IO uint32_t REG_410;
    __IO uint32_t REG_414;
    __IO uint32_t REG_418;
    __IO uint32_t REG_41C;
    __IO uint32_t REG_420;
    __IO uint32_t REG_424;
    __IO uint32_t REG_428;
    __IO uint32_t REG_42C;
    __IO uint32_t REG_430;
    __IO uint32_t REG_434;
    __IO uint32_t REG_438;
    __IO uint32_t REG_43C;
    __IO uint32_t REG_440;
    __IO uint32_t REG_444;
    __IO uint32_t REG_448;
    __IO uint32_t REG_44C;
    __IO uint32_t REG_450;
    __IO uint32_t REG_454;
    __IO uint32_t REG_458;
    __IO uint32_t REG_45C;
    __IO uint32_t REG_460;
    __IO uint32_t REG_464;
    __IO uint32_t REG_468;
    __IO uint32_t REG_46C;
    __IO uint32_t REG_470;
    __IO uint32_t REG_474;
    __IO uint32_t REG_478;
    __IO uint32_t REG_47C;
    __IO uint32_t REG_480;
    __IO uint32_t REG_484;
    __IO uint32_t REG_488;
    __IO uint32_t REG_48C;
    __IO uint32_t REG_490;
    __IO uint32_t REG_494;
    __IO uint32_t REG_498;
    __IO uint32_t REG_49C;
    __IO uint32_t REG_4A0;
    __IO uint32_t REG_4A4;
    __IO uint32_t REG_4A8;
    __IO uint32_t REG_4AC;
    __IO uint32_t REG_4B0;
    __IO uint32_t REG_4B4;
    __IO uint32_t REG_4B8;
    __IO uint32_t REG_4BC;
    __IO uint32_t REG_4C0;
    __IO uint32_t REG_4C4;
    __IO uint32_t REG_4C8;
    __IO uint32_t REG_4CC;
    __IO uint32_t REG_4D0;
    __IO uint32_t REG_4D4;
    __IO uint32_t REG_4D8;
    __IO uint32_t REG_4DC;
    __IO uint32_t REG_4E0;
    __IO uint32_t REG_4E4;
    __IO uint32_t REG_4E8;
    __IO uint32_t REG_4EC;
    __IO uint32_t REG_4F0;
    __IO uint32_t REG_4F4;
    __IO uint32_t REG_4F8;
    __IO uint32_t REG_4FC;
    __IO uint32_t REG_500;
    __IO uint32_t REG_504;
    __IO uint32_t REG_508;
    __IO uint32_t REG_50C;
    __IO uint32_t REG_510;
    __IO uint32_t REG_514;
    __IO uint32_t REG_518;
    __IO uint32_t REG_51C;
    __IO uint32_t REG_520;
    __IO uint32_t REG_524;
    __IO uint32_t REG_528;
    __IO uint32_t REG_52C;
    __IO uint32_t REG_530;
    __IO uint32_t REG_534;
    __IO uint32_t REG_538;
    __IO uint32_t REG_53C;
    __IO uint32_t REG_540;
    __IO uint32_t REG_544;
    __IO uint32_t REG_548;
    __IO uint32_t REG_54C;
    __IO uint32_t REG_550;
    __IO uint32_t REG_554;
    __IO uint32_t REG_558;
    __IO uint32_t REG_55C;
    __IO uint32_t REG_560;
    __IO uint32_t REG_564;
    __IO uint32_t REG_568;
    __IO uint32_t REG_56C;
    __IO uint32_t REG_570;
    __IO uint32_t REG_574;
    __IO uint32_t REG_578;
    __IO uint32_t REG_57C;
    __IO uint32_t REG_580;
    __IO uint32_t REG_584;
    __IO uint32_t REG_588;
    __IO uint32_t REG_58C;
    __IO uint32_t REG_590;
    __IO uint32_t REG_594;
    __IO uint32_t REG_598;
    __IO uint32_t REG_59C;
    __IO uint32_t REG_5A0;
    __IO uint32_t REG_5A4;
    __IO uint32_t REG_5A8;
    __IO uint32_t REG_5AC;
    __IO uint32_t REG_5B0;
    __IO uint32_t REG_5B4;
    __IO uint32_t REG_5B8;
    __IO uint32_t REG_5BC;
    __IO uint32_t REG_5C0;
    __IO uint32_t REG_5C4;
    __IO uint32_t REG_5C8;
    __IO uint32_t REG_5CC;
    __IO uint32_t REG_5D0;
    __IO uint32_t REG_5D4;
    __IO uint32_t REG_5D8;
    __IO uint32_t REG_5DC;
    __IO uint32_t REG_5E0;
    __IO uint32_t REG_5E4;
    __IO uint32_t REG_5E8;
    __IO uint32_t REG_5EC;
    __IO uint32_t REG_5F0;
    __IO uint32_t REG_5F4;
    __IO uint32_t REG_5F8;
    __IO uint32_t REG_5FC;
    __IO uint32_t REG_600;
    __IO uint32_t REG_604;
    __IO uint32_t REG_608;
    __IO uint32_t REG_60C;
    __IO uint32_t REG_610;
    __IO uint32_t REG_614;
    __IO uint32_t REG_618;
    __IO uint32_t REG_61C;
    __IO uint32_t REG_620;
    __IO uint32_t REG_624;
    __IO uint32_t REG_628;
    __IO uint32_t REG_62C;
    __IO uint32_t REG_630;
    __IO uint32_t REG_634;
    __IO uint32_t REG_638;
    __IO uint32_t REG_63C;
    __IO uint32_t REG_640;
    __IO uint32_t REG_644;
    __IO uint32_t REG_648;
    __IO uint32_t REG_64C;
    __IO uint32_t REG_650;
    __IO uint32_t REG_654;
    __IO uint32_t REG_658;
    __IO uint32_t REG_65C;
    __IO uint32_t REG_660;
    __IO uint32_t REG_664;
    __IO uint32_t REG_668;
    __IO uint32_t REG_66C;
    __IO uint32_t REG_670;
    __IO uint32_t REG_674;
    __IO uint32_t REG_678;
    __IO uint32_t REG_67C;
    __IO uint32_t REG_680;
    __IO uint32_t REG_684;
    __IO uint32_t REG_688;
    __IO uint32_t REG_68C;
    __IO uint32_t REG_690;
    __IO uint32_t REG_694;
    __IO uint32_t REG_698;
    __IO uint32_t REG_69C;
    __IO uint32_t REG_6A0;
    __IO uint32_t REG_6A4;
    __IO uint32_t REG_6A8;
    __IO uint32_t REG_6AC;
    __IO uint32_t REG_6B0;
    __IO uint32_t REG_6B4;
    __IO uint32_t REG_6B8;
    __IO uint32_t REG_6BC;
    __IO uint32_t REG_6C0;
    __IO uint32_t REG_6C4;
    __IO uint32_t REG_6C8;
    __IO uint32_t REG_6CC;
    __IO uint32_t REG_6D0;
    __IO uint32_t REG_6D4;
    __IO uint32_t REG_6D8;
    __IO uint32_t REG_6DC;
    __IO uint32_t REG_6E0;
    __IO uint32_t REG_6E4;
    __IO uint32_t REG_6E8;
    __IO uint32_t REG_6EC;
    __IO uint32_t REG_6F0;
    __IO uint32_t REG_6F4;
    __IO uint32_t REG_6F8;
    __IO uint32_t REG_6FC;
    __IO uint32_t REG_700;
    __IO uint32_t REG_704;
    __IO uint32_t REG_708;
    __IO uint32_t REG_70C;
    __IO uint32_t REG_710;
    __IO uint32_t REG_714;
    __IO uint32_t REG_718;
    __IO uint32_t REG_71C;
    __IO uint32_t REG_720;
    __IO uint32_t REG_724;
    __IO uint32_t REG_728;
    __IO uint32_t REG_72C;
    __IO uint32_t REG_730;
    __IO uint32_t REG_734;
    __IO uint32_t REG_738;
    __IO uint32_t REG_73C;
    __IO uint32_t REG_740;
    __IO uint32_t REG_744;
    __IO uint32_t REG_748;
    __IO uint32_t REG_74C;
    __IO uint32_t REG_750;
    __IO uint32_t REG_754;
    __IO uint32_t REG_758;
    __IO uint32_t REG_75C;
    __IO uint32_t REG_760;
    __IO uint32_t REG_764;
    __IO uint32_t REG_768;
    __IO uint32_t REG_76C;
    __IO uint32_t REG_770;
    __IO uint32_t REG_774;
    __IO uint32_t REG_778;
    __IO uint32_t REG_77C;
    __IO uint32_t REG_780;
    __IO uint32_t REG_784;
    __IO uint32_t REG_788;
    __IO uint32_t REG_78C;
    __IO uint32_t REG_790;
    __IO uint32_t REG_794;
    __IO uint32_t REG_798;
    __IO uint32_t REG_79C;
    __IO uint32_t REG_7A0;
    __IO uint32_t REG_7A4;
    __IO uint32_t REG_7A8;
    __IO uint32_t REG_7AC;
    __IO uint32_t REG_7B0;
    __IO uint32_t REG_7B4;
    __IO uint32_t REG_7B8;
    __IO uint32_t REG_7BC;
    __IO uint32_t REG_7C0;
    __IO uint32_t REG_7C4;
    __IO uint32_t REG_7C8;
    __IO uint32_t REG_7CC;
    __IO uint32_t REG_7D0;
    __IO uint32_t REG_7D4;
    __IO uint32_t REG_7D8;
    __IO uint32_t REG_7DC;
    __IO uint32_t REG_7E0;
    __IO uint32_t REG_7E4;
    __IO uint32_t REG_7E8;
    __IO uint32_t REG_7EC;
    __IO uint32_t REG_7F0;
    __IO uint32_t REG_7F4;
    __IO uint32_t REG_7F8;
};

// reg_000
#define CODEC_CODEC_IF_EN                                   (1 << 0)
#define CODEC_ADC_ENABLE                                    (1 << 1)
#define CODEC_ADC_ENABLE_CH0                                (1 << 2)
#define CODEC_ADC_ENABLE_CH1                                (1 << 3)
#define CODEC_ADC_ENABLE_CH2                                (1 << 4)
#define CODEC_ADC_ENABLE_CH3                                (1 << 5)
#define CODEC_ADC_ENABLE_CH4                                (1 << 6)
#define CODEC_ADC_ENABLE_CH5                                (1 << 7)
#define CODEC_DAC_ENABLE                                    (1 << 8)
#define CODEC_DMACTRL_RX                                    (1 << 9)
#define CODEC_DMACTRL_TX                                    (1 << 10)

// reg_004
#define CODEC_RX_FIFO_FLUSH_CH0                             (1 << 0)
#define CODEC_RX_FIFO_FLUSH_CH1                             (1 << 1)
#define CODEC_RX_FIFO_FLUSH_CH2                             (1 << 2)
#define CODEC_RX_FIFO_FLUSH_CH3                             (1 << 3)
#define CODEC_RX_FIFO_FLUSH_CH4                             (1 << 4)
#define CODEC_RX_FIFO_FLUSH_CH5                             (1 << 5)
#define CODEC_TX_FIFO_FLUSH                                 (1 << 6)
#define CODEC_DSD_RX_FIFO_FLUSH                             (1 << 7)
#define CODEC_DSD_TX_FIFO_FLUSH                             (1 << 8)
#define CODEC_MC_FIFO_FLUSH                                 (1 << 9)

// reg_008
#define CODEC_CODEC_RX_THRESHOLD(n)                         (((n) & 0xF) << 0)
#define CODEC_CODEC_RX_THRESHOLD_MASK                       (0xF << 0)
#define CODEC_CODEC_RX_THRESHOLD_SHIFT                      (0)
#define CODEC_CODEC_TX_THRESHOLD(n)                         (((n) & 0xF) << 4)
#define CODEC_CODEC_TX_THRESHOLD_MASK                       (0xF << 4)
#define CODEC_CODEC_TX_THRESHOLD_SHIFT                      (4)
#define CODEC_DSD_RX_THRESHOLD(n)                           (((n) & 0xF) << 8)
#define CODEC_DSD_RX_THRESHOLD_MASK                         (0xF << 8)
#define CODEC_DSD_RX_THRESHOLD_SHIFT                        (8)
#define CODEC_DSD_TX_THRESHOLD(n)                           (((n) & 0x7) << 12)
#define CODEC_DSD_TX_THRESHOLD_MASK                         (0x7 << 12)
#define CODEC_DSD_TX_THRESHOLD_SHIFT                        (12)
#define CODEC_MC_THRESHOLD(n)                               (((n) & 0xF) << 15)
#define CODEC_MC_THRESHOLD_MASK                             (0xF << 15)
#define CODEC_MC_THRESHOLD_SHIFT                            (15)

// reg_00c
#define CODEC_CODEC_RX_OVERFLOW(n)                          (((n) & 0xFF) << 0)
#define CODEC_CODEC_RX_OVERFLOW_MASK                        (0xFF << 0)
#define CODEC_CODEC_RX_OVERFLOW_SHIFT                       (0)
#define CODEC_CODEC_RX_UNDERFLOW(n)                         (((n) & 0xFF) << 8)
#define CODEC_CODEC_RX_UNDERFLOW_MASK                       (0xFF << 8)
#define CODEC_CODEC_RX_UNDERFLOW_SHIFT                      (8)
#define CODEC_CODEC_TX_OVERFLOW                             (1 << 16)
#define CODEC_CODEC_TX_UNDERFLOW                            (1 << 17)
#define CODEC_DSD_RX_OVERFLOW                               (1 << 18)
#define CODEC_DSD_RX_UNDERFLOW                              (1 << 19)
#define CODEC_DSD_TX_OVERFLOW                               (1 << 20)
#define CODEC_DSD_TX_UNDERFLOW                              (1 << 21)
#define CODEC_MC_OVERFLOW                                   (1 << 22)
#define CODEC_MC_UNDERFLOW                                  (1 << 23)
#define CODEC_EVENT_TRIGGER                                 (1 << 24)
#define CODEC_FB_CHECK_ERROR_TRIG_CH0                       (1 << 25)
#define CODEC_FB_CHECK_ERROR_TRIG_CH1                       (1 << 26)
#define CODEC_ADC_MAX_OVERFLOW                              (1 << 27)
#define CODEC_TIME_TRIGGER                                  (1 << 28)

// reg_010
#define CODEC_CODEC_RX_OVERFLOW_MSK(n)                     (((n) & 0xFF) << 0)
#define CODEC_CODEC_RX_OVERFLOW_MSK_MASK                   (0xFF << 0)
#define CODEC_CODEC_RX_OVERFLOW_MSK_SHIFT                  (0)
#define CODEC_CODEC_RX_UNDERFLOW_MSK(n)                    (((n) & 0xFF) << 8)
#define CODEC_CODEC_RX_UNDERFLOW_MSK_MASK                  (0xFF << 8)
#define CODEC_CODEC_RX_UNDERFLOW_MSK_SHIFT                 (8)
#define CODEC_CODEC_TX_OVERFLOW_MASK                        (1 << 16)
#define CODEC_CODEC_TX_UNDERFLOW_MASK                       (1 << 17)
#define CODEC_DSD_RX_OVERFLOW_MASK                          (1 << 18)
#define CODEC_DSD_RX_UNDERFLOW_MASK                         (1 << 19)
#define CODEC_DSD_TX_OVERFLOW_MASK                          (1 << 20)
#define CODEC_DSD_TX_UNDERFLOW_MASK                         (1 << 21)
#define CODEC_MC_OVERFLOW_MASK                              (1 << 22)
#define CODEC_MC_UNDERFLOW_MASK                             (1 << 23)
#define CODEC_EVENT_TRIGGER_MASK                            (1 << 24)
#define CODEC_FB_CHECK_ERROR_TRIG_CH0_MASK                  (1 << 25)
#define CODEC_FB_CHECK_ERROR_TRIG_CH1_MASK                  (1 << 26)
#define CODEC_ADC_MAX_OVERFLOW_MASK                         (1 << 27)
#define CODEC_TIME_TRIGGER_MASK                             (1 << 28)

// reg_014
#define CODEC_FIFO_COUNT_CH0(n)                             (((n) & 0xF) << 0)
#define CODEC_FIFO_COUNT_CH0_MASK                           (0xF << 0)
#define CODEC_FIFO_COUNT_CH0_SHIFT                          (0)
#define CODEC_FIFO_COUNT_CH1(n)                             (((n) & 0xF) << 4)
#define CODEC_FIFO_COUNT_CH1_MASK                           (0xF << 4)
#define CODEC_FIFO_COUNT_CH1_SHIFT                          (4)
#define CODEC_FIFO_COUNT_CH2(n)                             (((n) & 0xF) << 8)
#define CODEC_FIFO_COUNT_CH2_MASK                           (0xF << 8)
#define CODEC_FIFO_COUNT_CH2_SHIFT                          (8)
#define CODEC_FIFO_COUNT_CH3(n)                             (((n) & 0xF) << 12)
#define CODEC_FIFO_COUNT_CH3_MASK                           (0xF << 12)
#define CODEC_FIFO_COUNT_CH3_SHIFT                          (12)
#define CODEC_FIFO_COUNT_CH4(n)                             (((n) & 0xF) << 16)
#define CODEC_FIFO_COUNT_CH4_MASK                           (0xF << 16)
#define CODEC_FIFO_COUNT_CH4_SHIFT                          (16)
#define CODEC_FIFO_COUNT_CH5(n)                             (((n) & 0xF) << 20)
#define CODEC_FIFO_COUNT_CH5_MASK                           (0xF << 20)
#define CODEC_FIFO_COUNT_CH5_SHIFT                          (20)

// reg_018
#define CODEC_FIFO_COUNT_TX(n)                              (((n) & 0xF) << 0)
#define CODEC_FIFO_COUNT_TX_MASK                            (0xF << 0)
#define CODEC_FIFO_COUNT_TX_SHIFT                           (0)
#define CODEC_STATE_RX_CH(n)                                (((n) & 0x1FF) << 4)
#define CODEC_STATE_RX_CH_MASK                              (0x1FF << 4)
#define CODEC_STATE_RX_CH_SHIFT                             (4)
#define CODEC_FIFO_COUNT_TX_DSD(n)                          (((n) & 0x7) << 13)
#define CODEC_FIFO_COUNT_TX_DSD_MASK                        (0x7 << 13)
#define CODEC_FIFO_COUNT_TX_DSD_SHIFT                       (13)
#define CODEC_MC_FIFO_COUNT(n)                              (((n) & 0xF) << 16)
#define CODEC_MC_FIFO_COUNT_MASK                            (0xF << 16)
#define CODEC_MC_FIFO_COUNT_SHIFT                           (16)
#define CODEC_FIFO_COUNT_RX_DSD(n)                          (((n) & 0xF) << 20)
#define CODEC_FIFO_COUNT_RX_DSD_MASK                        (0xF << 20)
#define CODEC_FIFO_COUNT_RX_DSD_SHIFT                       (20)

// reg_01c
#define CODEC_RX_FIFO_DATA(n)                               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_MASK                             (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SHIFT                            (0)

// reg_020
#define CODEC_RX_FIFO_DATA(n)                               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_MASK                             (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SHIFT                            (0)

// reg_024
#define CODEC_RX_FIFO_DATA(n)                               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_MASK                             (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SHIFT                            (0)

// reg_028
#define CODEC_RX_FIFO_DATA(n)                               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_MASK                             (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SHIFT                            (0)

// reg_02c
#define CODEC_RX_FIFO_DATA(n)                               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_MASK                             (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SHIFT                            (0)

// reg_030
#define CODEC_RX_FIFO_DATA(n)                               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_MASK                             (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SHIFT                            (0)

// reg_034
#define CODEC_RX_FIFO_DATA(n)                               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_MASK                             (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SHIFT                            (0)

// reg_038
#define CODEC_RX_FIFO_DATA_DSD(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_DSD_MASK                         (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_DSD_SHIFT                        (0)

// reg_03c
#define CODEC_MC_FIFO_DATA(n)                               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_MC_FIFO_DATA_MASK                             (0xFFFFFFFF << 0)
#define CODEC_MC_FIFO_DATA_SHIFT                            (0)

// reg_040
#define CODEC_MODE_16BIT_ADC                                (1 << 0)
#define CODEC_MODE_24BIT_ADC                                (1 << 1)
#define CODEC_MODE_32BIT_ADC                                (1 << 2)
#define CODEC_PLAYTIME_STAMP_MASK                           (1 << 3)
#define CODEC_PLAYTIME_STAMP1_MASK                          (1 << 4)
#define CODEC_PLAYTIME_STAMP2_MASK                          (1 << 5)
#define CODEC_PLAYTIME_STAMP3_MASK                          (1 << 6)
#define CODEC_RME_RF                                        (1 << 7)
#define CODEC_RM_RF(n)                                      (((n) & 0xF) << 8)
#define CODEC_RM_RF_MASK                                    (0xF << 8)
#define CODEC_RM_RF_SHIFT                                   (8)
#define CODEC_RA_RF(n)                                      (((n) & 0x3) << 12)
#define CODEC_RA_RF_MASK                                    (0x3 << 12)
#define CODEC_RA_RF_SHIFT                                   (12)
#define CODEC_WA_RF(n)                                      (((n) & 0x7) << 14)
#define CODEC_WA_RF_MASK                                    (0x7 << 14)
#define CODEC_WA_RF_SHIFT                                   (14)
#define CODEC_WPULSE_RF(n)                                  (((n) & 0x7) << 17)
#define CODEC_WPULSE_RF_MASK                                (0x7 << 17)
#define CODEC_WPULSE_RF_SHIFT                               (17)
#define CODEC_TEST1_RF                                      (1 << 20)
#define CODEC_TEST_RNM_RF                                   (1 << 21)
#define CODEC_BC1_RF                                        (1 << 22)
#define CODEC_BC2_RF                                        (1 << 23)
#define CODEC_RTSEL_RF(n)                                   (((n) & 0x3) << 24)
#define CODEC_RTSEL_RF_MASK                                 (0x3 << 24)
#define CODEC_RTSEL_RF_SHIFT                                (24)
#define CODEC_WTSEL_RF(n)                                   (((n) & 0x3) << 26)
#define CODEC_WTSEL_RF_MASK                                 (0x3 << 26)
#define CODEC_WTSEL_RF_SHIFT                                (26)
#define CODEC_RTSEL_SRAM(n)                                 (((n) & 0x3) << 28)
#define CODEC_RTSEL_SRAM_MASK                               (0x3 << 28)
#define CODEC_RTSEL_SRAM_SHIFT                              (28)
#define CODEC_WTSEL_SRAM(n)                                 (((n) & 0x3) << 30)
#define CODEC_WTSEL_SRAM_MASK                               (0x3 << 30)
#define CODEC_WTSEL_SRAM_SHIFT                              (30)

// reg_044
#define CODEC_DUAL_CHANNEL_DAC                              (1 << 0)
#define CODEC_DAC_EXCHANGE_L_R                              (1 << 1)
#define CODEC_MODE_16BIT_DAC                                (1 << 2)
#define CODEC_MODE_32BIT_DAC                                (1 << 3)
#define CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL(n)      (((n) & 0x3) << 4)
#define CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_MASK    (0x3 << 4)
#define CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_SHIFT   (4)
#define CODEC_CODEC_RESAMPLE_ADC_UPDATE_TRIGGER_SEL(n)      (((n) & 0x3) << 6)
#define CODEC_CODEC_RESAMPLE_ADC_UPDATE_TRIGGER_SEL_MASK    (0x3 << 6)
#define CODEC_CODEC_RESAMPLE_ADC_UPDATE_TRIGGER_SEL_SHIFT   (6)
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE_TRIGGER_SEL(n)      (((n) & 0x3) << 8)
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE_TRIGGER_SEL_MASK    (0x3 << 8)
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE_TRIGGER_SEL_SHIFT   (8)
#define CODEC_CODEC_RESAMPLE_ADC_ENABLE_TRIGGER_SEL(n)      (((n) & 0x3) << 10)
#define CODEC_CODEC_RESAMPLE_ADC_ENABLE_TRIGGER_SEL_MASK    (0x3 << 10)
#define CODEC_CODEC_RESAMPLE_ADC_ENABLE_TRIGGER_SEL_SHIFT   (10)
#define CODEC_CODEC_DAC_GAIN_TRIGGER_SEL(n)                 (((n) & 0x3) << 12)
#define CODEC_CODEC_DAC_GAIN_TRIGGER_SEL_MASK               (0x3 << 12)
#define CODEC_CODEC_DAC_GAIN_TRIGGER_SEL_SHIFT              (12)
#define CODEC_RME_SRAM                                      (1 << 14)
#define CODEC_RM_SRAM(n)                                    (((n) & 0xF) << 15)
#define CODEC_RM_SRAM_MASK                                  (0xF << 15)
#define CODEC_RM_SRAM_SHIFT                                 (15)
#define CODEC_RA_SRAM(n)                                    (((n) & 0x3) << 19)
#define CODEC_RA_SRAM_MASK                                  (0x3 << 19)
#define CODEC_RA_SRAM_SHIFT                                 (19)
#define CODEC_WA_SRAM(n)                                    (((n) & 0x7) << 21)
#define CODEC_WA_SRAM_MASK                                  (0x7 << 21)
#define CODEC_WA_SRAM_SHIFT                                 (21)
#define CODEC_WPULSE_SRAM(n)                                (((n) & 0x7) << 24)
#define CODEC_WPULSE_SRAM_MASK                              (0x7 << 24)
#define CODEC_WPULSE_SRAM_SHIFT                             (24)
#define CODEC_TEST1_SRAM                                    (1 << 27)
#define CODEC_TEST_RNM_SRAM                                 (1 << 28)
#define CODEC_BC1_SRAM                                      (1 << 29)
#define CODEC_BC2_SRAM                                      (1 << 30)
#define CODEC_TM_ROM                                        (1 << 31)

// reg_048
#define CODEC_DSD_IF_EN                                     (1 << 0)
#define CODEC_DSD_ENABLE                                    (1 << 1)
#define CODEC_DSD_DUAL_CHANNEL                              (1 << 2)
#define CODEC_DSD_MSB_FIRST                                 (1 << 3)
#define CODEC_MODE_24BIT_DSD                                (1 << 4)
#define CODEC_MODE_32BIT_DSD                                (1 << 5)
#define CODEC_DMACTRL_RX_DSD                                (1 << 6)
#define CODEC_DMACTRL_TX_DSD                                (1 << 7)
#define CODEC_MODE_16BIT_DSD                                (1 << 8)
#define CODEC_DSD_IN_16BIT                                  (1 << 9)
#define CODEC_TOG_TRIG0_MASK                                (1 << 10)
#define CODEC_TOG_TRIG1_MASK                                (1 << 11)
#define CODEC_TOG_TRIG2_MASK                                (1 << 12)
#define CODEC_TOG_TRIG3_MASK                                (1 << 13)
#define CODEC_RESAMPLE_DAC_ENABLE_PLAYTIME_STAMP_SEL(n)     (((n) & 0x7) << 14)
#define CODEC_RESAMPLE_DAC_ENABLE_PLAYTIME_STAMP_SEL_MASK   (0x7 << 14)
#define CODEC_RESAMPLE_DAC_ENABLE_PLAYTIME_STAMP_SEL_SHIFT  (14)
#define CODEC_RESAMPLE_ADC_ENABLE_PLAYTIME_STAMP_SEL(n)     (((n) & 0x7) << 17)
#define CODEC_RESAMPLE_ADC_ENABLE_PLAYTIME_STAMP_SEL_MASK   (0x7 << 17)
#define CODEC_RESAMPLE_ADC_ENABLE_PLAYTIME_STAMP_SEL_SHIFT  (17)
#define CODEC_RESAMPLE_DAC_PHASE_PLAYTIME_STAMP_SEL(n)      (((n) & 0x7) << 20)
#define CODEC_RESAMPLE_DAC_PHASE_PLAYTIME_STAMP_SEL_MASK    (0x7 << 20)
#define CODEC_RESAMPLE_DAC_PHASE_PLAYTIME_STAMP_SEL_SHIFT   (20)
#define CODEC_RESAMPLE_ADC_PHASE_PLAYTIME_STAMP_SEL(n)      (((n) & 0x7) << 23)
#define CODEC_RESAMPLE_ADC_PHASE_PLAYTIME_STAMP_SEL_MASK    (0x7 << 23)
#define CODEC_RESAMPLE_ADC_PHASE_PLAYTIME_STAMP_SEL_SHIFT   (23)
#define CODEC_CODEC_DAC_GAIN_PLAYTIME_STAMP_SEL(n)          (((n) & 0x7) << 26)
#define CODEC_CODEC_DAC_GAIN_PLAYTIME_STAMP_SEL_MASK        (0x7 << 26)
#define CODEC_CODEC_DAC_GAIN_PLAYTIME_STAMP_SEL_SHIFT       (26)

// reg_04c
#define CODEC_MC_ENABLE                                     (1 << 0)
#define CODEC_DUAL_CHANNEL_MC                               (1 << 1)
#define CODEC_MODE_16BIT_MC                                 (1 << 2)
#define CODEC_DMACTRL_MC                                    (1 << 3)
#define CODEC_MC_DELAY(n)                                   (((n) & 0xFF) << 4)
#define CODEC_MC_DELAY_MASK                                 (0xFF << 4)
#define CODEC_MC_DELAY_SHIFT                                (4)
#define CODEC_MC_RATE_SEL                                   (1 << 12)
#define CODEC_MODE_32BIT_MC                                 (1 << 13)
#define CODEC_MC_EN_SEL                                     (1 << 14)
#define CODEC_MC_RATE_SRC_SEL                               (1 << 15)
#define CODEC_MC_CH_SEL                                     (1 << 16)

// reg_050
#define CODEC_CODEC_COUNT_KEEP(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_COUNT_KEEP_MASK                         (0xFFFFFFFF << 0)
#define CODEC_CODEC_COUNT_KEEP_SHIFT                        (0)

// reg_054
#define CODEC_DAC_ENABLE_SEL(n)                             (((n) & 0x3) << 0)
#define CODEC_DAC_ENABLE_SEL_MASK                           (0x3 << 0)
#define CODEC_DAC_ENABLE_SEL_SHIFT                          (0)
#define CODEC_ADC_ENABLE_SEL(n)                             (((n) & 0x3) << 2)
#define CODEC_ADC_ENABLE_SEL_MASK                           (0x3 << 2)
#define CODEC_ADC_ENABLE_SEL_SHIFT                          (2)
#define CODEC_CODEC_DAC_ENABLE_SEL(n)                       (((n) & 0x3) << 4)
#define CODEC_CODEC_DAC_ENABLE_SEL_MASK                     (0x3 << 4)
#define CODEC_CODEC_DAC_ENABLE_SEL_SHIFT                    (4)
#define CODEC_CODEC_ADC_ENABLE_SEL(n)                       (((n) & 0x3) << 6)
#define CODEC_CODEC_ADC_ENABLE_SEL_MASK                     (0x3 << 6)
#define CODEC_CODEC_ADC_ENABLE_SEL_SHIFT                    (6)
#define CODEC_CODEC_ADC_UH_ENABLE_SEL(n)                    (((n) & 0x3) << 8)
#define CODEC_CODEC_ADC_UH_ENABLE_SEL_MASK                  (0x3 << 8)
#define CODEC_CODEC_ADC_UH_ENABLE_SEL_SHIFT                 (8)
#define CODEC_CODEC_ADC_LH_ENABLE_SEL(n)                    (((n) & 0x3) << 10)
#define CODEC_CODEC_ADC_LH_ENABLE_SEL_MASK                  (0x3 << 10)
#define CODEC_CODEC_ADC_LH_ENABLE_SEL_SHIFT                 (10)
#define CODEC_CODEC_DAC_UH_ENABLE_SEL(n)                    (((n) & 0x3) << 12)
#define CODEC_CODEC_DAC_UH_ENABLE_SEL_MASK                  (0x3 << 12)
#define CODEC_CODEC_DAC_UH_ENABLE_SEL_SHIFT                 (12)
#define CODEC_CODEC_DAC_LH_ENABLE_SEL(n)                    (((n) & 0x3) << 14)
#define CODEC_CODEC_DAC_LH_ENABLE_SEL_MASK                  (0x3 << 14)
#define CODEC_CODEC_DAC_LH_ENABLE_SEL_SHIFT                 (14)
#define CODEC_GPIO_TRIGGER_DB_ENABLE                        (1 << 16)
#define CODEC_STAMP_CLR_USED                                (1 << 17)
#define CODEC_EVENT_SEL                                     (1 << 18)
#define CODEC_EVENT_FOR_CAPTURE                             (1 << 19)
#define CODEC_TEST_PORT_SEL(n)                              (((n) & 0x7) << 20)
#define CODEC_TEST_PORT_SEL_MASK                            (0x7 << 20)
#define CODEC_TEST_PORT_SEL_SHIFT                           (20)
#define CODEC_PLL_OSC_TRIGGER_SEL(n)                        (((n) & 0x3) << 23)
#define CODEC_PLL_OSC_TRIGGER_SEL_MASK                      (0x3 << 23)
#define CODEC_PLL_OSC_TRIGGER_SEL_SHIFT                     (23)
#define CODEC_FAULT_MUTE_DAC_ENABLE                         (1 << 25)
#define CODEC_MODE_HCLK_ACCESS_REG                          (1 << 26)

// reg_058
#define CODEC_RX_FIFO_DATA(n)                               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_MASK                             (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SHIFT                            (0)

// reg_05c
#define CODEC_RX_FIFO_DATA(n)                               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_MASK                             (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SHIFT                            (0)

// reg_060
#define CODEC_EN_CLK_ADC_ANA(n)                             (((n) & 0x3F) << 0)
#define CODEC_EN_CLK_ADC_ANA_MASK                           (0x3F << 0)
#define CODEC_EN_CLK_ADC_ANA_SHIFT                          (0)
#define CODEC_EN_CLK_ADC(n)                                 (((n) & 0x1FF) << 6)
#define CODEC_EN_CLK_ADC_MASK                               (0x1FF << 6)
#define CODEC_EN_CLK_ADC_SHIFT                              (6)
#define CODEC_EN_CLK_DAC                                    (1 << 15)
#define CODEC_EN_CLK_RSADC(n)                               (((n) & 0x3) << 16)
#define CODEC_EN_CLK_RSADC_MASK                             (0x3 << 16)
#define CODEC_EN_CLK_RSADC_SHIFT                            (16)
#define CODEC_EN_CLK_RSDAC(n)                               (((n) & 0x7) << 18)
#define CODEC_EN_CLK_RSDAC_MASK                             (0x7 << 18)
#define CODEC_EN_CLK_RSDAC_SHIFT                            (18)
#define CODEC_EN_CLK_ADC_DSF(n)                             (((n) & 0x3) << 21)
#define CODEC_EN_CLK_ADC_DSF_MASK                           (0x3 << 21)
#define CODEC_EN_CLK_ADC_DSF_SHIFT                          (21)
#define CODEC_EN_CLK_DAC_USF(n)                             (((n) & 0x7) << 23)
#define CODEC_EN_CLK_DAC_USF_MASK                           (0x7 << 23)
#define CODEC_EN_CLK_DAC_USF_SHIFT                          (23)
#define CODEC_POL_ADC_ANA(n)                                (((n) & 0x3F) << 26)
#define CODEC_POL_ADC_ANA_MASK                              (0x3F << 26)
#define CODEC_POL_ADC_ANA_SHIFT                             (26)

// reg_064
#define CODEC_SOFT_RSTN_ADC_ANA(n)                          (((n) & 0x3F) << 0)
#define CODEC_SOFT_RSTN_ADC_ANA_MASK                        (0x3F << 0)
#define CODEC_SOFT_RSTN_ADC_ANA_SHIFT                       (0)
#define CODEC_SOFT_RSTN_ADC(n)                              (((n) & 0x1FF) << 6)
#define CODEC_SOFT_RSTN_ADC_MASK                            (0x1FF << 6)
#define CODEC_SOFT_RSTN_ADC_SHIFT                           (6)
#define CODEC_SOFT_RSTN_DAC                                 (1 << 15)
#define CODEC_SOFT_RSTN_RS_ADC(n)                           (((n) & 0x3) << 16)
#define CODEC_SOFT_RSTN_RS_ADC_MASK                         (0x3 << 16)
#define CODEC_SOFT_RSTN_RS_ADC_SHIFT                        (16)
#define CODEC_SOFT_RSTN_RS_DAC(n)                           (((n) & 0x7) << 18)
#define CODEC_SOFT_RSTN_RS_DAC_MASK                         (0x7 << 18)
#define CODEC_SOFT_RSTN_RS_DAC_SHIFT                        (18)
#define CODEC_SOFT_RSTN_ADC_DSF(n)                          (((n) & 0x3) << 21)
#define CODEC_SOFT_RSTN_ADC_DSF_MASK                        (0x3 << 21)
#define CODEC_SOFT_RSTN_ADC_DSF_SHIFT                       (21)
#define CODEC_SOFT_RSTN_DAC_USF(n)                          (((n) & 0x7) << 23)
#define CODEC_SOFT_RSTN_DAC_USF_MASK                        (0x7 << 23)
#define CODEC_SOFT_RSTN_DAC_USF_SHIFT                       (23)
#define CODEC_POL_DAC_OUT                                   (1 << 26)
#define CODEC_POL_CLK_CODEC                                 (1 << 27)

// reg_068
#define CODEC_SLP_RF(n)                                     (((n) & 0x7F) << 0)
#define CODEC_SLP_RF_MASK                                   (0x7F << 0)
#define CODEC_SLP_RF_SHIFT                                  (0)
#define CODEC_DSLP_RF(n)                                    (((n) & 0x7F) << 7)
#define CODEC_DSLP_RF_MASK                                  (0x7F << 7)
#define CODEC_DSLP_RF_SHIFT                                 (7)
#define CODEC_SD_RF(n)                                      (((n) & 0x7F) << 14)
#define CODEC_SD_RF_MASK                                    (0x7F << 14)
#define CODEC_SD_RF_SHIFT                                   (14)
#define CODEC_SLP_SRAM(n)                                   (((n) & 0x3) << 21)
#define CODEC_SLP_SRAM_MASK                                 (0x3 << 21)
#define CODEC_SLP_SRAM_SHIFT                                (21)
#define CODEC_DSLP_SRAM(n)                                  (((n) & 0x3) << 23)
#define CODEC_DSLP_SRAM_MASK                                (0x3 << 23)
#define CODEC_DSLP_SRAM_SHIFT                               (23)
#define CODEC_SD_SRAM(n)                                    (((n) & 0x3) << 25)
#define CODEC_SD_SRAM_MASK                                  (0x3 << 25)
#define CODEC_SD_SRAM_SHIFT                                 (25)
#define CODEC_PGEN_ROM_0                                    (1 << 27)
#define CODEC_PGEN_ROM_1                                    (1 << 28)

// reg_06c
#define CODEC_RTSEL_ROM(n)                                  (((n) & 0x3) << 0)
#define CODEC_RTSEL_ROM_MASK                                (0x3 << 0)
#define CODEC_RTSEL_ROM_SHIFT                               (0)
#define CODEC_PTSEL_ROM(n)                                  (((n) & 0x3) << 2)
#define CODEC_PTSEL_ROM_MASK                                (0x3 << 2)
#define CODEC_PTSEL_ROM_SHIFT                               (2)
#define CODEC_TRB_ROM(n)                                    (((n) & 0x3) << 4)
#define CODEC_TRB_ROM_MASK                                  (0x3 << 4)
#define CODEC_TRB_ROM_SHIFT                                 (4)
#define CODEC_EN_CLK_IIR_ANC(n)                             (((n) & 0xFF) << 6)
#define CODEC_EN_CLK_IIR_ANC_MASK                           (0xFF << 6)
#define CODEC_EN_CLK_IIR_ANC_SHIFT                          (6)
#define CODEC_EN_CLK_IIR_EQ                                 (1 << 14)
#define CODEC_EN_CLK_FIR(n)                                 (((n) & 0xF) << 15)
#define CODEC_EN_CLK_FIR_MASK                               (0xF << 15)
#define CODEC_EN_CLK_FIR_SHIFT                              (15)
#define CODEC_EN_CLK_PSAP(n)                                (((n) & 0x3) << 19)
#define CODEC_EN_CLK_PSAP_MASK                              (0x3 << 19)
#define CODEC_EN_CLK_PSAP_SHIFT                             (19)
#define CODEC_CFG_DIV_CODEC_EQIIR(n)                        (((n) & 0x3) << 21)
#define CODEC_CFG_DIV_CODEC_EQIIR_MASK                      (0x3 << 21)
#define CODEC_CFG_DIV_CODEC_EQIIR_SHIFT                     (21)
#define CODEC_BYPASS_DIV_CODEC_EQIIR                        (1 << 23)
#define CODEC_SEL_OSCX4_EQIIR                               (1 << 24)
#define CODEC_SEL_OSCX2_EQIIR                               (1 << 25)
#define CODEC_SEL_OSC_EQIIR                                 (1 << 26)
#define CODEC_SEL_OSC_PSAP                                  (1 << 27)
#define CODEC_SEL_OSC_4_EQIIR                               (1 << 28)
#define CODEC_SEL_OSC_2_EQIIR                               (1 << 29)

// reg_070
#define CODEC_TRIG_TIME(n)                                  (((n) & 0x1FFFF) << 0)
#define CODEC_TRIG_TIME_MASK                                (0x1FFFF << 0)
#define CODEC_TRIG_TIME_SHIFT                               (0)
#define CODEC_TRIG_TIME_ENABLE                              (1 << 17)
#define CODEC_TRIG_MODE                                     (1 << 18)
#define CODEC_GET_CNT_TRIG                                  (1 << 19)
#define CODEC_CFG_DIV_CODEC(n)                              (((n) & 0xF) << 20)
#define CODEC_CFG_DIV_CODEC_MASK                            (0xF << 20)
#define CODEC_CFG_DIV_CODEC_SHIFT                           (20)
#define CODEC_BYPASS_DIV_CODEC                              (1 << 24)
#define CODEC_SEL_PLL_CODEC                                 (1 << 25)
#define CODEC_CFG_DIV_CODEC_ADC_DSF(n)                      (((n) & 0x3) << 26)
#define CODEC_CFG_DIV_CODEC_ADC_DSF_MASK                    (0x3 << 26)
#define CODEC_CFG_DIV_CODEC_ADC_DSF_SHIFT                   (26)
#define CODEC_BYPASS_DIV_CODEC_ADC_DSF                      (1 << 28)
#define CODEC_SEL_OSCX2_ADC_DSF                             (1 << 29)
#define CODEC_SEL_OSC_ADC_DSF                               (1 << 30)
#define CODEC_SEL_OSC_2_ADC_DSF                             (1 << 31)

// reg_074
#define CODEC_SEL_PLL_AUD(n)                                (((n) & 0x3) << 0)
#define CODEC_SEL_PLL_AUD_MASK                              (0x3 << 0)
#define CODEC_SEL_PLL_AUD_SHIFT                             (0)
#define CODEC_CFG_DIV_CODEC_FIR(n)                          (((n) & 0x3) << 2)
#define CODEC_CFG_DIV_CODEC_FIR_MASK                        (0x3 << 2)
#define CODEC_CFG_DIV_CODEC_FIR_SHIFT                       (2)
#define CODEC_CFG_DIV_CODEC_RSADC(n)                        (((n) & 0x3) << 4)
#define CODEC_CFG_DIV_CODEC_RSADC_MASK                      (0x3 << 4)
#define CODEC_CFG_DIV_CODEC_RSADC_SHIFT                     (4)
#define CODEC_CFG_DIV_CODEC_RSDAC(n)                        (((n) & 0x3) << 6)
#define CODEC_CFG_DIV_CODEC_RSDAC_MASK                      (0x3 << 6)
#define CODEC_CFG_DIV_CODEC_RSDAC_SHIFT                     (6)
#define CODEC_CFG_DIV_CODEC_ANCIIR(n)                       (((n) & 0x3) << 8)
#define CODEC_CFG_DIV_CODEC_ANCIIR_MASK                     (0x3 << 8)
#define CODEC_CFG_DIV_CODEC_ANCIIR_SHIFT                    (8)
#define CODEC_BYPASS_DIV_CODEC_FIR                          (1 << 10)
#define CODEC_SEL_OSCX4_FIR                                 (1 << 11)
#define CODEC_SEL_OSCX2_FIR                                 (1 << 12)
#define CODEC_SEL_OSC_FIR                                   (1 << 13)
#define CODEC_SEL_HCLK_FIR                                  (1 << 14)
#define CODEC_BYPASS_DIV_CODEC_RSADC                        (1 << 15)
#define CODEC_SEL_OSCX4_RSADC                               (1 << 16)
#define CODEC_SEL_OSCX2_RSADC                               (1 << 17)
#define CODEC_SEL_OSC_RSADC                                 (1 << 18)
#define CODEC_BYPASS_DIV_CODEC_RSDAC                        (1 << 19)
#define CODEC_SEL_OSCX4_RSDAC                               (1 << 20)
#define CODEC_SEL_OSCX2_RSDAC                               (1 << 21)
#define CODEC_SEL_OSC_RSDAC                                 (1 << 22)
#define CODEC_BYPASS_DIV_CODEC_ANCIIR                       (1 << 23)
#define CODEC_SEL_OSCX4_ANCIIR                              (1 << 24)
#define CODEC_SEL_OSCX2_ANCIIR                              (1 << 25)
#define CODEC_SEL_OSC_ANCIIR                                (1 << 26)
#define CODEC_SEL_OSC_CODEC                                 (1 << 27)
#define CODEC_SEL_CLK_PLL_CODEC                             (1 << 28)
#define CODEC_SEL_PLL_CODECHCLK                             (1 << 29)
#define CODEC_SEL_HCLK_OSC                                  (1 << 30)
#define CODEC_SEL_CODEC_HCLK_SENS                           (1 << 31)

// reg_078
#define CODEC_ADC_ENABLE_PLAYTIME_STAMP_SEL(n)              (((n) & 0x7) << 0)
#define CODEC_ADC_ENABLE_PLAYTIME_STAMP_SEL_MASK            (0x7 << 0)
#define CODEC_ADC_ENABLE_PLAYTIME_STAMP_SEL_SHIFT           (0)
#define CODEC_DAC_ENABLE_PLAYTIME_STAMP_SEL(n)              (((n) & 0x7) << 3)
#define CODEC_DAC_ENABLE_PLAYTIME_STAMP_SEL_MASK            (0x7 << 3)
#define CODEC_DAC_ENABLE_PLAYTIME_STAMP_SEL_SHIFT           (3)
#define CODEC_CODEC_ADC_ENABLE_PLAYTIME_STAMP_SEL(n)        (((n) & 0x7) << 6)
#define CODEC_CODEC_ADC_ENABLE_PLAYTIME_STAMP_SEL_MASK      (0x7 << 6)
#define CODEC_CODEC_ADC_ENABLE_PLAYTIME_STAMP_SEL_SHIFT     (6)
#define CODEC_CODEC_DAC_ENABLE_PLAYTIME_STAMP_SEL(n)        (((n) & 0x7) << 9)
#define CODEC_CODEC_DAC_ENABLE_PLAYTIME_STAMP_SEL_MASK      (0x7 << 9)
#define CODEC_CODEC_DAC_ENABLE_PLAYTIME_STAMP_SEL_SHIFT     (9)
#define CODEC_CODEC_ADC_UH_ENABLE_PLAYTIME_STAMP_SEL(n)     (((n) & 0x7) << 12)
#define CODEC_CODEC_ADC_UH_ENABLE_PLAYTIME_STAMP_SEL_MASK   (0x7 << 12)
#define CODEC_CODEC_ADC_UH_ENABLE_PLAYTIME_STAMP_SEL_SHIFT  (12)
#define CODEC_CODEC_ADC_LH_ENABLE_PLAYTIME_STAMP_SEL(n)     (((n) & 0x7) << 15)
#define CODEC_CODEC_ADC_LH_ENABLE_PLAYTIME_STAMP_SEL_MASK   (0x7 << 15)
#define CODEC_CODEC_ADC_LH_ENABLE_PLAYTIME_STAMP_SEL_SHIFT  (15)
#define CODEC_CODEC_DAC_UH_ENABLE_PLAYTIME_STAMP_SEL(n)     (((n) & 0x7) << 18)
#define CODEC_CODEC_DAC_UH_ENABLE_PLAYTIME_STAMP_SEL_MASK   (0x7 << 18)
#define CODEC_CODEC_DAC_UH_ENABLE_PLAYTIME_STAMP_SEL_SHIFT  (18)
#define CODEC_CODEC_DAC_LH_ENABLE_PLAYTIME_STAMP_SEL(n)     (((n) & 0x7) << 21)
#define CODEC_CODEC_DAC_LH_ENABLE_PLAYTIME_STAMP_SEL_MASK   (0x7 << 21)
#define CODEC_CODEC_DAC_LH_ENABLE_PLAYTIME_STAMP_SEL_SHIFT  (21)
#define CODEC_PLL_OSC_TRIGGER_PLAYTIME_STAMP_SEL(n)         (((n) & 0x7) << 24)
#define CODEC_PLL_OSC_TRIGGER_PLAYTIME_STAMP_SEL_MASK       (0x7 << 24)
#define CODEC_PLL_OSC_TRIGGER_PLAYTIME_STAMP_SEL_SHIFT      (24)
#define CODEC_TRIGGER_EVENT_PLAYTIME_STAMP_SEL(n)           (((n) & 0x7) << 27)
#define CODEC_TRIGGER_EVENT_PLAYTIME_STAMP_SEL_MASK         (0x7 << 27)
#define CODEC_TRIGGER_EVENT_PLAYTIME_STAMP_SEL_SHIFT        (27)

// reg_07c
#define CODEC_SOFT_RSTN_IIR_ANC(n)                          (((n) & 0xFF) << 0)
#define CODEC_SOFT_RSTN_IIR_ANC_MASK                        (0xFF << 0)
#define CODEC_SOFT_RSTN_IIR_ANC_SHIFT                       (0)
#define CODEC_SOFT_RSTN_IIR_EQ                              (1 << 8)
#define CODEC_SOFT_RSTN_FIR(n)                              (((n) & 0xF) << 9)
#define CODEC_SOFT_RSTN_FIR_MASK                            (0xF << 9)
#define CODEC_SOFT_RSTN_FIR_SHIFT                           (9)
#define CODEC_SOFT_RSTN_PSAP(n)                             (((n) & 0x3) << 13)
#define CODEC_SOFT_RSTN_PSAP_MASK                           (0x3 << 13)
#define CODEC_SOFT_RSTN_PSAP_SHIFT                          (13)
#define CODEC_SEL_I2S_MCLK(n)                               (((n) & 0xF) << 15)
#define CODEC_SEL_I2S_MCLK_MASK                             (0xF << 15)
#define CODEC_SEL_I2S_MCLK_SHIFT                            (15)
#define CODEC_EN_I2S_MCLK                                   (1 << 19)
#define CODEC_CFG_CLK_OUT(n)                                (((n) & 0xF) << 20)
#define CODEC_CFG_CLK_OUT_MASK                              (0xF << 20)
#define CODEC_CFG_CLK_OUT_SHIFT                             (20)
#define CODEC_CODEC_CLKX2_EN                                (1 << 24)
#define CODEC_CFG_DIV_CODEC_DAC_USF(n)                      (((n) & 0x3) << 25)
#define CODEC_CFG_DIV_CODEC_DAC_USF_MASK                    (0x3 << 25)
#define CODEC_CFG_DIV_CODEC_DAC_USF_SHIFT                   (25)
#define CODEC_BYPASS_DIV_CODEC_DAC_USF                      (1 << 27)
#define CODEC_SEL_OSCX2_DAC_USF                             (1 << 28)
#define CODEC_SEL_OSC_DAC_USF                               (1 << 29)
#define CODEC_SEL_OSC_2_DAC_USF                             (1 << 30)

// reg_080
#define CODEC_CODEC_ADC_EN                                  (1 << 0)
#define CODEC_CODEC_ADC_EN_CH0                              (1 << 1)
#define CODEC_CODEC_ADC_EN_CH1                              (1 << 2)
#define CODEC_CODEC_ADC_EN_CH2                              (1 << 3)
#define CODEC_CODEC_ADC_EN_CH3                              (1 << 4)
#define CODEC_CODEC_EC_EN_CH0                               (1 << 5)
#define CODEC_CODEC_EC_EN_CH1                               (1 << 6)
#define CODEC_CODEC_SIDE_TONE_GAIN(n)                       (((n) & 0x1F) << 7)
#define CODEC_CODEC_SIDE_TONE_GAIN_MASK                     (0x1F << 7)
#define CODEC_CODEC_SIDE_TONE_GAIN_SHIFT                    (7)
#define CODEC_CODEC_SIDE_TONE_MIC_SEL(n)                    (((n) & 0x7) << 12)
#define CODEC_CODEC_SIDE_TONE_MIC_SEL_MASK                  (0x7 << 12)
#define CODEC_CODEC_SIDE_TONE_MIC_SEL_SHIFT                 (12)
#define CODEC_CODEC_SIDE_TONE_IIR_ENABLE                    (1 << 15)
#define CODEC_CODEC_ADC_LOOP                                (1 << 16)
#define CODEC_CODEC_LOOP_SEL_L(n)                           (((n) & 0x7) << 17)
#define CODEC_CODEC_LOOP_SEL_L_MASK                         (0x7 << 17)
#define CODEC_CODEC_LOOP_SEL_L_SHIFT                        (17)
#define CODEC_CODEC_LOOP_SEL_R(n)                           (((n) & 0x7) << 20)
#define CODEC_CODEC_LOOP_SEL_R_MASK                         (0x7 << 20)
#define CODEC_CODEC_LOOP_SEL_R_SHIFT                        (20)
#define CODEC_CODEC_ADC_UH_EN                               (1 << 23)
#define CODEC_CODEC_ADC_LH_EN                               (1 << 24)
#define CODEC_CODEC_ADDA_ALIGN_MODE                         (1 << 25)

// reg_084
#define CODEC_CODEC_ADC_SIGNED_CH0                          (1 << 0)
#define CODEC_CODEC_ADC_IN_SEL_CH0(n)                       (((n) & 0x7) << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH0_MASK                     (0x7 << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH0_SHIFT                    (1)
#define CODEC_CODEC_ADC_DOWN_SEL_CH0(n)                     (((n) & 0x3) << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH0_MASK                   (0x3 << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH0_SHIFT                  (4)
#define CODEC_CODEC_ADC_HBF3_BYPASS_CH0                     (1 << 6)
#define CODEC_CODEC_ADC_HBF2_BYPASS_CH0                     (1 << 7)
#define CODEC_CODEC_ADC_HBF1_BYPASS_CH0                     (1 << 8)
#define CODEC_CODEC_ADC_GAIN_SEL_CH0                        (1 << 9)
#define CODEC_CODEC_ADC_GAIN_CH0(n)                         (((n) & 0xFFFFF) << 10)
#define CODEC_CODEC_ADC_GAIN_CH0_MASK                       (0xFFFFF << 10)
#define CODEC_CODEC_ADC_GAIN_CH0_SHIFT                      (10)
#define CODEC_CODEC_ADC_HBF3_SEL_CH0(n)                     (((n) & 0x3) << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH0_MASK                   (0x3 << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH0_SHIFT                  (30)

// reg_088
#define CODEC_CODEC_ADC_SIGNED_CH1                          (1 << 0)
#define CODEC_CODEC_ADC_IN_SEL_CH1(n)                       (((n) & 0x7) << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH1_MASK                     (0x7 << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH1_SHIFT                    (1)
#define CODEC_CODEC_ADC_DOWN_SEL_CH1(n)                     (((n) & 0x3) << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH1_MASK                   (0x3 << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH1_SHIFT                  (4)
#define CODEC_CODEC_ADC_HBF3_BYPASS_CH1                     (1 << 6)
#define CODEC_CODEC_ADC_HBF2_BYPASS_CH1                     (1 << 7)
#define CODEC_CODEC_ADC_HBF1_BYPASS_CH1                     (1 << 8)
#define CODEC_CODEC_ADC_GAIN_SEL_CH1                        (1 << 9)
#define CODEC_CODEC_ADC_GAIN_CH1(n)                         (((n) & 0xFFFFF) << 10)
#define CODEC_CODEC_ADC_GAIN_CH1_MASK                       (0xFFFFF << 10)
#define CODEC_CODEC_ADC_GAIN_CH1_SHIFT                      (10)
#define CODEC_CODEC_ADC_HBF3_SEL_CH1(n)                     (((n) & 0x3) << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH1_MASK                   (0x3 << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH1_SHIFT                  (30)

// reg_08c
#define CODEC_CODEC_ADC_SIGNED_CH2                          (1 << 0)
#define CODEC_CODEC_ADC_IN_SEL_CH2(n)                       (((n) & 0x7) << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH2_MASK                     (0x7 << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH2_SHIFT                    (1)
#define CODEC_CODEC_ADC_DOWN_SEL_CH2(n)                     (((n) & 0x3) << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH2_MASK                   (0x3 << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH2_SHIFT                  (4)
#define CODEC_CODEC_ADC_HBF3_BYPASS_CH2                     (1 << 6)
#define CODEC_CODEC_ADC_HBF2_BYPASS_CH2                     (1 << 7)
#define CODEC_CODEC_ADC_HBF1_BYPASS_CH2                     (1 << 8)
#define CODEC_CODEC_ADC_GAIN_SEL_CH2                        (1 << 9)
#define CODEC_CODEC_ADC_GAIN_CH2(n)                         (((n) & 0xFFFFF) << 10)
#define CODEC_CODEC_ADC_GAIN_CH2_MASK                       (0xFFFFF << 10)
#define CODEC_CODEC_ADC_GAIN_CH2_SHIFT                      (10)
#define CODEC_CODEC_ADC_HBF3_SEL_CH2(n)                     (((n) & 0x3) << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH2_MASK                   (0x3 << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH2_SHIFT                  (30)

// reg_090
#define CODEC_CODEC_ADC_SIGNED_CH3                          (1 << 0)
#define CODEC_CODEC_ADC_IN_SEL_CH3(n)                       (((n) & 0x7) << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH3_MASK                     (0x7 << 1)
#define CODEC_CODEC_ADC_IN_SEL_CH3_SHIFT                    (1)
#define CODEC_CODEC_ADC_DOWN_SEL_CH3(n)                     (((n) & 0x3) << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH3_MASK                   (0x3 << 4)
#define CODEC_CODEC_ADC_DOWN_SEL_CH3_SHIFT                  (4)
#define CODEC_CODEC_ADC_HBF3_BYPASS_CH3                     (1 << 6)
#define CODEC_CODEC_ADC_HBF2_BYPASS_CH3                     (1 << 7)
#define CODEC_CODEC_ADC_HBF1_BYPASS_CH3                     (1 << 8)
#define CODEC_CODEC_ADC_GAIN_SEL_CH3                        (1 << 9)
#define CODEC_CODEC_ADC_GAIN_CH3(n)                         (((n) & 0xFFFFF) << 10)
#define CODEC_CODEC_ADC_GAIN_CH3_MASK                       (0xFFFFF << 10)
#define CODEC_CODEC_ADC_GAIN_CH3_SHIFT                      (10)
#define CODEC_CODEC_ADC_HBF3_SEL_CH3(n)                     (((n) & 0x3) << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH3_MASK                   (0x3 << 30)
#define CODEC_CODEC_ADC_HBF3_SEL_CH3_SHIFT                  (30)

// reg_09c
#define CODEC_CODEC_ECHO_ENABLE_CH0                         (1 << 0)
#define CODEC_CODEC_ECHO_SEL_CH0(n)                         (((n) & 0x7) << 1)
#define CODEC_CODEC_ECHO_SEL_CH0_MASK                       (0x7 << 1)
#define CODEC_CODEC_ECHO_SEL_CH0_SHIFT                      (1)
#define CODEC_CODEC_EC_DOWN_SEL_CH0(n)                      (((n) & 0x3) << 4)
#define CODEC_CODEC_EC_DOWN_SEL_CH0_MASK                    (0x3 << 4)
#define CODEC_CODEC_EC_DOWN_SEL_CH0_SHIFT                   (4)
#define CODEC_CODEC_EC_HBF3_BYPASS_CH0                      (1 << 6)
#define CODEC_CODEC_EC_HBF2_BYPASS_CH0                      (1 << 7)
#define CODEC_CODEC_EC_HBF1_BYPASS_CH0                      (1 << 8)
#define CODEC_CODEC_EC_GAIN_SEL_CH0                         (1 << 9)
#define CODEC_CODEC_EC_GAIN_CH0(n)                          (((n) & 0xFFFFF) << 10)
#define CODEC_CODEC_EC_GAIN_CH0_MASK                        (0xFFFFF << 10)
#define CODEC_CODEC_EC_GAIN_CH0_SHIFT                       (10)
#define CODEC_CODEC_ECHO_RATE_SEL_CH0(n)                    (((n) & 0x3) << 30)
#define CODEC_CODEC_ECHO_RATE_SEL_CH0_MASK                  (0x3 << 30)
#define CODEC_CODEC_ECHO_RATE_SEL_CH0_SHIFT                 (30)

// reg_0a0
#define CODEC_CODEC_ECHO_ENABLE_CH1                         (1 << 0)
#define CODEC_CODEC_ECHO_SEL_CH1(n)                         (((n) & 0x7) << 1)
#define CODEC_CODEC_ECHO_SEL_CH1_MASK                       (0x7 << 1)
#define CODEC_CODEC_ECHO_SEL_CH1_SHIFT                      (1)
#define CODEC_CODEC_EC_DOWN_SEL_CH1(n)                      (((n) & 0x3) << 4)
#define CODEC_CODEC_EC_DOWN_SEL_CH1_MASK                    (0x3 << 4)
#define CODEC_CODEC_EC_DOWN_SEL_CH1_SHIFT                   (4)
#define CODEC_CODEC_EC_HBF3_BYPASS_CH1                      (1 << 6)
#define CODEC_CODEC_EC_HBF2_BYPASS_CH1                      (1 << 7)
#define CODEC_CODEC_EC_HBF1_BYPASS_CH1                      (1 << 8)
#define CODEC_CODEC_EC_GAIN_SEL_CH1                         (1 << 9)
#define CODEC_CODEC_EC_GAIN_CH1(n)                          (((n) & 0xFFFFF) << 10)
#define CODEC_CODEC_EC_GAIN_CH1_MASK                        (0xFFFFF << 10)
#define CODEC_CODEC_EC_GAIN_CH1_SHIFT                       (10)
#define CODEC_CODEC_ECHO_RATE_SEL_CH1(n)                    (((n) & 0x3) << 30)
#define CODEC_CODEC_ECHO_RATE_SEL_CH1_MASK                  (0x3 << 30)
#define CODEC_CODEC_ECHO_RATE_SEL_CH1_SHIFT                 (30)

// reg_0a4
#define CODEC_CODEC_ADC_SINC1_CAS_CH0(n)                    (((n) & 0x3) << 0)
#define CODEC_CODEC_ADC_SINC1_CAS_CH0_MASK                  (0x3 << 0)
#define CODEC_CODEC_ADC_SINC1_CAS_CH0_SHIFT                 (0)
#define CODEC_CODEC_ADC_SINC2_CAS_CH0(n)                    (((n) & 0x3) << 2)
#define CODEC_CODEC_ADC_SINC2_CAS_CH0_MASK                  (0x3 << 2)
#define CODEC_CODEC_ADC_SINC2_CAS_CH0_SHIFT                 (2)
#define CODEC_CODEC_ADC_SINC3_CAS_CH0(n)                    (((n) & 0x3) << 4)
#define CODEC_CODEC_ADC_SINC3_CAS_CH0_MASK                  (0x3 << 4)
#define CODEC_CODEC_ADC_SINC3_CAS_CH0_SHIFT                 (4)
#define CODEC_CODEC_ADC_SINC1_CAS_CH1(n)                    (((n) & 0x3) << 6)
#define CODEC_CODEC_ADC_SINC1_CAS_CH1_MASK                  (0x3 << 6)
#define CODEC_CODEC_ADC_SINC1_CAS_CH1_SHIFT                 (6)
#define CODEC_CODEC_ADC_SINC2_CAS_CH1(n)                    (((n) & 0x3) << 8)
#define CODEC_CODEC_ADC_SINC2_CAS_CH1_MASK                  (0x3 << 8)
#define CODEC_CODEC_ADC_SINC2_CAS_CH1_SHIFT                 (8)
#define CODEC_CODEC_ADC_SINC3_CAS_CH1(n)                    (((n) & 0x3) << 10)
#define CODEC_CODEC_ADC_SINC3_CAS_CH1_MASK                  (0x3 << 10)
#define CODEC_CODEC_ADC_SINC3_CAS_CH1_SHIFT                 (10)
#define CODEC_CODEC_ADC_SINC1_CAS_CH2(n)                    (((n) & 0x3) << 12)
#define CODEC_CODEC_ADC_SINC1_CAS_CH2_MASK                  (0x3 << 12)
#define CODEC_CODEC_ADC_SINC1_CAS_CH2_SHIFT                 (12)
#define CODEC_CODEC_ADC_SINC2_CAS_CH2(n)                    (((n) & 0x3) << 14)
#define CODEC_CODEC_ADC_SINC2_CAS_CH2_MASK                  (0x3 << 14)
#define CODEC_CODEC_ADC_SINC2_CAS_CH2_SHIFT                 (14)
#define CODEC_CODEC_ADC_SINC3_CAS_CH2(n)                    (((n) & 0x3) << 16)
#define CODEC_CODEC_ADC_SINC3_CAS_CH2_MASK                  (0x3 << 16)
#define CODEC_CODEC_ADC_SINC3_CAS_CH2_SHIFT                 (16)
#define CODEC_CODEC_ADC_SINC1_CAS_CH3(n)                    (((n) & 0x3) << 18)
#define CODEC_CODEC_ADC_SINC1_CAS_CH3_MASK                  (0x3 << 18)
#define CODEC_CODEC_ADC_SINC1_CAS_CH3_SHIFT                 (18)
#define CODEC_CODEC_ADC_SINC2_CAS_CH3(n)                    (((n) & 0x3) << 20)
#define CODEC_CODEC_ADC_SINC2_CAS_CH3_MASK                  (0x3 << 20)
#define CODEC_CODEC_ADC_SINC2_CAS_CH3_SHIFT                 (20)
#define CODEC_CODEC_ADC_SINC3_CAS_CH3(n)                    (((n) & 0x3) << 22)
#define CODEC_CODEC_ADC_SINC3_CAS_CH3_MASK                  (0x3 << 22)
#define CODEC_CODEC_ADC_SINC3_CAS_CH3_SHIFT                 (22)
#define CODEC_CODEC_ADC_SINC2_MODE_CH0                      (1 << 24)
#define CODEC_CODEC_ADC_SINC2_MODE_CH1                      (1 << 25)
#define CODEC_CODEC_ADC_SINC2_MODE_CH2                      (1 << 26)
#define CODEC_CODEC_ADC_SINC2_MODE_CH3                      (1 << 27)

// reg_0ac
#define CODEC_CODEC_ADC_FIFO_THRESHOLD(n)                   (((n) & 0xF) << 0)
#define CODEC_CODEC_ADC_FIFO_THRESHOLD_MASK                 (0xF << 0)
#define CODEC_CODEC_ADC_FIFO_THRESHOLD_SHIFT                (0)
#define CODEC_CODEC_ADC_FIFO_FLUSH_CH0                      (1 << 4)
#define CODEC_CODEC_ADC_FIFO_FLUSH_CH1                      (1 << 5)
#define CODEC_CODEC_ADC_FIFO_FLUSH_CH2                      (1 << 6)
#define CODEC_CODEC_ECHO_SND_ENABLE_CH0                     (1 << 7)
#define CODEC_CODEC_ECHO_SND_SEL_CH0(n)                     (((n) & 0x7) << 8)
#define CODEC_CODEC_ECHO_SND_SEL_CH0_MASK                   (0x7 << 8)
#define CODEC_CODEC_ECHO_SND_SEL_CH0_SHIFT                  (8)
#define CODEC_CODEC_ECHO_SND_RATE_SEL_CH0(n)                (((n) & 0x3) << 11)
#define CODEC_CODEC_ECHO_SND_RATE_SEL_CH0_MASK              (0x3 << 11)
#define CODEC_CODEC_ECHO_SND_RATE_SEL_CH0_SHIFT             (11)
#define CODEC_CODEC_ECHO_SND_ENABLE_CH1                     (1 << 13)
#define CODEC_CODEC_ECHO_SND_SEL_CH1(n)                     (((n) & 0x7) << 14)
#define CODEC_CODEC_ECHO_SND_SEL_CH1_MASK                   (0x7 << 14)
#define CODEC_CODEC_ECHO_SND_SEL_CH1_SHIFT                  (14)
#define CODEC_CODEC_ECHO_SND_RATE_SEL_CH1(n)                (((n) & 0x3) << 17)
#define CODEC_CODEC_ECHO_SND_RATE_SEL_CH1_MASK              (0x3 << 17)
#define CODEC_CODEC_ECHO_SND_RATE_SEL_CH1_SHIFT             (17)

// reg_0b0
#define CODEC_CODEC_DAC_EN                                  (1 << 0)
#define CODEC_CODEC_DAC_EN_CH0                              (1 << 1)
#define CODEC_CODEC_DAC_EN_CH1                              (1 << 2)
#define CODEC_CODEC_DAC_DITHER_GAIN(n)                      (((n) & 0x1F) << 3)
#define CODEC_CODEC_DAC_DITHER_GAIN_MASK                    (0x1F << 3)
#define CODEC_CODEC_DAC_DITHER_GAIN_SHIFT                   (3)
#define CODEC_CODEC_DAC_SDM_GAIN(n)                         (((n) & 0x7) << 8)
#define CODEC_CODEC_DAC_SDM_GAIN_MASK                       (0x7 << 8)
#define CODEC_CODEC_DAC_SDM_GAIN_SHIFT                      (8)
#define CODEC_CODEC_DITHER_BYPASS                           (1 << 11)
#define CODEC_CODEC_DAC_HBF3_BYPASS                         (1 << 12)
#define CODEC_CODEC_DAC_HBF2_BYPASS                         (1 << 13)
#define CODEC_CODEC_DAC_HBF1_BYPASS                         (1 << 14)
#define CODEC_CODEC_DAC_UP_SEL(n)                           (((n) & 0x7) << 15)
#define CODEC_CODEC_DAC_UP_SEL_MASK                         (0x7 << 15)
#define CODEC_CODEC_DAC_UP_SEL_SHIFT                        (15)
#define CODEC_CODEC_DAC_TONE_TEST                           (1 << 18)
#define CODEC_CODEC_DAC_SIN1K_STEP(n)                       (((n) & 0xF) << 19)
#define CODEC_CODEC_DAC_SIN1K_STEP_MASK                     (0xF << 19)
#define CODEC_CODEC_DAC_SIN1K_STEP_SHIFT                    (19)
#define CODEC_CODEC_DAC_OSR_SEL(n)                          (((n) & 0x3) << 23)
#define CODEC_CODEC_DAC_OSR_SEL_MASK                        (0x3 << 23)
#define CODEC_CODEC_DAC_OSR_SEL_SHIFT                       (23)
#define CODEC_CODEC_DAC_LR_SWAP                             (1 << 25)
#define CODEC_CODEC_DAC_SDM_H4_6M_CH0                       (1 << 26)
#define CODEC_CODEC_DAC_SDM_H4_6M_CH1                       (1 << 27)
#define CODEC_CODEC_DAC_L_FIR_UPSAMPLE                      (1 << 28)
#define CODEC_CODEC_DAC_R_FIR_UPSAMPLE                      (1 << 29)
#define CODEC_CODEC_DAC_USE_HBF4                            (1 << 30)
#define CODEC_CODEC_DAC_USE_HBF5                            (1 << 31)

// reg_0b4
#define CODEC_CODEC_DAC_GAIN_CH0(n)                         (((n) & 0xFFFFF) << 0)
#define CODEC_CODEC_DAC_GAIN_CH0_MASK                       (0xFFFFF << 0)
#define CODEC_CODEC_DAC_GAIN_CH0_SHIFT                      (0)
#define CODEC_CODEC_DAC_GAIN_SEL_CH0                        (1 << 20)
#define CODEC_CODEC_DAC_GAIN_UPDATE                         (1 << 21)
#define CODEC_CODEC_ADC_GAIN_UPDATE_CH0                     (1 << 22)
#define CODEC_CODEC_ADC_GAIN_UPDATE_CH1                     (1 << 23)
#define CODEC_CODEC_ADC_GAIN_UPDATE_CH2                     (1 << 24)
#define CODEC_CODEC_ADC_GAIN_UPDATE_CH3                     (1 << 25)
#define CODEC_CODEC_EC_GAIN_UPDATE_CH0                      (1 << 26)
#define CODEC_CODEC_EC_GAIN_UPDATE_CH1                      (1 << 27)
#define CODEC_CODEC_DAC_SINC_CAS_CH0                        (1 << 28)
#define CODEC_CODEC_DAC_SINC_CAS_CH1                        (1 << 29)
#define CODEC_CODEC_DAC_SINC_MODE_CH0                       (1 << 30) //TODO
#define CODEC_CODEC_DAC_SINC_MODE_CH1                       (1 << 31) //TODO

// reg_0b8
#define CODEC_CODEC_DAC_GAIN_CH1(n)                         (((n) & 0xFFFFF) << 0)
#define CODEC_CODEC_DAC_GAIN_CH1_MASK                       (0xFFFFF << 0)
#define CODEC_CODEC_DAC_GAIN_CH1_SHIFT                      (0)
#define CODEC_CODEC_DAC_GAIN_SEL_CH1                        (1 << 20)
#define CODEC_CODEC_DAC_OUT_SWAP                            (1 << 21)
#define CODEC_CODEC_DAC_H4_DELAY_CH0(n)                     (((n) & 0x3) << 22)
#define CODEC_CODEC_DAC_H4_DELAY_CH0_MASK                   (0x3 << 22)
#define CODEC_CODEC_DAC_H4_DELAY_CH0_SHIFT                  (22)
#define CODEC_CODEC_DAC_L4_DELAY_CH0(n)                     (((n) & 0x3) << 24)
#define CODEC_CODEC_DAC_L4_DELAY_CH0_MASK                   (0x3 << 24)
#define CODEC_CODEC_DAC_L4_DELAY_CH0_SHIFT                  (24)
#define CODEC_CODEC_DAC_H4_DELAY_CH1(n)                     (((n) & 0x3) << 26)
#define CODEC_CODEC_DAC_H4_DELAY_CH1_MASK                   (0x3 << 26)
#define CODEC_CODEC_DAC_H4_DELAY_CH1_SHIFT                  (26)
#define CODEC_CODEC_DAC_L4_DELAY_CH1(n)                     (((n) & 0x3) << 28)
#define CODEC_CODEC_DAC_L4_DELAY_CH1_MASK                   (0x3 << 28)
#define CODEC_CODEC_DAC_L4_DELAY_CH1_SHIFT                  (28)
#define CODEC_CODEC_DAC_HBF4_DELAY_SEL                      (1 << 30)

// reg_0bc
#define CODEC_CODEC_DAC_UH_EN                               (1 << 0)
#define CODEC_CODEC_DAC_LH_EN                               (1 << 1)
#define CODEC_CODEC_DAC_SDM_3RD_EN_CH0                      (1 << 2)
#define CODEC_CODEC_DAC_SDM_3RD_EN_CH1                      (1 << 3)
#define CODEC_CODEC_DAC_SDM_CLOSE                           (1 << 4)
#define CODEC_CODEC_DITHERF_BYPASS                          (1 << 5)
#define CODEC_CODEC_DAC_DITHERF_GAIN(n)                     (((n) & 0x3) << 6)
#define CODEC_CODEC_DAC_DITHERF_GAIN_MASK                   (0x3 << 6)
#define CODEC_CODEC_DAC_DITHERF_GAIN_SHIFT                  (6)
#define CODEC_EN_48KX64_MODE(n)                             (((n) & 0x3) << 8)
#define CODEC_EN_48KX64_MODE_MASK                           (0x3 << 8)
#define CODEC_EN_48KX64_MODE_SHIFT                          (8)
#define CODEC_ADC_DCF_BYPASS_CH0                            (1 << 10)
#define CODEC_ADC_DCF_BYPASS_CH1                            (1 << 11)
#define CODEC_ADC_DCF_BYPASS_CH2                            (1 << 12)
#define CODEC_ADC_DCF_BYPASS_CH3                            (1 << 13)
#define CODEC_ADC_DC_SEL                                    (1 << 14)
#define CODEC_CODEC_DAC_ECHO_DATA_SEL_CH0(n)                (((n) & 0xF) << 15)
#define CODEC_CODEC_DAC_ECHO_DATA_SEL_CH0_MASK              (0xF << 15)
#define CODEC_CODEC_DAC_ECHO_DATA_SEL_CH0_SHIFT             (15)
#define CODEC_CODEC_DAC_ECHO_DATA_SEL_CH1(n)                (((n) & 0xF) << 19)
#define CODEC_CODEC_DAC_ECHO_DATA_SEL_CH1_MASK              (0xF << 19)
#define CODEC_CODEC_DAC_ECHO_DATA_SEL_CH1_SHIFT             (19)
#define CODEC_CODEC_TEST_PORT_SEL(n)                        (((n) & 0x3F) << 23)
#define CODEC_CODEC_TEST_PORT_SEL_MASK                      (0x3F << 23)
#define CODEC_CODEC_TEST_PORT_SEL_SHIFT                     (23)
#define CODEC_CODEC_TONE_TEST_MODE                          (1 << 29)

// reg_0c0
#define CODEC_ADC_UDC_CH0(n)                                (((n) & 0xF) << 0)
#define CODEC_ADC_UDC_CH0_MASK                              (0xF << 0)
#define CODEC_ADC_UDC_CH0_SHIFT                             (0)
#define CODEC_ADC_UDC_CH1(n)                                (((n) & 0xF) << 4)
#define CODEC_ADC_UDC_CH1_MASK                              (0xF << 4)
#define CODEC_ADC_UDC_CH1_SHIFT                             (4)
#define CODEC_ADC_UDC_CH2(n)                                (((n) & 0xF) << 8)
#define CODEC_ADC_UDC_CH2_MASK                              (0xF << 8)
#define CODEC_ADC_UDC_CH2_SHIFT                             (8)
#define CODEC_ADC_UDC_CH3(n)                                (((n) & 0xF) << 12)
#define CODEC_ADC_UDC_CH3_MASK                              (0xF << 12)
#define CODEC_ADC_UDC_CH3_SHIFT                             (12)
#define CODEC_CODEC_DAC_ECHO_DATA_DBG_SEL_CH0               (1 << 16)
#define CODEC_CODEC_DAC_ECHO_DATA_DBG_SEL_CH1               (1 << 17)
#define CODEC_CODEC_DAC_SND_ECHO_KEEP_CH0                   (1 << 18)
#define CODEC_CODEC_DAC_TRI_ECHO_KEEP_CH0                   (1 << 19)
#define CODEC_CODEC_DAC_SND_ECHO_KEEP_CH1                   (1 << 20)
#define CODEC_CODEC_DAC_TRI_ECHO_KEEP_CH1                   (1 << 21)
#define CODEC_CODEC_SDM_DELAY_CH0(n)                        (((n) & 0x7) << 22)
#define CODEC_CODEC_SDM_DELAY_CH0_MASK                      (0x7 << 22)
#define CODEC_CODEC_SDM_DELAY_CH0_SHIFT                     (22)

// reg_0c4
#define CODEC_CODEC_PDM_ENABLE                              (1 << 0)
#define CODEC_CODEC_PDM_DATA_INV                            (1 << 1)
#define CODEC_CODEC_PDM_RATE_SEL_01(n)                      (((n) & 0x3) << 2)
#define CODEC_CODEC_PDM_RATE_SEL_01_MASK                    (0x3 << 2)
#define CODEC_CODEC_PDM_RATE_SEL_01_SHIFT                   (2)
#define CODEC_CODEC_PDM_RATE_SEL_23(n)                      (((n) & 0x3) << 4)
#define CODEC_CODEC_PDM_RATE_SEL_23_MASK                    (0x3 << 4)
#define CODEC_CODEC_PDM_RATE_SEL_23_SHIFT                   (4)
#define CODEC_CODEC_PDM_RATE_SEL_45(n)                      (((n) & 0x3) << 6)
#define CODEC_CODEC_PDM_RATE_SEL_45_MASK                    (0x3 << 6)
#define CODEC_CODEC_PDM_RATE_SEL_45_SHIFT                   (6)
#define CODEC_CODEC_PDM_ADC_SEL_CH0                         (1 << 8)
#define CODEC_CODEC_PDM_ADC_SEL_CH1                         (1 << 9)
#define CODEC_CODEC_PDM_ADC_SEL_CH2                         (1 << 10)
#define CODEC_CODEC_PDM_ADC_SEL_CH3                         (1 << 11)

// reg_0c8
#define CODEC_CODEC_PDM_MUX_CH0(n)                          (((n) & 0x7) << 0)
#define CODEC_CODEC_PDM_MUX_CH0_MASK                        (0x7 << 0)
#define CODEC_CODEC_PDM_MUX_CH0_SHIFT                       (0)
#define CODEC_CODEC_PDM_MUX_CH1(n)                          (((n) & 0x7) << 3)
#define CODEC_CODEC_PDM_MUX_CH1_MASK                        (0x7 << 3)
#define CODEC_CODEC_PDM_MUX_CH1_SHIFT                       (3)
#define CODEC_CODEC_PDM_MUX_CH2(n)                          (((n) & 0x7) << 6)
#define CODEC_CODEC_PDM_MUX_CH2_MASK                        (0x7 << 6)
#define CODEC_CODEC_PDM_MUX_CH2_SHIFT                       (6)
#define CODEC_CODEC_PDM_MUX_CH3(n)                          (((n) & 0x7) << 9)
#define CODEC_CODEC_PDM_MUX_CH3_MASK                        (0x7 << 9)
#define CODEC_CODEC_PDM_MUX_CH3_SHIFT                       (9)
#define CODEC_CODEC_PDM_CAP_PHASE_CH0(n)                    (((n) & 0x3) << 12)
#define CODEC_CODEC_PDM_CAP_PHASE_CH0_MASK                  (0x3 << 12)
#define CODEC_CODEC_PDM_CAP_PHASE_CH0_SHIFT                 (12)
#define CODEC_CODEC_PDM_CAP_PHASE_CH1(n)                    (((n) & 0x3) << 14)
#define CODEC_CODEC_PDM_CAP_PHASE_CH1_MASK                  (0x3 << 14)
#define CODEC_CODEC_PDM_CAP_PHASE_CH1_SHIFT                 (14)
#define CODEC_CODEC_PDM_CAP_PHASE_CH2(n)                    (((n) & 0x3) << 16)
#define CODEC_CODEC_PDM_CAP_PHASE_CH2_MASK                  (0x3 << 16)
#define CODEC_CODEC_PDM_CAP_PHASE_CH2_SHIFT                 (16)
#define CODEC_CODEC_PDM_CAP_PHASE_CH3(n)                    (((n) & 0x3) << 18)
#define CODEC_CODEC_PDM_CAP_PHASE_CH3_MASK                  (0x3 << 18)
#define CODEC_CODEC_PDM_CAP_PHASE_CH3_SHIFT                 (18)
#define CODEC_CODEC_PDM_CAP_PHASE_CH4(n)                    (((n) & 0x3) << 20)
#define CODEC_CODEC_PDM_CAP_PHASE_CH4_MASK                  (0x3 << 20)
#define CODEC_CODEC_PDM_CAP_PHASE_CH4_SHIFT                 (20)
#define CODEC_CODEC_PDM_CAP_PHASE_CH5(n)                    (((n) & 0x3) << 22)
#define CODEC_CODEC_PDM_CAP_PHASE_CH5_MASK                  (0x3 << 22)
#define CODEC_CODEC_PDM_CAP_PHASE_CH5_SHIFT                 (22)

// reg_0cc
#define CODEC_ADC_UDC1_CH0(n)                               (((n) & 0xF) << 0)
#define CODEC_ADC_UDC1_CH0_MASK                             (0xF << 0)
#define CODEC_ADC_UDC1_CH0_SHIFT                            (0)
#define CODEC_ADC_UDC1_CH1(n)                               (((n) & 0xF) << 4)
#define CODEC_ADC_UDC1_CH1_MASK                             (0xF << 4)
#define CODEC_ADC_UDC1_CH1_SHIFT                            (4)
#define CODEC_ADC_UDC1_CH2(n)                               (((n) & 0xF) << 8)
#define CODEC_ADC_UDC1_CH2_MASK                             (0xF << 8)
#define CODEC_ADC_UDC1_CH2_SHIFT                            (8)
#define CODEC_ADC_UDC1_CH3(n)                               (((n) & 0xF) << 12)
#define CODEC_ADC_UDC1_CH3_MASK                             (0xF << 12)
#define CODEC_ADC_UDC1_CH3_SHIFT                            (12)
#define CODEC_ADC_DCF1_BYPASS_CH0                           (1 << 16)
#define CODEC_ADC_DCF1_BYPASS_CH1                           (1 << 17)
#define CODEC_ADC_DCF1_BYPASS_CH2                           (1 << 18)
#define CODEC_ADC_DCF1_BYPASS_CH3                           (1 << 19)

// reg_0d0
#define CODEC_CODEC_ANC_ENABLE_CH0                          (1 << 0)
#define CODEC_CODEC_ANC_ENABLE_CH1                          (1 << 1)
#define CODEC_CODEC_DUAL_ANC_CH0                            (1 << 2)
#define CODEC_CODEC_DUAL_ANC_CH1                            (1 << 3)
#define CODEC_CODEC_ANC_MUTE_CH0                            (1 << 4)
#define CODEC_CODEC_ANC_MUTE_CH1                            (1 << 5)
#define CODEC_CODEC_FF_CH0_FIR_EN                           (1 << 6)
#define CODEC_CODEC_FF_CH1_FIR_EN                           (1 << 7)
#define CODEC_CODEC_FB_CH0_FIR_EN                           (1 << 8)
#define CODEC_CODEC_FB_CH1_FIR_EN                           (1 << 9)
#define CODEC_CODEC_ANC_RATE_SEL                            (1 << 10)
#define CODEC_CODEC_ANC_FF_RATE_SEL(n)                      (((n) & 0x3) << 11)
#define CODEC_CODEC_ANC_FF_RATE_SEL_MASK                    (0x3 << 11)
#define CODEC_CODEC_ANC_FF_RATE_SEL_SHIFT                   (11)
#define CODEC_CODEC_ANC_FF_SR_SEL(n)                        (((n) & 0x3) << 13)
#define CODEC_CODEC_ANC_FF_SR_SEL_MASK                      (0x3 << 13)
#define CODEC_CODEC_ANC_FF_SR_SEL_SHIFT                     (13)
#define CODEC_CODEC_ANC_FF_IN_PHASE_SEL(n)                  (((n) & 0x7) << 15)
#define CODEC_CODEC_ANC_FF_IN_PHASE_SEL_MASK                (0x7 << 15)
#define CODEC_CODEC_ANC_FF_IN_PHASE_SEL_SHIFT               (15)
#define CODEC_CODEC_ANC_FB_SR_SEL(n)                        (((n) & 0x3) << 18)
#define CODEC_CODEC_ANC_FB_SR_SEL_MASK                      (0x3 << 18)
#define CODEC_CODEC_ANC_FB_SR_SEL_SHIFT                     (18)
#define CODEC_CODEC_ANC_FB_IN_PHASE_SEL(n)                  (((n) & 0x7) << 20)
#define CODEC_CODEC_ANC_FB_IN_PHASE_SEL_MASK                (0x7 << 20)
#define CODEC_CODEC_ANC_FB_IN_PHASE_SEL_SHIFT               (20)
#define CODEC_CODEC_FEEDBACK_CH0                            (1 << 23)
#define CODEC_CODEC_FEEDBACK_CH1                            (1 << 24)
#define CODEC_CODEC_TT0_FIR_EN                              (1 << 25)
#define CODEC_CODEC_TT1_FIR_EN                              (1 << 26)
#define CODEC_CODEC_MM0_FIR_EN                              (1 << 27)
#define CODEC_CODEC_MM1_FIR_EN                              (1 << 28)
#define CODEC_CODEC_DEQ_FIR_EN_CH0                          (1 << 29)
#define CODEC_CODEC_DEQ_FIR_EN_CH1                          (1 << 30)

// reg_0d4
#define CODEC_CODEC_ANC_MUTE_GAIN_FF_CH0(n)                 (((n) & 0xFFF) << 0)
#define CODEC_CODEC_ANC_MUTE_GAIN_FF_CH0_MASK               (0xFFF << 0)
#define CODEC_CODEC_ANC_MUTE_GAIN_FF_CH0_SHIFT              (0)
#define CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FF_CH0              (1 << 12)
#define CODEC_CODEC_ANC_MUTE_GAIN_UPDATE_FF_CH0             (1 << 13)
#define CODEC_CODEC_ANC_MUTE_GAIN_FF_CH1(n)                 (((n) & 0xFFF) << 14)
#define CODEC_CODEC_ANC_MUTE_GAIN_FF_CH1_MASK               (0xFFF << 14)
#define CODEC_CODEC_ANC_MUTE_GAIN_FF_CH1_SHIFT              (14)
#define CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FF_CH1              (1 << 26)
#define CODEC_CODEC_ANC_MUTE_GAIN_UPDATE_FF_CH1             (1 << 27)

// reg_0d8
#define CODEC_CODEC_ANC_MUTE_GAIN_FB_CH0(n)                 (((n) & 0xFFF) << 0)
#define CODEC_CODEC_ANC_MUTE_GAIN_FB_CH0_MASK               (0xFFF << 0)
#define CODEC_CODEC_ANC_MUTE_GAIN_FB_CH0_SHIFT              (0)
#define CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FB_CH0              (1 << 12)
#define CODEC_CODEC_ANC_MUTE_GAIN_UPDATE_FB_CH0             (1 << 13)
#define CODEC_CODEC_ANC_MUTE_GAIN_FB_CH1(n)                 (((n) & 0xFFF) << 14)
#define CODEC_CODEC_ANC_MUTE_GAIN_FB_CH1_MASK               (0xFFF << 14)
#define CODEC_CODEC_ANC_MUTE_GAIN_FB_CH1_SHIFT              (14)
#define CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FB_CH1              (1 << 26)
#define CODEC_CODEC_ANC_MUTE_GAIN_UPDATE_FB_CH1             (1 << 27)

// reg_0dc
#define CODEC_CODEC_ADC_MC_EN_CH0                           (1 << 0)
#define CODEC_CODEC_ADC_MC_EN_CH1                           (1 << 1)
#define CODEC_CODEC_FEEDBACK_MC_EN_CH0                      (1 << 2)
#define CODEC_CODEC_FEEDBACK_MC_EN_CH1                      (1 << 3)
#define CODEC_CODEC_DAC_L_EQ_ENABLE                         (1 << 4)
#define CODEC_CODEC_DAC_R_EQ_ENABLE                         (1 << 5)
#define CODEC_CODEC_ADC_CH0_IIR_ENABLE                      (1 << 6)
#define CODEC_CODEC_ADC_CH1_IIR_ENABLE                      (1 << 7)
#define CODEC_CODEC_ADC_CH2_IIR_ENABLE                      (1 << 8)
#define CODEC_CODEC_ADC_CH3_IIR_ENABLE                      (1 << 9)
#define CODEC_CODEC_ADC_CH4_IIR_ENABLE                      (1 << 10)
#define CODEC_CODEC_ADC_CH5_IIR_ENABLE                      (1 << 11)
#define CODEC_CODEC_ADC_CH6_IIR_ENABLE                      (1 << 12)
#define CODEC_CODEC_ADC_CH7_IIR_ENABLE                      (1 << 13)
#define CODEC_CODEC_ADC_FIR_DS_EN_CH0                       (1 << 14)
#define CODEC_CODEC_ADC_FIR_DS_SEL_CH0                      (1 << 15)
#define CODEC_CODEC_ADC_FIR_DS_RES_EN_CH0                   (1 << 16)
#define CODEC_CODEC_ADC_FIR_DS_EN_CH1                       (1 << 17)
#define CODEC_CODEC_ADC_FIR_DS_SEL_CH1                      (1 << 18)
#define CODEC_CODEC_ADC_FIR_DS_RES_EN_CH1                   (1 << 19)
#define CODEC_CODEC_ADC_FIR_DS_EN_CH2                       (1 << 20)
#define CODEC_CODEC_ADC_FIR_DS_SEL_CH2                      (1 << 21)
#define CODEC_CODEC_ADC_FIR_DS_RES_EN_CH2                   (1 << 22)
#define CODEC_CODEC_ADC_FIR_DS_EN_CH3                       (1 << 23)
#define CODEC_CODEC_ADC_FIR_DS_SEL_CH3                      (1 << 24)
#define CODEC_CODEC_ADC_FIR_DS_RES_EN_CH3                   (1 << 25)
#define CODEC_CODEC_FF_DBG_KEEP_CH0                         (1 << 26)
#define CODEC_CODEC_FB_DBG_KEEP_CH0                         (1 << 27)
#define CODEC_CODEC_TT_DBG_KEEP_CH0                         (1 << 28)
#define CODEC_CODEC_FF_DBG_KEEP_CH1                         (1 << 29)
#define CODEC_CODEC_FB_DBG_KEEP_CH1                         (1 << 30)
#define CODEC_CODEC_TT_DBG_KEEP_CH1                         (1 << 31)

// reg_0e0
#define CODEC_CODEC_ADC_IIR_CH0_SEL(n)                      (((n) & 0xF) << 0)
#define CODEC_CODEC_ADC_IIR_CH0_SEL_MASK                    (0xF << 0)
#define CODEC_CODEC_ADC_IIR_CH0_SEL_SHIFT                   (0)
#define CODEC_CODEC_ADC_IIR_CH1_SEL(n)                      (((n) & 0xF) << 4)
#define CODEC_CODEC_ADC_IIR_CH1_SEL_MASK                    (0xF << 4)
#define CODEC_CODEC_ADC_IIR_CH1_SEL_SHIFT                   (4)
#define CODEC_CODEC_ADC_IIR_CH2_SEL(n)                      (((n) & 0xF) << 8)
#define CODEC_CODEC_ADC_IIR_CH2_SEL_MASK                    (0xF << 8)
#define CODEC_CODEC_ADC_IIR_CH2_SEL_SHIFT                   (8)
#define CODEC_CODEC_ADC_IIR_CH3_SEL(n)                      (((n) & 0xF) << 12)
#define CODEC_CODEC_ADC_IIR_CH3_SEL_MASK                    (0xF << 12)
#define CODEC_CODEC_ADC_IIR_CH3_SEL_SHIFT                   (12)
#define CODEC_CODEC_ADC_IIR_CH4_SEL(n)                      (((n) & 0xF) << 16)
#define CODEC_CODEC_ADC_IIR_CH4_SEL_MASK                    (0xF << 16)
#define CODEC_CODEC_ADC_IIR_CH4_SEL_SHIFT                   (16)
#define CODEC_CODEC_ADC_IIR_CH5_SEL(n)                      (((n) & 0xF) << 20)
#define CODEC_CODEC_ADC_IIR_CH5_SEL_MASK                    (0xF << 20)
#define CODEC_CODEC_ADC_IIR_CH5_SEL_SHIFT                   (20)
#define CODEC_CODEC_ADC_IIR_CH6_SEL(n)                      (((n) & 0xF) << 24)
#define CODEC_CODEC_ADC_IIR_CH6_SEL_MASK                    (0xF << 24)
#define CODEC_CODEC_ADC_IIR_CH6_SEL_SHIFT                   (24)
#define CODEC_CODEC_ADC_IIR_CH7_SEL(n)                      (((n) & 0xF) << 28)
#define CODEC_CODEC_ADC_IIR_CH7_SEL_MASK                    (0xF << 28)
#define CODEC_CODEC_ADC_IIR_CH7_SEL_SHIFT                   (28)

// reg_0e4
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE                     (1 << 0)
#define CODEC_CODEC_RESAMPLE_DAC_L_ENABLE                   (1 << 1)
#define CODEC_CODEC_RESAMPLE_DAC_R_ENABLE                   (1 << 2)
#define CODEC_CODEC_RESAMPLE_DAC_FIFO_ENABLE                (1 << 3)
#define CODEC_CODEC_RESAMPLE_ADC_ENABLE                     (1 << 4)
#define CODEC_CODEC_RESAMPLE_ADC_CH_CNT(n)                  (((n) & 0x7) << 5)
#define CODEC_CODEC_RESAMPLE_ADC_CH_CNT_MASK                (0x7 << 5)
#define CODEC_CODEC_RESAMPLE_ADC_CH_CNT_SHIFT               (5)
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE               (1 << 8)
#define CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE               (1 << 9)
#define CODEC_CODEC_ADC_REMAP_ENABLE                        (1 << 10)
#define CODEC_CODEC_RESAMPLE_DAC_FIFO_DELAY(n)              (((n) & 0x7) << 11)
#define CODEC_CODEC_RESAMPLE_DAC_FIFO_DELAY_MASK             (0x7 << 11)
#define CODEC_CODEC_RESAMPLE_DAC_FIFO_DELAY_SHIFT            (11)

// reg_0e8
#define CODEC_CODEC_RESAMPLE_ADC_CH0_SEL(n)                 (((n) & 0xF) << 0)
#define CODEC_CODEC_RESAMPLE_ADC_CH0_SEL_MASK               (0xF << 0)
#define CODEC_CODEC_RESAMPLE_ADC_CH0_SEL_SHIFT              (0)
#define CODEC_CODEC_RESAMPLE_ADC_CH1_SEL(n)                 (((n) & 0xF) << 4)
#define CODEC_CODEC_RESAMPLE_ADC_CH1_SEL_MASK               (0xF << 4)
#define CODEC_CODEC_RESAMPLE_ADC_CH1_SEL_SHIFT              (4)
#define CODEC_CODEC_RESAMPLE_ADC_CH2_SEL(n)                 (((n) & 0xF) << 8)
#define CODEC_CODEC_RESAMPLE_ADC_CH2_SEL_MASK               (0xF << 8)
#define CODEC_CODEC_RESAMPLE_ADC_CH2_SEL_SHIFT              (8)
#define CODEC_CODEC_RESAMPLE_ADC_CH3_SEL(n)                 (((n) & 0xF) << 12)
#define CODEC_CODEC_RESAMPLE_ADC_CH3_SEL_MASK               (0xF << 12)
#define CODEC_CODEC_RESAMPLE_ADC_CH3_SEL_SHIFT              (12)
#define CODEC_CODEC_RESAMPLE_ADC_CH4_SEL(n)                 (((n) & 0xF) << 16)
#define CODEC_CODEC_RESAMPLE_ADC_CH4_SEL_MASK               (0xF << 16)
#define CODEC_CODEC_RESAMPLE_ADC_CH4_SEL_SHIFT              (16)
#define CODEC_CODEC_RESAMPLE_ADC_CH5_SEL(n)                 (((n) & 0xF) << 20)
#define CODEC_CODEC_RESAMPLE_ADC_CH5_SEL_MASK               (0xF << 20)
#define CODEC_CODEC_RESAMPLE_ADC_CH5_SEL_SHIFT              (20)

// reg_0ec
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_INC(n)               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_INC_MASK             (0xFFFFFFFF << 0)
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_INC_SHIFT            (0)

// reg_0f0
#define CODEC_CODEC_RESAMPLE_ADC_PHASE_INC(n)               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_RESAMPLE_ADC_PHASE_INC_MASK             (0xFFFFFFFF << 0)
#define CODEC_CODEC_RESAMPLE_ADC_PHASE_INC_SHIFT            (0)

// reg_100
#define CODEC_FIR_STREAM_ENABLE_CH0                         (1 << 0)
#define CODEC_FIR_STREAM_ENABLE_CH1                         (1 << 1)
#define CODEC_FIR_STREAM_ENABLE_CH2                         (1 << 2)
#define CODEC_FIR_STREAM_ENABLE_CH3                         (1 << 3)
#define CODEC_FIR_ENABLE_CH0                                (1 << 4)
#define CODEC_FIR_ENABLE_CH1                                (1 << 5)
#define CODEC_FIR_ENABLE_CH2                                (1 << 6)
#define CODEC_FIR_ENABLE_CH3                                (1 << 7)
#define CODEC_DMACTRL_RX_FIR                                (1 << 8)
#define CODEC_DMACTRL_TX_FIR                                (1 << 9)
#define CODEC_FIR_UPSAMPLE_CH0                              (1 << 10)
#define CODEC_FIR_UPSAMPLE_CH1                              (1 << 11)
#define CODEC_FIR_UPSAMPLE_CH2                              (1 << 12)
#define CODEC_FIR_UPSAMPLE_CH3                              (1 << 13)
#define CODEC_MODE_32BIT_FIR                                (1 << 14)
#define CODEC_FIR_RESERVED_REG0                             (1 << 15)
#define CODEC_MODE_16BIT_FIR_TX_CH0                         (1 << 16)
#define CODEC_MODE_16BIT_FIR_RX_CH0                         (1 << 17)
#define CODEC_MODE_16BIT_FIR_TX_CH1                         (1 << 18)
#define CODEC_MODE_16BIT_FIR_RX_CH1                         (1 << 19)
#define CODEC_MODE_16BIT_FIR_TX_CH2                         (1 << 20)
#define CODEC_MODE_16BIT_FIR_RX_CH2                         (1 << 21)
#define CODEC_MODE_16BIT_FIR_TX_CH3                         (1 << 22)
#define CODEC_MODE_16BIT_FIR_RX_CH3                         (1 << 23)
#define CODEC_FIR_FULL_OUT_CH0                              (1 << 24)
#define CODEC_FIR_FULL_OUT_CH1                              (1 << 25)
#define CODEC_FIR_FULL_OUT_CH2                              (1 << 26)
#define CODEC_FIR_FULL_OUT_CH3                              (1 << 27)

// reg_104
#define CODEC_FIR_ACCESS_OFFSET_CH0(n)                      (((n) & 0x7) << 0)
#define CODEC_FIR_ACCESS_OFFSET_CH0_MASK                    (0x7 << 0)
#define CODEC_FIR_ACCESS_OFFSET_CH0_SHIFT                   (0)
#define CODEC_FIR_ACCESS_OFFSET_CH1(n)                      (((n) & 0x7) << 3)
#define CODEC_FIR_ACCESS_OFFSET_CH1_MASK                    (0x7 << 3)
#define CODEC_FIR_ACCESS_OFFSET_CH1_SHIFT                   (3)
#define CODEC_FIR_ACCESS_OFFSET_CH2(n)                      (((n) & 0x7) << 6)
#define CODEC_FIR_ACCESS_OFFSET_CH2_MASK                    (0x7 << 6)
#define CODEC_FIR_ACCESS_OFFSET_CH2_SHIFT                   (6)
#define CODEC_FIR_ACCESS_OFFSET_CH3(n)                      (((n) & 0x7) << 9)
#define CODEC_FIR_ACCESS_OFFSET_CH3_MASK                    (0x7 << 9)
#define CODEC_FIR_ACCESS_OFFSET_CH3_SHIFT                   (9)
#define CODEC_PDU_FS_SWAP                                   (1 << 12)
#define CODEC_ANC_COEF_SEL_PDU0_PDU1                        (1 << 13)
#define CODEC_ANC_COEF_SEL_FS0_FS1                          (1 << 14)
#define CODEC_ANC_COEF_SEL_PDU0_FS0                         (1 << 15)
#define CODEC_ANC_COEF_SEL_PDU1_FS1                         (1 << 16)
#define CODEC_ANC_COEF_SEL_PDU0_PDU1_NEW                    (1 << 17)
#define CODEC_ANC_COEF_SEL_FS0_FS1_NEW                      (1 << 18)
#define CODEC_ANC_COEF_SEL_PDU0_FS0_NEW                     (1 << 19)
#define CODEC_ANC_COEF_SEL_PDU1_FS1_NEW                     (1 << 20)

// reg_108
#define CODEC_STREAM0_FIR1_CH0                              (1 << 0)
#define CODEC_FIR_MODE_CH0(n)                               (((n) & 0x3) << 1)
#define CODEC_FIR_MODE_CH0_MASK                             (0x3 << 1)
#define CODEC_FIR_MODE_CH0_SHIFT                            (1)
#define CODEC_FIR_ORDER_CH0(n)                              (((n) & 0x3FF) << 3)
#define CODEC_FIR_ORDER_CH0_MASK                            (0x3FF << 3)
#define CODEC_FIR_ORDER_CH0_SHIFT                           (3)
#define CODEC_FIR_SAMPLE_START_CH0(n)                       (((n) & 0x1FF) << 13)
#define CODEC_FIR_SAMPLE_START_CH0_MASK                     (0x1FF << 13)
#define CODEC_FIR_SAMPLE_START_CH0_SHIFT                    (13)
#define CODEC_FIR_SAMPLE_NUM_CH0(n)                         (((n) & 0x1FF) << 22)
#define CODEC_FIR_SAMPLE_NUM_CH0_MASK                       (0x1FF << 22)
#define CODEC_FIR_SAMPLE_NUM_CH0_SHIFT                      (22)
#define CODEC_FIR_DO_REMAP_CH0                              (1 << 31)

// reg_10c
#define CODEC_FIR_RESULT_BASE_ADDR_CH0(n)                   (((n) & 0x1FF) << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH0_MASK                 (0x1FF << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH0_SHIFT                (0)
#define CODEC_FIR_SLIDE_OFFSET_CH0(n)                       (((n) & 0x3F) << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH0_MASK                     (0x3F << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH0_SHIFT                    (9)
#define CODEC_FIR_BURST_LENGTH_CH0(n)                       (((n) & 0x3F) << 15)
#define CODEC_FIR_BURST_LENGTH_CH0_MASK                     (0x3F << 15)
#define CODEC_FIR_BURST_LENGTH_CH0_SHIFT                    (15)
#define CODEC_FIR_GAIN_SEL_CH0(n)                           (((n) & 0xF) << 21)
#define CODEC_FIR_GAIN_SEL_CH0_MASK                         (0xF << 21)
#define CODEC_FIR_GAIN_SEL_CH0_SHIFT                        (21)
#define CODEC_FIR_LOOP_NUM_CH0(n)                           (((n) & 0x7F) << 25)
#define CODEC_FIR_LOOP_NUM_CH0_MASK                         (0x7F << 25)
#define CODEC_FIR_LOOP_NUM_CH0_SHIFT                        (25)

// reg_110
#define CODEC_STREAM0_FIR1_CH1                              (1 << 0)
#define CODEC_FIR_MODE_CH1(n)                               (((n) & 0x3) << 1)
#define CODEC_FIR_MODE_CH1_MASK                             (0x3 << 1)
#define CODEC_FIR_MODE_CH1_SHIFT                            (1)
#define CODEC_FIR_ORDER_CH1(n)                              (((n) & 0x3FF) << 3)
#define CODEC_FIR_ORDER_CH1_MASK                            (0x3FF << 3)
#define CODEC_FIR_ORDER_CH1_SHIFT                           (3)
#define CODEC_FIR_SAMPLE_START_CH1(n)                       (((n) & 0x1FF) << 13)
#define CODEC_FIR_SAMPLE_START_CH1_MASK                     (0x1FF << 13)
#define CODEC_FIR_SAMPLE_START_CH1_SHIFT                    (13)
#define CODEC_FIR_SAMPLE_NUM_CH1(n)                         (((n) & 0x1FF) << 22)
#define CODEC_FIR_SAMPLE_NUM_CH1_MASK                       (0x1FF << 22)
#define CODEC_FIR_SAMPLE_NUM_CH1_SHIFT                      (22)
#define CODEC_FIR_DO_REMAP_CH1                              (1 << 31)

// reg_114
#define CODEC_FIR_RESULT_BASE_ADDR_CH1(n)                   (((n) & 0x1FF) << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH1_MASK                 (0x1FF << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH1_SHIFT                (0)
#define CODEC_FIR_SLIDE_OFFSET_CH1(n)                       (((n) & 0x3F) << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH1_MASK                     (0x3F << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH1_SHIFT                    (9)
#define CODEC_FIR_BURST_LENGTH_CH1(n)                       (((n) & 0x3F) << 15)
#define CODEC_FIR_BURST_LENGTH_CH1_MASK                     (0x3F << 15)
#define CODEC_FIR_BURST_LENGTH_CH1_SHIFT                    (15)
#define CODEC_FIR_GAIN_SEL_CH1(n)                           (((n) & 0xF) << 21)
#define CODEC_FIR_GAIN_SEL_CH1_MASK                         (0xF << 21)
#define CODEC_FIR_GAIN_SEL_CH1_SHIFT                        (21)
#define CODEC_FIR_LOOP_NUM_CH1(n)                           (((n) & 0x7F) << 25)
#define CODEC_FIR_LOOP_NUM_CH1_MASK                         (0x7F << 25)
#define CODEC_FIR_LOOP_NUM_CH1_SHIFT                        (25)

// reg_118
#define CODEC_STREAM0_FIR1_CH2                              (1 << 0)
#define CODEC_FIR_MODE_CH2(n)                               (((n) & 0x3) << 1)
#define CODEC_FIR_MODE_CH2_MASK                             (0x3 << 1)
#define CODEC_FIR_MODE_CH2_SHIFT                            (1)
#define CODEC_FIR_ORDER_CH2(n)                              (((n) & 0x3FF) << 3)
#define CODEC_FIR_ORDER_CH2_MASK                            (0x3FF << 3)
#define CODEC_FIR_ORDER_CH2_SHIFT                           (3)
#define CODEC_FIR_SAMPLE_START_CH2(n)                       (((n) & 0x1FF) << 13)
#define CODEC_FIR_SAMPLE_START_CH2_MASK                     (0x1FF << 13)
#define CODEC_FIR_SAMPLE_START_CH2_SHIFT                    (13)
#define CODEC_FIR_SAMPLE_NUM_CH2(n)                         (((n) & 0x1FF) << 22)
#define CODEC_FIR_SAMPLE_NUM_CH2_MASK                       (0x1FF << 22)
#define CODEC_FIR_SAMPLE_NUM_CH2_SHIFT                      (22)
#define CODEC_FIR_DO_REMAP_CH2                              (1 << 31)

// reg_11c
#define CODEC_FIR_RESULT_BASE_ADDR_CH2(n)                   (((n) & 0x1FF) << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH2_MASK                 (0x1FF << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH2_SHIFT                (0)
#define CODEC_FIR_SLIDE_OFFSET_CH2(n)                       (((n) & 0x3F) << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH2_MASK                     (0x3F << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH2_SHIFT                    (9)
#define CODEC_FIR_BURST_LENGTH_CH2(n)                       (((n) & 0x3F) << 15)
#define CODEC_FIR_BURST_LENGTH_CH2_MASK                     (0x3F << 15)
#define CODEC_FIR_BURST_LENGTH_CH2_SHIFT                    (15)
#define CODEC_FIR_GAIN_SEL_CH2(n)                           (((n) & 0xF) << 21)
#define CODEC_FIR_GAIN_SEL_CH2_MASK                         (0xF << 21)
#define CODEC_FIR_GAIN_SEL_CH2_SHIFT                        (21)
#define CODEC_FIR_LOOP_NUM_CH2(n)                           (((n) & 0x7F) << 25)
#define CODEC_FIR_LOOP_NUM_CH2_MASK                         (0x7F << 25)
#define CODEC_FIR_LOOP_NUM_CH2_SHIFT                        (25)

// reg_120
#define CODEC_STREAM0_FIR1_CH3                              (1 << 0)
#define CODEC_FIR_MODE_CH3(n)                               (((n) & 0x3) << 1)
#define CODEC_FIR_MODE_CH3_MASK                             (0x3 << 1)
#define CODEC_FIR_MODE_CH3_SHIFT                            (1)
#define CODEC_FIR_ORDER_CH3(n)                              (((n) & 0x3FF) << 3)
#define CODEC_FIR_ORDER_CH3_MASK                            (0x3FF << 3)
#define CODEC_FIR_ORDER_CH3_SHIFT                           (3)
#define CODEC_FIR_SAMPLE_START_CH3(n)                       (((n) & 0x1FF) << 13)
#define CODEC_FIR_SAMPLE_START_CH3_MASK                     (0x1FF << 13)
#define CODEC_FIR_SAMPLE_START_CH3_SHIFT                    (13)
#define CODEC_FIR_SAMPLE_NUM_CH3(n)                         (((n) & 0x1FF) << 22)
#define CODEC_FIR_SAMPLE_NUM_CH3_MASK                       (0x1FF << 22)
#define CODEC_FIR_SAMPLE_NUM_CH3_SHIFT                      (22)
#define CODEC_FIR_DO_REMAP_CH3                              (1 << 31)

// reg_124
#define CODEC_FIR_RESULT_BASE_ADDR_CH3(n)                   (((n) & 0x1FF) << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH3_MASK                 (0x1FF << 0)
#define CODEC_FIR_RESULT_BASE_ADDR_CH3_SHIFT                (0)
#define CODEC_FIR_SLIDE_OFFSET_CH3(n)                       (((n) & 0x3F) << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH3_MASK                     (0x3F << 9)
#define CODEC_FIR_SLIDE_OFFSET_CH3_SHIFT                    (9)
#define CODEC_FIR_BURST_LENGTH_CH3(n)                       (((n) & 0x3F) << 15)
#define CODEC_FIR_BURST_LENGTH_CH3_MASK                     (0x3F << 15)
#define CODEC_FIR_BURST_LENGTH_CH3_SHIFT                    (15)
#define CODEC_FIR_GAIN_SEL_CH3(n)                           (((n) & 0xF) << 21)
#define CODEC_FIR_GAIN_SEL_CH3_MASK                         (0xF << 21)
#define CODEC_FIR_GAIN_SEL_CH3_SHIFT                        (21)
#define CODEC_FIR_LOOP_NUM_CH3(n)                           (((n) & 0x7F) << 25)
#define CODEC_FIR_LOOP_NUM_CH3_MASK                         (0x7F << 25)
#define CODEC_FIR_LOOP_NUM_CH3_SHIFT                        (25)

// reg_128
#define CODEC_FIR_CH0_STATE(n)                              (((n) & 0xFF) << 0)
#define CODEC_FIR_CH0_STATE_MASK                            (0xFF << 0)
#define CODEC_FIR_CH0_STATE_SHIFT                           (0)
#define CODEC_FIR_CH1_STATE(n)                              (((n) & 0xFF) << 8)
#define CODEC_FIR_CH1_STATE_MASK                            (0xFF << 8)
#define CODEC_FIR_CH1_STATE_SHIFT                           (8)
#define CODEC_FIR_CH2_STATE(n)                              (((n) & 0xFF) << 16)
#define CODEC_FIR_CH2_STATE_MASK                            (0xFF << 16)
#define CODEC_FIR_CH2_STATE_SHIFT                           (16)
#define CODEC_FIR_CH3_STATE(n)                              (((n) & 0xFF) << 24)
#define CODEC_FIR_CH3_STATE_MASK                            (0xFF << 24)
#define CODEC_FIR_CH3_STATE_SHIFT                           (24)

// reg_12c
#define CODEC_CODEC_ADC_FIR_DS_DATA_SEL_CH0(n)              (((n) & 0x7) << 0)
#define CODEC_CODEC_ADC_FIR_DS_DATA_SEL_CH0_MASK            (0x7 << 0)
#define CODEC_CODEC_ADC_FIR_DS_DATA_SEL_CH0_SHIFT           (0)
#define CODEC_CODEC_ADC_FIR_DS_DATA_SEL_CH1(n)              (((n) & 0x7) << 3)
#define CODEC_CODEC_ADC_FIR_DS_DATA_SEL_CH1_MASK            (0x7 << 3)
#define CODEC_CODEC_ADC_FIR_DS_DATA_SEL_CH1_SHIFT           (3)
#define CODEC_CODEC_ADC_FIR_DS_DATA_SEL_CH2(n)              (((n) & 0x7) << 6)
#define CODEC_CODEC_ADC_FIR_DS_DATA_SEL_CH2_MASK            (0x7 << 6)
#define CODEC_CODEC_ADC_FIR_DS_DATA_SEL_CH2_SHIFT           (6)
#define CODEC_CODEC_ADC_FIR_DS_DATA_SEL_CH3(n)              (((n) & 0x7) << 9)
#define CODEC_CODEC_ADC_FIR_DS_DATA_SEL_CH3_MASK            (0x7 << 9)
#define CODEC_CODEC_ADC_FIR_DS_DATA_SEL_CH3_SHIFT           (9)

// reg_130
#define CODEC_CODEC_FB_CHECK_ENABLE_CH0                     (1 << 0)
#define CODEC_CODEC_FB_CHECK_ACC_SAMPLE_RATE_CH0(n)         (((n) & 0x3) << 1)
#define CODEC_CODEC_FB_CHECK_ACC_SAMPLE_RATE_CH0_MASK       (0x3 << 1)
#define CODEC_CODEC_FB_CHECK_ACC_SAMPLE_RATE_CH0_SHIFT      (1)
#define CODEC_CODEC_FB_CHECK_SRC_SEL_CH0(n)                 (((n) & 0x3) << 3)
#define CODEC_CODEC_FB_CHECK_SRC_SEL_CH0_MASK               (0x3 << 3)
#define CODEC_CODEC_FB_CHECK_SRC_SEL_CH0_SHIFT              (3)
#define CODEC_CODEC_FB_CHECK_KEEP_SEL_CH0                   (1 << 5)
#define CODEC_CODEC_FB_CHECK_ACC_WINDOW_CH0(n)              (((n) & 0xFFF) << 6)
#define CODEC_CODEC_FB_CHECK_ACC_WINDOW_CH0_MASK            (0xFFF << 6)
#define CODEC_CODEC_FB_CHECK_ACC_WINDOW_CH0_SHIFT           (6)
#define CODEC_CODEC_FB_CHECK_TRIG_WINDOW_CH0(n)             (((n) & 0x3FF) << 18)
#define CODEC_CODEC_FB_CHECK_TRIG_WINDOW_CH0_MASK           (0x3FF << 18)
#define CODEC_CODEC_FB_CHECK_TRIG_WINDOW_CH0_SHIFT          (18)
#define CODEC_CODEC_FB_CHECK_KEEP_CH0                       (1 << 28)

// reg_134
#define CODEC_CODEC_FB_CHECK_ENABLE_CH1                     (1 << 0)
#define CODEC_CODEC_FB_CHECK_ACC_SAMPLE_RATE_CH1(n)         (((n) & 0x3) << 1)
#define CODEC_CODEC_FB_CHECK_ACC_SAMPLE_RATE_CH1_MASK       (0x3 << 1)
#define CODEC_CODEC_FB_CHECK_ACC_SAMPLE_RATE_CH1_SHIFT      (1)
#define CODEC_CODEC_FB_CHECK_SRC_SEL_CH1(n)                 (((n) & 0x3) << 3)
#define CODEC_CODEC_FB_CHECK_SRC_SEL_CH1_MASK               (0x3 << 3)
#define CODEC_CODEC_FB_CHECK_SRC_SEL_CH1_SHIFT              (3)
#define CODEC_CODEC_FB_CHECK_KEEP_SEL_CH1                   (1 << 5)
#define CODEC_CODEC_FB_CHECK_ACC_WINDOW_CH1(n)              (((n) & 0xFFF) << 6)
#define CODEC_CODEC_FB_CHECK_ACC_WINDOW_CH1_MASK            (0xFFF << 6)
#define CODEC_CODEC_FB_CHECK_ACC_WINDOW_CH1_SHIFT           (6)
#define CODEC_CODEC_FB_CHECK_TRIG_WINDOW_CH1(n)             (((n) & 0x3FF) << 18)
#define CODEC_CODEC_FB_CHECK_TRIG_WINDOW_CH1_MASK           (0x3FF << 18)
#define CODEC_CODEC_FB_CHECK_TRIG_WINDOW_CH1_SHIFT          (18)
#define CODEC_CODEC_FB_CHECK_KEEP_CH1                       (1 << 28)

// reg_138
#define CODEC_CODEC_FB_CHECK_THRESHOLD_CH0(n)               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_FB_CHECK_THRESHOLD_CH0_MASK             (0xFFFFFFFF << 0)
#define CODEC_CODEC_FB_CHECK_THRESHOLD_CH0_SHIFT            (0)

// reg_13c
#define CODEC_CODEC_FB_CHECK_THRESHOLD_CH1(n)               (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_FB_CHECK_THRESHOLD_CH1_MASK             (0xFFFFFFFF << 0)
#define CODEC_CODEC_FB_CHECK_THRESHOLD_CH1_SHIFT            (0)

// reg_140
#define CODEC_CODEC_FB_CHECK_DATA_AVG_KEEP_CH0(n)           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_FB_CHECK_DATA_AVG_KEEP_CH0_MASK         (0xFFFFFFFF << 0)
#define CODEC_CODEC_FB_CHECK_DATA_AVG_KEEP_CH0_SHIFT        (0)

// reg_144
#define CODEC_CODEC_FB_CHECK_DATA_AVG_KEEP_CH1(n)           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_FB_CHECK_DATA_AVG_KEEP_CH1_MASK         (0xFFFFFFFF << 0)
#define CODEC_CODEC_FB_CHECK_DATA_AVG_KEEP_CH1_SHIFT        (0)

// reg_148
#define CODEC_CODEC_CLASSG_EN                               (1 << 0)
#define CODEC_CODEC_CLASSG_QUICK_DOWN                       (1 << 1)
#define CODEC_CODEC_CLASSG_STEP_4_3N                        (1 << 2)
#define CODEC_CODEC_CLASSG_LR                               (1 << 3)
#define CODEC_CODEC_CLASSG_WINDOW(n)                        (((n) & 0xFFF) << 4)
#define CODEC_CODEC_CLASSG_WINDOW_MASK                      (0xFFF << 4)
#define CODEC_CODEC_CLASSG_WINDOW_SHIFT                     (4)

// reg_14c
#define CODEC_CODEC_CLASSG_THD0(n)                          (((n) & 0xFF) << 0)
#define CODEC_CODEC_CLASSG_THD0_MASK                        (0xFF << 0)
#define CODEC_CODEC_CLASSG_THD0_SHIFT                       (0)
#define CODEC_CODEC_CLASSG_THD1(n)                          (((n) & 0xFF) << 8)
#define CODEC_CODEC_CLASSG_THD1_MASK                        (0xFF << 8)
#define CODEC_CODEC_CLASSG_THD1_SHIFT                       (8)
#define CODEC_CODEC_CLASSG_THD2(n)                          (((n) & 0xFF) << 16)
#define CODEC_CODEC_CLASSG_THD2_MASK                        (0xFF << 16)
#define CODEC_CODEC_CLASSG_THD2_SHIFT                       (16)

// reg_150
#define CODEC_CODEC_DSD_ENABLE_L                            (1 << 0)
#define CODEC_CODEC_DSD_ENABLE_R                            (1 << 1)
#define CODEC_CODEC_DSD_DATA_INV                            (1 << 2)
#define CODEC_CODEC_DSD_SAMPLE_RATE(n)                      (((n) & 0x3) << 3)
#define CODEC_CODEC_DSD_SAMPLE_RATE_MASK                    (0x3 << 3)
#define CODEC_CODEC_DSD_SAMPLE_RATE_SHIFT                   (3)

// reg_154
#define CODEC_CODEC_RAMP_STEP_CH0(n)                        (((n) & 0xFFF) << 0)
#define CODEC_CODEC_RAMP_STEP_CH0_MASK                      (0xFFF << 0)
#define CODEC_CODEC_RAMP_STEP_CH0_SHIFT                     (0)
#define CODEC_CODEC_RAMP_EN_CH0                             (1 << 12)
#define CODEC_CODEC_RAMP_INTERVAL_CH0(n)                    (((n) & 0x7) << 13)
#define CODEC_CODEC_RAMP_INTERVAL_CH0_MASK                  (0x7 << 13)
#define CODEC_CODEC_RAMP_INTERVAL_CH0_SHIFT                 (13)

// reg_158
#define CODEC_CODEC_RAMP_STEP_CH1(n)                        (((n) & 0xFFF) << 0)
#define CODEC_CODEC_RAMP_STEP_CH1_MASK                      (0xFFF << 0)
#define CODEC_CODEC_RAMP_STEP_CH1_SHIFT                     (0)
#define CODEC_CODEC_RAMP_EN_CH1                             (1 << 12)
#define CODEC_CODEC_RAMP_INTERVAL_CH1(n)                    (((n) & 0x7) << 13)
#define CODEC_CODEC_RAMP_INTERVAL_CH1_MASK                  (0x7 << 13)
#define CODEC_CODEC_RAMP_INTERVAL_CH1_SHIFT                 (13)

// reg_15c
#define CODEC_CODEC_FB_CHECK_DCF1_BYPASS_CH0                (1 << 0)
#define CODEC_CODEC_FB_CHECK_DCF1_UDC_CH0(n)                (((n) & 0xF) << 1)
#define CODEC_CODEC_FB_CHECK_DCF1_UDC_CH0_MASK              (0xF << 1)
#define CODEC_CODEC_FB_CHECK_DCF1_UDC_CH0_SHIFT             (1)
#define CODEC_CODEC_FB_CHECK_LPF1_BYPASS_CH0                (1 << 5)
#define CODEC_CODEC_FB_CHECK_LPF1_COEF_A1_CH0(n)            (((n) & 0x7F) << 6)
#define CODEC_CODEC_FB_CHECK_LPF1_COEF_A1_CH0_MASK          (0x7F << 6)
#define CODEC_CODEC_FB_CHECK_LPF1_COEF_A1_CH0_SHIFT         (6)
#define CODEC_CODEC_FB_CHECK_LPF1_COEF_B0_CH0(n)            (((n) & 0x7F) << 13)
#define CODEC_CODEC_FB_CHECK_LPF1_COEF_B0_CH0_MASK          (0x7F << 13)
#define CODEC_CODEC_FB_CHECK_LPF1_COEF_B0_CH0_SHIFT         (13)
#define CODEC_CODEC_FB_CHECK_LPF1_COEF_B1_CH0(n)            (((n) & 0x7F) << 20)
#define CODEC_CODEC_FB_CHECK_LPF1_COEF_B1_CH0_MASK          (0x7F << 20)
#define CODEC_CODEC_FB_CHECK_LPF1_COEF_B1_CH0_SHIFT         (20)

// reg_160
#define CODEC_CODEC_FB_CHECK_DCF2_BYPASS_CH0                (1 << 0)
#define CODEC_CODEC_FB_CHECK_DCF2_UDC_CH0(n)                (((n) & 0xF) << 1)
#define CODEC_CODEC_FB_CHECK_DCF2_UDC_CH0_MASK              (0xF << 1)
#define CODEC_CODEC_FB_CHECK_DCF2_UDC_CH0_SHIFT             (1)
#define CODEC_CODEC_FB_CHECK_LPF2_BYPASS_CH0                (1 << 5)
#define CODEC_CODEC_FB_CHECK_LPF2_COEF_A1_CH0(n)            (((n) & 0x7F) << 6)
#define CODEC_CODEC_FB_CHECK_LPF2_COEF_A1_CH0_MASK          (0x7F << 6)
#define CODEC_CODEC_FB_CHECK_LPF2_COEF_A1_CH0_SHIFT         (6)
#define CODEC_CODEC_FB_CHECK_LPF2_COEF_B0_CH0(n)            (((n) & 0x7F) << 13)
#define CODEC_CODEC_FB_CHECK_LPF2_COEF_B0_CH0_MASK          (0x7F << 13)
#define CODEC_CODEC_FB_CHECK_LPF2_COEF_B0_CH0_SHIFT         (13)
#define CODEC_CODEC_FB_CHECK_LPF2_COEF_B1_CH0(n)            (((n) & 0x7F) << 20)
#define CODEC_CODEC_FB_CHECK_LPF2_COEF_B1_CH0_MASK          (0x7F << 20)
#define CODEC_CODEC_FB_CHECK_LPF2_COEF_B1_CH0_SHIFT         (20)

// reg_164
#define CODEC_CODEC_FB_CHECK_DCF1_BYPASS_CH1                (1 << 0)
#define CODEC_CODEC_FB_CHECK_DCF1_UDC_CH1(n)                (((n) & 0xF) << 1)
#define CODEC_CODEC_FB_CHECK_DCF1_UDC_CH1_MASK              (0xF << 1)
#define CODEC_CODEC_FB_CHECK_DCF1_UDC_CH1_SHIFT             (1)
#define CODEC_CODEC_FB_CHECK_LPF1_BYPASS_CH1                (1 << 5)
#define CODEC_CODEC_FB_CHECK_LPF1_COEF_A1_CH1(n)            (((n) & 0x7F) << 6)
#define CODEC_CODEC_FB_CHECK_LPF1_COEF_A1_CH1_MASK          (0x7F << 6)
#define CODEC_CODEC_FB_CHECK_LPF1_COEF_A1_CH1_SHIFT         (6)
#define CODEC_CODEC_FB_CHECK_LPF1_COEF_B0_CH1(n)            (((n) & 0x7F) << 13)
#define CODEC_CODEC_FB_CHECK_LPF1_COEF_B0_CH1_MASK          (0x7F << 13)
#define CODEC_CODEC_FB_CHECK_LPF1_COEF_B0_CH1_SHIFT         (13)
#define CODEC_CODEC_FB_CHECK_LPF1_COEF_B1_CH1(n)            (((n) & 0x7F) << 20)
#define CODEC_CODEC_FB_CHECK_LPF1_COEF_B1_CH1_MASK          (0x7F << 20)
#define CODEC_CODEC_FB_CHECK_LPF1_COEF_B1_CH1_SHIFT         (20)

// reg_168
#define CODEC_CODEC_FB_CHECK_DCF2_BYPASS_CH1                (1 << 0)
#define CODEC_CODEC_FB_CHECK_DCF2_UDC_CH1(n)                (((n) & 0xF) << 1)
#define CODEC_CODEC_FB_CHECK_DCF2_UDC_CH1_MASK              (0xF << 1)
#define CODEC_CODEC_FB_CHECK_DCF2_UDC_CH1_SHIFT             (1)
#define CODEC_CODEC_FB_CHECK_LPF2_BYPASS_CH1                (1 << 5)
#define CODEC_CODEC_FB_CHECK_LPF2_COEF_A1_CH1(n)            (((n) & 0x7F) << 6)
#define CODEC_CODEC_FB_CHECK_LPF2_COEF_A1_CH1_MASK          (0x7F << 6)
#define CODEC_CODEC_FB_CHECK_LPF2_COEF_A1_CH1_SHIFT         (6)
#define CODEC_CODEC_FB_CHECK_LPF2_COEF_B0_CH1(n)            (((n) & 0x7F) << 13)
#define CODEC_CODEC_FB_CHECK_LPF2_COEF_B0_CH1_MASK          (0x7F << 13)
#define CODEC_CODEC_FB_CHECK_LPF2_COEF_B0_CH1_SHIFT         (13)
#define CODEC_CODEC_FB_CHECK_LPF2_COEF_B1_CH1(n)            (((n) & 0x7F) << 20)
#define CODEC_CODEC_FB_CHECK_LPF2_COEF_B1_CH1_MASK          (0x7F << 20)
#define CODEC_CODEC_FB_CHECK_LPF2_COEF_B1_CH1_SHIFT         (20)

// reg_174
#define CODEC_CODEC_ADC_DC_DOUT_CH0_SYNC(n)                 (((n) & 0x1FFFFF) << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH0_SYNC_MASK               (0x1FFFFF << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH0_SYNC_SHIFT              (0)

// reg_178
#define CODEC_CODEC_ADC_DC_DOUT_CH1_SYNC(n)                 (((n) & 0x1FFFFF) << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH1_SYNC_MASK               (0x1FFFFF << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH1_SYNC_SHIFT              (0)

// reg_17c
#define CODEC_CODEC_ADC_DC_DOUT_CH2_SYNC(n)                 (((n) & 0x1FFFFF) << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH2_SYNC_MASK               (0x1FFFFF << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH2_SYNC_SHIFT              (0)

// reg_180
#define CODEC_CODEC_ADC_DC_DOUT_CH3_SYNC(n)                 (((n) & 0x1FFFFF) << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH3_SYNC_MASK               (0x1FFFFF << 0)
#define CODEC_CODEC_ADC_DC_DOUT_CH3_SYNC_SHIFT              (0)

// reg_18c
#define CODEC_CODEC_ADC_DC_DIN_CH0(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH0_MASK                     (0xFFFF << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH0_SHIFT                    (0)
#define CODEC_CODEC_ADC_DC_UPDATE_CH0                       (1 << 16)

// reg_190
#define CODEC_CODEC_ADC_DC_DIN_CH1(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH1_MASK                     (0xFFFF << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH1_SHIFT                    (0)
#define CODEC_CODEC_ADC_DC_UPDATE_CH1                       (1 << 16)

// reg_194
#define CODEC_CODEC_ADC_DC_DIN_CH2(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH2_MASK                     (0xFFFF << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH2_SHIFT                    (0)
#define CODEC_CODEC_ADC_DC_UPDATE_CH2                       (1 << 16)

// reg_198
#define CODEC_CODEC_ADC_DC_DIN_CH3(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH3_MASK                     (0xFFFF << 0)
#define CODEC_CODEC_ADC_DC_DIN_CH3_SHIFT                    (0)
#define CODEC_CODEC_ADC_DC_UPDATE_CH3                       (1 << 16)

// reg_1b8
#define CODEC_CODEC_DAC_DC_CH0(n)                           (((n) & 0x7FFFF) << 0)
#define CODEC_CODEC_DAC_DC_CH0_MASK                         (0x7FFFF << 0)
#define CODEC_CODEC_DAC_DC_CH0_SHIFT                        (0)
#define CODEC_CODEC_DAC_DC_UPDATE_CH0                       (1 << 19)
#define CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH0(n)        (((n) & 0xFF) << 20)
#define CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH0_MASK      (0xFF << 20)
#define CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH0_SHIFT     (20)
#define CODEC_CODEC_DAC_DC_UPDATE_PASS0_CH0                 (1 << 28)
#define CODEC_CODEC_DAC_DC_UPDATE_STATUS_CH0                (1 << 29)

// reg_1bc
#define CODEC_CODEC_DAC_DC_CH1(n)                           (((n) & 0x7FFFF) << 0)
#define CODEC_CODEC_DAC_DC_CH1_MASK                         (0x7FFFF << 0)
#define CODEC_CODEC_DAC_DC_CH1_SHIFT                        (0)
#define CODEC_CODEC_DAC_DC_UPDATE_CH1                       (1 << 19)
#define CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH1(n)        (((n) & 0xFF) << 20)
#define CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH1_MASK      (0xFF << 20)
#define CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH1_SHIFT     (20)
#define CODEC_CODEC_DAC_DC_UPDATE_PASS0_CH1                 (1 << 28)
#define CODEC_CODEC_DAC_DC_UPDATE_STATUS_CH1                (1 << 29)

// reg_1c0
#define CODEC_CODEC_ADC_DRE_ENABLE_CH0                      (1 << 0)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH0(n)                (((n) & 0x3) << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH0_MASK              (0x3 << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH0_SHIFT             (1)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH0(n)            (((n) & 0xF) << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH0_MASK          (0xF << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH0_SHIFT         (3)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH0(n)             (((n) & 0xF) << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH0_MASK           (0xF << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH0_SHIFT          (7)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH0(n)                (((n) & 0x7) << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH0_MASK              (0x7 << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH0_SHIFT             (11)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH0(n)                (((n) & 0x1F) << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH0_MASK              (0x1F << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH0_SHIFT             (14)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_SIGN_CH0          (1 << 19)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH0(n)                  (((n) & 0x3) << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH0_MASK                (0x3 << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH0_SHIFT               (20)
#define CODEC_CODEC_ADC_DRE_OVERFLOW_MUTE_EN_CH0            (1 << 22)
#define CODEC_CODEC_ADC_DRE_MUTE_MODE_CH0                   (1 << 23)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH0(n)           (((n) & 0x3) << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH0_MASK         (0x3 << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH0_SHIFT        (24)
#define CODEC_CODEC_ADC_DRE_MUTE_STATUS_CH0                 (1 << 26)

// reg_1c4
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH0(n)                 (((n) & 0x7FF) << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH0_MASK               (0x7FF << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH0_SHIFT              (0)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH0(n)                   (((n) & 0xFFFFF) << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH0_MASK                 (0xFFFFF << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH0_SHIFT                (11)

// reg_1c8
#define CODEC_CODEC_ADC_DRE_ENABLE_CH1                      (1 << 0)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH1(n)                (((n) & 0x3) << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH1_MASK              (0x3 << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH1_SHIFT             (1)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH1(n)            (((n) & 0xF) << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH1_MASK          (0xF << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH1_SHIFT         (3)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH1(n)             (((n) & 0xF) << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH1_MASK           (0xF << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH1_SHIFT          (7)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH1(n)                (((n) & 0x7) << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH1_MASK              (0x7 << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH1_SHIFT             (11)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH1(n)                (((n) & 0x1F) << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH1_MASK              (0x1F << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH1_SHIFT             (14)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_SIGN_CH1          (1 << 19)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH1(n)                  (((n) & 0x3) << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH1_MASK                (0x3 << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH1_SHIFT               (20)
#define CODEC_CODEC_ADC_DRE_OVERFLOW_MUTE_EN_CH1            (1 << 22)
#define CODEC_CODEC_ADC_DRE_MUTE_MODE_CH1                   (1 << 23)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH1(n)           (((n) & 0x3) << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH1_MASK         (0x3 << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH1_SHIFT        (24)
#define CODEC_CODEC_ADC_DRE_MUTE_STATUS_CH1                 (1 << 26)

// reg_1cc
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH1(n)                 (((n) & 0x7FF) << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH1_MASK               (0x7FF << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH1_SHIFT              (0)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH1(n)                   (((n) & 0xFFFFF) << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH1_MASK                 (0xFFFFF << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH1_SHIFT                (11)

// reg_1d0
#define CODEC_CODEC_ADC_DRE_ENABLE_CH2                      (1 << 0)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH2(n)                (((n) & 0x3) << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH2_MASK              (0x3 << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH2_SHIFT             (1)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH2(n)            (((n) & 0xF) << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH2_MASK          (0xF << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH2_SHIFT         (3)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH2(n)             (((n) & 0xF) << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH2_MASK           (0xF << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH2_SHIFT          (7)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH2(n)                (((n) & 0x7) << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH2_MASK              (0x7 << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH2_SHIFT             (11)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH2(n)                (((n) & 0x1F) << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH2_MASK              (0x1F << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH2_SHIFT             (14)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_SIGN_CH2          (1 << 19)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH2(n)                  (((n) & 0x3) << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH2_MASK                (0x3 << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH2_SHIFT               (20)
#define CODEC_CODEC_ADC_DRE_OVERFLOW_MUTE_EN_CH2            (1 << 22)
#define CODEC_CODEC_ADC_DRE_MUTE_MODE_CH2                   (1 << 23)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH2(n)           (((n) & 0x3) << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH2_MASK         (0x3 << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH2_SHIFT        (24)
#define CODEC_CODEC_ADC_DRE_MUTE_STATUS_CH2                 (1 << 26)

// reg_1d4
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH2(n)                 (((n) & 0x7FF) << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH2_MASK               (0x7FF << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH2_SHIFT              (0)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH2(n)                   (((n) & 0xFFFFF) << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH2_MASK                 (0xFFFFF << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH2_SHIFT                (11)

// reg_1d8
#define CODEC_CODEC_ADC_DRE_ENABLE_CH3                      (1 << 0)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH3(n)                (((n) & 0x3) << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH3_MASK              (0x3 << 1)
#define CODEC_CODEC_ADC_DRE_STEP_MODE_CH3_SHIFT             (1)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH3(n)            (((n) & 0xF) << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH3_MASK          (0xF << 3)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_CH3_SHIFT         (3)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH3(n)             (((n) & 0xF) << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH3_MASK           (0xF << 7)
#define CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH3_SHIFT          (7)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH3(n)                (((n) & 0x7) << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH3_MASK              (0x7 << 11)
#define CODEC_CODEC_ADC_DRE_DELAY_DIG_CH3_SHIFT             (11)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH3(n)                (((n) & 0x1F) << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH3_MASK              (0x1F << 14)
#define CODEC_CODEC_ADC_DRE_DELAY_ANA_CH3_SHIFT             (14)
#define CODEC_CODEC_ADC_DRE_THD_DB_OFFSET_SIGN_CH3          (1 << 19)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH3(n)                  (((n) & 0x3) << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH3_MASK                (0x3 << 20)
#define CODEC_CODEC_ADC_DRE_BIT_SEL_CH3_SHIFT               (20)
#define CODEC_CODEC_ADC_DRE_OVERFLOW_MUTE_EN_CH3            (1 << 22)
#define CODEC_CODEC_ADC_DRE_MUTE_MODE_CH3                   (1 << 23)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH3(n)           (((n) & 0x3) << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH3_MASK         (0x3 << 24)
#define CODEC_CODEC_ADC_DRE_MUTE_RANGE_SEL_CH3_SHIFT        (24)
#define CODEC_CODEC_ADC_DRE_MUTE_STATUS_CH3                 (1 << 26)

// reg_1dc
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH3(n)                 (((n) & 0x7FF) << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH3_MASK               (0x7FF << 0)
#define CODEC_CODEC_ADC_DRE_AMP_HIGH_CH3_SHIFT              (0)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH3(n)                   (((n) & 0xFFFFF) << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH3_MASK                 (0xFFFFF << 11)
#define CODEC_CODEC_ADC_DRE_WINDOW_CH3_SHIFT                (11)

// reg_1e0
#define CODEC_CODEC_ADC_DRE_GAIN_STEP0_CH0(n)               (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP0_CH0_MASK             (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP0_CH0_SHIFT            (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP1_CH0(n)               (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP1_CH0_MASK             (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP1_CH0_SHIFT            (14)

// reg_1e4
#define CODEC_CODEC_ADC_DRE_GAIN_STEP2_CH0(n)               (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP2_CH0_MASK             (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP2_CH0_SHIFT            (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP3_CH0(n)               (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP3_CH0_MASK             (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP3_CH0_SHIFT            (14)

// reg_1e8
#define CODEC_CODEC_ADC_DRE_GAIN_STEP4_CH0(n)               (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP4_CH0_MASK             (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP4_CH0_SHIFT            (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP5_CH0(n)               (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP5_CH0_MASK             (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP5_CH0_SHIFT            (14)

// reg_1ec
#define CODEC_CODEC_ADC_DRE_GAIN_STEP6_CH0(n)               (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP6_CH0_MASK             (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP6_CH0_SHIFT            (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP7_CH0(n)               (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP7_CH0_MASK             (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP7_CH0_SHIFT            (14)

// reg_1f0
#define CODEC_CODEC_ADC_DRE_GAIN_STEP8_CH0(n)               (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP8_CH0_MASK             (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP8_CH0_SHIFT            (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP9_CH0(n)               (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP9_CH0_MASK             (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP9_CH0_SHIFT            (14)

// reg_1f4
#define CODEC_CODEC_ADC_DRE_GAIN_STEP10_CH0(n)              (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP10_CH0_MASK            (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP10_CH0_SHIFT           (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP11_CH0(n)              (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP11_CH0_MASK            (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP11_CH0_SHIFT           (14)

// reg_1f8
#define CODEC_CODEC_ADC_DRE_GAIN_STEP12_CH0(n)              (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP12_CH0_MASK            (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP12_CH0_SHIFT           (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP13_CH0(n)              (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP13_CH0_MASK            (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP13_CH0_SHIFT           (14)

// reg_1fc
#define CODEC_CODEC_ADC_DRE_GAIN_STEP14_CH0(n)              (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP14_CH0_MASK            (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP14_CH0_SHIFT           (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP15_CH0(n)              (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP15_CH0_MASK            (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP15_CH0_SHIFT           (14)

// reg_200
#define CODEC_CODEC_ADC_DRE_GAIN_STEP0_CH1(n)               (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP0_CH1_MASK             (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP0_CH1_SHIFT            (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP1_CH1(n)               (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP1_CH1_MASK             (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP1_CH1_SHIFT            (14)

// reg_204
#define CODEC_CODEC_ADC_DRE_GAIN_STEP2_CH1(n)               (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP2_CH1_MASK             (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP2_CH1_SHIFT            (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP3_CH1(n)               (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP3_CH1_MASK             (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP3_CH1_SHIFT            (14)

// reg_208
#define CODEC_CODEC_ADC_DRE_GAIN_STEP4_CH1(n)               (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP4_CH1_MASK             (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP4_CH1_SHIFT            (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP5_CH1(n)               (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP5_CH1_MASK             (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP5_CH1_SHIFT            (14)

// reg_20c
#define CODEC_CODEC_ADC_DRE_GAIN_STEP6_CH1(n)               (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP6_CH1_MASK             (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP6_CH1_SHIFT            (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP7_CH1(n)               (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP7_CH1_MASK             (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP7_CH1_SHIFT            (14)

// reg_210
#define CODEC_CODEC_ADC_DRE_GAIN_STEP8_CH1(n)               (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP8_CH1_MASK             (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP8_CH1_SHIFT            (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP9_CH1(n)               (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP9_CH1_MASK             (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP9_CH1_SHIFT            (14)

// reg_214
#define CODEC_CODEC_ADC_DRE_GAIN_STEP10_CH1(n)              (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP10_CH1_MASK            (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP10_CH1_SHIFT           (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP11_CH1(n)              (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP11_CH1_MASK            (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP11_CH1_SHIFT           (14)

// reg_218
#define CODEC_CODEC_ADC_DRE_GAIN_STEP12_CH1(n)              (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP12_CH1_MASK            (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP12_CH1_SHIFT           (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP13_CH1(n)              (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP13_CH1_MASK            (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP13_CH1_SHIFT           (14)

// reg_21c
#define CODEC_CODEC_ADC_DRE_GAIN_STEP14_CH1(n)              (((n) & 0x3FFF) << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP14_CH1_MASK            (0x3FFF << 0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP14_CH1_SHIFT           (0)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP15_CH1(n)              (((n) & 0x3FFF) << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP15_CH1_MASK            (0x3FFF << 14)
#define CODEC_CODEC_ADC_DRE_GAIN_STEP15_CH1_SHIFT           (14)

// reg_220
#define CODEC_CODEC_IIR_RAMP_STEP(n)                        (((n) & 0x7FFFFFF) << 0)
#define CODEC_CODEC_IIR_RAMP_STEP_MASK                      (0x7FFFFFF << 0)
#define CODEC_CODEC_IIR_RAMP_STEP_SHIFT                     (0)
#define CODEC_CODEC_IIR_RAMP_INTERVAL(n)                    (((n) & 0x7) << 27)
#define CODEC_CODEC_IIR_RAMP_INTERVAL_MASK                  (0x7 << 27)
#define CODEC_CODEC_IIR_RAMP_INTERVAL_SHIFT                 (27)

// reg_224
#define CODEC_CODEC_IIR0_RAMP_EN                            (1 << 0)
#define CODEC_CODEC_IIR2_RAMP_EN                            (1 << 1)
#define CODEC_CODEC_IIR4_RAMP_EN                            (1 << 2)
#define CODEC_CODEC_IIR5_RAMP_EN                            (1 << 3)
#define CODEC_CODEC_IIR6_RAMP_EN                            (1 << 4)
#define CODEC_CODEC_IIR7_RAMP_EN                            (1 << 5)

// reg_228
#define CODEC_CODEC_DEQ_IIR_RAMP_STEP(n)                    (((n) & 0x7FFFFFF) << 0)
#define CODEC_CODEC_DEQ_IIR_RAMP_STEP_MASK                  (0x7FFFFFF << 0)
#define CODEC_CODEC_DEQ_IIR_RAMP_STEP_SHIFT                 (0)
#define CODEC_CODEC_DEQ_IIR_RAMP_EN_CH0                     (1 << 27)
#define CODEC_CODEC_DEQ_IIR_RAMP_EN_CH1                     (1 << 28)
#define CODEC_CODEC_DEQ_IIR_RAMP_INTERVAL(n)                (((n) & 0x7) << 29)
#define CODEC_CODEC_DEQ_IIR_RAMP_INTERVAL_MASK              (0x7 << 29)
#define CODEC_CODEC_DEQ_IIR_RAMP_INTERVAL_SHIFT             (29)

// reg_22c
#define CODEC_CODEC_TT_ENABLE_CH0                           (1 << 0)
#define CODEC_CODEC_TT_ADC_SEL_CH0(n)                       (((n) & 0x7) << 1)
#define CODEC_CODEC_TT_ADC_SEL_CH0_MASK                     (0x7 << 1)
#define CODEC_CODEC_TT_ADC_SEL_CH0_SHIFT                    (1)
#define CODEC_CODEC_MM_ENABLE_CH0                           (1 << 4)
#define CODEC_CODEC_MM_FIFO_EN_CH0                          (1 << 5)
#define CODEC_CODEC_MM_FIFO_BYPASS_CH0                      (1 << 6)
#define CODEC_CODEC_MM_DELAY_CH0(n)                         (((n) & 0x1F) << 7)
#define CODEC_CODEC_MM_DELAY_CH0_MASK                       (0x1F << 7)
#define CODEC_CODEC_MM_DELAY_CH0_SHIFT                      (7)
#define CODEC_CODEC_TT_ENABLE_CH1                           (1 << 12)
#define CODEC_CODEC_TT_ADC_SEL_CH1(n)                       (((n) & 0x7) << 13)
#define CODEC_CODEC_TT_ADC_SEL_CH1_MASK                     (0x7 << 13)
#define CODEC_CODEC_TT_ADC_SEL_CH1_SHIFT                    (13)
#define CODEC_CODEC_MM_ENABLE_CH1                           (1 << 16)
#define CODEC_CODEC_MM_FIFO_EN_CH1                          (1 << 17)
#define CODEC_CODEC_MM_FIFO_BYPASS_CH1                      (1 << 18)
#define CODEC_CODEC_MM_DELAY_CH1(n)                         (((n) & 0x1F) << 19)
#define CODEC_CODEC_MM_DELAY_CH1_MASK                       (0x1F << 19)
#define CODEC_CODEC_MM_DELAY_CH1_SHIFT                      (19)

// reg_230
#define CODEC_CODEC_MUTE_GAIN_COEF_TT_CH0(n)                (((n) & 0xFFF) << 0)
#define CODEC_CODEC_MUTE_GAIN_COEF_TT_CH0_MASK              (0xFFF << 0)
#define CODEC_CODEC_MUTE_GAIN_COEF_TT_CH0_SHIFT             (0)
#define CODEC_CODEC_MUTE_GAIN_PASS0_TT_CH0                  (1 << 12)
#define CODEC_CODEC_MUTE_GAIN_UPDATE_TT_CH0                 (1 << 13)
#define CODEC_CODEC_MUTE_GAIN_COEF_TT_CH1(n)                (((n) & 0xFFF) << 14)
#define CODEC_CODEC_MUTE_GAIN_COEF_TT_CH1_MASK              (0xFFF << 14)
#define CODEC_CODEC_MUTE_GAIN_COEF_TT_CH1_SHIFT             (14)
#define CODEC_CODEC_MUTE_GAIN_PASS0_TT_CH1                  (1 << 26)
#define CODEC_CODEC_MUTE_GAIN_UPDATE_TT_CH1                 (1 << 27)

// reg_234
#define CODEC_CODEC_MUTE_GAIN_COEF_MM_CH0(n)                (((n) & 0xFFF) << 0)
#define CODEC_CODEC_MUTE_GAIN_COEF_MM_CH0_MASK              (0xFFF << 0)
#define CODEC_CODEC_MUTE_GAIN_COEF_MM_CH0_SHIFT             (0)
#define CODEC_CODEC_MUTE_GAIN_PASS0_MM_CH0                  (1 << 12)
#define CODEC_CODEC_MUTE_GAIN_UPDATE_MM_CH0                 (1 << 13)
#define CODEC_CODEC_MUTE_GAIN_COEF_MM_CH1(n)                (((n) & 0xFFF) << 14)
#define CODEC_CODEC_MUTE_GAIN_COEF_MM_CH1_MASK              (0xFFF << 14)
#define CODEC_CODEC_MUTE_GAIN_COEF_MM_CH1_SHIFT             (14)
#define CODEC_CODEC_MUTE_GAIN_PASS0_MM_CH1                  (1 << 26)
#define CODEC_CODEC_MUTE_GAIN_UPDATE_MM_CH1                 (1 << 27)

// reg_238
#define CODEC_CODEC_ANC_CALIB_GAIN_COEF_FF_CH0(n)           (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_ANC_CALIB_GAIN_COEF_FF_CH0_MASK         (0xFFFF << 0)
#define CODEC_CODEC_ANC_CALIB_GAIN_COEF_FF_CH0_SHIFT        (0)
#define CODEC_CODEC_ANC_CALIB_GAIN_COEF_FF_CH1(n)           (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_ANC_CALIB_GAIN_COEF_FF_CH1_MASK         (0xFFFF << 16)
#define CODEC_CODEC_ANC_CALIB_GAIN_COEF_FF_CH1_SHIFT        (16)

// reg_23c
#define CODEC_CODEC_ANC_CALIB_GAIN_COEF_FB_CH0(n)           (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_ANC_CALIB_GAIN_COEF_FB_CH0_MASK         (0xFFFF << 0)
#define CODEC_CODEC_ANC_CALIB_GAIN_COEF_FB_CH0_SHIFT        (0)
#define CODEC_CODEC_ANC_CALIB_GAIN_COEF_FB_CH1(n)           (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_ANC_CALIB_GAIN_COEF_FB_CH1_MASK         (0xFFFF << 16)
#define CODEC_CODEC_ANC_CALIB_GAIN_COEF_FB_CH1_SHIFT        (16)

// reg_240
#define CODEC_CODEC_CALIB_GAIN_COEF_TT_CH0(n)               (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_CALIB_GAIN_COEF_TT_CH0_MASK             (0xFFFF << 0)
#define CODEC_CODEC_CALIB_GAIN_COEF_TT_CH0_SHIFT            (0)
#define CODEC_CODEC_CALIB_GAIN_COEF_TT_CH1(n)               (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_CALIB_GAIN_COEF_TT_CH1_MASK             (0xFFFF << 16)
#define CODEC_CODEC_CALIB_GAIN_COEF_TT_CH1_SHIFT            (16)

// reg_244
#define CODEC_CODEC_ANC_CALIB_GAIN_PASS0_FF_CH0             (1 << 0)
#define CODEC_CODEC_ANC_CALIB_GAIN_PASS0_FF_CH1             (1 << 1)
#define CODEC_CODEC_ANC_CALIB_GAIN_UPDATE_FF_CH0            (1 << 2)
#define CODEC_CODEC_ANC_CALIB_GAIN_UPDATE_FF_CH1            (1 << 3)
#define CODEC_CODEC_ANC_CALIB_GAIN_PASS0_FB_CH0             (1 << 4)
#define CODEC_CODEC_ANC_CALIB_GAIN_PASS0_FB_CH1             (1 << 5)
#define CODEC_CODEC_ANC_CALIB_GAIN_UPDATE_FB_CH0            (1 << 6)
#define CODEC_CODEC_ANC_CALIB_GAIN_UPDATE_FB_CH1            (1 << 7)
#define CODEC_CODEC_CALIB_GAIN_PASS0_TT_CH0                 (1 << 8)
#define CODEC_CODEC_CALIB_GAIN_PASS0_TT_CH1                 (1 << 9)
#define CODEC_CODEC_CALIB_GAIN_UPDATE_TT_CH0                (1 << 10)
#define CODEC_CODEC_CALIB_GAIN_UPDATE_TT_CH1                (1 << 11)
#define CODEC_CODEC_TT_FIFO_EN_CH0                          (1 << 12)
#define CODEC_CODEC_TT_FIFO_BYPASS_CH0                      (1 << 13)
#define CODEC_CODEC_TT_DELAY_UPDATE_CH0                     (1 << 14)
#define CODEC_CODEC_TT_DELAY_CH0(n)                         (((n) & 0xF) << 15)
#define CODEC_CODEC_TT_DELAY_CH0_MASK                       (0xF << 15)
#define CODEC_CODEC_TT_DELAY_CH0_SHIFT                      (15)
#define CODEC_CODEC_TT_FIFO_EN_CH1                          (1 << 19)
#define CODEC_CODEC_TT_FIFO_BYPASS_CH1                      (1 << 20)
#define CODEC_CODEC_TT_DELAY_UPDATE_CH1                     (1 << 21)
#define CODEC_CODEC_TT_DELAY_CH1(n)                         (((n) & 0xF) << 22)
#define CODEC_CODEC_TT_DELAY_CH1_MASK                       (0xF << 22)
#define CODEC_CODEC_TT_DELAY_CH1_SHIFT                      (22)

// reg_248
#define CODEC_CODEC_IIR0_ENABLE                             (1 << 0)
#define CODEC_CODEC_IIR0_IIRA_ENABLE                        (1 << 1)
#define CODEC_CODEC_IIR0_IIRB_ENABLE                        (1 << 2)
#define CODEC_CODEC_IIR0_BYPASS                             (1 << 3)
#define CODEC_CODEC_IIR0_GAINCAL_EXT_BYPASS                 (1 << 4)
#define CODEC_CODEC_IIR0_GAINUSE_EXT_BYPASS                 (1 << 5)
#define CODEC_CODEC_IIR0_LMT_BYPASS                         (1 << 6)
#define CODEC_CODEC_IIR0_COUNT(n)                           (((n) & 0x1F) << 7)
#define CODEC_CODEC_IIR0_COUNT_MASK                         (0x1F << 7)
#define CODEC_CODEC_IIR0_COUNT_SHIFT                        (7)
#define CODEC_CODEC_IIR0_COEF_SWAP                          (1 << 12)
#define CODEC_CODEC_IIR0_AUTO_STOP                          (1 << 13)
#define CODEC_CODEC_IIR0_ANF_MODE                           (1 << 14)
#define CODEC_CODEC_IIR0_GC_DBG_SEL                         (1 << 15)
#define CODEC_CODEC_IIR0_COEF_SWAP_STATUS_SYNC_1            (1 << 16)
#define CODEC_CODEC_IIR0_IIRA_STOP_STATUS_SYNC_1            (1 << 17)
#define CODEC_CODEC_IIR0_IIRB_STOP_STATUS_SYNC_1            (1 << 18)

// reg_24c
//todo
// reg_250
#define CODEC_CODEC_IIR2_ENABLE                             (1 << 0)
#define CODEC_CODEC_IIR2_IIRA_ENABLE                        (1 << 1)
#define CODEC_CODEC_IIR2_IIRB_ENABLE                        (1 << 2)
#define CODEC_CODEC_IIR2_BYPASS                             (1 << 3)
#define CODEC_CODEC_IIR2_GAINCAL_EXT_BYPASS                 (1 << 4)
#define CODEC_CODEC_IIR2_GAINUSE_EXT_BYPASS                 (1 << 5)
#define CODEC_CODEC_IIR2_LMT_BYPASS                         (1 << 6)
#define CODEC_CODEC_IIR2_COUNT(n)                           (((n) & 0x1F) << 7)
#define CODEC_CODEC_IIR2_COUNT_MASK                         (0x1F << 7)
#define CODEC_CODEC_IIR2_COUNT_SHIFT                        (7)
#define CODEC_CODEC_IIR2_COEF_SWAP                          (1 << 12)
#define CODEC_CODEC_IIR2_AUTO_STOP                          (1 << 13)
#define CODEC_CODEC_IIR2_ANF_MODE                           (1 << 14)
#define CODEC_CODEC_IIR2_GC_DBG_SEL                         (1 << 15)
#define CODEC_CODEC_IIR2_COEF_SWAP_STATUS_SYNC_1            (1 << 16)
#define CODEC_CODEC_IIR2_IIRA_STOP_STATUS_SYNC_1            (1 << 17)
#define CODEC_CODEC_IIR2_IIRB_STOP_STATUS_SYNC_1            (1 << 18)

// reg_254
//todo

// reg_258
#define CODEC_CODEC_DEQ_IIR_ENABLE                          (1 << 0)
#define CODEC_CODEC_DEQ_IIR_IIRA_ENABLE                     (1 << 1)
#define CODEC_CODEC_DEQ_IIR_IIRB_ENABLE                     (1 << 2)
#define CODEC_CODEC_DEQ_IIR_CH0_BYPASS                      (1 << 3)
#define CODEC_CODEC_DEQ_IIR_CH1_BYPASS                      (1 << 4)
#define CODEC_CODEC_DEQ_IIR_GAINCAL_EXT_CH0_BYPASS          (1 << 5)
#define CODEC_CODEC_DEQ_IIR_GAINCAL_EXT_CH1_BYPASS          (1 << 6)
#define CODEC_CODEC_DEQ_IIR_GAINUSE_EXT_CH0_BYPASS          (1 << 7)
#define CODEC_CODEC_DEQ_IIR_GAINUSE_EXT_CH1_BYPASS          (1 << 8)
#define CODEC_CODEC_DEQ_IIR_LMT_CH0_BYPASS                  (1 << 9)
#define CODEC_CODEC_DEQ_IIR_LMT_CH1_BYPASS                  (1 << 10)
#define CODEC_CODEC_DEQ_IIR_COUNT_CH0(n)                    (((n) & 0x3F) << 11)
#define CODEC_CODEC_DEQ_IIR_COUNT_CH0_MASK                  (0x3F << 11)
#define CODEC_CODEC_DEQ_IIR_COUNT_CH0_SHIFT                 (11)
#define CODEC_CODEC_DEQ_IIR_COUNT_CH1(n)                    (((n) & 0x3F) << 17)
#define CODEC_CODEC_DEQ_IIR_COUNT_CH1_MASK                  (0x3F << 17)
#define CODEC_CODEC_DEQ_IIR_COUNT_CH1_SHIFT                 (17)
#define CODEC_CODEC_DEQ_IIR_COEF_SWAP                       (1 << 23)
#define CODEC_CODEC_DEQ_IIR_AUTO_STOP                       (1 << 24)
#define CODEC_CODEC_DEQ_IIR_GC_DBG_SEL                      (1 << 25)
#define CODEC_CODEC_DEQ_IIR_COEF_SWAP_STATUS_SYNC_1         (1 << 26)
#define CODEC_CODEC_DEQ_IIR_IIRA_STOP_STATUS_SYNC_1         (1 << 27)
#define CODEC_CODEC_DEQ_IIR_IIRB_STOP_STATUS_SYNC_1         (1 << 28)

// reg_25c
//todo
#define CODEC_CODEC_IIR0_GAIN_EXT_UPDATE                    (1 << 0)
#define CODEC_CODEC_IIR0_GAIN_EXT_SEL                       (1 << 1)
#define CODEC_CODEC_IIR2_GAIN_EXT_UPDATE                    (1 << 2)
#define CODEC_CODEC_IIR2_GAIN_EXT_SEL                       (1 << 3)
#define CODEC_CODEC_IIR4_GAIN_EXT_UPDATE                    (1 << 4)
#define CODEC_CODEC_IIR4_GAIN_EXT_SEL                       (1 << 5)
#define CODEC_CODEC_IIR5_GAIN_EXT_UPDATE                    (1 << 6)
#define CODEC_CODEC_IIR5_GAIN_EXT_SEL                       (1 << 7)
#define CODEC_CODEC_IIR6_GAIN_EXT_UPDATE                    (1 << 8)
#define CODEC_CODEC_IIR6_GAIN_EXT_SEL                       (1 << 9)
#define CODEC_CODEC_IIR7_GAIN_EXT_UPDATE                    (1 << 10)
#define CODEC_CODEC_IIR7_GAIN_EXT_SEL                       (1 << 11)
#define CODEC_CODEC_DEQ_IIR_GAIN_EXT_UPDATE_CH0             (1 << 12)
#define CODEC_CODEC_DEQ_IIR_GAIN_EXT_UPDATE_CH1             (1 << 13)
#define CODEC_CODEC_DEQ_IIR_GAIN_EXT_SEL_CH0                (1 << 14)
#define CODEC_CODEC_DEQ_IIR_GAIN_EXT_SEL_CH1                (1 << 15)
#define CODEC_CODEC_IIR0_LMT_TH_UPDATE                      (1 << 16)
#define CODEC_CODEC_IIR2_LMT_TH_UPDATE                      (1 << 17)
#define CODEC_CODEC_IIR4_LMT_TH_UPDATE                      (1 << 18)
#define CODEC_CODEC_IIR5_LMT_TH_UPDATE                      (1 << 19)
#define CODEC_CODEC_IIR6_LMT_TH_UPDATE                      (1 << 20)
#define CODEC_CODEC_IIR7_LMT_TH_UPDATE                      (1 << 21)
#define CODEC_CODEC_DEQ_IIR_LMT_TH_UPDATE_CH0               (1 << 22)
#define CODEC_CODEC_DEQ_IIR_LMT_TH_UPDATE_CH1               (1 << 23)
#define CODEC_CODEC_IIR_LMT_DBG_SEL(n)                      (((n) & 0xF) << 24)
#define CODEC_CODEC_IIR_LMT_DBG_SEL_MASK                    (0xF << 24)
#define CODEC_CODEC_IIR_LMT_DBG_SEL_SHIFT                   (24)

// reg_260
#define CODEC_CODEC_IIR0_GAINA_EXT(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR0_GAINA_EXT_MASK                     (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR0_GAINA_EXT_SHIFT                    (0)

// reg_264
#define CODEC_CODEC_IIR0_GAINB_EXT(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR0_GAINB_EXT_MASK                     (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR0_GAINB_EXT_SHIFT                    (0)

// reg_268
//todo

// reg_26c
//todo

// reg_270
#define CODEC_CODEC_IIR2_GAINA_EXT(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR2_GAINA_EXT_MASK                     (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR2_GAINA_EXT_SHIFT                    (0)

// reg_274
#define CODEC_CODEC_IIR2_GAINB_EXT(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR2_GAINB_EXT_MASK                     (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR2_GAINB_EXT_SHIFT                    (0)

// reg_278
//todo

// reg_27c
//todo

// reg_280
#define CODEC_CODEC_IIR4_GAINA_EXT(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR4_GAINA_EXT_MASK                     (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR4_GAINA_EXT_SHIFT                    (0)

// reg_284
#define CODEC_CODEC_IIR4_GAINB_EXT(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR4_GAINB_EXT_MASK                     (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR4_GAINB_EXT_SHIFT                    (0)

// reg_288
#define CODEC_CODEC_IIR5_GAINA_EXT(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR5_GAINA_EXT_MASK                     (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR5_GAINA_EXT_SHIFT                    (0)

// reg_28c
#define CODEC_CODEC_IIR5_GAINB_EXT(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR5_GAINB_EXT_MASK                     (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR5_GAINB_EXT_SHIFT                    (0)

// reg_290
#define CODEC_CODEC_IIR6_GAINA_EXT(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR6_GAINA_EXT_MASK                     (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR6_GAINA_EXT_SHIFT                    (0)

// reg_294
#define CODEC_CODEC_IIR6_GAINB_EXT(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR6_GAINB_EXT_MASK                     (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR6_GAINB_EXT_SHIFT                    (0)

// reg_298
#define CODEC_CODEC_IIR7_GAINA_EXT(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR7_GAINA_EXT_MASK                     (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR7_GAINA_EXT_SHIFT                    (0)

// reg_29c
#define CODEC_CODEC_IIR7_GAINB_EXT(n)                       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR7_GAINB_EXT_MASK                     (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR7_GAINB_EXT_SHIFT                    (0)

// reg_2a0
#define CODEC_CODEC_IIR0_GAINA_EXT_OUT_SYNC(n)              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR0_GAINA_EXT_OUT_SYNC_MASK            (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR0_GAINA_EXT_OUT_SYNC_SHIFT           (0)

// reg_2a4
#define CODEC_CODEC_IIR0_GAINB_EXT_OUT_SYNC(n)              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR0_GAINB_EXT_OUT_SYNC_MASK            (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR0_GAINB_EXT_OUT_SYNC_SHIFT           (0)

// reg_2a8
//todo

// reg_2a8
//todo
// reg_2b0
#define CODEC_CODEC_IIR2_GAINA_EXT_OUT_SYNC(n)              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR2_GAINA_EXT_OUT_SYNC_MASK            (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR2_GAINA_EXT_OUT_SYNC_SHIFT           (0)

// reg_2b4
#define CODEC_CODEC_IIR2_GAINB_EXT_OUT_SYNC(n)              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR2_GAINB_EXT_OUT_SYNC_MASK            (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR2_GAINB_EXT_OUT_SYNC_SHIFT           (0)

// reg_2b8
//todo

// reg_2bc
//todo

// reg_2c0
#define CODEC_CODEC_IIR4_GAINA_EXT_OUT_SYNC(n)              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR4_GAINA_EXT_OUT_SYNC_MASK            (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR4_GAINA_EXT_OUT_SYNC_SHIFT           (0)

// reg_2c4
#define CODEC_CODEC_IIR4_GAINB_EXT_OUT_SYNC(n)              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR4_GAINB_EXT_OUT_SYNC_MASK            (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR4_GAINB_EXT_OUT_SYNC_SHIFT           (0)

// reg_2c8
#define CODEC_CODEC_IIR5_GAINA_EXT_OUT_SYNC(n)              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR5_GAINA_EXT_OUT_SYNC_MASK            (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR5_GAINA_EXT_OUT_SYNC_SHIFT           (0)

// reg_2cc
#define CODEC_CODEC_IIR5_GAINB_EXT_OUT_SYNC(n)              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR5_GAINB_EXT_OUT_SYNC_MASK            (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR5_GAINB_EXT_OUT_SYNC_SHIFT           (0)

// reg_2d0
#define CODEC_CODEC_IIR6_GAINA_EXT_OUT_SYNC(n)              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR6_GAINA_EXT_OUT_SYNC_MASK            (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR6_GAINA_EXT_OUT_SYNC_SHIFT           (0)

// reg_2d4
#define CODEC_CODEC_IIR6_GAINB_EXT_OUT_SYNC(n)              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR6_GAINB_EXT_OUT_SYNC_MASK            (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR6_GAINB_EXT_OUT_SYNC_SHIFT           (0)

// reg_2d8
#define CODEC_CODEC_IIR7_GAINA_EXT_OUT_SYNC(n)              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR7_GAINA_EXT_OUT_SYNC_MASK            (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR7_GAINA_EXT_OUT_SYNC_SHIFT           (0)

// reg_2dc
#define CODEC_CODEC_IIR7_GAINB_EXT_OUT_SYNC(n)              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR7_GAINB_EXT_OUT_SYNC_MASK            (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR7_GAINB_EXT_OUT_SYNC_SHIFT           (0)

// reg_2e0
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_CH0(n)                (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_CH0_MASK              (0xFFFFFFFF << 0)
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_CH0_SHIFT             (0)

// reg_2e4
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_CH1(n)                (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_CH1_MASK              (0xFFFFFFFF << 0)
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_CH1_SHIFT             (0)

// reg_2e8
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_CH0(n)                (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_CH0_MASK              (0xFFFFFFFF << 0)
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_CH0_SHIFT             (0)

// reg_2ec
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_CH1(n)                (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_CH1_MASK              (0xFFFFFFFF << 0)
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_CH1_SHIFT             (0)

// reg_2f0
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_OUT_CH0_SYNC(n)       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_OUT_CH0_SYNC_MASK     (0xFFFFFFFF << 0)
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_OUT_CH0_SYNC_SHIFT    (0)

// reg_2f4
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_OUT_CH1_SYNC(n)       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_OUT_CH1_SYNC_MASK     (0xFFFFFFFF << 0)
#define CODEC_CODEC_DEQ_IIR_GAINA_EXT_OUT_CH1_SYNC_SHIFT    (0)

// reg_2f8
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_OUT_CH0_SYNC(n)       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_OUT_CH0_SYNC_MASK     (0xFFFFFFFF << 0)
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_OUT_CH0_SYNC_SHIFT    (0)

// reg_2fc
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_OUT_CH1_SYNC(n)       (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_OUT_CH1_SYNC_MASK     (0xFFFFFFFF << 0)
#define CODEC_CODEC_DEQ_IIR_GAINB_EXT_OUT_CH1_SYNC_SHIFT    (0)

// reg_300
//todo delete iir1 iir3
#define CODEC_CODEC_IIR0_LMT_DELAY(n)                       (((n) & 0x7F) << 0)
#define CODEC_CODEC_IIR0_LMT_DELAY_MASK                     (0x7F << 0)
#define CODEC_CODEC_IIR0_LMT_DELAY_SHIFT                    (0)
#define CODEC_CODEC_IIR2_LMT_DELAY(n)                       (((n) & 0x7F) << 7)
#define CODEC_CODEC_IIR2_LMT_DELAY_MASK                     (0x7F << 7)
#define CODEC_CODEC_IIR2_LMT_DELAY_SHIFT                    (7)

// reg_304
#define CODEC_CODEC_IIR4_LMT_DELAY(n)                       (((n) & 0x7F) << 0)
#define CODEC_CODEC_IIR4_LMT_DELAY_MASK                     (0x7F << 0)
#define CODEC_CODEC_IIR4_LMT_DELAY_SHIFT                    (0)
#define CODEC_CODEC_IIR5_LMT_DELAY(n)                       (((n) & 0x7F) << 7)
#define CODEC_CODEC_IIR5_LMT_DELAY_MASK                     (0x7F << 7)
#define CODEC_CODEC_IIR5_LMT_DELAY_SHIFT                    (7)
#define CODEC_CODEC_IIR6_LMT_DELAY(n)                       (((n) & 0x7F) << 14)
#define CODEC_CODEC_IIR6_LMT_DELAY_MASK                     (0x7F << 14)
#define CODEC_CODEC_IIR6_LMT_DELAY_SHIFT                    (14)
#define CODEC_CODEC_IIR7_LMT_DELAY(n)                       (((n) & 0x7F) << 21)
#define CODEC_CODEC_IIR7_LMT_DELAY_MASK                     (0x7F << 21)
#define CODEC_CODEC_IIR7_LMT_DELAY_SHIFT                    (21)

// reg_308
#define CODEC_CODEC_IIR0_GAIN_EXT_TH(n)                     (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR0_GAIN_EXT_TH_MASK                   (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR0_GAIN_EXT_TH_SHIFT                  (0)

//todo
// reg_310
#define CODEC_CODEC_IIR2_GAIN_EXT_TH(n)                     (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR2_GAIN_EXT_TH_MASK                   (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR2_GAIN_EXT_TH_SHIFT                  (0)

//todo

// reg_318
#define CODEC_CODEC_DEQ_IIR_GAIN_EXT_TH(n)                  (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_DEQ_IIR_GAIN_EXT_TH_MASK                (0xFFFFFFFF << 0)
#define CODEC_CODEC_DEQ_IIR_GAIN_EXT_TH_SHIFT               (0)

// reg_31c
#define CODEC_CODEC_IIR0_LMT_TH(n)                          (((n) & 0x7FFFFF) << 0)
#define CODEC_CODEC_IIR0_LMT_TH_MASK                        (0x7FFFFF << 0)
#define CODEC_CODEC_IIR0_LMT_TH_SHIFT                       (0)

//todo
// reg_324
#define CODEC_CODEC_IIR2_LMT_TH(n)                          (((n) & 0x7FFFFF) << 0)
#define CODEC_CODEC_IIR2_LMT_TH_MASK                        (0x7FFFFF << 0)
#define CODEC_CODEC_IIR2_LMT_TH_SHIFT                       (0)

//todo
// reg_32c
#define CODEC_CODEC_IIR4_LMT_TH(n)                          (((n) & 0x7FFFFF) << 0)
#define CODEC_CODEC_IIR4_LMT_TH_MASK                        (0x7FFFFF << 0)
#define CODEC_CODEC_IIR4_LMT_TH_SHIFT                       (0)

// reg_330
#define CODEC_CODEC_IIR5_LMT_TH(n)                          (((n) & 0x7FFFFF) << 0)
#define CODEC_CODEC_IIR5_LMT_TH_MASK                        (0x7FFFFF << 0)
#define CODEC_CODEC_IIR5_LMT_TH_SHIFT                       (0)

// reg_334
#define CODEC_CODEC_IIR6_LMT_TH(n)                          (((n) & 0x7FFFFF) << 0)
#define CODEC_CODEC_IIR6_LMT_TH_MASK                        (0x7FFFFF << 0)
#define CODEC_CODEC_IIR6_LMT_TH_SHIFT                       (0)

// reg_338
#define CODEC_CODEC_IIR7_LMT_TH(n)                          (((n) & 0x7FFFFF) << 0)
#define CODEC_CODEC_IIR7_LMT_TH_MASK                        (0x7FFFFF << 0)
#define CODEC_CODEC_IIR7_LMT_TH_SHIFT                       (0)

// reg_33c
#define CODEC_CODEC_MUSIC_MIX_OFF_CH0                       (1 << 0)
#define CODEC_CODEC_PDU_OFF_CH0                             (1 << 1)
#define CODEC_CODEC_PDU_MIX_EN_CH0                          (1 << 2)
#define CODEC_CODEC_DEHOWL_EN_CH0                           (1 << 3)
#define CODEC_CODEC_MUSIC_MIX_OFF_CH1                       (1 << 4)
#define CODEC_CODEC_PDU_OFF_CH1                             (1 << 5)
#define CODEC_CODEC_PDU_MIX_EN_CH1                          (1 << 6)
#define CODEC_CODEC_DEHOWL_EN_CH1                           (1 << 7)
#define CODEC_CODEC_TT_OUT_OFF_CH0                          (1 << 8)
#define CODEC_CODEC_TT_PDU_OFF_CH0                          (1 << 9)
#define CODEC_CODEC_TT_OUT_OFF_CH1                          (1 << 10)
#define CODEC_CODEC_TT_PDU_OFF_CH1                          (1 << 11)
#define CODEC_CODEC_MM_EX_TT_CH0                            (1 << 12)
#define CODEC_CODEC_MM_EX_TT_CH1                            (1 << 13)
#define CODEC_CODEC_DEHOWL_KEEP_CH0                         (1 << 14)
#define CODEC_CODEC_DEHOWL_KEEP_SEL_CH0(n)                  (((n) & 0x7) << 15)
#define CODEC_CODEC_DEHOWL_KEEP_SEL_CH0_MASK                (0x7 << 15)
#define CODEC_CODEC_DEHOWL_KEEP_SEL_CH0_SHIFT               (15)
#define CODEC_CODEC_DEHOWL_KEEP_CH1                         (1 << 18)
#define CODEC_CODEC_DEHOWL_KEEP_SEL_CH1(n)                  (((n) & 0x7) << 19)
#define CODEC_CODEC_DEHOWL_KEEP_SEL_CH1_MASK                (0x7 << 19)
#define CODEC_CODEC_DEHOWL_KEEP_SEL_CH1_SHIFT               (19)
#define CODEC_CODEC_TWS_ANC_EQ                              (1 << 22)
#define CODEC_CODEC_TWS_PSAP_EQ                             (1 << 23)
#define CODEC_CODEC_ANC_FF_TWSCH_EN                         (1 << 24)
#define CODEC_CODEC_ANC_FB_TWSCH_EN                         (1 << 25)
#define CODEC_CODEC_TT_IIR_SEL_CH0                          (1 << 26)
#define CODEC_CODEC_FB_DUAL_MODE_CH0                        (1 << 27)
#define CODEC_CODEC_MC_DUAL_MODE_CH0                        (1 << 28)

// reg_340
#define CODEC_CODEC_DEQ_IIR_LMT_DELAY_CH0(n)                (((n) & 0x7F) << 0)
#define CODEC_CODEC_DEQ_IIR_LMT_DELAY_CH0_MASK              (0x7F << 0)
#define CODEC_CODEC_DEQ_IIR_LMT_DELAY_CH0_SHIFT             (0)
#define CODEC_CODEC_DEQ_IIR_LMT_TH_CH0(n)                   (((n) & 0x7FFFFF) << 7)
#define CODEC_CODEC_DEQ_IIR_LMT_TH_CH0_MASK                 (0x7FFFFF << 7)
#define CODEC_CODEC_DEQ_IIR_LMT_TH_CH0_SHIFT                (7)

// reg_344
#define CODEC_CODEC_DEQ_IIR_LMT_DELAY_CH1(n)                (((n) & 0x7F) << 0)
#define CODEC_CODEC_DEQ_IIR_LMT_DELAY_CH1_MASK              (0x7F << 0)
#define CODEC_CODEC_DEQ_IIR_LMT_DELAY_CH1_SHIFT             (0)
#define CODEC_CODEC_DEQ_IIR_LMT_TH_CH1(n)                   (((n) & 0x7FFFFF) << 7)
#define CODEC_CODEC_DEQ_IIR_LMT_TH_CH1_MASK                 (0x7FFFFF << 7)
#define CODEC_CODEC_DEQ_IIR_LMT_TH_CH1_SHIFT                (7)

// reg_34c
#define CODEC_CODEC_DRE_DELAY_MODE_CH0                      (1 << 0)
#define CODEC_CODEC_DRE_DELAY_HL_CH0(n)                     (((n) & 0xFF) << 1)
#define CODEC_CODEC_DRE_DELAY_HL_CH0_MASK                   (0xFF << 1)
#define CODEC_CODEC_DRE_DELAY_HL_CH0_SHIFT                  (1)
#define CODEC_CODEC_DRE_DELAY_MODE_CH1                      (1 << 9)
#define CODEC_CODEC_DRE_DELAY_HL_CH1(n)                     (((n) & 0xFF) << 10)
#define CODEC_CODEC_DRE_DELAY_HL_CH1_MASK                   (0xFF << 10)
#define CODEC_CODEC_DRE_DELAY_HL_CH1_SHIFT                  (10)

// reg_350
//todo
#define CODEC_CODEC_DRE_ENABLE_CH0                          (1 << 0)
#define CODEC_CODEC_DRE_STEP_MODE_CH0(n)                    (((n) & 0x7) << 1)
#define CODEC_CODEC_DRE_STEP_MODE_CH0_MASK                  (0x7 << 1)
#define CODEC_CODEC_DRE_STEP_MODE_CH0_SHIFT                 (1)
#define CODEC_CODEC_DRE_INI_ANA_GAIN_CH0(n)                 (((n) & 0xF) << 4)
#define CODEC_CODEC_DRE_INI_ANA_GAIN_CH0_MASK               (0xF << 4)
#define CODEC_CODEC_DRE_INI_ANA_GAIN_CH0_SHIFT              (4)
#define CODEC_CODEC_DRE_DELAY_LH_CH0(n)                     (((n) & 0xFF) << 8)
#define CODEC_CODEC_DRE_DELAY_LH_CH0_MASK                   (0xFF << 8)
#define CODEC_CODEC_DRE_DELAY_LH_CH0_SHIFT                  (8)
#define CODEC_CODEC_DRE_AMP_HIGH_CH0(n)                     (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DRE_AMP_HIGH_CH0_MASK                   (0xFFFF << 16)
#define CODEC_CODEC_DRE_AMP_HIGH_CH0_SHIFT                  (16)

// reg_354
#define CODEC_CODEC_DRE_WINDOW_CH0(n)                       (((n) & 0x1FFFFF) << 0)
#define CODEC_CODEC_DRE_WINDOW_CH0_MASK                     (0x1FFFFF << 0)
#define CODEC_CODEC_DRE_WINDOW_CH0_SHIFT                    (0)
#define CODEC_CODEC_DRE_THD_DB_OFFSET_CH0(n)                (((n) & 0xF) << 21)
#define CODEC_CODEC_DRE_THD_DB_OFFSET_CH0_MASK              (0xF << 21)
#define CODEC_CODEC_DRE_THD_DB_OFFSET_CH0_SHIFT             (21)
#define CODEC_CODEC_DRE_THD_DB_OFFSET_SIGN_CH0              (1 << 25)
#define CODEC_CODEC_DRE_GAIN_OFFSET_CH0(n)                  (((n) & 0x1F) << 26)
#define CODEC_CODEC_DRE_GAIN_OFFSET_CH0_MASK                (0x1F << 26)
#define CODEC_CODEC_DRE_GAIN_OFFSET_CH0_SHIFT               (26)

// reg_358
//todo dre reborn
#define CODEC_CODEC_DRE_DB_HIGH_CH0(n)                      (((n) & 0x3F) << 0)
#define CODEC_CODEC_DRE_DB_HIGH_CH0_MASK                    (0x3F << 0)
#define CODEC_CODEC_DRE_DB_HIGH_CH0_SHIFT                   (0)
#define CODEC_CODEC_DRE_DB_LOW_CH0(n)                       (((n) & 0x3F) << 6)
#define CODEC_CODEC_DRE_DB_LOW_CH0_MASK                     (0x3F << 6)
#define CODEC_CODEC_DRE_DB_LOW_CH0_SHIFT                    (6)
#define CODEC_CODEC_DRE_GAIN_TOP_CH0(n)                     (((n) & 0xF) << 12)
#define CODEC_CODEC_DRE_GAIN_TOP_CH0_MASK                   (0xF << 12)
#define CODEC_CODEC_DRE_GAIN_TOP_CH0_SHIFT                  (12)
#define CODEC_CODEC_DRE_DELAY_DC_LH_CH0(n)                  (((n) & 0xFF) << 16)
#define CODEC_CODEC_DRE_DELAY_DC_LH_CH0_MASK                (0xFF << 16)
#define CODEC_CODEC_DRE_DELAY_DC_LH_CH0_SHIFT               (16)
#define CODEC_CODEC_DRE_DELAY_DC_HL_CH0(n)                  (((n) & 0xFF) << 24)
#define CODEC_CODEC_DRE_DELAY_DC_HL_CH0_MASK                (0xFF << 24)
#define CODEC_CODEC_DRE_DELAY_DC_HL_CH0_SHIFT               (24)

// reg_35c
//todo
#define CODEC_CODEC_DRE_ENABLE_CH1                          (1 << 0)
#define CODEC_CODEC_DRE_STEP_MODE_CH1(n)                    (((n) & 0x7) << 1)
#define CODEC_CODEC_DRE_STEP_MODE_CH1_MASK                  (0x7 << 1)
#define CODEC_CODEC_DRE_STEP_MODE_CH1_SHIFT                 (1)
#define CODEC_CODEC_DRE_INI_ANA_GAIN_CH1(n)                 (((n) & 0xF) << 4)
#define CODEC_CODEC_DRE_INI_ANA_GAIN_CH1_MASK               (0xF << 4)
#define CODEC_CODEC_DRE_INI_ANA_GAIN_CH1_SHIFT              (4)
#define CODEC_CODEC_DRE_DELAY_LH_CH1(n)                     (((n) & 0xFF) << 8)
#define CODEC_CODEC_DRE_DELAY_LH_CH1_MASK                   (0xFF << 8)
#define CODEC_CODEC_DRE_DELAY_LH_CH1_SHIFT                  (8)
#define CODEC_CODEC_DRE_AMP_HIGH_CH1(n)                     (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DRE_AMP_HIGH_CH1_MASK                   (0xFFFF << 16)
#define CODEC_CODEC_DRE_AMP_HIGH_CH1_SHIFT                  (16)

// reg_360
#define CODEC_CODEC_DRE_WINDOW_CH1(n)                       (((n) & 0x1FFFFF) << 0)
#define CODEC_CODEC_DRE_WINDOW_CH1_MASK                     (0x1FFFFF << 0)
#define CODEC_CODEC_DRE_WINDOW_CH1_SHIFT                    (0)
#define CODEC_CODEC_DRE_THD_DB_OFFSET_CH1(n)                (((n) & 0xF) << 21)
#define CODEC_CODEC_DRE_THD_DB_OFFSET_CH1_MASK              (0xF << 21)
#define CODEC_CODEC_DRE_THD_DB_OFFSET_CH1_SHIFT             (21)
#define CODEC_CODEC_DRE_THD_DB_OFFSET_SIGN_CH1              (1 << 25)
#define CODEC_CODEC_DRE_GAIN_OFFSET_CH1(n)                  (((n) & 0x1F) << 26)
#define CODEC_CODEC_DRE_GAIN_OFFSET_CH1_MASK                (0x1F << 26)
#define CODEC_CODEC_DRE_GAIN_OFFSET_CH1_SHIFT               (26)

// reg_364
//todo
#define CODEC_CODEC_DRE_DB_HIGH_CH1(n)                      (((n) & 0x3F) << 0)
#define CODEC_CODEC_DRE_DB_HIGH_CH1_MASK                    (0x3F << 0)
#define CODEC_CODEC_DRE_DB_HIGH_CH1_SHIFT                   (0)
#define CODEC_CODEC_DRE_DB_LOW_CH1(n)                       (((n) & 0x3F) << 6)
#define CODEC_CODEC_DRE_DB_LOW_CH1_MASK                     (0x3F << 6)
#define CODEC_CODEC_DRE_DB_LOW_CH1_SHIFT                    (6)
#define CODEC_CODEC_DRE_GAIN_TOP_CH1(n)                     (((n) & 0xF) << 12)
#define CODEC_CODEC_DRE_GAIN_TOP_CH1_MASK                   (0xF << 12)
#define CODEC_CODEC_DRE_GAIN_TOP_CH1_SHIFT                  (12)
#define CODEC_CODEC_DRE_DELAY_DC_LH_CH1(n)                  (((n) & 0xFF) << 16)
#define CODEC_CODEC_DRE_DELAY_DC_LH_CH1_MASK                (0xFF << 16)
#define CODEC_CODEC_DRE_DELAY_DC_LH_CH1_SHIFT               (16)
#define CODEC_CODEC_DRE_DELAY_DC_HL_CH1(n)                  (((n) & 0xFF) << 24)
#define CODEC_CODEC_DRE_DELAY_DC_HL_CH1_MASK                (0xFF << 24)
#define CODEC_CODEC_DRE_DELAY_DC_HL_CH1_SHIFT               (24)

// reg_368
#define CODEC_CODEC_DAC_DRE_GAIN_STEP0_CH0(n)               (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP0_CH0_MASK             (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP0_CH0_SHIFT            (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP1_CH0(n)               (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP1_CH0_MASK             (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP1_CH0_SHIFT            (16)

// reg_36c
#define CODEC_CODEC_DAC_DRE_GAIN_STEP2_CH0(n)               (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP2_CH0_MASK             (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP2_CH0_SHIFT            (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP3_CH0(n)               (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP3_CH0_MASK             (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP3_CH0_SHIFT            (16)

// reg_370
#define CODEC_CODEC_DAC_DRE_GAIN_STEP4_CH0(n)               (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP4_CH0_MASK             (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP4_CH0_SHIFT            (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP5_CH0(n)               (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP5_CH0_MASK             (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP5_CH0_SHIFT            (16)

// reg_374
#define CODEC_CODEC_DAC_DRE_GAIN_STEP6_CH0(n)               (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP6_CH0_MASK             (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP6_CH0_SHIFT            (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP7_CH0(n)               (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP7_CH0_MASK             (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP7_CH0_SHIFT            (16)

// reg_378
#define CODEC_CODEC_DAC_DRE_GAIN_STEP8_CH0(n)               (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP8_CH0_MASK             (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP8_CH0_SHIFT            (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP9_CH0(n)               (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP9_CH0_MASK             (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP9_CH0_SHIFT            (16)

// reg_37c
#define CODEC_CODEC_DAC_DRE_GAIN_STEP10_CH0(n)              (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP10_CH0_MASK            (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP10_CH0_SHIFT           (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP11_CH0(n)              (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP11_CH0_MASK            (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP11_CH0_SHIFT           (16)

// reg_380
#define CODEC_CODEC_DAC_DRE_GAIN_STEP12_CH0(n)              (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP12_CH0_MASK            (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP12_CH0_SHIFT           (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP13_CH0(n)              (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP13_CH0_MASK            (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP13_CH0_SHIFT           (16)

// reg_384
#define CODEC_CODEC_DAC_DRE_GAIN_STEP14_CH0(n)              (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP14_CH0_MASK            (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP14_CH0_SHIFT           (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP15_CH0(n)              (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP15_CH0_MASK            (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP15_CH0_SHIFT           (16)

// reg_388
#define CODEC_CODEC_DAC_DRE_GAIN_STEP0_CH1(n)               (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP0_CH1_MASK             (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP0_CH1_SHIFT            (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP1_CH1(n)               (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP1_CH1_MASK             (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP1_CH1_SHIFT            (16)

// reg_38c
#define CODEC_CODEC_DAC_DRE_GAIN_STEP2_CH1(n)               (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP2_CH1_MASK             (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP2_CH1_SHIFT            (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP3_CH1(n)               (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP3_CH1_MASK             (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP3_CH1_SHIFT            (16)

// reg_390
#define CODEC_CODEC_DAC_DRE_GAIN_STEP4_CH1(n)               (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP4_CH1_MASK             (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP4_CH1_SHIFT            (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP5_CH1(n)               (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP5_CH1_MASK             (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP5_CH1_SHIFT            (16)

// reg_394
#define CODEC_CODEC_DAC_DRE_GAIN_STEP6_CH1(n)               (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP6_CH1_MASK             (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP6_CH1_SHIFT            (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP7_CH1(n)               (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP7_CH1_MASK             (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP7_CH1_SHIFT            (16)

// reg_398
#define CODEC_CODEC_DAC_DRE_GAIN_STEP8_CH1(n)               (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP8_CH1_MASK             (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP8_CH1_SHIFT            (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP9_CH1(n)               (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP9_CH1_MASK             (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP9_CH1_SHIFT            (16)

// reg_39c
#define CODEC_CODEC_DAC_DRE_GAIN_STEP10_CH1(n)              (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP10_CH1_MASK            (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP10_CH1_SHIFT           (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP11_CH1(n)              (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP11_CH1_MASK            (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP11_CH1_SHIFT           (16)

// reg_3a0
#define CODEC_CODEC_DAC_DRE_GAIN_STEP12_CH1(n)              (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP12_CH1_MASK            (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP12_CH1_SHIFT           (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP13_CH1(n)              (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP13_CH1_MASK            (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP13_CH1_SHIFT           (16)

// reg_3a4
#define CODEC_CODEC_DAC_DRE_GAIN_STEP14_CH1(n)              (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP14_CH1_MASK            (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP14_CH1_SHIFT           (0)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP15_CH1(n)              (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP15_CH1_MASK            (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_GAIN_STEP15_CH1_SHIFT           (16)

// reg_3a8
#define CODEC_CODEC_DAC_DRE_DC0_CH0(n)                      (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC0_CH0_MASK                    (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC0_CH0_SHIFT                   (0)
#define CODEC_CODEC_DAC_DRE_DC1_CH0(n)                      (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC1_CH0_MASK                    (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC1_CH0_SHIFT                   (16)

// reg_3ac
#define CODEC_CODEC_DAC_DRE_DC2_CH0(n)                      (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC2_CH0_MASK                    (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC2_CH0_SHIFT                   (0)
#define CODEC_CODEC_DAC_DRE_DC3_CH0(n)                      (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC3_CH0_MASK                    (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC3_CH0_SHIFT                   (16)

// reg_3b0
#define CODEC_CODEC_DAC_DRE_DC4_CH0(n)                      (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC4_CH0_MASK                    (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC4_CH0_SHIFT                   (0)
#define CODEC_CODEC_DAC_DRE_DC5_CH0(n)                      (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC5_CH0_MASK                    (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC5_CH0_SHIFT                   (16)

// reg_3b4
#define CODEC_CODEC_DAC_DRE_DC6_CH0(n)                      (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC6_CH0_MASK                    (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC6_CH0_SHIFT                   (0)
#define CODEC_CODEC_DAC_DRE_DC7_CH0(n)                      (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC7_CH0_MASK                    (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC7_CH0_SHIFT                   (16)

// reg_3b8
#define CODEC_CODEC_DAC_DRE_DC8_CH0(n)                      (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC8_CH0_MASK                    (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC8_CH0_SHIFT                   (0)
#define CODEC_CODEC_DAC_DRE_DC9_CH0(n)                      (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC9_CH0_MASK                    (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC9_CH0_SHIFT                   (16)

// reg_3bc
#define CODEC_CODEC_DAC_DRE_DC10_CH0(n)                     (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC10_CH0_MASK                   (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC10_CH0_SHIFT                  (0)
#define CODEC_CODEC_DAC_DRE_DC11_CH0(n)                     (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC11_CH0_MASK                   (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC11_CH0_SHIFT                  (16)

// reg_3c0
#define CODEC_CODEC_DAC_DRE_DC12_CH0(n)                     (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC12_CH0_MASK                   (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC12_CH0_SHIFT                  (0)
#define CODEC_CODEC_DAC_DRE_DC13_CH0(n)                     (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC13_CH0_MASK                   (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC13_CH0_SHIFT                  (16)

// reg_3c4
#define CODEC_CODEC_DAC_DRE_DC14_CH0(n)                     (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC14_CH0_MASK                   (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC14_CH0_SHIFT                  (0)
#define CODEC_CODEC_DAC_DRE_DC15_CH0(n)                     (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC15_CH0_MASK                   (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC15_CH0_SHIFT                  (16)

// reg_3c8
#define CODEC_CODEC_DAC_DRE_DC0_CH1(n)                      (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC0_CH1_MASK                    (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC0_CH1_SHIFT                   (0)
#define CODEC_CODEC_DAC_DRE_DC1_CH1(n)                      (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC1_CH1_MASK                    (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC1_CH1_SHIFT                   (16)

// reg_3cc
#define CODEC_CODEC_DAC_DRE_DC2_CH1(n)                      (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC2_CH1_MASK                    (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC2_CH1_SHIFT                   (0)
#define CODEC_CODEC_DAC_DRE_DC3_CH1(n)                      (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC3_CH1_MASK                    (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC3_CH1_SHIFT                   (16)

// reg_3d0
#define CODEC_CODEC_DAC_DRE_DC4_CH1(n)                      (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC4_CH1_MASK                    (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC4_CH1_SHIFT                   (0)
#define CODEC_CODEC_DAC_DRE_DC5_CH1(n)                      (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC5_CH1_MASK                    (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC5_CH1_SHIFT                   (16)

// reg_3d4
#define CODEC_CODEC_DAC_DRE_DC6_CH1(n)                      (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC6_CH1_MASK                    (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC6_CH1_SHIFT                   (0)
#define CODEC_CODEC_DAC_DRE_DC7_CH1(n)                      (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC7_CH1_MASK                    (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC7_CH1_SHIFT                   (16)

// reg_3d8
#define CODEC_CODEC_DAC_DRE_DC8_CH1(n)                      (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC8_CH1_MASK                    (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC8_CH1_SHIFT                   (0)
#define CODEC_CODEC_DAC_DRE_DC9_CH1(n)                      (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC9_CH1_MASK                    (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC9_CH1_SHIFT                   (16)

// reg_3dc
#define CODEC_CODEC_DAC_DRE_DC10_CH1(n)                     (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC10_CH1_MASK                   (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC10_CH1_SHIFT                  (0)
#define CODEC_CODEC_DAC_DRE_DC11_CH1(n)                     (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC11_CH1_MASK                   (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC11_CH1_SHIFT                  (16)

// reg_3e0
#define CODEC_CODEC_DAC_DRE_DC12_CH1(n)                     (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC12_CH1_MASK                   (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC12_CH1_SHIFT                  (0)
#define CODEC_CODEC_DAC_DRE_DC13_CH1(n)                     (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC13_CH1_MASK                   (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC13_CH1_SHIFT                  (16)

// reg_3e4
#define CODEC_CODEC_DAC_DRE_DC14_CH1(n)                     (((n) & 0xFFFF) << 0)
#define CODEC_CODEC_DAC_DRE_DC14_CH1_MASK                   (0xFFFF << 0)
#define CODEC_CODEC_DAC_DRE_DC14_CH1_SHIFT                  (0)
#define CODEC_CODEC_DAC_DRE_DC15_CH1(n)                     (((n) & 0xFFFF) << 16)
#define CODEC_CODEC_DAC_DRE_DC15_CH1_MASK                   (0xFFFF << 16)
#define CODEC_CODEC_DAC_DRE_DC15_CH1_SHIFT                  (16)

// reg_3e8
#define CODEC_CODEC_DRE_ANA_GAIN_CH0_SYNC(n)                (((n) & 0x1F) << 0)
#define CODEC_CODEC_DRE_ANA_GAIN_CH0_SYNC_MASK              (0x1F << 0)
#define CODEC_CODEC_DRE_ANA_GAIN_CH0_SYNC_SHIFT             (0)
#define CODEC_CODEC_DRE_COUNT_CH0_SYNC(n)                   (((n) & 0x1FFFFF) << 5)
#define CODEC_CODEC_DRE_COUNT_CH0_SYNC_MASK                 (0x1FFFFF << 5)
#define CODEC_CODEC_DRE_COUNT_CH0_SYNC_SHIFT                (5)

// reg_3ec
#define CODEC_CODEC_DRE_ANA_GAIN_CH1_SYNC(n)                (((n) & 0x1F) << 0)
#define CODEC_CODEC_DRE_ANA_GAIN_CH1_SYNC_MASK              (0x1F << 0)
#define CODEC_CODEC_DRE_ANA_GAIN_CH1_SYNC_SHIFT             (0)
#define CODEC_CODEC_DRE_COUNT_CH1_SYNC(n)                   (((n) & 0x1FFFFF) << 5)
#define CODEC_CODEC_DRE_COUNT_CH1_SYNC_MASK                 (0x1FFFFF << 5)
#define CODEC_CODEC_DRE_COUNT_CH1_SYNC_SHIFT                (5)

// reg_3f0
#define CODEC_PSAP_CPD_GL_00(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_GL_00_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_GL_00_SHIFT                          (0)
#define CODEC_PSAP_CPD_GL_01(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_GL_01_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_GL_01_SHIFT                          (16)

// reg_3f4
#define CODEC_PSAP_CPD_GL_02(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_GL_02_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_GL_02_SHIFT                          (0)
#define CODEC_PSAP_CPD_GL_03(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_GL_03_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_GL_03_SHIFT                          (16)

// reg_3f8
#define CODEC_PSAP_CPD_GL_04(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_GL_04_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_GL_04_SHIFT                          (0)
#define CODEC_PSAP_CPD_GL_05(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_GL_05_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_GL_05_SHIFT                          (16)

// reg_3fc
#define CODEC_PSAP_CPD_GL_06(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_GL_06_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_GL_06_SHIFT                          (0)
#define CODEC_PSAP_CPD_GL_07(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_GL_07_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_GL_07_SHIFT                          (16)

// reg_400
#define CODEC_PSAP_ENABLE                                   (1 << 0)
#define CODEC_PSAP_ENABLE_CH0                               (1 << 1)
#define CODEC_PSAP_ENABLE_CH1                               (1 << 2)
#define CODEC_PSAP_ADC0_SEL_CH0(n)                          (((n) & 0x7) << 3)
#define CODEC_PSAP_ADC0_SEL_CH0_MASK                        (0x7 << 3)
#define CODEC_PSAP_ADC0_SEL_CH0_SHIFT                       (3)
#define CODEC_PSAP_ADC1_SEL_CH0(n)                          (((n) & 0x7) << 6)
#define CODEC_PSAP_ADC1_SEL_CH0_MASK                        (0x7 << 6)
#define CODEC_PSAP_ADC1_SEL_CH0_SHIFT                       (6)
#define CODEC_PSAP_ADC0_SEL_CH1(n)                          (((n) & 0x7) << 9)
#define CODEC_PSAP_ADC0_SEL_CH1_MASK                        (0x7 << 9)
#define CODEC_PSAP_ADC0_SEL_CH1_SHIFT                       (9)
#define CODEC_PSAP_ADC1_SEL_CH1(n)                          (((n) & 0x7) << 12)
#define CODEC_PSAP_ADC1_SEL_CH1_MASK                        (0x7 << 12)
#define CODEC_PSAP_ADC1_SEL_CH1_SHIFT                       (12)
#define CODEC_PSAP_RATE_SEL                                 (1 << 15)
#define CODEC_PSAP_ADC_RATE_SEL                             (1 << 16)
#define CODEC_ADC_PSAP_MODE                                 (1 << 17)
#define CODEC_DOWN_SEL_PSAP(n)                              (((n) & 0x3) << 18)
#define CODEC_DOWN_SEL_PSAP_MASK                            (0x3 << 18)
#define CODEC_DOWN_SEL_PSAP_SHIFT                           (18)
#define CODEC_DAC_PSAP_MODE                                 (1 << 20)
#define CODEC_UP_SEL_PSAP(n)                                (((n) & 0x7) << 21)
#define CODEC_UP_SEL_PSAP_MASK                              (0x7 << 21)
#define CODEC_UP_SEL_PSAP_SHIFT                             (21)
#define CODEC_PSAP_MIX_MODE(n)                              (((n) & 0x3) << 24)
#define CODEC_PSAP_MIX_MODE_MASK                            (0x3 << 24)
#define CODEC_PSAP_MIX_MODE_SHIFT                           (24)
#define CODEC_PSAP_DEHOWL_ENABLE                            (1 << 26)
#define CODEC_PSAP_DEHOWL_FIR_EN_CH0                        (1 << 27)
#define CODEC_PSAP_DEHOWL_FIR_DS_EN_CH0                     (1 << 28)

// reg_404
#define CODEC_PSAP_GAIN_EXT_UPDATE(n)                       (((n) & 0x1FFFF) << 0)
#define CODEC_PSAP_GAIN_EXT_UPDATE_MASK                     (0x1FFFF << 0)
#define CODEC_PSAP_GAIN_EXT_UPDATE_SHIFT                    (0)

// reg_408
#define CODEC_PSAP_GAIN_EXT_SEL(n)                          (((n) & 0x1FFFF) << 0)
#define CODEC_PSAP_GAIN_EXT_SEL_MASK                        (0x1FFFF << 0)
#define CODEC_PSAP_GAIN_EXT_SEL_SHIFT                       (0)
#define CODEC_PSAP_SBD_CNT(n)                               (((n) & 0xF) << 17)
#define CODEC_PSAP_SBD_CNT_MASK                             (0xF << 17)
#define CODEC_PSAP_SBD_CNT_SHIFT                            (17)

// reg_40c
#define CODEC_PSAP_GAIN_EXT_0(n)                            (((n) & 0xFFFFFFFF) << 0)
#define CODEC_PSAP_GAIN_EXT_0_MASK                          (0xFFFFFFFF << 0)
#define CODEC_PSAP_GAIN_EXT_0_SHIFT                         (0)

// reg_410
#define CODEC_PSAP_GAIN_EXT_1(n)                            (((n) & 0xFFFFFFFF) << 0)
#define CODEC_PSAP_GAIN_EXT_1_MASK                          (0xFFFFFFFF << 0)
#define CODEC_PSAP_GAIN_EXT_1_SHIFT                         (0)

// reg_414
#define CODEC_PSAP_GAIN_EXT_2(n)                            (((n) & 0xFFFFFFFF) << 0)
#define CODEC_PSAP_GAIN_EXT_2_MASK                          (0xFFFFFFFF << 0)
#define CODEC_PSAP_GAIN_EXT_2_SHIFT                         (0)

// reg_418
#define CODEC_PSAP_GAIN_EXT_3(n)                            (((n) & 0xFFFFFFFF) << 0)
#define CODEC_PSAP_GAIN_EXT_3_MASK                          (0xFFFFFFFF << 0)
#define CODEC_PSAP_GAIN_EXT_3_SHIFT                         (0)

// reg_41c
#define CODEC_PSAP_GAIN_EXT_4(n)                            (((n) & 0xFFFFFFFF) << 0)
#define CODEC_PSAP_GAIN_EXT_4_MASK                          (0xFFFFFFFF << 0)
#define CODEC_PSAP_GAIN_EXT_4_SHIFT                         (0)

// reg_420
#define CODEC_PSAP_GAIN_EXT_5(n)                            (((n) & 0xFFFFFFFF) << 0)
#define CODEC_PSAP_GAIN_EXT_5_MASK                          (0xFFFFFFFF << 0)
#define CODEC_PSAP_GAIN_EXT_5_SHIFT                         (0)

// reg_424
#define CODEC_PSAP_GAIN_EXT_6(n)                            (((n) & 0xFFFFFFFF) << 0)
#define CODEC_PSAP_GAIN_EXT_6_MASK                          (0xFFFFFFFF << 0)
#define CODEC_PSAP_GAIN_EXT_6_SHIFT                         (0)

// reg_428
#define CODEC_PSAP_GAIN_EXT_7(n)                            (((n) & 0xFFFFFFFF) << 0)
#define CODEC_PSAP_GAIN_EXT_7_MASK                          (0xFFFFFFFF << 0)
#define CODEC_PSAP_GAIN_EXT_7_SHIFT                         (0)

// reg_42c
#define CODEC_PSAP_GAIN_EXT_8(n)                            (((n) & 0xFFFFFFFF) << 0)
#define CODEC_PSAP_GAIN_EXT_8_MASK                          (0xFFFFFFFF << 0)
#define CODEC_PSAP_GAIN_EXT_8_SHIFT                         (0)

// reg_430
#define CODEC_PSAP_GAIN_EXT_9(n)                            (((n) & 0xFFFFFFFF) << 0)
#define CODEC_PSAP_GAIN_EXT_9_MASK                          (0xFFFFFFFF << 0)
#define CODEC_PSAP_GAIN_EXT_9_SHIFT                         (0)

// reg_434
#define CODEC_PSAP_GAIN_EXT_10(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_PSAP_GAIN_EXT_10_MASK                         (0xFFFFFFFF << 0)
#define CODEC_PSAP_GAIN_EXT_10_SHIFT                        (0)

// reg_438
#define CODEC_PSAP_GAIN_EXT_11(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_PSAP_GAIN_EXT_11_MASK                         (0xFFFFFFFF << 0)
#define CODEC_PSAP_GAIN_EXT_11_SHIFT                        (0)

// reg_43c
#define CODEC_PSAP_GAIN_EXT_12(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_PSAP_GAIN_EXT_12_MASK                         (0xFFFFFFFF << 0)
#define CODEC_PSAP_GAIN_EXT_12_SHIFT                        (0)

// reg_440
#define CODEC_PSAP_GAIN_EXT_13(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_PSAP_GAIN_EXT_13_MASK                         (0xFFFFFFFF << 0)
#define CODEC_PSAP_GAIN_EXT_13_SHIFT                        (0)

// reg_444
#define CODEC_PSAP_GAIN_EXT_14(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_PSAP_GAIN_EXT_14_MASK                         (0xFFFFFFFF << 0)
#define CODEC_PSAP_GAIN_EXT_14_SHIFT                        (0)

// reg_448
#define CODEC_PSAP_GAIN_EXT_15(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_PSAP_GAIN_EXT_15_MASK                         (0xFFFFFFFF << 0)
#define CODEC_PSAP_GAIN_EXT_15_SHIFT                        (0)

// reg_44c
#define CODEC_PSAP_GAIN_EXT_16(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_PSAP_GAIN_EXT_16_MASK                         (0xFFFFFFFF << 0)
#define CODEC_PSAP_GAIN_EXT_16_SHIFT                        (0)

// reg_450
#define CODEC_PSAP_CPD_CT_00(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_CT_00_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_CT_00_SHIFT                          (0)
#define CODEC_PSAP_CPD_CS_00(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_CS_00_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_CS_00_SHIFT                          (16)

// reg_454
#define CODEC_PSAP_CPD_WT_00(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_WT_00_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_WT_00_SHIFT                          (0)
#define CODEC_PSAP_CPD_WS_00(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_WS_00_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_WS_00_SHIFT                          (16)

// reg_458
#define CODEC_PSAP_CPD_ET_00(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_ET_00_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_ET_00_SHIFT                          (0)
#define CODEC_PSAP_CPD_ES_00(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_ES_00_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_ES_00_SHIFT                          (16)

// reg_45c
#define CODEC_PSAP_CPD_COEFA_AT_00(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_AT_00_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_AT_00_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_AT_00(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_AT_00_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_AT_00_SHIFT                    (16)

// reg_460
#define CODEC_PSAP_LMT_COEFA_AT(n)                          (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_LMT_COEFA_AT_MASK                        (0xFFFF << 0)
#define CODEC_PSAP_LMT_COEFA_AT_SHIFT                       (0)
#define CODEC_PSAP_LMT_COEFB_AT(n)                          (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_LMT_COEFB_AT_MASK                        (0xFFFF << 16)
#define CODEC_PSAP_LMT_COEFB_AT_SHIFT                       (16)

// reg_464
#define CODEC_PSAP_LMT_TH(n)                                (((n) & 0x7FFFF) << 0)
#define CODEC_PSAP_LMT_TH_MASK                              (0x7FFFF << 0)
#define CODEC_PSAP_LMT_TH_SHIFT                             (0)
#define CODEC_PSAP_LMT_TH_UPDATE                            (1 << 19)

// reg_468
#define CODEC_PSAP_CPD_TAVA_00(n)                           (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_TAVA_00_MASK                         (0xFFFF << 0)
#define CODEC_PSAP_CPD_TAVA_00_SHIFT                        (0)
#define CODEC_PSAP_CPD_TAVB_00(n)                           (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_TAVB_00_MASK                         (0xFFFF << 16)
#define CODEC_PSAP_CPD_TAVB_00_SHIFT                        (16)

// reg_46c
#define CODEC_PSAP_CPD_COEFA_RT_00(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_RT_00_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_RT_00_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_RT_00(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_RT_00_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_RT_00_SHIFT                    (16)

// reg_470
#define CODEC_PSAP_LMT_COEFA_RT(n)                          (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_LMT_COEFA_RT_MASK                        (0xFFFF << 0)
#define CODEC_PSAP_LMT_COEFA_RT_SHIFT                       (0)
#define CODEC_PSAP_LMT_COEFB_RT(n)                          (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_LMT_COEFB_RT_MASK                        (0xFFFF << 16)
#define CODEC_PSAP_LMT_COEFB_RT_SHIFT                       (16)

// reg_474
#define CODEC_PSAP_IIR_ENABLE                               (1 << 0)
#define CODEC_PSAP_IIR_EXT_BYPASS                           (1 << 1)
#define CODEC_PSAP_IIR_EXT_BYPASS_1                         (1 << 2)
#define CODEC_PSAP_IIR_EXT_BYPASS_2                         (1 << 3)
#define CODEC_PSAP_IIR_EXT_BYPASS_3                         (1 << 4)
#define CODEC_PSAP_IIR_EXT_BYPASS_4                         (1 << 5)
#define CODEC_PSAP_IIR_GAINCAL_EXT_BYPASS                   (1 << 6)
#define CODEC_PSAP_IIR_GAINUSE_EXT_BYPASS                   (1 << 7)
#define CODEC_PSAP_IIR_OUTPUT_BIT(n)                        (((n) & 0x3) << 8)
#define CODEC_PSAP_IIR_OUTPUT_BIT_MASK                      (0x3 << 8)
#define CODEC_PSAP_IIR_OUTPUT_BIT_SHIFT                     (8)
#define CODEC_PSAP_CPD_ENABLE                               (1 << 10)
#define CODEC_PSAP_CPD_DELAY(n)                             (((n) & 0x7F) << 11)
#define CODEC_PSAP_CPD_DELAY_MASK                           (0x7F << 11)
#define CODEC_PSAP_CPD_DELAY_SHIFT                          (11)
#define CODEC_PSAP_LMT_ENABLE                               (1 << 18)
#define CODEC_PSAP_LMT_DELAY(n)                             (((n) & 0x7F) << 19)
#define CODEC_PSAP_LMT_DELAY_MASK                           (0x7F << 19)
#define CODEC_PSAP_LMT_DELAY_SHIFT                          (19)
#define CODEC_PSAP_CPD_OUTPUT_FORMAT                        (1 << 26)
#define CODEC_PSAP_ANF_ENABLE                               (1 << 27)
#define CODEC_PSAP_ANF_MODE                                 (1 << 28)

// reg_478
#define CODEC_PSAP_RAMP_STEP(n)                             (((n) & 0x7FFFFFF) << 0)
#define CODEC_PSAP_RAMP_STEP_MASK                           (0x7FFFFFF << 0)
#define CODEC_PSAP_RAMP_STEP_SHIFT                          (0)
#define CODEC_PSAP_RAMP_EN                                  (1 << 27)
#define CODEC_PSAP_RAMP_INTERVAL(n)                         (((n) & 0x7) << 28)
#define CODEC_PSAP_RAMP_INTERVAL_MASK                       (0x7 << 28)
#define CODEC_PSAP_RAMP_INTERVAL_SHIFT                      (28)

// reg_480
#define CODEC_MUTE_GAIN_COEF_DIN0_PSAP_CH0(n)               (((n) & 0xFFF) << 0)
#define CODEC_MUTE_GAIN_COEF_DIN0_PSAP_CH0_MASK             (0xFFF << 0)
#define CODEC_MUTE_GAIN_COEF_DIN0_PSAP_CH0_SHIFT            (0)
#define CODEC_MUTE_GAIN_PASS0_DIN0_PSAP_CH0                 (1 << 12)
#define CODEC_MUTE_GAIN_UPDATE_DIN0_PSAP_CH0                (1 << 13)
#define CODEC_DIN0_PSAP_DELAY_CH0(n)                        (((n) & 0xF) << 14)
#define CODEC_DIN0_PSAP_DELAY_CH0_MASK                      (0xF << 14)
#define CODEC_DIN0_PSAP_DELAY_CH0_SHIFT                     (14)
#define CODEC_DIN1_PSAP_INV_CH0                             (1 << 18)
#define CODEC_DIN0_PSAP_FIFO_EN                             (1 << 19)
#define CODEC_DIN0_PSAP_FIFO_BYPASS                         (1 << 20)
#define CODEC_DIN0_PSAP_DELAY_UPDATE                        (1 << 21)
#define CODEC_ADC_PSAP_DEHOWL_KEEP_CH0                      (1 << 22)

// reg_484
#define CODEC_MUTE_GAIN_COEF_DIN0_PSAP_CH1(n)               (((n) & 0xFFF) << 0)
#define CODEC_MUTE_GAIN_COEF_DIN0_PSAP_CH1_MASK             (0xFFF << 0)
#define CODEC_MUTE_GAIN_COEF_DIN0_PSAP_CH1_SHIFT            (0)
#define CODEC_MUTE_GAIN_PASS0_DIN0_PSAP_CH1                 (1 << 12)
#define CODEC_MUTE_GAIN_UPDATE_DIN0_PSAP_CH1                (1 << 13)
#define CODEC_DIN0_PSAP_DELAY_CH1(n)                        (((n) & 0xF) << 14)
#define CODEC_DIN0_PSAP_DELAY_CH1_MASK                      (0xF << 14)
#define CODEC_DIN0_PSAP_DELAY_CH1_SHIFT                     (14)
#define CODEC_DIN1_PSAP_INV_CH1                             (1 << 18)

// reg_488
#define CODEC_RESERVED_REG0(n)                              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RESERVED_REG0_MASK                            (0xFFFFFFFF << 0)
#define CODEC_RESERVED_REG0_SHIFT                           (0)

// reg_48c
#define CODEC_RESERVED_REG1(n)                              (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RESERVED_REG1_MASK                            (0xFFFFFFFF << 0)
#define CODEC_RESERVED_REG1_SHIFT                           (0)

// reg_490
#define CODEC_WS_FLUSH                                      (1 << 0)
#define CODEC_WS_TRIG_MODE                                  (1 << 1)
#define CODEC_WS_TRIGGER_INTERVAL(n)                        (((n) & 0xF) << 2)
#define CODEC_WS_TRIGGER_INTERVAL_MASK                      (0xF << 2)
#define CODEC_WS_TRIGGER_INTERVAL_SHIFT                     (2)
#define CODEC_TRIG_BT0_I2S1                                 (1 << 6)
#define CODEC_CALIB_INTERVAL(n)                             (((n) & 0xF) << 7)
#define CODEC_CALIB_INTERVAL_MASK                           (0xF << 7)
#define CODEC_CALIB_INTERVAL_SHIFT                          (7)
#define CODEC_WS_CNT_INI(n)                                 (((n) & 0xFFFFF) << 11)
#define CODEC_WS_CNT_INI_MASK                               (0xFFFFF << 11)
#define CODEC_WS_CNT_INI_SHIFT                              (11)

// reg_494
#define CODEC_WS_CNT(n)                                     (((n) & 0xFFFFF) << 0)
#define CODEC_WS_CNT_MASK                                   (0xFFFFF << 0)
#define CODEC_WS_CNT_SHIFT                                  (0)

// reg_4a0
#define CODEC_PSAP_CPD_CT_01(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_CT_01_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_CT_01_SHIFT                          (0)
#define CODEC_PSAP_CPD_CS_01(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_CS_01_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_CS_01_SHIFT                          (16)

// reg_4a4
#define CODEC_PSAP_CPD_WT_01(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_WT_01_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_WT_01_SHIFT                          (0)
#define CODEC_PSAP_CPD_WS_01(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_WS_01_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_WS_01_SHIFT                          (16)

// reg_4a8
#define CODEC_PSAP_CPD_ET_01(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_ET_01_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_ET_01_SHIFT                          (0)
#define CODEC_PSAP_CPD_ES_01(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_ES_01_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_ES_01_SHIFT                          (16)

// reg_4ac
#define CODEC_PSAP_CPD_TAVA_01(n)                           (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_TAVA_01_MASK                         (0xFFFF << 0)
#define CODEC_PSAP_CPD_TAVA_01_SHIFT                        (0)
#define CODEC_PSAP_CPD_TAVB_01(n)                           (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_TAVB_01_MASK                         (0xFFFF << 16)
#define CODEC_PSAP_CPD_TAVB_01_SHIFT                        (16)

// reg_4b0
#define CODEC_PSAP_CPD_COEFA_AT_01(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_AT_01_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_AT_01_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_AT_01(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_AT_01_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_AT_01_SHIFT                    (16)

// reg_4b4
#define CODEC_PSAP_CPD_COEFA_RT_01(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_RT_01_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_RT_01_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_RT_01(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_RT_01_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_RT_01_SHIFT                    (16)

// reg_4b8
#define CODEC_PSAP_CPD_CT_02(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_CT_02_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_CT_02_SHIFT                          (0)
#define CODEC_PSAP_CPD_CS_02(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_CS_02_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_CS_02_SHIFT                          (16)

// reg_4bc
#define CODEC_PSAP_CPD_WT_02(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_WT_02_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_WT_02_SHIFT                          (0)
#define CODEC_PSAP_CPD_WS_02(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_WS_02_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_WS_02_SHIFT                          (16)

// reg_4c0
#define CODEC_PSAP_CPD_ET_02(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_ET_02_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_ET_02_SHIFT                          (0)
#define CODEC_PSAP_CPD_ES_02(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_ES_02_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_ES_02_SHIFT                          (16)

// reg_4c4
#define CODEC_PSAP_CPD_TAVA_02(n)                           (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_TAVA_02_MASK                         (0xFFFF << 0)
#define CODEC_PSAP_CPD_TAVA_02_SHIFT                        (0)
#define CODEC_PSAP_CPD_TAVB_02(n)                           (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_TAVB_02_MASK                         (0xFFFF << 16)
#define CODEC_PSAP_CPD_TAVB_02_SHIFT                        (16)

// reg_4c8
#define CODEC_PSAP_CPD_COEFA_AT_02(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_AT_02_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_AT_02_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_AT_02(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_AT_02_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_AT_02_SHIFT                    (16)

// reg_4cc
#define CODEC_PSAP_CPD_COEFA_RT_02(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_RT_02_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_RT_02_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_RT_02(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_RT_02_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_RT_02_SHIFT                    (16)

// reg_4d0
#define CODEC_PSAP_CPD_CT_03(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_CT_03_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_CT_03_SHIFT                          (0)
#define CODEC_PSAP_CPD_CS_03(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_CS_03_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_CS_03_SHIFT                          (16)

// reg_4d4
#define CODEC_PSAP_CPD_WT_03(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_WT_03_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_WT_03_SHIFT                          (0)
#define CODEC_PSAP_CPD_WS_03(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_WS_03_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_WS_03_SHIFT                          (16)

// reg_4d8
#define CODEC_PSAP_CPD_ET_03(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_ET_03_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_ET_03_SHIFT                          (0)
#define CODEC_PSAP_CPD_ES_03(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_ES_03_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_ES_03_SHIFT                          (16)

// reg_4dc
#define CODEC_PSAP_CPD_TAVA_03(n)                           (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_TAVA_03_MASK                         (0xFFFF << 0)
#define CODEC_PSAP_CPD_TAVA_03_SHIFT                        (0)
#define CODEC_PSAP_CPD_TAVB_03(n)                           (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_TAVB_03_MASK                         (0xFFFF << 16)
#define CODEC_PSAP_CPD_TAVB_03_SHIFT                        (16)

// reg_4e0
#define CODEC_PSAP_CPD_COEFA_AT_03(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_AT_03_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_AT_03_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_AT_03(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_AT_03_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_AT_03_SHIFT                    (16)

// reg_4e4
#define CODEC_PSAP_CPD_COEFA_RT_03(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_RT_03_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_RT_03_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_RT_03(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_RT_03_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_RT_03_SHIFT                    (16)

// reg_4e8
#define CODEC_PSAP_CPD_CT_04(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_CT_04_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_CT_04_SHIFT                          (0)
#define CODEC_PSAP_CPD_CS_04(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_CS_04_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_CS_04_SHIFT                          (16)

// reg_4ec
#define CODEC_PSAP_CPD_WT_04(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_WT_04_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_WT_04_SHIFT                          (0)
#define CODEC_PSAP_CPD_WS_04(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_WS_04_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_WS_04_SHIFT                          (16)

// reg_4f0
#define CODEC_PSAP_CPD_ET_04(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_ET_04_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_ET_04_SHIFT                          (0)
#define CODEC_PSAP_CPD_ES_04(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_ES_04_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_ES_04_SHIFT                          (16)

// reg_4f4
#define CODEC_PSAP_CPD_TAVA_04(n)                           (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_TAVA_04_MASK                         (0xFFFF << 0)
#define CODEC_PSAP_CPD_TAVA_04_SHIFT                        (0)
#define CODEC_PSAP_CPD_TAVB_04(n)                           (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_TAVB_04_MASK                         (0xFFFF << 16)
#define CODEC_PSAP_CPD_TAVB_04_SHIFT                        (16)

// reg_4f8
#define CODEC_PSAP_CPD_COEFA_AT_04(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_AT_04_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_AT_04_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_AT_04(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_AT_04_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_AT_04_SHIFT                    (16)

// reg_4fc
#define CODEC_PSAP_CPD_COEFA_RT_04(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_RT_04_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_RT_04_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_RT_04(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_RT_04_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_RT_04_SHIFT                    (16)

// reg_500
#define CODEC_PSAP_CPD_CT_05(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_CT_05_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_CT_05_SHIFT                          (0)
#define CODEC_PSAP_CPD_CS_05(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_CS_05_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_CS_05_SHIFT                          (16)

// reg_504
#define CODEC_PSAP_CPD_WT_05(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_WT_05_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_WT_05_SHIFT                          (0)
#define CODEC_PSAP_CPD_WS_05(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_WS_05_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_WS_05_SHIFT                          (16)

// reg_508
#define CODEC_PSAP_CPD_ET_05(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_ET_05_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_ET_05_SHIFT                          (0)
#define CODEC_PSAP_CPD_ES_05(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_ES_05_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_ES_05_SHIFT                          (16)

// reg_50c
#define CODEC_PSAP_CPD_TAVA_05(n)                           (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_TAVA_05_MASK                         (0xFFFF << 0)
#define CODEC_PSAP_CPD_TAVA_05_SHIFT                        (0)
#define CODEC_PSAP_CPD_TAVB_05(n)                           (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_TAVB_05_MASK                         (0xFFFF << 16)
#define CODEC_PSAP_CPD_TAVB_05_SHIFT                        (16)

// reg_510
#define CODEC_PSAP_CPD_COEFA_AT_05(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_AT_05_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_AT_05_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_AT_05(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_AT_05_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_AT_05_SHIFT                    (16)

// reg_514
#define CODEC_PSAP_CPD_COEFA_RT_05(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_RT_05_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_RT_05_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_RT_05(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_RT_05_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_RT_05_SHIFT                    (16)

// reg_518
#define CODEC_PSAP_CPD_CT_06(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_CT_06_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_CT_06_SHIFT                          (0)
#define CODEC_PSAP_CPD_CS_06(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_CS_06_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_CS_06_SHIFT                          (16)

// reg_51c
#define CODEC_PSAP_CPD_WT_06(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_WT_06_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_WT_06_SHIFT                          (0)
#define CODEC_PSAP_CPD_WS_06(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_WS_06_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_WS_06_SHIFT                          (16)

// reg_520
#define CODEC_PSAP_CPD_ET_06(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_ET_06_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_ET_06_SHIFT                          (0)
#define CODEC_PSAP_CPD_ES_06(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_ES_06_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_ES_06_SHIFT                          (16)

// reg_524
#define CODEC_PSAP_CPD_TAVA_06(n)                           (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_TAVA_06_MASK                         (0xFFFF << 0)
#define CODEC_PSAP_CPD_TAVA_06_SHIFT                        (0)
#define CODEC_PSAP_CPD_TAVB_06(n)                           (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_TAVB_06_MASK                         (0xFFFF << 16)
#define CODEC_PSAP_CPD_TAVB_06_SHIFT                        (16)

// reg_528
#define CODEC_PSAP_CPD_COEFA_AT_06(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_AT_06_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_AT_06_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_AT_06(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_AT_06_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_AT_06_SHIFT                    (16)

// reg_52c
#define CODEC_PSAP_CPD_COEFA_RT_06(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_RT_06_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_RT_06_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_RT_06(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_RT_06_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_RT_06_SHIFT                    (16)

// reg_530
#define CODEC_PSAP_CPD_CT_07(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_CT_07_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_CT_07_SHIFT                          (0)
#define CODEC_PSAP_CPD_CS_07(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_CS_07_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_CS_07_SHIFT                          (16)

// reg_534
#define CODEC_PSAP_CPD_WT_07(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_WT_07_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_WT_07_SHIFT                          (0)
#define CODEC_PSAP_CPD_WS_07(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_WS_07_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_WS_07_SHIFT                          (16)

// reg_538
#define CODEC_PSAP_CPD_ET_07(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_ET_07_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_ET_07_SHIFT                          (0)
#define CODEC_PSAP_CPD_ES_07(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_ES_07_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_ES_07_SHIFT                          (16)

// reg_53c
#define CODEC_PSAP_CPD_TAVA_07(n)                           (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_TAVA_07_MASK                         (0xFFFF << 0)
#define CODEC_PSAP_CPD_TAVA_07_SHIFT                        (0)
#define CODEC_PSAP_CPD_TAVB_07(n)                           (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_TAVB_07_MASK                         (0xFFFF << 16)
#define CODEC_PSAP_CPD_TAVB_07_SHIFT                        (16)

// reg_540
#define CODEC_PSAP_CPD_COEFA_AT_07(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_AT_07_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_AT_07_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_AT_07(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_AT_07_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_AT_07_SHIFT                    (16)

// reg_544
#define CODEC_PSAP_CPD_COEFA_RT_07(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_RT_07_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_RT_07_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_RT_07(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_RT_07_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_RT_07_SHIFT                    (16)

// reg_548
#define CODEC_PSAP_CPD_CT_08(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_CT_08_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_CT_08_SHIFT                          (0)
#define CODEC_PSAP_CPD_CS_08(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_CS_08_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_CS_08_SHIFT                          (16)

// reg_54c
#define CODEC_PSAP_CPD_WT_08(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_WT_08_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_WT_08_SHIFT                          (0)
#define CODEC_PSAP_CPD_WS_08(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_WS_08_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_WS_08_SHIFT                          (16)

// reg_550
#define CODEC_PSAP_CPD_ET_08(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_ET_08_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_ET_08_SHIFT                          (0)
#define CODEC_PSAP_CPD_ES_08(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_ES_08_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_ES_08_SHIFT                          (16)

// reg_554
#define CODEC_PSAP_CPD_TAVA_08(n)                           (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_TAVA_08_MASK                         (0xFFFF << 0)
#define CODEC_PSAP_CPD_TAVA_08_SHIFT                        (0)
#define CODEC_PSAP_CPD_TAVB_08(n)                           (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_TAVB_08_MASK                         (0xFFFF << 16)
#define CODEC_PSAP_CPD_TAVB_08_SHIFT                        (16)

// reg_558
#define CODEC_PSAP_CPD_COEFA_AT_08(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_AT_08_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_AT_08_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_AT_08(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_AT_08_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_AT_08_SHIFT                    (16)

// reg_55c
#define CODEC_PSAP_CPD_COEFA_RT_08(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_RT_08_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_RT_08_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_RT_08(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_RT_08_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_RT_08_SHIFT                    (16)

// reg_560
#define CODEC_PSAP_CPD_CT_09(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_CT_09_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_CT_09_SHIFT                          (0)
#define CODEC_PSAP_CPD_CS_09(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_CS_09_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_CS_09_SHIFT                          (16)

// reg_564
#define CODEC_PSAP_CPD_WT_09(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_WT_09_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_WT_09_SHIFT                          (0)
#define CODEC_PSAP_CPD_WS_09(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_WS_09_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_WS_09_SHIFT                          (16)

// reg_568
#define CODEC_PSAP_CPD_ET_09(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_ET_09_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_ET_09_SHIFT                          (0)
#define CODEC_PSAP_CPD_ES_09(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_ES_09_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_ES_09_SHIFT                          (16)

// reg_56c
#define CODEC_PSAP_CPD_TAVA_09(n)                           (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_TAVA_09_MASK                         (0xFFFF << 0)
#define CODEC_PSAP_CPD_TAVA_09_SHIFT                        (0)
#define CODEC_PSAP_CPD_TAVB_09(n)                           (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_TAVB_09_MASK                         (0xFFFF << 16)
#define CODEC_PSAP_CPD_TAVB_09_SHIFT                        (16)

// reg_570
#define CODEC_PSAP_CPD_COEFA_AT_09(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_AT_09_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_AT_09_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_AT_09(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_AT_09_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_AT_09_SHIFT                    (16)

// reg_574
#define CODEC_PSAP_CPD_COEFA_RT_09(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_RT_09_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_RT_09_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_RT_09(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_RT_09_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_RT_09_SHIFT                    (16)

// reg_578
#define CODEC_PSAP_CPD_CT_10(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_CT_10_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_CT_10_SHIFT                          (0)
#define CODEC_PSAP_CPD_CS_10(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_CS_10_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_CS_10_SHIFT                          (16)

// reg_57c
#define CODEC_PSAP_CPD_WT_10(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_WT_10_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_WT_10_SHIFT                          (0)
#define CODEC_PSAP_CPD_WS_10(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_WS_10_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_WS_10_SHIFT                          (16)

// reg_580
#define CODEC_PSAP_CPD_ET_10(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_ET_10_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_ET_10_SHIFT                          (0)
#define CODEC_PSAP_CPD_ES_10(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_ES_10_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_ES_10_SHIFT                          (16)

// reg_584
#define CODEC_PSAP_CPD_TAVA_10(n)                           (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_TAVA_10_MASK                         (0xFFFF << 0)
#define CODEC_PSAP_CPD_TAVA_10_SHIFT                        (0)
#define CODEC_PSAP_CPD_TAVB_10(n)                           (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_TAVB_10_MASK                         (0xFFFF << 16)
#define CODEC_PSAP_CPD_TAVB_10_SHIFT                        (16)

// reg_588
#define CODEC_PSAP_CPD_COEFA_AT_10(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_AT_10_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_AT_10_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_AT_10(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_AT_10_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_AT_10_SHIFT                    (16)

// reg_58c
#define CODEC_PSAP_CPD_COEFA_RT_10(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_RT_10_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_RT_10_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_RT_10(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_RT_10_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_RT_10_SHIFT                    (16)

// reg_590
#define CODEC_PSAP_CPD_CT_11(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_CT_11_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_CT_11_SHIFT                          (0)
#define CODEC_PSAP_CPD_CS_11(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_CS_11_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_CS_11_SHIFT                          (16)

// reg_594
#define CODEC_PSAP_CPD_WT_11(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_WT_11_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_WT_11_SHIFT                          (0)
#define CODEC_PSAP_CPD_WS_11(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_WS_11_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_WS_11_SHIFT                          (16)

// reg_598
#define CODEC_PSAP_CPD_ET_11(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_ET_11_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_ET_11_SHIFT                          (0)
#define CODEC_PSAP_CPD_ES_11(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_ES_11_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_ES_11_SHIFT                          (16)

// reg_59c
#define CODEC_PSAP_CPD_TAVA_11(n)                           (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_TAVA_11_MASK                         (0xFFFF << 0)
#define CODEC_PSAP_CPD_TAVA_11_SHIFT                        (0)
#define CODEC_PSAP_CPD_TAVB_11(n)                           (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_TAVB_11_MASK                         (0xFFFF << 16)
#define CODEC_PSAP_CPD_TAVB_11_SHIFT                        (16)

// reg_5a0
#define CODEC_PSAP_CPD_COEFA_AT_11(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_AT_11_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_AT_11_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_AT_11(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_AT_11_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_AT_11_SHIFT                    (16)

// reg_5a4
#define CODEC_PSAP_CPD_COEFA_RT_11(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_RT_11_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_RT_11_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_RT_11(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_RT_11_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_RT_11_SHIFT                    (16)

// reg_5a8
#define CODEC_PSAP_CPD_CT_12(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_CT_12_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_CT_12_SHIFT                          (0)
#define CODEC_PSAP_CPD_CS_12(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_CS_12_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_CS_12_SHIFT                          (16)

// reg_5ac
#define CODEC_PSAP_CPD_WT_12(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_WT_12_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_WT_12_SHIFT                          (0)
#define CODEC_PSAP_CPD_WS_12(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_WS_12_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_WS_12_SHIFT                          (16)

// reg_5b0
#define CODEC_PSAP_CPD_ET_12(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_ET_12_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_ET_12_SHIFT                          (0)
#define CODEC_PSAP_CPD_ES_12(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_ES_12_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_ES_12_SHIFT                          (16)

// reg_5b4
#define CODEC_PSAP_CPD_TAVA_12(n)                           (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_TAVA_12_MASK                         (0xFFFF << 0)
#define CODEC_PSAP_CPD_TAVA_12_SHIFT                        (0)
#define CODEC_PSAP_CPD_TAVB_12(n)                           (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_TAVB_12_MASK                         (0xFFFF << 16)
#define CODEC_PSAP_CPD_TAVB_12_SHIFT                        (16)

// reg_5b8
#define CODEC_PSAP_CPD_COEFA_AT_12(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_AT_12_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_AT_12_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_AT_12(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_AT_12_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_AT_12_SHIFT                    (16)

// reg_5bc
#define CODEC_PSAP_CPD_COEFA_RT_12(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_RT_12_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_RT_12_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_RT_12(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_RT_12_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_RT_12_SHIFT                    (16)

// reg_5c0
#define CODEC_PSAP_CPD_CT_13(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_CT_13_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_CT_13_SHIFT                          (0)
#define CODEC_PSAP_CPD_CS_13(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_CS_13_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_CS_13_SHIFT                          (16)

// reg_5c4
#define CODEC_PSAP_CPD_WT_13(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_WT_13_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_WT_13_SHIFT                          (0)
#define CODEC_PSAP_CPD_WS_13(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_WS_13_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_WS_13_SHIFT                          (16)

// reg_5c8
#define CODEC_PSAP_CPD_ET_13(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_ET_13_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_ET_13_SHIFT                          (0)
#define CODEC_PSAP_CPD_ES_13(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_ES_13_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_ES_13_SHIFT                          (16)

// reg_5cc
#define CODEC_PSAP_CPD_TAVA_13(n)                           (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_TAVA_13_MASK                         (0xFFFF << 0)
#define CODEC_PSAP_CPD_TAVA_13_SHIFT                        (0)
#define CODEC_PSAP_CPD_TAVB_13(n)                           (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_TAVB_13_MASK                         (0xFFFF << 16)
#define CODEC_PSAP_CPD_TAVB_13_SHIFT                        (16)

// reg_5d0
#define CODEC_PSAP_CPD_COEFA_AT_13(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_AT_13_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_AT_13_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_AT_13(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_AT_13_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_AT_13_SHIFT                    (16)

// reg_5d4
#define CODEC_PSAP_CPD_COEFA_RT_13(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_RT_13_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_RT_13_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_RT_13(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_RT_13_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_RT_13_SHIFT                    (16)

// reg_5d8
#define CODEC_PSAP_CPD_CT_14(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_CT_14_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_CT_14_SHIFT                          (0)
#define CODEC_PSAP_CPD_CS_14(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_CS_14_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_CS_14_SHIFT                          (16)

// reg_5dc
#define CODEC_PSAP_CPD_WT_14(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_WT_14_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_WT_14_SHIFT                          (0)
#define CODEC_PSAP_CPD_WS_14(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_WS_14_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_WS_14_SHIFT                          (16)

// reg_5e0
#define CODEC_PSAP_CPD_ET_14(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_ET_14_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_ET_14_SHIFT                          (0)
#define CODEC_PSAP_CPD_ES_14(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_ES_14_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_ES_14_SHIFT                          (16)

// reg_5e4
#define CODEC_PSAP_CPD_TAVA_14(n)                           (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_TAVA_14_MASK                         (0xFFFF << 0)
#define CODEC_PSAP_CPD_TAVA_14_SHIFT                        (0)
#define CODEC_PSAP_CPD_TAVB_14(n)                           (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_TAVB_14_MASK                         (0xFFFF << 16)
#define CODEC_PSAP_CPD_TAVB_14_SHIFT                        (16)

// reg_5e8
#define CODEC_PSAP_CPD_COEFA_AT_14(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_AT_14_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_AT_14_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_AT_14(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_AT_14_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_AT_14_SHIFT                    (16)

// reg_5ec
#define CODEC_PSAP_CPD_COEFA_RT_14(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_RT_14_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_RT_14_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_RT_14(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_RT_14_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_RT_14_SHIFT                    (16)

// reg_5f0
#define CODEC_PSAP_CPD_CT_15(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_CT_15_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_CT_15_SHIFT                          (0)
#define CODEC_PSAP_CPD_CS_15(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_CS_15_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_CS_15_SHIFT                          (16)

// reg_5f4
#define CODEC_PSAP_CPD_WT_15(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_WT_15_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_WT_15_SHIFT                          (0)
#define CODEC_PSAP_CPD_WS_15(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_WS_15_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_WS_15_SHIFT                          (16)

// reg_5f8
#define CODEC_PSAP_CPD_ET_15(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_ET_15_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_ET_15_SHIFT                          (0)
#define CODEC_PSAP_CPD_ES_15(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_ES_15_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_ES_15_SHIFT                          (16)

// reg_5fc
#define CODEC_PSAP_CPD_TAVA_15(n)                           (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_TAVA_15_MASK                         (0xFFFF << 0)
#define CODEC_PSAP_CPD_TAVA_15_SHIFT                        (0)
#define CODEC_PSAP_CPD_TAVB_15(n)                           (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_TAVB_15_MASK                         (0xFFFF << 16)
#define CODEC_PSAP_CPD_TAVB_15_SHIFT                        (16)

// reg_600
#define CODEC_PSAP_CPD_COEFA_AT_15(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_AT_15_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_AT_15_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_AT_15(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_AT_15_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_AT_15_SHIFT                    (16)

// reg_604
#define CODEC_PSAP_CPD_COEFA_RT_15(n)                       (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_COEFA_RT_15_MASK                     (0xFFFF << 0)
#define CODEC_PSAP_CPD_COEFA_RT_15_SHIFT                    (0)
#define CODEC_PSAP_CPD_COEFB_RT_15(n)                       (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_COEFB_RT_15_MASK                     (0xFFFF << 16)
#define CODEC_PSAP_CPD_COEFB_RT_15_SHIFT                    (16)

// reg_608
#define CODEC_CODEC_ADC_DSF_ENABLE                          (1 << 0)
#define CODEC_CODEC_ADC_DSF_ORDER(n)                        (((n) & 0xFF) << 1)
#define CODEC_CODEC_ADC_DSF_ORDER_MASK                      (0xFF << 1)
#define CODEC_CODEC_ADC_DSF_ORDER_SHIFT                     (1)
#define CODEC_CODEC_ADC_DSF_SAMPLE_START(n)                 (((n) & 0xFF) << 9)
#define CODEC_CODEC_ADC_DSF_SAMPLE_START_MASK               (0xFF << 9)
#define CODEC_CODEC_ADC_DSF_SAMPLE_START_SHIFT              (9)
#define CODEC_CODEC_ADC_DSF_CH_ENABLE(n)                    (((n) & 0xFF) << 17)
#define CODEC_CODEC_ADC_DSF_CH_ENABLE_MASK                  (0xFF << 17)
#define CODEC_CODEC_ADC_DSF_CH_ENABLE_SHIFT                 (17)
#define CODEC_CODEC_DOWN_SEL_FIR(n)                         (((n) & 0x7) << 25)
#define CODEC_CODEC_DOWN_SEL_FIR_MASK                       (0x7 << 25)
#define CODEC_CODEC_DOWN_SEL_FIR_SHIFT                      (25)
#define CODEC_CODEC_DOWN_SEL_FREE(n)                        (((n) & 0x3) << 28)
#define CODEC_CODEC_DOWN_SEL_FREE_MASK                      (0x3 << 28)
#define CODEC_CODEC_DOWN_SEL_FREE_SHIFT                     (28)

// reg_60c
#define CODEC_CODEC_DAC_USF_ENABLE                          (1 << 0)
#define CODEC_CODEC_DAC_USF_ORDER(n)                        (((n) & 0xFF) << 1)
#define CODEC_CODEC_DAC_USF_ORDER_MASK                      (0xFF << 1)
#define CODEC_CODEC_DAC_USF_ORDER_SHIFT                     (1)
#define CODEC_CODEC_DAC_USF_SAMPLE_START(n)                 (((n) & 0xFF) << 9)
#define CODEC_CODEC_DAC_USF_SAMPLE_START_MASK               (0xFF << 9)
#define CODEC_CODEC_DAC_USF_SAMPLE_START_SHIFT              (9)
#define CODEC_CODEC_DAC_USF_UPSAMPLE                        (1 << 17)
#define CODEC_CODEC_DAC_USF_UPSAMPLE_MODE                   (1 << 18)
#define CODEC_CODEC_DAC_USF_UP_RATIO(n)                     (((n) & 0xF) << 19)
#define CODEC_CODEC_DAC_USF_UP_RATIO_MASK                   (0xF << 19)
#define CODEC_CODEC_DAC_USF_UP_RATIO_SHIFT                  (19)
#define CODEC_CODEC_DAC_USF_CH_ENABLE(n)                    (((n) & 0x3) << 23)
#define CODEC_CODEC_DAC_USF_CH_ENABLE_MASK                  (0x3 << 23)
#define CODEC_CODEC_DAC_USF_CH_ENABLE_SHIFT                 (23)
#define CODEC_CODEC_UP_SEL_FIR(n)                           (((n) & 0x7) << 25)
#define CODEC_CODEC_UP_SEL_FIR_MASK                         (0x7 << 25)
#define CODEC_CODEC_UP_SEL_FIR_SHIFT                        (25)
#define CODEC_CODEC_ADC_HBF3_BYPASS_FREE                    (1 << 28)
#define CODEC_CODEC_ADC_HBF2_BYPASS_FREE                    (1 << 29)
#define CODEC_CODEC_ADC_HBF1_BYPASS_FREE                    (1 << 30)

// reg_610
#define CODEC_CODEC_IIR0_ANF_RA(n)                          (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR0_ANF_RA_MASK                        (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR0_ANF_RA_SHIFT                       (0)
#define CODEC_CODEC_IIR0_ANF_BYPASS                         (1 << 25)

// reg_614
#define CODEC_CODEC_IIR0_ANF_RAS(n)                         (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR0_ANF_RAS_MASK                       (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR0_ANF_RAS_SHIFT                      (0)

// reg_618
#define CODEC_CODEC_IIR0_ANF_ALPHA(n)                       (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR0_ANF_ALPHA_MASK                     (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR0_ANF_ALPHA_SHIFT                    (0)

// reg_61c
#define CODEC_CODEC_IIR0_ANF_DELTA(n)                       (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR0_ANF_DELTA_MASK                     (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR0_ANF_DELTA_SHIFT                    (0)

// reg_620
#define CODEC_CODEC_IIR0_ANF_FLT_TH1(n)                     (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR0_ANF_FLT_TH1_MASK                   (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR0_ANF_FLT_TH1_SHIFT                  (0)

// reg_624
#define CODEC_CODEC_IIR0_ANF_FLT_TH2(n)                     (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR0_ANF_FLT_TH2_MASK                   (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR0_ANF_FLT_TH2_SHIFT                  (0)

// reg_628
#define CODEC_CODEC_IIR0_ANF_VAR_TH(n)                      (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR0_ANF_VAR_TH_MASK                    (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR0_ANF_VAR_TH_SHIFT                   (0)

//todo
// reg_648
#define CODEC_CODEC_IIR2_ANF_RA(n)                          (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR2_ANF_RA_MASK                        (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR2_ANF_RA_SHIFT                       (0)
#define CODEC_CODEC_IIR2_ANF_BYPASS                         (1 << 25)

// reg_64c
#define CODEC_CODEC_IIR2_ANF_RAS(n)                         (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR2_ANF_RAS_MASK                       (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR2_ANF_RAS_SHIFT                      (0)

// reg_650
#define CODEC_CODEC_IIR2_ANF_ALPHA(n)                       (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR2_ANF_ALPHA_MASK                     (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR2_ANF_ALPHA_SHIFT                    (0)

// reg_654
#define CODEC_CODEC_IIR2_ANF_DELTA(n)                       (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR2_ANF_DELTA_MASK                     (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR2_ANF_DELTA_SHIFT                    (0)

// reg_658
#define CODEC_CODEC_IIR2_ANF_FLT_TH1(n)                     (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR2_ANF_FLT_TH1_MASK                   (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR2_ANF_FLT_TH1_SHIFT                  (0)

// reg_65c
#define CODEC_CODEC_IIR2_ANF_FLT_TH2(n)                     (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR2_ANF_FLT_TH2_MASK                   (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR2_ANF_FLT_TH2_SHIFT                  (0)

// reg_660
#define CODEC_CODEC_IIR2_ANF_VAR_TH(n)                      (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR2_ANF_VAR_TH_MASK                    (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR2_ANF_VAR_TH_SHIFT                   (0)

// reg_664
#define CODEC_CODEC_IIR0_ANF_COSW_FLT_SYNC(n)               (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR0_ANF_COSW_FLT_SYNC_MASK             (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR0_ANF_COSW_FLT_SYNC_SHIFT            (0)

// reg_668
#define CODEC_CODEC_IIR2_ANF_COSW_FLT_SYNC(n)               (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR2_ANF_COSW_FLT_SYNC_MASK             (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR2_ANF_COSW_FLT_SYNC_SHIFT            (0)

// reg_66c
#define CODEC_CODEC_IIR4_ANF_COSW_FLT_SYNC(n)               (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR4_ANF_COSW_FLT_SYNC_MASK             (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR4_ANF_COSW_FLT_SYNC_SHIFT            (0)

// reg_670
#define CODEC_CODEC_IIR6_ANF_COSW_FLT_SYNC(n)               (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR6_ANF_COSW_FLT_SYNC_MASK             (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR6_ANF_COSW_FLT_SYNC_SHIFT            (0)

// reg_674
#define CODEC_CODEC_PSAP_ANF_COSW_FLT_CH0_SYNC(n)            (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_PSAP_ANF_COSW_FLT_CH0_SYNC_MASK             (0x1FFFFFF << 0)
#define CODEC_CODEC_PSAP_ANF_COSW_FLT_CH0_SYNC_SHIFT            (0)

// reg_678
#define CODEC_CODEC_IIR_GC_DBG_OUT_SYNC(n)                  (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR_GC_DBG_OUT_SYNC_MASK                (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR_GC_DBG_OUT_SYNC_SHIFT               (0)

// reg_67c
#define CODEC_CODEC_IIR_GS_DBG_OUT_SYNC(n)                  (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR_GS_DBG_OUT_SYNC_MASK                (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR_GS_DBG_OUT_SYNC_SHIFT               (0)

// reg_680
#define CODEC_CODEC_IIR4_ENABLE                             (1 << 0)
#define CODEC_CODEC_IIR4_IIRA_ENABLE                        (1 << 1)
#define CODEC_CODEC_IIR4_IIRB_ENABLE                        (1 << 2)
#define CODEC_CODEC_IIR4_BYPASS                             (1 << 3)
#define CODEC_CODEC_IIR4_GAINCAL_EXT_BYPASS                 (1 << 4)
#define CODEC_CODEC_IIR4_GAINUSE_EXT_BYPASS                 (1 << 5)
#define CODEC_CODEC_IIR4_LMT_BYPASS                         (1 << 6)
#define CODEC_CODEC_IIR4_COUNT(n)                           (((n) & 0x1F) << 7)
#define CODEC_CODEC_IIR4_COUNT_MASK                         (0x1F << 7)
#define CODEC_CODEC_IIR4_COUNT_SHIFT                        (7)
#define CODEC_CODEC_IIR4_COEF_SWAP                          (1 << 12)
#define CODEC_CODEC_IIR4_AUTO_STOP                          (1 << 13)
#define CODEC_CODEC_IIR4_ANF_MODE                           (1 << 14)
#define CODEC_CODEC_IIR4_GC_DBG_SEL                         (1 << 15)
#define CODEC_CODEC_IIR4_COEF_SWAP_STATUS_SYNC_1            (1 << 16)
#define CODEC_CODEC_IIR4_IIRA_STOP_STATUS_SYNC_1            (1 << 17)
#define CODEC_CODEC_IIR4_IIRB_STOP_STATUS_SYNC_1            (1 << 18)

// reg_684
#define CODEC_CODEC_IIR5_ENABLE                             (1 << 0)
#define CODEC_CODEC_IIR5_IIRA_ENABLE                        (1 << 1)
#define CODEC_CODEC_IIR5_IIRB_ENABLE                        (1 << 2)
#define CODEC_CODEC_IIR5_BYPASS                             (1 << 3)
#define CODEC_CODEC_IIR5_GAINCAL_EXT_BYPASS                 (1 << 4)
#define CODEC_CODEC_IIR5_GAINUSE_EXT_BYPASS                 (1 << 5)
#define CODEC_CODEC_IIR5_LMT_BYPASS                         (1 << 6)
#define CODEC_CODEC_IIR5_COUNT(n)                           (((n) & 0x1F) << 7)
#define CODEC_CODEC_IIR5_COUNT_MASK                         (0x1F << 7)
#define CODEC_CODEC_IIR5_COUNT_SHIFT                        (7)
#define CODEC_CODEC_IIR5_COEF_SWAP                          (1 << 12)
#define CODEC_CODEC_IIR5_AUTO_STOP                          (1 << 13)
#define CODEC_CODEC_IIR5_GC_DBG_SEL                         (1 << 14)
#define CODEC_CODEC_IIR5_COEF_SWAP_STATUS_SYNC_1            (1 << 15)
#define CODEC_CODEC_IIR5_IIRA_STOP_STATUS_SYNC_1            (1 << 16)
#define CODEC_CODEC_IIR5_IIRB_STOP_STATUS_SYNC_1            (1 << 17)

// reg_688
#define CODEC_CODEC_IIR6_ENABLE                             (1 << 0)
#define CODEC_CODEC_IIR6_IIRA_ENABLE                        (1 << 1)
#define CODEC_CODEC_IIR6_IIRB_ENABLE                        (1 << 2)
#define CODEC_CODEC_IIR6_BYPASS                             (1 << 3)
#define CODEC_CODEC_IIR6_GAINCAL_EXT_BYPASS                 (1 << 4)
#define CODEC_CODEC_IIR6_GAINUSE_EXT_BYPASS                 (1 << 5)
#define CODEC_CODEC_IIR6_LMT_BYPASS                         (1 << 6)
#define CODEC_CODEC_IIR6_COUNT(n)                           (((n) & 0x1F) << 7)
#define CODEC_CODEC_IIR6_COUNT_MASK                         (0x1F << 7)
#define CODEC_CODEC_IIR6_COUNT_SHIFT                        (7)
#define CODEC_CODEC_IIR6_COEF_SWAP                          (1 << 12)
#define CODEC_CODEC_IIR6_AUTO_STOP                          (1 << 13)
#define CODEC_CODEC_IIR6_ANF_MODE                           (1 << 14)
#define CODEC_CODEC_IIR6_GC_DBG_SEL                         (1 << 15)
#define CODEC_CODEC_IIR6_COEF_SWAP_STATUS_SYNC_1            (1 << 16)
#define CODEC_CODEC_IIR6_IIRA_STOP_STATUS_SYNC_1            (1 << 17)
#define CODEC_CODEC_IIR6_IIRB_STOP_STATUS_SYNC_1            (1 << 18)

// reg_68c
#define CODEC_CODEC_IIR7_ENABLE                             (1 << 0)
#define CODEC_CODEC_IIR7_IIRA_ENABLE                        (1 << 1)
#define CODEC_CODEC_IIR7_IIRB_ENABLE                        (1 << 2)
#define CODEC_CODEC_IIR7_BYPASS                             (1 << 3)
#define CODEC_CODEC_IIR7_GAINCAL_EXT_BYPASS                 (1 << 4)
#define CODEC_CODEC_IIR7_GAINUSE_EXT_BYPASS                 (1 << 5)
#define CODEC_CODEC_IIR7_LMT_BYPASS                         (1 << 6)
#define CODEC_CODEC_IIR7_COUNT(n)                           (((n) & 0x1F) << 7)
#define CODEC_CODEC_IIR7_COUNT_MASK                         (0x1F << 7)
#define CODEC_CODEC_IIR7_COUNT_SHIFT                        (7)
#define CODEC_CODEC_IIR7_COEF_SWAP                          (1 << 12)
#define CODEC_CODEC_IIR7_AUTO_STOP                          (1 << 13)
#define CODEC_CODEC_IIR7_GC_DBG_SEL                         (1 << 14)
#define CODEC_CODEC_IIR7_COEF_SWAP_STATUS_SYNC_1            (1 << 15)
#define CODEC_CODEC_IIR7_IIRA_STOP_STATUS_SYNC_1            (1 << 16)
#define CODEC_CODEC_IIR7_IIRB_STOP_STATUS_SYNC_1            (1 << 17)

// reg_690
#define CODEC_CODEC_IIR4_GAIN_EXT_TH(n)                     (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR4_GAIN_EXT_TH_MASK                   (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR4_GAIN_EXT_TH_SHIFT                  (0)

// reg_694
#define CODEC_CODEC_IIR5_GAIN_EXT_TH(n)                     (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR5_GAIN_EXT_TH_MASK                   (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR5_GAIN_EXT_TH_SHIFT                  (0)

// reg_698
#define CODEC_CODEC_IIR6_GAIN_EXT_TH(n)                     (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR6_GAIN_EXT_TH_MASK                   (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR6_GAIN_EXT_TH_SHIFT                  (0)

// reg_69c
#define CODEC_CODEC_IIR7_GAIN_EXT_TH(n)                     (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_IIR7_GAIN_EXT_TH_MASK                   (0xFFFFFFFF << 0)
#define CODEC_CODEC_IIR7_GAIN_EXT_TH_SHIFT                  (0)

// reg_6a0
#define CODEC_CODEC_IIR4_ANF_RA(n)                          (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR4_ANF_RA_MASK                        (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR4_ANF_RA_SHIFT                       (0)
#define CODEC_CODEC_IIR4_ANF_BYPASS                         (1 << 25)

// reg_6a4
#define CODEC_CODEC_IIR4_ANF_RAS(n)                         (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR4_ANF_RAS_MASK                       (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR4_ANF_RAS_SHIFT                      (0)

// reg_6a8
#define CODEC_CODEC_IIR4_ANF_ALPHA(n)                       (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR4_ANF_ALPHA_MASK                     (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR4_ANF_ALPHA_SHIFT                    (0)

// reg_6ac
#define CODEC_CODEC_IIR4_ANF_DELTA(n)                       (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR4_ANF_DELTA_MASK                     (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR4_ANF_DELTA_SHIFT                    (0)

// reg_6b0
#define CODEC_CODEC_IIR4_ANF_FLT_TH1(n)                     (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR4_ANF_FLT_TH1_MASK                   (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR4_ANF_FLT_TH1_SHIFT                  (0)

// reg_6b4
#define CODEC_CODEC_IIR4_ANF_FLT_TH2(n)                     (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR4_ANF_FLT_TH2_MASK                   (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR4_ANF_FLT_TH2_SHIFT                  (0)

// reg_6b8
#define CODEC_CODEC_IIR4_ANF_VAR_TH(n)                      (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR4_ANF_VAR_TH_MASK                    (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR4_ANF_VAR_TH_SHIFT                   (0)

// reg_6bc
#define CODEC_CODEC_IIR6_ANF_RA(n)                          (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR6_ANF_RA_MASK                        (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR6_ANF_RA_SHIFT                       (0)
#define CODEC_CODEC_IIR6_ANF_BYPASS                         (1 << 25)

// reg_6c0
#define CODEC_CODEC_IIR6_ANF_RAS(n)                         (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR6_ANF_RAS_MASK                       (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR6_ANF_RAS_SHIFT                      (0)

// reg_6c4
#define CODEC_CODEC_IIR6_ANF_ALPHA(n)                       (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR6_ANF_ALPHA_MASK                     (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR6_ANF_ALPHA_SHIFT                    (0)

// reg_6c8
#define CODEC_CODEC_IIR6_ANF_DELTA(n)                       (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR6_ANF_DELTA_MASK                     (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR6_ANF_DELTA_SHIFT                    (0)

// reg_6cc
#define CODEC_CODEC_IIR6_ANF_FLT_TH1(n)                     (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR6_ANF_FLT_TH1_MASK                   (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR6_ANF_FLT_TH1_SHIFT                  (0)

// reg_6d0
#define CODEC_CODEC_IIR6_ANF_FLT_TH2(n)                     (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR6_ANF_FLT_TH2_MASK                   (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR6_ANF_FLT_TH2_SHIFT                  (0)

// reg_6d4
#define CODEC_CODEC_IIR6_ANF_VAR_TH(n)                      (((n) & 0x1FFFFFF) << 0)
#define CODEC_CODEC_IIR6_ANF_VAR_TH_MASK                    (0x1FFFFFF << 0)
#define CODEC_CODEC_IIR6_ANF_VAR_TH_SHIFT                   (0)

// reg_6d8
#define CODEC_PSAP_ANF_RA(n)                                (((n) & 0x1FFFFFF) << 0)
#define CODEC_PSAP_ANF_RA_MASK                              (0x1FFFFFF << 0)
#define CODEC_PSAP_ANF_RA_SHIFT                             (0)

// reg_6dc
#define CODEC_PSAP_ANF_RAS(n)                               (((n) & 0x1FFFFFF) << 0)
#define CODEC_PSAP_ANF_RAS_MASK                             (0x1FFFFFF << 0)
#define CODEC_PSAP_ANF_RAS_SHIFT                            (0)

// reg_6e4
#define CODEC_PSAP_ANF_ALPHA(n)                             (((n) & 0x1FFFFFF) << 0)
#define CODEC_PSAP_ANF_ALPHA_MASK                           (0x1FFFFFF << 0)
#define CODEC_PSAP_ANF_ALPHA_SHIFT                          (0)

// reg_6e8
#define CODEC_PSAP_ANF_DELTA(n)                             (((n) & 0x1FFFFFF) << 0)
#define CODEC_PSAP_ANF_DELTA_MASK                           (0x1FFFFFF << 0)
#define CODEC_PSAP_ANF_DELTA_SHIFT                          (0)

// reg_6ec
#define CODEC_PSAP_ANF_FLT_TH1(n)                           (((n) & 0x1FFFFFF) << 0)
#define CODEC_PSAP_ANF_FLT_TH1_MASK                         (0x1FFFFFF << 0)
#define CODEC_PSAP_ANF_FLT_TH1_SHIFT                        (0)

// reg_6f0
#define CODEC_PSAP_ANF_FLT_TH2(n)                           (((n) & 0x1FFFFFF) << 0)
#define CODEC_PSAP_ANF_FLT_TH2_MASK                         (0x1FFFFFF << 0)
#define CODEC_PSAP_ANF_FLT_TH2_SHIFT                        (0)

// reg_6f4
#define CODEC_PSAP_ANF_VAR_TH(n)                            (((n) & 0x1FFFFFF) << 0)
#define CODEC_PSAP_ANF_VAR_TH_MASK                          (0x1FFFFFF << 0)
#define CODEC_PSAP_ANF_VAR_TH_SHIFT                         (0)

// reg_700
#define CODEC_ADC_SND_ENABLE                                (1 << 0)
#define CODEC_ADC_SND_ENABLE_CH0                            (1 << 1)
#define CODEC_ADC_SND_ENABLE_CH1                            (1 << 2)
#define CODEC_ADC_SND_ENABLE_CH2                            (1 << 3)
#define CODEC_ADC_SND_ENABLE_CH3                            (1 << 4)
#define CODEC_ADC_SND_ENABLE_CH4                            (1 << 5)
#define CODEC_ADC_SND_ENABLE_CH5                            (1 << 6)
#define CODEC_DAC_ENABLE_SND                                (1 << 7)
#define CODEC_DAC_ENABLE_TRI                                (1 << 8)
#define CODEC_DMACTRL_RX_SND                                (1 << 9)
#define CODEC_DMACTRL_TX_SND                                (1 << 10)
#define CODEC_DMACTRL_TX_TRI                                (1 << 11)

// reg_704
#define CODEC_RX_FIFO_FLUSH_SND_CH0                         (1 << 0)
#define CODEC_RX_FIFO_FLUSH_SND_CH1                         (1 << 1)
#define CODEC_RX_FIFO_FLUSH_SND_CH2                         (1 << 2)
#define CODEC_RX_FIFO_FLUSH_SND_CH3                         (1 << 3)
#define CODEC_RX_FIFO_FLUSH_SND_CH4                         (1 << 4)
#define CODEC_RX_FIFO_FLUSH_SND_CH5                         (1 << 5)
#define CODEC_TX_FIFO_FLUSH_SND                             (1 << 6)
#define CODEC_TX_FIFO_FLUSH_TRI                             (1 << 7)

// reg_708
#define CODEC_CODEC_RX_THRESHOLD_SND(n)                     (((n) & 0xF) << 0)
#define CODEC_CODEC_RX_THRESHOLD_SND_MASK                   (0xF << 0)
#define CODEC_CODEC_RX_THRESHOLD_SND_SHIFT                  (0)
#define CODEC_CODEC_TX_THRESHOLD_SND(n)                     (((n) & 0xF) << 4)
#define CODEC_CODEC_TX_THRESHOLD_SND_MASK                   (0xF << 4)
#define CODEC_CODEC_TX_THRESHOLD_SND_SHIFT                  (4)
#define CODEC_CODEC_TX_THRESHOLD_TRI(n)                     (((n) & 0xF) << 8)
#define CODEC_CODEC_TX_THRESHOLD_TRI_MASK                   (0xF << 8)
#define CODEC_CODEC_TX_THRESHOLD_TRI_SHIFT                  (8)

// reg_70c
#define CODEC_CODEC_RX_OVERFLOW_SND(n)                      (((n) & 0xFF) << 0)
#define CODEC_CODEC_RX_OVERFLOW_SND_MASK                    (0xFF << 0)
#define CODEC_CODEC_RX_OVERFLOW_SND_SHIFT                   (0)
#define CODEC_CODEC_RX_UNDERFLOW_SND(n)                     (((n) & 0xFF) << 8)
#define CODEC_CODEC_RX_UNDERFLOW_SND_MASK                   (0xFF << 8)
#define CODEC_CODEC_RX_UNDERFLOW_SND_SHIFT                  (8)
#define CODEC_CODEC_TX_OVERFLOW_SND                         (1 << 16)
#define CODEC_CODEC_TX_UNDERFLOW_SND                        (1 << 17)
#define CODEC_CODEC_TX_OVERFLOW_TRI                         (1 << 18)
#define CODEC_CODEC_TX_UNDERFLOW_TRI                        (1 << 19)
#define CODEC_BT_TRIGGER                                    (1 << 20)
#define CODEC_BT_TRIGGER1                                   (1 << 21)
#define CODEC_BT_TRIGGER2                                   (1 << 22)
#define CODEC_BT_TRIGGER3                                   (1 << 23)
#define CODEC_RX_I2S_OVERFLOW                               (1 << 24)
#define CODEC_RX_I2S_UNDERFLOW                              (1 << 25)
#define CODEC_TX_I2S_OVERFLOW                               (1 << 26)
#define CODEC_TX_I2S_UNDERFLOW                              (1 << 27)
#define CODEC_RX_I2S_FIFO_TRIGGER                           (1 << 28)
#define CODEC_TX_I2S_FIFO_TRIGGER                           (1 << 29)
#define CODEC_CODEC_PSAP_ANF_DONE_CH0                       (1 << 30)

// reg_710
#define CODEC_CODEC_RX_OVERFLOW_SND_MSK(n)                 (((n) & 0xFF) << 0)
#define CODEC_CODEC_RX_OVERFLOW_SND_MSK_MASK               (0xFF << 0)
#define CODEC_CODEC_RX_OVERFLOW_SND_MSK_SHIFT              (0)
#define CODEC_CODEC_RX_UNDERFLOW_SND_MSK(n)                (((n) & 0xFF) << 8)
#define CODEC_CODEC_RX_UNDERFLOW_SND_MSK_MASK              (0xFF << 8)
#define CODEC_CODEC_RX_UNDERFLOW_SND_MSK_SHIFT             (8)
#define CODEC_CODEC_TX_OVERFLOW_SND_MASK                    (1 << 16)
#define CODEC_CODEC_TX_UNDERFLOW_SND_MASK                   (1 << 17)
#define CODEC_CODEC_TX_OVERFLOW_TRI_MASK                    (1 << 18)
#define CODEC_CODEC_TX_UNDERFLOW_TRI_MASK                   (1 << 19)
#define CODEC_BT_TRIGGER_MASK                               (1 << 20)
#define CODEC_BT_TRIGGER1_MASK                              (1 << 21)
#define CODEC_BT_TRIGGER2_MASK                              (1 << 22)
#define CODEC_BT_TRIGGER3_MASK                              (1 << 23)
#define CODEC_RX_I2S_OVERFLOW_MASK                          (1 << 24)
#define CODEC_RX_I2S_UNDERFLOW_MASK                         (1 << 25)
#define CODEC_TX_I2S_OVERFLOW_MASK                          (1 << 26)
#define CODEC_TX_I2S_UNDERFLOW_MASK                         (1 << 27)
#define CODEC_RX_I2S_FIFO_TRIGGER_MASK                      (1 << 28)
#define CODEC_TX_I2S_FIFO_TRIGGER_MASK                      (1 << 29)
#define CODEC_CODEC_PSAP_ANF_DONE_CH0_MASK                  (1 << 30)

// reg_714
#define CODEC_FIFO_COUNT_SND_CH0(n)                         (((n) & 0xF) << 0)
#define CODEC_FIFO_COUNT_SND_CH0_MASK                       (0xF << 0)
#define CODEC_FIFO_COUNT_SND_CH0_SHIFT                      (0)
#define CODEC_FIFO_COUNT_SND_CH1(n)                         (((n) & 0xF) << 4)
#define CODEC_FIFO_COUNT_SND_CH1_MASK                       (0xF << 4)
#define CODEC_FIFO_COUNT_SND_CH1_SHIFT                      (4)
#define CODEC_FIFO_COUNT_SND_CH2(n)                         (((n) & 0xF) << 8)
#define CODEC_FIFO_COUNT_SND_CH2_MASK                       (0xF << 8)
#define CODEC_FIFO_COUNT_SND_CH2_SHIFT                      (8)
#define CODEC_FIFO_COUNT_SND_CH3(n)                         (((n) & 0xF) << 12)
#define CODEC_FIFO_COUNT_SND_CH3_MASK                       (0xF << 12)
#define CODEC_FIFO_COUNT_SND_CH3_SHIFT                      (12)
#define CODEC_FIFO_COUNT_SND_CH4(n)                         (((n) & 0xF) << 16)
#define CODEC_FIFO_COUNT_SND_CH4_MASK                       (0xF << 16)
#define CODEC_FIFO_COUNT_SND_CH4_SHIFT                      (16)
#define CODEC_FIFO_COUNT_SND_CH5(n)                         (((n) & 0xF) << 20)
#define CODEC_FIFO_COUNT_SND_CH5_MASK                       (0xF << 20)
#define CODEC_FIFO_COUNT_SND_CH5_SHIFT                      (20)

// reg_718
#define CODEC_FIFO_COUNT_TX_SND(n)                          (((n) & 0xF) << 0)
#define CODEC_FIFO_COUNT_TX_SND_MASK                        (0xF << 0)
#define CODEC_FIFO_COUNT_TX_SND_SHIFT                       (0)
#define CODEC_FIFO_COUNT_TX_TRI(n)                          (((n) & 0xF) << 4)
#define CODEC_FIFO_COUNT_TX_TRI_MASK                        (0xF << 4)
#define CODEC_FIFO_COUNT_TX_TRI_SHIFT                       (4)
#define CODEC_STATE_RX_CH_SND(n)                            (((n) & 0x1FF) << 8)
#define CODEC_STATE_RX_CH_SND_MASK                          (0x1FF << 8)
#define CODEC_STATE_RX_CH_SND_SHIFT                         (8)
#define CODEC_FIFO_COUNT_RX_I2S(n)                          (((n) & 0x1F) << 17)
#define CODEC_FIFO_COUNT_RX_I2S_MASK                        (0x1F << 17)
#define CODEC_FIFO_COUNT_RX_I2S_SHIFT                       (17)
#define CODEC_FIFO_COUNT_TX_I2S(n)                          (((n) & 0x1F) << 22)
#define CODEC_FIFO_COUNT_TX_I2S_MASK                        (0x1F << 22)
#define CODEC_FIFO_COUNT_TX_I2S_SHIFT                       (22)

// reg_71c
#define CODEC_RX_FIFO_DATA_SND(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_SND_MASK                         (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SND_SHIFT                        (0)

// reg_720
#define CODEC_RX_FIFO_DATA_SND(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_SND_MASK                         (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SND_SHIFT                        (0)

// reg_724
#define CODEC_RX_FIFO_DATA_SND(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_SND_MASK                         (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SND_SHIFT                        (0)

// reg_728
#define CODEC_RX_FIFO_DATA_SND(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_SND_MASK                         (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SND_SHIFT                        (0)

// reg_72c
#define CODEC_RX_FIFO_DATA_SND(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_SND_MASK                         (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SND_SHIFT                        (0)

// reg_730
#define CODEC_RX_FIFO_DATA_SND(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_SND_MASK                         (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SND_SHIFT                        (0)

// reg_734
#define CODEC_RX_FIFO_DATA_SND(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_SND_MASK                         (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SND_SHIFT                        (0)

// reg_738
#define CODEC_RX_FIFO_DATA_SND(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_SND_MASK                         (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SND_SHIFT                        (0)

// reg_73c
#define CODEC_RX_FIFO_DATA_SND(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_RX_FIFO_DATA_SND_MASK                         (0xFFFFFFFF << 0)
#define CODEC_RX_FIFO_DATA_SND_SHIFT                        (0)

// reg_740
#define CODEC_TX_FIFO_DATA_TRI(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_TX_FIFO_DATA_TRI_MASK                         (0xFFFFFFFF << 0)
#define CODEC_TX_FIFO_DATA_TRI_SHIFT                        (0)

// reg_744
#define CODEC_MODE_16BIT_ADC_SND                            (1 << 0)
#define CODEC_MODE_24BIT_ADC_SND                            (1 << 1)
#define CODEC_MODE_32BIT_ADC_SND                            (1 << 2)
#define CODEC_DUAL_CHANNEL_DAC_SND                          (1 << 3)
#define CODEC_DAC_EXCHANGE_L_R_SND                          (1 << 4)
#define CODEC_MODE_16BIT_DAC_SND                            (1 << 5)
#define CODEC_MODE_32BIT_DAC_SND                            (1 << 6)
#define CODEC_DUAL_CHANNEL_DAC_TRI                          (1 << 7)
#define CODEC_DAC_EXCHANGE_L_R_TRI                          (1 << 8)
#define CODEC_MODE_16BIT_DAC_TRI                            (1 << 9)
#define CODEC_MODE_32BIT_DAC_TRI                            (1 << 10)
#define CODEC_ADC_SND_ENABLE_PLAYTIME_STAMP_SEL(n)          (((n) & 0x7) << 11)
#define CODEC_ADC_SND_ENABLE_PLAYTIME_STAMP_SEL_MASK        (0x7 << 11)
#define CODEC_ADC_SND_ENABLE_PLAYTIME_STAMP_SEL_SHIFT       (11)
#define CODEC_RESAMPLE_ADC_SND_ENABLE_PLAYTIME_STAMP_SEL(n) (((n) & 0x7) << 14)
#define CODEC_RESAMPLE_ADC_SND_ENABLE_PLAYTIME_STAMP_SEL_MASK (0x7 << 14)
#define CODEC_RESAMPLE_ADC_SND_ENABLE_PLAYTIME_STAMP_SEL_SHIFT (14)
#define CODEC_RESAMPLE_ADC_SND_PHASE_PLAYTIME_STAMP_SEL(n)  (((n) & 0x7) << 17)
#define CODEC_RESAMPLE_ADC_SND_PHASE_PLAYTIME_STAMP_SEL_MASK (0x7 << 17)
#define CODEC_RESAMPLE_ADC_SND_PHASE_PLAYTIME_STAMP_SEL_SHIFT (17)

// reg_748
#define CODEC_ADC_SND_ENABLE_SEL(n)                         (((n) & 0x3) << 0)
#define CODEC_ADC_SND_ENABLE_SEL_MASK                       (0x3 << 0)
#define CODEC_ADC_SND_ENABLE_SEL_SHIFT                      (0)
#define CODEC_CODEC_RESAMPLE_ADC_SND_UPDATE_TRIGGER_SEL(n)  (((n) & 0x3) << 2)
#define CODEC_CODEC_RESAMPLE_ADC_SND_UPDATE_TRIGGER_SEL_MASK (0x3 << 2)
#define CODEC_CODEC_RESAMPLE_ADC_SND_UPDATE_TRIGGER_SEL_SHIFT (2)
#define CODEC_CODEC_RESAMPLE_ADC_SND_ENABLE_TRIGGER_SEL(n)  (((n) & 0x3) << 4)
#define CODEC_CODEC_RESAMPLE_ADC_SND_ENABLE_TRIGGER_SEL_MASK (0x3 << 4)
#define CODEC_CODEC_RESAMPLE_ADC_SND_ENABLE_TRIGGER_SEL_SHIFT (4)
#define CODEC_DAC_ENABLE_SEL_SND(n)                         (((n) & 0x3) << 6)
#define CODEC_DAC_ENABLE_SEL_SND_MASK                       (0x3 << 6)
#define CODEC_DAC_ENABLE_SEL_SND_SHIFT                      (6)
#define CODEC_CODEC_DAC_ENABLE_SND_SEL(n)                   (((n) & 0x3) << 8)
#define CODEC_CODEC_DAC_ENABLE_SND_SEL_MASK                 (0x3 << 8)
#define CODEC_CODEC_DAC_ENABLE_SND_SEL_SHIFT                (8)
#define CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_SND(n)  (((n) & 0x3) << 10)
#define CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_SND_MASK (0x3 << 10)
#define CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_SND_SHIFT (10)
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE_TRIGGER_SEL_SND(n)  (((n) & 0x3) << 12)
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE_TRIGGER_SEL_SND_MASK (0x3 << 12)
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE_TRIGGER_SEL_SND_SHIFT (12)
#define CODEC_DAC_ENABLE_SEL_TRI(n)                         (((n) & 0x3) << 14)
#define CODEC_DAC_ENABLE_SEL_TRI_MASK                       (0x3 << 14)
#define CODEC_DAC_ENABLE_SEL_TRI_SHIFT                      (14)
#define CODEC_CODEC_DAC_ENABLE_TRI_SEL(n)                   (((n) & 0x3) << 16)
#define CODEC_CODEC_DAC_ENABLE_TRI_SEL_MASK                 (0x3 << 16)
#define CODEC_CODEC_DAC_ENABLE_TRI_SEL_SHIFT                (16)
#define CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_TRI(n)  (((n) & 0x3) << 18)
#define CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_TRI_MASK (0x3 << 18)
#define CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL_TRI_SHIFT (18)
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE_TRIGGER_SEL_TRI(n)  (((n) & 0x3) << 20)
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE_TRIGGER_SEL_TRI_MASK (0x3 << 20)
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE_TRIGGER_SEL_TRI_SHIFT (20)
#define CODEC_FAULT_MUTE_DAC_ENABLE_SND                     (1 << 22)
#define CODEC_FAULT_MUTE_DAC_ENABLE_TRI                     (1 << 23)

// reg_74c
#define CODEC_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL(n)          (((n) & 0x7) << 0)
#define CODEC_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL_MASK        (0x7 << 0)
#define CODEC_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL_SHIFT       (0)
#define CODEC_CODEC_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL(n)    (((n) & 0x7) << 3)
#define CODEC_CODEC_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL_MASK  (0x7 << 3)
#define CODEC_CODEC_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL_SHIFT (3)
#define CODEC_RESAMPLE_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL(n) (((n) & 0x7) << 6)
#define CODEC_RESAMPLE_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL_MASK (0x7 << 6)
#define CODEC_RESAMPLE_DAC_ENABLE_SND_PLAYTIME_STAMP_SEL_SHIFT (6)
#define CODEC_RESAMPLE_DAC_PHASE_SND_PLAYTIME_STAMP_SEL(n)  (((n) & 0x7) << 9)
#define CODEC_RESAMPLE_DAC_PHASE_SND_PLAYTIME_STAMP_SEL_MASK (0x7 << 9)
#define CODEC_RESAMPLE_DAC_PHASE_SND_PLAYTIME_STAMP_SEL_SHIFT (9)
#define CODEC_CODEC_DAC_GAIN_SND_PLAYTIME_STAMP_SEL(n)      (((n) & 0x7) << 12)
#define CODEC_CODEC_DAC_GAIN_SND_PLAYTIME_STAMP_SEL_MASK    (0x7 << 12)
#define CODEC_CODEC_DAC_GAIN_SND_PLAYTIME_STAMP_SEL_SHIFT   (12)
#define CODEC_DAC_ENABLE_TRI_PLAYTIME_STAMP_SEL(n)          (((n) & 0x7) << 15)
#define CODEC_DAC_ENABLE_TRI_PLAYTIME_STAMP_SEL_MASK        (0x7 << 15)
#define CODEC_DAC_ENABLE_TRI_PLAYTIME_STAMP_SEL_SHIFT       (15)
#define CODEC_CODEC_DAC_ENABLE_TRI_PLAYTIME_STAMP_SEL(n)    (((n) & 0x7) << 18)
#define CODEC_CODEC_DAC_ENABLE_TRI_PLAYTIME_STAMP_SEL_MASK  (0x7 << 18)
#define CODEC_CODEC_DAC_ENABLE_TRI_PLAYTIME_STAMP_SEL_SHIFT (18)
#define CODEC_RESAMPLE_DAC_ENABLE_TRI_PLAYTIME_STAMP_SEL(n) (((n) & 0x7) << 21)
#define CODEC_RESAMPLE_DAC_ENABLE_TRI_PLAYTIME_STAMP_SEL_MASK (0x7 << 21)
#define CODEC_RESAMPLE_DAC_ENABLE_TRI_PLAYTIME_STAMP_SEL_SHIFT (21)
#define CODEC_RESAMPLE_DAC_PHASE_TRI_PLAYTIME_STAMP_SEL(n)  (((n) & 0x7) << 24)
#define CODEC_RESAMPLE_DAC_PHASE_TRI_PLAYTIME_STAMP_SEL_MASK (0x7 << 24)
#define CODEC_RESAMPLE_DAC_PHASE_TRI_PLAYTIME_STAMP_SEL_SHIFT (24)
#define CODEC_CODEC_DAC_GAIN_TRI_PLAYTIME_STAMP_SEL(n)      (((n) & 0x7) << 27)
#define CODEC_CODEC_DAC_GAIN_TRI_PLAYTIME_STAMP_SEL_MASK    (0x7 << 27)
#define CODEC_CODEC_DAC_GAIN_TRI_PLAYTIME_STAMP_SEL_SHIFT   (27)

// reg_750
#define CODEC_ADC_I2S_ENABLE                                (1 << 0)
#define CODEC_ADC_I2S_ENABLE_CH0                            (1 << 1)
#define CODEC_ADC_I2S_ENABLE_CH1                            (1 << 2)
#define CODEC_ADC_I2S_ENABLE_CH2                            (1 << 3)
#define CODEC_ADC_I2S_ENABLE_CH3                            (1 << 4)
#define CODEC_ADC_I2S_ENABLE_CH4                            (1 << 5)
#define CODEC_ADC_I2S_ENABLE_CH5                            (1 << 6)
#define CODEC_DAC_I2S_ENABLE                                (1 << 7)
#define CODEC_CODEC2I2S_DATA_MODE                           (1 << 8)
#define CODEC_I2S2CODEC_DATA_MODE(n)                        (((n) & 0x3) << 9)
#define CODEC_I2S2CODEC_DATA_MODE_MASK                      (0x3 << 9)
#define CODEC_I2S2CODEC_DATA_MODE_SHIFT                     (9)
#define CODEC_ADC_I2S_VALID_MODE                            (1 << 11)
#define CODEC_ADC_I2S_TDM_MODE                              (1 << 12)
#define CODEC_ADC_I2S_TDM_SLOT(n)                           (((n) & 0x3) << 13)
#define CODEC_ADC_I2S_TDM_SLOT_MASK                         (0x3 << 13)
#define CODEC_ADC_I2S_TDM_SLOT_SHIFT                        (13)
#define CODEC_ADC_I2S_ENABLE_SEL(n)                         (((n) & 0x3) << 15)
#define CODEC_ADC_I2S_ENABLE_SEL_MASK                       (0x3 << 15)
#define CODEC_ADC_I2S_ENABLE_SEL_SHIFT                      (15)
#define CODEC_DAC_I2S_ENABLE_SEL(n)                         (((n) & 0x3) << 17)
#define CODEC_DAC_I2S_ENABLE_SEL_MASK                       (0x3 << 17)
#define CODEC_DAC_I2S_ENABLE_SEL_SHIFT                      (17)
#define CODEC_DAC_I2S_DATA_SEL                              (1 << 19)
#define CODEC_ADC_I2S_ENABLE_PLAYTIME_STAMP_SEL(n)          (((n) & 0x7) << 20)
#define CODEC_ADC_I2S_ENABLE_PLAYTIME_STAMP_SEL_MASK        (0x7 << 20)
#define CODEC_ADC_I2S_ENABLE_PLAYTIME_STAMP_SEL_SHIFT       (20)
#define CODEC_DAC_I2S_ENABLE_PLAYTIME_STAMP_SEL(n)          (((n) & 0x7) << 23)
#define CODEC_DAC_I2S_ENABLE_PLAYTIME_STAMP_SEL_MASK        (0x7 << 23)
#define CODEC_DAC_I2S_ENABLE_PLAYTIME_STAMP_SEL_SHIFT       (23)

// reg_754
#define CODEC_RX_I2S_FIFO_FLUSH                             (1 << 0)
#define CODEC_TX_I2S_FIFO_FLUSH                             (1 << 1)
#define CODEC_RX_I2S_FIFO_FCNT_INI(n)                       (((n) & 0x1F) << 2)
#define CODEC_RX_I2S_FIFO_FCNT_INI_MASK                     (0x1F << 2)
#define CODEC_RX_I2S_FIFO_FCNT_INI_SHIFT                    (2)
#define CODEC_RX_I2S_FIFO_TH_HIGH(n)                        (((n) & 0x1F) << 7)
#define CODEC_RX_I2S_FIFO_TH_HIGH_MASK                      (0x1F << 7)
#define CODEC_RX_I2S_FIFO_TH_HIGH_SHIFT                     (7)
#define CODEC_RX_I2S_FIFO_TH_LOW(n)                         (((n) & 0x1F) << 12)
#define CODEC_RX_I2S_FIFO_TH_LOW_MASK                       (0x1F << 12)
#define CODEC_RX_I2S_FIFO_TH_LOW_SHIFT                      (12)
#define CODEC_TX_I2S_FIFO_FCNT_INI(n)                       (((n) & 0x1F) << 17)
#define CODEC_TX_I2S_FIFO_FCNT_INI_MASK                     (0x1F << 17)
#define CODEC_TX_I2S_FIFO_FCNT_INI_SHIFT                    (17)
#define CODEC_TX_I2S_FIFO_TH_HIGH(n)                        (((n) & 0x1F) << 22)
#define CODEC_TX_I2S_FIFO_TH_HIGH_MASK                      (0x1F << 22)
#define CODEC_TX_I2S_FIFO_TH_HIGH_SHIFT                     (22)
#define CODEC_TX_I2S_FIFO_TH_LOW(n)                         (((n) & 0x1F) << 27)
#define CODEC_TX_I2S_FIFO_TH_LOW_MASK                       (0x1F << 27)
#define CODEC_TX_I2S_FIFO_TH_LOW_SHIFT                      (27)

// reg_758
#define CODEC_ADC_FIFO_OVERFLOW(n)                          (((n) & 0x3F) << 0)
#define CODEC_ADC_FIFO_OVERFLOW_MASK                        (0x3F << 0)
#define CODEC_ADC_FIFO_OVERFLOW_SHIFT                       (0)
#define CODEC_ADC_FIFO_UNDERFLOW(n)                         (((n) & 0x3F) << 6)
#define CODEC_ADC_FIFO_UNDERFLOW_MASK                       (0x3F << 6)
#define CODEC_ADC_FIFO_UNDERFLOW_SHIFT                      (6)
#define CODEC_CI2S_ADC_FIFO_OVERFLOW(n)                     (((n) & 0x3F) << 12)
#define CODEC_CI2S_ADC_FIFO_OVERFLOW_MASK                   (0x3F << 12)
#define CODEC_CI2S_ADC_FIFO_OVERFLOW_SHIFT                  (12)
#define CODEC_CI2S_ADC_FIFO_UNDERFLOW(n)                    (((n) & 0x3F) << 18)
#define CODEC_CI2S_ADC_FIFO_UNDERFLOW_MASK                  (0x3F << 18)
#define CODEC_CI2S_ADC_FIFO_UNDERFLOW_SHIFT                 (18)
#define CODEC_CI2S_DAC_FIFO_OVERFLOW(n)                     (((n) & 0x3) << 24)
#define CODEC_CI2S_DAC_FIFO_OVERFLOW_MASK                   (0x3 << 24)
#define CODEC_CI2S_DAC_FIFO_OVERFLOW_SHIFT                  (24)
#define CODEC_CI2S_DAC_FIFO_UNDERFLOW(n)                    (((n) & 0x3) << 26)
#define CODEC_CI2S_DAC_FIFO_UNDERFLOW_MASK                  (0x3 << 26)
#define CODEC_CI2S_DAC_FIFO_UNDERFLOW_SHIFT                 (26)
#define CODEC_CODEC_IIR0_ANF_DONE                           (1 << 28)
#define CODEC_CODEC_IIR2_ANF_DONE                           (1 << 29)
#define CODEC_CODEC_IIR4_ANF_DONE                           (1 << 30)
#define CODEC_CODEC_IIR6_ANF_DONE                           (1 << 31)

// reg_75c
#define CODEC_ADC_FIFO_OVERFLOW_MSK(n)                     (((n) & 0x3F) << 0)
#define CODEC_ADC_FIFO_OVERFLOW_MSK_MASK                   (0x3F << 0)
#define CODEC_ADC_FIFO_OVERFLOW_MSK_SHIFT                  (0)
#define CODEC_ADC_FIFO_UNDERFLOW_MSK(n)                    (((n) & 0x3F) << 6)
#define CODEC_ADC_FIFO_UNDERFLOW_MSK_MASK                  (0x3F << 6)
#define CODEC_ADC_FIFO_UNDERFLOW_MSK_SHIFT                 (6)
#define CODEC_CI2S_ADC_FIFO_OVERFLOW_MSK(n)                (((n) & 0x3F) << 12)
#define CODEC_CI2S_ADC_FIFO_OVERFLOW_MSK_MASK              (0x3F << 12)
#define CODEC_CI2S_ADC_FIFO_OVERFLOW_MSK_SHIFT             (12)
#define CODEC_CI2S_ADC_FIFO_UNDERFLOW_MSK(n)               (((n) & 0x3F) << 18)
#define CODEC_CI2S_ADC_FIFO_UNDERFLOW_MSK_MASK             (0x3F << 18)
#define CODEC_CI2S_ADC_FIFO_UNDERFLOW_MSK_SHIFT            (18)
#define CODEC_CI2S_DAC_FIFO_OVERFLOW_MSK(n)                (((n) & 0x3) << 24)
#define CODEC_CI2S_DAC_FIFO_OVERFLOW_MSK_MASK              (0x3 << 24)
#define CODEC_CI2S_DAC_FIFO_OVERFLOW_MSK_SHIFT             (24)
#define CODEC_CI2S_DAC_FIFO_UNDERFLOW_MSK(n)               (((n) & 0x3) << 26)
#define CODEC_CI2S_DAC_FIFO_UNDERFLOW_MSK_MASK             (0x3 << 26)
#define CODEC_CI2S_DAC_FIFO_UNDERFLOW_MSK_SHIFT            (26)
#define CODEC_CODEC_IIR0_ANF_DONE_MASK                      (1 << 28)
#define CODEC_CODEC_IIR2_ANF_DONE_MASK                      (1 << 29)
#define CODEC_CODEC_IIR4_ANF_DONE_MASK                      (1 << 30)
#define CODEC_CODEC_IIR6_ANF_DONE_MASK                      (1 << 31)

// reg_760
#define CODEC_CFG_DIV_CI2S(n)                               (((n) & 0x1FFF) << 0)
#define CODEC_CFG_DIV_CI2S_MASK                             (0x1FFF << 0)
#define CODEC_CFG_DIV_CI2S_SHIFT                            (0)
#define CODEC_BYPASS_DIV_CI2S                               (1 << 13)
#define CODEC_SEL_CI2S_OSC                                  (1 << 14)
#define CODEC_SEL_CI2S_OSCX2                                (1 << 15)
#define CODEC_POL_CLK_CI2S_IN                               (1 << 16)
#define CODEC_SEL_CI2S_CLKIN                                (1 << 17)
#define CODEC_EN_CLK_CI2S                                   (1 << 18)
#define CODEC_EN_CLK_CI2S_OUT                               (1 << 19)
#define CODEC_POL_CLK_CI2S_OUT                              (1 << 20)
#define CODEC_SOFT_RSTN_CI2S                                (1 << 21)
#define CODEC_CODEC_IO_DRV(n)                               (((n) & 0xF) << 22)
#define CODEC_CODEC_IO_DRV_MASK                             (0xF << 22)
#define CODEC_CODEC_IO_DRV_SHIFT                            (22)

// reg_780
#define CODEC_CODEC_ADC_SND_EN_CH0                          (1 << 0)
#define CODEC_CODEC_ADC_SND_EN_CH1                          (1 << 1)
#define CODEC_CODEC_ADC_SND_EN_CH2                          (1 << 2)
#define CODEC_CODEC_ADC_SND_EN_CH3                          (1 << 3)
#define CODEC_CODEC_EC_SND_EN_CH0                           (1 << 4)
#define CODEC_CODEC_EC_SND_EN_CH1                           (1 << 5)
#define CODEC_CODEC_ADC_DATA_SEL_SND_CH0(n)                 (((n) & 0x7) << 6)
#define CODEC_CODEC_ADC_DATA_SEL_SND_CH0_MASK               (0x7 << 6)
#define CODEC_CODEC_ADC_DATA_SEL_SND_CH0_SHIFT              (6)
#define CODEC_CODEC_ADC_DATA_SEL_SND_CH1(n)                 (((n) & 0x7) << 9)
#define CODEC_CODEC_ADC_DATA_SEL_SND_CH1_MASK               (0x7 << 9)
#define CODEC_CODEC_ADC_DATA_SEL_SND_CH1_SHIFT              (9)
#define CODEC_CODEC_ADC_DATA_SEL_SND_CH2(n)                 (((n) & 0x7) << 12)
#define CODEC_CODEC_ADC_DATA_SEL_SND_CH2_MASK               (0x7 << 12)
#define CODEC_CODEC_ADC_DATA_SEL_SND_CH2_SHIFT              (12)
#define CODEC_CODEC_ADC_DATA_SEL_SND_CH3(n)                 (((n) & 0x7) << 15)
#define CODEC_CODEC_ADC_DATA_SEL_SND_CH3_MASK               (0x7 << 15)
#define CODEC_CODEC_ADC_DATA_SEL_SND_CH3_SHIFT              (15)
#define CODEC_CODEC_ADC_RATE_SEL_SND(n)                     (((n) & 0x7) << 18)
#define CODEC_CODEC_ADC_RATE_SEL_SND_MASK                   (0x7 << 18)
#define CODEC_CODEC_ADC_RATE_SEL_SND_SHIFT                  (18)

// reg_784
#define CODEC_CODEC_DAC_EN_SND                              (1 << 0)
#define CODEC_CODEC_DAC_EN_SND_CH0                          (1 << 1)
#define CODEC_CODEC_DAC_EN_SND_CH1                          (1 << 2)
#define CODEC_CODEC_DAC_HBF3_BYPASS_SND                     (1 << 3)
#define CODEC_CODEC_DAC_HBF2_BYPASS_SND                     (1 << 4)
#define CODEC_CODEC_DAC_HBF1_BYPASS_SND                     (1 << 5)
#define CODEC_CODEC_DAC_UP_SEL_SND(n)                       (((n) & 0x7) << 6)
#define CODEC_CODEC_DAC_UP_SEL_SND_MASK                     (0x7 << 6)
#define CODEC_CODEC_DAC_UP_SEL_SND_SHIFT                    (6)
#define CODEC_CODEC_DAC_HBF4_DELAY_SEL_SND                  (1 << 9)
#define CODEC_CODEC_DAC_SND_ALIGN_MODE                      (1 << 10)
#define CODEC_CODEC_DAC_TONE_TEST_SND                       (1 << 11)
#define CODEC_CODEC_DAC_SND_MIX_SEL_CH0(n)                  (((n) & 0x7) << 12)
#define CODEC_CODEC_DAC_SND_MIX_SEL_CH0_MASK                (0x7 << 12)
#define CODEC_CODEC_DAC_SND_MIX_SEL_CH0_SHIFT               (12)
#define CODEC_CODEC_DAC_SND_MIX_SEL_CH1(n)                  (((n) & 0x7) << 15)
#define CODEC_CODEC_DAC_SND_MIX_SEL_CH1_MASK                (0x7 << 15)
#define CODEC_CODEC_DAC_SND_MIX_SEL_CH1_SHIFT               (15)
#define CODEC_CODEC_DAC_LR_SWAP_SND                         (1 << 18)
#define CODEC_CODEC_DAC_SIN1K_SEL(n)                        (((n) & 0x3) << 19)
#define CODEC_CODEC_DAC_SIN1K_SEL_MASK                      (0x3 << 19)
#define CODEC_CODEC_DAC_SIN1K_SEL_SHIFT                     (19)

// reg_788
#define CODEC_CODEC_DAC_GAIN_SND_CH0(n)                     (((n) & 0xFFFFF) << 0)
#define CODEC_CODEC_DAC_GAIN_SND_CH0_MASK                   (0xFFFFF << 0)
#define CODEC_CODEC_DAC_GAIN_SND_CH0_SHIFT                  (0)
#define CODEC_CODEC_DAC_GAIN_SEL_SND_CH0                    (1 << 20)
#define CODEC_CODEC_DAC_GAIN_UPDATE_SND                     (1 << 21)
#define CODEC_CODEC_DAC_GAIN_TRIGGER_SEL_SND(n)             (((n) & 0x3) << 22)
#define CODEC_CODEC_DAC_GAIN_TRIGGER_SEL_SND_MASK           (0x3 << 22)
#define CODEC_CODEC_DAC_GAIN_TRIGGER_SEL_SND_SHIFT          (22)

// reg_78c
#define CODEC_CODEC_DAC_GAIN_SND_CH1(n)                     (((n) & 0xFFFFF) << 0)
#define CODEC_CODEC_DAC_GAIN_SND_CH1_MASK                   (0xFFFFF << 0)
#define CODEC_CODEC_DAC_GAIN_SND_CH1_SHIFT                  (0)
#define CODEC_CODEC_DAC_GAIN_SEL_SND_CH1                    (1 << 20)

// reg_790
#define CODEC_CODEC_RAMP_STEP_SND_CH0(n)                    (((n) & 0xFFF) << 0)
#define CODEC_CODEC_RAMP_STEP_SND_CH0_MASK                  (0xFFF << 0)
#define CODEC_CODEC_RAMP_STEP_SND_CH0_SHIFT                 (0)
#define CODEC_CODEC_RAMP_EN_SND_CH0                         (1 << 12)
#define CODEC_CODEC_RAMP_INTERVAL_SND_CH0(n)                (((n) & 0x7) << 13)
#define CODEC_CODEC_RAMP_INTERVAL_SND_CH0_MASK              (0x7 << 13)
#define CODEC_CODEC_RAMP_INTERVAL_SND_CH0_SHIFT             (13)

// reg_794
#define CODEC_CODEC_RAMP_STEP_SND_CH1(n)                    (((n) & 0xFFF) << 0)
#define CODEC_CODEC_RAMP_STEP_SND_CH1_MASK                  (0xFFF << 0)
#define CODEC_CODEC_RAMP_STEP_SND_CH1_SHIFT                 (0)
#define CODEC_CODEC_RAMP_EN_SND_CH1                         (1 << 12)
#define CODEC_CODEC_RAMP_INTERVAL_SND_CH1(n)                (((n) & 0x7) << 13)
#define CODEC_CODEC_RAMP_INTERVAL_SND_CH1_MASK              (0x7 << 13)
#define CODEC_CODEC_RAMP_INTERVAL_SND_CH1_SHIFT             (13)

// reg_798
#define CODEC_CODEC_DAC_EN_TRI                              (1 << 0)
#define CODEC_CODEC_DAC_EN_TRI_CH0                          (1 << 1)
#define CODEC_CODEC_DAC_EN_TRI_CH1                          (1 << 2)
#define CODEC_CODEC_DAC_HBF3_BYPASS_TRI                     (1 << 3)
#define CODEC_CODEC_DAC_HBF2_BYPASS_TRI                     (1 << 4)
#define CODEC_CODEC_DAC_HBF1_BYPASS_TRI                     (1 << 5)
#define CODEC_CODEC_DAC_UP_SEL_TRI(n)                       (((n) & 0x7) << 6)
#define CODEC_CODEC_DAC_UP_SEL_TRI_MASK                     (0x7 << 6)
#define CODEC_CODEC_DAC_UP_SEL_TRI_SHIFT                    (6)
#define CODEC_CODEC_DAC_HBF4_DELAY_SEL_TRI                  (1 << 9)
#define CODEC_CODEC_DAC_TRI_ALIGN_MODE                      (1 << 10)
#define CODEC_CODEC_DAC_TONE_TEST_TRI                       (1 << 11)
#define CODEC_CODEC_DAC_LR_SWAP_TRI                         (1 << 12)
#define CODEC_CODEC_DAC_TRI_MIX_SEL_CH0(n)                  (((n) & 0x7) << 13)
#define CODEC_CODEC_DAC_TRI_MIX_SEL_CH0_MASK                (0x7 << 13)
#define CODEC_CODEC_DAC_TRI_MIX_SEL_CH0_SHIFT               (13)
#define CODEC_CODEC_DAC_TRI_MIX_SEL_CH1(n)                  (((n) & 0x7) << 16)
#define CODEC_CODEC_DAC_TRI_MIX_SEL_CH1_MASK                (0x7 << 16)
#define CODEC_CODEC_DAC_TRI_MIX_SEL_CH1_SHIFT               (16)

// reg_79c
#define CODEC_CODEC_DAC_GAIN_TRI_CH0(n)                     (((n) & 0xFFFFF) << 0)
#define CODEC_CODEC_DAC_GAIN_TRI_CH0_MASK                   (0xFFFFF << 0)
#define CODEC_CODEC_DAC_GAIN_TRI_CH0_SHIFT                  (0)
#define CODEC_CODEC_DAC_GAIN_SEL_TRI_CH0                    (1 << 20)
#define CODEC_CODEC_DAC_GAIN_UPDATE_TRI                     (1 << 21)
#define CODEC_CODEC_DAC_GAIN_TRIGGER_SEL_TRI(n)             (((n) & 0x3) << 22)
#define CODEC_CODEC_DAC_GAIN_TRIGGER_SEL_TRI_MASK           (0x3 << 22)
#define CODEC_CODEC_DAC_GAIN_TRIGGER_SEL_TRI_SHIFT          (22)

// reg_7a0
#define CODEC_CODEC_DAC_GAIN_TRI_CH1(n)                     (((n) & 0xFFFFF) << 0)
#define CODEC_CODEC_DAC_GAIN_TRI_CH1_MASK                   (0xFFFFF << 0)
#define CODEC_CODEC_DAC_GAIN_TRI_CH1_SHIFT                  (0)
#define CODEC_CODEC_DAC_GAIN_SEL_TRI_CH1                    (1 << 20)

// reg_7a8
#define CODEC_CODEC_RAMP_STEP_TRI_CH0(n)                    (((n) & 0xFFF) << 0)
#define CODEC_CODEC_RAMP_STEP_TRI_CH0_MASK                  (0xFFF << 0)
#define CODEC_CODEC_RAMP_STEP_TRI_CH0_SHIFT                 (0)
#define CODEC_CODEC_RAMP_EN_TRI_CH0                         (1 << 12)
#define CODEC_CODEC_RAMP_INTERVAL_TRI_CH0(n)                (((n) & 0x7) << 13)
#define CODEC_CODEC_RAMP_INTERVAL_TRI_CH0_MASK              (0x7 << 13)
#define CODEC_CODEC_RAMP_INTERVAL_TRI_CH0_SHIFT             (13)

// reg_7ac
#define CODEC_CODEC_RAMP_STEP_TRI_CH1(n)                    (((n) & 0xFFF) << 0)
#define CODEC_CODEC_RAMP_STEP_TRI_CH1_MASK                  (0xFFF << 0)
#define CODEC_CODEC_RAMP_STEP_TRI_CH1_SHIFT                 (0)
#define CODEC_CODEC_RAMP_EN_TRI_CH1                         (1 << 12)
#define CODEC_CODEC_RAMP_INTERVAL_TRI_CH1(n)                (((n) & 0x7) << 13)
#define CODEC_CODEC_RAMP_INTERVAL_TRI_CH1_MASK              (0x7 << 13)
#define CODEC_CODEC_RAMP_INTERVAL_TRI_CH1_SHIFT             (13)

// reg_7b0
#define CODEC_CODEC_ADC_SND_DSF_ENABLE                      (1 << 0)
#define CODEC_CODEC_ADC_SND_DSF_ORDER(n)                    (((n) & 0xFF) << 1)
#define CODEC_CODEC_ADC_SND_DSF_ORDER_MASK                  (0xFF << 1)
#define CODEC_CODEC_ADC_SND_DSF_ORDER_SHIFT                 (1)
#define CODEC_CODEC_ADC_SND_DSF_SAMPLE_START(n)             (((n) & 0xFF) << 9)
#define CODEC_CODEC_ADC_SND_DSF_SAMPLE_START_MASK           (0xFF << 9)
#define CODEC_CODEC_ADC_SND_DSF_SAMPLE_START_SHIFT          (9)
#define CODEC_CODEC_ADC_SND_DSF_CH_ENABLE(n)                (((n) & 0xFF) << 17)
#define CODEC_CODEC_ADC_SND_DSF_CH_ENABLE_MASK              (0xFF << 17)
#define CODEC_CODEC_ADC_SND_DSF_CH_ENABLE_SHIFT             (17)
#define CODEC_CODEC_DOWN_SEL_FIR_SND(n)                     (((n) & 0x7) << 25)
#define CODEC_CODEC_DOWN_SEL_FIR_SND_MASK                   (0x7 << 25)
#define CODEC_CODEC_DOWN_SEL_FIR_SND_SHIFT                  (25)

// reg_7b4
#define CODEC_CODEC_DAC_SND_USF_ENABLE                      (1 << 0)
#define CODEC_CODEC_DAC_SND_USF_ORDER(n)                    (((n) & 0xFF) << 1)
#define CODEC_CODEC_DAC_SND_USF_ORDER_MASK                  (0xFF << 1)
#define CODEC_CODEC_DAC_SND_USF_ORDER_SHIFT                 (1)
#define CODEC_CODEC_DAC_SND_USF_SAMPLE_START(n)             (((n) & 0xFF) << 9)
#define CODEC_CODEC_DAC_SND_USF_SAMPLE_START_MASK           (0xFF << 9)
#define CODEC_CODEC_DAC_SND_USF_SAMPLE_START_SHIFT          (9)
#define CODEC_CODEC_DAC_SND_USF_UPSAMPLE                    (1 << 17)
#define CODEC_CODEC_DAC_SND_USF_UPSAMPLE_MODE               (1 << 18)
#define CODEC_CODEC_DAC_SND_USF_UP_RATIO(n)                 (((n) & 0xF) << 19)
#define CODEC_CODEC_DAC_SND_USF_UP_RATIO_MASK               (0xF << 19)
#define CODEC_CODEC_DAC_SND_USF_UP_RATIO_SHIFT              (19)
#define CODEC_CODEC_DAC_SND_USF_CH_ENABLE(n)                (((n) & 0x3) << 23)
#define CODEC_CODEC_DAC_SND_USF_CH_ENABLE_MASK              (0x3 << 23)
#define CODEC_CODEC_DAC_SND_USF_CH_ENABLE_SHIFT             (23)
#define CODEC_CODEC_UP_SEL_FIR_SND(n)                       (((n) & 0x7) << 25)
#define CODEC_CODEC_UP_SEL_FIR_SND_MASK                     (0x7 << 25)
#define CODEC_CODEC_UP_SEL_FIR_SND_SHIFT                    (25)

// reg_7b8
#define CODEC_CODEC_DAC_TRI_USF_ENABLE                      (1 << 0)
#define CODEC_CODEC_DAC_TRI_USF_ORDER(n)                    (((n) & 0xFF) << 1)
#define CODEC_CODEC_DAC_TRI_USF_ORDER_MASK                  (0xFF << 1)
#define CODEC_CODEC_DAC_TRI_USF_ORDER_SHIFT                 (1)
#define CODEC_CODEC_DAC_TRI_USF_SAMPLE_START(n)             (((n) & 0xFF) << 9)
#define CODEC_CODEC_DAC_TRI_USF_SAMPLE_START_MASK           (0xFF << 9)
#define CODEC_CODEC_DAC_TRI_USF_SAMPLE_START_SHIFT          (9)
#define CODEC_CODEC_DAC_TRI_USF_UPSAMPLE                    (1 << 17)
#define CODEC_CODEC_DAC_TRI_USF_UPSAMPLE_MODE               (1 << 18)
#define CODEC_CODEC_DAC_TRI_USF_UP_RATIO(n)                 (((n) & 0xF) << 19)
#define CODEC_CODEC_DAC_TRI_USF_UP_RATIO_MASK               (0xF << 19)
#define CODEC_CODEC_DAC_TRI_USF_UP_RATIO_SHIFT              (19)
#define CODEC_CODEC_DAC_TRI_USF_CH_ENABLE(n)                (((n) & 0x3) << 23)
#define CODEC_CODEC_DAC_TRI_USF_CH_ENABLE_MASK              (0x3 << 23)
#define CODEC_CODEC_DAC_TRI_USF_CH_ENABLE_SHIFT             (23)
#define CODEC_CODEC_UP_SEL_FIR_TRI(n)                       (((n) & 0x7) << 25)
#define CODEC_CODEC_UP_SEL_FIR_TRI_MASK                     (0x7 << 25)
#define CODEC_CODEC_UP_SEL_FIR_TRI_SHIFT                    (25)

// reg_7c0
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE_SND                 (1 << 0)
#define CODEC_CODEC_RESAMPLE_DAC_L_ENABLE_SND               (1 << 1)
#define CODEC_CODEC_RESAMPLE_DAC_R_ENABLE_SND               (1 << 2)
#define CODEC_CODEC_RESAMPLE_DAC_FIFO_ENABLE_SND            (1 << 3)
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_SND           (1 << 4)
#define CODEC_CODEC_RESAMPLE_DAC_FIFO_DELAY_SND(n)          (((n) & 0x7) << 5)
#define CODEC_CODEC_RESAMPLE_DAC_FIFO_DELAY_SND_MASK        (0x7 << 5)
#define CODEC_CODEC_RESAMPLE_DAC_FIFO_DELAY_SND_SHIFT       (5)
#define CODEC_CODEC_RESAMPLE_DAC_ENABLE_TRI                 (1 << 8)
#define CODEC_CODEC_RESAMPLE_DAC_L_ENABLE_TRI               (1 << 9)
#define CODEC_CODEC_RESAMPLE_DAC_R_ENABLE_TRI               (1 << 10)
#define CODEC_CODEC_RESAMPLE_DAC_FIFO_ENABLE_TRI            (1 << 11)
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE_TRI           (1 << 12)
#define CODEC_CODEC_RESAMPLE_DAC_FIFO_DELAY_TRI(n)          (((n) & 0x7) << 13)
#define CODEC_CODEC_RESAMPLE_DAC_FIFO_DELAY_TRI_MASK        (0x7 << 13)
#define CODEC_CODEC_RESAMPLE_DAC_FIFO_DELAY_TRI_SHIFT       (13)
#define CODEC_CODEC_RESAMPLE_ADC_SND_ENABLE                 (1 << 16)
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH_CNT(n)              (((n) & 0x7) << 17)
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH_CNT_MASK            (0x7 << 17)
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH_CNT_SHIFT           (17)
#define CODEC_CODEC_RESAMPLE_ADC_SND_PHASE_UPDATE           (1 << 20)
#define CODEC_CODEC_ADC_SND_REMAP_ENABLE                    (1 << 21)

// reg_7c4
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH0_SEL(n)             (((n) & 0xF) << 0)
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH0_SEL_MASK           (0xF << 0)
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH0_SEL_SHIFT          (0)
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH1_SEL(n)             (((n) & 0xF) << 4)
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH1_SEL_MASK           (0xF << 4)
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH1_SEL_SHIFT          (4)
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH2_SEL(n)             (((n) & 0xF) << 8)
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH2_SEL_MASK           (0xF << 8)
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH2_SEL_SHIFT          (8)
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH3_SEL(n)             (((n) & 0xF) << 12)
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH3_SEL_MASK           (0xF << 12)
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH3_SEL_SHIFT          (12)
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH4_SEL(n)             (((n) & 0xF) << 16)
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH4_SEL_MASK           (0xF << 16)
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH4_SEL_SHIFT          (16)
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH5_SEL(n)             (((n) & 0xF) << 20)
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH5_SEL_MASK           (0xF << 20)
#define CODEC_CODEC_RESAMPLE_ADC_SND_CH5_SEL_SHIFT          (20)

// reg_7c8
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_INC_SND(n)           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_INC_SND_MASK         (0xFFFFFFFF << 0)
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_INC_SND_SHIFT        (0)

// reg_7cc
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_INC_TRI(n)           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_INC_TRI_MASK         (0xFFFFFFFF << 0)
#define CODEC_CODEC_RESAMPLE_DAC_PHASE_INC_TRI_SHIFT        (0)

// reg_7d0
#define CODEC_CODEC_RESAMPLE_ADC_SND_PHASE_INC(n)           (((n) & 0xFFFFFFFF) << 0)
#define CODEC_CODEC_RESAMPLE_ADC_SND_PHASE_INC_MASK         (0xFFFFFFFF << 0)
#define CODEC_CODEC_RESAMPLE_ADC_SND_PHASE_INC_SHIFT        (0)

// reg_7e0
#define CODEC_PSAP_CPD_GL_08(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_GL_08_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_GL_08_SHIFT                          (0)
#define CODEC_PSAP_CPD_GL_09(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_GL_09_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_GL_09_SHIFT                          (16)

// reg_7e4
#define CODEC_PSAP_CPD_GL_10(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_GL_10_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_GL_10_SHIFT                          (0)
#define CODEC_PSAP_CPD_GL_11(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_GL_11_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_GL_11_SHIFT                          (16)

// reg_7e8
#define CODEC_PSAP_CPD_GL_12(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_GL_12_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_GL_12_SHIFT                          (0)
#define CODEC_PSAP_CPD_GL_13(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_GL_13_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_GL_13_SHIFT                          (16)

// reg_7ec
#define CODEC_PSAP_CPD_GL_14(n)                             (((n) & 0xFFFF) << 0)
#define CODEC_PSAP_CPD_GL_14_MASK                           (0xFFFF << 0)
#define CODEC_PSAP_CPD_GL_14_SHIFT                          (0)
#define CODEC_PSAP_CPD_GL_15(n)                             (((n) & 0xFFFF) << 16)
#define CODEC_PSAP_CPD_GL_15_MASK                           (0xFFFF << 16)
#define CODEC_PSAP_CPD_GL_15_SHIFT                          (16)

// reg_7f0
#define CODEC_CI2S_RATE_SEL(n)                              (((n) & 0x7) << 0)
#define CODEC_CI2S_RATE_SEL_MASK                            (0x7 << 0)
#define CODEC_CI2S_RATE_SEL_SHIFT                           (0)
#define CODEC_EN_CI2S_PHASE_SEL(n)                          (((n) & 0x3) << 3)
#define CODEC_EN_CI2S_PHASE_SEL_MASK                        (0x3 << 3)
#define CODEC_EN_CI2S_PHASE_SEL_SHIFT                       (3)
#define CODEC_CI2S_DAC_OUT_SWAP                             (1 << 5)
#define CODEC_CI2S_ADC_FIFO_FLUSH_CH0                       (1 << 6)
#define CODEC_CI2S_ADC_FIFO_FLUSH_CH1                       (1 << 7)
#define CODEC_CI2S_ADC_FIFO_FLUSH_CH2                       (1 << 8)
#define CODEC_CI2S_ADC_FIFO_FLUSH_CH3                       (1 << 9)
#define CODEC_CI2S_ADC_FIFO_THRESHOLD(n)                    (((n) & 0xF) << 10)
#define CODEC_CI2S_ADC_FIFO_THRESHOLD_MASK                  (0xF << 10)
#define CODEC_CI2S_ADC_FIFO_THRESHOLD_SHIFT                 (10)
#define CODEC_CI2S_ADC_IN_SEL_CH0(n)                        (((n) & 0x7) << 14)
#define CODEC_CI2S_ADC_IN_SEL_CH0_MASK                      (0x7 << 14)
#define CODEC_CI2S_ADC_IN_SEL_CH0_SHIFT                     (14)
#define CODEC_CI2S_ADC_IN_SEL_CH1(n)                        (((n) & 0x7) << 17)
#define CODEC_CI2S_ADC_IN_SEL_CH1_MASK                      (0x7 << 17)
#define CODEC_CI2S_ADC_IN_SEL_CH1_SHIFT                     (17)
#define CODEC_CI2S_ADC_IN_SEL_CH2(n)                        (((n) & 0x7) << 20)
#define CODEC_CI2S_ADC_IN_SEL_CH2_MASK                      (0x7 << 20)
#define CODEC_CI2S_ADC_IN_SEL_CH2_SHIFT                     (20)
#define CODEC_CI2S_ADC_IN_SEL_CH3(n)                        (((n) & 0x7) << 23)
#define CODEC_CI2S_ADC_IN_SEL_CH3_MASK                      (0x7 << 23)
#define CODEC_CI2S_ADC_IN_SEL_CH3_SHIFT                     (23)
#define CODEC_CI2S_DAC_FIFO_FLUSH_CH0                       (1 << 26)
#define CODEC_CI2S_DAC_FIFO_FLUSH_CH1                       (1 << 27)
#define CODEC_CI2S_DAC_FIFO_THRESHOLD(n)                    (((n) & 0xF) << 28)
#define CODEC_CI2S_DAC_FIFO_THRESHOLD_MASK                  (0xF << 28)
#define CODEC_CI2S_DAC_FIFO_THRESHOLD_SHIFT                 (28)

// reg_7f4
#define CODEC_CI2S_EN                                       (1 << 0)
#define CODEC_CI2S_CLK_EN                                   (1 << 1)
#define CODEC_CI2S_TX_EN                                    (1 << 2)
#define CODEC_CI2S_RX_EN                                    (1 << 3)
#define CODEC_CI2S_TXCH_EN(n)                               (((n) & 0xF) << 4)
#define CODEC_CI2S_TXCH_EN_MASK                             (0xF << 4)
#define CODEC_CI2S_TXCH_EN_SHIFT                            (4)
#define CODEC_CI2S_RXCH_EN(n)                               (((n) & 0xF) << 8)
#define CODEC_CI2S_RXCH_EN_MASK                             (0xF << 8)
#define CODEC_CI2S_RXCH_EN_SHIFT                            (8)
#define CODEC_CI2S_CLK_EN_MODE                              (1 << 12)
#define CODEC_CI2S_TX_EN_MODE                               (1 << 13)
#define CODEC_CI2S_RX_EN_MODE                               (1 << 14)
#define CODEC_CI2S_CLK_WORD_SIZE(n)                         (((n) & 0x3) << 15)
#define CODEC_CI2S_CLK_WORD_SIZE_MASK                       (0x3 << 15)
#define CODEC_CI2S_CLK_WORD_SIZE_SHIFT                      (15)
#define CODEC_CI2S_CLK_GATE(n)                              (((n) & 0x7) << 17)
#define CODEC_CI2S_CLK_GATE_MASK                            (0x7 << 17)
#define CODEC_CI2S_CLK_GATE_SHIFT                           (17)
#define CODEC_CI2S_MSTN_SLV_MODE                            (1 << 20)
#define CODEC_CI2S_RX_DM(n)                                 (((n) & 0x7) << 21)
#define CODEC_CI2S_RX_DM_MASK                               (0x7 << 21)
#define CODEC_CI2S_RX_DM_SHIFT                              (21)
#define CODEC_CI2S_TX_DM(n)                                 (((n) & 0x7) << 24)
#define CODEC_CI2S_TX_DM_MASK                               (0x7 << 24)
#define CODEC_CI2S_TX_DM_SHIFT                              (24)
#define CODEC_CI2S_DAC_ENABLE_CH0                           (1 << 27)
#define CODEC_CI2S_DAC_ENABLE_CH1                           (1 << 28)

// reg_7f8
#define CODEC_CI2S_TDM_EN                                   (1 << 0)
#define CODEC_CI2S_TDM_MODE                                 (1 << 1)
#define CODEC_CI2S_TDM_WS_NEGEDGE                           (1 << 2)
#define CODEC_CI2S_TDM_FRAME_WIDTH(n)                       (((n) & 0x7) << 3)
#define CODEC_CI2S_TDM_FRAME_WIDTH_MASK                     (0x7 << 3)
#define CODEC_CI2S_TDM_FRAME_WIDTH_SHIFT                    (3)
#define CODEC_CI2S_TDM_WS_WIDTH(n)                          (((n) & 0x7) << 6)
#define CODEC_CI2S_TDM_WS_WIDTH_MASK                        (0x7 << 6)
#define CODEC_CI2S_TDM_WS_WIDTH_SHIFT                       (6)
#define CODEC_CI2S_TDM_SLOT_WIDTH(n)                        (((n) & 0x3) << 9)
#define CODEC_CI2S_TDM_SLOT_WIDTH_MASK                      (0x3 << 9)
#define CODEC_CI2S_TDM_SLOT_WIDTH_SHIFT                     (9)
#define CODEC_CI2S_TDM_DATA_OFFSET(n)                       (((n) & 0x1F) << 11)
#define CODEC_CI2S_TDM_DATA_OFFSET_MASK                     (0x1F << 11)
#define CODEC_CI2S_TDM_DATA_OFFSET_SHIFT                    (11)
#define CODEC_CI2S_DATA_OFFSET(n)                           (((n) & 0xF) << 16)
#define CODEC_CI2S_DATA_OFFSET_MASK                         (0xF << 16)
#define CODEC_CI2S_DATA_OFFSET_SHIFT                        (16)
#define CODEC_DAC_SINC_RATE_SEL_CH0(n)                      (((n) & 0x3) << 20)
#define CODEC_DAC_SINC_RATE_SEL_CH0_MASK                    (0x3 << 20)
#define CODEC_DAC_SINC_RATE_SEL_CH0_SHIFT                   (20)
#define CODEC_DAC_SINC_RATE_SEL_CH1(n)                      (((n) & 0x3) << 22)
#define CODEC_DAC_SINC_RATE_SEL_CH1_MASK                    (0x3 << 22)
#define CODEC_DAC_SINC_RATE_SEL_CH1_SHIFT                   (22)
#define CODEC_CI2S_TXD_PUSH_SEL                             (1 << 24)

#endif
