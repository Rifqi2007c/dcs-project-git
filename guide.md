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


> sebelum compile copy asset, paste ke `mingw64\bin`<p>
> for some bs reason, dia tak nak ambik asset dari folder `asset`

masalah ni ade kat windows je, nak betul kan kene pakai visual studio (kalau korang sanggup nak download) atau CMAKE
jadi nanti aku akan try implement cmake script nanti nanti. that`s that, this is this.
