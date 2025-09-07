usbipd bind --busid=5-1
usbipd attach --wsl --busid=5-1

timeout 1

wsl -e bash -c "/home/panagiotis/.pico-sdk/ninja/v1.12.1/ninja -C /home/panagiotis/pico_projects/pico-ds18b20/build"
wsl -e bash -c "echo '1379' | sudo -S /home/panagiotis/.pico-sdk/picotool/2.2.0/picotool/picotool load /home/panagiotis/pico_projects/pico-ds18b20/build/ds18b20.elf -fx"

timeout 2

usbipd attach --wsl --busid=5-1 --auto-attach