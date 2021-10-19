#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <algorithm>
#include "Collision.h"

#define PI 3.1415

class enemy {
  public:
    sf::Sprite obj;
    sf::Vector2f drct = sf::Vector2f(0,0);
    
    sf::Vector2f generateDrct(float pos1, float pos2, float side1, float side2, bool inverse) {
      float drctX;
      float drctY;
      
      if(pos1 > side1/3 && pos1 < side1*2/3){
        float hypt = sqrt(pow(side1/2, 2) + pow(side2, 2));
        float angle1 = asin(side2/hypt);

        float angle = rand() % (int)(((PI - angle1*2) + angle1) + 1);
        
        drctX = sin(angle); //rand() % (hypt + 1);
        drctY = sqrt(1 - pow(drctX, 2)); //sqrt(pow(hypt, 2) - pow(dirctX, 2));
        
        if(inverse) {
          std::swap(drctX, drctY);
        }
        
        float nrml = ((drctX > drctY)? drctX : drctY) * 10;
        drctX /= nrml;
        drctY /= nrml;
    
      } else {
        float hypt1 = sqrt(pow(side2*2, 2) + pow(side1, 2));
        float hypt2 = sqrt(pow(side2/2, 2) + pow(side1, 2));
        float angle1 = asin((side2*2)/hypt1);
        float angle2 = asin((side2/2)/hypt2);
        float angle = (rand() % (int)(angle1 - angle2 + 1)) + angle2;
       
        drctY = sin(angle); //(rand() % ((side2/2)*3 + 1)) + side2/2;
        drctX = sqrt(1 - pow(drctY, 2)); //sqrt(pow(hypt, 2) - pow(dirctY, 2));
        
        if(inverse) {
          std::swap(drctX, drctY);
        }
       
        float nrml = ((drctX > drctY)? drctX : drctY) * 10;
        drctX /= nrml;
        drctY /= nrml;
      }
      
      return sf::Vector2f(drctX, drctY);
    }
    
    void setDrctPos(float width, float height){
      float posX = rand() % (int)(width + 1);
      float posY = rand() % (int)(height + 1);
      
      if(rand() % 2) {
        posX = width * (rand() % 2);
        this->drct = generateDrct(posY, posX, height, width, true);
        if (posY > height/2) {
          this->drct.y *= -1;
        }
        if (posX > 0) {
          this->drct.x *= -1;
        }
      } else {
        posY = height * (rand() % 2);
        this->drct = generateDrct(posX, posY, width, height, false);
        if (posY > 0) {
          this->drct.y *= -1;
        }
        if (posX > width/2) {
          this->drct.x *= -1;
        }
      }
      
      this->obj.setPosition(posX, posY);
    }
    
    enemy(sf::Sprite obj, float width, float height) {
      this->setDrctPos(width, height);
      this->obj = obj;
    }
};
  
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
  if(curRotation >= angle - 1 && curRotation <= angle + 1) {
    return true;
  }
  if(angle > 359){
    if(curRotation <= angle + 1 - 360 || curRotation >= angle - 1) {
      return true;
    }
  }
  if(angle < 1) {
    if(curRotation <= angle + 1 || curRotation >= angle - 1 + 360) {
      return true;
    }
  }
  return false;
}

sf::Vector2f bulletDirection(float rotation);
sf::Vector2f bulletDirection(float rotation) {
  float cosn = cos(rotation / (180 / PI));
  float hypt = (cosn == 0) ? 10: 10/cosn;
  float sideX = sqrt((hypt*hypt) - (100));
  float sideY = 10;
  
  if(rotation >= 0 && rotation < 90) {
    sideY *= -1;
  }
  if(rotation >= 90 && rotation < 180) {
    sideY *= -1;
    sideX *= -1;
  }
  if(rotation >= 180 && rotation < 270) {
    sideX *= -1;
  }
  
  return sf::Vector2f(sideX, sideY);
}

