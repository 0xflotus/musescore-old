//=============================================================================
//  MuseScore
//  Linux Music Score Editor
//  $Id: MP3Exporter.cpp 2992 2010-04-22 14:42:39Z lasconic $
//
//  Copyright (C) 2007 Werner Schweer and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================

#include "score.h"
#include "fluid.h"
#include "al/tempo.h"
#include "note.h"
#include "mscore.h"
#include "part.h"
#include "preferences.h"
#include "seq.h"
#include "exportmp3.h"


MP3Exporter::MP3Exporter()
      {
      mLibraryLoaded = false;
      mEncoding = false;
      mGF = NULL;
      
      QSettings settings;
      mLibPath = settings.value("/Export/lameMP3LibPath", "").toString();
      
      mBitrate = 128;
      mQuality = QUALITY_2;
      mChannel = CHANNEL_STEREO;
      mMode = MODE_CBR;
      mRoutine = ROUTINE_FAST;
      }

MP3Exporter::~MP3Exporter()
      {
      freeLibrary();
      }

bool MP3Exporter::findLibrary(QWidget *parent)
      {
      QString path;
      QString name;
      
      if (!mLibPath.isEmpty()) {
            QFileInfo fi(mLibPath);
            path = fi.absolutePath();
            name = fi.baseName();
            }
      else {
            path = getLibraryPath();
            name = getLibraryName();
            }
            
      if(noGui)
            return false;
            
      QString libPath = QFileDialog::getOpenFileName(
           parent, tr("Where is %1 ?").arg(getLibraryName()),
           path,
           getLibraryTypeString());
      
      if (libPath.isEmpty())
            return false;
      
      QFileInfo fp(libPath);
      if (!fp.exists())
            return false;
      
      mLibPath = libPath;
      
      QSettings settings;
      settings.setValue("/Export/lameMP3LibPath", mLibPath);
      
      return true;
}

bool MP3Exporter::loadLibrary(QWidget *parent, AskUser askuser)
      {
      if (validLibraryLoaded()) {
            freeLibrary();
            mLibraryLoaded = false;
            }

      // First try loading it from a previously located path
      if (!mLibPath.isEmpty()) {
            printf("Attempting to load LAME from previously defined path\n");
            mLibraryLoaded = initLibrary(mLibPath);
            }

      // If not successful, try loading using system search paths
      if (!validLibraryLoaded()) {
            printf("Attempting to load LAME from system search paths\n");
            mLibPath = getLibraryName();
            mLibraryLoaded = initLibrary(mLibPath);
            }

      // If not successful, try loading using compiled in path
      if (!validLibraryLoaded()) {
            printf("Attempting to load LAME from builtin path\n");
            QFileInfo fn(QDir(getLibraryPath()), getLibraryName());
            mLibPath = fn.absoluteFilePath();
            mLibraryLoaded = initLibrary(mLibPath);
            }

      // If not successful, must ask the user
      if (!validLibraryLoaded()) {
            printf("(Maybe) ask user for library\n");
            int ret = QMessageBox::question(0, tr("Save as MP3"), 
                  tr("MuseScore does not export MP3 files directly, but instead uses \n"
                   "the freely available LAME library.  You must obtain %1 \n"
                   "separately, and then locate the file for MuseScore.\n"
                   "You only need to do this once.\n\n"
                   "Would you like to locate %2 now?").arg(getLibraryName()).arg(getLibraryName()), 
                   QMessageBox::Yes|QMessageBox::No, QMessageBox::NoButton);
            if (ret == QMessageBox::Yes && askuser == MP3Exporter::Maybe && findLibrary(parent)) {
                  mLibraryLoaded = initLibrary(mLibPath);
                  }
            }

      // Oh well, just give up
      if (!validLibraryLoaded()) {
            printf("Failed to locate LAME library\n");
            return false;
            }

      printf("LAME library successfully loaded\n");
      return true;
      }

bool MP3Exporter::validLibraryLoaded()
      {
      return mLibraryLoaded;
      }

