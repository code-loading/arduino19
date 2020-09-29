// Ehsan Ahmed
// Last edit date:
// 19.02.19
//
// Take average of 5 resistance values and display in kOhm or MOhm.
// And show fill level of the container
// FreqCount.begin(100); 

#include <LiquidCrystal.h>
#include <FreqCount.h>  // The external library which is used to measure the frequency output of the 555
// The library can be downloaded by following the links on the following webpage: 
// https://www.pjrc.com/teensy/td_libs_FreqCount.html#compare

// BEGIN RESISTANCE CODE
#define analogPin      0          
#define chargePin      13 
#define dischargePin   10 
#define capacitanceValue 0.0000000435F
LiquidCrystal lcd(12,11,6,4,3,2);

unsigned long startTime;
unsigned long elapsedTime;
float MegaOhm;                
float KiloOhm;
// END RESISTANCE CODE

// BEGIN FREQUENCY CODE
long sum = 0;
long avrg;
long freq[5];
int size = 5;
// END FREQUENCY CODE

void setup() {
  // put your setup code here, to run once:
  // Start Resistance measurement code:
 pinMode(chargePin, OUTPUT);     
  digitalWrite(chargePin, LOW);  
  // End Resistance measurement code
  lcd.begin(16, 2);
  lcd.clear();
  Serial.begin(57600);
  lcd.setCursor(0, 1);
  lcd.print("R:");

  lcd.setCursor(0, 0);
  lcd.print("FL:");
  
  FreqCount.begin(100); //This tells the Arduino how many milliseconds to read the frequency signal per reading

}

void loop() {
  // put your main code here, to run repeatedly:
  // START FILL LEVEL CODE:
  long *ans = fre();
  long aveg = avg(ans,size);
  fill(aveg);
  // END FILL LEVEL CODE

  // START RESISTANCE CODE:
  float Ohm[6];
  //for loop to charge and discharge cap X times
  for (int j = 0; j<5; j++){
  digitalWrite(chargePin, HIGH);  
  startTime = millis();
  while(analogRead(analogPin) < 648){       
  }

  elapsedTime = millis() - startTime;
  float Res;
  
  Res = (((float)elapsedTime*0.001) / capacitanceValue);   
  
//  Serial.print(elapsedTime);       //can be used to see the time it takes the capacitor to charge to 63%
//  Serial.println(" mS    ");
    
  Ohm[j] = Res;
  

  // START Capacitor discharge code
  digitalWrite(chargePin, LOW);            
  pinMode(dischargePin, OUTPUT);            
  digitalWrite(dischargePin, LOW);          
  while(analogRead(analogPin) > 0){         
  }

  pinMode(dischargePin, INPUT);            
  
  }
  // END Capacitor discharge code
  
  long calc = 0;
  long avg;

  for (int k = 0; k<5; k++){
    calc = calc + Ohm[k]; 

  }

  // calculating size of the Ohm array
  int arraysize = sizeof(Ohm) / sizeof(Ohm[0]);
    avg = calc/(arraysize);
  float R = avg/1000;
// delay can be added here to see resistance values.
    
if(R>=999){
  
  lcd.setCursor(7, 1);
  lcd.print("schlecht");
  lcd.setCursor(2,1);
  long Mega;
  Mega = (long)R/1000;

  Serial.print("Resistance = ");
  Serial.print(Mega);
  Serial.println(" Megaohm");

    if (R < 10){
      //
      lcd.setCursor(2, 1);
      lcd.print("     "); // used to clear the space where the value will be displayed
      lcd.setCursor(2, 1);
      lcd.print(Mega);
      lcd.setCursor(5, 1);
      lcd.print("M");

    }
    else{
      lcd.setCursor(2, 1);
      lcd.print("     "); //used to clear the space where the value will be displayed
      lcd.setCursor(2, 1);
      lcd.print(Mega);
      lcd.setCursor(5, 1);
      lcd.print("M");
      
      }
}
else{ //KiloOhm values: R<1
  long Kilo;

  Kilo = (long)R;

  lcd.setCursor(7, 1);
  lcd.print("gut     ");
  
  lcd.setCursor(2, 1);


  Serial.print("Resistance = ");
  Serial.print(Kilo);
  Serial.println(" Kiloohm");
  
  if(Kilo<100){
    lcd.setCursor(2, 1);
    lcd.print("     ");
    lcd.setCursor(2, 1);
    lcd.print(Kilo);
    lcd.setCursor(5, 1);
    lcd.print("K");
    }
    else{
      lcd.setCursor(2, 1);
      lcd.print("     ");
      lcd.setCursor(2, 1);
      lcd.print(Kilo);
      lcd.setCursor(5, 1);
      lcd.print("K");
      }
}
Serial.println(".");
  //  END RESISTANCE CODE
}

//START Functions for fill level 

