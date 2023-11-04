
ifeq ($(DSP_ALL_TCM_TEST),1)
KBUILD_CPPFLAGS += \
                -DHIFI4_ITCM_SIZE=0x000C0000 \
                -DHIFI4_DTCM_SIZE=0x00140000 \
                -DM55_ITCM_SIZE=0 \
                -DM55_DTCM_SIZE=0

else ifeq ($(M55_ALL_TCM_TEST),1)
KBUILD_CPPFLAGS +=      -DHIFI4_ITCM_SIZE=0 \
                        -DHIFI4_DTCM_SIZE=0 \
                        -DM55_ITCM_SIZE=0x60000 \
                        -DM55_DTCM_SIZE=0x1A0000

else ifeq ($(GRAPHIC_TEST),1)
KBUILD_CPPFLAGS += \
                -DM55_ITCM_SIZE=0x00020000 \
                -DM55_DTCM_SIZE=0x001e0000 \
                -DHIFI4_ITCM_SIZE=0x00000000 \
                -DHIFI4_DTCM_SIZE=0x00000000

else ifeq ($(PT_APP),1)
KBUILD_CPPFLAGS += \
                -DHIFI4_ITCM_SIZE=0x40000 \
                -DHIFI4_ITCM_BASE=0x00880000 \
                -DM55_ITCM_BASE=0x00040000 \
                -DM55_ITCM_SIZE=0x80000 \
                -DM55_DTCM_BASE=0x200C0000 \
                -DM55_DTCM_SIZE=0x00020000 \
                -DHIFI4_DTCM_BASE=0x20800000 \
                -DHIFI4_DTCM_SIZE=0xC0000

else
KBUILD_CPPFLAGS += \
                -DHIFI4_ITCM_SIZE=0x60000 \
                -DHIFI4_DTCM_SIZE=0xC0000 \
                -DM55_ITCM_SIZE=0x40000 \
                -DM55_DTCM_SIZE=0xA0000 \
                -DM55_ITCM_BASE=0x000A0000 \
                -DM55_DTCM_BASE=0x200C0000

endif