void MP3Exporter::setMode(int mode)
      {
      mMode = mode;
      }

void MP3Exporter::setBitrate(int rate)
      {
      mBitrate = rate;
      }

void MP3Exporter::setQuality(int q, int r)
      {
      mQuality = q;
      mRoutine = r;
      }

void MP3Exporter::setChannel(int mode)
      {
      mChannel = mode;
      }

bool MP3Exporter::initLibrary(QString libpath)
      {
      printf("Loading LAME from %s\n", qPrintable(libpath));
      lame_lib = new QLibrary(libpath, 0);
      if (!lame_lib->load()) {
            printf("load failed\n");
            return false;
            }
      
      /*printf("Actual LAME path %s\n",
                FileNames::PathFromAddr(lame_lib->resolve("lame_init")));*/
      
      lame_init = (lame_init_t *)
        lame_lib->resolve("lame_init");
      get_lame_version = (get_lame_version_t *)
        lame_lib->resolve("get_lame_version");
      lame_init_params = (lame_init_params_t *)
        lame_lib->resolve("lame_init_params");
      lame_encode_buffer_float = (lame_encode_buffer_float_t *)
        lame_lib->resolve("lame_encode_buffer_float");
      lame_encode_flush = (lame_encode_flush_t *)
        lame_lib->resolve("lame_encode_flush");
      lame_close = (lame_close_t *)
        lame_lib->resolve("lame_close");
      
      lame_set_in_samplerate = (lame_set_in_samplerate_t *)
         lame_lib->resolve("lame_set_in_samplerate");
      lame_set_out_samplerate = (lame_set_out_samplerate_t *)
         lame_lib->resolve("lame_set_out_samplerate");
      lame_set_num_channels = (lame_set_num_channels_t *)
         lame_lib->resolve("lame_set_num_channels");
      lame_set_quality = (lame_set_quality_t *)
         lame_lib->resolve("lame_set_quality");
      lame_set_brate = (lame_set_brate_t *)
         lame_lib->resolve("lame_set_brate");
      lame_set_VBR = (lame_set_VBR_t *)
         lame_lib->resolve("lame_set_VBR");
      lame_set_VBR_q = (lame_set_VBR_q_t *)
         lame_lib->resolve("lame_set_VBR_q");
      lame_set_VBR_min_bitrate_kbps = (lame_set_VBR_min_bitrate_kbps_t *)
         lame_lib->resolve("lame_set_VBR_min_bitrate_kbps");
      lame_set_mode = (lame_set_mode_t *) 
         lame_lib->resolve("lame_set_mode");
      lame_set_preset = (lame_set_preset_t *)
         lame_lib->resolve("lame_set_preset");
      lame_set_error_protection = (lame_set_error_protection_t *)
         lame_lib->resolve("lame_set_error_protection");
      lame_set_disable_reservoir = (lame_set_disable_reservoir_t *)
         lame_lib->resolve("lame_set_disable_reservoir");
      lame_set_padding_type = (lame_set_padding_type_t *)
         lame_lib->resolve("lame_set_padding_type");
      lame_set_bWriteVbrTag = (lame_set_bWriteVbrTag_t *)
         lame_lib->resolve("lame_set_bWriteVbrTag");
      
      // These are optional
      lame_get_lametag_frame = (lame_get_lametag_frame_t *)
         lame_lib->resolve("lame_get_lametag_frame");
      lame_mp3_tags_fid = (lame_mp3_tags_fid_t *)
         lame_lib->resolve("lame_mp3_tags_fid");
#if defined(Q_WS_WIN)
      beWriteInfoTag = (beWriteInfoTag_t *)
         lame_lib->resolve("beWriteInfoTag");
      beVersion = (beVersion_t *)
         lame_lib->resolve("beVersion");
#endif
      
      if (!lame_init ||
        !get_lame_version ||
        !lame_init_params ||
        !lame_encode_buffer_float ||
        !lame_encode_flush ||
        !lame_close ||
        !lame_set_in_samplerate ||
        !lame_set_out_samplerate ||
        !lame_set_num_channels ||
        !lame_set_quality ||
        !lame_set_brate ||
        !lame_set_VBR ||
        !lame_set_VBR_q ||
        !lame_set_mode ||
        !lame_set_preset ||
        !lame_set_error_protection ||
        !lame_set_disable_reservoir ||
        !lame_set_padding_type ||
        !lame_set_bWriteVbrTag) {
            printf("Failed to find a required symbol in the LAME library\n");
#if defined(Q_WS_WIN)
            if (beVersion) {
                  be_version v;
                  beVersion(&v);
      
                  mBladeVersion = QString("You are linking to lame_enc.dll v%d.%d. This version is not compatible with Audacity %d.\nPlease download the latest version of the LAME MP3 library.")
                                .arg(v.byMajorVersion)
                                .arg(v.byMinorVersion)
                                .arg(1); //TODO 
                  }
#endif
      
            lame_lib->unload();
            delete lame_lib;
            return false;
            }
      
      mGF = lame_init();
      if (mGF == NULL) {
            return false;
            }
      
      return true;
      }

