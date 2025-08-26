# AirSoft game multi functional device

## Prerequisite
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