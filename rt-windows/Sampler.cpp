#include "Sampler.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
struct Color {
	float r, g, b, a;
};
class Sample {
	glm::vec2 count;
public:
	int x;
	int y;

	Sample(int x, int y) {
		this->x = x;
		this->y = y;
	}
	bool makeSample(glm::vec2* sample ){
		if (count[0] < y) {
			*sample = count;
			count[1]++;
			if (count[1] > x) {
				count[1] = 0;
				count[0]++;
			}
			return true;
		}
		else
			return false;
	}
};
	