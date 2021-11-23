#include <list>
#include "HistogramObject.h"

#pragma once
class Histogram
{
	private:
		bool bucketed;
		std::list<HistogramObject> objects;

	public:
		Histogram();
		bool isBucketed();
		void setBucketed(bool pBucketed);
		std::list<HistogramObject> getObjects();

};

