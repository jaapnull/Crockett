
// line-line, 2D, with intersect result
inline bool intersectLL2(const Line<float, 2>& l1, const Line<float,2>& l2, Vector<float, 2>& intersect)
{
	Vector<float, 2> a,b,c;
	a = l2.mEnd   - l2.mBegin;
	b = l1.mEnd   - l1.mBegin;
	c = l1.mBegin - l2.mBegin;
	float d = c.GetPerpDot(a);
	float e = c.GetPerpDot(b);
	float f = a.GetPerpDot(b);

	if (f > 0)
	{
		if (d < 0 || d > f) return false;
	}
	else
	{
		if (d > 0 || d < f) return false;
	}
	if (f > 0)
	{
		if (e < 0 || e > f) return false;
	}
	else
	{
		if (e > 0 || e < f) return false;
	}
	intersect = l1.mBegin + (b * ((d/f)));
	return true;
}

// line-line, 2D, without intersect result
inline bool intersectLL2(const Line<float, 2>& l1, const Line<float,2>& l2)
{
	Vector<float, 2> a,b,c;
	a = l1.mEnd   - l1.mBegin;
	b = l2.mEnd   - l2.mBegin;
	c = l1.mBegin - l2.mBegin;
	float d = c.GetPerpDot(a);
	float e = c.GetPerpDot(b);
	float f = a.GetPerpDot(b);
	if (f > 0)
	{
		if (d < 0 || d > f) return false;
	}
	else
	{
		if (d > 0 || d < f) return false;
	}
	if (f > 0)
	{
		if (e < 0 || e > f) return false;
	}
	else
	{
		if (e > 0 || e < f) return false;
	}
	return true;
}