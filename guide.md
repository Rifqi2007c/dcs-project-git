# Setup sfml project
- ### vscode (luar vscode)
  - download sfml(MinGW 64-bit) dan compiler (MinGW 64-bit) dari sfml

![alt text](https://github.com/Rifqi2007c/dcs-project/blob/main/readme-asset/3.png)
  - downlaod repo ni
    - download zip atau pakai git: `git clone https://github.com/Rifqi2007c/dcs-project.git`
    - letak folder `dcs-project` dalam user directory. cth `Users\rifqi`
  - unzip sfml dengan compiler, letak dalam dcs-project folder

![alt text](https://github.com/Rifqi2007c/dcs-project/blob/main/readme-asset/1.png)

- ### vscode (dalam vscode)
  - buka folder `dcs-project` dalam vscode (boleh drag n drop)
  - edit `task.json` dalam `.vscode` folder, tukar "rifqi" jadi nama kau ikut dalam `Users\<user>` atau buka powershell untuk tengok username

![alt text](https://github.com/Rifqi2007c/dcs-project/blob/main/readme-asset/4.png) 

> sebelum compile copy asset, paste ke `mingw64\bin`<p>
> for some bs reason, dia tak nak ambik asset dari folder `asset`

masalah ni ade kat windows je, nak betul kan kene pakai visual studio (kalau korang sanggup nak download) atau CMAKE
jadi nanti aku akan try implement cmake script nanti nanti. that`s that, this is this.
