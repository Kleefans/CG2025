#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include "CanvasPoint.h"
#include "Colour.h"
#include <Utils.h>
#include <fstream>
#include <vector>

#define WIDTH 320
#define HEIGHT 240
std::vector<float> interpolateSingleFloats(float, float, int);
std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfSteps);
void drawTask2Test(DrawingWindow &window);
std::vector<CanvasTriangle> devideTriangle(const CanvasTriangle &triangle);

void draw(DrawingWindow &window) {
	window.clearPixels();
	/*auto leftStartList = interpolateThreeElementValues(
		glm::vec3(255.0, 0.0, 0.0),
		glm::vec3(255.0, 255.0, 0.0),
		window.height);
	auto rightStartList = interpolateThreeElementValues(
		glm::vec3(0.0, 0.0, 255.0),
		glm::vec3(0.0, 255.0, 0.0),
		window.height);
	
	for (size_t y = 0; y < window.height; y++) {
		auto ColurList = interpolateThreeElementValues(
			leftStartList[y],
			rightStartList[y],
			window.width);
		for (size_t x = 0; x < window.width; x++) {
			float red = ColurList[x][0];
			float green = ColurList[x][1];
			float blue = ColurList[x][2];
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}*/
		glm::vec3 v0 = glm::vec3(255.0, 0.0, 0.0);
		glm::vec3 v1 = glm::vec3(0.0, 255.0, 0.0);
		glm::vec3 v2 = glm::vec3(0.0, 0.0, 255.0);

		for (size_t y = 0; y < window.height; y++) {
			for (size_t x = 0; x < window.width; x++) {
				auto pointWeight = convertToBarycentricCoordinates(
					glm::vec2(0, window.height - 1),
					glm::vec2(window.width/2, 0),
					glm::vec2(window.width -1, window.height -1),
					glm::vec2(x, y)
				);
				if (pointWeight.x < 0 || pointWeight.y < 0 || pointWeight.z < 0) continue;
				glm::vec3 pointColour = pointWeight.x * v1 + pointWeight.y * v2 + pointWeight.z * v0;
				uint32_t colour = (255 << 24) + (int(pointColour.x) << 16) + (int(pointColour.y) << 8) + int(pointColour.z);
				window.setPixelColour(x, y, colour);
			}
	}
}


void drawLine (DrawingWindow &window, CanvasPoint from, CanvasPoint to, Colour colour) {
	float dx = to.x - from.x;
	float dy = to.y - from.y;
	int steps = std::max(std::abs(dx), std::abs(dy));
	float xIncrement = dx / float(steps);
	float yIncrement = dy / float(steps);
	float x = from.x;
	float y = from.y;
	for (int i = 0; i <= steps; i++) {
		window.setPixelColour(round(x), round(y),
			(255 << 24) + (colour.red << 16) + (colour.green << 8) + colour.blue);
		x += xIncrement;
		y += yIncrement;
	}
}		

void drawStrokedTriangle(DrawingWindow &window, CanvasTriangle triangle, Colour colour){
	drawLine(window, triangle.v0(), triangle.v1(), colour);
	drawLine(window, triangle.v1(), triangle.v2(), colour);
	drawLine(window, triangle.v2(), triangle.v0(), colour);
}

void drawFilledTriangle(DrawingWindow &window, CanvasTriangle triangle, Colour colour){
	drawStrokedTriangle(window, triangle, Colour(0.0, 0.0, 0.0));
}

std::vector<CanvasTriangle> divideTriangle(const CanvasTriangle &triangle){
	float ypoints[3] = { triangle[0].y, triangle[1].y, triangle[2].y };
	float *midpoint = &ypoints[1];
	float dx_dy = (triangle[2].x - triangle[0].x) / (triangle[2].y - triangle[0].y);
	float x_target_float = triangle[0].x + (ypoints[1] - triangle[0].y) * dx_dy;
}


void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
		else if (event.key.keysym.sym == SDLK_u) drawStrokedTriangle(window,
			CanvasTriangle(CanvasPoint(rand() % WIDTH, rand() % HEIGHT), CanvasPoint(rand() % WIDTH, rand() % HEIGHT), CanvasPoint(rand() % WIDTH, rand() % HEIGHT)),
			Colour(rand() % 256, rand() % 256, rand() % 256));
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

std::vector<float> interpolateSingleFloats(float from, float to, int numberOfSteps) {
	float step = (to - from) / (float(numberOfSteps) - 1);
	std::vector<float> result;
	for (int i = 0; i < numberOfSteps; i++) {
		result.push_back(from + step * float(i));
	}
	return result;
}	

std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfSteps) {
	glm::vec3 step = (to - from) / (float(numberOfSteps) - 1);
	std::vector<glm::vec3> result;
	for (int i = 0; i < numberOfSteps; i++) {
		result.push_back(from + step * float(i));
	}
	return result;
}

void testInterpolateSingleFloats() {
	std::vector<float> result;
	result = interpolateSingleFloats(2.2, 8.5, 7);
	for(size_t i=0; i<result.size(); i++) std::cout << result[i] << " ";
	std::cout << std::endl;
}

void testInterpolateThreeElementValues() {
	glm::vec3 from(1.0, 4.0, 9.2);
	glm::vec3 to(4.0, 1.0, 9.8);
	auto result = interpolateThreeElementValues(from, to, 4);
	for (size_t i = 0; i < result.size(); i++) {
		std::cout << (float)result[i].x << " " << (float)result[i].y << " " << (float)result[i].z << std::endl;
	}
}

void drawTask2Test(DrawingWindow &window) {
    
    
    // 计算中心点坐标
    CanvasPoint center(WIDTH / 2, HEIGHT / 2); // (160, 120)
    
    // 定义一个颜色来绘制，比如红色 (R, G, B)
    Colour testColour(255, 0, 0);

    // --- 2. 绘制四条测试线 ---

    // A) A line from the top-left corner to the centre
    CanvasPoint topLeft(0, 0);
    drawLine(window, topLeft, center, testColour);

    // B) A line from the top-right corner to the centre
    // 坐标是从0开始的，所以 top-right 是 (WIDTH - 1, 0)
    CanvasPoint topRight(WIDTH - 1, 0); // (319, 0)
    drawLine(window, topRight, center, testColour);

    // C) A vertical line all the way down the middle
    CanvasPoint middleTop(WIDTH / 2, 0); // (160, 0)
    CanvasPoint middleBottom(WIDTH / 2, HEIGHT - 1); // (160, 239)
    drawLine(window, middleTop, middleBottom, testColour);

    // D) A horizontal line a third the width of the screen, centred
    
    int oneThirdWidth = WIDTH / 3; // 320 / 3 = 106 (使用整数除法)
    
    // 水平居中：从中心点向左和向右各移动 (1/3宽度 / 2)
    int startX = center.x - (oneThirdWidth / 2); // 160 - (106 / 2) = 160 - 53 = 107
    int endX = center.x + (oneThirdWidth / 2);   // 160 + (106 / 2) = 160 + 53 = 213
    
    // 垂直居中：Y 坐标就是中心点的 Y
    int centerY = center.y; // 120

    CanvasPoint horizontalStart(startX, centerY); // (107, 120)
    CanvasPoint horizontalEnd(endX, centerY);   // (213, 120)
    
    drawLine(window, horizontalStart, horizontalEnd, testColour);
}

void test() {
	testInterpolateSingleFloats();
	testInterpolateThreeElementValues();
}

int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;
	test();
	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
