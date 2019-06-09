#include <Adafruit_NeoPixel.h>
#include <esp32-hal.h>

#define LED_PIN 13
#define RESTART_BUTTON_PIN 15
#define LED_COUNT 16
#define BRIGHTNESS 20

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


TimeKeeper timeKeeper = TimeKeeper(32);

void restart() {
  timeKeeper = TimeKeeper(32);
}


void setup()
{
  strip.begin();                   // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();                    // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS); // Set BRIGHTNESS to about 1/5 (max = 255)
  pinMode(RESTART_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(RESTART_BUTTON_PIN, restart, FALLING);
}

void loop()
{
  display();
}


void display() {
  while (timeKeeper.getRemainingTime() > 0)
  {
    log_printf("New loop is starting with %d \n", timeKeeper.getRemainingTime());
    displayTimeKeeper(timeKeeper);
  }
  display();
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