int main() {
    std::setlocale(LC_ALL, "spanish");
    srand(time(NULL));
    sf::RenderWindow window(sf::VideoMode(800, 600), "asteroids");
    window.setFramerateLimit(60);
    
    sf::Texture triangle;
    triangle.loadFromFile("triangulo.png");
    triangle.setSmooth(true);

    sf::Texture circle;
    circle.loadFromFile("circle.png");
    
    sf::RectangleShape lifePoint(sf::Vector2f(50, 50));
    adjust(lifePoint);
    lifePoint.setFillColor(sf::Color::Green);
    lifePoint.setPosition(window.getSize().x - 35, 10);
    
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

    /*sf::Sprite ponta(circle);
    adjust(ponta);
    ponta.setScale(0.2, 0.2);
    ponta.setPosition(spr.getPosition());*/

    sf::Sprite player = sf::Sprite(spr);
    sf::Vector2f playerSize = sf::Vector2f(player.getGlobalBounds().width * player.getScale(), player.getGlobalBounds().height * player.getScale());

    sf::Sprite bullet(circle);
    bullet.setScale(0.2, 0.2);

    std::vector<sf::Vector2f> bulletsdirection;

    sf::Font font;
    font.loadFromFile("CozetteVector.ttf");

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(100);
    text.setFillColor(sf::Color::White);

    std::vector<sf::Sprite> bullets{};

    std::vector<enemy> spawns;

    bool isPressed = false;
    bool intersecting = false;
    std::cout << intersecting;
    
    int score { 0 };

    sf::Clock spawn;
    sf::Clock timeBullet;
    
    sf::Clock frameRate;
    
    while (window.isOpen()){
        float delta = frameRate.getElapsedTime().asMilliseconds();
        delta /= 16.6667;
        delta *= 5;
        frameRate.restart();
        
        if(spawn.getElapsedTime().asSeconds() >= 10){
          spawn.restart();
          spawns.push_back(enemy(sf::Sprite(spr),
          window.getSize().x, window.getSize().y));
          /*int randX = (rand() % 10) + 1;
          int randY = (rand() % 10) + 1;
          spawns[spawns.size()-1].obj.setPosition(sf::Vector2f(50 + 50 * randX, randY * 50));
          */
        }

        std::vector<int> its;

        for (int i = 0; i < (int)spawns.size(); i++)
        {
            sf::Sprite& x = spawns[i].obj;
            bool intersect = false;
            sf::FloatRect enmrect = x.getGlobalBounds();
            for (auto &y : bullets){
                sf::FloatRect rect = y.getGlobalBounds();

                intersect = rect.intersects(enmrect);
                if(intersect){
                    its.push_back(i);
                }
            }

            x.move(0, 0.1 * delta);
            x.rotate(0.1 * delta);
                        
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

            if (evnt.type == evnt.Closed)
            {
                window.close();
            }
            if (evnt.type == evnt.Resized) {
              sf::FloatRect newView(0,0,evnt.size.width, evnt.size.height);
              window.setView(sf::View(newView));
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A )) {
          if(!isPressed && timeBullet.getElapsedTime().asSeconds() >=2) {
            bulletsdirection.push_back(bulletDirection(player.getRotation()));
            timeBullet.restart();
            bullets.push_back(sf::Sprite(bullet));
            bullets[bullets.size()-1].setPosition(player.getPosition().x - playerSize.x / 2, player.getPosition().y - playerSize.y / 2);
            isPressed = true;
          }
        } else {
          isPressed  = false;
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            //sf::FloatRect playerCoord = player.getGlobalBounds();
            bool intersect = false;
            for (auto &x : spawns) {
                /*sf::FloatRect tempCoord = x.getGlobalBounds();
                intersect = playerCoord.intersects(tempCoord); */
                if(Collision::PixelPerfectTest(player, x.obj, 0)){
                  // std::cout << "intersect";
                  intersect = true;
                }
            }

           if (intersect)
            {
              if(!intersecting) {
                    std::cout<<"intersecting:\n";
                  intersecting = true;
                 
                  if(life.size()>0){
                      life.pop_back();
                  }
                  player.setColor(sf::Color::Blue);
              }
            }
            else
            {
                intersecting = false;
                player.setColor(sf::Color::Red);
            }

            sf::Vector2f mousePos(sf::Mouse::getPosition(window));
            sf::Vector2f playerPos = player.getPosition();
            float sideX = mousePos.x - playerPos.x;
            float sideY = mousePos.y - playerPos.y;
            float hypt = sqrt(pow(sideX, 2) + pow(sideY, 2));
            float angle;

            if (sideX > -1 && sideY < 0)
            {
                angle = (asin(sideX / hypt) * 180.0) / PI;
                angle += 270;
            }
            if (sideX < 0 && sideY < 0)
            {
                angle = (asin(abs(sideY) / hypt) * 180.0) / PI;
                angle += 180;
            }
            if (sideX < 0 && sideY > -1)
            {
                angle = (asin(abs(sideX) / hypt) * 180.0) / PI;
                angle += 90;
            }
            if (sideX > -1 && sideY > -1)
            {
                angle = (asin(sideY / hypt) * 180.0) / PI;
            }

            float curRotation = player.getRotation();

            if(!testAngle(curRotation, angle)) {
                float newAngle;
                
                float clockRotation = angle - curRotation;
                float rclockRotation = 360 - abs(angle - curRotation);
                rclockRotation = (clockRotation < 0)? rclockRotation : rclockRotation * -1;
                newAngle = (abs(clockRotation) < abs(rclockRotation)) ? clockRotation : rclockRotation;
                
                newAngle = (newAngle < 0) ? -0.1 : 0.1;

                player.rotate(newAngle * delta);
            }
            
            if(sideX != 0 && sideY != 0) {
              float pMax = (abs(sideX) > abs(sideY)) ? abs(sideX) : abs(sideY);
              sf::Vector2f newPos(sideX / pMax, sideY / pMax);
              player.move(newPos * delta);
            }
        }

        text.setString(std::to_string(score));
        
        window.clear();

        for (int i = 0; i < (int)bullets.size(); i++) {
            sf::Sprite& x = bullets[i];
            sf::Vector2f windowSize = sf::Vector2f(window.getSize()) ;
            if(x.getPosition().x > windowSize.x ||
            x.getPosition().y > windowSize.y ||
            x.getPosition().x < 0 ||
            x.getPosition().y < 0
            ) {
             bullets.erase(bullets.begin()+i);
              bulletsdirection.erase(bulletsdirection.begin()+i);
              i--;
              continue;
            }
            x.move(bulletsdirection[i]);
            window.draw(x);
        }

        for(auto& x : life) {
          window.draw(x);
        }

        window.draw(text);

        if (life.empty()) {
            player.scale(0.99 * delta, 0.99 * delta);
        }

        if (!life.empty() || (life.empty() && player.getScale().x > 0.01)) {
            window.draw(player);
            //window.draw(ponta);
        }

        for (auto &x : spawns) {
            window.draw(x.obj);
        }

        window.display();
    }

    return 0;
}
