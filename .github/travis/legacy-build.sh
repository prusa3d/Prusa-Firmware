#!/bin/sh
set -xe
cp Firmware/variants/1_75mm_MK3S-EINSy10a-E3Dv6full.h Firmware/Configuration_prusa.h
bash -x build.sh || { echo "1_75mm_MK3S-EINSy10a-E3Dv6full variant failed" && false; }
bash -x build.sh EN_FARM || { echo "1_75mm_MK3S-EINSy10a-E3Dv6full EN_FARM failed" && false; }
rm Firmware/Configuration_prusa.h
cp Firmware/variants/1_75mm_MK3-EINSy10a-E3Dv6full.h Firmware/Configuration_prusa.h
bash -x build.sh || { echo "1_75mm_MK3-EINSy10a-E3Dv6full variant failed" && false; }
bash -x build.sh EN_FARM || { echo "1_75mm_MK3-EINSy10a-E3Dv6full EN_FARM failed" && false; }
rm Firmware/Configuration_prusa.h
cp Firmware/variants/1_75mm_MK25S-RAMBo13a-E3Dv6full.h Firmware/Configuration_prusa.h
bash -x build.sh || { echo "1_75mm_MK25S-RAMBo13a-E3Dv6full variant failed" && false; }
rm Firmware/Configuration_prusa.h
cp Firmware/variants/1_75mm_MK25S-RAMBo10a-E3Dv6full.h Firmware/Configuration_prusa.h
bash -x build.sh || { echo "1_75mm_MK25S-RAMBo10a-E3Dv6full variant failed" && false; }
rm Firmware/Configuration_prusa.h
cp Firmware/variants/1_75mm_MK25-RAMBo13a-E3Dv6full.h Firmware/Configuration_prusa.h
bash -x build.sh || { echo "1_75mm_MK25-RAMBo13a-E3Dv6full variant failed" && false; }
rm Firmware/Configuration_prusa.h
cp Firmware/variants/1_75mm_MK25-RAMBo10a-E3Dv6full.h Firmware/Configuration_prusa.h
bash -x build.sh || { echo "1_75mm_MK25-RAMBo10a-E3Dv6full variant failed" && false; }
rm Firmware/Configuration_prusa.h