void MP3Exporter::freeLibrary()
      {
      if (mGF) {
            lame_close(mGF);
            mGF = NULL;
            }
      lame_lib->unload();
      delete lame_lib;
      return;
      }

QString MP3Exporter::getLibraryVersion()
      {
      if (!mLibraryLoaded) {
            return QString("");
            }
      
      return QString("LAME %s").arg(get_lame_version());
      }

int MP3Exporter::initializeStream(int channels, int sampleRate)
      {
      if (!mLibraryLoaded) {
            return -1;
            }
      
      if (channels > 2) {
            return -1;
            }
      
      lame_set_error_protection(mGF, false);
      lame_set_num_channels(mGF, channels);
      lame_set_in_samplerate(mGF, sampleRate);
      lame_set_out_samplerate(mGF, sampleRate);
      lame_set_disable_reservoir(mGF, true);
      lame_set_padding_type(mGF, PAD_NO);
      
      // Add the VbrTag for all types.  For ABR/VBR, a Xing tag will be created.
      // For CBR, it will be a Lame Info tag.
      lame_set_bWriteVbrTag(mGF, true);
      
      // Set the VBR quality or ABR/CBR bitrate
      switch (mMode) {
      case MODE_SET:
      {
      int preset;
      
      if (mQuality == PRESET_INSANE) {
        preset = INSANE;
      }
      else if (mRoutine == ROUTINE_FAST) {
        if (mQuality == PRESET_EXTREME) {
           preset = EXTREME_FAST;
        }
        else if (mQuality == PRESET_STANDARD) {
           preset = STANDARD_FAST;
        }
        else {
           preset = 1007;    // Not defined until 3.96
        }
      }
      else {
        if (mQuality == PRESET_EXTREME) {
           preset = EXTREME;
        }
        else if (mQuality == PRESET_STANDARD) {
           preset = STANDARD;
        }
        else {
           preset = 1006;    // Not defined until 3.96
        }
      }
      
      lame_set_preset(mGF, preset);
      }
      break;
      
      case MODE_VBR:
      lame_set_VBR(mGF, (mRoutine == ROUTINE_STANDARD ? vbr_rh : vbr_mtrh ));
      lame_set_VBR_q(mGF, mQuality);
      break;
      
      case MODE_ABR:
      lame_set_preset(mGF, mBitrate );
      break;
      
      default:
      lame_set_VBR(mGF, vbr_off);
      lame_set_brate(mGF, mBitrate);
      break;
      }
      
      // Set the channel mode
      MPEG_mode mode;
      if (channels == 1) {
      mode = MONO;
      }
      else if (mChannel == CHANNEL_JOINT) {
      mode = JOINT_STEREO;
      }
      else {
      mode = STEREO;
      }
      lame_set_mode(mGF, mode);
      
      int rc = lame_init_params(mGF);
      if (rc < 0) {
      return rc;
      }
      
      #if 0
      dump_config(mGF);
      #endif
      
      mInfoTagLen = 0;
      mEncoding = true;
      
      return mSamplesPerChunk;
      }

