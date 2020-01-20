#pragma once

//------------
// Description
//--------------
// Basic Renderer Interface for Purposes of the Program. Contains EndFrame which presents to the screen and clear plus a basic DrawCube function.
//------------

class IRenderer
{
public:
	virtual ~IRenderer() {}

	virtual void EndFrame() = 0;
	virtual void Clear(float a_r, float a_g, float a_b) = 0;
	virtual void DrawCube(float a_x, float a_y, float a_z, float a_scale, float a_angle) = 0;
};