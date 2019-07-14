/*
 * INTRO SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#ifndef INTRO_SCENE_HEADER
#define INTRO_SCENE_HEADER

#include <memory>
#include <basics/Canvas>
#include <basics/Scene>
#include <basics/Texture_2D>
#include <basics/Timer>

namespace project_template {

    using basics::Timer;
    using basics::Canvas;
    using basics::Texture_2D;
    using basics::Graphics_Context;

    class Intro_Scene : public basics::Scene {

        typedef basics::Graphics_Context::Accessor Context;

        ///Determina el estado de la escena
        enum State {
            UNINITIALIZED,
            LOADING,
            FADING_IN,
            WAITING,
            FADING_OUT,
            FINISHED,
            ERROR
        };

    private:
        //Determina es estadp de ña escena
        State state;
        //True cuando la app esta en segundo plano
        bool suspended;
        bool aspect_ratio_adjusted;

        //Ancho y alto del canvas donde se ejecutara la escena
        unsigned canvas_width;
        unsigned canvas_height;

        //Cronometro para determinar el tiempo del fade out y fade in
        Timer timer;

        //Opacidad de la tecxtura elegida como logo
        float opacity;

        std::shared_ptr<Texture_2D> logo_texture;

    public:
        Intro_Scene()
        {
            state = UNINITIALIZED;
            suspended = true;
            canvas_width = 720;
            canvas_height = 1280;
            aspect_ratio_adjusted = false;
        }

        //Devuelve el tamaño del canvas donde se ejecuta el juego
        basics::Size2u get_view_size() override
        {
            return { canvas_width, canvas_height };
        }

//Se reinician los atributos
        bool initialize() override;

       //Se suspende ele juego
        void suspend() override
        {
            suspended = true;
        }

     //Se reinicial el juego
        void resume() override
        {
            suspended = false;
        }

       //Metodo update para crear el ciclo de la escena
        void update(float time) override;

        //Renderiza las texturas de la escena
        void render(Graphics_Context::Accessor& context) override;

    private:
        void update_loading();
        void update_fading_in();
        void update_waiting();
        void update_fading_out();
        void adjust_aspect_ratio(Context& context);
    };
}

#endif
