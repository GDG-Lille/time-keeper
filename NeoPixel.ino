#include <Adafruit_NeoPixel.h>
#include <esp32-hal.h>

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN 13

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 16

// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 50

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_KHZ800 + NEO_GRB);

class TimeKeeper
{
private:
  int totalTime, remainingTime;

public:
  TimeKeeper(int seconds)
  {
    this->totalTime = seconds;
    this->remainingTime = seconds;
  }
  /**
   * Retrieve the totalTime set for this timeKeeper
   */
  int getTotalTime() const
  {
    return this->totalTime;
  }

  /**
   * Retrieve the remainingTime for this timeKeeper.
   */
  int getRemainingTime() const
  {
    return this->remainingTime;
  }

  /**
   * wait for the given number of seconds.
   */
  void wait(int seconds)
  {
    delay(seconds * 1000);
    this->remainingTime -= seconds;
  }
};

void setup()
{
  strip.begin();                   // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();                    // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void loop()
{
  TimeKeeper timeKeeper = TimeKeeper(32);
  while (timeKeeper.getRemainingTime() > 0)
  {
    log_printf("New loop is starting with %d \n", timeKeeper.getRemainingTime());
    displayTimeKeeper(timeKeeper);
  }
  loop();
}

void displayTimeKeeper(TimeKeeper& timeKeeper)
{
  for (int seconds = strip.numPixels() - 1; seconds >= 0; seconds--)
  {
    displayRemainingTime(timeKeeper);
    strip.setPixelColor(seconds, strip.Color(BRIGHTNESS, BRIGHTNESS, BRIGHTNESS));
    strip.show();
    timeKeeper.wait(1);
  }
}

void displayRemainingTime(TimeKeeper& timeKeeper)
{
  strip.clear();
  int coeff = (timeKeeper.getTotalTime() * 10 / timeKeeper.getRemainingTime());
  for (int i = 0; i < int((strip.numPixels()) * 10 / coeff); i++)
  {
    strip.setPixelColor(i, strip.Color(0, BRIGHTNESS, 0));
  }
}
