# AirSoft game multi functional device

## Hardware prerequisite
1. [Wemos Lolin S3 Pro](https://www.wemos.cc/en/latest/s3/s3_pro.html)
2. [ASG-MFD PCB board](https://oshwlab.com/saskiuhia/asg-mfd) (optional, can be replaced by breadboard)

## Software prerequisite
1. [ESP-IDF Installer](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/windows-setup.html)
2. [NodeJS](https://nodejs.org/en/download/)
3. [VS Code](https://code.visualstudio.com/Download)
4. [ESP-IDF extension](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension)
5. Install Angular CLI
   ``` ps
   npm install -g @angular/cli
   ```
6. Download node_modules
   ``` ps
   cd main_front
   npm install
   ```

## Build & flash procedure
1. Build angular frontend (on first build and after each frontend change)
   ``` ps
   cd main_front
   npm run build
   ```
2. Build ESP binary via ESP-IDF extension
3. Flash ESP binary via ESP-IDF extension