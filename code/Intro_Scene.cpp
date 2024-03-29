/*
 * INTRO SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#include "Intro_Scene.hpp"
#include "Menu_Scene.hpp"
#include "Game_Scene.hpp"
#include <basics/Canvas>
#include <basics/Director>

using namespace basics;
using namespace std;

namespace project_template
{

    // ---------------------------------------------------------------------------------------------

    bool Intro_Scene::initialize ()
    {
        if (state == UNINITIALIZED)
        {
            state = LOADING;
        }
        else
        {
            timer.reset ();

            opacity = 0.f;
            state   = FADING_IN;
        }
        return true;
    }

//ejecuta la funcion que segun el estado en el que se enceuntre
    void Intro_Scene::update (float time)
    {
        if (!suspended) switch (state)
        {
            case LOADING:    update_loading    (); break;
            case FADING_IN:  update_fading_in  (); break;
            case WAITING:    update_waiting    (); break;
            case FADING_OUT: update_fading_out (); break;
            default: break;
        }
    }


    void Intro_Scene::render (Graphics_Context::Accessor & context)
    {
        //Si nos encontramos dentro del juego
        if (!suspended)
        {
            //Se intenta conseguir el canvas
            Canvas * canvas = context->get_renderer< Canvas > (ID(canvas));

            //Se crea el canvas si no existe
            if (!canvas)
            {
                 canvas = Canvas::create (ID(canvas), context, {{ canvas_width, canvas_height }});
            }

           //Se dibuja dentro del canvas
            if (canvas)
            {
                //Limpiamos el canvas
                canvas->clear ();

                if (logo_texture)
                {
                    //Determinamos la opacidad en funcion del estado
                    canvas->set_opacity (opacity);

                    //Rellenamos un rectangulo con la textura
                    canvas->fill_rectangle
                    (
                        { canvas_width * 0.5f, canvas_height * 0.6f },
                        { logo_texture->get_width ()/1.5f, logo_texture->get_height () /2},
                          logo_texture. get ()
                    );
                }
            }
        }
    }

    void Intro_Scene::update_loading ()
    {
        Graphics_Context::Accessor context = director.lock_graphics_context ();

        if (context)
        {

            if(!aspect_ratio_adjusted)
            {
                adjust_aspect_ratio(context);
            }


            // Se carga la textura del logo:
            logo_texture = Texture_2D::create (0, context, "menu/logo.png");


            // Se comprueba si la textura se ha podido cargar correctamente:
            if (logo_texture)
            {
                context->add (logo_texture);

                timer.reset ();


                opacity = 0.f;
                state   = FADING_IN;
            }
            else
                state   = ERROR;
        }
    }

    void Intro_Scene::update_fading_in ()
    {
        float elapsed_seconds = timer.get_elapsed_seconds ();

        if (elapsed_seconds < 1.f)
        {
            // Se aumenta la opacidad del logo a medida que pasa el tiempo
            opacity = elapsed_seconds;
        }
        else
        {
            timer.reset ();

            opacity = 1.f;
            state   = WAITING;
        }
    }

    void Intro_Scene::update_waiting ()
    {

        // Se esperan un segundo sin hacer nada:
        if (timer.get_elapsed_seconds () > 1.f)
        {
            timer.reset ();

            state = FADING_OUT;
        }
    }

    void Intro_Scene::update_fading_out ()
    {
        float elapsed_seconds = timer.get_elapsed_seconds ();

        if (elapsed_seconds < .5f)
        {
            // Se reduce la opacidad de 1 a 0 en medio segundo
            opacity = 1.f - elapsed_seconds * 2.f;
        }
        else
        {

            // Cuando el faceout se ha completado, se lanza la siguiente escena:
            state = FINISHED;

            director.run_scene (shared_ptr< Scene >(new Menu_Scene));
        }
    }

    void Intro_Scene::adjust_aspect_ratio(Context & context)
{

    float real_aspect_ratio = float( context->get_surface_width () ) / context->get_surface_height ();

    // Si se desea mantener el ancho y ajustar el alto (disposición vertical)
    canvas_height = unsigned ( canvas_width * real_aspect_ratio);

    aspect_ratio_adjusted = true;
}

}
