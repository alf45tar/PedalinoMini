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
//


#include "MidiTimeCode.h"

// Allow 3 sec between taps at max (eq. to 20BPM)
#define TAP_TIMEOUT_MS 3000

///////////////////////////////////// TapTempo
TapTempo::TapTempo()
{
  reset();
}

TapTempo::~TapTempo()
{
}

void TapTempo::reset()
{
  mLastTap = 0;
  mCurrentReadingPos = 0;
  for ( int i = 0; i < TAP_NUM_READINGS; ++i )
  {
    mReadings[i] = 0.0f;
  }
}

float TapTempo::tap()
{
  const unsigned long currentTime = millis();
  if ( mLastTap > 0 )
  {
    if ( timeout(currentTime) )
      reset();

    mReadings[ mCurrentReadingPos % TAP_NUM_READINGS ] = calcBpmFromTime(currentTime);
    ++mCurrentReadingPos;

    if ( mCurrentReadingPos >= 2 )
    {
      mLastTap = currentTime;
      return computeAverage(); // Enough readings to compute average
    }
  }

  mLastTap = currentTime;
  return 0.0f;
}

bool TapTempo::timeout(const unsigned long currentTime) const
{
  if ( (currentTime - mLastTap) > TAP_TIMEOUT_MS)
    return true;

  return false;
}

float TapTempo::calcBpmFromTime(unsigned long currentTime) const
{
  if ( mLastTap == 0 || currentTime <= mLastTap )
    return 0.0f;

  const float msInAMinute = 1000 * 60.0;
  return msInAMinute / (currentTime - mLastTap);
}

float TapTempo::computeAverage() const
{
  float sum = 0.0f;
  const int count = min(mCurrentReadingPos, TAP_NUM_READINGS);
  for ( int i = 0; i < count; ++i )
  {
    sum += mReadings[i];
  }
  return sum / count;
}

///////////////////////////////////// MidiTimeCode
MidiTimeCode::MidiTimeCode()
{
}

MidiTimeCode::~MidiTimeCode()
{
}

void MidiTimeCode::setup(void (*midi_send_callback_1)(byte), void (*midi_send_callback_2)(byte))
{
  mMidiSendCallback1 = midi_send_callback_1;
  mMidiSendCallback2 = midi_send_callback_2;
  // Timer needed in setup even if no synchro occurring
  setTimer(1.0f);
}

void MidiTimeCode::doSendMidiClock()
{
  if ( mEventTime != 0 && (millis() - mEventTime) >= 1 )
  {
    // Reset timer giving slaves time to prepare for playback
    mEventTime = 0;
  }

  if ( mNextEvent != InvalidType )
  {
    mEventTime = millis();
    mMidiSendCallback1(mNextEvent);
    mPlaying = (mNextEvent == Start) || (mNextEvent == Continue);
    mNextEvent = InvalidType;
  }

  if ( mEventTime == 0 )
  {
    mMidiSendCallback1(Clock);
    mClick = (mClick + 1) % MidiTimeCode::mMidiClockPpqn;
    if (mClick == 0) mBeat = (mBeat + 1) % mTimeSignature;
  }
}

void MidiTimeCode::sendPlay()
{
  noInterrupts();
  mNextEvent = Start;
  mClick = 0;
  mBeat = 0;
  interrupts();
}

void MidiTimeCode::sendStop()
{
  noInterrupts();
  mNextEvent = Stop;
  interrupts();
}

void MidiTimeCode::sendContinue()
{
  noInterrupts();
  mNextEvent = Continue;
  interrupts();
}

void MidiTimeCode::sendPosition(byte hours, byte minutes, byte seconds, byte frames)
{
  noInterrupts();
  setPlayhead(hours, minutes, seconds, frames);
  if (mMode == MidiTimeCode::SynchroMTCMaster) mNextEvent = SongPosition;
  mCurrentQFrame = 0;
  interrupts();
}

byte MidiTimeCode::getHours()
{
  return mPlayhead.hours;
}

byte MidiTimeCode::getMinutes()
{
  return mPlayhead.minutes;
}

byte MidiTimeCode::getSeconds()
{
  return mPlayhead.seconds;
}

byte MidiTimeCode::getFrames()
{
  return mPlayhead.frames;
}

bool MidiTimeCode::isPlaying() const
{
  return (mNextEvent == Continue) || (mNextEvent == Start) || mPlaying;
}

void MidiTimeCode::doSendMTC()
{
  if ( mNextEvent == SongPosition)
  {
    sendMTCFullFrame();
    mNextEvent = Stop;
    return;
  }
  if ( mNextEvent != Stop )
  {
    if ( mNextEvent == Start)
    {
      resetPlayhead();
      //mCurrentQFrame = 0;
      mClick = 0;
      mBeat = 0;
      mNextEvent = Continue;
    }

    sendMTCQuarterFrame(mCurrentQFrame);
    mCurrentQFrame = (mCurrentQFrame + 1) % 8;

    if (mCurrentQFrame == 0)
      updatePlayhead();
  }
}

