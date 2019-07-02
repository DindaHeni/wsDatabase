//inclue library yang dibutuhkan
#include <LiquidCrystal.h> 
#include <EEPROM.h> 
#define analogInPin  A0  // Analog input pin  
LiquidCrystal lcd(7,8,9,10,11,12); 

//deklarasi variabel dan nilai
int tombol_save=6; 
int sensorValue; 
float outputValueConductivity; 
float outputValueTDS; 
byte cursor_segitiga_kanan[8] = {0b01000,0b01100,0b01110,0b01111,0b01110,0b01100,0b01000,0b00000}; 
byte cursor_segitiga_kiri[8] = {0b00001,0b00011,0b00111,0b01111,0b00111,0b00011,0b00001,0b00000}; 
byte cursor_bulat[8] = {0b00100,0b01110,0b11111,0b11111,0b11111,0b01110,0b00100,0b00000}; 
 
int pembagi_outputValueConductivity=0; //didepan koma 
float hasil_bagi_outputValueConductivity =0; 
float hasil_belakang_koma_outputValueConductivity=0; 
int pembagi_outputValueTDS=0; //didepan koma 
float hasil_bagi_outputValueTDS=0; 
 
float hasil_belakang_koma_outputValueTDS =0;
int pembagi_outputValueConductivity_ribuan ;
int pembagi_outputValueConductivity_puluhan;
int pembagi_outputValueTDS_ribuan;
int pembagi_outputValueTDS_puluhan;
float depan_koma_outputValueConductivity;
float read_belakang_koma_outputValueConductivity;

//defini fungsi
void tampilSave();
void silahkanTunggu();
void bersih2_layar();
void sensor_K();
 
void setup()   //fungsi dijalankan pada saat alat dinyalakan
{ 
  Serial.begin(9600); //mulai komunikasi serial
  lcd.begin(2, 16); 
  lcd.createChar(1, cursor_segitiga_kanan); 
  lcd.createChar(2, cursor_segitiga_kiri); 
  lcd.createChar(3, cursor_bulat); 
  pinMode(tombol_save, INPUT_PULLUP);  //pendeskripsian variabel sebagai INPUT_PULLUP
  silahkanTunggu();   //menjalankan fungsisilahkanTunggu()
} 
 

void loop() {   //fungsi berjalan ketika sensor mendeteksi air
  sensor_K();  
  if(digitalRead(tombol_save)==LOW) {  
    //perhitungan untuk mencari nilai dari konduktivitas dan TDS 
    pembagi_outputValueConductivity=outputValueConductivity;//dari float jadi int, sehingga yg diambil nilai di depan koma (XXXX,00) 
    hasil_bagi_outputValueConductivity = outputValueConductivity - pembagi_outputValueConductivity;  //belakang koma, berasal dari nilai float - nilai depan koma (0000,XX) 
    hasil_belakang_koma_outputValueConductivity = hasil_bagi_outputValueConductivity*100; //nilai dua digit dibelakang koma jadi bulat (0000,XX to XX) 
    
    int pembagi_outputValueConductivity_ribuan = pembagi_outputValueConductivity/100; //ribuan 2 digit (XX00,00) 
    int pembagi_outputValueConductivity_puluhan = pembagi_outputValueConductivity*100; //puluhan 2 digit (00XX,00) 
    
    EEPROM.write(1,pembagi_outputValueConductivity_ribuan); 
    EEPROM.write(2,pembagi_outputValueConductivity_puluhan); 
    EEPROM.write(3,hasil_belakang_koma_outputValueConductivity); 
    pembagi_outputValueTDS = outputValueTDS; 
    hasil_bagi_outputValueTDS = outputValueTDS / pembagi_outputValueTDS; 
    hasil_belakang_koma_outputValueTDS = hasil_bagi_outputValueTDS*100; 
    pembagi_outputValueTDS_ribuan = pembagi_outputValueTDS/100 ; //ribuan 2 digit (XX00,00) 
    pembagi_outputValueTDS_puluhan = pembagi_outputValueTDS*100; //puluhan 2 digit (00XX,00) 
    EEPROM.write(4,pembagi_outputValueTDS_ribuan); 
    EEPROM.write(5,pembagi_outputValueTDS_puluhan); 
    EEPROM.write(6,hasil_belakang_koma_outputValueTDS);     
    tampilSave(); 
   //ketika tombol save ditekan yang kedua kalinya
    while(1) {       
     if(digitalRead(tombol_save)==LOW){ 
        lcd.clear(); 
        lcd.setCursor(0,0); 
        lcd.print("Selesai..."); //tampil pada lcd
        delay(200); 
        break; 
      } 
      else { 
        long int read_depan_koma_outputValueConductivity_ribuan = EEPROM.read(1) ; 
        long int read_depan_koma_outputValueConductivity_puluhan = EEPROM.read(2); 
        int read_belakang_koma_outputValueConductivity = EEPROM.read(3); 
        float depan_koma_outputValueConductivity = (read_depan_koma_outputValueConductivity_ribuan*10000)+(read_depan_koma_outputValueConductivity_puluhan*100) + read_belakang_koma_outputValueConductivity; //depan koma * 100 ditambah dua digit 
        float outputValueConductivity_fix = depan_koma_outputValueConductivity / 100; //dibagi 100 untuk mendapat nilai pecahan dua digit dibelakang koma 
        long int read_depan_koma_outputValueTDS_ribuan = EEPROM.read(4); 
        long int read_depan_koma_outputValueTDS_puluhan = EEPROM.read(5); 
        int read_belakang_koma_outputValueTDS = EEPROM.read(6);  
        float depan_koma_outputValueTDS=(read_depan_koma_outputValueTDS_ribuan*10000)+(read_depan_koma_outputValueTDS_puluhan*100)+read_belakang_koma_outputValueTDS; 
        float outputValueTDS_fix=depan_koma_outputValueTDS/100; 
        Serial.print("K = ");       //ttampil pada serial monitor
        Serial.print(outputValueConductivity_fix); 
        Serial.print("uS/cm");    
        Serial.print("TDS=");       //tampil pada serial monitor
        Serial.print(outputValueTDS_fix); 
        Serial.println("ppm"); 
        lcd.setCursor(0,0); 
        lcd.print("K="); //tampil pada lcd
        lcd.setCursor(2,0); 
        lcd.print(outputValueConductivity_fix); 
        lcd.print("uS/cm");      //tampil pada lcd  
        lcd.setCursor(0,1); 
        lcd.print("TDS=");  //tampil pada lcd
        lcd.setCursor(4,1); 
        lcd.print(outputValueTDS_fix); 
        lcd.print("ppm"); 
        delay(100); 
        lcd.clear(); 
        //delay(100); 
      }  
    } 
  } 
} 
 