int MP3Exporter::getOutBufferSize()
{
   if (!mEncoding)
      return -1;

   return mOutBufferSize;
}

void MP3Exporter::bufferPreamp(float buffer[], int nSamples)
{
   int i;
   for (i = 0; i < nSamples; i++)
      buffer[i] = buffer[i] * 32768;
}

int MP3Exporter::encodeBuffer(float inbufferL[], float inbufferR[], unsigned char outbuffer[])
{
   if (!mEncoding) {
      return -1;
   }

   bufferPreamp(inbufferL, mSamplesPerChunk);
   bufferPreamp(inbufferR, mSamplesPerChunk);
   return lame_encode_buffer_float(mGF, inbufferL, inbufferR, mSamplesPerChunk,
      outbuffer, mOutBufferSize);
}

int MP3Exporter::encodeRemainder(float inbufferL[], float inbufferR[], int nSamples,
                  unsigned char outbuffer[])
{
   if (!mEncoding) {
      return -1;
   }

   bufferPreamp(inbufferL, nSamples);
   bufferPreamp(inbufferR, nSamples);
   return lame_encode_buffer_float(mGF, inbufferL, inbufferR, nSamples, outbuffer,
      mOutBufferSize);
}

int MP3Exporter::encodeBufferMono(float inbuffer[], unsigned char outbuffer[])
{
   if (!mEncoding) {
      return -1;
   }

   bufferPreamp(inbuffer, mSamplesPerChunk);
   return lame_encode_buffer_float(mGF, inbuffer, inbuffer, mSamplesPerChunk,
      outbuffer, mOutBufferSize);
}

int MP3Exporter::encodeRemainderMono(float inbuffer[], int nSamples,
                  unsigned char outbuffer[])
{
   if (!mEncoding) {
      return -1;
   }

   bufferPreamp(inbuffer, nSamples);
   return lame_encode_buffer_float(mGF, inbuffer, inbuffer, nSamples, outbuffer,
      mOutBufferSize);
}

int MP3Exporter::finishStream(unsigned char outbuffer[])
{
   if (!mEncoding) {
      return -1;
   }

   mEncoding = false;

   int result = lame_encode_flush(mGF, outbuffer, mOutBufferSize);

   if (lame_get_lametag_frame) {
      mInfoTagLen = lame_get_lametag_frame(mGF, mInfoTagBuf, sizeof(mInfoTagBuf));
   }

   return result;
}

void MP3Exporter::cancelEncoding()
{
   mEncoding = false;
}

/*void MP3Exporter::PutInfoTag(QFile f, qint64 off)
{
  QDataStream out(&f); 
   if (mGF) {
      if (mInfoTagLen > 0) {
         out.skipRawData (off);
         out.writeRawData(mInfoTagBuf, mInfoTagLen);
      }
#if defined(Q_WS_WIN)
      else if (beWriteInfoTag) {
         f.flush();
         QFileInfo fi(f);
         beWriteInfoTag(mGF, qPrintable(fi.baseName()));
         mGF = NULL;
      }
#endif
      else if (lame_mp3_tags_fid) {
      std::FILE *fp;
      if ((fp = std::fdopen(file.handle(), "w+")) != NULL)            
         lame_mp3_tags_fid(mGF, fp);
      }
   }

   f.seek(f.size());
}*/

#if defined(Q_WS_WIN)
/* values for Windows */

QString MP3Exporter::getLibraryPath()
{
      QSettings settings("HKEY_LOCAL_MACHINE\\Software\\Lame for Audacity", QSettings::NativeFormat);
      QString sReturnedValue = settings.value( "InstallPath", "" ).toString();
      if (! sReturnedValue.isEmpty()) {
            return sReturnedValue;
            }
      return QDir::rootPath();
}

QString MP3Exporter::getLibraryName()
{
   return QString("lame_enc.dll");
}

