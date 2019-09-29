#pragma once

#include <string>

struct Config
{
	struct Display
	{
		static std::string TITLE;

		static int OPENGL_VERSION_MAJOR, OPENGL_VERSION_MINOR;
		static unsigned short int WIDTH;
		static unsigned short int HEIGHT;

		static int FPS_BUFFER_SIZE;
		static int FPS_CAP;

		static float NEAR_PLANE;
		static float FAR_PLANE;
		static float FOV;
	};

	struct Camera
	{
		static const float MOVEMENT_SPEED;
		static const float LOOK_SPEED;

		static float PITCH_MIN;
		static float PITCH_MAX;
	};
};