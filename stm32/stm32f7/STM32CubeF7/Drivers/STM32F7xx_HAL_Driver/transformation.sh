mkdir -p transformed_Inc
mkdir -p fun_implementation

rm transformed_Inc/*
rm fun_implementation/*.c

for F in Inc/*.h; do
	BASENAME_F="$(basename -- $F)"
	echo ST_LL_converter.py -i $F -o transformed_Inc/$BASENAME_F -s fun_implementation/$BASENAME_F.c
	python ST_LL_converter.py -i $F -o transformed_Inc/$BASENAME_F -s fun_implementation/$BASENAME_F.c
	sed -i '1s/^/#include "stm32f7xx_hal.h"\n\n /' fun_implementation/$BASENAME_F.c;
done

rm -v transformed_Inc/*usb* fun_implementation/*usb*
rm -v transformed_Inc/*pcd* fun_implementation/*pcd*
rm -v transformed_Inc/*hcd* fun_implementation/*hcd*
rm -v transformed_Inc/*cortex* fun_implementation/*cortex*
rm -v transformed_Inc/*pwr* fun_implementation/*pwr*
rm -v transformed_Inc/*eth.h* fun_implementation/*eth.h*