QString MP3Exporter::getLibraryTypeString()
{
   return QString("Only lame_enc.dll (lame_enc.dll);;Dynamically Linked Libraries (*.dll);;All Files (*.*)");
}

#elif defined(Q_WS_MAC)
/* values for Mac OS X */

QString MP3Exporter::getLibraryPath()
{
   return QString("/usr/local/lib/audacity");
}

QString MP3Exporter::getLibraryName()
{
   return QString("libmp3lame.dylib");
}

QString MP3Exporter::getLibraryTypeString()
{
   return QString("Only libmp3lame.dylib (libmp3lame.dylib);;Dynamic Libraries (*.dylib);;All Files (*)");
}

#else //!Q_WS_MAC
/* Values for Linux / Unix systems */

QString MP3Exporter::getLibraryPath()
{
   return QString(LIBDIR);
}

QString MP3Exporter::getLibraryName()
{
   return QString("libmp3lame.so.0");
}

QString MP3Exporter::getLibraryTypeString()
{
   return QString("Only libmp3lame.so.0 (libmp3lame.so.0);;Primary Shared Object files (*.so);;Extended Libraries (*.so*);;All Files (*)");
}
#endif //mac


bool Score::saveMp3(const QString& name, QString soundFont) 
      {   
      int sampleRate = preferences.exportAudioSampleRate;
      
      if (soundFont.isEmpty()) {
            if (!preferences.soundFont.isEmpty())
                  soundFont = preferences.soundFont;
            else
                  soundFont = QString(getenv("DEFAULT_SOUNDFONT"));
            if (soundFont.isEmpty()) {
                  fprintf(stderr, "MuseScore: error: no soundfont configured\n");
                  return false;
                  }
            }
            
      MP3Exporter exporter;
      if (!exporter.loadLibrary(mscore, MP3Exporter::Maybe)) {
            QSettings settings;
            settings.setValue("/Export/lameMP3LibPath", "");
            if(!noGui)
                  QMessageBox::warning(0,
                               tr("Error opening lame library"),
                               tr("Could not open MP3 encoding library!"),
                               QString::null, QString::null);
            printf("Could not open MP3 encoding library!\n");
            return false;
            }
  
      if (!exporter.validLibraryLoaded()) {
            QSettings settings;
            settings.setValue("/Export/lameMP3LibPath", "");
            if(!noGui)
                  QMessageBox::warning(0,
                               tr("Error opening lame library"),
                               tr("Not a valid or supported MP3 encoding library!"),
                               QString::null, QString::null);
            printf("Not a valid or supported MP3 encoding library!\n");
            return false;
            }
      
      // Retrieve preferences
      int highrate = 48000;
      int lowrate = 8000;
      int bitrate = 0;
      int brate = 128;
      int rmode = MODE_CBR;
      int vmode = ROUTINE_FAST;
      int cmode = CHANNEL_STEREO;
      
      int channels = 2;

      exporter.setMode(MODE_CBR);
      exporter.setBitrate(bitrate);
      exporter.setChannel(CHANNEL_STEREO);
      
      int inSamples = exporter.initializeStream(channels, sampleRate);
      if (inSamples < 0) {
            if(!noGui)
                  QMessageBox::warning(0,
                                 tr("Encoding error"),
                                 tr("Unable to initialize MP3 stream"),
                                 QString::null, QString::null);
            printf("Unable to initialize MP3 stream\n");
            return false;
            }
      
      int bufferSize = exporter.getOutBufferSize();
      unsigned char *bufferOut = new unsigned char[bufferSize];
      long bytes; // output length
      
      QFile file(name);
      if(! file.open(QIODevice::WriteOnly)) {
            if(!noGui)
                  QMessageBox::warning(0,
                         tr("Encoding error"),
                         tr("Unable to open target file for writing"),
                         QString::null, QString::null);
            return false;
            }
          
      QDataStream out(&file);
      
      MasterSynth* synti = new MasterSynth();
      synti->init(sampleRate);
      synti->setState(syntiState());

      EventMap events;
      toEList(&events);

      QProgressBar* pBar = mscore->showProgressBar();
      pBar->reset();

      double peak = 0.0;
      double gain = 1.0;
      for (int pass = 0; pass < 2; ++pass) {
            EventMap::const_iterator playPos;
            playPos = events.constBegin();
            EventMap::const_iterator endPos = events.constEnd();
            --endPos;
            double et = utick2utime(endPos.key());
            et += 1.0;   // add trailer (sec)
            pBar->setRange(0, int(et));

            //
            // init instruments
            //
            foreach(const Part* part, _parts) {
                  foreach(const Channel& a, part->instr()->channel()) {
                        a.updateInitList();
                        foreach(Event e, a.init) {
                              if (e.type() == ME_INVALID)
                                    continue;
                              e.setChannel(a.channel);
                              int syntiIdx= _midiMapping[a.channel].articulation->synti;
                              synti->play(e, syntiIdx);
                              }
                        }
                  }

            int FRAMES = 512;
            float bufferL[FRAMES];
            float bufferR[FRAMES];
            int stride      = 1;
            double playTime = 0.0;
            synti->setGain(gain);

            for (;;) {
                  unsigned frames = FRAMES;
                  //
                  // collect events for one segment
                  //
                  memset(bufferL, 0, sizeof(float) * FRAMES);
                  memset(bufferR, 0, sizeof(float) * FRAMES);
                  double endTime = playTime + double(frames)/double(sampleRate);
                  float* l = bufferL;
                  float* r = bufferR;                  
                  for (; playPos != events.constEnd(); ++playPos) {                  
                        double f = utick2utime(playPos.key());
                        if (f >= endTime)
                              break;
                        int n = lrint((f - playTime) * sampleRate);
                        synti->process(n, l, r, stride);
                        
                        l         += n * stride;
                        r         += n * stride;
                        playTime += double(n)/double(sampleRate);
                        frames    -= n;
                        const Event& e = playPos.value();
                        if (e.isChannelEvent()) {
                              int channelIdx = e.channel();
                              Channel* c = _midiMapping[channelIdx].articulation;
                              if (!c->mute) {
                                    synti->play(e, c->synti);
                                    }
                              }
                        }
                  if (frames) {
                        synti->process(frames, l, r, stride);
                        playTime += double(frames)/double(sampleRate);
                        }
                  if (pass == 1) {
                        if (FRAMES < inSamples) {
                              if (channels > 1) {
                                    bytes = exporter.encodeRemainder(bufferL, bufferR,  FRAMES , bufferOut);
                                    }
                              else {
                                    bytes = exporter.encodeRemainderMono(bufferL,  FRAMES , bufferOut);
                                    }
                              }
                              else {
                                    if (channels > 1) {
                                          bytes = exporter.encodeBuffer(bufferL, bufferR, bufferOut);
                                          }
                                    else {
                                          bytes = exporter.encodeBufferMono(bufferL, bufferOut);
                                          }
                                    }
                              if (bytes < 0) {
                                    if(!noGui)
                                          QMessageBox::warning(0,
                                           tr("Encoding error"),
                                           tr("Error %1 returned from MP3 encoder").arg(bytes),
                                           QString::null, QString::null);
                                    break;
                                    }
                              for (unsigned j = 0; j < bytes; ++j)
                                    out << bufferOut[j];
                        }
                  else {
                        for (unsigned i = 0; i < FRAMES; ++i) {
                              if (qAbs(bufferL[i]) > peak)
                                    peak = qAbs(bufferL[i]);
                              if (qAbs(bufferR[i]) > peak)
                                    peak = qAbs(bufferR[i]);
                              }
                        }
                  playTime = endTime;
                  pBar->setValue(int(playTime));
                  if (playTime > et)
                        break;
                  }
            gain = 0.99 / peak;
            }

      bytes = exporter.finishStream(bufferOut);
      if (bytes) {
            for (unsigned j = 0; j < bytes; ++j)
                  out << bufferOut[j];
      }

      mscore->hideProgressBar();

      delete synti;
      
      file.close();
      
      return true;
}