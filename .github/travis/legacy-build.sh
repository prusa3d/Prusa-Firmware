#!/bin/sh
set -xe
cp Firmware/variants/MK3S.h Firmware/Configuration_prusa.h
bash -x build.sh || { echo "MK3S variant failed" && false; }
bash -x build.sh EN_FARM || { echo "MK3S EN_FARM failed" && false; }
rm Firmware/Configuration_prusa.h
cp Firmware/variants/MK3.h Firmware/Configuration_prusa.h
bash -x build.sh || { echo "MK3 variant failed" && false; }
bash -x build.sh EN_FARM || { echo "MK3 EN_FARM failed" && false; }
rm Firmware/Configuration_prusa.h
cp Firmware/variants/MK25S-RAMBo13a.h Firmware/Configuration_prusa.h
bash -x build.sh || { echo "MK25S-RAMBo13a variant failed" && false; }
rm Firmware/Configuration_prusa.h
cp Firmware/variants/MK25S-RAMBo10a.h Firmware/Configuration_prusa.h
bash -x build.sh || { echo "MK25S-RAMBo10a variant failed" && false; }
rm Firmware/Configuration_prusa.h
cp Firmware/variants/MK25-RAMBo13a.h Firmware/Configuration_prusa.h
bash -x build.sh || { echo "MK25-RAMBo13a variant failed" && false; }
rm Firmware/Configuration_prusa.h
cp Firmware/variants/MK25-RAMBo10a.h Firmware/Configuration_prusa.h
bash -x build.sh || { echo "MK25-RAMBo10a variant failed" && false; }
rm Firmware/Configuration_prusa.h
cp Firmware/variants/MK3S-E3DREVO.h Firmware/Configuration_prusa.h
bash -x build.sh || { echo "MK3S-E3DREVO variant failed" && false; }
rm Firmware/Configuration_prusa.h
cp Firmware/variants/MK3-E3DREVO.h Firmware/Configuration_prusa.h
bash -x build.sh || { echo "MK3-E3DREVO variant failed" && false; }
rm Firmware/Configuration_prusa.h
cp Firmware/variants/MK3S-E3DREVO_HF_60W.h Firmware/Configuration_prusa.h
bash -x build.sh || { echo "MK3S-E3DREVO_HF_60W variant failed" && false; }
rm Firmware/Configuration_prusa.h
cp Firmware/variants/MK3-E3DREVO_HF_60W.h Firmware/Configuration_prusa.h
bash -x build.sh || { echo "MK3-E3DREVO_HF_60W variant failed" && false; }
rm Firmware/Configuration_prusa.h