void MidiTimeCode::setMode(MidiTimeCode::MidiSynchro newMode)
{
  if ( mMode != newMode )
  {
    mMode = newMode;

    switch (mMode) {

      case MidiTimeCode::SynchroMTCMaster:
        setTimer(24 * 4);
        break;

      default:
        setTimer(1.0f);
        break;
    }

  }
}

MidiTimeCode::MidiSynchro MidiTimeCode::getMode()
{
  return mMode;
}

void MidiTimeCode::decodMTCQuarterFrame(byte MTCData)
{
  /*
  
  Quarter-frame messages

  When the time is running continuously, the 32-bit time code is broken into 8 4-bit pieces,
  and one piece is transmitted each quarter frame. I.e. 96—120 times per second, depending on the frame rate.
  Since it takes eight quarter frames for a complete time code message, the complete SMPTE time
  is updated every two frames.
  A quarter-frame messages consists of a status byte of 0xF1, followed by a single 7-bit data value:
  3 bits to identify the piece, and 4 bits of partial time code.
  
  When time is running forward, the piece numbers increment from 0–7; with the time that piece 0
  is transmitted is the coded instant, and the remaining pieces are transmitted later.

  If the MIDI data stream is being rewound, the piece numbers count backward. Again, piece 0
  is transmitted at the coded moment.

  The time code is divided little-endian as follows:

  MIDI time code pieces

  Piece #	Data byte	Significance
      0   0000 ffff	Frame number lsbits
      1  	0001 000f	Frame number msbit
      2   0010 ssss	Second lsbits
      3   0011 00ss	Second msbits
      4   0100 mmmm	Minute lsbits
      5   0101 00mm	Minute msbits
      6   0110 hhhh	Hour lsbits
      7   0111 0rrh	Rate and hour msbit

  Rate (0-3)
      rr  Frames/s
      00    24
      01    25
      10    29.97 (SMPTE drop-frame timecode)
      11    30

  */
  static byte b[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  if (mMode == MidiTimeCode::SynchroMTCSlave) {
    byte i = (MTCData & 0xf0) >> 4;   // MSB
    if (i > 7) return;

    b[i] = MTCData & 0x0f;            // LSB

    if (i == 7)
    {
      //byte frameType = b[7] & 0x06;

      byte h = (b[7] & 0x01) << 4 | b[6];
      byte m = (b[5] & 0x03) << 4 | b[4];
      byte s = (b[3] & 0x03) << 4 | b[2];
      byte f = (b[1] & 0x01) << 4 | b[0];

      if (h > 23)  h = 0;
      if (m > 59)  m = 0;
      if (s > 59)  s = 0;
      if (f > 30)  f = 0;

      setPlayhead(h, m, s, f);
      for (i = 0; i < 8; i++)
        b[i] = 0;
    }
  }
}

void MidiTimeCode::decodeMTCFullFrame(unsigned size, const byte* array)
{
  /*
    Full time code

    When there is a jump in the time code, a single full-time code is sent to synchronize attached equipment. This takes the form of a special global system exclusive message:

    F0 7F 7F 01 01 hh mm ss ff F7

    The manufacturer ID of 7F indicates a real-time universal message, the channel of 7F indicates
    it is a global broadcast. The following ID of 01 identifies this is a time code type message,
    and the second 01 indicates it is a full-time code message. The 4 bytes of time code follow.
    Although MIDI is generally little-endian, the 4 time code bytes follow in big-endian order,
    followed by a F7 "end of exclusive" byte.

    After a jump, the time clock stops until the first following quarter-frame message is received.
  */
  
  if (mMode == MidiTimeCode::SynchroMTCSlave && size == 11)
    if (array[1] == 0xf0 && array[2] == 0x7f && array[3] == 0x7f && array[4] == 0x01 && array[5] == 0x01 && array[10] == 0xf7)
      setPlayhead(array[6], array[7], array[8], array[9]);
}

void MidiTimeCode::sendMTCQuarterFrame(int index)
{
  mMidiSendCallback2(TimeCodeQuarterFrame);

  byte MTCData = 0;
  switch (mMTCQuarterFrameTypes[index])
  {
    case FramesLow:
      MTCData = mPlayhead.frames & 0x0f;
      break;
    case FramesHigh:
      MTCData = (mPlayhead.frames & 0xf0) >> 4;
      break;
    case SecondsLow:
      MTCData = mPlayhead.seconds & 0x0f;
      break;
    case SecondsHigh:
      MTCData = (mPlayhead.seconds & 0xf0) >> 4;
      break;
    case MinutesLow:
      MTCData = mPlayhead.minutes & 0x0f;
      break;
    case MinutesHigh:
      MTCData = (mPlayhead.minutes & 0xf0) >> 4;
      break;
    case HoursLow:
      MTCData = mPlayhead.hours & 0x0f;
      break;
    case HoursHighAndSmpte:
      MTCData = (mPlayhead.hours & 0xf0) >> 4 | mCurrentSmpteType;
      break;
  }
  mMidiSendCallback2(mMTCQuarterFrameTypes[index] | MTCData);
}

void MidiTimeCode::sendMTCFullFrame()
{
  mMidiSendCallback2(0xf0);
  mMidiSendCallback2(0x7f);
  mMidiSendCallback2(0x7f);
  mMidiSendCallback2(0x01);
  mMidiSendCallback2(0x01);
  mMidiSendCallback2(mPlayhead.hours);
  mMidiSendCallback2(mPlayhead.minutes);
  mMidiSendCallback2(mPlayhead.seconds);
  mMidiSendCallback2(mPlayhead.frames);
  mMidiSendCallback2(0xf7);
}

// To be called every two frames (so once a complete cycle of quarter frame messages have passed)
void MidiTimeCode::updatePlayhead()
{
  // Compute counter progress
  // update occurring every 2 frames
  mPlayhead.frames  += 2;
  mPlayhead.seconds += mPlayhead.frames  / 24;
  mPlayhead.minutes += mPlayhead.seconds / 60;
  mPlayhead.hours   += mPlayhead.minutes / 60;
  mPlayhead.frames  %= 24;
  mPlayhead.seconds %= 60;
  mPlayhead.minutes %= 60;
  mPlayhead.hours   %= 24;
}

void MidiTimeCode::resetPlayhead()
{
  mPlayhead.frames  = 0;
  mPlayhead.seconds = 0;
  mPlayhead.minutes = 0;
  mPlayhead.hours   = 0;
}

void MidiTimeCode::setPlayhead(byte hours, byte minutes, byte seconds, byte frames)
{
  mPlayhead.frames  = frames;
  mPlayhead.seconds = seconds;
  mPlayhead.minutes = minutes;
  mPlayhead.hours   = hours;
}

void MidiTimeCode::setBpm(const float iBpm)
{
  if ( mMode == SynchroClockMaster )
  {
    const double midiClockPerSec = mMidiClockPpqn * constrain(iBpm, 40, 300) / 60;
    setTimer(midiClockPerSec);
  }
}

const float MidiTimeCode::tapTempo()
{
  static float bpm = 0.0f;

  switch (mMode) {

    case SynchroClockMaster:
      return mTapTempo.tap();

    case SynchroClockSlave:
      mClick = (mClick + 1) % MidiTimeCode::mMidiClockPpqn;
      if (mClick == 0) {
        mBeat = (mBeat + 1) % mTimeSignature;
        bpm = mTapTempo.tap();
      }
      return bpm;

    case SynchroNone:
    case SynchroMTCMaster:
    case SynchroMTCSlave:
      break;
  }

  return 0.0f;
}

byte MidiTimeCode::getBeat()
{
  return mBeat;
}

void MidiTimeCode::setBeat(byte signature)
{
  mTimeSignature = signature;
}

void ISR()
{
  MidiTimeCode::mInterruptCounter++;
}


void MidiTimeCode::setTimer(const double frequency)
{
  mTimer.attach_ms(1000/frequency, ISR);
}

void MidiTimeCode::loop()
{
  if (mInterruptCounter > 0) {

    mInterruptCounter--;

    if ( MidiTimeCode::getMode() == MidiTimeCode::SynchroMTCMaster )
      MidiTimeCode::doSendMTC();
    else if ( MidiTimeCode::getMode() == MidiTimeCode::SynchroClockMaster )
      MidiTimeCode::doSendMidiClock();
  }
}

Ticker                  MidiTimeCode::mTimer;

unsigned char           MidiTimeCode::mSelectBits = 0;
volatile int            MidiTimeCode::mInterruptCounter = 0;

const int               MidiTimeCode::mMidiClockPpqn = 24;
volatile unsigned long  MidiTimeCode::mEventTime = 0;
volatile MidiTimeCode::MidiType MidiTimeCode::mNextEvent = InvalidType;
volatile byte           MidiTimeCode::mClick = 0;
volatile byte           MidiTimeCode::mBeat = 0;
volatile byte           MidiTimeCode::mTimeSignature = 4;
volatile bool           MidiTimeCode::mPlaying = false;


const         MidiTimeCode::SmpteMask MidiTimeCode::mCurrentSmpteType = Frames24;
volatile      MidiTimeCode::Playhead  MidiTimeCode::mPlayhead = MidiTimeCode::Playhead();
volatile int  MidiTimeCode::mCurrentQFrame = 0;
const         MidiTimeCode::MTCQuarterFrameType MidiTimeCode::mMTCQuarterFrameTypes[8] = { FramesLow, FramesHigh,
                                                                                           SecondsLow, SecondsHigh,
                                                                                           MinutesLow, MinutesHigh,
                                                                                           HoursLow, HoursHighAndSmpte
                                                                                         };
MidiTimeCode::MidiSynchro MidiTimeCode::mMode = MidiTimeCode::SynchroNone;
void (*MidiTimeCode::mMidiSendCallback1)(byte b) = 0;
void (*MidiTimeCode::mMidiSendCallback2)(byte b) = 0;
