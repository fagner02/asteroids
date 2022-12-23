#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <algorithm>
#include "Collision.h"

#define PI 3.1415

class Enemy {
  public:
    sf::Sprite obj;
    sf::Vector2f drct = sf::Vector2f(0,0);
    float speed = 5;
    sf::Vector2f generateDrct(float pos, float height, float width, bool inverse) {
      float drctX;
      float drctY;

      if(pos > height/3 && pos < height*2/3){
        float hypt = sqrt(pow(height/2, 2) + pow(width, 2));
        float angle1 = asin(width/hypt);

        float angle = rand() % (int)(((PI - angle1*2) + angle1) + 1);

        drctX = sin(angle); 
        drctY = sqrt(1 - pow(drctX, 2));

        if(inverse) {
          std::swap(drctX, drctY);
        }
 
        float nrml = ((drctX > drctY)? drctX : drctY) * 10;
        drctX /= nrml;
        drctY /= nrml;

      } else {
        float hypt1 = sqrt(pow(width*2, 2) + pow(height, 2));
        float hypt2 = sqrt(pow(width/2, 2) + pow(height, 2));
        float angle1 = asin((width*2)/hypt1);
        float angle2 = asin((width/2)/hypt2);
        float angle = (rand() % (int)(angle1 - angle2 + 1)) + angle2;

        drctY = sin(angle); //(rand() % ((side2/2)*3 + 1)) + side2/2;
        drctX = sqrt(1 - pow(drctY, 2)); //sqrt(pow(hypt, 2) - pow(dirctY, 2));

        if(inverse) {
          std::swap(drctX, drctY);
        }

        float nrml = ((drctX > drctY)? drctX : drctY);
        drctX /= nrml;
        drctY /= nrml;
      }

      return sf::Vector2f(drctX, drctY);
    }

    void setDrctPos(float width, float height){
      float posX = rand() % (int)(width + 1);
      float posY = rand() % (int)(height + 1);

      if(rand() % 2) {
        double half = height / 2;
        posX = width * (rand() % 2);
        this->drct = sf::Vector2f(1,posY/half);
        // this->drct = generateDrct(posY, height, width, true);
        if (posY > height/2) {
          this->drct.y *= -1;
        }
        if (posX > 0) {
          this->drct.x *= -1;
        }
      } else {
        double half = width / 2;
        posY = height * (rand() % 2);
        
        this->drct = sf::Vector2f(posX/half,1);
        // this->drct = generateDrct(posX, width, height, false);
        if (posY > 0) {
          this->drct.y *= -1;
        }
        if (posX > width/2) {
          this->drct.x *= -1;
        }
      }

      this->obj.setPosition(posX, posY);
    }
    
    void update(float delta) {
      this->obj.move(this->drct * speed * delta);
      this->obj.rotate(5 * delta);
    }
    
    void draw(sf::RenderWindow &window) {
      window.draw(this->obj);
    }

    Enemy(sf::Sprite spr, float width, float height) : obj(spr){
      this->setDrctPos(width, height);
    }
};
sf::Texture n;

template<typename T>
void adjust(T& obj);
template<typename T>
void adjust(T& obj) {
    sf::Vector2f playerSize(obj.getLocalBounds().width, obj.getLocalBounds().height);
    obj.setOrigin((playerSize.x / 2), (playerSize.y / 2));
    obj.move(obj.getOrigin());
}

bool testAngle(float curRotation, float angle);
bool testAngle(float curRotation, float angle) {
  int limitDeviation = 3;
  if(curRotation >= angle - limitDeviation && curRotation <= angle + limitDeviation) {
    return true;
  }
  if(angle > 359){
    if(curRotation <= angle + limitDeviation - 360 || curRotation >= angle - limitDeviation) {
      return true;
    }
  }
  if(angle < limitDeviation) {
    if(curRotation <= angle + limitDeviation || curRotation >= angle - limitDeviation + 360) {
      return true;
    }
  }
  return false;
}
/**
 * Returns a vector with the bullet's X and Y direction coordinates
*/
sf::Vector2f bulletDirection(float rotation);
sf::Vector2f bulletDirection(float rotation) {
  float cosn = cos(rotation / (180 / PI));
  float angle = rotation / (180 / PI);
  float hypt = 10;
  float sideY = abs(cos(angle) * hypt); 
  float sideX = abs(sin(angle) * hypt);

  if(rotation >= 0 && rotation < 90) {
    sideY *= -1;
  }
  if(rotation >= 180 && rotation < 270) {
    sideX *= -1;
  }
  if(rotation >= 270 && rotation < 360) {
    sideY *= -1;
    sideX *= -1;
  }

  return sf::Vector2f(sideX, sideY);
}

class Player {
  sf::RenderWindow& window;
  float speed = 5;