// This function reads the frequency frequency which the 555 Timer outputs
long* fre(){
  delay(320); //Without this the first set is 0
  for (int m = 0; m<size; m++){
    if (FreqCount.available()){
      float count = FreqCount.read();
      float correction;
//      correction = count*3.125; //old value. Even with a gate interval of 100 ms the measurement remains accurate and is quicker
      correction = count*10; //In the void setup the FreqCount.begin() was set with 320 milliseconds. 
      // 320 * 3.125 = 1000 ms = 1 second. Whatever value is in the FreqCount.begin(), it needs to be multiplied to equal 1000 ms.
      unsigned long lcorrect;
      lcorrect = (long)correction;
      freq[m] = lcorrect;
      delay(100); //Necessary so that every reading can be done; Should match gate Interval set in setup with FreqCount.begin()
      }
    }
    
    
    
  return freq;
  }
  
  // This function calculates the average frequency after taking 5 readings then displays the fill level
  long avg(long* arr, int size){
    sum = 0;
    for (int i = 0; i<size; i++){
      sum = sum + arr[i];
      }

     avrg = sum/size;
      return avrg;
      
   }

   void fill(long avg){
//      Serial.print("Average f: ");    //This displays the average frequency of 5 readings of the output of the 555-Timer
//      Serial.println(avg);
      
      
      // This code removes the Zu Viel! message which is displayed on the LCD if the fill level exceed 40 cm
      lcd.setCursor(3, 0);
      lcd.print("        ");
      lcd.setCursor(6, 0);
      lcd.print("cm");
      // end of lcd code to remove ZU Viel!
      
      // This is used to set the range of frequencies to correspond to each fill level
      if (avg >= 31000){
        Serial.print("Fill Level: ");
        Serial.println("0");
        lcd.setCursor(3, 0);
        lcd.print("00");
      }else if ((31000 > avg) && (avg >= 27500)){
        Serial.print("Fill Level: ");
        Serial.println("1");
        lcd.setCursor(3, 0);
        lcd.print("01");
      }else if ((27500 > avg) && (avg >= 24000)){
        Serial.print("Fill Level: ");
        Serial.println("2");
        lcd.setCursor(3, 0);
        lcd.print("02");
      }else if ((24000 > avg) && (avg >= 21000)){
        Serial.print("Fill Level: ");
        Serial.println("3");
        lcd.setCursor(3, 0);
        lcd.print("03");
       }else if ((21000 > avg) && (avg >= 19000)){
        Serial.print("Fill Level: ");
        Serial.println("4");
        lcd.setCursor(3, 0);
        lcd.print("04");
       }else if ((19000 > avg) && (avg >= 17000)){
        Serial.print("Fill Level: ");
        Serial.println("5");
        lcd.setCursor(3, 0);
        lcd.print("05");
       }else if ((17000 > avg) && (avg >= 15800)){
        Serial.print("Fill Level: ");
        Serial.println("6");
        lcd.setCursor(3, 0);
        lcd.print("06");
       }else if ((15800 > avg) && (avg >= 14600)){
        Serial.print("Fill Level: ");
        Serial.println("7");
        lcd.setCursor(3, 0);
        lcd.print("07");
       }else if ((14600 > avg) && (avg >= 13200)){
        Serial.print("Fill Level: ");
        Serial.println("8");
        lcd.setCursor(3, 0);
        lcd.print("08");
       }else if ((13200 > avg) && (avg >= 12200)){
        Serial.print("Fill Level: ");
        Serial.println("9");
        lcd.setCursor(3, 0);
        lcd.print("09");
       }else if ((12200 > avg) && (avg >= 11600)){
        Serial.print("Fill Level: ");
        Serial.println("10");
        lcd.setCursor(3, 0);
        lcd.print("10");
       }else if ((11600 > avg) && (avg >= 10800)){
        Serial.print("Fill Level: ");
        Serial.println("11");
        lcd.setCursor(3, 0);
        lcd.print("11");
       }else if ((10800 > avg) && (avg >= 10200)){
        Serial.print("Fill Level: ");
        Serial.println("12");
        lcd.setCursor(3, 0);
        lcd.print("12");
       }else if ((10200 > avg) && (avg >= 9650)){
        Serial.print("Fill Level: ");
        Serial.println("13");
        lcd.setCursor(3, 0);
        lcd.print("13");
       }else if ((9650 > avg) && (avg >= 9300)){
        Serial.print("Fill Level: ");
        Serial.println("14");
        lcd.setCursor(3, 0);
        lcd.print("14");
       }else if ((9300 > avg) && (avg >= 8850)){
        Serial.print("Fill Level: ");
        Serial.println("15");
        lcd.setCursor(3, 0);
        lcd.print("15");
       }else if ((8850 > avg) && (avg >= 8450)){
        Serial.print("Fill Level: ");
        Serial.println("16");
        lcd.setCursor(3, 0);
        lcd.print("16");
       }else if ((8450 > avg) && (avg >= 8100)){
        Serial.print("Fill Level: ");
        Serial.println("17");
        lcd.setCursor(3, 0);
        lcd.print("17");
       }else if ((8100 > avg) && (avg >= 7800)){
        Serial.print("Fill Level: ");
        Serial.println("18");
        lcd.setCursor(3, 0);
        lcd.print("18");
       }else if ((7800 > avg) && (avg >= 7450)){
        Serial.print("Fill Level: ");
        Serial.println("19");
        lcd.setCursor(3, 0);
        lcd.print("19");
       }else if ((7450 > avg) && (avg >= 7250)){
        Serial.print("Fill Level: ");
        Serial.println("20");
        lcd.setCursor(3, 0);
        lcd.print("20");
       }else if ((7250 > avg) && (avg >= 6975)){
        Serial.print("Fill Level: ");
        Serial.println("21");
        lcd.setCursor(3, 0);
        lcd.print("21");
       }else if ((6975 > avg) && (avg >= 6700)){
        Serial.print("Fill Level: ");
        Serial.println("22");
        lcd.setCursor(3, 0);
        lcd.print("22");
       }else if ((6700 > avg) && (avg >= 6500)){
        Serial.print("Fill Level: ");
        Serial.println("23");
        lcd.setCursor(3, 0);
        lcd.print("23");
       }else if ((6500 > avg) && (avg >= 6300)){
        Serial.print("Fill Level: ");
        Serial.println("24");
        lcd.setCursor(3, 0);
        lcd.print("24");
       }else if ((6300 > avg) && (avg >= 6100)){
        Serial.print("Fill Level: ");
        Serial.println("25");
        lcd.setCursor(3, 0);
        lcd.print("25");
       }else if ((6100 > avg) && (avg >= 5900)){
        Serial.print("Fill Level: ");
        Serial.println("26");
        lcd.setCursor(3, 0);
        lcd.print("26");
       }else if ((5900 > avg) && (avg >= 5700)){
        Serial.print("Fill Level: ");
        Serial.println("27");
        lcd.setCursor(3, 0);
        lcd.print("27");
       }else if ((5700 > avg) && (avg >= 5550)){
        Serial.print("Fill Level: ");
        Serial.println("28");
        lcd.setCursor(3, 0);
        lcd.print("28");
       }else if ((5550 > avg) && (avg >= 5375)){
        Serial.print("Fill Level: ");
        Serial.println("29");
        lcd.setCursor(3, 0);
        lcd.print("29");
       }else if ((5375 > avg) && (avg >= 5200)){
        Serial.print("Fill Level: ");
        Serial.println("30");
        lcd.setCursor(3, 0);
        lcd.print("30");
       }else if ((5200 > avg) && (avg >= 5050)){
        Serial.print("Fill Level: ");
        Serial.println("31");
        lcd.setCursor(3, 0);
        lcd.print("31");
       }else if ((5050 > avg) && (avg >= 4900)){
        Serial.print("Fill Level: ");
        Serial.println("32");
        lcd.setCursor(3, 0);
        lcd.print("32");
       }else if ((4900 > avg) && (avg >= 4795)){
        Serial.print("Fill Level: ");
        Serial.println("33");
        lcd.setCursor(3, 0);
        lcd.print("33");
       }else if ((4795 > avg) && (avg >= 4675)){
        Serial.print("Fill Level: ");
        Serial.println("34");
        lcd.setCursor(3, 0);
        lcd.print("34");
       }else if ((4675 > avg) && (avg >= 4550)){
        Serial.print("Fill Level: ");
        Serial.println("35");
        lcd.setCursor(3, 0);
        lcd.print("35");
       }else if ((4550 > avg) && (avg >= 4450)){
        Serial.print("Fill Level: ");
        Serial.println("36");
        lcd.setCursor(3, 0);
        lcd.print("36");
       }else if ((4450 > avg) && (avg >= 4350)){
        Serial.print("Fill Level: ");
        Serial.println("37");
        lcd.setCursor(3, 0);
        lcd.print("37");
       }else if ((4350 > avg) && (avg >= 4275)){
        Serial.print("Fill Level: ");
        Serial.println("38");
        lcd.setCursor(3, 0);
        lcd.print("38");
       }else if ((4275 > avg) && (avg >= 4200)){
        Serial.print("Fill Level: ");
        Serial.println("39");
        lcd.setCursor(3, 0);
        lcd.print("39");
       }else if ((4200 > avg) && (avg >= 4100)){
        Serial.print("Fill Level: ");
        Serial.println("40");
        lcd.setCursor(3, 0);
        lcd.print("40");
       }else if (4100 > avg){
        Serial.println("Warning, overflow imminent!!!");
        lcd.setCursor(3, 0);
        lcd.print("Zu Viel!");
        }
      
      }
      //END Functions for fill level measurement