void sensor_K() { 
  sensorValue = analogRead(analogInPin); 
  outputValueConductivity = (4.3248*sensorValue)-534; 
  if (sensorValue == 0){  //ketika sensor tidak mendeteksi adanya air
    outputValueConductivity = 0; 
  } 
  outputValueTDS= outputValueConductivity/2; 
  if (sensorValue == 0) { 
    outputValueTDS = 0; 
    //didalam arduino akan menampilkan sesuai perhitungan
    Serial.print("sensor ADC = "); 
    Serial.print(sensorValue); 
    Serial.print("  conductivity (uSiemens) = "); 
    Serial.print(outputValueConductivity); 
    Serial.print("  TDS(ppm) = "); 
    Serial.println(outputValueTDS); 
    lcd.setCursor(0,0); 
    lcd.print("K="); 
    lcd.setCursor(2,0); 
    lcd.print(outputValueConductivity) ; 
    lcd.print("uS/cm");        
    lcd.setCursor(0,1); 
    lcd.print("TDS="); 
    lcd.setCursor(4,1); 
    lcd.print(outputValueTDS); 
    lcd.print("ppm");  
    delay(500); 
  } 
 }

void bersih2_layar() {  //mereset layar agar kembali seperti keadaan awaal
  lcd.setCursor(0,0); 
  lcd.print("                    "); 
  lcd.setCursor(0,1); 
  lcd.print("                    "); 
  lcd.setCursor(0,2); 
  lcd.print("                    "); 
  lcd.setCursor(0,3); 
  lcd.print("                    "); 
  delay(100); 
} 
 
void silahkanTunggu() { //layar akan menampilkan sebuah loading untuk menjalankan perhitungan
  int i; 
  for (i = 0 ; i < 1; i ++){  
    bersih2_layar(); 
    lcd.setCursor(0,0); 
    lcd.print("      ETDS"); 
    for(int b=0;b<16;b++) { 
      lcd.setCursor(b,1); 
      lcd.write(3); 
      delay(80);        
    }  
  } 
  lcd.clear(); 
} 
 
void tampilSave(){ //menyimpan data yang telah keluar di LCD sebelumnya
  lcd.clear(); 
  int i; 
  bersih2_layar(); 
  lcd.setCursor(0,0); 
  lcd.print("      SAVE"); 
  for(int b=0;b<16;b++) { 
    lcd.setCursor(b,1); 
    lcd.write(1); 
    delay(50); 
  } 
  lcd.clear(); 
} 
 
 