  public:
    sf::Sprite sprite;
    sf::Vector2f playerSize;

    Player(sf::RenderWindow& window, sf::Texture& texture) : window(window), sprite(texture) {
      adjust(this->sprite);
      this->sprite.setColor(sf::Color::Cyan);
      this->sprite.setPosition(100, 100);
      this->sprite.setScale(0.1, 0.1);
      sf::Vector2f(sprite.getGlobalBounds().width * sprite.getScale().x, sprite.getGlobalBounds().height * sprite.getScale().y);
    }

    void move(float delta) {
      sf::Vector2f mousePos(sf::Mouse::getPosition(window));
      sf::Vector2f playerPos = sprite.getPosition();
      float sideX = mousePos.x - playerPos.x;
      float sideY = mousePos.y - playerPos.y;

      int limitDeviation = 2;
      float pMax = (abs(sideX) > abs(sideY)) ? abs(sideX) : abs(sideY);
      if (pMax != 0 && pMax > limitDeviation) {
        sf::Vector2f newPos((sideX / pMax) * speed * delta, (sideY / pMax) * speed * delta);
        sprite.move(newPos);
      }
    }

    void rotate(float delta) {
      sf::Vector2f mousePos(sf::Mouse::getPosition(window));
      sf::Vector2f playerPos = sprite.getPosition();
      float sideX = mousePos.x - playerPos.x;
      float sideY = mousePos.y - playerPos.y;
      float hypt = sqrt(pow(sideX, 2) + pow(sideY, 2));
      float angle;

      if (sideX > -1 && sideY < 0)
      {
        angle = (asin(sideX / hypt) * 180.0) / PI;
      }
      if (sideX < 0 && sideY < 0)
      {
        angle = (asin(abs(sideY) / hypt) * 180.0) / PI;
        angle += 270;
      }
      if (sideX < 0 && sideY > -1)
      {
        angle = (asin(abs(sideX) / hypt) * 180.0) / PI;
        angle += 180;
      }
      if (sideX > -1 && sideY > -1)
      {
        angle = (asin(sideY / hypt) * 180.0) / PI;
        angle += 90;
      }

      float curRotation = sprite.getRotation();

      if (!testAngle(curRotation, angle)) {
        float newAngle;

        float clockRotation = angle - curRotation;
        float rclockRotation = 360 - abs(angle - curRotation);
        rclockRotation = (clockRotation < 0) ? rclockRotation : rclockRotation * -1;
        newAngle = (abs(clockRotation) < abs(rclockRotation)) ? clockRotation : rclockRotation;

        newAngle = (newAngle < 0) ? -1 : 1;

        sprite.rotate(newAngle * speed * delta);
        // arrow.setOrigin(10, 20);
        // arrow.rotate(newAngle * delta);
      }
    }
    void draw() {
      window.draw(sprite);
    }
};

class Bullet {
  public:
    sf::Sprite sprite;
    sf::Vector2f direction = sf::Vector2f(0, 0);
    bool toErase = false;
    Bullet(){

    }

    void setDirection(float rotation)
    {
      float cosn = cos(rotation / (180 / PI));
      float angle = rotation / (180 / PI);
      float hypt = 10;
      float sideY = abs(cos(angle) * hypt);
      float sideX = abs(sin(angle) * hypt);

      if (rotation >= 0 && rotation < 90)
      {
        sideY *= -1;
      }
      if (rotation >= 180 && rotation < 270)
      {
        sideX *= -1;
      }
      if (rotation >= 270 && rotation < 360)
      {
        sideY *= -1;
        sideX *= -1;
      }

      direction = sf::Vector2f(sideX, sideY);
    }
};

void intersection(std::vector<Enemy>& spawns, Player& player, std::vector<sf::RectangleShape>& life, bool& intersecting) {
  bool intersect = false;
  for (auto &x : spawns)
  {
    if (Collision::PixelPerfectTest(player.sprite, x.obj, 0))
    {
      intersect = true;
    }
  }
  if (intersect)
  {
    if (!intersecting)
    {
      std::cout << "intersecting:\n";
      intersecting = true;

      if (life.size() > 0)
      {
        life.pop_back();
      }
      player.sprite.setColor(sf::Color::Blue);
    }
  }
  else
  {
    intersecting = false;
    player.sprite.setColor(sf::Color::Red);
  }
}

