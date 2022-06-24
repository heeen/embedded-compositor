#!/bin/bash
export QT_SCREEN_SCALE_FACTORS=
export QT_AUTO_SCREEN_SCALE_FACTOR=0
../build-compositor-5_15_2-Debug/compositor &
sleep 1
export QT_QPA_PLATFORM=wayland
export QT_WAYLAND_SHELL_INTEGRATION=ivi-shell

QT_IVI_SURFACE_ID=1 kcalc &
QT_IVI_SURFACE_ID=1000 ../build-compositor-5_15_2-Debug/leftclient &
QT_IVI_SURFACE_ID=2000 ../build-compositor-5_15_2-Debug/rightclient &
QT_IVI_SURFACE_ID=3000 ../build-compositor-5_15_2-Debug/topclient &
QT_IVI_SURFACE_ID=4000 ../build-compositor-5_15_2-Debug/bottomclient &
wait


