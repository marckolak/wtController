#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

class Settings
{
public:
    Settings();


public:
    //! RPi Controller IP address
    static QString host;

    //! RPi Controller port
    static int port;

    //! receive port
    static int rcvPort;


public:
    //!
    //! \brief loadSettings loads settings from settings.json file, which should b eplaced in the same folder as executable
    //! \return False if file not found
    //!
    static bool loadSettings();

};

#endif // SETTINGS_H
