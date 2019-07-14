#ifndef GAMEOBJECT_HEADER
#define GAMEOBJECT_HEADER

#include <memory>
#include <basics/Canvas>
#include <basics/Texture_2D>
#include <basics/Vector>

namespace project_template {

    using basics::Canvas;
    using basics::Size2f;
    using basics::Point2f;
    using basics::Vector2f;
    using basics::Texture_2D;

    //Todos los elementos que aparezcan en pantalla heredaran de esta clase
    class GameObject {
    protected:

          //Puntero a la textura de esta clase
        Texture_2D* texture;
        //Lugar donde se crea la textura en x e y
        int anchor;

        //Tamaño del objeto
        Size2f size;
        //Posicion
        Point2f position;
        //Escala del objeto - Por defecto es 0.3
        float scale;
        //aspect_ratio
        float aspect_ratio;

        //Velocidad del gameobject, si se mueve
        Vector2f speed;

        //Indica si es visible en la escena
        bool visible;

    public:

        //Constructor que determina algunos parametros del gameobject. Si no se declara una escala, se pone la de 0.3
        GameObject(Texture_2D* texture, float aspect_ratio, float _scale = 0.3f);

 //Destructor de la clase
        virtual ~GameObject() = default;

    public:
        // Getters (con nombres autoexplicativos):

        const Size2f& get_size() const { return size; }
        const float& get_width() const { return size.width; }
        const float& get_height() const { return size.height; }
        const Point2f& get_position() const { return position; }
        void set_sullScreen(){ size = { texture->get_width() * scale * aspect_ratio, texture->get_height() * scale * aspect_ratio }; };
        const float& get_position_x() const { return position[0]; }
        const float& get_position_y() const { return position[1]; }
        const Vector2f& get_speed() const { return speed; }
        const float& get_speed_x() const { return speed[0]; }
        const float& get_speed_y() const { return speed[1]; }

        float get_left_x() const
        {
            return (anchor & 0x3) == basics::LEFT ? position[0] : (anchor & 0x3) == basics::RIGHT ? position[0] - size[0] : position[0] - size[0] * .5f;
        }

        float get_right_x() const
        {
            return get_left_x() + size.width;
        }

        float get_bottom_y() const
        {
            return (anchor & 0xC) == basics::BOTTOM ? position[1] : (anchor & 0xC) == basics::TOP ? position[1] - size[1] : position[1] - size[1] * .5f;
        }

        float get_top_y() const
        {
            return get_bottom_y() + size.height;
        }

        bool is_visible() const
        {
            return visible;
        }

        bool is_not_visible() const
        {
            return !visible;
        }

    public:
        // Setters (con nombres autoexplicativos):

        void set_anchor(int new_anchor)
        {
            anchor = new_anchor;
        }

        void set_position(const Point2f& new_position)
        {
            position = new_position;
        }

        void set_position_x(const float& new_position_x)
        {
            position.coordinates.x() = new_position_x;
        }

        void set_position_y(const float& new_position_y)
        {
            position.coordinates.y() = new_position_y;
        }

        void set_scale(float new_scale)
        {
            scale = new_scale;
            size = { texture->get_width() * scale, texture->get_height() * scale * aspect_ratio };

        }

        void set_speed(const Vector2f& new_speed)
        {
            speed = new_speed;
        }

        void set_speed_x(const float& new_speed_x)
        {
            speed.coordinates.x() = new_speed_x;
        }

        void set_speed_y(const float& new_speed_y)
        {
            speed.coordinates.y() = new_speed_y;
        }

        void set_texture(Texture_2D* _texture)
        {
            texture = _texture;
        }

    public:
     //Hace el objeto invisible
        void hide()
        {
            visible = false;
        }

      //Hace el objeto visible
        void show()
        {
            visible = true;
        }

    public:

    //Comprueba si un punto está dentro de otro
        bool contains(const Point2f& point);

    public:
       //Actualiza la posicion del objeto
        virtual void update(float time)
        {
            if (visible) {
                Vector2f displacement = speed * time;

                position.coordinates.x() += displacement.coordinates.x();
                position.coordinates.y() += displacement.coordinates.y();
            }
        }

        //Renderiza el objeto para mostrarlo por pantalla
        virtual void render(Canvas& canvas)
        {
            if (visible) {
                canvas.fill_rectangle(position, size, texture, anchor);
            }
        }
    };
}

#endif
