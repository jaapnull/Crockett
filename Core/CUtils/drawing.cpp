template<class Color_T> class ColorPen
{
public:
	void SetShape(PenShape inShape)				{ mShape = inShape; }
	PenShape GetShape()					const	{ return mShape; }
	void SetPattern(uint32 inPattern)			{ mPattern = inPattern; }
	uint32 GetPattern()					const	{ return mPattern; }
	void SetSize(int inSize)					{ mSize = inSize; }
	int GetSize()						const	{ return mSize; }
	void SetColor(const Color_T& inColor)		{ mColor = inColor; }
	Color_T GetColor()					const	{ return mColor; }

	ColorPen(BaseFrame<Color_T>& inCanvas) : mSize(0), mShape(psCircle), mPattern(0xFFFFFFFF), mCanvas(inCanvas), mColor(0, 0, 0) {}

	inline void SetPixel(int x0, int y0)
	{
		mCanvas.Set(x0, y0, mColor);
	}

	bool RotatePattern(unsigned int steps)
	{
		for (unsigned int x = 0; x < steps; x++)
		{
			if (mPattern & 1)
			{
				mPattern >>= 1;
				mPattern |= 0x80000000;
			}
			else
			{
				mPattern >>= 1;
			}
		}
		return (mPattern & 1) != 0;
	}

	void DrawDot(int x, int y)
	{
		if (mPattern & 1)
		{
			for (int cx = -mSize; cx <= mSize; cx++)
			{
				for (int cy = -mSize; cy <= mSize; cy++)
				{
					switch (mShape)
					{
					case psCross:
						if (cx == 0 || cy == 0)
							SetPixel(x + cx, y + cy); break;
					case psCircle:
						if (cx*cx + cy*cy <= mSize*mSize)
							SetPixel(x + cx, y + cy); break;
					case psSquare:
					default:
						SetPixel(x + cx, y + cy); break;
					}
				}
			}
		}
		RotatePattern(1);
	}

	void DrawCircleQuad(int x0, int y0, int radius, int quad)
	{
		int f = 1 - radius;
		int ddF_x = 1;
		int ddF_y = -2 * radius;
		int x = 0;
		int y = radius;

		while (x < y)
		{
			if (f >= 0)
			{
				y--;
				ddF_y += 2;
				f += ddF_y;
			}
			x++;
			ddF_x += 2;
			f += ddF_x;
			if (quad == 0) DrawDot(x0 + x, y0 - y);
			if (quad == 1) DrawDot(x0 + y, y0 + x);
			if (quad == 2) DrawDot(x0 - x, y0 + y);
			if (quad == 3) DrawDot(x0 - y, y0 - x);

		}
		do
		{
			if (f <= 0)
			{
				y++;
				ddF_y -= 2;
				f -= ddF_y;
			}
			x--;
			ddF_x -= 2;
			f -= ddF_x;
			if (quad == 0) DrawDot(x0 + y, y0 - x);
			if (quad == 1) DrawDot(x0 + x, y0 + y);
			if (quad == 2) DrawDot(x0 - y, y0 + x);
			if (quad == 3) DrawDot(x0 - x, y0 - y);

		} while (x >= 0);
	}

	void DrawCircle(int x0, int y0, int radius)
	{
		//for (unsigned int c = 0; c< 8; c++)
		DrawCircleQuad(x0, y0, radius, 2);
		DrawCircleQuad(x0, y0, radius, 3);
		DrawCircleQuad(x0, y0, radius, 0);
		DrawCircleQuad(x0, y0, radius, 1);
	}

	void DrawCircleQuick(int x0, int y0, int radius)
	{
		int f = 1 - radius;
		int ddF_x = 1;
		int ddF_y = -2 * radius;
		int x = 0;
		int y = radius;

		SetPixel(x0, y0 + radius);
		SetPixel(x0, y0 - radius);
		SetPixel(x0 + radius, y0);
		SetPixel(x0 - radius, y0);

		while (x < y)
		{
			if (f >= 0)
			{
				y--;
				ddF_y += 2;
				f += ddF_y;
			}
			x++;
			ddF_x += 2;
			f += ddF_x;
			SetPixel(x0 + x, y0 + y);
			SetPixel(x0 - x, y0 + y);
			SetPixel(x0 + x, y0 - y);
			SetPixel(x0 - x, y0 - y);
			SetPixel(x0 + y, y0 + x);
			SetPixel(x0 - y, y0 + x);
			SetPixel(x0 + y, y0 - x);
			SetPixel(x0 - y, y0 - x);
		}
	}

	void FillCircle(int x0, int y0, int radius)
	{
		for (int x = -radius; x < radius; x++)
		for (int y = -radius; y < radius; y++)
		{
			if (x*x + y*y <= radius*radius) SetPixel(x0 + x, y0 + y);
		}
	}


	void FillSquare(int x0, int y0, int x1, int y1)
	{
		for (int x = x0; x < x1; x++)
		for (int y = y0; y < y1; y++)
		{
			SetPixel(x, y);
		}
	}

	void DrawSquare(int x0, int y0, int x1, int y1)
	{
		DrawLine(x0, y0, x1, y0);
		DrawLine(x1, y0, x1, y1);
		DrawLine(x1, y1, x0, y1);
		DrawLine(x0, y1, x0, y0);
	}


	void DrawPoly(const Poly<int>& inPoly)
	{
		if (inPoly.size() > 2)
		{
			unsigned int x;
			for (x = 0; x < inPoly.size() - 1; x++)
			{
				DrawLine(inPoly[x].x, inPoly[x].y, inPoly[x + 1].x, inPoly[x + 1].y);
				RotatePattern(31);
			}
			DrawLine(inPoly[x].x, inPoly[x].y, inPoly[0].x, inPoly[0].y);
		}
	}

	void FillPoly(const Poly<int>& inPoly)
	{
		if (inPoly.size() > 2)
		{
			unsigned int x;
			int min = inPoly[inPoly.size() - 1].y;
			int max = min;
			for (x = 0; x < inPoly.size() - 1; x++)
			{
				max = (inPoly[x].y > max) ? inPoly[x].y : max;
				min = (inPoly[x].y < min) ? inPoly[x].y : min;
			}
			Array<int> intersects;
			for (int y = min; y < max; y++)
			{
				// populate intersects
				for (x = 0; x < inPoly.size(); x++)
				{
					int nextx = (x == inPoly.size() - 1) ? 0 : x + 1;
					if (inPoly[x].y == y && inPoly[nextx].y > y)
					{
						intersects.push_back(inPoly[x].x);
					}
					else if (inPoly[x].y > y && inPoly[nextx].y == y)
					{
						intersects.push_back(inPoly[nextx].x);
					}
					else if ((inPoly[x].y > y) != (inPoly[nextx].y > y))
					{
						float f = float(y - inPoly[x].y) / float(inPoly[nextx].y - inPoly[x].y);
						intersects.push_back(inPoly[x].x + int((float(inPoly[nextx].x - inPoly[x].x) * f) + .5f));
					}
				}
				std::sort(intersects.begin(), intersects.end());
				if (intersects.size() > 1)
				for (unsigned int i = 0; i < intersects.size() - 1; i += 2)
				{
					Color_T* begin_pointer = &(mCanvas.Get(intersects[i], y));
					Color_T* end_pointer = &(mCanvas.Get(intersects[i + 1], y));
					for (Color_T* pointer = begin_pointer; pointer != end_pointer; pointer++)
					{
						*pointer = mColor;
					}
				}
				intersects.clear();
			}
		}
	}

	void DrawLine(int x0, int y0, int x1, int y1)
	{

		/*if (x0 + y0 > y1 + x1)
		{
		std::swap(x0, x1);
		std::swap(y0, y1);
		}*/

		bool steep = abs(y1 - y0) > abs(x1 - x0);
		if (steep)
		{
			std::swap(x0, y0);
			std::swap(x1, y1);
		}
		int deltax = abs(x1 - x0);
		int deltay = abs(y1 - y0);
		int error = deltax / 2;
		int ystep = (y0 < y1) ? 1 : -1;
		int xstep = (x0 < x1) ? 1 : -1;
		int y = y0;
		if (xstep > 0)
		for (int x = x0; x <= x1; x += xstep)
		{
			if (steep) DrawDot(y, x); else DrawDot(x, y);
			error -= deltay;
			if (error < 0)
			{
				y += ystep;
				error += deltax;
			}
		}
		else
		for (int x = x0; x >= x1; x += xstep)
		{
			if (steep) DrawDot(y, x); else DrawDot(x, y);
			error -= deltay;
			if (error < 0)
			{
				y += ystep;
				error += deltax;
			}
		}

	}
	inline void DrawLine(ivec2 inStart, ivec2 inEnd) { DrawLine(inStart.x, inStart.y, inEnd.x, inEnd.y); }



	void DrawLineFast(ivec2 inBegin, ivec2 inEnd)
	{

		if (inBegin.x + inBegin.y > inEnd.x + inEnd.y)
		{
			std::swap(inBegin, inEnd);
		}

		bool steep = abs(inEnd.y - inBegin.y) > abs(inEnd.x - inBegin.x);
		if (steep)
		{
			std::swap(inBegin.x, inBegin.y);
			std::swap(inEnd.x, inEnd.y);
		}
		ivec2 delta = inEnd - inBegin;
		int error = delta.x / 2;

		ivec2 step;
		step.x = delta.x > 0 ? 1 : -1;
		step.y = delta.y > 0 ? 1 : -1;
		int y = inBegin.y;

		uint color; uint xptrstep; uint yptrstep;
		if (steep)
		{
			color = (uint)&mCanvas.Get(inBegin.y, inBegin.x);
			xptrstep = mCanvas.GetPitch();
			yptrstep = sizeof(Color_T);
		}
		else
		{
			color = (uint)&mCanvas.Get(inBegin.x, inBegin.y);
			xptrstep = sizeof(Color_T);
			yptrstep = mCanvas.GetPitch();
		}

		if (step.x > 0)
		for (int x = inBegin.x; x <= inEnd.x; x += step.x)
		{
			if (RotatePattern(1)) *(Color_T*)color = mColor;
			error -= delta.y;
			if (error < 0)
			{
				y += step.y;
				color += step.y*yptrstep;
				error += delta.x;
			}
			color += step.x*xptrstep;
		}
		else
		for (int x = inBegin.x; x >= inEnd.x; x += step.x)
		{
			if (RotatePattern(1)) *(Color_T*)color = mColor;
			error -= delta.y;
			if (error < 0)
			{
				color += step.y*yptrstep;
				y += step.y;
				error += delta.x;
			}
			color += step.x*xptrstep;
		}

	}