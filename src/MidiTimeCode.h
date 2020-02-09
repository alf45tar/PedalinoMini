/*
__________           .___      .__  .__                 _____  .__       .__     ___ ________________    ___    
\______   \ ____   __| _/____  |  | |__| ____   ____   /     \ |__| ____ |__|   /  / \__    ___/     \   \  \   
 |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \ /  \ /  \|  |/    \|  |  /  /    |    | /  \ /  \   \  \  
 |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> )    Y    \  |   |  \  | (  (     |    |/    Y    \   )  ) 
 |____|    \___  >____ |(____  /____/__|___|  /\____/\____|__  /__|___|  /__|  \  \    |____|\____|__  /  /  /  
               \/     \/     \/             \/               \/        \/       \__\                 \/  /__/   
                                                                                   (c) 2018-2019 alf45star
                                                                       https://github.com/alf45tar/PedalinoMini
 */

//                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    /*
// Thanks to https://github.com/adanselm/padchokola


#ifndef _MIDI_CLOCK_MTC_H_
#define _MIDI_CLOCK_MTC_H_

#include <Arduino.h>
#include <Ticker.h>

// TAP_NUM_READINGS doesn't mean we have to wait for this many samples
// to change BPM, just that smoothing operates on this value.
#define TAP_NUM_READINGS 5

/////////////////////////////////////
class TapTempo
{
  public:
    TapTempo();
    ~TapTempo();

    float         tap();
    void          reset();

  private:
    int           mCurrentReadingPos;
    float         mReadings[TAP_NUM_READINGS];
    unsigned long mLastTap;

    bool          timeout(const unsigned long currentTime) const;
    float         calcBpmFromTime(unsigned long currentTime) const;
    float         computeAverage() const;
};

/////////////////////////////////////
class MidiTimeCode
{
  public:
    enum MidiSynchro
    {
      SynchroNone = 0,
      SynchroClockMaster,
      SynchroClockSlave,
      SynchroMTCMaster,
      SynchroMTCSlave
    };

    MidiTimeCode();
    ~MidiTimeCode();

    // To be called on main program setup
    void setup(void (*midi_send_callback_1)(byte b), void (*midi_send_callback_2)(byte b));

    // To be called on main program loop
    void loop();

    // Only active in Midi Clock mode
    void        setBpm(const float iBpm);
    const float tapTempo();
    byte        getBeat();
    void        setBeat(byte signature);
    //

    static void setMode(MidiSynchro newMode);
    static MidiSynchro getMode();

    // Only active in clock and MTC :
    void sendPlay();
    void sendStop();
    void sendContinue();
    bool isPlaying() const;
    //

    // Only active in MTC :
    void sendPosition(byte hours, byte minutes, byte seconds, byte frames);
    byte getHours();
    byte getMinutes();
    byte getSeconds();
    byte getFrames();
    void decodMTCQuarterFrame(byte MTCData);
    void decodeMTCFullFrame(unsigned size, const byte* array);
    //

    static void doSendMidiClock();
    static void doSendMTC();

    static Ticker                     mTimer;
    static volatile int               mInterruptCounter;

  private:
    enum MidiType
    {
      InvalidType           = 0x00,    ///< For notifying errors
      NoteOff               = 0x80,    ///< Note Off
      NoteOn                = 0x90,    ///< Note On
      AfterTouchPoly        = 0xA0,    ///< Polyphonic AfterTouch
      ControlChange         = 0xB0,    ///< Control Change / Channel Mode
      ProgramChange         = 0xC0,    ///< Program Change
      AfterTouchChannel     = 0xD0,    ///< Channel (monophonic) AfterTouch
      PitchBend             = 0xE0,    ///< Pitch Bend
      SystemExclusive       = 0xF0,    ///< System Exclusive
      TimeCodeQuarterFrame  = 0xF1,    ///< System Common - MIDI Time Code Quarter Frame
      SongPosition          = 0xF2,    ///< System Common - Song Position Pointer
      SongSelect            = 0xF3,    ///< System Common - Song Select
      TuneRequest           = 0xF6,    ///< System Common - Tune Request
      Clock                 = 0xF8,    ///< System Real Time - Timing Clock
      Start                 = 0xFA,    ///< System Real Time - Start
      Continue              = 0xFB,    ///< System Real Time - Continue
      Stop                  = 0xFC,    ///< System Real Time - Stop
      ActiveSensing         = 0xFE,    ///< System Real Time - Active Sensing
      SystemReset           = 0xFF,    ///< System Real Time - System Reset
    };

    enum MTCQuarterFrameType
    {
      FramesLow             = 0x00,
      FramesHigh            = 0x10,
      SecondsLow            = 0x20,
      SecondsHigh           = 0x30,
      MinutesLow            = 0x40,
      MinutesHigh           = 0x50,
      HoursLow              = 0x60,
      HoursHighAndSmpte     = 0x70,
    };

    enum SmpteMask
    {
      Frames24              = B0000,
      Frames25              = B0010,
      Frames30drop          = B0100,
      Frames30              = B0110,
    };

    struct Playhead
    {
      byte frames;
      byte seconds;
      byte minutes;
      byte hours;
    };

  private:
    static void sendMTCQuarterFrame(int index);
    static void sendMTCFullFrame();
    static void updatePlayhead();
    static void resetPlayhead();
    static void setPlayhead(byte hours, byte minutes, byte seconds, byte frames);
    static void setTimer(const double frequency);

  private:
    static MidiSynchro                mMode;
    static void (*mMidiSendCallback1)(byte b);
    static void (*mMidiSendCallback2)(byte b);

    // Midi Clock Stuff
    TapTempo                          mTapTempo;
    static const int                  mMidiClockPpqn;
    static volatile unsigned long     mEventTime;
    static volatile MidiType          mNextEvent;
    static int                        mPrescaler;
    static unsigned char              mSelectBits;
    static volatile byte              mClick;
    static volatile byte              mBeat;
    static volatile byte              mTimeSignature;
    static volatile bool              mPlaying;

    // MTC stuff
    static const SmpteMask            mCurrentSmpteType;
    static volatile Playhead          mPlayhead;
    static volatile int               mCurrentQFrame;
    static const MTCQuarterFrameType  mMTCQuarterFrameTypes[8];
};

#endif