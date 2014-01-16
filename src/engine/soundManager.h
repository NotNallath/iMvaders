#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <SFML/Audio.hpp>

#include <map>
#include <vector>

class SoundManager;
extern SoundManager soundManager;
class SoundManager
{
private:
    std::map<std::string, sf::SoundBuffer*> soundMap;
    std::vector<sf::Sound> activeSoundList;
public:
    SoundManager();
    ~SoundManager();
    
    void playSound(const char* name, float pitch = 1.0f, float volume = 100.0f);
private:
    sf::SoundBuffer* loadSound(const char* name);
};

#endif//SOUNDMANAGER_H
