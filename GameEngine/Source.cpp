#include "Source.h"

#include "OpenALFunctions.h"
#include "AssetLoader.h"
#include "Debug.h"

#pragma warning(disable : 26495)
Source::Source() {}
#pragma warning(restore : 26495)

Source::Source(const std::string filename, glm::vec3 position, glm::vec3 velocity, ALfloat pitch, ALfloat gain,
	ALfloat referenceDistance, ALfloat maxDistance, ALfloat rolloffFactor, ALboolean looping) :
	position(position), velocity(velocity), pitch(pitch), gain(gain), referenceDistance(referenceDistance),
	maxDistance(maxDistance), rolloffFactor(rolloffFactor), looping(looping)
{
	this->sound = AssetLoader::loadWav(filename);
	if (this->sound.rawSoundData == nullptr || this->sound.dataSize == 0)
		LOG_fileLoad(filename, "Audio", false);
	alCall(alGenBuffers, 1, &this->buffer);

	ALenum format;
	if (sound.channels == 1 && sound.bitsPerSample == 8)
		format = AL_FORMAT_MONO8;
	else if (sound.channels == 1 && sound.bitsPerSample == 16)
		format = AL_FORMAT_MONO16;
	else if (sound.channels == 2 && sound.bitsPerSample == 8)
		format = AL_FORMAT_STEREO8;
	else if (sound.channels == 2 && sound.bitsPerSample == 16)
		format = AL_FORMAT_STEREO16;
	else
	{
		LOG_fileLoad(filename, "Audio", false, "Unrecognized Wave Format");
		format = NULL;
	}

	std::vector<char> soundData(this->sound.rawSoundData, this->sound.rawSoundData + this->sound.dataSize);
	alCall(alBufferData, buffer, format, soundData.data(), soundData.size(), sound.sampleRate);
	soundData.clear();

	alCall(alGenSources, 1, &this->id);
	alCall(alSourcef, this->id, AL_PITCH, this->pitch);
	alCall(alSourcef, this->id, AL_GAIN, this->gain);
	alCall(alSource3f, this->id, AL_POSITION, this->position.x, this->position.y, this->position.z);
	alCall(alSource3f, this->id, AL_VELOCITY, this->velocity.x, this->velocity.y, this->velocity.z);
	alCall(alSourcei, this->id, AL_LOOPING, this->looping);
	alCall(alSourcef, this->id, AL_ROLLOFF_FACTOR, this->rolloffFactor);
	alCall(alSourcef, this->id, AL_REFERENCE_DISTANCE, this->referenceDistance);
	alCall(alSourcef, this->id, AL_MAX_DISTANCE, this->maxDistance);
	alCall(alSourcei, this->id, AL_BUFFER, buffer);

	this->state = AL_FALSE;
}

Source::~Source() 
{
	alCall(alDeleteSources, 1, &this->id);
	alCall(alDeleteBuffers, 1, &this->buffer);
}

void Source::play()
{
	alCall(alSourcePlay, this->id);
}

void Source::pause()
{
	alCall(alSourcePause, this->id);
}

void Source::stop()
{
	alCall(alSourceStop, this->id);
}

void Source::setPosition(glm::vec3 position)
{
	this->position = position;
	alCall(alSource3f, this->id, AL_POSITION, this->position.x, this->position.y, this->position.z);
}
