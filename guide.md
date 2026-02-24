# Setup sfml project
- ### download dan setup
  - download sfml(MinGW 64-bit) dan compiler (MinGW 64-bit) dari sfml

![alt text](https://github.com/Rifqi2007c/dcs-project/blob/main/readme-asset/3.png)
  - downlaod repo ni
    - download zip atau pakai git: `git clone https://github.com/Rifqi2007c/dcs-project.git`
    - letak folder `dcs-project` dalam user directory. cth `Users\rifqi`
  - unzip sfml dengan compiler, letak dalam C: `drive`
![alt text](https://github.com/Rifqi2007c/dcs-project/blob/main/readme-asset/sfml2.png)

- ### vscode
- buka download C/C++ extention pack
![alt text](https://github.com/Rifqi2007c/dcs-project/blob/main/readme-asset/sfml4.png)

- buka folder `dcs-project` dalam vscode
- buka C/C++ configuration:
  - buka command panel 
    - tekan searh bar dan click `Show and run command` atau ctrl + shift + p
  - pilih C/C++: Edit configuration (UI)
![alt text](https://github.com/Rifqi2007c/dcs-project/blob/main/readme-asset/sfml5.png)

- letak lokasi directory compiler dalam Compiler path
![alt text](https://github.com/Rifqi2007c/dcs-project/blob/main/readme-asset/sfml3.png)

- letak `${workspaceFolder}/**` dan lokasi directory SFML/include dalam Include path
![alt text](https://github.com/Rifqi2007c/dcs-project/blob/main/readme-asset/sfml6.png)

- run code untuk generate task.json dalam .vscode
![alt text](https://github.com/Rifqi2007c/dcs-project/blob/main/readme-asset/sfml7.png)
> kali pertama run code, vscode akan tanya nak guna debugger apa. pilih `C++ (GDB/LLDB)`
![alt text](https://github.com/Rifqi2007c/dcs-project/blob/main/readme-asset/sfml10.png)
![alt text](https://github.com/Rifqi2007c/dcs-project/blob/main/readme-asset/sfml11.png)

> bila run code main sebelum ada task.json vscode akan bagi error. abort
![alt text](https://github.com/Rifqi2007c/dcs-project/blob/main/readme-asset/sfml8.png)

- edit task.json. tambah:
  - sfml/include
  - sfml/lib
  - `-lsfml-graphics`
  - `-lsfml-window`
  - `lsfml-system`
 ![alt text](https://github.com/Rifqi2007c/dcs-project/blob/main/readme-asset/sfml9.png)


> sebelum compile copy asset, paste ke `mingw64\bin`<p>
> for some bs reason, dia tak nak ambik asset dari folder `asset`

masalah ni ade kat windows je, nak betul kan kene pakai visual studio (kalau korang sanggup nak download) atau CMAKE
jadi nanti aku akan try implement cmake script nanti nanti. that`s that, this is this.
