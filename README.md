# Sagger
As in Simple Audio Tagger and [anti-tank guided missle](https://en.wikipedia.org/wiki/AT-3_Sagger) is a simple program for audio files tagging using Chromaprint library (via *fpcalc*) and [AcoustID](https://acoustid.org/) service with [MusicBrainz](https://musicbrainz.org/) metadata. Both GUI [(GTK+)](https://www.gtk.org) and CLI are supported with simple configuration settings allowing to set source and target directory to which all the files will be copied using given directory and file naming scheme. For now only these values are changed so Sagger is not a "true" tagger in a sense that no additional file metadata will be set. Sagger is fault-tolerant for basic errors like wrong permissions and no Internet access and to my knowledge no data should be lost while using this software however it is still in early phase of development so it is strongly advised not to use it with data of which you have no backup copy.

## Usage
**Remember to fill *YOUR_KEY* variable from `run.c` with valid API key from [AcoustID](https://acoustid.org/)**

If you run Sagger with no additional arguments, it will assume that you want to use GUI mode.

### For CLI mode:
sagger [OPTIONS] SOURCE_DIRECTORY TARGET_DIRECTORY

Where options are:
* -album/-artist/-both choose what to include in directory name and filename. Two arguments are mandatory, song name is included in filename each time.
* -h/--help print help
* -v/--version print version number and license
  
### Sample
`sagger -both -artist ~/Music/source ~/Music/dir`

## Dependencies with respective package names:
* GTK3 (libgtk-3-dev)
* libcurl (libcurl4-gnutls-dev)
* json-glib (libjson-glib-dev)
* fpcalc (libchromaprint-tools)
* cmake (for now my cmake file is very basic, please report if it does not work in your environement or you know the oldest library version for which Sagger will compile)

I have not checked minimum versions for libraries but anything not older than what is shipped with Ubuntu 19.10 (with CMake 3.15) should work.

## Closing notes
Take note that this is my first CS project. It serves it's purpose but despite my best efforts it may contain code not following established coding patters or outright ugly. If you notice parts fitting this description please file an issue or prepare pull request and I will review them all but if you are in need of mature versatile project to use in production environment you may also want to check out out [other Music Brainz enabled software](https://musicbrainz.org/doc/MusicBrainz_Enabled_Applications)
