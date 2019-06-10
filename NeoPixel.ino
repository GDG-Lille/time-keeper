#include <Adafruit_NeoPixel.h>
#include <esp32-hal.h>

#define LED_PIN 13
#define RESTART_BUTTON_PIN 15
#define SELECTOR_BUTTON 14
#define LED_COUNT 16
#define BRIGHTNESS 20
#define CONFERENCE_DURATION 3000
#define QUICKY_DURATION 1200

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_KHZ800 + NEO_GRB);

class TimeKeeper
{
private:
  int totalTime, remainingTime;
  bool started;

public:
  TimeKeeper(int seconds)
  {
    this->totalTime = seconds;
    this->remainingTime = seconds;
    this->started = false;
  }
  /**
   * Retrieve the totalTime set for this timeKeeper.
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
   * Start the TimeKeeper.
   */
  void start() {
    this->started = true;
  }

  /**
   * Stop the TimeKeeper.
   */
  void stop() {
    this->started = false;
  }

  /**
   * Is the TimeKeeper started ? 
   */
  bool isStarted(){
    return this->started;
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

class TimeSelector{
  private:
    int index;
    int PRESETS [2] = {CONFERENCE_DURATION, QUICKY_DURATION};

  public:  
    TimeSelector(){
      this->index = 0;
    }

    /**
     * Switch to next preset.
     */
    void next(){
      this->index=(this->index+1) % 2; 
    }

    /**
     * Get preset value.
     */
    int getPreset(){
      return this->PRESETS[this->index];
    }


};

TimeSelector timeSelector = TimeSelector();
TimeKeeper timeKeeper = TimeKeeper(timeSelector.getPreset());

void restart() {
  timeKeeper.stop();
  timeKeeper = TimeKeeper(timeSelector.getPreset());
  timeKeeper.start();
}

void select() {
  timeKeeper.stop();
  timeSelector.next();
}

void setup()
{
  strip.begin();                   // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();                    // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS); // Set BRIGHTNESS to about 1/5 (max = 255)
  pinMode(RESTART_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(RESTART_BUTTON_PIN, restart, FALLING);
  pinMode(SELECTOR_BUTTON, INPUT_PULLUP);
  attachInterrupt(SELECTOR_BUTTON, select, FALLING);
}

void loop()
{
  display();
}

void display() {
  log_printf("time keeper is started %b \n", timeKeeper.isStarted());
    while (timeKeeper.getRemainingTime() > 0 && timeKeeper.isStarted())
    {
      log_printf("New loop is starting with %d \n", timeKeeper.getRemainingTime());
      displayTimeKeeper();
    }
    log_printf("Selector with %d \n", timeSelector.getPreset());
    displayTimeSelector();
}

void displayTimeSelector(){
  strip.clear();
  int coeff = (CONFERENCE_DURATION / timeSelector.getPreset());
  for (int i = 0; i < int((strip.numPixels()) / coeff); i++)
  {
    strip.setPixelColor(i, strip.Color(BRIGHTNESS, BRIGHTNESS, BRIGHTNESS));
  }
  strip.show();
  delay(1000);
}

void displayTimeKeeper()
{
  int seconds = strip.numPixels() - 1;
  while (seconds >= 0 && timeKeeper.isStarted())
  {
    displayRemainingTime();
    strip.setPixelColor(seconds, strip.Color(BRIGHTNESS, BRIGHTNESS, BRIGHTNESS));
    strip.show();
    timeKeeper.wait(1);
    seconds--;
  }
}

void displayRemainingTime()
{
  strip.clear();
  int coeff = (timeKeeper.getTotalTime() * 10 / timeKeeper.getRemainingTime());
  for (int i = 0; i < int((strip.numPixels()) * 10 / coeff); i++)
  {
    strip.setPixelColor(i, strip.Color(0, BRIGHTNESS, 0));
  }
}
