/*
 * GAME SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#ifndef GAME_SCENE_HEADER
#define GAME_SCENE_HEADER

    #include <map>
    #include <list>
    #include <memory>
#include <string>


#include <basics/Canvas>
    #include <basics/Id>
    #include <basics/Scene>
    #include <basics/Texture_2D>
    #include <basics/Timer>

    #include "GameObject.hpp"

    namespace project_template
    {

        using basics::Id;
        using basics::Timer;
        using basics::Canvas;
        using basics::Texture_2D;

        class Game_Scene : public basics::Scene
        {

            // Estos typedefs pueden ayudar a hacer el código más compacto y claro:
            typedef std::shared_ptr < GameObject >         GameObject_Handle;
            typedef std::vector< GameObject_Handle >       GameObject_List;
            typedef std::shared_ptr< Texture_2D  >         Texture_Handle;
            typedef std::map< Id, Texture_Handle >         Texture_Map;
            typedef basics::Graphics_Context::Accessor     Context;
            typedef std::unique_ptr< basics::Raster_Font > Font_Handle;

            /**
             * Representa el estado de la escena en su conjunto.
             */
            enum State
            {
                LOADING,
                RUNNING,
                ERROR
            };

            /**
             * Representa el estado del juego cuando el estado de la escena es RUNNING.
             */
            enum Gameplay_State
            {
                UNINITIALIZED,
                WAITING_TO_START,
                PLAYING,
                ENDING,
            };

        private:

            //Maneja la fuente
            Font_Handle font;


            //Estructura que guarda el id de la textura y el path donde se debe buscar
            struct   Texture_Data { Id id; const char * path; };
            static Texture_Data textures_data[];

         // Numero de texturas de la escena
            static unsigned textures_count;

          //Numero de clicks objtenidos por el jugador
            static int clicks;

        private:

            //Cantidad de tiempo por partida
            static constexpr int max_time=10;

            int currentTime = 0;

            GameObject * menuBtn;
            GameObject * reiniciarBtn;

        private:

            State state; //Estado de la escena.
            Gameplay_State gameplay;//Estado del juego cuando la escena está RUNNING.
            bool suspended;// true cuando la escena está en segundo plano y viceversa.
            bool game_paused;

            //Informacion del canvas
            unsigned canvas_width;
            unsigned canvas_height;
            bool aspect_ratio_adjusted;
            float real_aspect_ratio;

            Texture_Map textures;
            GameObject_List gameobjects;

            Timer timer; // Cronómetro usado para medir intervalos de tiempo
            float timer_in_pause = 0;

            //Punteros a objetos
            GameObject * pause;
            GameObject * pause_text;
            GameObject * clicable_ptr;


        public:

            /**
             * Solo inicializa los atributos que deben estar inicializados la primera vez, cuando se
             * crea la escena desde cero.
             */
            Game_Scene();

            /**
             * Este método lo llama Director para conocer la resolución virtual con la que está
             * trabajando la escena.
             * @return Tamaño en coordenadas virtuales que está usando la escena.
             */
            basics::Size2u get_view_size () override
            {
                return { canvas_width, canvas_height };
            }

            /**
             * Aquí se inicializan los atributos que deben restablecerse cada vez que se inicia la escena.
             * @return
             */
            bool initialize () override;

            /**
             * Este método lo invoca Director automáticamente cuando el juego pasa a segundo plano.
             */
            void suspend () override;

            /**
             * Este método lo invoca Director automáticamente cuando el juego pasa a primer plano.
             */
            void resume () override;

            /**
             * Este método se invoca automáticamente una vez por fotograma cuando se acumulan
             * eventos dirigidos a la escena.
             */
            void handle (basics::Event & event) override;

            /**
             * Este método se invoca automáticamente una vez por fotograma para que la escena
             * actualize su estado.
             */
            void update (float time) override;

            /**
             * Este método se invoca automáticamente una vez por fotograma para que la escena
             * dibuje su contenido.
             */
            void render (Context & context) override;

        private:

            /**
             * En este método se cargan las texturas (una cada fotograma para facilitar que la
             * propia carga se pueda pausar cuando la aplicación pasa a segundo plano).
             */
            void load_textures ();

            /**
             * En este método se crean los gameobjects cuando termina la carga de texturas.
             */
            void create_gameobjects();

            /**
             * Se llama cada vez que se debe reiniciar el juego. En concreto la primera vez y cada
             * vez que un jugador pierde.
             */
            void restart_game ();

            /**
             * Cuando se ha reiniciado el juego
             */
            void start_playing ();

            /**
             * Actualiza el estado del juego cuando el estado de la escena es RUNNING.
             */
            void run_simulation (float time);


            /**
             * Dibuja la escena de juego cuando el estado de la escena es RUNNING.
             * @param canvas Referencia al Canvas con el que dibujar.
             */
            void render_playfield (Canvas & canvas);

            /**
             * Ajusta el aspect ratio
             */
            void adjust_aspect_ratio(Context & context);


            /**
             * Método que pausa el juego
             */
            void pause_the_game (bool paused);

            /**
             * Metodo para dibujar los textos
             * */
            void drawFont(Canvas & canvas);
        };

    }

#endif
