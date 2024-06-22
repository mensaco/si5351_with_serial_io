#include <Adafruit_SI5351.h>
#include <ArduinoJson.h>

Adafruit_SI5351 clockgen = Adafruit_SI5351();

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void) {
  Serial.begin(9600);
  Serial.println("Si5351 Clockgen Test");
  Serial.println("");

  /* Initialise the sensor */
  if (clockgen.begin() != ERROR_NONE) {
    /* There was a problem detecting the IC ... check your connections */
    Serial.print("Ooops, no Si5351 detected ... Check your wiring or I2C ADDR!");
    while (1)
      ;
  }

  Serial.println("OK!");

  // /* INTEGER ONLY MODE --> most accurate output */
  // /* Setup PLLA to integer only mode @ 900MHz (must be 600..900MHz) */
  // /* Set Multisynth 0 to 112.5MHz using integer only mode (div by 4/6/8) */
  // /* 25MHz * 36 = 900 MHz, then 900 MHz / 8 = 112.5 MHz */
  // Serial.println("Set PLLA to 900MHz");
  // clockgen.setupPLLInt(SI5351_PLL_A, 36);
  // Serial.println("Set Output #0 to 112.5MHz");
  // clockgen.setupMultisynthInt(0, SI5351_PLL_A, SI5351_MULTISYNTH_DIV_8);

  // /* FRACTIONAL MODE --> More flexible but introduce clock jitter */
  // /* Setup PLLB to fractional mode @616.66667MHz (XTAL * 24 + 2/3) */
  // /* Setup Multisynth 1 to 13.55311MHz (PLLB/45.5) */
  // clockgen.setupPLL(SI5351_PLL_B, 24, 2, 3);
  // Serial.println("Set Output #1 to 13.553115MHz");
  // clockgen.setupMultisynth(1, SI5351_PLL_B, 45, 1, 2);

  // /* Multisynth 2 is not yet used and won't be enabled, but can be */
  // /* Use PLLB @ 616.66667MHz, then divide by 900 -> 685.185 KHz */
  // /* then divide by 64 for 10.706 KHz */
  // /* configured using either PLL in either integer or fractional mode */

  // Serial.println("Set Output #2 to 10.706 KHz");
  // clockgen.setupMultisynth(2, SI5351_PLL_B, 900, 0, 1);
  // clockgen.setupRdiv(2, SI5351_R_DIV_64);

  // /* Enable the clocks */
  clockgen.enableOutputs(true);
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop(void) {


  String s = Serial.readString();



  /*
    expects a string representation of the following json object (without the comments):
    {
      "output":0, // index of the output [0..2]
      "mode":0, // 0-integer, 1-fractional, 2-multisynth 
      "pll":0,  // 0-PLLA, 1-PLLB
      "mult":27, // 25MHz multiplier [24..36]
      "multNum":0, // multiplier numerator [0..1048575]
      "multDen":1, // multiplier denominator [1..1048575]
      "div": 6, // integer divider (preferably even number)
      "divNum": 0,  // divider numerator [0..1048575]
      "divDen": 1,  // divider denominator [1..1048575]
      "rDivPow2":0, // power of 2 - R divider [0..7]
      "resOutputs": 1   // reset outputs on frequency change 0-false, 1-true
    }

    for example to get 8kHz on output 0, enter:
    {   
      "output":0,   
      "mode":2,   
      "pll":0,   
      "mult":27,   
      "multNum":3049,   
      "multDen":3125,   
      "div": 1366,   
      "divNum": 0,   
      "divDen": 1,   
      "rDivPow2":6,   
      "resPll": 1 
      }

  */

  delay(200);
  if (s != "") {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, s);

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    int output = doc["output"];
    int mode = doc["mode"];
    int pll = doc["pll"];

    int mult = doc["mult"];
    int multNum = doc["multNum"];
    int multDen = doc["multDen"];

    int div = doc["div"];
    int divNum = doc["divNum"];
    int divDen = doc["divDen"];

    int rDivPow2 = doc["rDivPow2"];
    int resOutputs = doc["resOutputs"];

    if (output < 0 || output > 2) {
      Serial.println("OUTPUT value " + String(output) + " is out of obligatory range [0..2]. Aborting.");
      return;
    }

    if (mode < 0 || mode > 2) {
      Serial.println("MODE value " + String(mode) + " is out of obligatory range [0..2]. Aborting.");
      return;
    }

    if (mult < 24 || mult > 36) {
      Serial.println("MULT value " + String(mult) + " is out of recommended range [24..36]. Result unpredictable.");
    }

    if (pll < 0 || pll > 1) {
      Serial.println("PLL value " + String(pll) + " is out of obligatory range [0,1]. Aborting.");
      return;
    }

    if (multNum < 0 || multNum > 1048575) {
      Serial.println("MULTDEN value " + String(multNum) + " is out of obligatory range [0,1048575]. Aborting.");
      return;
    }

    if (divNum < 0 || divNum > 1048575) {
      Serial.println("DIVDEN value " + String(divNum) + " is out of obligatory range [0,1048575]. Aborting.");
      return;
    }

    if (multDen < 1 || multDen > 1048575) {
      Serial.println("MULTDEN value " + String(multDen) + " is out of obligatory range [1,1048575]. Aborting.");
      return;
    }

    if (div < 1) {
      Serial.println("DIV value " + String(div) + " can't be 0 or negative. Aborting.");
      return;
    }

    if (divDen < 1 || divDen > 1048575) {
      Serial.println("DIVDEN value " + String(divDen) + " is out of obligatory range [1,1048575]. Aborting");
      return;
    }


    if (rDivPow2 < 0 || rDivPow2 > 7) {
      Serial.println("RDIVPOW2 value " + String(rDivPow2) + " is out of obligatory range [0,7]. Aborting.");
      return;
    }

    if (resOutputs < 0 || resOutputs > 1) {
      Serial.println("RESOUTPUTS value " + String(resOutputs) + " is out of obligatory range [0,1]. Aborting.");
      return;
    }


    if (resOutputs == 1) {
      /* Disable the clocks */
      clockgen.enableOutputs(false);
    }

    String plls = "PLLA";
    if (pll == 1) {
      plls = "PLLB";
    }

    if (mode == 0) {

      /* INTEGER ONLY MODE --> most accurate output */
      Serial.println("Set " + plls + " to " + String(25 * mult) + "MHz");
      clockgen.setupPLLInt((si5351PLL_t)pll, mult);
      Serial.println("Using " + plls + " set output #" + String(output) + " to " + String(25 * mult / (double)div) + "MHz");
      clockgen.setupMultisynthInt(output, (si5351PLL_t)pll, (si5351MultisynthDiv_t)div);
    }

    if (mode == 1 || mode == 2) {
      /* FRACTIONAL MODE --> More flexible but introduce clock jitter */
      clockgen.setupPLL((si5351PLL_t)pll, mult, multNum, multDen);
      Serial.println("Using " + plls + " set output #" + String(output) + " to " + String(25.0 * (double)(mult + multNum / (double)multDen) / (double)(div + divNum / (double)divDen)) + "MHz");
      clockgen.setupMultisynth(output, (si5351PLL_t)pll, div, divNum, divDen);
    }

    if (mode == 2) {
      // Multisynth
      Serial.println("Set Output #" + String(output) + " to " + String(1024.0 * 25.0 * (double)(mult + multNum / (double)multDen) / (double)(div + divNum / (double)divDen) / (double)(1 << rDivPow2)) + " kHz");
      clockgen.setupRdiv(output, (si5351RDiv_t)rDivPow2);
    }






    if (resOutputs == 1) {
      /* Enable the clocks */
      clockgen.enableOutputs(true);
    }
  }
}