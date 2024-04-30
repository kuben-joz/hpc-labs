#include <iostream>


struct Sphere {
	float red, green, blue;
	float radius;
	float x, y, z;

	__device__ float hit(float bitmapX, float bitmapY, float *colorFalloff) {
		float distX = bitmapX - x;
		float distY = bitmapY - y;

		if (distX * distX + distY * distY < radius * radius) { 
			float distZ = sqrtf(radius * radius - distX * distX - distY * distY);
			*colorFalloff = distZ / sqrtf(radius * radius);
			return distZ + z; 
		}

		return -1;
	}
};

int main() {
    std::cout << sizeof(Sphere) << std::endl;
    return 0;
}
