#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <vector>
#include "SFML/Graphics.hpp"

struct vec3d
{
	float x, y, z;
};

struct triangle
{
	vec3d p[3];
};

struct mesh
{
	std::vector<triangle> tris;
};

struct mat4x4
{
	float m[4][4] = { 0 };
};

class Game
{
public:
	Game()
	{
		setWindow();
		
		meshCube.tris = {
			{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },
                                                  
			{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
			{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

			{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
			{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },
                                                 
			{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },
                                               
			{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },
                                                
			{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
		};

		float fNear = 0.1f;
		float fFar = 1000.0f;
		float fFov = 90.0f;
		float fAspectRatio = (float)window.getSize().y / (float)window.getSize().x;
		float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

		matProj.m[0][0] = fAspectRatio * fFovRad;
		matProj.m[1][1] = fFovRad;
		matProj.m[2][2] = fFar / (fFar - fNear);
		matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
		matProj.m[2][3] = 1.0f;
		matProj.m[3][3] = 0.0f;

		startGameLoop();
	}
private:
	int frames = 0, updates = 0;
	sf::RenderWindow window;
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();
	mesh meshCube;
	mat4x4 matProj;
	vec3d vCamera;

	void update()
	{
		updates++;
	}

	void draw()
	{
		window.clear(sf::Color::Black);

		float fTheta = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::high_resolution_clock::now() - startTime).count();
		mat4x4 matRotZ, matRotX;

		matRotZ.m[0][0] = cosf(fTheta);
		matRotZ.m[0][1] = sinf(fTheta);
		matRotZ.m[1][0] = -sinf(fTheta);
		matRotZ.m[1][1] = cosf(fTheta);
		matRotZ.m[2][2] = 1;
		matRotZ.m[3][3] = 1;

		matRotX.m[0][0] = 1;
		matRotX.m[1][1] = cosf(fTheta * 0.5f);
		matRotX.m[1][2] = sinf(fTheta * 0.5f);
		matRotX.m[2][1] = -sinf(fTheta * 0.5f);
		matRotX.m[2][2] = cosf(fTheta * 0.5f);
		matRotX.m[3][3] = 1;

		for (auto tri : meshCube.tris)
		{
			triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

			MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
			MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
			MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);

			MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
			MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
			MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

			triTranslated = triRotatedZX;
			triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
			triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
			triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

			vec3d normal, line1, line2;
			line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
			line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
			line1.z = triTranslated.p[1].z - triTranslated.p[0].z;

			line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
			line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
			line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

			normal.x = line1.y * line2.z - line1.z * line2.y;
			normal.y = line1.z * line2.x - line1.x * line2.z;
			normal.z = line1.x * line2.y - line1.y * line2.x;

			float l = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
			normal.x /= l; normal.y /= l; normal.z /= l;

			if (normal.x * (triTranslated.p[0].x - vCamera.x) +
				normal.y * (triTranslated.p[0].y - vCamera.y) +
				normal.z * (triTranslated.p[0].z - vCamera.z) < 0.0f)
			{
				MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
				MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
				MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

				triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
				triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
				triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
				triProjected.p[0].x *= 0.5f * (float)window.getSize().x;
				triProjected.p[0].y *= 0.5f * (float)window.getSize().y;
				triProjected.p[1].x *= 0.5f * (float)window.getSize().x;
				triProjected.p[1].y *= 0.5f * (float)window.getSize().y;
				triProjected.p[2].x *= 0.5f * (float)window.getSize().x;
				triProjected.p[2].y *= 0.5f * (float)window.getSize().y;

				drawTriangle(triProjected.p[0].x, triProjected.p[0].y, triProjected.p[1].x, triProjected.p[1].y, triProjected.p[2].x, triProjected.p[2].y, window, sf::Color::White);
			}
		}


		window.display();
		frames++;
	}

	void startGameLoop()
	{
		std::thread gameThread(&Game::startUpdateLoop, this);
		while (window.isOpen())
		{
			sf::Event e;
			while (window.pollEvent(e))
			{
				switch (e.type)
				{
				case sf::Event::Closed:
					window.close();
				}
			}

			draw();
		}
		gameThread.join();
	}

	void startUpdateLoop()
	{
		auto cTime = std::chrono::high_resolution_clock::now(), pTime = std::chrono::high_resolution_clock::now(), pCTime = std::chrono::high_resolution_clock::now(), cCTime = std::chrono::high_resolution_clock::now();
		float deltaU = 0.f, timePerUpdate = 1000000000.0f / 200.0f;

		while (window.isOpen())
		{
			cTime = std::chrono::high_resolution_clock::now();
			deltaU += std::chrono::duration_cast<std::chrono::duration<float, std::nano>>(cTime - pTime).count() / timePerUpdate;
			pTime = cTime;

			if (deltaU >= 1)
			{
				deltaU--;
				update();
			}

			cCTime = std::chrono::high_resolution_clock::now();
			if (std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(cCTime - pCTime).count() > 1000)
			{
				std::cout << "FPS : " << frames << " | " << "UPS : " << updates << std::endl;
				frames = 0; updates = 0;
				pCTime = cCTime;
			}
		}
	}

	void setWindow()
	{
		window.create(sf::VideoMode(1024, 960), "3D Engine");
		window.setVerticalSyncEnabled(true);
	}

	void MultiplyMatrixVector(vec3d& i, vec3d& o, mat4x4& m)
	{
		o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
		o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
		o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
		float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

		if (w != 0.0f)
		{
			o.x /= w; o.y /= w; o.z /= w;
		}
	}

	void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, sf::RenderWindow& window, sf::Color color)
	{
		sf::VertexArray line(sf::Lines, 2);
		line[0].color = color;
		line[1].color = color;

		line[0].position.x = x1; line[0].position.y = y1;
		line[1].position.x = x2; line[1].position.y = y2;
		window.draw(line);

		line[0].position.x = x3; line[0].position.y = y3;
		line[1].position.x = x2; line[1].position.y = y2;
		window.draw(line);

		line[0].position.x = x1; line[0].position.y = y1;
		line[1].position.x = x3; line[1].position.y = y3;
		window.draw(line);
	}
};

int main()
{
	Game game;
	return 0;
}