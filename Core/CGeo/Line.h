template <class T, int I>
class Line
{
public:
	Vector<T, I>	mBegin;
	Vector<T, I>	mEnd;
	float			GetLength()				{return (mEnd-mBegin).Length();}
	T				GetLengthSquared()		{return (mEnd-mBegin).LengthSquared();}
	Line(const Vector<T, I>& mBegin, const Vector<T, I>& mEnd) : mBegin(mBegin), mEnd(mEnd) {}

};

template <class T>
class Poly : public Array<Vector<T, 2>>
{
public:

	bool CoversPoint(const Vector<T, 2>& point) const
	{
		int intersects = 0;
		for (unsigned int x = 0; x < size(); x++)
		{		
			int nextx = (x == size() - 1) ? 0 : x + 1;
			if (at(x).y == point.y && at(nextx).y > point.y)
			{
				if (at(x).x <= point.x) intersects++;
			}
			else if (at(x).y > point.y && at(nextx).y == point.y)
			{
				if (at(nextx).x <= point.x) intersects++;				
			}
			else if ((at(x).y > point.y) != (at(nextx).y > point.y))
			{
				float f = float(point.y - at(x).y)/float(at(nextx).y-at(x).y);
				if ((at(x).x + ((float(at(nextx).x - at(x).x) * f))) <= point.x) intersects++;
			}
		}
		return (intersects % 2 == 1);
	}
};



