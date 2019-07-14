/*
 * MENU SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#ifndef MENU_SCENE_HEADER
#define MENU_SCENE_HEADER

#include <map>
#include <list>
#include <memory>

#include <basics/Canvas>
#include <basics/Id>
#include <basics/Scene>
#include <basics/Texture_2D>
#include <basics/Timer>

#include "GameObject.hpp"

namespace project_template {

    using basics::Id;
    using basics::Timer;
    using basics::Canvas;
    using basics::Texture_2D;

    class Menu_Scene : public basics::Scene {

        // Estos typedefs pueden ayudar a hacer el código más compacto y claro:
        typedef std::shared_ptr<GameObject> GameObject_Handle;
        typedef std::vector<GameObject_Handle> GameObject_List;
        typedef std::shared_ptr<Texture_2D> Texture_Handle;
        typedef std::map<Id, Texture_Handle> Texture_Map;
        typedef basics::Graphics_Context::Accessor Context;

        //Representa el estado de la escena
        enum State {
            LOADING,
            READY,
            ERROR
        };

        //Estructura que almacena informacion de la textura y de la ruta a buscar (Las imagenes)
        static struct Texture_Data {
            Id id;
            const char* path;
        };
        //Array que almacena la informacion de las texturas
        static Texture_Data textures_data[];

        //Numero de texturas en el array de texture_data
        static unsigned textures_count;

    private:

        //EStado de la escena
        State state;
        //Cuando la escena está en segundo plano
        bool suspended;
        //Cuando se muestran las intstrucciones por pantalla
        bool showing_instructions;

        //Resolucion del canvas
        unsigned canvas_width;
        unsigned canvas_height;
        bool aspect_ratio_adjusted;
        float real_aspect_ratio;

        //Punteros a las texturas creadas
        Texture_Map textures;
        //Lista de gameobjects que se utilizan en la escena
        GameObject_List objects;

        Timer timer;

        //Puntero al boton de jugar
        GameObject* play_button_pointer;
        //Puntero al boton de como jugar
        GameObject* instructions_button_pointer;
        //PUntero a la informacion
        GameObject* instructions_text_pointer;
        //Puntero al logo
        GameObject* logo_pointer;

    public:
        Menu_Scene();

       //Devuelve las dimensiones del canvas
        basics::Size2u get_view_size() override
        {
            return { canvas_width, canvas_height };
        }

        //Se reinician los atributos
        bool initialize() override;

   //Si el juego esta en 2 plano
        void suspend() override
        {
            suspended = true;
        }

      //Si el uego vuelve a esta en 1 plano
        void resume() override
        {
            suspended = false;
        }

            /**
           * Este método se invoca automáticamente una vez por fotograma cuando se acumulan
           * eventos dirigidos a la escena.
           * */
        void handle(basics::Event& event) override;

             /**
            * Este método se invoca automáticamente una vez por fotograma para que la escena
            * actualize su estado.
            */
        void update(float time) override;

            /**
            * Este método se invoca automáticamente una vez por fotograma para que la escena
            * dibuje su contenido.
            */
        void render(Context& context) override;

    private:
            /**
            * En este método se cargan las texturas (una cada fotograma para facilitar que la
            * propia carga se pueda pausar cuando la aplicación pasa a segundo plano).
            */
        void load_textures();

            /**
            * Ajusta el aspect ratio al dispositivo
            */
        void adjust_aspect_ratio(Context& context);

            /**
            * En este método se crean los gameobjects cuando termina la carga de texturas.
            */
        void create_gameobjects();

            /**
            * Actualiza el estado del juego cuando el estado de la escena es READY.
            */
        void run_simulation(float time);

           /**
           * Dibuja la escena del menú cuando el estado de la escena es READY.
           * @param canvas Referencia al Canvas con el que dibujar.
           */
        void render_playfield(Canvas& canvas);

            /**
            * Empieza la partida
            */
        void play();

            /**
            * Muestra las instrucciones
            */
        void show_instructions(bool _showing);
    };
}

#endif
