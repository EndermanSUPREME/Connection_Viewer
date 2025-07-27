# Connection Viewer

## About
Connection Viewer is a Linux Targeted program that allows a system to view current out-bound connections. For a rich terminal interface this program uses the ncurse library.

## :scroll: Dependenies
```bash
sudo apt update && sudo apt install libncurses5-dev libncursesw5-dev
```

## :hammer: Build
```bash
git clone https://github.com/EndermanSUPREME/Connection_Viewer.git
cd Connection_Viewer
mkdir build
cd build
cmake ../
make
```