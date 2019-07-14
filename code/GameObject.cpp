#include "GameObject.hpp"

using namespace basics;

namespace project_template {

    //Constructor del objeto
    GameObject::GameObject(Texture_2D* _texture, float _aspect_ratio, float _scale)
    {
        texture = _texture;
        anchor = basics::CENTER;
        position = { 0.f, 0.f };
        scale = _scale;
        aspect_ratio = _aspect_ratio;
        size = { texture->get_width() * scale, texture->get_height() * scale * aspect_ratio };
        speed = { 0.f, 0.f };
        visible = true;
    }

    //Comprobamos que hay un punto dentro del rectangulo
    bool GameObject::contains(const Point2f& point)
    {
        float this_left = this->get_left_x();

        if (point.coordinates.x() > this_left) {
            float this_bottom = this->get_bottom_y();

            if (point.coordinates.y() > this_bottom) {
                float this_right = this_left + this->size.width;

                if (point.coordinates.x() < this_right) {
                    float this_top = this_bottom + this->size.height;

                    if (point.coordinates.y() < this_top) {
                        return true;
                    }
                }
            }
        }

        return false;
    }
}
