
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include<list>
using namespace sf; // подключаем пространство имен sf

class Entity {
public:
	std::vector<Entity> obj;
	float dx, dy, x, y, speed;
	int w, h, health;
	bool life;
	Texture texture;
	Sprite sprite;
	String name;
	Entity(Image& image, String Name, float X, float Y, int W, int H) {
		x = X; y = Y; w = W; h = H; name = Name;
		speed = 0; health = 100; dx = 0; dy = 0;
		life = true;
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setOrigin(w / 2, h / 2);
	}

	FloatRect getRect() {
		return FloatRect(x, y, w, h);
	}

	virtual void update(float time) = 0;//все потомки переопределят эту ф-цию

};

class Player :public Entity {
public:
	enum { left, right, up, down} state;
	bool isShoot = false;

	Player(Image& image, String Name, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H){}
	

	void control() {
		if (Keyboard::isKeyPressed) {
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				state = left; speed = 0.1; sprite.setRotation(0);
			}
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				state = right; speed = 0.1; sprite.setRotation(180);
			}

			if (Keyboard::isKeyPressed(Keyboard::Up) ) {
				state = up; speed = -0.1; sprite.setRotation(90);
			}

			if (Keyboard::isKeyPressed(Keyboard::Down)) {
				state = down; speed = 0.1; sprite.setRotation(-90);
			}

			/////выстрел
			if (Keyboard::isKeyPressed(Keyboard::Space)) {
				isShoot = true;
			}
		}
	}

	void update(float time)
	{
		control();
		switch (state)
		{
		case right:dx = speed; dy = 0;  break;
		case left:dx = -speed; dy = 0;  break;
		case up: dx = 0; dy = speed;  break;
		case down: dx = 0; dy = speed;  break;
		
		}
		x += dx * time;
		y += dy * time;

		sprite.setPosition(x, y);
		speed = 0;
	}
};


class Bullet :public Entity {//класс пули
public:
	int direction;//направление пули

	Bullet(Image& image, String Name, float X, float Y, int W, int H, int dir) :Entity(image, Name, X, Y, W, H)
	{
		x = X;
		y = Y;
		direction = dir;
		speed = 0.8;
		w = h = 32;
		life = true;
	}


	void update(float time)
	{
		switch (direction)
		{
		case 0: dx = -speed; dy = 0; sprite.setRotation(-90);  break;
		case 1: dx = speed; dy = 0;  sprite.setRotation(90);   break;
		case 2: dx = 0; dy = -speed; sprite.setRotation(0);  break;
		case 3: dx = 0; dy = speed;  sprite.setRotation(180);   break;
		}

		x += dx * time;//само движение пули по х
		y += dy * time;//по у

		if ((x <= 0)||(y<=0)||(x>=639)||(y>=479))  life = false;

		sprite.setOrigin(16, 16);
		sprite.setPosition(x, y);//задается позицию пуле
	}
};

int main()
{
	RenderWindow window(VideoMode(640, 480), "Tanks");

	Image heroImage;//изображение для танка
	heroImage.loadFromFile("images/Tank.png");//загрузили картинку в объект изображения

	Image BulletImage;//изображение для пули
	BulletImage.loadFromFile("images/bullet.png");//загрузили картинку в объект изображения
	

	std::list<Entity*>  entities;
	std::list<Entity*>::iterator it;

	Player p(heroImage, "Player1", 100, 100, 64, 64);

	Clock clock;
	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asMicroseconds();

		clock.restart();
		time = time / 800;

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (p.isShoot == true) { p.isShoot = false; entities.push_back(new Bullet(BulletImage, "Bullet", p.x, p.y, 16, 16, p.state)); }//если выстрелили, то появляется пуля. enum передаем как int 

		}

		for (it = entities.begin(); it != entities.end();)//говорим что проходимся от начала до конца
		{
			Entity* b = *it;
			b->update(time);
			if (b->life == false) { it = entities.erase(it); delete b; }// если этот объект мертв, то удаляем его
			else it++;//и идем курсором (итератором) к след объекту. так делаем со всеми объектами списка
		}

		p.update(time);// update игрока
		
		window.clear(Color::Green);

		for (it = entities.begin(); it != entities.end(); it++) {
			window.draw((*it)->sprite);
		}
		window.draw(p.sprite);
		window.display();

	}

	return 0;
}
