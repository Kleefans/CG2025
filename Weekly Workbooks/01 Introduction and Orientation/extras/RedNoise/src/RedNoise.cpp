#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include "CanvasPoint.h"
#include "Colour.h"
#include "TextureMap.h"
#include <Utils.h>
#include <algorithm>
#include <fstream>
#include <vector>

#define WIDTH 320
#define HEIGHT 240
std::vector<float> interpolateSingleFloats(float, float, int);
std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfSteps);
void drawTask2Test(DrawingWindow &window);
std::vector<CanvasTriangle> divideTriangle(const CanvasTriangle &triangle);
void drawFilledTriangleWorker(DrawingWindow &window, CanvasTriangle triangle, Colour colour);
void drawTexturedTriangleWorker(DrawingWindow &window, CanvasTriangle triangle, const TextureMap& texture);
void drawTexturedTriangle(DrawingWindow &window, CanvasTriangle triangle, const TextureMap& texture);

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
	std::vector<CanvasTriangle> subTriangles = divideTriangle(triangle);
	for (auto subTriangle : subTriangles) {
		drawFilledTriangleWorker(window, subTriangle, colour);
	}
	drawStrokedTriangle(window, triangle, Colour(255,255,255));
}

void drawTexturedTriangle(DrawingWindow &window, CanvasTriangle triangle, const TextureMap& texture){

	std::vector<CanvasTriangle> subTriangles = divideTriangle(triangle);
	for (auto subTriangle : subTriangles) {
		drawTexturedTriangleWorker(window, subTriangle, texture);
	}
}

void drawTexturedTriangleWorker(DrawingWindow &window, CanvasTriangle triangle, const TextureMap& texture){
	CanvasPoint p1 = (triangle[1].x < triangle[2].x) ? triangle[1] : triangle[2];
	CanvasPoint p2 = (triangle[1].x >= triangle[2].x) ? triangle[1] : triangle[2];

	if (triangle.v1().y - triangle.v0().y == 0 || triangle.v2().y - triangle.v0().y == 0) {
        return; 
    }

	TexturePoint vT0 = triangle.v0().texturePoint;
	TexturePoint vT1 = triangle.v1().texturePoint;
	TexturePoint vT2 = triangle.v2().texturePoint;

	float dx_dy_1 = (p1.x - triangle.v0().x) / (p1.y - triangle.v0().y);
	float dx_dy_2 = (p2.x - triangle.v0().x) / (p2.y - triangle.v0().y);
	float yTop = std::min(triangle.v0().y, std::max(triangle.v1().y, triangle.v2().y));
	float yBottom = std::max(triangle.v0().y, std::min(triangle.v1().y, triangle.v2().y));
	for (int y = round(yTop); y <= round(yBottom); y++){
		float x1 = triangle.v0().x + (y - triangle.v0().y) * dx_dy_1;
		float x2 = triangle.v0().x + (y - triangle.v0().y) * dx_dy_2;
		for (int x = round(x1); x <= round(x2); x++) {
			glm::vec3 weights = convertToBarycentricCoordinates(
				glm::vec2(triangle.v0().x, triangle.v0().y),
				glm::vec2(triangle.v1().x, triangle.v1().y),
				glm::vec2(triangle.v2().x, triangle.v2().y),
				glm::vec2(x, y)
			);
			float u = weights.x * vT1.x + weights.y * vT2.x + weights.z * vT0.x;
			float v = weights.x * vT1.y + weights.y * vT2.y + weights.z * vT0.y;
			int u_int = round(u);
			int v_int = round(v);
			if(u_int >= 0 && u_int < static_cast<int>(texture.width) && v_int >= 0 && v_int < static_cast<int>(texture.height)){
				int index = v_int * texture.width + u_int;
				window.setPixelColour(x, y, texture.pixels[index]);
			}
    	}
	}
}

