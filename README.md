# Fjernstyring av port i hønsehus

Fjernstyring av port i hønsehus
Dette prosjektet består av en kontrollenhet som styrer en motor som åpner og lukker en port i et hønsehus.

Motoren er koblet til en snorinnretning som drar en port fra åpen til lukket tilstand. Det er en bryter som indikerer at porten er lukket og en som indikerer at porten er åpen

Porten kan åpnes eller lukkes ved enten å trykke på knapp på kontrollboksen for å åpne porten, en anne knapp for å lukke den og en tredje knapp for å stoppe bevegelsen på porten Porten kan også styres via en URL-adresse for å åpne og lukke porten. Dette skjer via MQTT-meldinger fra kontrollboksen til en Mosquitto-server som igjen er konfigurert av json-modul satt opp ved hjelp av Node-red. I tillegg til å lukke, åpne og avbryte bevegelsen for porten, kan kontrollboksen aktiviseres eller utkobles.

Utkobling skjer ved ulike feiltilstander som for eksempel at porten ikke lukkes/åpnes innenfor en timeoutgrense. Node-red-modulen brukes også til ulike konfigurasjoner, som modus for kontaktbryterne, timeoutgrense, hastighet på motor og registrert tid for lukking av porten. I tillegg kan kontrollboksen resettes. I et eget menyvalg kan fire ulike WiFi-tilkoblinger med hver sid SSID og passord. Når kontrollboksen starter, vil den søke etter et av de fire nettverkene som den er konfigurert med å sjekke. Etter at kontakt er oppnådd, vil den ta kontakt med MQTT-serveren. Da er systemet operativt. Dersom kontrollenheten ikke finner noe WiFi-forbindelse, vil den selv lage et accesspunkt som kan tilkobles ved hjelp av WiFi med PC eller telefon. Her kan nytt oppsett for WiFi skrives inn med SSID og passord. Dette lagres i første posisjon for søkende WiFi-forbindelser. Kontrollenheten vil etter denne registreringen, automatisk restarte på dette WiFi-forbindelsen.

Programoppdateringer av kontrollenheten skjer via WiFi fra Arduino IDE, uten at mikrokontrolleren trenger å tilkobles på annen måte.

Kontrollboksen drives av en 12 volt batterieliminator. Motoren er en vanlig børstemotor med planetgir som har maksimal omdreiningstall på 400 o/min ved 12 volt. Kontrollboksen vil imidlertid styre hastigheten av motoren ved hjelp av pulsbreddemodulasjon. Mikrokontrolleren som brukes er en ESP32-basert DOIT ESP32 DEVKIT V1. Den har et OLED LED display for meldinger. Motoren drives at en motorkontroller som gir maks 800mA, Motoren drar max 600mA. Mikrokontrolleren er programmert i Arduino IDE som en Arduin programskisse.
