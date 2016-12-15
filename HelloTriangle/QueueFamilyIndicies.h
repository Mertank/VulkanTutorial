#ifndef __QUEUEFAMILYINDICIES_H__
#define __QUEUEFAMILYINDICIES_H__

namespace tut {

struct QueueFamilyIndicies {
	int GraphicsFamily = -1;

	bool IsComplete() {
		return GraphicsFamily >= 0;
	}
};

}

#endif