int main() {
    std::setlocale(LC_ALL, "spanish");
    srand(time(NULL));
    sf::RenderWindow window(sf::VideoMode(800, 600), "asteroids");
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);

    sf::Texture triangle;
    if(!triangle.loadFromFile("triangle.png")){
      std::cout << "error loading picture";
    }
    triangle.setSmooth(true);

    sf::Texture circle;
    if(!circle.loadFromFile("circle.png")){
      std::cout << "error loading picture";
    }

    sf::RectangleShape lifePoint(sf::Vector2f(50, 50));
    adjust(lifePoint);
    lifePoint.setFillColor(sf::Color::Green);
    lifePoint.setPosition(window.getSize().x - 35, 35);

    std::vector<sf::RectangleShape> life;

    for(int i = 0; i < 5; i++) {
      life.push_back(sf::RectangleShape(lifePoint));
      life[i].move(-60 * i, 0);
    }

    sf::Sprite spr(triangle);
    adjust(spr);
    spr.setColor(sf::Color::Cyan);
    spr.setPosition(100, 100);
    spr.setScale(0.1, 0.1);

    sf::Texture enmTexture;
    if(!enmTexture.loadFromFile("enemy.png")){
      std::cout << "error loading picture";
    }
    sf::Sprite enmSprite(enmTexture);
    adjust(enmSprite);
    enmSprite.setColor(sf::Color::Cyan);
    enmSprite.setPosition(100, 100);
    enmSprite.setScale(0.1, 0.1);

    Player player(window, triangle);

    sf::Sprite bullet(circle);
    bullet.setScale(0.2, 0.2);

    std::vector<sf::Vector2f> bulletsdirection;

    sf::Font font;
    font.loadFromFile("CozetteVector.ttf");

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(100);
    text.setFillColor(sf::Color::White);

    std::vector<Bullet> bullets{};

    std::vector<Enemy> spawns{};

    bool isPressed = false;
    bool intersecting = false;
    std::cout << intersecting;

    int score { 0 };

    sf::Clock spawn;
    sf::Clock timeBullet;

    sf::Clock frameRate;

    bool rotate = false;
    bool left = false;

    while (window.isOpen()){
        float delta = frameRate.getElapsedTime().asMilliseconds();
        delta /= 1000.0 / 60.0;
        
        frameRate.restart();

        if(spawn.getElapsedTime().asSeconds() >= 3){
          spawn.restart();
          for (int m = 0; m < 3; m++) {
            spawns.push_back(Enemy(enmSprite, window.getSize().x, window.getSize().y));
          }
        }

        std::vector<int> its;

        for (int i = 0; i < (int)spawns.size(); i++)
        {
            sf::Sprite& x = spawns[i].obj;
            bool intersect = false;
            sf::FloatRect enmrect = x.getGlobalBounds();
            for (auto &y : bullets){
                sf::FloatRect rect = y.sprite.getGlobalBounds();

                intersect = rect.intersects(enmrect);
                if(intersect){
                  y.toErase = true;
                  its.push_back(i);
                }
            }
            spawns[i].update(delta);
        }

        for(auto &x : its){
            if(spawns.size() == 0){
                break;
            }
            spawns.erase(spawns.begin()+x);
            score+=10;
        }

        sf::Event evnt;
        
        
        while (window.pollEvent(evnt)){
            if (evnt.type == sf::Event::Closed) {
                window.close();
            }
            if (evnt.type == sf::Event::Resized) {
              sf::FloatRect newView(0,0,evnt.size.width, evnt.size.height);
              window.setView(sf::View(newView));
            }
        }

        intersection(spawns, player, life, intersecting);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
          if(timeBullet.getElapsedTime().asSeconds() >=0.3) {
            timeBullet.restart();
            Bullet temp;
            temp.setDirection(player.sprite.getRotation());
            temp.sprite = bullet;
            temp.sprite.setPosition(player.sprite.getPosition().x - player.playerSize.x / 2, player.sprite.getPosition().y - player.playerSize.y / 2);
            bullets.push_back(temp);
          }
        }
        
          
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
          player.rotate(delta);
          player.move(delta);
        }

        text.setString(std::to_string(score));

        window.clear();

        for (int i = 0; i < (int)bullets.size(); i++) {
            sf::Sprite& x = bullets[i].sprite;
            sf::Vector2f windowSize = sf::Vector2f(window.getSize()) ;
            if((x.getPosition().x > windowSize.x ||
            x.getPosition().y > windowSize.y ||
            x.getPosition().x < 0 ||
            x.getPosition().y < 0) || bullets[i].toErase
            ) {
             bullets.erase(bullets.begin()+i);
              bulletsdirection.erase(bulletsdirection.begin()+i);
              i--;
              continue;
            }
            x.move(bullets[i].direction * delta);
            window.draw(x);
        }

        for(auto& x : life) {
          window.draw(x);
        }

        window.draw(text);

        if (life.empty()) {
          player.sprite.scale(1 - (0.05 * delta), 1 - (0.05 * delta));
        }
        
        if (!life.empty() || (life.empty() && player.sprite.getScale().x > 0.01)) {
            player.draw();
        }

        for (auto &x : spawns) {
            window.draw(x.obj);
        }

        window.display();
    }

    return 0;
}