void drawFilledTriangleWorker(DrawingWindow &window, CanvasTriangle triangle, Colour colour){
	CanvasPoint p1 = (triangle[1].x < triangle[2].x) ? triangle[1] : triangle[2];
	CanvasPoint p2 = (triangle[1].x > triangle[2].x) ? triangle[1] : triangle[2];

	if (triangle.v1().y - triangle.v0().y == 0 || triangle.v2().y - triangle.v0().y == 0) {
        return; 
    }

	float dx_dy_1 = (p1.x - triangle.v0().x) / (p1.y - triangle.v0().y);
	float dx_dy_2 = (p2.x - triangle.v0().x) / (p2.y - triangle.v0().y);
	float yTop = std::min(triangle.v0().y, std::max(triangle.v1().y, triangle.v2().y));
	float yBottom = std::max(triangle.v0().y, std::min(triangle.v1().y, triangle.v2().y));
	for (int y = round(yTop); y <= round(yBottom); y++){
		float x1 = triangle.v0().x + (y - triangle.v0().y) * dx_dy_1;
		float x2 = triangle.v0().x + (y - triangle.v0().y) * dx_dy_2;
		for (int x = round(x1); x <= round(x2); x++) {
        uint32_t packed = (255 << 24) | (colour.red << 16) | (colour.green << 8) | colour.blue;
        window.setPixelColour(x, y, packed);
    }
	}
}

std::vector<CanvasTriangle> divideTriangle(const CanvasTriangle &triangle){
	std::array<CanvasPoint,3> vertices = {triangle[0], triangle[1], triangle[2]};
	std::sort(vertices.begin(),vertices.end(),[](CanvasPoint& a, CanvasPoint& b){return a.y < b.y;});
	

	if (vertices[0].y == vertices[2].y) return {};
	if (vertices[0].y == vertices[1].y || vertices[1].y == vertices[2].y){
		if(vertices[0].y == vertices[1].y) return {CanvasTriangle(vertices[2], vertices[0], vertices[1])};
		if(vertices[1].y == vertices[2].y) return {CanvasTriangle(vertices[0], vertices[1], vertices[2])};
	}
	
	float dx_dy = (vertices[2].x - vertices[0].x) / (vertices[2].y - vertices[0].y);
	float x_target_float = vertices[0].x + (vertices[1].y - vertices[0].y) * dx_dy;
	float yDiffTotal = vertices[2].y - vertices[0].y;
	float yDiffMid = vertices[1].y - vertices[0].y;
	
	float du_dv = (vertices[2].texturePoint.x - vertices[0].texturePoint.x) / yDiffTotal;
	float dv_du = (vertices[2].texturePoint.y - vertices[0].texturePoint.y) / yDiffTotal;

	float textureU = vertices[0].texturePoint.x + (yDiffMid * du_dv);
	float textureV = vertices[0].texturePoint.y + (yDiffMid * dv_du);
	
	CanvasPoint newPoint = CanvasPoint(x_target_float, vertices[1].y);
	newPoint.texturePoint = TexturePoint(textureU, textureV);

	CanvasTriangle top = CanvasTriangle(vertices[0], vertices[1], newPoint);
	CanvasTriangle bottom = CanvasTriangle(vertices[2], newPoint, vertices[1]);
	return {top, bottom};
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
		else if (event.key.keysym.sym == SDLK_f) drawFilledTriangle(window,
			CanvasTriangle(CanvasPoint(rand() % WIDTH, rand() % HEIGHT), CanvasPoint(rand() % WIDTH, rand() % HEIGHT), CanvasPoint(rand() % WIDTH, rand() % HEIGHT)),
			Colour(rand() % 256, rand() % 256, rand() % 256));
		else if (event.key.keysym.sym == SDLK_t) {
			TextureMap texture("assets/texture.ppm");
			CanvasPoint v0(160, 10);
			CanvasPoint v1(300, 230);
			CanvasPoint v2(10, 150);
			v0.texturePoint = TexturePoint(195, 5);
    		v1.texturePoint = TexturePoint(395, 380);
    		v2.texturePoint = TexturePoint(65, 330);
			CanvasTriangle triangle(v0, v1, v2);
			drawTexturedTriangle(window, triangle, texture);
		}
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
    CanvasPoint middleTop(static_cast<float>(WIDTH) / 2, 0); // (160, 0)
    CanvasPoint middleBottom(static_cast<float>(WIDTH) / 2, HEIGHT - 1); // (160, 239)
    drawLine(window, middleTop, middleBottom, testColour);

    // D) A horizontal line a third the width of the screen, centred
    
    int oneThirdWidth = WIDTH / 3; // 320 / 3 = 106 (使用整数除法)
    
    // 水平居中：从中心点向左和向右各移动 (1/3宽度 / 2)
    int startX = center.x - (static_cast<float>(oneThirdWidth) / 2); // 160 - (106 / 2) = 160 - 53 = 107
    int endX = center.x + (static_cast<float>(oneThirdWidth) / 2);   // 160 + (106 / 2) = 160 + 53 = 213
    
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
